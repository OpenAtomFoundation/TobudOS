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

#ifndef _OTA_PATCH_H_
#define  _OTA_PATCH_H_

#include "stddef.h"
#include "stdint.h"

#define ERROR()         \
    rc = -1;            \
    goto OUT;

#define FREE(p)                         if (p) { free(p); }

#define CMD_COPY                        0

#define CMD_DIFF                        1

#define CMD_EXTRA                       2

#define BLK_UPPER_BOUND(off, blk_len)       (ALIGN_UP((off) + 1, (blk_len)))

#define BLK_LOWER_BOUND(idx, blk_len)         ((idx) * (blk_len))

#define FOLD_N(x, n)                         ((n) * (x))

#define THE_BUF(buf, idx, blk_len)          ((buf) + (idx) * (blk_len))

#define BLK_IDX(off, blk_len)                 ((off) / (blk_len))

#define ALIGN_UP(v, align)                      (((v) + ((align) - 1)) & ~((align) - 1))

#define BLK_CNT(size, blk_len)                (((size) + (blk_len) - 1) / (blk_len))

#pragma pack(push, 1)
typedef struct patch_header_st {
    uint16_t        blk_len;
    uint16_t        blk_cnt;
} patch_hdr_t;

typedef struct patch_for_block_header_st {
    uint16_t        unzipped_len;
    uint16_t        zipped_len;
} patch4blk_hdr_t;

typedef struct patch_for_block_info_st {
    uint16_t        blk_idx;
    uint16_t        cmd_cnt;
    uint16_t        ctrl_size;
    uint16_t        diff_size;
} patch4blk_info_t;
#pragma pack(pop)

typedef struct backup_map_st {
    uint16_t       *cache;
    uint16_t        backup_n;
    uint16_t        cursor;
    uint16_t        blk_len;
    uint32_t        start;
} bkup_map_t;

typedef enum query_res_en {
    QUERY_RESULT_OK,
    QUERY_RESULT_NOT_EXIST,
    QUERY_RESULT_FETCH_FAILED,
} query_res_t;

int ota_patch(uint32_t patch, size_t patchsize, size_t ota_size);

int ota_patch_xip(uint8_t *patch, size_t patchsize, size_t ota_size);

#endif

