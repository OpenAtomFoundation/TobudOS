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

__STATIC__ void *local_symtab_lookup(int fd, int32_t load_bias, char *sym_name,
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
                return (void *)(sym.st_value + load_bias);
            }
        }

        symtab_offset += symtab_entsize;
    }

    return K_NULL;
}

__STATIC__ el_err_t elfloader_relocate(int fd, int32_t load_bias,
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
            addr = local_symtab_lookup(fd, load_bias, name,
                                         symtab_offset, symtab_size, symtab_entsize,
                                         strtab_offset);
        }

        /* still not found */
        if (!addr) {
            return ELFLOADER_ERR_SYM_NOT_FOUND;
        }

        if (elfloader_arch_relocate(rela.r_offset + load_bias, load_bias, (uint32_t)addr, &rela, is_rela, &sym) != ELFLOADER_ERR_NONE) {
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
    elf32_dyn_t dyn;
    elf32_phdr_t phdr;

    void *base = K_NULL, *addr_seg2cp;  /* ram base for LOAD segments */

    int32_t load_bias;

    uint32_t shdr_offset, phdr_offset;

    uint32_t vaddr_start = (uint32_t)-1, vaddr_end = 0;

    uint32_t dyn_offset, dyn_size = 0, dyn_entsize;

    uint32_t strtab_offset, strtab_size = 0;
    uint32_t symtab_offset, symtab_size = 0, symtab_entsize;

    uint32_t rel_entsize;
    uint32_t rel_dyn_offset, rel_dyn_size = 0;
    uint32_t rel_plt_offset, rel_plt_size = 0;

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

    /* it should be a shared object */
    if (ehdr.e_type != ET_DYN) {
        return ELFLOADER_ERR_TYPE_INVALID;
    }

    shdr_offset = ehdr.e_shoff;

    for (i = 0; i < ehdr.e_shnum; ++i) {
        if (elfloader_fd_read(fd, shdr_offset, &shdr, sizeof(elf32_shdr_t)) != ELFLOADER_ERR_NONE) {
            return ELFLOADER_ERR_FD_READ_FAILED;
        }

        if (shdr.sh_type == SHT_DYNAMIC) {          /* dynamic section, .dynamic */
            dyn_offset  = shdr.sh_offset;
            dyn_size    = shdr.sh_size;
            dyn_entsize = shdr.sh_entsize;
        } else if (shdr.sh_type == SHT_DYNSYM) {    /* .dynsym */
            symtab_size = shdr.sh_size;
        }

        shdr_offset += ehdr.e_shentsize;
    }

    if (dyn_size == 0) {
        return ELFLOADER_ERR_NO_DYN;
    }

    if (symtab_size == 0) {
        return ELFLOADER_ERR_NO_SYMTAB;
    }

    for (i = 0; i < dyn_size / dyn_entsize; ++i) {
        if (elfloader_fd_read(fd, dyn_offset, &dyn, sizeof(elf32_dyn_t)) != ELFLOADER_ERR_NONE) {
            return ELFLOADER_ERR_FD_READ_FAILED;
        }

        if (dyn.d_tag == DT_NULL) {
            break;
        } else if (dyn.d_tag == DT_SYMTAB) {    /* dynamic symbol table */
            symtab_offset   = dyn.d_un.d_ptr;
        } else if (dyn.d_tag == DT_SYMENT) {    /* entry size of symbol table */
            symtab_entsize  = dyn.d_un.d_val;
        } else if (dyn.d_tag == DT_STRTAB) {    /* dynamic string table */
            strtab_offset   = dyn.d_un.d_ptr;
        } else if (dyn.d_tag == DT_STRSZ) {     /* size of dynamic string table */
            strtab_size     = dyn.d_un.d_val;
        } else if (dyn.d_tag == DT_REL) {       /* dynamic relocation table */
            rel_dyn_offset  = dyn.d_un.d_ptr;
        } else if (dyn.d_tag == DT_RELSZ) {     /* size of rel.dyn */
            rel_dyn_size    = dyn.d_un.d_val;
        } else if (dyn.d_tag == DT_RELENT) {    /* entry size of rel.dyn */
            rel_entsize     = dyn.d_un.d_val;
        } else if (dyn.d_tag == DT_JMPREL) {    /* plt relocation table. why NOT DT_PLTREL, confusing */
            rel_plt_offset  = dyn.d_un.d_ptr;
        } else if (dyn.d_tag == DT_PLTRELSZ) {  /* size of rel.plt */
            rel_plt_size    = dyn.d_un.d_val;
        }

        dyn_offset += dyn_entsize;
    }

    if (strtab_size == 0) {
        return ELFLOADER_ERR_NO_STRTAB;
    }

    phdr_offset = ehdr.e_phoff;

    for (i = 0; i < ehdr.e_phnum; ++i) {
        if (elfloader_fd_read(fd, phdr_offset, &phdr, sizeof(elf32_phdr_t)) != ELFLOADER_ERR_NONE) {
            return ELFLOADER_ERR_FD_READ_FAILED;
        }

        if (phdr.p_type == PHT_LOAD) {
            if (phdr.p_vaddr < vaddr_start) {
                vaddr_start = phdr.p_vaddr;
            }

            if (phdr.p_vaddr + phdr.p_memsz > vaddr_end) {
                vaddr_end   = phdr.p_vaddr + phdr.p_memsz;
            }
        }

        phdr_offset += ehdr.e_phentsize;
    }

    if (vaddr_start == (uint32_t)-1 || vaddr_end == 0) {
        return ELFLOADER_ERR_NO_LOAD_SEGMENTS;
    }

    /* reserving memory for LOAD segments */
    base = tos_mmheap_alloc(vaddr_end - vaddr_start);
    if (!base) {
        return ELFLOADER_ERR_OUT_OF_MEMORY;
    }

    load_bias = (uint32_t)base - vaddr_start;

    /* do segments load */
    phdr_offset = ehdr.e_phoff;

    for (i = 0; i < ehdr.e_phnum; ++i) {
        if (elfloader_fd_read(fd, phdr_offset, &phdr, sizeof(elf32_phdr_t)) != ELFLOADER_ERR_NONE) {
            err = ELFLOADER_ERR_FD_READ_FAILED;
            goto OUT;
        }

        if (phdr.p_type == PHT_LOAD) {
            addr_seg2cp = (void *)(load_bias + phdr.p_vaddr);

            if (elfloader_fd_read(fd,
                                    phdr.p_offset,
                                    addr_seg2cp,
                                    phdr.p_filesz) != ELFLOADER_ERR_NONE) {
                err = ELFLOADER_ERR_FD_READ_FAILED;
                goto OUT;
            }

            if (phdr.p_memsz > phdr.p_filesz) {
                /* clear bss */
                memset((void *)((uint8_t *)addr_seg2cp + phdr.p_filesz),
                        0,
                        phdr.p_memsz - phdr.p_filesz);
            }
        }

        phdr_offset += ehdr.e_phentsize;
    }

    /* do relocation */
    if (rel_dyn_size > 0) {
        err = elfloader_relocate(fd, load_bias,
                                   rel_dyn_offset, rel_dyn_size, rel_entsize,
                                   symtab_offset, symtab_size, symtab_entsize,
                                   strtab_offset, strtab_size);
        if (err != ELFLOADER_ERR_NONE) {
            goto OUT;
        }
    }

    if (rel_plt_size > 0) {
        err = elfloader_relocate(fd, load_bias,
                                   rel_plt_offset, rel_plt_size, rel_entsize,
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
    module->info.so.load_bias   = load_bias;
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
    return local_symtab_lookup(module->fd, module->info.so.load_bias, symbol,
                                  module->symtab_offset, module->symtab_size, module->symtab_entsize,
                                  module->strtab_offset);
}

