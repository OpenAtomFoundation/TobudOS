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

#ifndef _OTA_PARTITION_H_
#define  _OTA_PARTITION_H_

#include "crc8.h"
#include "ota_image.h"
#include "ota_err.h"

#define OTA_PARTITION_INVALID               (uint32_t)-1

#define OTA_PARTITION_MAGIC                 0x6420

#define IP_PTS          (ctrl.pts.ip.ip_pts)
#define PP_PTS          (ctrl.pts.pp.pp_pts)

#define IP_PT(t)        (IP_PTS.t)
#define PP_PT(t)        (PP_PTS.t)

#define IP_PTS_ARRAY    (&ctrl.pts.ip.pts[0])
#define PP_PTS_ARRAY    (&ctrl.pts.pp.pts[0])

#pragma pack(push, 1)
typedef struct ota_partition_header_st {
    uint16_t        magic;      /* a certain number */
    ota_img_vs_t    version;    /* the initial version of the application */
    uint8_t         crc;        /* crc of the whole partition table */
    uint8_t         reserved[3];
} ota_pt_hdr_t;
#pragma pack(pop)

/*
    if you have a in-position update partition, partition table should be:
        0   2   magic                       |
        2   2   version                     |
        4   1   crc                         | partition table header
        5   3   reserved                    |

        8   4   start(active application)   | active application partition
        12  4   end(active application)     |

        16  4   start(ota)                  | ota partition(store update firmware)
        20  4   end(ota)                    |

        24  4   start(kv)                   | kv partition
        28  4   end(kv)                     |

    if you have a ping-pong update partition, partition table should be:
        0   2   magic                       |
        2   2   version                     |
        4   1   crc                         | partition table header
        5   3   reserved                    |

        8   4   start(active application)   | active application partition
        12  4   end(active application)     |

        8   4   start(backup application)   | backup application partition
        12  4   end(backup application)     |

        16  4   start(ota)                  | ota partition(store update firmware)
        20  4   end(ota)                    |

        24  4   start(kv)                   | kv partition
        28  4   end(kv)                     |
 */
typedef struct ota_partition_st {
    uint32_t        start;          /* start address */
    uint32_t        end;            /* end address */
} ota_pt_t;

typedef enum ota_update_type_en {
    OTA_UPDATE_IN_POSITION,
    OTA_UPDATE_PING_PONG,
} ota_updt_type_t;

typedef enum ota_partition_type_en {
    OTA_PARTITION_ACTIVE_APP,
    OTA_PARTITION_BACKUP_APP,
    OTA_PARTITION_OTA,
    OTA_PARTITION_KV,
} ota_pt_type_t;

typedef struct ota_in_position_partitions_st {
    ota_pt_t    active_app;
    ota_pt_t    ota;
    ota_pt_t    kv;
} ota_ip_pts_t;

typedef struct ota_ping_pong_partitions_st {
    ota_pt_t    active_app;
    ota_pt_t    backup_app;
    ota_pt_t    ota;
    ota_pt_t    kv;
} ota_pp_pts_t;

typedef union ip_un {
    ota_pt_t        pts[sizeof(ota_ip_pts_t) / sizeof(ota_pt_t)];
    ota_ip_pts_t    ip_pts;
} ip_u;

typedef union pp_un {
    ota_pt_t        pts[sizeof(ota_pp_pts_t) / sizeof(ota_pt_t)];
    ota_pp_pts_t    pp_pts;
} pp_u;

typedef union ota_partitions_un {
    ip_u            ip;
    pp_u            pp;
} ota_pts_t;

typedef struct ota_partition_control_st {
    ota_updt_type_t updt_type;
    ota_img_vs_t    version;
    ota_pts_t       pts;
} ota_pt_ctrl_t;

ota_err_t ota_partition_load(ota_updt_type_t updt_type, uint32_t partition_addr);

uint32_t ota_partition_start(ota_pt_type_t pt_type);

uint32_t ota_partition_end(ota_pt_type_t pt_type);

uint32_t ota_partition_size(ota_pt_type_t pt_type);

int ota_partition_is_pingpong(void);

ota_img_vs_t ota_partition_init_version_get(void);

static uint8_t partitions_crc(uint8_t crc, ota_pt_t *pts, int n)
{
    return crc8(crc, (uint8_t *)pts, n * sizeof(ota_pt_t));
}

static uint8_t partition_hdr_crc(ota_pt_hdr_t *pt_hdr)
{
    uint8_t crc = 0;

    crc = crc8(crc, (uint8_t *)&pt_hdr->magic,              sizeof(uint16_t));
    crc = crc8(crc, (uint8_t *)&pt_hdr->version,            sizeof(ota_img_vs_t));
    return crc;
}

#endif /* _OTA_PARTITION_H_ */

