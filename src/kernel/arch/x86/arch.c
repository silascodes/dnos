
#include <stdint.h>
#include <stddef.h>
#include "kernel/arch.h"

extern void _x86_dbg_setCode(uint8_t pCode);

void arch_dbg_setCode(uint8_t pCode) {
    _x86_dbg_setCode(pCode);
}

static uint32_t kmem_physBase;
static uint32_t kmem_physUsableBase;
static size_t kmem_physSize;
static uint32_t *kmem_physStack;
static size_t kmem_pageSize;

void arch_mem_physInit(uint32_t pBase, size_t pSize, size_t pPageSize) {
    kmem_physBase = pBase;
    kmem_physSize = pSize;
    kmem_pageSize = pPageSize;

    // TODO: fix base if not aligned to page size

    // Reserve space for free page stack
    size_t totalPages = kmem_physSize / kmem_pageSize;
    size_t stackMaxSize = 4 * totalPages;   // TODO: there is a hardcoded 4 here
    // TODO: probably should check we have enough space for this and some space left to allocate...

    // Set usable base to next page size aligned address after the stack
    size_t stackPages = (stackMaxSize / kmem_pageSize);
    if(stackMaxSize % kmem_pageSize > 0) {
        stackPages++;
    }
    kmem_physUsableBase = kmem_physBase + (stackPages * kmem_pageSize);

    // Initialise the stack
    kmem_physStack = (uint32_t*)kmem_physBase;
    for(uint32_t i = 0; i < (totalPages - stackPages); i++, kmem_physStack++) {
        *kmem_physStack = i;
    }

    //kprintf("Physical Memory Manager: %x %x %u %x\n", kmem_physBase, kmem_physUsableBase, kmem_physSize, kmem_physStack);
}

size_t arch_mem_getPageSize() {
    return kmem_pageSize;
}

void *arch_mem_physAlloc() {
    // If stack is at base, we're out of physical memory
    if(kmem_physStack == (uint32_t*)kmem_physBase) {
        return NULL;
    }

    // Grab index of next available frame
    kmem_physStack--;
    uint32_t frameIdx = *kmem_physStack;

    // Calculate frame address
    uint8_t *ptr = (uint8_t*)kmem_physUsableBase;
    ptr += (frameIdx * kmem_pageSize);

    return (void*)ptr;
}

void arch_mem_physFree(void *pBase) {
    // TODO: sanity check pBase

    // Figure out which frame pBase references and put it back on the stack
    uint32_t frameIdx = (((uint32_t)pBase - kmem_physUsableBase) / kmem_pageSize);
    *kmem_physStack = frameIdx;
    kmem_physStack++;
}

void arch_process_setInitialState(process *pProcess) {
    // allocate/init arch data area
    // set up paging tables, other descriptor tables whatever
    // set initial values for all registers
}

void arch_process_mapPage(process *pProcess, void *pPhysBase, void *pVirtBase) {
    // if the addresses aren't aligned, we're out
    // if page already mapped free it
    // set new page
    // if process is active flush cache somehow? will this even happen? not atm anyway...
}

void arch_process_setEntryPoint(process *pProcess, uint32_t pEntryPoint) {
    // Set EIP
    //process->archData->eip = pEntryPoint;
}

void arch_process_switch(process *pCurrent, process *pNew) {

}
