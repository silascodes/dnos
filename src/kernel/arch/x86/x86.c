
#include <stddef.h>
#include "kernel/multiboot.h"
#include "kernel/arch.h"
#include "kernel/kernel.h"
#include "kernel/kstring.h"
#include "kernel/term.h"
#include "kernel/kstdio.h"

// ----------
// Early debug output
// ----------

void _x86_dbg_clear() {
    uint16_t *clearMem = (uint16_t*)0xB8000;
    size_t entries = 80 * 25;
    for(size_t i = 0; i < entries; i++, clearMem++) {
        *clearMem = 0x0700;
    }

    uint8_t msg[12] = "arch-x86: 00";
    uint8_t *writeMem = (uint8_t*)0xB8000;
    for(int i = 0; i < 12; i++, writeMem += 2) {
        *writeMem = msg[i];
    }
}

void _x86_dbg_setCode(uint8_t pCode) {
    uint8_t *writeMem = (uint8_t*)0xB8014;
    for(int i = 0; i < 2; i++, writeMem += 2) {
        uint8_t nibble = 0;
        if(i == 0) {
            nibble = pCode >> 4;
        }
        else {
            nibble = pCode & 0x0F;
        }

        if(nibble <= 0x09) {
            *writeMem = 0x30 + nibble;
        }
        else {
            *writeMem = 0x37 + nibble;
        }
    }
}



// ----------
// GDT
// ----------

static uint8_t *kgdt_table;

typedef struct {
    uint16_t prevTask;
    uint16_t reserved1;
    uint32_t esp0;
    uint16_t ss0;
    uint16_t reserved2;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t reserved3;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t reserved4;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t reserved5;
    uint16_t cs;
    uint16_t reserved6;
    uint16_t ss;
    uint16_t reserved7;
    uint16_t ds;
    uint16_t reserved8;
    uint16_t fs;
    uint16_t reserved9;
    uint16_t gs;
    uint16_t reserved10;
    uint16_t ldt;
    uint16_t reserved11;
    uint16_t reserved12; // The first bit of this is actually the "debug trap" flag
    uint16_t ioMapBase;
} x86_tss;
static x86_tss ktss;

extern void _enableInterrupts();
extern void _disableInterrupts();
extern void _loadGDT(void *pTable, uint16_t pSize);
extern void _reloadSegs();
extern void _loadTR();

void _x86_tss_init() {
    kmemset(&ktss, 0, sizeof(ktss));
    ktss.ioMapBase = sizeof(ktss);      // Apparently set this to size if it doesnt exist? Check this maybe...
    ktss.ss0 = 0x10;                    // Kernel data segment
    // TODO: we also need to set ktss.esp0 to kernel stack? not sure
}

uint64_t _x86_gdt_encode(uint32_t base, uint32_t limit, uint16_t flag) {
    uint64_t descriptor;
 
    // Create the high 32 bit segment
    descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
    descriptor |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
    descriptor |=  base        & 0xFF000000;         // set base bits 31:24
 
    // Shift by 32 to allow for low part of segment
    descriptor <<= 32;
 
    // Create the low 32 bit segment
    descriptor |= base  << 16;                       // set base bits 15:0
    descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0
 
    return descriptor;
}

void _x86_gdt_init() {
    // One physical frame allows us 512 entries, should be enough for now
    kgdt_table = (uint8_t*)arch_mem_physAlloc();

    // Initialise table to 0 (this also will setup null descriptor)
    kmemset(kgdt_table, 0, 4096);

    // Setup our basic flat model entries
    uint64_t *gdt = (uint64_t*)kgdt_table;
    gdt++;
    *gdt = _x86_gdt_encode(0, 0xFFFFFFFF, 0xC09A);        // Code segment
    gdt++;
    *gdt = _x86_gdt_encode(0, 0xFFFFFFFF, 0xC092);        // Data segment
    gdt++;
    _x86_tss_init();
    uint32_t tssBase = (uint32_t)&ktss;
    *gdt = _x86_gdt_encode(tssBase, tssBase + sizeof(ktss), 0x4089);     // TSS

    // Load the GDT
    _loadGDT(kgdt_table, 32); // Size - 1? Intel manual says 8n-1 but bootloader is 32 with 4 entires? also actually has 4096 bytes at this point... pass small val for now
    _reloadSegs();

    _loadTR();

    //kprintf("Initial GDT loaded\n");
}



// ----------
// IDT
// ----------

static uint8_t *kidt_table;

extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

extern void _loadIDT(void *pTable, uint16_t pSize);

static inline void outb(uint16_t port, uint8_t val)
{
    __asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
    return ret;
}

static inline void io_wait(void)
{
    /* TODO: This is probably fragile. */
    __asm volatile ( "jmp 1f\n\t"
                   "1:jmp 2f\n\t"
                   "2:" );
}

#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1

void _x86_pic_disable() {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void _x86_pic_remap() {
    uint8_t mask1, mask2;

    mask1 = inb(PIC1_DATA);
    mask2 = inb(PIC2_DATA);

    outb(PIC1_CMD, 0x11);
    io_wait();
    outb(PIC2_CMD, 0x11);
    io_wait();

    outb(PIC1_DATA, 32);
    io_wait();
    outb(PIC2_DATA, 40);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void _x86_pic_setMask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void _x86_pic_clearMask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

uint64_t _x86_idt_encode(uint32_t pOffset, uint16_t pSelector) {
    uint64_t descriptor = 0;

    descriptor |= (pOffset & 0xFFFF0000);       // Offset high
    descriptor |= 0x00008E00;                   // Type, present (this value means present 32bit interrupt gate)

    descriptor <<= 32;

    descriptor |= ((uint32_t)pSelector << 16);  // Selector
    descriptor |= (pOffset & 0x0000FFFF);       // Offset low

    return descriptor;
}

void _x86_idt_init() {
    kidt_table = (uint8_t*)arch_mem_physAlloc();

    kmemset(kidt_table, 0, 4096);

    uint64_t *ptr = (uint64_t*)kidt_table;

    // NOTE: all reserved interrupts are mapped to the first reserved interrupt (15)
    ptr[0] = _x86_idt_encode((uint32_t)&_isr0, 0x08);
    ptr[1] = _x86_idt_encode((uint32_t)&_isr1, 0x08);
    ptr[2] = _x86_idt_encode((uint32_t)&_isr2, 0x08);
    ptr[3] = _x86_idt_encode((uint32_t)&_isr3, 0x08);
    ptr[4] = _x86_idt_encode((uint32_t)&_isr4, 0x08);
    ptr[5] = _x86_idt_encode((uint32_t)&_isr5, 0x08);
    ptr[6] = _x86_idt_encode((uint32_t)&_isr6, 0x08);
    ptr[7] = _x86_idt_encode((uint32_t)&_isr7, 0x08);
    ptr[8] = _x86_idt_encode((uint32_t)&_isr8, 0x08);
    ptr[9] = _x86_idt_encode((uint32_t)&_isr9, 0x08);
    ptr[10] = _x86_idt_encode((uint32_t)&_isr10, 0x08);
    ptr[11] = _x86_idt_encode((uint32_t)&_isr11, 0x08);
    ptr[12] = _x86_idt_encode((uint32_t)&_isr12, 0x08);
    ptr[13] = _x86_idt_encode((uint32_t)&_isr13, 0x08);
    ptr[14] = _x86_idt_encode((uint32_t)&_isr14, 0x08);
    ptr[15] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[16] = _x86_idt_encode((uint32_t)&_isr16, 0x08);
    ptr[17] = _x86_idt_encode((uint32_t)&_isr17, 0x08);
    ptr[18] = _x86_idt_encode((uint32_t)&_isr18, 0x08);
    ptr[19] = _x86_idt_encode((uint32_t)&_isr19, 0x08);
    ptr[20] = _x86_idt_encode((uint32_t)&_isr20, 0x08);
    ptr[21] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[22] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[23] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[24] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[25] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[26] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[27] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[28] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[29] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[30] = _x86_idt_encode((uint32_t)&_isr15, 0x08);
    ptr[31] = _x86_idt_encode((uint32_t)&_isr15, 0x08);

    ptr[32] = _x86_idt_encode((uint32_t)&_irq0, 0x08);
    ptr[33] = _x86_idt_encode((uint32_t)&_irq1, 0x08);
    ptr[34] = _x86_idt_encode((uint32_t)&_irq2, 0x08);
    ptr[35] = _x86_idt_encode((uint32_t)&_irq3, 0x08);
    ptr[36] = _x86_idt_encode((uint32_t)&_irq4, 0x08);
    ptr[37] = _x86_idt_encode((uint32_t)&_irq5, 0x08);
    ptr[38] = _x86_idt_encode((uint32_t)&_irq6, 0x08);
    ptr[39] = _x86_idt_encode((uint32_t)&_irq7, 0x08);
    ptr[40] = _x86_idt_encode((uint32_t)&_irq8, 0x08);
    ptr[41] = _x86_idt_encode((uint32_t)&_irq9, 0x08);
    ptr[42] = _x86_idt_encode((uint32_t)&_irq10, 0x08);
    ptr[43] = _x86_idt_encode((uint32_t)&_irq11, 0x08);
    ptr[44] = _x86_idt_encode((uint32_t)&_irq12, 0x08);
    ptr[45] = _x86_idt_encode((uint32_t)&_irq13, 0x08);
    ptr[46] = _x86_idt_encode((uint32_t)&_irq14, 0x08);
    ptr[47] = _x86_idt_encode((uint32_t)&_irq15, 0x80);

    _loadIDT(kidt_table, 4095);

    _x86_pic_remap();
    _x86_pic_disable();
    //_x86_pic_clearMask(0);
    _x86_pic_clearMask(1);

    //kprintf("Initial IDT loaded\n");
}

void _x86_irq_handler(/*uint32_t pIRQ*/) {
    kprintf("IRQ\n");
}



// ----------
// Paging
// ----------

static uint32_t *_x86_paging_pdt;

extern void _x86_enablePaging(uint32_t *pPageDirectory);

void _x86_paging_init() {
    // Create PDT for kernel
    _x86_paging_pdt = arch_mem_physAlloc();

    // Create PDEs for kernel (identity map whole physical space)
    uint32_t *pde;
    for(int i = 0; i < 1024; i++) {
        pde = arch_mem_physAlloc();

        // Create PDT entry
        _x86_paging_pdt[i] = (uint32_t)pde | 0x00000003;

        // Create PDE entries identity mapping this to physical addresses
        uint32_t physAddr = 0;
        for(int j = 0; j < 1024; j++) {
            pde[j] = physAddr | 0x00000003;
            physAddr += 4096;
        }
    }

    // Enable 32 bit paging by setting CR0.PG to 1 and loading CR3
    _x86_enablePaging(_x86_paging_pdt);
}



// ----------
// Memory info
// ----------

/*void _x86_getMemoryMap(struct startup_info *pStartupInfo, struct multiboot_info *pBootInfo) {
    // mem_* fields present?
    if(pBootInfo->flags & (1 << 0)) {
        //kprintf("mem_lower: %u kB\nmem_upper: %u kB\n", pBootInfo->mem_lower, pBootInfo->mem_upper);
    }

    // mmap_* present?
    if(pBootInfo->flags & (1 << 6)) {
        //kprintf("mmap_length: %u\nmmap_addr: %x\n", pBootInfo->mmap_length, pBootInfo->mmap_addr);
        
        struct mmap_entry *entry;
        uint32_t size;
        uint8_t *ptr = (uint8_t*)pBootInfo->mmap_addr;
        while(ptr < ((uint8_t*)pBootInfo->mmap_addr + pBootInfo->mmap_length)) {
            size = *ptr;
            ptr += 4;
            
            entry = (struct mmap_entry*)ptr;
            ptr += size;
            
            //kprintf("[%y - %U - %u]\n", entry->base_addr, entry->length, entry->type);
        }
    }

    // TODO: try another way? BIOS?
}*/



// ----------
// x86 initialisation entry point
// ----------

extern void arch_mem_physInit(uint32_t pBase, size_t pSize, size_t pPageSize);

void _x86_init(uint32_t pBootMagic, struct multiboot_info *pBootInfo) {
    kterm_init();
    kprintf("\n");

    //struct startup_info startupInfo;    // TODO: this needs zeroing
    _x86_dbg_clear();

    // Check multiboot magic
    if(pBootMagic != 0x2BADB002) {
        //kprintf("ERROR: non multiboot compliant bootloaders are not supported yet\n");
        _x86_dbg_setCode(0xB0);
        return;
    }

    // Get memory map
    //_x86_getMemoryMap(&startupInfo, pBootInfo);
    // _x86_dbg_setCode(0x01);
    arch_mem_physInit(0x200000, 0x400000, 4096);
    _x86_dbg_setCode(0x02);

    // Setup GDT
    _x86_gdt_init();
    _x86_dbg_setCode(0x03);

    // Setup IDT
    _x86_idt_init();
    _x86_dbg_setCode(0x04);
    _enableInterrupts();

    // Setup paging
    //_x86_paging_init();
    //_x86_dbg_setCode(0x05);

    // Call kernel
    kernel_main(pBootInfo);
}
