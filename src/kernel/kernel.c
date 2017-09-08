
#include "kernel/kernel.h"
#include "kernel/taskmgr.h"
#include "kernel/arch.h"

void kernel_main(struct multiboot_info *pStartupInfo) {
    // At this point arch_* interface will be fully running
    arch_dbg_setCode(0x10);

    // Initialise task manager
    /*taskmgr_init();
    // Load and run startup modules
    // mods_* present?
    int startedServers;
    if(pStartupInfo->flags & (1 << 3) && pStartupInfo->mods_count > 0) {
        //kprintf("mods_count: %u\n", pMultiBootInfo->mods_count);
        struct mods_entry *mod = (struct mods_entry*)pStartupInfo->mods_addr;
        for(uint32_t i = 0; i < pStartupInfo->mods_count; i++, mod++) {
            if(taskmgr_start((void*)mod->start, (mod->end - mod->start), PROCESS_TYPE_SERVER) > 0) {
                startedServers++;
            }
        }
    }

    if(startedServers == 0) {
        arch_dbg_setCode(0xC0);
        return;
    }

    taskmgr_switch(0, 1);*/

    // And we're running!
    while(1) {
        // TODO: yield
        __asm("hlt");
    }
}
