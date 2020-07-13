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

#include "tos_k.h"
#include "tos_kv.h"

#include "crc8.h"
#include "ota_flash.h"
#include "ota_partition.h"
#include "ota_image.h"
#include "ota_info.h"
#include "ota_env.h"
#include "lzma_uncompress.h"
#include "ota_recovery.h"

#define MIN(a, b)       ((a) < (b) ? (a) : (b))

static ota_err_t patch_verify(ota_img_hdr_t *img_hdr)
{
#define BUF_SIZE        128
    static uint8_t buf[BUF_SIZE];
    uint8_t crc = 0;
    ota_img_vs_t new_version;
    ota_img_vs_t cur_version;
    size_t version_len, remain_len, read_len;
    uint32_t active_app_start = ota_partition_start(OTA_PARTITION_ACTIVE_APP);
    uint32_t patch_start = ota_partition_start(OTA_PARTITION_OTA);

    /* drag the ota_img_hdr out of the flash */
    if (ota_flash_read(patch_start, img_hdr, sizeof(ota_img_hdr_t)) < 0) {
        return OTA_ERR_PATCH_READ_FAIL;
    }

    /* 1. check whether new version patch downloaded */
    if (tos_kv_get("new_version", &new_version, sizeof(ota_img_vs_t), &version_len) != KV_ERR_NONE) {
        return OTA_ERR_KV_GET_FAIL;
    }

    if (new_version.major != img_hdr->new_version.major ||
        new_version.minor != img_hdr->new_version.minor) {
        return OTA_ERR_NEW_VERSION_NOT_SAME;
    }

    /* 2. verify magic */
    if (img_hdr->magic != OTA_IMAGE_MAGIC) {
        return OTA_ERR_PATCH_MAGIC_NOT_SAME;
    }

    /* 3. is this patch for current version? */
    cur_version = ota_info_curr_version();
    if (cur_version.major != img_hdr->old_version.major ||
        cur_version.minor != img_hdr->old_version.minor) {
        return OTA_ERR_OLD_VERSION_NOT_SAME;
    }

    /* 4. verify the patch crc checksum */
    remain_len      = img_hdr->patch_size;
    crc             = ota_img_hdr_crc(img_hdr);
    patch_start    += sizeof(ota_img_hdr_t);

    while (remain_len > 0) {
        read_len = MIN(sizeof(buf), remain_len);
        if (ota_flash_read(patch_start, buf, read_len) < 0) {
            return OTA_ERR_PATCH_READ_FAIL;
        }

        crc = crc8(crc, buf, read_len);

        remain_len     -= read_len;
        patch_start    += read_len;
    }

    if (crc != img_hdr->patch_crc) {
        return OTA_ERR_PATCH_CRC_FAIL;
    }

    /* 5. verify the old crc checksum */
    remain_len      = img_hdr->old_size;
    crc             = 0;

    while (remain_len > 0) {
        read_len = MIN(sizeof(buf), remain_len);
        if (ota_flash_read(active_app_start, buf, read_len) < 0) {
            return OTA_ERR_ACTIVE_APP_READ_FAIL;
        }

        crc = crc8(crc, buf, read_len);

        remain_len         -= read_len;
        active_app_start   += read_len;
    }

    if (crc != img_hdr->old_crc) {
        return OTA_ERR_ACTIVE_APP_CRC_FAIL;
    }

    return OTA_ERR_NONE;
}

static int do_recovery(ota_img_hdr_t *hdr)
{
    uint32_t patch =  ota_partition_start(OTA_PARTITION_OTA) + sizeof(ota_img_hdr_t);

    return ota_patch(patch, hdr->patch_size - sizeof(ota_img_hdr_t), hdr->patch_size);
}

static int app_copy(uint32_t dst, uint32_t dst_size, uint32_t src, uint32_t src_size)
{
#define BUF_SIZE        128
    static uint8_t buf[BUF_SIZE];

    size_t read_len = 0;
    size_t remain_len = src_size;
    uint32_t r_addr = src, w_addr = dst;

    if (src_size != dst_size) {
        return -1;
    }

    /* make flash ready */
    if (ota_flash_erase_blocks(dst, dst_size) < 0) {
        return -1;
    }

    while (remain_len > 0) {
        read_len = MIN(sizeof(buf), remain_len);
        if (ota_flash_read(r_addr, buf, read_len) < 0) {
            return -1;
        }

        if (ota_flash_write(w_addr, (void *)buf, ota_flash_write_size_aligned_get(read_len)) < 0) {
            return -1;
        }

        w_addr      += read_len;
        r_addr      += read_len;
        remain_len  -= read_len;
    }

    return 0;
}

ota_err_t ota_recovery(void)
{
    ota_err_t ret;
    ota_img_hdr_t img_hdr;

    if ((ret = patch_verify(&img_hdr)) != OTA_ERR_NONE) {
        return ret;
    }

    /* backup */
    if (ota_partition_is_pingpong() &&
        app_copy(ota_partition_start(OTA_PARTITION_BACKUP_APP),
                    ota_partition_size(OTA_PARTITION_BACKUP_APP),
                    ota_partition_start(OTA_PARTITION_ACTIVE_APP),
                    ota_partition_size(OTA_PARTITION_ACTIVE_APP)) < 0) {
        return OTA_ERR_BACK_UP_FAIL;
    }

    if (do_recovery(&img_hdr) != 0) {

        /* restore */
        if (ota_partition_is_pingpong()) {
            app_copy(ota_partition_start(OTA_PARTITION_ACTIVE_APP),
                        ota_partition_size(OTA_PARTITION_ACTIVE_APP),
                        ota_partition_start(OTA_PARTITION_BACKUP_APP),
                        ota_partition_size(OTA_PARTITION_BACKUP_APP));
        }
        return OTA_ERR_RECOVERRY_FAIL;
    }

    if ((ret = ota_info_update(img_hdr.new_version)) != OTA_ERR_NONE) {
        return ret;
    }

    return OTA_ERR_NONE;
}

