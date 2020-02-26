#!/bin/bash
while [ "$#" -gt "0" ] ;do
    case $1 in
        -m)
            mon_ip=$2
            shift 2
            ;;
        -n)
            name="$2.ceph.client.secret"
            shift 2
            ;;
        -x)
            hostname=$2
            shift 2
            ;;
        -I)
            image_name=$2
            shift 2
            ;;
        -B)
            net=$2
            shift 2
            ;;
                -h)
            echo "usage: $0 -m ceph_mon_ip -x host_name -I Image_path(rbd/image) -B ethernet [-n cluster_define_name]"
            exit 0
            ;;
        *)
            shift 1
    esac
done
if [ "a$mon_ip" == "a" ] || [ "a$hostname" == "a" ] || [ "a$image_name" == "a" ] || [ "a$net" == "a" ];then
    echo "usage: $0 -m ceph_mon_ip -x host_name -I Image_path(rbd/image) -B ethernet [-n cluster_define_name]"
    exit 0
fi             
if [ "a$name" == "a" ];then
    name="ceph.client.secret"
fi
ping -c1 -w1 $mon_ip &>/dev/null
if [ "$?" -ne "0" ];then
    echo "failed to send ICMP to ceph monitor $hostip"
    exit -1
fi
if [ "a$key" == "a" ];then
    key=$(ssh $mon_ip 'ceph auth get-or-create-key client.admin ' 2>/dev/null)
fi
num=$(virsh secret-list |grep ${name} | wc -l  | awk '{a = $1 + 1 ;print a}')
if [ "$num" -gt "0" ];then
    name="${name}_${num}"
    echo "name already in used change to: $name"
fi
cat <<EOF > secret.xml
<secret ephemeral='no' private='no'>
        <usage type='ceph'>
                <name>${name}</name>
        </usage>
</secret>
EOF
virsh secret-define --file secret.xml 
ceph_secret_uuid=$(virsh secret-list |egrep ${name} | awk '{ print $1 }')
virsh secret-set-value ${ceph_secret_uuid} --base64 $key
kvm_path=$(which kvm 2>/dev/null )
if [ "a$kvm_path" == "a" ];then
    echo "Cannot found KVM path, please enter 'kvm' binary file's path:"
    read kvm_path
fi
cat <<EOF > ${hostname}.xml
<domain type='kvm'>
  <name>${hostname}</name>
  <memory unit='MiB'>2048</memory>
  <currentMemory unit='MiB'>2048</currentMemory>
  <vcpu placement='static'>2</vcpu>
  <resource>
    <partition>/machine</partition>
  </resource>
  <os>
    <type arch='x86_64'>hvm</type>
    <boot dev='hd'/>
  </os>
  <features>
    <acpi/>
    <apic/>
    <pae/>
  </features>
  <clock offset='utc'>
    <timer name='rtc' tickpolicy='catchup'/>
    <timer name='pit' tickpolicy='delay'/>
    <timer name='hpet' present='no'/>
  </clock>
  <on_poweroff>destroy</on_poweroff>
  <on_reboot>restart</on_reboot>
  <on_crash>restart</on_crash>
  <devices>
    <emulator>${kvm_path}</emulator>
    <disk type='network' device='disk'>
      <driver name='qemu' type='raw'/>
      <auth username='admin'>
        <secret type='ceph' uuid='${ceph_secret_uuid}'/>
      </auth>
      <source protocol='rbd' name='${image_name}'>
        <host name='${mon_ip}' port='6789'/>
      </source>
      <target dev='sda' bus='scsi'/>
      <address type='drive' controller='0' bus='0' target='0' unit='0'/>
    </disk>
    <controller type='scsi' index='0' model='virtio-scsi'/>
    <interface type='direct'>
      <source dev='${net}' mode='bridge'/>
      <model type='virtio'/>
    </interface>
    <serial type='pty'>
      <target type='isa-serial' port='0'>
        <model name='isa-serial'/>
      </target>
    </serial>
    <console type='pty'>
      <target type='serial' port='0'/>
    </console>
    <input type='tablet' bus='usb'/>
    <input type='mouse' bus='ps2'/>
    <input type='keyboard' bus='ps2'/>
    <graphics type='spice'>
      <listen type='none'/>
    </graphics>
    <video>
      <model type='virtio' heads='1' primary='yes'/>
    </video>
    <memballoon model='virtio'/>
  </devices>
</domain>
EOF
