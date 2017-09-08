
#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include <stdint.h>

struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint8_t boot_device[4];
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
};

struct mmap_entry {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
};

struct mods_entry {
    uint32_t start;
    uint32_t end;
    char *cmdline;
    uint32_t reserved;
};

#endif
