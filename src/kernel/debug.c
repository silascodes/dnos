
#include "kdbg.h"
#include "kstdio.h"

extern uint32_t _storeGDTAddress();
extern size_t _storeGDTSize();

void kdbg_dumpmem(uint32_t pBase, size_t pBytes) {
    kprintf("DBG: dumping %u bytes at address %X\n", pBytes, pBase);

    uint8_t *ptr = (uint8_t*)pBase;
    for(size_t i = 0, j = 0; i < pBytes; i++, j++, ptr++) {
        if(j >= 16) {
            kprintf("\n");
            j = 0;
        }

        uint32_t theByte = 0;
        theByte = *ptr;

        kprintf("%X ", theByte);
    }

    kprintf("\n");
}

void kdbg_dumpgdt() {
    kdbg_dumpmem(_storeGDTAddress(), _storeGDTSize());
}
