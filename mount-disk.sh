#!/bin/sh

sudo losetup /dev/loop0 ./disk.img -o 1048576
sudo mount /dev/loop0 ./disk
