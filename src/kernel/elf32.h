
#ifndef _ELF32_H
#define _ELF32_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define EI_NIDENT       16
 
typedef struct {
    unsigned char   e_ident[EI_NIDENT]; 
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint32_t      e_entry;
    uint32_t       e_phoff;
    uint32_t       e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} elf32_header;

typedef struct {
    uint32_t      p_type;
    uint32_t       p_offset;
    uint32_t      p_vaddr;
    uint32_t      p_paddr;
    uint32_t      p_filesz;
    uint32_t      p_memsz;
    uint32_t      p_flags;
    uint32_t      p_align;
} elf32_program_header;

typedef struct {
    uint8_t *start;
    size_t length;
    elf32_header *header;
    elf32_program_header *programHeader;
    size_t numProgramHeaders;
    size_t totalBytes;
    uint32_t virtualBase;
} elf32_handle;

bool elf32_checkHeader(elf32_header *pHeader);

bool elf32_loadMem(elf32_handle *pHandle, void *pStart, size_t pLength);

#endif
