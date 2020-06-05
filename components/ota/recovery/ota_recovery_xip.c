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

static int patch_verify(ota_img_hdr_t *img_hdr)
{
    uint8_t crc = 0;
    size_t version_len;
    ota_img_vs_t new_version;
    ota_img_vs_t cur_version;

    /* read the ota_img_hdr */
    memcpy(img_hdr, (void *)(ota_partition_start(OTA_PARTITION_OTA)), sizeof(ota_img_hdr_t));

    /* 1. check whether new version patch downloaded */
    if (tos_kv_get("new_version", &new_version, sizeof(new_version), &version_len) != KV_ERR_NONE) {
        return -1;
    }

    if (new_version.major != img_hdr->new_version.major ||
        new_version.minor != img_hdr->new_version.minor) {
        return -1;
    }

    /* 2. verify magic */
    if (img_hdr->magic != OTA_IMAGE_MAGIC) {
        return -1;
    }

    /* 3. is this patch for current version? */
    cur_version = ota_info_curr_version();
    if (cur_version.major != img_hdr->old_version.major ||
        cur_version.minor != img_hdr->old_version.minor) {
        return -1;
    }

    /* 4. verify the patch crc checksum */
    crc = ota_img_hdr_crc(img_hdr);
    crc = crc8(crc, (uint8_t *)(ota_partition_start(OTA_PARTITION_OTA) + sizeof(ota_img_hdr_t)), img_hdr->patch_size);

    if (crc != img_hdr->patch_crc) {
        return -1;
    }

    /* 5. verify the old crc checksum */
    crc = crc8(0, (uint8_t *)ota_partition_start(OTA_PARTITION_ACTIVE_APP), img_hdr->old_size);
    if (crc != img_hdr->old_crc) {
        return -1;
    }

    return 0;
}

static int do_recovery_xip(ota_img_hdr_t *hdr)
{
    uint8_t *patch = (uint8_t *)(ota_partition_start(OTA_PARTITION_OTA) + sizeof(ota_img_hdr_t));

    return ota_patch_xip(patch, hdr->patch_size - sizeof(ota_img_hdr_t), hdr->patch_size);
}

static int app_copy(uint32_t dst, uint32_t dst_size, uint32_t src, uint32_t src_size)
{
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

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
    while (remain_len > 0) {
        read_len = MIN(remain_len, 128);
        if (ota_flash_write(w_addr, (void *)r_addr, ota_flash_write_size_aligned_get(read_len)) < 0) {
            return -1;
        }

        w_addr      += read_len;
        r_addr      += read_len;
        remain_len  -= read_len;
    }

    return 0;
}

int ota_recovery_xip(void)
{
    ota_img_hdr_t img_hdr;

    if (patch_verify(&img_hdr)) {
        return -1;
    }

    /* backup */
    if (ota_partition_is_pingpong() &&
        app_copy(ota_partition_start(OTA_PARTITION_BACKUP_APP),
                    ota_partition_size(OTA_PARTITION_BACKUP_APP),
                    ota_partition_start(OTA_PARTITION_ACTIVE_APP),
                    ota_partition_size(OTA_PARTITION_ACTIVE_APP)) < 0) {
        return -1;
    }

    if (do_recovery_xip(&img_hdr) != 0) {
        printf("recovery failed\n");

        /* restore */
        if (ota_partition_is_pingpong()) {
            app_copy(ota_partition_start(OTA_PARTITION_ACTIVE_APP),
                        ota_partition_size(OTA_PARTITION_ACTIVE_APP),
                        ota_partition_start(OTA_PARTITION_BACKUP_APP),
                        ota_partition_size(OTA_PARTITION_BACKUP_APP));
        }
        return -1;
    }

    if (ota_info_update(img_hdr.new_version) != 0) {
        return -1;
    }

    return 0;
}

