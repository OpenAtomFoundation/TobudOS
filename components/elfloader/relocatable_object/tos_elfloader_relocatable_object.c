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

#include "tos_elfloader.h"

__STATIC__ void *local_symtab_lookup(int fd, char *sym_name,
                                                el_section_t bss, el_section_t data, el_section_t rodata, el_section_t text,
                                                uint32_t symtab_offset, uint32_t symtab_size, uint32_t symtab_entsize,
                                                uint32_t strtab_offset)
{
    int i = 0;

    elf32_sym_t sym;
#define SYMBOL_NAME_MAX     30
    static char name[SYMBOL_NAME_MAX];

    for (i = 0; i < symtab_size / symtab_entsize; ++i) {
        if (elfloader_fd_read(fd, symtab_offset, &sym, sizeof(elf32_sym_t)) != ELFLOADER_ERR_NONE) {
            return K_NULL;
        }

        if (sym.st_name) {
            if (elfloader_fd_read(fd, strtab_offset + sym.st_name, name, sizeof(name)) != ELFLOADER_ERR_NONE) {
                 return K_NULL;
            }

            if (strcmp(name, sym_name) == 0) {
                if (sym.st_shndx == bss.shndx && bss.address) {
                    return (void *)((uint32_t)bss.address + sym.st_value);
                } else if (sym.st_shndx == data.shndx && data.address) {
                    return (void *)((uint32_t)data.address + sym.st_value);
                } else if (sym.st_shndx == rodata.shndx && rodata.address) {
                    return (void *)((uint32_t)rodata.address + sym.st_value);
                } else if (sym.st_shndx == text.shndx && text.address) {
                    return (void *)((uint32_t)text.address + sym.st_value);
                } else {
                    return K_NULL;
                }
            }
        }

        symtab_offset += symtab_entsize;
    }

    return K_NULL;
}

__STATIC__ el_err_t elfloader_relocate(int fd, void *address,
                                                 el_section_t bss, el_section_t data, el_section_t rodata, el_section_t text,
                                                 uint32_t rel_offset, uint32_t rel_size, uint32_t rel_entsize,
                                                 uint32_t symtab_offset, uint32_t symtab_size, uint32_t symtab_entsize,
                                                 uint32_t strtab_offset, uint32_t strtab_size)
{
    int i = 0;
    elf32_rela_t rela;

    elf32_sym_t sym;
    void *addr;
#define SYMBOL_NAME_MAX    30
    static char name[SYMBOL_NAME_MAX];

    int is_rela = (rel_entsize == sizeof(elf32_rela_t) ? K_TRUE : K_FALSE);

    for (i = 0; i < rel_size / rel_entsize; ++i) {
        addr = K_NULL;

        if (elfloader_fd_read(fd, rel_offset, &rela, rel_entsize) != ELFLOADER_ERR_NONE) {
            return ELFLOADER_ERR_FD_READ_FAILED;
        }

        if (elfloader_fd_read(fd,
                                symtab_offset + ELF32_R_SYM(rela.r_info) * symtab_entsize,
                                &sym,
                                symtab_entsize) != ELFLOADER_ERR_NONE) {
            return ELFLOADER_ERR_FD_READ_FAILED;
        }

        if (sym.st_name) {
            /* load the symbol's name */
            if (elfloader_fd_read(fd,
                                     strtab_offset + sym.st_name,
                                     name,
                                     sizeof(name)) != ELFLOADER_ERR_NONE) {
                return ELFLOADER_ERR_FD_READ_FAILED;
            }

            /* an external symbol, or a weak one */
            if (sym.st_shndx == SHN_UNDEF ||
                ELF32_SYM_TYPE(sym.st_info) == STB_WEAK) {
                /* look up in the global symbol table */
                addr = elfloader_symtab_lookup(name);
            }

            /* an external symbol but not found in the global symbol table */
            if (sym.st_shndx == SHN_UNDEF && !addr) {
                return ELFLOADER_ERR_SYM_NOT_FOUND;
            }

            /* an internal symbol, or a weak symbol without STRONG one in global symbol table */
            if (!addr) {
                addr = local_symtab_lookup(fd, name,
                                             bss, data, rodata, text,
                                             symtab_offset, symtab_size, symtab_entsize,
                                             strtab_offset);
            }
        } else  {
            if (sym.st_shndx == bss.shndx) {
                addr = bss.address;
            } else if (sym.st_shndx == data.shndx) {
                addr = data.address;
            } else if (sym.st_shndx == rodata.shndx) {
                addr = rodata.address;
            } else if (sym.st_shndx == text.shndx) {
                addr = text.address;
            } else {
                return ELFLOADER_ERR_SECTION_NOT_FOUND;
            }
        }

        /* still not found */
        if (!addr) {
            return ELFLOADER_ERR_SYM_NOT_FOUND;
        }

        if (elfloader_arch_relocate(rela.r_offset + (uint32_t)address, 0, (uint32_t)addr, &rela, is_rela, &sym) != ELFLOADER_ERR_NONE) {
            return ELFLOADER_ERR_RELOCATE_FAILED;
        }

        rel_offset += rel_entsize;
    }

    return ELFLOADER_ERR_NONE;
}

__API__ el_err_t tos_elfloader_load(el_module_t *module, int fd)
{
    int i = 0;
    el_err_t err;
    elf32_ehdr_t ehdr;
    elf32_shdr_t shdr;

    static el_section_t bss     = { -1, K_NULL };
    static el_section_t data    = { -1, K_NULL };
    static el_section_t rodata  = { -1, K_NULL };
    static el_section_t text    = { -1, K_NULL };

    void *base = K_NULL, *addr_sec2cp;  /* ram base for LOAD sections */

    uint32_t shdr_offset;

    uint32_t strtab_offset, strtab_size = 0;
    uint32_t symtab_offset, symtab_size = 0, symtab_entsize;

    uint32_t text_offset = 0, text_size = 0;
    uint32_t data_offset = 0, data_size = 0;
    uint32_t rodata_offset = 0, rodata_size = 0;
    uint32_t bss_size = 0;

    uint32_t rel_text_offset, rel_text_size = 0, rel_text_entsize;
    uint32_t rel_data_offset, rel_data_size = 0, rel_data_entsize;
    uint32_t rel_rodata_offset, rel_rodata_size = 0, rel_rodata_entsize;

    uint32_t rela_text_offset, rela_text_size = 0, rela_text_entsize;
    uint32_t rela_data_offset, rela_data_size = 0, rela_data_entsize;
    uint32_t rela_rodata_offset, rela_rodata_size = 0, rela_rodata_entsize;

    if (!module) {
        return ELFLOADER_ERR_PTR_NULL;
    }

    memset(module, 0, sizeof(el_module_t));

    /* read the elf header */
    if (elfloader_fd_read(fd, 0, &ehdr, sizeof(elf32_ehdr_t)) != ELFLOADER_ERR_NONE) {
        return ELFLOADER_ERR_FD_READ_FAILED;
    }

    /* sanity check, magic verify */
    if (memcmp(ehdr.e_ident, elf_header_magic, sizeof(elf_header_magic)) != 0) {
        return ELFLOADER_ERR_HEADER_INVALID;
    }

    /* it should be a relocatable object */
    if (ehdr.e_type != ET_REL) {
        return ELFLOADER_ERR_TYPE_INVALID;
    }

    shdr_offset = ehdr.e_shoff;

    for (i = 0; i < ehdr.e_shnum; ++i) {
        if (elfloader_fd_read(fd, shdr_offset, &shdr, sizeof(elf32_shdr_t)) != ELFLOADER_ERR_NONE) {
            return ELFLOADER_ERR_FD_READ_FAILED;
        }

        /*
        |-----------------------------------------------------|
        |  Name    |   sh_type    |         sh_flag           |
        |----------|--------------|---------------------------|
        | .bss     | SHT_NOBITS   | SHF_ALLOC + SHF_WRITE     |
        | .data    | SHT_PROGBITS | SHF_ALLOC + SHF_WRITE     |
        | .rodata  | SHT_PROGBITS | SHF_ALLOC                 |
        | .text    | SHT_PROGBITS | SHF_ALLOC + SHF_EXECINSTR |
        |-----------------------------------------------------|
         */
        if (shdr.sh_type == SHT_NULL) {
            ;
        } else if (shdr.sh_type == SHT_STRTAB) {
            strtab_offset       = shdr.sh_offset;
            strtab_size         = shdr.sh_size;
        } else if (shdr.sh_type == SHT_SYMTAB) {
            symtab_offset       = shdr.sh_offset;
            symtab_size         = shdr.sh_size;
            symtab_entsize      = shdr.sh_entsize;
        } else if (shdr.sh_type == SHT_NOBITS && shdr.sh_flags == (SHF_ALLOC | SHF_WRITE)) {
            bss_size            = shdr.sh_size;

            bss.shndx           = i;
        } else if (shdr.sh_type == SHT_PROGBITS && shdr.sh_flags == (SHF_ALLOC | SHF_WRITE)) {
            data_offset         = shdr.sh_offset;
            data_size           = shdr.sh_size;

            data.shndx          = i;
        } else if (shdr.sh_type == SHT_PROGBITS && shdr.sh_flags == SHF_ALLOC) {
            rodata_offset       = shdr.sh_offset;
            rodata_size         = shdr.sh_size;

            rodata.shndx        = i;
        } else if (shdr.sh_type == SHT_PROGBITS && shdr.sh_flags == (SHF_ALLOC | SHF_EXECINSTR)) {
            text_offset         = shdr.sh_offset;
            text_size           = shdr.sh_size;

            text.shndx          = i;
        } else if (shdr.sh_type == SHT_REL) {
            if (shdr.sh_info == data.shndx) {
                rel_data_offset     = shdr.sh_offset;
                rel_data_size       = shdr.sh_size;
                rel_data_entsize    = shdr.sh_entsize;
            } else if (shdr.sh_info == rodata.shndx) {
                rel_rodata_offset   = shdr.sh_offset;
                rel_rodata_size     = shdr.sh_size;
                rel_rodata_entsize  = shdr.sh_entsize;
            } else if (shdr.sh_info == text.shndx) {
                rel_text_offset     = shdr.sh_offset;
                rel_text_size       = shdr.sh_size;
                rel_text_entsize    = shdr.sh_entsize;
            }
        } else if (shdr.sh_type == SHT_RELA) {
            if (shdr.sh_info == data.shndx) {
                rela_data_offset    = shdr.sh_offset;
                rela_data_size      = shdr.sh_size;
                rela_data_entsize   = shdr.sh_entsize;
            } else if (shdr.sh_info == rodata.shndx) {
                rela_rodata_offset  = shdr.sh_offset;
                rela_rodata_size    = shdr.sh_size;
                rela_rodata_entsize = shdr.sh_entsize;
            } else if (shdr.sh_info == text.shndx) {
                rela_text_offset    = shdr.sh_offset;
                rela_text_size      = shdr.sh_size;
                rela_text_entsize   = shdr.sh_entsize;
            }
        }

        shdr_offset += ehdr.e_shentsize;
    }

    if (symtab_size == 0) {
        return ELFLOADER_ERR_NO_SYMTAB;
    }
    if (strtab_size == 0) {
        return ELFLOADER_ERR_NO_STRTAB;
    }
    if (text_size == 0) {
        return ELFLOADER_ERR_NO_TEXT;
    }

    base = tos_mmheap_alloc(text_size + data_size + rodata_size + bss_size);
    if (!base) {
        return ELFLOADER_ERR_OUT_OF_MEMORY;
    }

    /* do sections load */
    addr_sec2cp = base;

    if (text_size > 0) {
        if (elfloader_fd_read(fd, text_offset, addr_sec2cp, text_size) != ELFLOADER_ERR_NONE) {
            err = ELFLOADER_ERR_FD_READ_FAILED;
            goto OUT;
        }
        text.address = addr_sec2cp;

        addr_sec2cp = (void *)((uint32_t)addr_sec2cp + text_size);
    }

    if (rodata_size > 0) {
        if (elfloader_fd_read(fd, rodata_offset, addr_sec2cp, rodata_size) != ELFLOADER_ERR_NONE) {
            err = ELFLOADER_ERR_FD_READ_FAILED;
            goto OUT;
        }
        rodata.address = addr_sec2cp;

        addr_sec2cp = (void *)((uint32_t)addr_sec2cp + rodata_size);
    }

    if (data_size > 0) {
        if (elfloader_fd_read(fd, data_offset, addr_sec2cp, data_size) != ELFLOADER_ERR_NONE) {
            err = ELFLOADER_ERR_FD_READ_FAILED;
            goto OUT;
        }
        data.address = addr_sec2cp;

        addr_sec2cp = (void *)((uint32_t)addr_sec2cp + data_size);
    }

    /* clear bss */
    if (bss_size > 0) {
        bss.address = addr_sec2cp;
        memset(bss.address, 0, bss_size);
    }

    /* do relocation */
    if (rel_data_size > 0) {
        err = elfloader_relocate(fd, data.address,
                                   bss, data, rodata, text,
                                   rel_data_offset, rel_data_size, rel_data_entsize,
                                   symtab_offset, symtab_size, symtab_entsize,
                                   strtab_offset, strtab_size);
        if (err != ELFLOADER_ERR_NONE) {
            goto OUT;
        }
    }

    if (rela_data_size > 0) {
        err = elfloader_relocate(fd, data.address,
                                   bss, data, rodata, text,
                                   rela_data_offset, rela_data_size, rela_data_entsize,
                                   symtab_offset, symtab_size, symtab_entsize,
                                   strtab_offset, strtab_size);
        if (err != ELFLOADER_ERR_NONE) {
            goto OUT;
        }
    }

    if (rel_rodata_size > 0) {
        err = elfloader_relocate(fd, rodata.address,
                                   bss, data, rodata, text,
                                   rel_rodata_offset, rel_rodata_size, rel_rodata_entsize,
                                   symtab_offset, symtab_size, symtab_entsize,
                                   strtab_offset, strtab_size);
        if (err != ELFLOADER_ERR_NONE) {
            goto OUT;
        }
    }

    if (rela_rodata_size > 0) {
        err = elfloader_relocate(fd, rodata.address,
                                   bss, data, rodata, text,
                                   rela_rodata_offset, rela_rodata_size, rela_rodata_entsize,
                                   symtab_offset, symtab_size, symtab_entsize,
                                   strtab_offset, strtab_size);
        if (err != ELFLOADER_ERR_NONE) {
            goto OUT;
        }
    }

    if (rel_text_size > 0) {
        err = elfloader_relocate(fd, text.address,
                                   bss, data, rodata, text,
                                   rel_text_offset, rel_text_size, rel_text_entsize,
                                   symtab_offset, symtab_size, symtab_entsize,
                                   strtab_offset, strtab_size);
        if (err != ELFLOADER_ERR_NONE) {
            goto OUT;
        }
    }

    if (rela_text_size > 0) {
        err = elfloader_relocate(fd, text.address,
                                   bss, data, rodata, text,
                                   rela_text_offset, rela_text_size, rela_text_entsize,
                                   symtab_offset, symtab_size, symtab_entsize,
                                   strtab_offset, strtab_size);
        if (err != ELFLOADER_ERR_NONE) {
            goto OUT;
        }
    }

    /* TODO: should do icache/dcahe flush here, sth. like:

    dcache_flush();
    icache_flush();

     */

    module->fd                  = fd;
    module->base                = base;

    module->info.obj.bss        = bss;
    module->info.obj.data       = data;
    module->info.obj.rodata     = rodata;
    module->info.obj.text       = text;

    module->symtab_offset       = symtab_offset;
    module->symtab_size         = symtab_size;
    module->symtab_entsize      = symtab_entsize;
    module->strtab_offset       = strtab_offset;

    return ELFLOADER_ERR_NONE;

OUT:
    if (base) {
        tos_mmheap_free(base);
    }

    return err;
}

__API__ el_err_t tos_elfloader_unload(el_module_t *module)
{
    if (!module || !module->base) {
        return ELFLOADER_ERR_PTR_NULL;
    }

    tos_mmheap_free(module->base);
    module->base = K_NULL;

    return ELFLOADER_ERR_NONE;
}

__API__ void *tos_elfloader_find_symbol(el_module_t *module, char *symbol)
{
    return local_symtab_lookup(module->fd, symbol,
                                  module->info.obj.bss, module->info.obj.data, module->info.obj.rodata, module->info.obj.text,
                                  module->symtab_offset, module->symtab_size, module->symtab_entsize,
                                  module->strtab_offset);
}

