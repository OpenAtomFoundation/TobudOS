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

#ifndef _TOS_KV_ERR_H_
#define  _TOS_KV_ERR_H_

typedef enum kv_err_en {
    KV_ERR_NONE = 0u,

    KV_ERR_BUF_TOO_SHORT    = 10u,
    KV_ERR_BLK_STATUS_ERROR,

    KV_ERR_DATA_FETCH_FAILED    = 20u,

    KV_ERR_FLASH_ERASE_FAILED   = 30u,
    KV_ERR_FLASH_READ_FAILED,
    KV_ERR_FLASH_WRITE_FAILED,

    KV_ERR_GC_NOTHING   = 40u,

    KV_ERR_INTERNAL_ERROR   = 50u,
    KV_ERR_INSUFFICIENT_SPACE,
    KV_ERR_INVALID_PARAM,
    KV_ERR_INVALID_ITEM,

    KV_ERR_NEXT_LOOP    = 60u,
    KV_ERR_NOT_EXIST,
    KV_ERR_NO_WRITEABLE_BLK,

    KV_ERR_OUT_OF_MEMORY    = 70u,

    KV_ERR_POS_FIX_FAILED   =80u,

    KV_ERR_SIZE_EXCEEDED    = 90u,
} kv_err_t;

#endif /* _TOS_KV_ERR_H_ */

