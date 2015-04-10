#!/bin/bash
#this script builds the builroot based image
BR_FOLDER=./buildroot
IMAGE_VERSION=XX.YY

BR_BOARD_SYSTEM_CONFIG=baytrail
BR_BOARD_CONFIG=qemu_x86_64_defconfig
BR_BOARD_LINUX_CONFIG=linux.config 

#RAW_DISK_IMAGE=./raw-images/disk.grub1.img.xz
GRUB1_PREPARE_SCRIPT=grub1-prepare.sh
GRUB1_COPY_SCRIPT=grub1-copy.sh
GRUB1_SUDO_EXE_SCRIPT=sudo-executor-grub1-prepare.sh
GRUB2_DISK_CREATOR=grub2-bootdisk.sh

#output images
BOOTABLE_USB_IMG=bootable-usb-disk.img
BRBOX_ROOT_IMG=uBrBoxRoot.uimg

IMAGE_ONLY=0
PREPARE_ONLY=0
###############################################################################
while getopts b:o:v:c:s:ip f
do
    case $f in
	b) BR_BOARD_SYSTEM_CONFIG=$OPTARG;;  #board/system type for different application
	o) BR_OUTPUT_FOLDER=$OPTARG;;        #buildroot output folder
	v) IMAGE_VERSION=$OPTARG ;;          #image version
	s) SUDOPW=$OPTARG ;;                 #sud password
	i) IMAGE_ONLY=1 ;;                   #-i can be used only for binary image creation(build will be skipped)
        p) PREPARE_ONLY=1 ;;                 #just prepare buildroot, dont build
    esac
done

#TODO: switch case for br-board-system-config

svn up > /dev/null
BUILDNUMBER=$(LANG=en_US; svn info 2>&1 | grep "Last Changed Rev:" | sed -e 's/.* //')
TMP_BUILDNUM=$(printf "%05d" $BUILDNUMBER)
BUILDNUMBER=$(printf "$IMAGE_VERSION.%05d" $BUILDNUMBER)
[ ! -d  "$BR_OUTPUT_FOLDER" ] && { echo "Error: output folder $BR_OUTPUT_FOLDER not found!"   ; exit -1; }

BR_OUTPUT_FOLDER=$BR_OUTPUT_FOLDER/$IMAGE_VERSION
if [ $IMAGE_ONLY = 0 ]; then
	mkdir -p $BR_OUTPUT_FOLDER
	cp configs/$BR_BOARD_SYSTEM_CONFIG/$BR_BOARD_CONFIG         $BR_FOLDER/configs/
	cp configs/$BR_BOARD_SYSTEM_CONFIG/$BR_BOARD_LINUX_CONFIG   $BR_FOLDER/board/qemu/x86_64/
	cp configs/$BR_BOARD_SYSTEM_CONFIG/busybox.config           $BR_FOLDER/package/busybox/
	#TODO: configs/$BR_BOARD_SYSTEM_CONFIG/uclibc.config        $BR_FOLDER/package/uclibc/
	pushd .
	cd $BR_FOLDER
	make O=$BR_OUTPUT_FOLDER $BR_BOARD_CONFIG BRBOX_RELVERSION=$IMAGE_VERSION BRBOX_BUILDNUM=$TMP_BUILDNUM
	if [ $PREPARE_ONLY = 1 ]; then
		popd
		exit 0
	fi	
	make O=$BR_OUTPUT_FOLDER BRBOX_RELVERSION=$IMAGE_VERSION BRBOX_BUILDNUM=$TMP_BUILDNUM
	BUILD_RESULT=$?	
	popd
fi
[ "$BUILD_RESULT" != "0" ] && echo "Error!!! build failed!!!!" && exit 1 
#create bootable-usb-disk.img
GRUB2_SCRIPT=$(pwd)/scripts/$GRUB2_DISK_CREATOR
./scripts/sudo-grub2-bootdisk.sh $GRUB2_SCRIPT $BR_OUTPUT_FOLDER $BUILDNUMBER $BR_OUTPUT_FOLDER/images/$BOOTABLE_USB_IMG $SUDOPW

#create Rootfs upgrade package
./scripts/brbox-mkuimg.sh -r $BR_OUTPUT_FOLDER/images/rootfs.tar.xz -v $BUILDNUMBER -o $BR_OUTPUT_FOLDER/images/$BRBOX_ROOT_IMG -m $BR_OUTPUT_FOLDER/host/usr/bin/brbox-mkimage


