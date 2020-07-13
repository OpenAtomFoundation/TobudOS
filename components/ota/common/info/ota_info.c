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

#include "tos_kv.h"
#include "ota_info.h"
#include "ota_partition.h"

ota_img_vs_t ota_info_curr_version(void)
{
    size_t version_len;
    ota_img_vs_t version;

    if (tos_kv_get("cur_version", &version, sizeof(version), &version_len) != KV_ERR_NONE) {
        /* no "version" key, means we have never do a update yet
           just return the initial version
         */
        return ota_partition_init_version_get();
    }

    return version;
}

ota_err_t ota_info_update(ota_img_vs_t new_version)
{
    kv_err_t err;

    err = tos_kv_del("new_version");
    if (err != KV_ERR_NONE && err != KV_ERR_NOT_EXIST) {
        return OTA_ERR_KV_DEL_FAIL;
    }

    if (tos_kv_set("cur_version", &new_version, sizeof(ota_img_vs_t)) != KV_ERR_NONE) {
        return OTA_ERR_KV_SET_FAIL;
    }

    return OTA_ERR_NONE;
}

