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

#ifndef _OTA_DIFF_H_
#define  _OTA_DIFF_H_

#include "list.h"

#define ERROR(msg)                                                                          \
    printf("ERROR: line number[%d], function[%s] msg[%s]\n", __LINE__, __FUNCTION__, msg);  \
    rc = -1;                                                                                \
    goto OUT;

#ifndef INT_MAX
#define INT_MAX                         ((~(unsigned int)0) >> 1)
#endif

#define BLK_CNT(size, blk_len)          (((size) + (blk_len) - 1) / (blk_len))

#define BLK_EXTRA(size, blk_len)        ((size) % (blk_len))

#define THE_BUF(buf, idx, blk_len)      ((buf) + (idx) * (blk_len))

#define BLK_IDX(off, blk_len)           ((off) / (blk_len))

#define ALIGN_UP(v, align)               (((v) + ((align) - 1)) & ~((align) - 1))

#define BLK_UPPER_BOUND(off, blk_len)     (ALIGN_UP((off) + 1, (blk_len)))

#define BLK_LOWER_BOUND(idx, blk_len)      ((idx) * (blk_len))

#define FOLD_N(x, n)                        ((n) * (x))

#define MIN(x, y)                           ((x) < (y) ? (x) : (y))

#define MIN_TRIPLE(x, y, z)                 (MIN(x, MIN(y, z)))

#define FREE(p)                             if (p) { free(p); }

#define CMD_COPY                        0

#define CMD_DIFF                        1

#define CMD_EXTRA                       2

typedef struct backup_map_t {
    uint16_t       *cache;
    uint16_t        backup_n;
    uint16_t        cursor;
    uint16_t        blk_len;
    uint8_t        *buf;
} bkup_map_t;

typedef struct relied_detail_st {
    list_t      list;

    int32_t     new_blk_idx;

    int32_t     new_from;
    int32_t     old_from;
    int32_t     diff_from;

    int32_t     step;
} relied_detail_t;

typedef struct simplified_relied_detail_st {
    list_t      list;

    int32_t     new_blk_idx;
} smpl_relied_detail_t;

typedef struct old_block_info_st {
    list_t      relied_list;
    int         relied_cnt;

    list_t      simplified_relied_list;
    int         simplified_relied_cnt;

    int         is_safe;
} old_blk_info_t;

typedef enum cmd_type_en {
    CMD_TYPE_DIFF,
    CMD_TYPE_EXTRA,
} cmd_type_t;

typedef struct diff_detail_st {
    int32_t     old_from;
    int32_t     diff_from;
} diff_detail_t;

typedef struct extra_detail_st {
    int32_t     extra_from;
} extra_detail_t;

typedef union cmd_detail_un {
    diff_detail_t   diff_detail;
    extra_detail_t  extra_detail;
} cmd_detail_t;

typedef struct cmd_st {
    list_t          list;

    int32_t         new_from;
    int32_t         step;

    cmd_type_t      type;

    cmd_detail_t    detail;
} cmd_t;

typedef struct new_block_info_st {
    list_t      cmd_list;
    int         cmd_cnt;
} new_blk_info_t;

typedef struct summary_st {
    int         ring_cnt;
    int         ring_size_max;
    int         ring_size_min;

    int         patch_size;

    int         block_len;
    int         block_cnt;

    int         zipped_patch4block_size_max;
    int         zipped_patch4block_size_min;

    int         patch4block_size_max;
    int         patch4block_size_min;

    int         safe_block_cnt;
    int         is_safe_block_ignored;
} summary_t;

int ota_diff(uint8_t *old, size_t oldsize, uint8_t *new, size_t newsize, size_t blk_len, int is_safe_ignored, int is_verbose, uint8_t **patch, size_t *patchsize);

#endif /* _OTA_DIFF_H_ */

