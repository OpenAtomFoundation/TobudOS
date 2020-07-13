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

#include "ota_patch.h"
#include "ota_flash.h"
#include "ota_partition.h"
#include "lzma_uncompress.h"

static int32_t offtin_u32(uint8_t *buf)
{
    int32_t y;

    y = buf[3] & 0x7F;
    y = y * 256; y += buf[2];
    y = y * 256; y += buf[1];
    y = y * 256; y += buf[0];

    if (buf[3] & 0x80) {
        y = -y;
    }

    return y;
}

static uint16_t offtin_u16(uint8_t *buf)
{
    int32_t y;

    y = buf[1];
    y = y * 256; y += buf[0];

    return y;
}

static int backup_map_create(bkup_map_t *bkup_map, size_t ota_size, uint16_t blk_len)
{
    uint16_t i = 0;
    uint16_t *cache;
    uint16_t remain_blk_cnt;
    uint32_t start;
    size_t remain_size;

    if (ota_partition_size(OTA_PARTITION_OTA) <= ALIGN_UP(ota_size, blk_len)) {
        return -1;
    }

    remain_size     = ota_partition_size(OTA_PARTITION_OTA) - ALIGN_UP(ota_size, blk_len);
    remain_blk_cnt  = BLK_CNT(remain_size, blk_len);
    start           = ota_partition_start(OTA_PARTITION_OTA) + ALIGN_UP(ota_size, blk_len);

    cache = malloc(remain_blk_cnt * sizeof(uint16_t));
    if (cache == NULL) {
        return -1;
    }

    for (i = 0; i < remain_blk_cnt; ++i) {
        cache[i] = (uint16_t)-1;
    }

    bkup_map->cache         = cache;
    bkup_map->backup_n      = remain_blk_cnt;
    bkup_map->blk_len       = blk_len;
    bkup_map->start         = start;
    bkup_map->cursor        =0;

    return 0;
}

static int backup_map_destroy(bkup_map_t *bkup_map)
{
    free(bkup_map->cache);
    return 0;
}

static int backup_map_add(bkup_map_t *bkup_map, uint16_t blk_idx, uint32_t backup_ed_addr, uint8_t *blk_buf)
{
    uint16_t slot = bkup_map->cursor;
    uint16_t blk_len = bkup_map->blk_len;
    uint32_t backup_ee_addr = THE_BUF(bkup_map->start, slot, bkup_map->blk_len);

    if (ota_flash_erase(backup_ee_addr, blk_len) != 0) {
        return -1;
    }

    if (ota_flash_read(backup_ed_addr, blk_buf, blk_len) != 0) {
        return -1;
    }

    if (ota_flash_write(backup_ee_addr, blk_buf, blk_len) != 0) {
        return -1;
    }

    bkup_map->cache[slot] = blk_idx;
    bkup_map->cursor = (slot + 1) % bkup_map->backup_n;

    return 0;
}

static query_res_t backup_map_query(bkup_map_t *bkup_map, uint16_t blk_idx, uint8_t *blk_buf)
{
    uint16_t i = 0;
    uint32_t backup_ee_addr;
    uint16_t blk_len = bkup_map->blk_len;

    for (i = 0; i < bkup_map->backup_n; ++i) {
        if (bkup_map->cache[i] == blk_idx) {
            backup_ee_addr = THE_BUF(bkup_map->start, i, blk_len);

            if (ota_flash_read(backup_ee_addr, blk_buf, blk_len) != 0) {
                return QUERY_RESULT_FETCH_FAILED;
            }

            return QUERY_RESULT_OK;
        }
    }

    return QUERY_RESULT_NOT_EXIST;
}

int ota_patch(uint32_t patch, size_t patchsize, size_t ota_size)
{
    int rc = 0, i = 0;

    /* the patch information stuff */
    uint8_t *block = NULL, *blk_buf = NULL;
    uint8_t *unzipped_patch = NULL, *zipped_patch = NULL;
    uint16_t blk_len, blk_cnt;
    uint16_t unzipped_len, zipped_len;
    size_t the_unzipped_len, the_zipped_len;

    /* the backup stuff */
    bkup_map_t bkup_map;
    query_res_t res;

    /* the patch4blk stuff */
    uint8_t X;
    uint32_t Z;
    uint16_t I, N, Y, C, D;
    uint8_t *ctrl_blk, *diff_blk, *extra_blk;
    uint16_t cursor = 0;
    uint16_t old_idx, old_offset;

    patch_hdr_t patch_hdr;
    patch4blk_hdr_t patch4blk_hdr;
    patch4blk_info_t *patch4blk_info;

    /*
      format of patch:
      patch_hdr + (patch4blk_hdr + zipped(patch4blk)) * N

      patch_hdr:
        0   2   length of one block
        4   2   total count of blocks in the patch

      patch4blk_hdr:
        4   2   original length of the patch
        8   2   zipped length of the patch
     */
    /* read the patch header */
    if (ota_flash_read(patch, &patch_hdr, sizeof(patch_hdr_t)) != 0) {
        ERROR();
    }
    patch              += sizeof(patch_hdr_t);
    patchsize          -= sizeof(patch_hdr_t);
    if (patchsize <= 0) {
        ERROR();
    }

    blk_len             = patch_hdr.blk_len;
    blk_cnt             = patch_hdr.blk_cnt;

    /* create backup map to cache old blocks */
    if (!ota_partition_is_pingpong() &&
        backup_map_create(&bkup_map, ota_size, blk_len) != 0) {
        ERROR();
    }

    block = malloc(blk_len);
    if (!block) {
        ERROR();
    }

    blk_buf = malloc(blk_len);
    if (!blk_buf) {
        ERROR();
    }

    while (blk_cnt--) {
        /* read the patch4blk header */
        if (ota_flash_read(patch, &patch4blk_hdr, sizeof(patch4blk_hdr_t)) != 0) {
            ERROR();
        }
        patch              += sizeof(patch4blk_hdr_t);
        patchsize          -= sizeof(patch4blk_hdr_t);
        if (patchsize <= 0) {
            ERROR();
        }

        unzipped_len        = patch4blk_hdr.unzipped_len;
        zipped_len          = patch4blk_hdr.zipped_len;
        the_unzipped_len    = unzipped_len;
        the_zipped_len      = zipped_len;

        /* some malloc */
        zipped_patch = malloc(the_zipped_len);
        if (!zipped_patch) {
            ERROR();
        }

        unzipped_patch = malloc(the_unzipped_len);
        if (!unzipped_patch) {
            ERROR();
        }

        /* read the zipped patch4blk */
        if (ota_flash_read(patch, zipped_patch, the_zipped_len) != 0) {
            ERROR();
        }
        patch              += the_zipped_len;
        patchsize          -= the_zipped_len;
        if (patchsize <= 0) {
            ERROR();
        }

        /* do unzip */
        if (lzma_uncompress(unzipped_patch, &the_unzipped_len, zipped_patch, &the_zipped_len) != LZMA_ERR_OK) {
            ERROR();
        }

        free(zipped_patch);

        /*
         format of patch for one block:
             0          2   I block index of the patch
             2          2   N count of cmd
             4          2   C sizeof(control block)
             6          2   D sizeof(diff block)

             6          C   control block
             6 + C      D   diff block
             6 + C + D  ?   extra block
         */
        /* control block:
            with a leading 8bit domain, indicating the cmd type, 0 for DIFF, 1 for EXTRA

            if current is a CMD_COPY(X, Y, Z) cmd:
                X is 0(COPY)
                locate old to Z, copy Y bytes from oldfile

            if current is a CMD_DIFF(X, Y, Z) cmd:
                X is 1(DIFF)
                locate old to Z, add Y bytes from oldfile to Y bytes from the diff block

            if current is a CMD_EXTRA(X, Y) cmd:
                X is 2(EXTRA)
                copy Y bytes from the extra block
         */
        /* here the show begins */

        /* read the patch4info */
        patch4blk_info = (patch4blk_info_t *)unzipped_patch;

        I               = patch4blk_info->blk_idx;
        N               = patch4blk_info->cmd_cnt;
        C               = patch4blk_info->ctrl_size;
        D               = patch4blk_info->diff_size;

        ctrl_blk        = unzipped_patch + sizeof(patch4blk_info_t);
        diff_blk        = unzipped_patch + sizeof(patch4blk_info_t) + C;
        extra_blk       = unzipped_patch + sizeof(patch4blk_info_t) + C + D;

        cursor = 0;
        memset(block, 0, blk_len);

        /* backup the old block I first */
        if (!ota_partition_is_pingpong() &&
            backup_map_add(&bkup_map, I,
                            THE_BUF(ota_partition_start(OTA_PARTITION_ACTIVE_APP), I, blk_len),
                            blk_buf) != 0) {
            ERROR();
        }

        while (N--) {
            X               = *(uint8_t *)ctrl_blk;
            ctrl_blk       += 1;

            if (X == CMD_COPY || X == CMD_DIFF) { // a COPY or DIFF cmd
                Y           = offtin_u16(ctrl_blk);
                ctrl_blk   += sizeof(uint16_t);

                Z           = offtin_u32(ctrl_blk);
                ctrl_blk   += sizeof(uint32_t);

                old_idx     = BLK_IDX(Z, blk_len);
                old_offset  = Z - BLK_LOWER_BOUND(old_idx, blk_len);

                /* sanity check */
                if (BLK_IDX(cursor + Y - 1, blk_len) != BLK_IDX(cursor, blk_len)) {
                    ERROR();
                }

                if (ota_partition_is_pingpong()) {
                    // if pingpong, copy from OTA_PARTITION_BACKUP_APP
                    if (ota_flash_read(THE_BUF(ota_partition_start(OTA_PARTITION_BACKUP_APP), old_idx, blk_len),
                                            blk_buf, blk_len) != 0) {
                        ERROR();
                    }
                } else {
                    res = backup_map_query(&bkup_map, old_idx, blk_buf);
                    if (res == QUERY_RESULT_FETCH_FAILED) {
                        ERROR();
                    }

                    if (res == QUERY_RESULT_NOT_EXIST) {
                        if (ota_flash_read(THE_BUF(ota_partition_start(OTA_PARTITION_ACTIVE_APP), old_idx, blk_len),
                                                blk_buf, blk_len) != 0) {
                            ERROR();
                        }
                    }
                }

                memcpy(block + cursor, blk_buf + old_offset, Y);

                if (X == CMD_DIFF) {
                    for (i = 0; i < Y; ++i) {
                        (block + cursor)[i] += *(int8_t *)(diff_blk + i);
                    }
                    diff_blk += Y;
                }
            } else if (X == CMD_EXTRA) {
                Y           = offtin_u16(ctrl_blk);
                ctrl_blk   += sizeof(uint16_t);

                /* sanity check */
                if (BLK_IDX(cursor + Y - 1, blk_len) != BLK_IDX(cursor, blk_len)) {
                    ERROR();
                }

                memcpy(block + cursor, extra_blk, Y);
                extra_blk  += Y;
            } else {
                ERROR();
            }

            cursor += Y;
            /* sanity check */
            if (cursor > blk_len) {
                ERROR();
            }
        }

        free(unzipped_patch);

        if (ota_flash_erase(THE_BUF(ota_partition_start(OTA_PARTITION_ACTIVE_APP), I, blk_len), blk_len) != 0) {
            ERROR();
        }

        if (ota_flash_write(THE_BUF(ota_partition_start(OTA_PARTITION_ACTIVE_APP), I, blk_len), block, blk_len) != 0) {
            ERROR();
        }
    }

OUT:
    FREE(block);
    FREE(blk_buf);

    if (!ota_partition_is_pingpong()) {
        backup_map_destroy(&bkup_map);
    }

    return rc;
}

