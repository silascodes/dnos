
#include "kterm.h"
#include "kstdio.h"
#include "kmem.h"
#include "kgdt.h"
#include "kidt.h"
#include "kpaging.h"
#include "kdbg.h"

struct multiboot_info {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint8_t boot_device[4];
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t syms[4];
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};

struct mmap_entry {
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
};

/*static void test_kprintf() {
	kprintf("Testing percent print: %%\n");
	kprintf("Testing char print: %c\n", 'z');
	kprintf("Testing string print: %s\n", "this is a test string");
	kprintf("Testing int print: %i %d\n", 1337, 1234567890);
	kprintf("Testing uint print: %u\n", 4294967295);  
	kprintf("Testing lower hex print: %x\n", 0xCAFEBABE);
	kprintf("Testing upper hex print: %X\n", 0xDEADBEEF);
}*/

/*static void test_kmem_phys() {
	uint8_t *block1 = kmem_physalloc();
	kprintf("Allocated 1 block, at address: %x\n", block1);
	kmem_physfree(block1);
	block1 = kmem_physalloc();
	kprintf("Allocated 1 block, at address: %x\n", block1);

	uint8_t *block2 = kmem_physalloc();
	kprintf("Allocated 1 block, at address: %x\n", block2);

	uint8_t *block3 = kmem_physalloc();
	kprintf("Allocated 1 block, at address: %x\n", block3);

	uint8_t *block1;
	for(int j = 0; j < 1024; j++) {
		block1 = kmem_physalloc();
		if(block1 == NULL) {
			kprintf("OUT OF MEMORY\n");
		}
		else {
			kprintf("Allocated block %d at %x\n", j, block1);
		}
	}
}*/

extern void _doInt3();
extern void _enableInterrupts();
extern uint32_t _storeGDTAddress();
 
void kmain(unsigned int pMultiBootMagic, struct multiboot_info *pMultiBootInfo) {
	kterm_init();
	
	/*for(int i = 0; i < 10; i++) {
		kterm_puts("Hello, kernel World!\n");
	}*/
	
	//test_kprintf();
	
	kprintf("Booting OS...\n");
	
	// Check we were booted by a multiboot compliant bootloader
	if(pMultiBootMagic != 0x2badb002) {
		kprintf("ERROR: non multiboot compliant bootloaders are not supported yet\n");
		return;
	}
	
	kprintf("Multiboot Flags: %x\n", pMultiBootInfo->flags);
	
	// mem_* fields present?
	if(pMultiBootInfo->flags & (1 << 0)) {
		kprintf("mem_lower: %u kB\nmem_upper: %u kB\n", pMultiBootInfo->mem_lower, pMultiBootInfo->mem_upper);
	}
	
	// boot_device present?
	if(pMultiBootInfo->flags & (1 << 1)) {
		kprintf("boot_device: %u:%u-%u-%u\n", pMultiBootInfo->boot_device[0], pMultiBootInfo->boot_device[1], pMultiBootInfo->boot_device[2], pMultiBootInfo->boot_device[3]);
	}
	
	// cmdline present?
	if(pMultiBootInfo->flags & (1 << 2)) {
		kprintf("cmdline: %s\n", (char*)pMultiBootInfo->cmdline);
	}
	
	// mods_* present?
	if(pMultiBootInfo->flags & (1 << 3)) {
		kprintf("mods_count: %u\n", pMultiBootInfo->mods_count);
	}
	
	// mmap_* present?
	if(pMultiBootInfo->flags & (1 << 6)) {
		kprintf("mmap_length: %u\nmmap_addr: %x\n", pMultiBootInfo->mmap_length, pMultiBootInfo->mmap_addr);
		
		struct mmap_entry *entry;
		uint32_t size;
		uint8_t *ptr = (uint8_t*)pMultiBootInfo->mmap_addr;
		while(ptr < ((uint8_t*)pMultiBootInfo->mmap_addr + pMultiBootInfo->mmap_length)) {
			size = *ptr;
			ptr += 4;
			
			entry = (struct mmap_entry*)ptr;
			ptr += size;
			
			kprintf("[%y - %U - %u]\n", entry->base_addr, entry->length, entry->type);
		}
	}
	
	// drives_* present?
	if(pMultiBootInfo->flags & (1 << 7)) {
		kprintf("drives_length: %u\ndrives_addr: %x\n", pMultiBootInfo->drives_length, pMultiBootInfo->drives_addr);
	}
	
	// config_table present?
	if(pMultiBootInfo->flags & (1 << 8)) {
		kprintf("config_table: %x\n", pMultiBootInfo->config_table);
	}
	
	// boot_loader_name present?
	if(pMultiBootInfo->flags & (1 << 9)) {
		//kprintf("boot_loader_name: %s\n", (char*)pMultiBootInfo->boot_loader_name);
	}
	
	// apm_table present?
	if(pMultiBootInfo->flags & (1 << 10)) {
		kprintf("apm_table: %x\n", pMultiBootInfo->apm_table);
	}
	
	// vbe_* present?
	if(pMultiBootInfo->flags & (1 << 11)) {
		//kprintf("VBE present\n");
	}

	// Init physical memory allocator with the range 2mB-6mB (later from mmap with possibly non contiguous regions... put it on the TODO)
	kmem_physinit(0x200000, 0x400000);

	// Init GDT
	kgdt_init();

	// Init IDT
	kidt_init();
	//_enableInterrupts();


	int val = 123;
	int val2 = 1;
	int otherVal = val / val2;

	//_doInt3();

	kpaging_init();

	// TODO: we really need to have a TSS in the GDT

	kprintf("kmain done... (%d)\n", otherVal);
}
