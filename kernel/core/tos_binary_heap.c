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

#include "tos.h"

__STATIC_INLINE__ void bin_heap_item_copy_to(k_bin_heap_t *bin_heap, void *item_out, size_t *item_size)
{
    memcpy(item_out, BIN_HEAP_FIRST_ITEM(bin_heap), bin_heap->item_size);
    if (item_size) {
        *item_size = bin_heap->item_size;
    }
}

__STATIC_INLINE__ void bin_heap_item_copy_from(k_bin_heap_t *bin_heap, void *item_in)
{
    memcpy(BIN_HEAP_LAST_ITEM(bin_heap), item_in, bin_heap->item_size);
}

__STATIC_INLINE__ void bin_heap_item_increase(k_bin_heap_t *bin_heap)
{
    ++bin_heap->total;
}

__STATIC_INLINE__ void bin_heap_item_decrease(k_bin_heap_t *bin_heap)
{
    --bin_heap->total;
}

__STATIC__ void bin_heap_do_percolate_up(k_bin_heap_t *bin_heap, uint16_t hole, void *item_backup)
{
    k_bin_heap_cmp cmp;
    size_t item_size;
    uint16_t parent, top;
    void *hole_item, *parent_item;

    top         = 0u;
    parent      = BIN_HEAP_PARENT(hole);
    cmp         = bin_heap->cmp;
    item_size   = bin_heap->item_size;

    hole_item   = BIN_HEAP_THE_ITEM(bin_heap, hole);
    parent_item = BIN_HEAP_THE_ITEM(bin_heap, parent);

    while (hole > top && cmp(item_backup, parent_item)) {
        memcpy(hole_item, parent_item, item_size);
        hole        = parent;
        parent      = BIN_HEAP_PARENT(hole);
        hole_item   = BIN_HEAP_THE_ITEM(bin_heap, hole);
        parent_item = BIN_HEAP_THE_ITEM(bin_heap, parent);
    }
    hole_item = BIN_HEAP_THE_ITEM(bin_heap, hole);
    memcpy(hole_item, item_backup, item_size);
}

__STATIC__ void bin_heap_percolate_up(k_bin_heap_t *bin_heap, void *item_backup)
{
    bin_heap_do_percolate_up(bin_heap, bin_heap->total, item_backup);
}

__STATIC__ void bin_heap_percolate_down(k_bin_heap_t *bin_heap)
{
    k_bin_heap_cmp cmp;
    size_t item_size;
    uint16_t lchild, rchild, the_child, hole;
    void *hole_item, *rchild_item, *lchild_item, *the_child_item;

    hole            = 0u;
    rchild          = BIN_HEAP_RCHILD(hole);
    lchild          = BIN_HEAP_LSIBLING(rchild);
    the_child       = rchild;
    cmp             = bin_heap->cmp;
    item_size       = bin_heap->item_size;

    hole_item       = BIN_HEAP_THE_ITEM(bin_heap, hole);
    rchild_item     = BIN_HEAP_THE_ITEM(bin_heap, rchild);
    lchild_item     = BIN_HEAP_THE_ITEM(bin_heap, lchild);

    while (the_child < bin_heap->total) {
        if (cmp(lchild_item, rchild_item)) {
            the_child = lchild;
        }
        the_child_item = BIN_HEAP_THE_ITEM(bin_heap, the_child);
        memcpy(hole_item, the_child_item, item_size);

        hole        = the_child;
        the_child   = BIN_HEAP_RCHILD(the_child);
        rchild      = the_child;
        lchild      = BIN_HEAP_LSIBLING(rchild);

        hole_item   = BIN_HEAP_THE_ITEM(bin_heap, hole);
        rchild_item = BIN_HEAP_THE_ITEM(bin_heap, rchild);
        lchild_item = BIN_HEAP_THE_ITEM(bin_heap, lchild);
    }

    if (the_child == bin_heap->total) {
        memcpy(hole_item, lchild_item, item_size);
        hole        = lchild;
        hole_item   = BIN_HEAP_THE_ITEM(bin_heap, hole);
    }
    bin_heap_do_percolate_up(bin_heap, hole, BIN_HEAP_LAST_ITEM(bin_heap));
}

__API__ k_err_t tos_bin_heap_create(k_bin_heap_t *bin_heap, void *pool, size_t item_cnt, size_t item_size, k_bin_heap_cmp cmp)
{
    TOS_PTR_SANITY_CHECK(bin_heap);
    TOS_PTR_SANITY_CHECK(pool);
    TOS_PTR_SANITY_CHECK(cmp);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_init(&bin_heap->knl_obj, KNL_OBJ_TYPE_BINARY_HEAP);
#endif
#if TOS_CFG_MMHEAP_EN > 0u
    knl_object_alloc_set_static(&bin_heap->knl_obj);
#endif

    bin_heap->total     = 0;
    bin_heap->cmp       = cmp;
    bin_heap->item_size = item_size;
    bin_heap->item_cnt  = item_cnt;
    bin_heap->pool      = (uint8_t *)pool;

    return K_ERR_NONE;
}

__API__ k_err_t tos_bin_heap_destroy(k_bin_heap_t *bin_heap)
{
    TOS_PTR_SANITY_CHECK(bin_heap);
    TOS_OBJ_VERIFY(bin_heap, KNL_OBJ_TYPE_BINARY_HEAP);

#if TOS_CFG_MMHEAP_EN > 0u
    if (!knl_object_alloc_is_static(&bin_heap->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }
#endif

    bin_heap->total     = 0;
    bin_heap->cmp       = K_NULL;
    bin_heap->item_size = 0;
    bin_heap->item_cnt  = 0;
    bin_heap->pool      = K_NULL;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_deinit(&bin_heap->knl_obj);
#endif
#if TOS_CFG_MMHEAP_EN > 0u
    knl_object_alloc_reset(&bin_heap->knl_obj);
#endif

    return K_ERR_NONE;
}

#if TOS_CFG_MMHEAP_EN > 0u

__API__ k_err_t tos_bin_heap_create_dyn(k_bin_heap_t *bin_heap, size_t item_cnt, size_t item_size, k_bin_heap_cmp cmp)
{
    void *pool;

    TOS_PTR_SANITY_CHECK(bin_heap);
    TOS_PTR_SANITY_CHECK(cmp);

    pool = tos_mmheap_alloc(item_cnt * item_size);
    if (!pool) {
        return K_ERR_OUT_OF_MEMORY;
    }

    bin_heap->total     = 0;
    bin_heap->cmp       = cmp;
    bin_heap->item_size = item_size;
    bin_heap->item_cnt  = item_cnt;
    bin_heap->pool      = (uint8_t *)pool;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_init(&bin_heap->knl_obj, KNL_OBJ_TYPE_BINARY_HEAP);
#endif
    knl_object_alloc_set_dynamic(&bin_heap->knl_obj);

    return K_ERR_NONE;
}

__API__ k_err_t tos_bin_heap_destroy_dyn(k_bin_heap_t *bin_heap)
{
    TOS_PTR_SANITY_CHECK(bin_heap);
    TOS_OBJ_VERIFY(bin_heap, KNL_OBJ_TYPE_BINARY_HEAP);

    if (!knl_object_alloc_is_dynamic(&bin_heap->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }

    tos_mmheap_free(bin_heap->pool);

    bin_heap->total     = 0;
    bin_heap->cmp       = K_NULL;
    bin_heap->item_size = 0;
    bin_heap->item_cnt  = 0;
    bin_heap->pool      = K_NULL;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_deinit(&bin_heap->knl_obj);
#endif
    knl_object_alloc_reset(&bin_heap->knl_obj);

    return K_ERR_NONE;
}

#endif

__API__ k_err_t tos_bin_heap_push(k_bin_heap_t *bin_heap, void *item, size_t item_size)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(bin_heap);
    TOS_PTR_SANITY_CHECK(item);
    TOS_OBJ_VERIFY(bin_heap, KNL_OBJ_TYPE_BINARY_HEAP);

    if (item_size != bin_heap->item_size) {
        return K_ERR_BIN_HEAP_ITEM_SIZE_NOT_MATCH;
    }

    if (tos_bin_heap_is_full(bin_heap)) {
        return K_ERR_BIN_HEAP_FULL;
    }

    TOS_CPU_INT_DISABLE();

    bin_heap_item_copy_from(bin_heap, item);
    bin_heap_percolate_up(bin_heap, item);
    bin_heap_item_increase(bin_heap);

    TOS_CPU_INT_ENABLE();
    return K_ERR_NONE;
}

__API__ k_err_t tos_bin_heap_pop(k_bin_heap_t *bin_heap, void *item, size_t *item_size)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(bin_heap);
    TOS_PTR_SANITY_CHECK(item);
    TOS_OBJ_VERIFY(bin_heap, KNL_OBJ_TYPE_BINARY_HEAP);

    TOS_CPU_INT_DISABLE();

    if (tos_bin_heap_is_empty(bin_heap)) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_BIN_HEAP_EMPTY;
    }

    bin_heap_item_copy_to(bin_heap, item, item_size);
    bin_heap_item_decrease(bin_heap);
    bin_heap_percolate_down(bin_heap);

    TOS_CPU_INT_ENABLE();

    return K_ERR_NONE;
}

__API__ k_err_t tos_bin_heap_flush(k_bin_heap_t *bin_heap)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(bin_heap);
    TOS_OBJ_VERIFY(bin_heap, KNL_OBJ_TYPE_BINARY_HEAP);

    TOS_CPU_INT_DISABLE();
    bin_heap->total = 0;
    TOS_CPU_INT_ENABLE();

    return K_ERR_NONE;
}

__API__ int tos_bin_heap_is_empty(k_bin_heap_t *bin_heap)
{
    TOS_CPU_CPSR_ALLOC();
    int is_empty = K_FALSE;

    TOS_PTR_SANITY_CHECK_RC(bin_heap, K_FALSE);
    TOS_OBJ_VERIFY_RC(bin_heap, KNL_OBJ_TYPE_BINARY_HEAP, K_FALSE);

    TOS_CPU_INT_DISABLE();
    is_empty = (bin_heap->total == 0);
    TOS_CPU_INT_ENABLE();

    return is_empty;
}

__API__ int tos_bin_heap_is_full(k_bin_heap_t *bin_heap)
{
    TOS_CPU_CPSR_ALLOC();
    int is_full = K_FALSE;

    TOS_PTR_SANITY_CHECK_RC(bin_heap, K_FALSE);
    TOS_OBJ_VERIFY_RC(bin_heap, KNL_OBJ_TYPE_BINARY_HEAP, K_FALSE);

    TOS_CPU_INT_DISABLE();
    is_full = (bin_heap->total == bin_heap->item_cnt);
    TOS_CPU_INT_ENABLE();

    return is_full;
}

