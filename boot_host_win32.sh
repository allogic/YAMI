#!/bin/bash

qemu_img="/mnt/c/Program Files/qemu/qemu-img.exe"
qemu_sys="/mnt/c/Program Files/qemu/qemu-system-x86_64.exe"

drive_name="drive.img"
mount_point="drive_mount"
bios_name="ovmf.fd"

"$qemu_img" create -f raw $drive_name 1G

mkfs.fat -F32 $drive_name
mkdir $mount_point
mount $drive_name $mount_point
mkdir -p $mount_point/efi/boot
cp loader.efi $mount_point/efi/boot
cp kernel.bin $mount_point
umount $mount_point
rm -rf $mount_point

"$qemu_sys" \
    -bios "$bios_name" \
    -drive file=$drive_name,format=raw,if=virtio \
    -m 1G \
    -netdev user,id=net0 \
    -device virtio-net-pci,netdev=net0