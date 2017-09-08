#!/bin/sh

CC=./compilers/i686-elf-4.9.1-Linux-x86_64/bin/i686-elf-gcc
CC_FLAGS='-std=c99 -ffreestanding -g -Wall -Wextra -Isrc'
LD_FLAGS='-ffreestanding -g -nostdlib'

# Build the x86 arch components
echo "Building arch-x86..."
nasm -felf32 src/kernel/arch/x86/boot.asm -o obj/kernel/arch/x86/boot.o
$CC -c src/kernel/arch/x86/x86.c -o obj/kernel/arch/x86/x86.o $CC_FLAGS
$CC -c src/kernel/arch/x86/arch.c -o obj/kernel/arch/x86/arch.o $CC_FLAGS

# Build the kernel
echo "Building kernel..."
$CC -c src/kernel/kernel.c -o obj/kernel/kernel.o $CC_FLAGS
$CC -c src/kernel/kstdlib.c -o obj/kernel/kstdlib.o $CC_FLAGS
$CC -c src/kernel/kstring.c -o obj/kernel/kstring.o $CC_FLAGS
$CC -c src/kernel/kstdio.c -o obj/kernel/kstdio.o $CC_FLAGS
$CC -c src/kernel/term.c -o obj/kernel/term.o $CC_FLAGS
#$CC -c src/kernel/mem.c -o obj/kernel/mem.o $CC_FLAGS
#$CC -c src/kernel/paging.c -o obj/kernel/paging.o $CC_FLAGS
#$CC -c src/kernel/debug.c -o obj/kernel/debug.o $CC_FLAGS
$CC -c src/kernel/taskmgr.c -o obj/kernel/taskmgr.o $CC_FLAGS
$CC -c src/kernel/elf32.c -o obj/kernel/elf32.o $CC_FLAGS
#$CC -T src/kernel/linker.ld -o bin/kernel.bin $LD_FLAGS obj/kernel/boot.o obj/kernel/kernel.o obj/kernel/kstdlib.o obj/kernel/kstring.o obj/kernel/kstdio.o obj/kernel/term.o obj/kernel/mem.o obj/kernel/gdt.o obj/kernel/idt.o obj/kernel/paging.o obj/kernel/debug.o -lgcc

# Link the kernel
echo "Linking kernel..."
$CC -T src/kernel/linker.ld -o bin/kernel.bin $LD_FLAGS obj/kernel/arch/x86/boot.o obj/kernel/arch/x86/x86.o obj/kernel/arch/x86/arch.o obj/kernel/kernel.o obj/kernel/taskmgr.o obj/kernel/elf32.o obj/kernel/kstdlib.o obj/kernel/kstring.o obj/kernel/kstdio.o obj/kernel/term.o -lgcc

# Build test server
echo "Building server test..."
$CC -c src/servers/test/test.c -o obj/servers/test/test.o $CC_FLAGS
$CC -o bin/servers/test.bin $LD_FLAGS obj/servers/test/test.o -lgcc
