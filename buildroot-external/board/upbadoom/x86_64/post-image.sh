#!/bin/sh

# Also used in build.sh, so be careful when changing
SYSLINUX_DIR=${BR2_EXTERNAL_UPBADOOM_PATH}/syslinux

OUT_DIR=${BUILD_DIR}/../images
IMAGE_NAME=UPBADOOM-$(git rev-parse HEAD | cut -c 1-8).img

## Move previous image (whin debugging)
# sudo mv "$OUT_DIR"/UPBADOOM*.img "$OUT_DIR"/UPBADOOM-previous.img

## Make the image
cd "$OUT_DIR" || return
dd if=/dev/zero of="${IMAGE_NAME}" bs=1M count=32
(echo "n"; echo ""; echo ""; echo ""; echo "ef00"; echo w; echo Y) | gdisk "$IMAGE_NAME"
LOOP=$(sudo losetup --show -f -o 1048576 "${IMAGE_NAME}")
sudo mkfs.vfat "$LOOP"
ls
sudo mkdir image-mnt
sudo mount "$LOOP" image-mnt
ls
sudo chmod 777 image-mnt
sudo mkdir -p image-mnt/EFI/Boot/
cd image-mnt/EFI/Boot/ || return
sudo cp "$SYSLINUX_DIR"/syslinux.efi ./bootx64.efi # SYSLIUX_DIR defined in build.sh (root of repo)
sudo cp "$SYSLINUX_DIR"/ldlinux.e64 ./
sudo cp $BR2_EXTERNAL_UPBADOOM_PATH/board/upbadoom/x86_64/syslinux.cfg ./
sudo cp "$OUT_DIR"/rootfs.cpio.gz ./
sudo cp "$OUT_DIR"/bzImage ./
cd "$OUT_DIR"
sudo sync
sudo umount image-mnt
sudo losetup -d "$LOOP"
sudo rmdir image-mnt

# If everything went well, delete the previous image (when debugging)
# sudo rm "$OUT_DIR"/UPBADOOM-previous.img
