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

#include "ota_flash.h"
#include "ota_partition.h"

static ota_pt_ctrl_t ctrl;

static ota_err_t partitions_verify(ota_pt_t *pts, int n)
{
    int i = 0;

    for (i = 0; i < n; ++i) {
        if (pts[i].start >= pts[i].end) {
            return OTA_ERR_PARTITION_ADDR_FAIL;
        }

        if (!ota_flash_size_is_aligned(pts[i].end - pts[i].start)) {
            return OTA_ERR_PARTITION_NOT_ALIGN;
        }
    }

    return OTA_ERR_NONE;
}

ota_err_t ota_partition_load(ota_updt_type_t updt_type, uint32_t partition_addr)
{
    uint8_t crc = 0;
    ota_pt_hdr_t hdr;
    ota_err_t ret;

    memset(&ctrl, 0, sizeof(ota_pt_ctrl_t));

    if (ota_flash_read(partition_addr, &hdr, sizeof(ota_pt_hdr_t)) != 0) {
        return OTA_ERR_PARTITION_READ_FAIL;
    }

    if (hdr.magic != OTA_PARTITION_MAGIC) {
        return OTA_ERR_PARTITION_MAGIC_NOT_SAME;
    }

    ctrl.updt_type  = updt_type;
    ctrl.version    = hdr.version;

    crc = partition_hdr_crc(&hdr);

    if (updt_type == OTA_UPDATE_IN_POSITION) {
        if (ota_flash_read(partition_addr + sizeof(ota_pt_hdr_t), &IP_PTS, sizeof(ota_ip_pts_t)) != 0) {
            return OTA_ERR_PARTITION_READ_FAIL;
        }

        if ((ret = partitions_verify(IP_PTS_ARRAY, sizeof(IP_PTS) / sizeof(ota_pt_t))) != OTA_ERR_NONE) {
            return ret;
        }

        crc = partitions_crc(crc, IP_PTS_ARRAY, sizeof(IP_PTS) / sizeof(ota_pt_t));
    } else if (updt_type == OTA_UPDATE_PING_PONG) {
        if (ota_flash_read(partition_addr + sizeof(ota_pt_hdr_t), &PP_PTS, sizeof(ota_pp_pts_t)) != 0) {
            return OTA_ERR_PARTITION_READ_FAIL;
        }

        if (partitions_verify(PP_PTS_ARRAY, sizeof(PP_PTS) / sizeof(ota_pt_t)) != 0) {
            return ret;
        }

        crc = partitions_crc(crc, PP_PTS_ARRAY, sizeof(PP_PTS) / sizeof(ota_pt_t));
    } else {
        return OTA_ERR_UPDT_TYPE_UNKOWN;
    }

    if (crc != hdr.crc) {
        return OTA_ERR_PARTITION_CRC_FAIL;
    }

    return OTA_ERR_NONE;
}

uint32_t ota_partition_start(ota_pt_type_t pt_type)
{
    if (pt_type == OTA_PARTITION_ACTIVE_APP) {
        if (ctrl.updt_type == OTA_UPDATE_IN_POSITION) {
            return IP_PT(active_app).start;
        } else {
            return PP_PT(active_app).start;
        }
    } else if (pt_type == OTA_PARTITION_BACKUP_APP) {
        if (ctrl.updt_type == OTA_UPDATE_IN_POSITION) {
            return OTA_PARTITION_INVALID;
        } else {
            return PP_PT(backup_app).start;
        }
    } else if (pt_type == OTA_PARTITION_OTA) {
        if (ctrl.updt_type == OTA_UPDATE_IN_POSITION) {
            return IP_PT(ota).start;
        } else {
            return PP_PT(ota).start;
        }
    } else if (pt_type == OTA_PARTITION_KV) {
        if (ctrl.updt_type == OTA_UPDATE_IN_POSITION) {
            return IP_PT(kv).start;
        } else {
            return PP_PT(kv).start;
        }
    } else {
        return OTA_PARTITION_INVALID;
    }
}

uint32_t ota_partition_end(ota_pt_type_t pt_type)
{
    if (pt_type == OTA_PARTITION_ACTIVE_APP) {
        if (ctrl.updt_type == OTA_UPDATE_IN_POSITION) {
            return IP_PT(active_app).end;
        } else {
            return PP_PT(active_app).end;
        }
    } else if (pt_type == OTA_PARTITION_BACKUP_APP) {
        if (ctrl.updt_type == OTA_UPDATE_IN_POSITION) {
            return OTA_PARTITION_INVALID;
        } else {
            return PP_PT(backup_app).end;
        }
    } else if (pt_type == OTA_PARTITION_OTA) {
        if (ctrl.updt_type == OTA_UPDATE_IN_POSITION) {
            return IP_PT(ota).end;
        } else {
            return PP_PT(ota).end;
        }
    } else if (pt_type == OTA_PARTITION_KV) {
        if (ctrl.updt_type == OTA_UPDATE_IN_POSITION) {
            return IP_PT(kv).end;
        } else {
            return PP_PT(kv).end;
        }
    } else {
        return OTA_PARTITION_INVALID;
    }
}

uint32_t ota_partition_size(ota_pt_type_t pt_type)
{
    if (ctrl.updt_type == OTA_UPDATE_IN_POSITION &&
        pt_type == OTA_PARTITION_BACKUP_APP) {
        return OTA_PARTITION_INVALID;
    }

    return ota_partition_end(pt_type) - ota_partition_start(pt_type);
}

int ota_partition_is_pingpong(void)
{
    return ctrl.updt_type == OTA_UPDATE_PING_PONG;
}

ota_img_vs_t ota_partition_init_version_get(void)
{
    return ctrl.version;
}

