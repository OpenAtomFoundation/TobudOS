/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _ELF32_H_
#define  _ELF32_H_

#define EI_NIDENT   16

typedef int32_t     elf32_sword;
typedef uint32_t    elf32_word;
typedef uint16_t    elf32_half;
typedef uint32_t    elf32_off;
typedef uint32_t    elf32_addr;

/* elf header */
typedef struct elf32_ehdr {
    unsigned char   e_ident[EI_NIDENT];     /* ident bytes */
    elf32_half      e_type;                 /* file type */
    elf32_half      e_machine;              /* target machine */
    elf32_word      e_version;              /* file version */
    elf32_addr      e_entry;                /* start address */
    elf32_off       e_phoff;                /* phdr file offset */
    elf32_off       e_shoff;                /* shdr file offset */
    elf32_word      e_flags;                /* file flags */
    elf32_half      e_ehsize;               /* sizeof ehdr */
    elf32_half      e_phentsize;            /* sizeof phdr */
    elf32_half      e_phnum;                /* number phdrs */
    elf32_half      e_shentsize;            /* sizeof shdr */
    elf32_half      e_shnum;                /* number shdrs */
    elf32_half      e_shstrndx;             /* shdr string index */
} elf32_ehdr_t;

/* values for e_type. */
#define ET_NONE     0                       /* unknown type. */
#define ET_REL      1                       /* relocatable. */
#define ET_EXEC     2                       /* executable. */
#define ET_DYN      3                       /* shared object. */

/* section header */
typedef struct elf32_shdr {
    elf32_word      sh_name;                /* section name */
    elf32_word      sh_type;                /* SHT_... */
    elf32_word      sh_flags;               /* SHF_... */
    elf32_addr      sh_addr;                /* virtual address */
    elf32_off       sh_offset;              /* file offset */
    elf32_word      sh_size;                /* section size */
    elf32_word      sh_link;                /* misc info */
    elf32_word      sh_info;                /* misc info */
    elf32_word      sh_addralign;           /* memory alignment */
    elf32_word      sh_entsize;             /* entry size if table */
} elf32_shdr_t;

/* values for sh_type */
#define SHT_NULL        0                   /* inactive */
#define SHT_PROGBITS    1                   /* program defined information */
#define SHT_SYMTAB      2                   /* symbol table section */
#define SHT_STRTAB      3                   /* string table section */
#define SHT_RELA        4                   /* relocation section with addends*/
#define SHT_HASH        5                   /* symbol hash table section */
#define SHT_DYNAMIC     6                   /* dynamic section */
#define SHT_NOTE        7                   /* note section */
#define SHT_NOBITS      8                   /* no space section */
#define SHT_REL         9                   /* relation section without addends */
#define SHT_SHLIB       10                  /* reserved - purpose unknown */
#define SHT_DYNSYM      11                  /* dynamic symbol table section */
#define SHT_LOPROC      0x70000000          /* reserved range for processor */
#define SHT_HIPROC      0x7fffffff          /* specific section header types */
#define SHT_LOUSER      0x80000000          /* reserved range for application */
#define SHT_HIUSER      0xffffffff          /* specific indexes */

/* values for sh_flags */
#define SHF_WRITE       1                   /* writable */
#define SHF_ALLOC       2                   /* occupies memory */
#define SHF_EXECINSTR   4                   /* executable */

typedef struct elf32_rel {
    elf32_addr      r_offset;               /* location to be relocated. */
    elf32_word      r_info;                 /* relocation type and symbol index. */
} elf32_rel_t;

typedef struct elf32_rela {
    elf32_addr      r_offset;               /* location to be relocated. */
    elf32_word      r_info;                 /* relocation type and symbol index. */
    elf32_sword     r_addend;               /* addend. */
} elf32_rela_t;

typedef struct elf32_sym {
    elf32_word      st_name;                /* string table index of name. */
    elf32_addr      st_value;               /* symbol value. */
    elf32_word      st_size;                /* size of associated object. */
    unsigned char   st_info;                /* type and binding information. */
    unsigned char   st_other;               /* reserved (not used). */
    elf32_half      st_shndx;               /* section index of symbol. */
} elf32_sym_t;

/* values for st_info(binding) */
#define STB_LOCAL           0
#define STB_GLOBAL          1
#define STB_WEAK            2

/* values for st_info(type) */
#define STT_NOTYPE          0
#define STT_OBJECT          1
#define STT_FUNC            2
#define STT_SECTION         3
#define STT_FILE            4

/* values for st_shndx */
#define SHN_ABS             0xFFF1
#define SHN_COMMON          0xFFF2
#define SHN_UNDEF           0x0000

#define ELF32_SYM_BINDING(info)     ((info) >> 4)
#define ELF32_SYM_TYPE(info)        ((info) & 0x0F)

typedef struct elf32_dyn {
    elf32_sword     d_tag;                  /* DT_... */
    union {
        elf32_word  d_val;
        elf32_addr  d_ptr;
    } d_un;
} elf32_dyn_t;

/* values for d_tag */
#define DT_NULL             0x00
#define DT_PLTRELSZ         0x02
#define DT_PLTGOT           0x03
#define DT_HASH             0x04
#define DT_STRTAB           0x05
#define DT_SYMTAB           0x06
#define DT_STRSZ            0x0a
#define DT_SYMENT           0x0b
#define DT_REL              0x11
#define DT_RELSZ            0x12
#define DT_RELENT           0x13
#define DT_PLTREL           0x14
#define DT_JMPREL           0x17

/* program header */
typedef struct elf32_phdr {
    elf32_word      p_type;                 /* PHT_... */
    elf32_off       p_offset;               /* file offset */
    elf32_addr      p_vaddr;                /* virtual address */
    elf32_addr      p_paddr;                /* physical address */
    elf32_word      p_filesz;               /* file size */
    elf32_word      p_memsz;                /* memory size */
    elf32_word      p_flags;                /* read write properties */
    elf32_word      p_align;                /* alignment attribute, 2 ^ p_align */
} elf32_phdr_t;

/* values for p_type */
#define PHT_LOAD            0x01
#define PHT_DYNAMIC         0x02

#define ELF32_R_SYM(info)            ((info) >> 8)
#define ELF32_R_TYPE(info)           ((unsigned char)(info))

#define ELF_MAGIC_HEADER            "\177ELF\001\001\001"
#define ELF_MAGIC_HEADER_SIZE         7

static const unsigned char elf_header_magic[] = {
    0x7f, 0x45, 0x4c, 0x46,     /* Magic:   0x7f, 'E', 'L', 'F' */
    0x01,                       /* Class:   ELF32 */
    0x01,                       /* Data:    2's complement, 'little endian */
    0x01,                       /* Version: 1(current) */
};

#endif /* _ELF32_H_ */

