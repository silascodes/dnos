
#ifndef _KERNEL_H
#define _KERNEL_H

#include <stdint.h>
#include "kernel/multiboot.h"

struct startup_info {
    uint32_t placeholder;
};

void kernel_main(struct multiboot_info *pStartupInfo);

#endif
