
#include "kernel/elf32.h"
#include "kernel/arch.h"
#include "kernel/kstring.h"

/**
 * Check an elf32 header is appropriate to run on the current system
 */
bool elf32_checkHeader(elf32_header *pHeader) {
    // Check this is an ELF file
    if(pHeader->e_ident[0] != 0x7F && pHeader->e_ident[1] != 'E' && pHeader->e_ident[2] != 'L' && pHeader->e_ident[3] != 'F') {
        arch_dbg_setCode(0xE0);
        return false;
    }

    // Check 32 bit little endian x86 (TODO: check current arch/bits/endian)
    if(pHeader->e_ident[4] != 1 || pHeader->e_ident[5] != 1 || pHeader->e_type != 2 || pHeader->e_machine != 3) {
        arch_dbg_setCode(0xE1);
        return false;
    }

    return true;
}

/**
 * Load and parse and elf32 file that is currently in memory
 */
bool elf32_loadMem(elf32_handle *pHandle, void *pStart, size_t pLength) {
    // Sanity check parameters
    if(!pHandle || !pStart || pLength < sizeof(elf32_header)) {
        return false;
    }

    // Set basic memory position/size values
    pHandle->start = (uint8_t*)pStart;
    pHandle->length = pLength;

    // Get header into handle, check its good for this arch
    pHandle->header = (elf32_header*)pHandle->start;
    if(!elf32_checkHeader(pHandle->header)) {
        return false;
    }

    // Check program header exists
    if(pHandle->header->e_phoff == 0 || pHandle->header->e_phnum == 0) {
        arch_dbg_setCode(0xE2);
        return false;
    }

    // Load program headers
    pHandle->programHeader = (elf32_program_header*)pHandle->header->e_phoff;
    pHandle->numProgramHeaders = pHandle->header->e_phnum;

    // Figure out total in memory space requirement
    pHandle->totalBytes = 0;
    pHandle->virtualBase = 0;
    for(size_t i = 0; i < pHandle->numProgramHeaders; i++) {
        // Only interested in LOAD type entires
        if(pHandle->programHeader[i].p_type != 1) {
            continue;
        }

        // If this is first LOAD entry, set our virtual base
        if(pHandle->virtualBase == 0) {
            pHandle->virtualBase = pHandle->programHeader[i].p_vaddr;
        }

        // The last entries results should give us the correct value...
        pHandle->totalBytes = (pHandle->programHeader[i].p_vaddr - pHandle->virtualBase) + pHandle->programHeader[i].p_memsz;
    }

    return true;
}
