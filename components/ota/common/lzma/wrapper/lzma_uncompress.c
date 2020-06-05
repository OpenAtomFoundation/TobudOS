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

#include "lzma_uncompress.h"

#include "Alloc.h"
#include "LzmaDec.h"
#include "LzmaEnc.h"

#include "stdint.h"
#include "stdlib.h"

#pragma pack(push, 1)
typedef struct lzma_props_st {
    uint8_t         data[LZMA_PROPS_SIZE];
} lzma_props_t;
#pragma pack(pop)

static void *lzma_wrapper_alloc(ISzAllocPtr p, size_t size)
{
    (void)p;
    if (size == 0) {
        return 0;
    }
    return malloc(size);
}

static void lzma_wrapper_free(ISzAllocPtr p, void *address)
{
    (void)p;
    free(address);
}

static ISzAlloc wrapper_alloc = { lzma_wrapper_alloc, lzma_wrapper_free };

static lzma_err_t lzma_do_uncompress(uint8_t *dst, size_t *dst_len, const uint8_t *src, size_t *src_len, lzma_props_t *props)
{
    ELzmaStatus status;

    return (lzma_err_t)LzmaDecode(dst, dst_len,
                                    src, src_len,
                                    props->data, LZMA_PROPS_SIZE,
                                    LZMA_FINISH_ANY, &status, &wrapper_alloc);
}

lzma_err_t lzma_uncompress(uint8_t *dst, size_t *dst_len, const uint8_t *src, size_t *src_len)
{
    lzma_props_t *props;

    if (*src_len <= sizeof(lzma_props_t)) {
        return LZMA_ERR_MEM;
    }

    props       = (lzma_props_t *)src;
    *src_len    = *src_len - sizeof(lzma_props_t);

    return lzma_do_uncompress(dst, dst_len,
                                src + sizeof(lzma_props_t), src_len,
                                props);
}

