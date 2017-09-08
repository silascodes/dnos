
#ifndef _KDBG_H
#define _KDBG_H

#include <stddef.h>
#include <stdint.h>

void kdbg_dumpmem(uint32_t pBase, size_t pBytes);
void kdbg_dumpgdt();

#endif
