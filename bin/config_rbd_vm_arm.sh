#!/bin/bash
help(){
    echo "usage: $0 -x host_name -B ethernet -k kernel_path -I initrd_path -r root_path -m disk_mode(RBD|QCOW2|RAW) -s storage_path"
    echo "storage path with RBD: mon_ip:pool:image"
    echo "storage path with QCOW2/RAW: image_path"
    exit 0
}

while [ "$#" -gt "0" ] ;do
    case $1 in
        -x)
            hostname=$2
            shift 2
            ;;
        -B)
            net=$2
            shift 2
            ;;
        -k)
            kernel_path=$2
            shift 2
            ;;
        -I)
            initrd_path=$2
            shift 2
            ;;
        -r)
            root_path=$2
            shift 2
            ;;
        -m)
            case $2 in
                rbd|RBD)
                    disk_mode=rbd
                    ;;
                QCOW2|qcow2)
                    disk_mode=qcow2
                    ;;
                RAW|raw)
                    disk_mode=raw
                    ;;
            esac
            shift 2
            ;;
        -s)
            image_path=$2
            shift 2
            ;;
        -h)
            help
            ;;
        *)
            shift 1
    esac
done
if [ "a$hostname" == "a" ];then
    help
elif [ "a$net" == "a" ];then
    help
elif [ "a$kernel_path" == "a" ];then
    help
elif [ "a$initrd_path" == "a" ];then
    help
elif [ "a$root_path" == "a" ];then
    help
elif [ "a$disk_mode" == "a" ];then
    help
elif [ "a$image_path" == "a" ];then
    help
fi

if [ "$disk_mode" == "rbd" ];then
    mon_ip=$(echo $image_path | awk -F ':' '{print $1}')
    storage_path=$(echo $image_path | awk -F ':' '{printf("%s/%s\n",$2,$3)}' )
    ping -c1 -w1 $mon_ip &>/dev/null
    if [ "$?" -ne "0" ];then
        echo "cannot access to CEPH mon"
        exit -1
    fi
    key=$(ssh root@${mon_ip}  'ceph auth get-or-create-key client.admin ' 2>/dev/null)
    name="$hostname.ceph.client.secret"
    num=$(virsh secret-list |grep ${name} | wc -l  | awk '{a = $1 + 1 ;print a}')
    if [ "$num" -gt "1" ];then
        echo "name already in used change to: $name"
        exit -1
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
else    
    storage_path=$(echo $image_path )
fi
kvm_path=$(which kvm 2>/dev/null )
if [ "a$kvm_path" == "a" ];then
    echo "Cannot found KVM path, please enter 'kvm' binary file's path:"
    read kvm_path
fi

cat <<EOF > ${hostname}.xml
<domain type='qemu'>
  <name>${hostname}</name>
  <memory unit='MiB'>2048</memory>
  <currentMemory unit='MiB'>2048</currentMemory>
  <vcpu placement='static'>2</vcpu>
  <resource>
    <partition>/machine</partition>
  </resource>
  <os>
    <type arch='armv7l' machine='virt-3.1'>hvm</type>
    <kernel>${kernel_path}</kernel>
    <initrd>${initrd_path}</initrd>
    <cmdline>earlyprintk=ttyAMA0 console=ttyAMA0 rw root=${root_path}</cmdline>
    <boot dev='hd'/>
  </os>
  <features>
    <gic version='2'/>
  </features>
  <clock offset='utc'/>
  <on_poweroff>destroy</on_poweroff>
  <on_reboot>restart</on_reboot>
  <on_crash>destroy</on_crash>
  <devices>
    <emulator>/usr/bin/qemu-system-arm</emulator>
EOF
case ${disk_mode} in
    raw|qcow2)
        cat <<EOF >> ${hostname}.xml

    <disk type='file' device='disk'>
      <driver name='qemu' type='${disk_mode}' cache='none'/>
      <source file='${storage_path}'/>
      <backingStore/>
      <target dev='sda' bus='scsi'/>
    </disk>
EOF
        ;;
    rbd)
        cat <<EOF >> ${hostname}.xml
    <disk type='network' device='disk'>
      <driver name='qemu' type='raw'/>
      <auth username='admin'>
        <secret type='ceph' uuid='${ceph_secret_uuid}'/>
      </auth>
      <source protocol='rbd' name='${storage_path}'>
        <host name='${mon_ip}' port='6789'/>
      </source>
      <target dev='sda' bus='scsi'/>
    </disk>
EOF
        ;;
esac
        cat <<EOF >> ${hostname}.xml
    <controller type='scsi' index='0' model='virtio-scsi'>
      <alias name='scsi0'/>
      <address type='virtio-mmio'/>
    </controller>
    <interface type='direct'>
      <source dev='${net}' mode='bridge'/>
      <model type='virtio'/>
      <address type='virtio-mmio'/>
    </interface>
    <serial type='pty'>
      <source path='/dev/pts/4'/>
      <target type='system-serial' port='0'>
        <model name='pl011'/>
      </target>
      <alias name='serial0'/>
    </serial>
    <console type='pty' tty='/dev/pts/4'>
      <source path='/dev/pts/4'/>
      <target type='serial' port='0'/>
      <alias name='serial0'/>
    </console>
  </devices>
</domain>
EOF
