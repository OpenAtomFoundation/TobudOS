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

#ifndef  _LZMA_ERR_H_
#define  _LZMA_ERR_H_

#include "7zTypes.h"

typedef enum lzma_err_en {
    LZMA_ERR_OK             = SZ_OK,

    LZMA_ERR_DATA           = SZ_ERROR_DATA,
    LZMA_ERR_MEM            = SZ_ERROR_MEM,
    LZMA_ERR_CRC            = SZ_ERROR_CRC,
    LZMA_ERR_UNSUPPORTED    = SZ_ERROR_UNSUPPORTED,
    LZMA_ERR_PARAM          = SZ_ERROR_PARAM,
    LZMA_ERR_INPUT_EOF      = SZ_ERROR_INPUT_EOF,
    LZMA_ERR_OUTPUT_EOF     = SZ_ERROR_OUTPUT_EOF,
    LZMA_ERR_READ           = SZ_ERROR_READ,
    LZMA_ERR_WRITE          = SZ_ERROR_WRITE,
    LZMA_ERR_PROGRESS       = SZ_ERROR_PROGRESS,
    LZMA_ERR_FAI            = SZ_ERROR_FAIL,
    LZMA_ERR_THREAD         = SZ_ERROR_THREAD,

    LZMA_ERR_ARCHIVE        = SZ_ERROR_ARCHIVE,
    LZMA_ERR_NO_ARCHIV      = SZ_ERROR_NO_ARCHIVE,

    LZMA_ERR_EXCEEDED,
} lzma_err_t;

#endif /* _LZMA_ERR_H_ */

