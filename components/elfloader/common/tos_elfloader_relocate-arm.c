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

/* values for ELF32_R_TYPE(info) */
#define R_ARM_ABS32             2
#define R_ARM_THM_CALL          10
#define R_ARM_GLOB_DAT          21
#define R_ARM_JUMP_SLOT         22
#define R_ARM_RELATIVE          23

/*
 - S (when used on its own) is the address of the symbol.
 - A is the addend for the relocation.
 - P is the address of the place being relocated (derived from r_offset).
 - Pa is the adjusted address of the place being relocated, defined as (P & 0xFFFFFFFC).
 - T is 1 if the target symbol S has type STT_FUNC and the symbol addresses a Thumb instruction; it is 0
otherwise.
 - B(S) is the addressing origin of the output segment defining the symbol S. The origin is not required to be the
base address of the segment. This value must always be word-aligned.
 - GOT_ORG is the addressing origin of the Global Offset Table (the indirection table for imported data
addresses). This value must always be word-aligned. See §4.6.1.8, Proxy generating relocations.
 - GOT(S) is the address of the GOT entry for the symbol S.
*/

// TODO: support more relocation type
__KNL__ void elfloader_arch_relocate(uint32_t reloc_addr, int32_t load_bias, uint32_t addr, elf32_rela_t *rela, int is_rela)
{
    /* ATTENTION:
        different reloc_addr calculation algorithm for relocatable object and shared object
     */

    switch (ELF32_R_TYPE(rela->r_info)) {
        case R_ARM_GLOB_DAT:
        case R_ARM_JUMP_SLOT:
            /* (S + A) | T */
            *(uint32_t *)reloc_addr = addr;
            break;

        case R_ARM_RELATIVE:
            /* B(S) + A */
            *(uint32_t *)reloc_addr += load_bias;
            break;

        case R_ARM_ABS32:
            /* (S + A) | T */
            *(uint32_t *)reloc_addr += addr;
            break;

        default:
            printf("Unsupported Relocation Type: %d\n", ELF32_R_TYPE(rela->r_info));
            break;
    }
}

