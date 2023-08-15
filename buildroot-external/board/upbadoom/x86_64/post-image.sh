#!/bin/sh

OUT_DIR=${BUILD_DIR}/../images
IMAGE_NAME=UPBADOOM.img
IMAGE_DIR=${OUT_DIR}/${IMAGE_NAME}-DIR

mkdir -p ${IMAGE_DIR}

## Make the image

cd ${OUT_DIR}
dd if=/dev/zero of=${IMAGE_NAME} bs=1M count=32
(echo "n";echo "";echo "";echo "";echo "ef00";echo w;echo Y) | gdisk $IMAGE_NAME
sudo mkfs.vfat $IMAGE_NAME
sudo mkdir $IMAGE_DIR
sudo mount -t vfat -o loop,rw,uid=$(id -u),gid=$(id -g) ${IMAGE_NAME} ${IMAGE_DIR}
sudo mkdir -p $IMAGE_DIR/EFI/Boot/
sudo cp ${OUT_DIR}/syslinux/syslinux.efi ${IMAGE_DIR}/EFI/Boot/bootx64.efi
sudo cp ${BUILD_DIR}/syslinux-*/efi64/com32/elflink/ldlinux/ldlinux.e64 ${IMAGE_DIR}/EFI/Boot/
sudo cp ${OUT_DIR}/bzImage ${IMAGE_DIR}/EFI/Boot/
sudo cp ${OUT_DIR}/rootfs.cpio.gz ${IMAGE_DIR}/EFI/Boot/
sudo cp ${BR2_EXTERNAL_UPBADOOM_PATH}/board/upbadoom/x86_64/syslinux.cfg ${IMAGE_DIR}/EFI/Boot/
sudo umount $IMAGE_NAME
rmdir $IMAGE_DIR
