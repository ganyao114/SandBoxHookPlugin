#ifndef __sotool_h__
#define __sotool_h__

#include <unistd.h>
#include <elf.h>

#if defined(__arm__) || defined(__i386__)
#define elf_header_t elf32_header_t
#define elf_pheader_t elf32_pheader_t
#define elf_sheader_t elf32_sheader_t
#define elf_sym_t elf32_sym_t
#else
#error "Not supported"
#endif

typedef struct {
    uint8_t  ident[16];    /* The first 4 bytes are the ELF magic */

    uint16_t type;         /* == 2, EXEC (executable file) */
    uint16_t machine;      /* == 8, MIPS r3000 */
    uint32_t version;      /* == 1, default ELF value */
    uint32_t entry;        /* program starting point */
    uint32_t phoff;        /* program header offset in the file */

    uint32_t shoff;        /* section header offset in the file, unused for us, so == 0 */
    uint32_t flags;        /* flags, unused for us. */
    uint16_t ehsize;       /* this header size ( == 52 ) */
    uint16_t phentsize;    /* size of a program header ( == 32 ) */
    uint16_t phnum;        /* number of program headers */
    uint16_t shentsize;    /* size of a section header, unused here */

    uint16_t shnum;        /* number of section headers, unused here */
    uint16_t shstrndx;     /* section index of the string table */
} elf32_header_t;

typedef struct {
    uint32_t type;         /* == 1, PT_LOAD (that is, this section will get loaded */
    uint32_t offset;       /* offset in file, on a 4096 bytes boundary */
    uint32_t vaddr;        /* virtual address where this section is loaded */
    uint32_t paddr;        /* physical address where this section is loaded */
    uint32_t filesz;       /* size of that section in the file */
    uint32_t memsz;        /* size of that section in memory (rest is zero filled) */
    uint32_t flags;        /* PF_X | PF_W | PF_R, that is executable, writable, readable */
    uint32_t align;        /* == 0x1000 that is 4096 bytes */
} elf32_pheader_t;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} elf32_sheader_t;

typedef struct {
    uint32_t name;
    uint32_t value;
    uint32_t size;
    uint8_t info;
    uint8_t other;
    uint16_t shndx;
} elf32_sym_t;

#endif /* __sotool_h__ */