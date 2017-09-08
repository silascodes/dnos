
#ifndef _ARCH_H
#define _ARCH_H

#include <stddef.h>
#include "kernel/taskmgr.h"

void arch_dbg_setCode(uint8_t pCode);

size_t arch_mem_getPageSize();
void *arch_mem_physAlloc();
void arch_mem_physFree(void *pBase);

void arch_process_setInitialState(process *pProcess);
void arch_process_mapPage(process *pProcess, void *pPhysBase, void *pVirtBase);
void arch_process_setEntryPoint(process *pProcess, uint32_t pEntryPoint);
void arch_process_switch(process *pCurrent, process *pNew);

#endif
