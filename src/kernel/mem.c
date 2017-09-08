
#include "kernel/arch.h"
#include "kernel/mem.h"

void *mem_physalloc() {
    return arch_mem_physAlloc();
}

void mem_physfree(void *pBase) {
    arch_mem_physFree(pBase);
}
