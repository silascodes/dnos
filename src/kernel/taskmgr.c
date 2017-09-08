
#include "kernel/taskmgr.h"
#include "kernel/elf32.h"
#include "kernel/kstring.h"
#include "kernel/arch.h"

static process processes[16];
static uint32_t nextFree = 0;

/**
 * Initialise task manager and setup process 0 as the kernel
 */
void taskmgr_init() {
    kmemset(processes, 0, sizeof(process) * 16);

    // Setup kernel as pid 0
    processes[0].type = PROCESS_TYPE_KERNEL;
    nextFree++;
}

/**
 * Start a new process from a image in memory
 */
uint32_t taskmgr_start(void *pLocation, size_t pLength, process_type pType) {
    // Create new process
    uint32_t pid = nextFree;
    nextFree++;

    //  Initialise new process struct
    processes[pid].type = pType;
    processes[pid].pid = pid;
    arch_process_setInitialState(&processes[pid]);

    //      Set up virtual memory/paging
    //      Load/relocate/expand ELF into memory, mapping into virtual address space as specified in headers
    //      Fuck I hope all the registers have been initialised correctly! EIP?

    // Check the elf32 image is good to go, and all the stuff we need is there, load it!
    elf32_handle handle;
    if(!elf32_loadMem(&handle, pLocation, pLength)) {
        //arch_dbg_setCode(0xD0);
        return 0;
    }

    // How many physical pages do we need to load this image?
    size_t pageSize = arch_mem_getPageSize();
    size_t pagesReq = handle.totalBytes / pageSize;
    if(handle.totalBytes % pageSize > 0) {
        pagesReq++;
    }

    // Allocate, map and fill each page
    size_t prog = 0;
    uint32_t vaddr = handle.virtualBase;
    size_t progDone = 0;
    for(size_t i = 0; i < pagesReq && prog < handle.numProgramHeaders; vaddr += pageSize) {
        // Check prog type is LOAD, if not skip
        if(handle.programHeader[prog].p_type != 1) {
            prog++;
            continue;
        }

        // Allocate and map new page
        uint8_t *page = (uint8_t*)arch_mem_physAlloc();
        kmemset(page, 0, pageSize);
        size_t pageUsed = 0;
        arch_process_mapPage(&processes[pid], page, (void*)vaddr);  

        // While page not full and we still have progs, fill page
        while(pageUsed < pageSize && prog < handle.numProgramHeaders) {
            size_t pageLeft = pageSize - pageUsed;

            // If still real bytes to copy, copy them
            if(handle.programHeader[prog].p_filesz < progDone) {
                size_t bytesToCopy = handle.programHeader[prog].p_filesz - progDone;
                if(bytesToCopy > pageLeft) {
                    bytesToCopy = pageLeft;
                }
                kmemcpy(page + pageUsed, (void*)handle.programHeader[prog].p_offset + progDone, bytesToCopy);
                pageUsed += bytesToCopy;
                progDone += bytesToCopy;
            }
            // Else, if there are bytes to zero
            else if(handle.programHeader[prog].p_memsz > handle.programHeader[prog].p_filesz) {
                // We don't actually zero bytes, just increment the counters because we cleared all pages earlier
                size_t bytesToZero = handle.programHeader[prog].p_memsz - handle.programHeader[prog].p_filesz;
                size_t bytesLeft = handle.programHeader[prog].p_memsz - progDone;
                if(bytesLeft < bytesToZero) {
                    bytesToZero = bytesLeft;
                }
                if(bytesToZero > pageLeft) {
                    bytesToZero = pageLeft;
                }
                pageUsed += bytesToZero;
                progDone += bytesToZero;
            }
            // Else, check the next prog
            else {
                prog++;
                progDone = 0;
            }
        }

        i++;
    }

    // Set entry point
    arch_process_setEntryPoint(&processes[pid], handle.header->e_entry);    

    return pid;
}

void taskmgr_switch(uint32_t pCurrent, uint32_t pNew) {
    arch_process_switch(&processes[pCurrent], &processes[pNew]);
}
