
#ifndef _TASKMGR_H
#define _TASKMGR_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    PROCESS_TYPE_NULL,
    PROCESS_TYPE_KERNEL,
    PROCESS_TYPE_SERVER,
    PROCESS_TYPE_USER
} process_type;

typedef struct {
    process_type type;
    uint32_t pid;
} process;

void taskmgr_init();
uint32_t taskmgr_start(void *pLocation, size_t pLength, process_type pType);
void taskmgr_switch(uint32_t pCurrent, uint32_t pNew);

#endif
