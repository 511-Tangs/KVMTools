#! /bin/bash

## Function Table
function prt_err() {
    echo -ne "\e[1;49;91m[ERROR] $@\e[m\n"
    exit 1 
}

function prt_info() {
    echo -ne "\e[1;49;92m[INFO] $@\e[m\n"

}
function prt_help() {
    echo -ne "
=======================================================
 This is used to resize qemu virtual machine
 Root partition is default as partition 1 
=======================================================
Using this command line to resize image file :\n
$0 \"\${Image_file}\" \"\${Add_Size(default using MB as unit.)}\"\n
ex :
$0 ../Image/test.img 2048 # enlarge 2048 MB for image file test.img
\n"
}
## Main script

# Value test
if [ "$#" -eq "2" ]; then
    prt_info "Start to check value and resize image file."
else
    prt_help
    prt_err "Value not correct"
fi

# Test Image value
/sbin/sfdisk -l $1 &>/dev/null
if [ "$?" -eq "0" ]; then
    prt_info "Get Image file :$1"
    prt_info "Testing $1 image file"
    PARTITION_NUMBER=$( /sbin/sfdisk -l $1 |grep $(basename $1 ) |sed 1d |wc -l)
    if [ "${PARTITION_NUMBER}" -eq  "2" ]; then
	prt_info "Image has one partition."
    else
	prt_help
	prt_err "Image file partition table not correct. "
    fi
else
    prt_help
    prt_err "Image file error, not a image file or permission denied"
fi

# Check e2fs version
if [ -f /sbin/e2fsck ]; then
    e2fs_ver=($(dpkg -l |grep e2fsprogs|awk -F ' ' '{print $3}'|grep -o -E '[0-9]+'))
    E2FSVERSION="1.43.3"
else
    prt_err "e2fsck not found, please install e2fsprogs and e2fslib packages"
fi
# Test Jessie version
cat /etc/apt/sources.list |sed 's/\#*//g' |grep jessie &>/dev/null
if [ "$?" -eq "0" ]; then
    prt_info "APT UPDATE VERSION : JESSIE"
    for num in $(seq 1 3); do
	TESTVALUE=$(echo ${E2FSVERSION}|grep -o -E '[0-9]+' | head -n ${num} |tail -n 1)
	GETVALUE=$(echo ${e2fs_ver[@]}|awk -F ' ' "{print \$${num}}")
	if [ "${GETVALUE}" -lt "${TESTVALUE}" ]; then
	    prt_err "e2fs version too old : $(dpkg -l |grep e2fslib|awk -F ' ' '{print $3}'), need greater than 1.43.3"
	fi
    done
    prt_info "e2fs verseion: $(dpkg -l |grep e2fslib|awk -F ' ' '{print $3}')"
fi
# Test resize 
#RESIZE_UNIT=$(echo $2 |grep -o -E '[a-zA-Z]+' )
RESIZE_SIZE=$(echo $2 |grep -o -E '[0-9.0-9]+' )
# Test loop 
which qemu-nbd &>/dev/null
if [ "$?" -eq "0" ]; then
    prt_info "Start mount and resize image file"
else
    prt_err "qemu-nbd not found."
fi

# Add size into image
prt_info "Add size into image file"
DATE=$(date +%s)
mv $1 $1.bak
dd if=/dev/zero of=add${DATE}.raw bs=1024K count=${RESIZE_SIZE}
cat $1.bak add${DATE}.raw >> $1

#${QEMUIMG} resize $1 +${RESIZE_SIZE}M
echo ",+" |/sbin/sfdisk -N 2 $1
rm -rf add${DATE}.raw $1.bak

# Get Root permission
prt_info "Super user permission test"
echo "Super User passwd, please:"
if [ $EUID -ne 0 ]
   then sudo echo -ne ""
        if [ $? -ne 0 ]
	    then  prt_err "Sorry, need su privilege!"
        else
            prt_info "Super user permission  test: succeed."
	    SUDO='/usr/bin/sudo'
        fi
else
    SUDO=''
fi

# Mount and fix image file
if [ -b /dev/nbd0 ]; then
    ${SUDO} rmmod nbd
    if [ "$?" -eq "0" ]; then
	prt_info "Clean nbd modules, using /dev/nbd0 to connect."
    else
	prt_err "Cannot reset nbd modules."
    fi
else
    prt_info "Using /dev/nbd0 to connect."
fi
${SUDO} modprobe nbd max_part=8
${SUDO} qemu-nbd --connect=/dev/nbd0 $1

ls -l /dev/nbd0p* &>/dev/null
if [ "$?" -ne "0" ]; then
    ${SUDO} partx -av /dev/nbd0
    if [ "$?" -ne "0"  ]; then
	${SUDO} rmmod nbd
	prt_err "Cannot read partition from $1."
    fi
fi
prt_info "Mount image file succeed"

# Resize and fsck image file

prt_info "Fix image file"
${SUDO} /sbin/e2fsck -y -f -v /dev/nbd0p2
prt_info "Resize2fs image file"
${SUDO} /sbin/resize2fs /dev/nbd0p2
prt_info "Resize image file clear, start to clean and restore environment."
${SUDO} partx -dv /dev/nbd0 
${SUDO} qemu-nbd --disconnect /dev/nbd0
${SUDO} rmmod nbd
prt_info "Output file :"
ls -l $1
