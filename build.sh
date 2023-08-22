#!/bin/bash

## Get the latest stable of buildroot

# git ls-remote is kinda slow :(
BUILDROOT_DIR=buildroot-$(git ls-remote --tags https://git.buildroot.net/buildroot.git | awk '!/{}/ {print $2}' | awk -F/ '!/rc/ {print $NF}' | grep -E '^[0-9]{4}' | sort -V | tail -n 1)

curl -L https://buildroot.net/downloads/${BUILDROOT_DIR}.tar.gz | tar xvzf -

# Pretty slow download as well (sometimes) :(
SYSLINUX_VER="syslinux-6.03"
curl -L https://mirrors.edge.kernel.org/pub/linux/utils/boot/syslinux/$SYSLINUX_VER.tar.gz | tar xvzf - $SYSLINUX_VER/efi64/efi/syslinux.efi $SYSLINUX_VER/efi64/com32/elflink/ldlinux/ldlinux.e64 --transform='s,.*/,,'

SYSLINUX_DIR=buildroot-external/syslinux

mkdir "$SYSLINUX_DIR"
mv -f syslinux.efi ldlinux.e64 "$SYSLINUX_DIR"

cd $BUILDROOT_DIR

make BR2_EXTERNAL=../buildroot-external/ x86_64_defconfig

make
