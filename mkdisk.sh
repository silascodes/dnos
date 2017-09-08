#!/bin/sh

sudo losetup /dev/loop0 ./disk.img -o 1048576
sudo mount /dev/loop0 ./disk
sudo cp bin/kernel.bin disk/kernel.bin
sudo cp bin/servers/* disk/servers/. -R
sudo cp grub.cfg disk/boot/grub/grub.cfg
sudo sync
sudo umount ./disk
sudo losetup -d /dev/loop0

# Some useful grub lines...
# sudo grub-install --root-directory=/mnt --no-floppy --modules="normal part_msdos ext2 multiboot biosdev" /dev/loop0
# grub-mkconfig -o disk/boot/grub/grub.cfg