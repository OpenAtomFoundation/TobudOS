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

#ifndef _TOS_ELFLOADER_H_
#define  _TOS_ELFLOADER_H_

#include "tos_k.h"

#include "elf/elf32.h"

#include "tos_elfloader_err.h"
#include "tos_elfloader_fd_read.h"
#include "tos_elfloader_symbol.h"
#include "tos_elfloader_symtab.h"
#include "tos_elfloader_relocate.h"

typedef struct el_section_st {
    int8_t          shndx;
    void           *address;
} el_section_t;

typedef struct el_obj_info_st {
    el_section_t    bss;
    el_section_t    data;
    el_section_t    rodata;
    el_section_t    text;
} el_obj_info_t;

typedef struct el_so_info_st {
    int32_t         load_bias;
} el_so_info_t;

typedef union el_info_un {
    el_obj_info_t   obj;
    el_so_info_t    so;
} el_info_t;

typedef struct el_module_st {
    int         fd;

    void       *base;

    el_info_t   info;

    uint32_t    symtab_offset;
    uint32_t    symtab_size;
    uint32_t    symtab_entsize;

    uint32_t    strtab_offset;
} el_module_t;

__API__ el_err_t tos_elfloader_load(el_module_t *module, int fd);

__API__ el_err_t tos_elfloader_unload(el_module_t *module);

__API__ void *tos_elfloader_find_symbol(el_module_t *module, char *symbol);


#endif /* _TOS_ELFLOADER_H_ */

