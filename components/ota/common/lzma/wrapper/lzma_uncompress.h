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

#ifndef  _LZMA_UNCOMPRESS_H_
#define  _LZMA_UNCOMPRESS_H_

#include "stdint.h"

#include "lzma_err.h"

lzma_err_t lzma_uncompress(uint8_t *dst, size_t *dst_len, const uint8_t *src, size_t *src_len);

#endif /* _LZMA_UNCOMPRESS_H_ */

