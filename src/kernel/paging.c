
#include "kmem.h"
#include "kstdio.h"
#include "kpaging.h"

static uint32_t *kpaging_pdt;

extern void _enablePaging(uint32_t *pPageDirectory);

void kpaging_init() {
    // Create PDT for kernel
    kpaging_pdt = kmem_physalloc();

    // Create PDEs for kernel (identity map whole physical space)
    uint32_t *pde;
    for(int i = 0; i < 1024; i++) {
        pde = kmem_physalloc();

        // Create PDT entry
        kpaging_pdt[i] = (uint32_t)pde | 0x00000003;

        // Create PDE entries identity mapping this to physical addresses
        uint32_t physAddr = 0;
        for(int j = 0; j < 1024; j++) {
            pde[j] = physAddr | 0x00000003;
            physAddr += 4096;
        }
    }

    // Enable 32 bit paging by setting CR0.PG to 1 and loading CR3
    _enablePaging(kpaging_pdt);
}
