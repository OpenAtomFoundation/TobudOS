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

__API__ k_err_t tos_mmblk_pool_create(k_mmblk_pool_t *mbp, void *pool_start, size_t blk_num, size_t blk_size)
{
    uint32_t    i;
    void       *blk_curr, *blk_next;

    TOS_IN_IRQ_CHECK();
    TOS_PTR_SANITY_CHECK(pool_start);

    if (((cpu_addr_t)pool_start & K_MMBLK_ALIGN_MASK) != 0u) {
        return K_ERR_MMBLK_INVALID_POOL_ADDR;
    }

    if ((blk_size & K_MMBLK_ALIGN_MASK) != 0u) {
        return K_ERR_MMBLK_INVALID_BLK_SIZE;
    }

    blk_curr = pool_start;
    blk_next = K_MMBLK_NEXT_BLK(blk_curr, blk_size);

    for (i = 0; i < blk_num - 1u; ++i) {
        *(void **)blk_curr  = blk_next;
        blk_curr            = blk_next;
        blk_next            = K_MMBLK_NEXT_BLK(blk_next, blk_size);
    }
    *(void **)blk_next = K_NULL;

    mbp->pool_start = pool_start;
    mbp->free_list  = pool_start;
    mbp->blk_free   = blk_num;
    mbp->blk_max    = blk_num;
    mbp->blk_size   = blk_size;

    TOS_OBJ_INIT(mbp, KNL_OBJ_TYPE_MMBLK_POOL);

    return K_ERR_NONE;
}

__API__ k_err_t tos_mmblk_pool_destroy(k_mmblk_pool_t *mbp)
{
    TOS_PTR_SANITY_CHECK(mbp);
    TOS_OBJ_VERIFY(mbp, KNL_OBJ_TYPE_MMBLK_POOL);

    mbp->pool_start = K_NULL;
    mbp->free_list  = K_NULL;
    mbp->blk_free   = 0;
    mbp->blk_max    = 0;
    mbp->blk_size   = 0;

    TOS_OBJ_DEINIT(mbp);

    return K_ERR_NONE;
}

__API__ k_err_t tos_mmblk_alloc(k_mmblk_pool_t *mbp, void **blk)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(mbp);
    TOS_OBJ_VERIFY(mbp, KNL_OBJ_TYPE_MMBLK_POOL);

    TOS_CPU_INT_DISABLE();
    if (mbp->blk_free == 0) {
        TOS_CPU_INT_ENABLE();
        *blk = K_NULL;
        return K_ERR_MMBLK_POOL_EMPTY;
    }
    *blk            = mbp->free_list;
    mbp->free_list  = *(void **)mbp->free_list;
    --mbp->blk_free;
    TOS_CPU_INT_ENABLE();
    return K_ERR_NONE;
}

__API__ k_err_t tos_mmblk_free(k_mmblk_pool_t *mbp, void *blk)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(mbp);
    TOS_PTR_SANITY_CHECK(blk);
    TOS_OBJ_VERIFY(mbp, KNL_OBJ_TYPE_MMBLK_POOL);

    TOS_CPU_INT_DISABLE();
    if (mbp->blk_free >= mbp->blk_max) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_MMBLK_POOL_FULL;
    }

    *(void **)blk   = mbp->free_list;
    mbp->free_list  = blk;
    ++mbp->blk_free;
    TOS_CPU_INT_ENABLE();
    return K_ERR_NONE;
}

