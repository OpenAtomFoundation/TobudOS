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
#define R_ARM_NONE              0
#define R_ARM_PC24              1
#define R_ARM_ABS32             2
#define R_ARM_THM_CALL          10
#define R_ARM_GLOB_DAT          21
#define R_ARM_JUMP_SLOT         22
#define R_ARM_RELATIVE          23
#define R_ARM_CALL              28
#define R_ARM_JUMP24            29
#define R_ARM_THM_JUMP24        30
#define R_ARM_TARGET1           38
#define R_ARM_V4BX              40
#define R_ARM_PREL31            42
#define R_ARM_MOVW_ABS_NC       43
#define R_ARM_MOVT_ABS          44
#define R_ARM_THM_MOVW_ABS_NC   47
#define R_ARM_THM_MOVT_ABS      48

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

__KNL__ el_err_t elfloader_arch_relocate(uint32_t reloc_addr, int32_t load_bias, uint32_t addr, elf32_rela_t *rela, int is_rela, elf32_sym_t *sym)
{
    /* ATTENTION:
        different reloc_addr calculation algorithm for relocatable object and shared object
     */
    int32_t offset;
    uint32_t tmp;
    uint32_t upper, lower, sign, j1, j2;

    switch (ELF32_R_TYPE(rela->r_info)) {
        case R_ARM_NONE:
            /* ignore */
            break;

        case R_ARM_ABS32:
        case R_ARM_TARGET1:
            /* (S + A) | T */
            *(uint32_t *)reloc_addr += addr;
            break;

        case R_ARM_PC24:
        case R_ARM_CALL:
        case R_ARM_JUMP24:
            /* ((S + A) | T) – P */
            if (addr & 3) {
                tos_kprintf("unsupported interworking call(ARM -> Thumb)\n");
                return ELFLOADER_ERR_RELOCATE_FAILED;
            }

            offset = *(uint32_t *)reloc_addr;
            offset = (offset & 0x00ffffff) << 2;
            if (offset & 0x02000000) {
                offset -= 0x04000000;
            }

            offset += addr - reloc_addr;

            if (offset <= (int32_t)0xfe000000 || offset >= (int32_t)0x02000000) {
                tos_kprintf("relocation out of range\n");
                return ELFLOADER_ERR_RELOCATE_FAILED;
            }

            offset >>= 2;
            offset &= 0x00ffffff;

            *(uint32_t *)reloc_addr &= 0xff000000;
            *(uint32_t *)reloc_addr |= offset;
            break;

        case R_ARM_V4BX:
            *(uint32_t *)reloc_addr &= 0xf000000f;
            *(uint32_t *)reloc_addr |= 0x01a0f000;
            break;

        case R_ARM_PREL31:
            /* ((S + A) | T) – P */
            offset = (*(int32_t *)reloc_addr << 1) >> 1; /* sign extend */
            offset += addr - reloc_addr;

            if (offset >= 0x40000000 || offset < -0x40000000) {
                tos_kprintf("relocation out of range\n");
                return ELFLOADER_ERR_RELOCATE_FAILED;
            }

            *(uint32_t *)reloc_addr &= 0x80000000;
            *(uint32_t *)reloc_addr |= offset & 0x7fffffff;
            break;

        case R_ARM_GLOB_DAT:
        case R_ARM_JUMP_SLOT:
            /* (S + A) | T */
            *(uint32_t *)reloc_addr = addr;
            break;

        case R_ARM_MOVW_ABS_NC:
        case R_ARM_MOVT_ABS:
            offset = tmp = *(uint32_t *)reloc_addr;
            offset = ((offset & 0xf0000) >> 4) | (offset & 0xfff);
            offset = (offset ^ 0x8000) - 0x8000;

            offset += addr;
            if (ELF32_R_TYPE(rela->r_info) == R_ARM_MOVT_ABS) {
                offset >>= 16;
            }

            tmp &= 0xfff0f000;
            tmp |= ((offset & 0xf000) << 4) | (offset & 0x0fff);

            *(uint32_t *)reloc_addr = tmp;
            break;

        case R_ARM_THM_CALL:
        case R_ARM_THM_JUMP24:
            if (ELF32_SYM_TYPE(sym->st_info) == STT_FUNC && !(addr & 1)) {
                tos_kprintf("unsupported interworking call(Thumb -> ARM)\n");
                return ELFLOADER_ERR_RELOCATE_FAILED;
            }

            upper = *(uint16_t *)reloc_addr;
            lower = *(uint16_t *)(reloc_addr + 2);

            sign = (upper >> 10) & 1;
            j1 = (lower >> 13) & 1;
            j2 = (lower >> 11) & 1;
            offset = (sign << 24) | ((~(j1 ^ sign) & 1) << 23) |
                     ((~(j2 ^ sign) & 1) << 22) |
                     ((upper & 0x03ff) << 12) |
                     ((lower & 0x07ff) << 1);
            if (offset & 0x01000000) {
                offset -= 0x02000000;
            }
            offset += addr - reloc_addr;

            if (offset <= (int32_t)0xff000000 || offset >= (int32_t)0x01000000) {
                tos_kprintf("relocation out of range\n");
                return ELFLOADER_ERR_RELOCATE_FAILED;
            }

            sign = (offset >> 24) & 1;
            j1 = sign ^ (~(offset >> 23) & 1);
            j2 = sign ^ (~(offset >> 22) & 1);
            upper = (uint16_t)((upper & 0xf800) | (sign << 10) |
                    ((offset >> 12) & 0x03ff));
            lower = (uint16_t)((lower & 0xd000) |
                    (j1 << 13) | (j2 << 11) |
                    ((offset >> 1) & 0x07ff));

            *(uint16_t *)reloc_addr = (uint16_t)upper;
            *(uint16_t *)(reloc_addr + 2) = (uint16_t)lower;
            break;

        case R_ARM_THM_MOVW_ABS_NC:
        case R_ARM_THM_MOVT_ABS:
            upper = *(uint16_t *)reloc_addr;
            lower = *(uint16_t *)(reloc_addr + 2);

            offset = ((upper & 0x000f) << 12) |
                    ((upper & 0x0400) << 1) |
                    ((lower & 0x7000) >> 4) | (lower & 0x00ff);
            offset = (offset ^ 0x8000) - 0x8000;
            offset += addr;

            if (ELF32_R_TYPE(rela->r_info) == R_ARM_THM_MOVT_ABS) {
                offset >>= 16;
            }

            upper = (uint16_t)((upper & 0xfbf0) |
                    ((offset & 0xf000) >> 12) |
                    ((offset & 0x0800) >> 1));
            lower = (uint16_t)((lower & 0x8f00) |
                    ((offset & 0x0700) << 4) |
                    (offset & 0x00ff));
            *(uint16_t *)reloc_addr = (uint16_t)upper;
            *(uint16_t *)(reloc_addr + 2) = (uint16_t)lower;
            break;

        case R_ARM_RELATIVE:
            /* B(S) + A */
            *(uint32_t *)reloc_addr += load_bias;
            break;

        default:
            tos_kprintf("unsupported relocation type: %d\n", ELF32_R_TYPE(rela->r_info));
            break;
    }

    return ELFLOADER_ERR_NONE;
}

