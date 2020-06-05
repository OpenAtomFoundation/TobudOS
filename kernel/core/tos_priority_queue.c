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

__STATIC_INLINE__ void prio_q_item_copy_to(k_prio_q_t *prio_q, void *item_out, size_t *item_size, prio_q_slot_t slot)
{
    memcpy(item_out, PRIO_Q_THE_ITEM(prio_q, slot), prio_q->item_size);
    if (item_size) {
        *item_size = prio_q->item_size;
    }
}

__STATIC_INLINE__ void prio_q_item_copy_from(k_prio_q_t *prio_q, void *item_in, prio_q_slot_t slot)
{
    memcpy(PRIO_Q_THE_ITEM(prio_q, slot), item_in, prio_q->item_size);
}

__STATIC_INLINE__ void prio_q_item_increase(k_prio_q_t *prio_q)
{
    ++prio_q->total;
}

__STATIC_INLINE__ void prio_q_item_decrease(k_prio_q_t *prio_q)
{
    --prio_q->total;
}

__STATIC__ void prio_q_pool_mgr_init(prio_q_pool_mgr_t *pool_mgr, prio_q_pool_mgr_ent_t *pool_mgr_ent_array, size_t item_cnt)
{
    prio_q_slot_t i;

    pool_mgr->first_free            = (prio_q_slot_t)0u;
    pool_mgr->pool_mgr_ent_array    = pool_mgr_ent_array;

    for (i = 0; i < item_cnt; ++i) {
        pool_mgr_ent_array[i].next = i + 1;
    }
    pool_mgr_ent_array[item_cnt - 1].next = PRIO_Q_POOL_SLOT_INVALID;
}

__STATIC__ void prio_q_pool_mgr_reset(prio_q_pool_mgr_t *pool_mgr, size_t item_cnt)
{
    prio_q_slot_t i;
    prio_q_pool_mgr_ent_t *pool_mgr_ent_array;

    pool_mgr->first_free    = (prio_q_slot_t)0u;
    pool_mgr_ent_array      = pool_mgr->pool_mgr_ent_array;

    for (i = 0; i < item_cnt; ++i) {
        pool_mgr_ent_array[i].next = i + 1;
    }
    pool_mgr_ent_array[item_cnt - 1].next = PRIO_Q_POOL_SLOT_INVALID;
}

__STATIC__ void prio_q_pool_mgr_deinit(prio_q_pool_mgr_t *pool_mgr)
{
    pool_mgr->first_free            = (prio_q_slot_t)0u;
    pool_mgr->pool_mgr_ent_array    = K_NULL;
}

__STATIC__ int prio_q_mgr_entry_cmp(void *first, void *second)
{
    prio_q_prio_mgr_ent_t *first_entry, *second_entry;

    first_entry     = (prio_q_prio_mgr_ent_t *)first;
    second_entry    = (prio_q_prio_mgr_ent_t *)second;

    // numerically bigger, actually smaller, we build a minimal binary heap here
    if (first_entry->priority < second_entry->priority) {
        return K_TRUE;
    }
    return K_FALSE;
}

__STATIC__ void prio_q_prio_mgr_init(prio_q_prio_mgr_t *prio_mgr, prio_q_prio_mgr_ent_t *prio_mgr_ent_pool, size_t item_cnt)
{
    prio_mgr->prio_mgr_ent_pool = prio_mgr_ent_pool;
    tos_bin_heap_create(&prio_mgr->prio_mgr_bin_heap, prio_mgr_ent_pool, item_cnt, sizeof(prio_q_prio_mgr_ent_t), prio_q_mgr_entry_cmp);
}

__STATIC__ void prio_q_prio_mgr_reset(prio_q_prio_mgr_t *prio_mgr)
{
    tos_bin_heap_flush(&prio_mgr->prio_mgr_bin_heap);
}

__STATIC__ void prio_q_prio_mgr_deinit(prio_q_prio_mgr_t *prio_mgr)
{
    prio_mgr->prio_mgr_ent_pool = K_NULL;
    tos_bin_heap_destroy(&prio_mgr->prio_mgr_bin_heap);
}

__STATIC__ prio_q_slot_t prio_q_pool_mgr_slot_alloc(prio_q_pool_mgr_t *pool_mgr)
{
    prio_q_slot_t fresh;
    prio_q_pool_mgr_ent_t *first_free;

    if (pool_mgr->first_free == PRIO_Q_POOL_SLOT_INVALID) {
        return PRIO_Q_POOL_SLOT_INVALID;
    }

    fresh = pool_mgr->first_free;
    first_free = &pool_mgr->pool_mgr_ent_array[pool_mgr->first_free];
    pool_mgr->first_free = first_free->next;

    return fresh;
}

__STATIC__ void prio_q_pool_mgr_slot_free(prio_q_pool_mgr_t *pool_mgr, prio_q_slot_t slot)
{
    prio_q_pool_mgr_ent_t *slot_entry;

    slot_entry = &pool_mgr->pool_mgr_ent_array[slot];
    slot_entry->next = pool_mgr->first_free;
    pool_mgr->first_free = slot;
}

__STATIC__ void prio_q_prio_mgr_slot_enqueue(prio_q_prio_mgr_t *prio_mgr, prio_q_slot_t slot, k_prio_t prio)
{
    k_err_t err;
    prio_q_prio_mgr_ent_t prio_mgr_entry;

    prio_mgr_entry.priority = prio;
    prio_mgr_entry.slot     = slot;

    err = tos_bin_heap_push(&prio_mgr->prio_mgr_bin_heap, &prio_mgr_entry, sizeof(prio_q_prio_mgr_ent_t));
    TOS_ASSERT(err == K_ERR_NONE);
}

__STATIC__ prio_q_slot_t prio_q_prio_mgr_slot_dequeue(prio_q_prio_mgr_t *prio_mgr, k_prio_t *prio)
{
    k_err_t err;
    size_t dummy;
    prio_q_prio_mgr_ent_t prio_mgr_entry;

    err = tos_bin_heap_pop(&prio_mgr->prio_mgr_bin_heap, &prio_mgr_entry, &dummy);
    TOS_ASSERT(err == K_ERR_NONE);
    TOS_ASSERT(dummy == sizeof(prio_q_prio_mgr_ent_t));

    if (prio) {
        *prio = prio_mgr_entry.priority;
    }

    return prio_mgr_entry.slot;
}

__API__ k_err_t tos_prio_q_create(k_prio_q_t *prio_q, void *mgr_array, void *pool, size_t item_cnt, size_t item_size)
{
    prio_q_pool_mgr_ent_t *pool_mgr_ent_array;
    prio_q_prio_mgr_ent_t *prio_mgr_ent_pool;

    TOS_PTR_SANITY_CHECK(prio_q);
    TOS_PTR_SANITY_CHECK(mgr_array);
    TOS_PTR_SANITY_CHECK(pool);

    pool_mgr_ent_array  = (prio_q_pool_mgr_ent_t *)mgr_array;
    prio_mgr_ent_pool   = (prio_q_prio_mgr_ent_t *)((uint8_t *)mgr_array + PRIO_Q_POOL_MGR_ENT_ARRAY_SIZE(item_cnt));

    prio_q_pool_mgr_init(&prio_q->pool_mgr, pool_mgr_ent_array, item_cnt);
    prio_q_prio_mgr_init(&prio_q->prio_mgr, prio_mgr_ent_pool, item_cnt);

    prio_q->total       = 0;
    prio_q->item_size   = item_size;
    prio_q->item_cnt    = item_cnt;
    prio_q->mgr_pool    = (uint8_t *)mgr_array;
    prio_q->data_pool   = (uint8_t *)pool;

    TOS_OBJ_INIT(prio_q, KNL_OBJ_TYPE_PRIORITY_QUEUE);
    knl_object_alloc_set_static(&prio_q->knl_obj);

    return K_ERR_NONE;
}

__API__ k_err_t tos_prio_q_destroy(k_prio_q_t *prio_q)
{
    TOS_PTR_SANITY_CHECK(prio_q);
    TOS_OBJ_VERIFY(prio_q, KNL_OBJ_TYPE_PRIORITY_QUEUE);

    if (!knl_object_alloc_is_static(&prio_q->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }

    prio_q_pool_mgr_deinit(&prio_q->pool_mgr);
    prio_q_prio_mgr_deinit(&prio_q->prio_mgr);

    prio_q->total       = 0;
    prio_q->item_size   = 0;
    prio_q->item_cnt    = 0;
    prio_q->mgr_pool    = K_NULL;
    prio_q->data_pool   = K_NULL;

    TOS_OBJ_DEINIT(prio_q);
    knl_object_alloc_reset(&prio_q->knl_obj);

    return K_ERR_NONE;
}

__API__ k_err_t tos_prio_q_create_dyn(k_prio_q_t *prio_q, size_t item_cnt, size_t item_size)
{
    k_err_t err;
    void *mgr_pool, *data_pool;

    TOS_PTR_SANITY_CHECK(prio_q);

    mgr_pool = tos_mmheap_alloc(TOS_PRIO_Q_MGR_ARRAY_SIZE(item_cnt));
    if (!mgr_pool) {
        return K_ERR_OUT_OF_MEMORY;
    }

    data_pool = tos_mmheap_alloc(item_cnt * item_size);
    if (!data_pool) {
        tos_mmheap_free(mgr_pool);
        return K_ERR_OUT_OF_MEMORY;
    }

    err = tos_prio_q_create(prio_q, mgr_pool, data_pool, item_cnt, item_size);
    if (err != K_ERR_NONE) {
        tos_mmheap_free(data_pool);
        tos_mmheap_free(mgr_pool);
    }

    knl_object_alloc_set_dynamic(&prio_q->knl_obj);

    return K_ERR_NONE;
}

__API__ k_err_t tos_prio_q_destroy_dyn(k_prio_q_t *prio_q)
{
    TOS_PTR_SANITY_CHECK(prio_q);
    TOS_OBJ_VERIFY(prio_q, KNL_OBJ_TYPE_PRIORITY_QUEUE);

    if (!knl_object_alloc_is_dynamic(&prio_q->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }

    prio_q_pool_mgr_deinit(&prio_q->pool_mgr);
    prio_q_prio_mgr_deinit(&prio_q->prio_mgr);

    tos_mmheap_free(prio_q->mgr_pool);
    tos_mmheap_free(prio_q->data_pool);

    prio_q->total       = 0;
    prio_q->item_size   = 0;
    prio_q->item_cnt    = 0;
    prio_q->mgr_pool    = K_NULL;
    prio_q->data_pool   = K_NULL;

    TOS_OBJ_DEINIT(prio_q);
    knl_object_alloc_reset(&prio_q->knl_obj);

    return K_ERR_NONE;
}

__STATIC__ void prio_q_do_enqueue(k_prio_q_t *prio_q, void *item, prio_q_slot_t slot, k_prio_t prio)
{
    prio_q_item_copy_from(prio_q, item, slot);
    prio_q_prio_mgr_slot_enqueue(&prio_q->prio_mgr, slot, prio);
    prio_q_item_increase(prio_q);
}

__API__ k_err_t tos_prio_q_enqueue(k_prio_q_t *prio_q, void *item, size_t item_size, k_prio_t prio)
{
    TOS_CPU_CPSR_ALLOC();
    prio_q_slot_t the_slot;

    TOS_PTR_SANITY_CHECK(prio_q);
    TOS_PTR_SANITY_CHECK(item);
    TOS_OBJ_VERIFY(prio_q, KNL_OBJ_TYPE_PRIORITY_QUEUE);

    if (item_size != prio_q->item_size) {
        return K_ERR_PRIO_Q_ITEM_SIZE_NOT_MATCH;
    }

    if (tos_prio_q_is_full(prio_q)) {
        return K_ERR_PRIO_Q_FULL;
    }

    TOS_CPU_INT_DISABLE();

    the_slot = prio_q_pool_mgr_slot_alloc(&prio_q->pool_mgr);
    TOS_ASSERT(the_slot != PRIO_Q_POOL_SLOT_INVALID);
    prio_q_do_enqueue(prio_q, item, the_slot, prio);

    TOS_CPU_INT_ENABLE();
    return K_ERR_NONE;
}

__STATIC__ void prio_q_do_dequeue(k_prio_q_t *prio_q, void *item, size_t *item_size, prio_q_slot_t slot)
{
    prio_q_pool_mgr_slot_free(&prio_q->pool_mgr, slot);
    prio_q_item_copy_to(prio_q, item, item_size, slot);
    prio_q_item_decrease(prio_q);
}

__API__ k_err_t tos_prio_q_dequeue(k_prio_q_t *prio_q, void *item, size_t *item_size, k_prio_t *prio)
{
    TOS_CPU_CPSR_ALLOC();
    prio_q_slot_t the_slot;

    TOS_PTR_SANITY_CHECK(prio_q);
    TOS_PTR_SANITY_CHECK(item);
    TOS_OBJ_VERIFY(prio_q, KNL_OBJ_TYPE_PRIORITY_QUEUE);

    if (tos_prio_q_is_empty(prio_q)) {
        return K_ERR_PRIO_Q_EMPTY;
    }

    TOS_CPU_INT_DISABLE();

    the_slot = prio_q_prio_mgr_slot_dequeue(&prio_q->prio_mgr, prio);
    prio_q_do_dequeue(prio_q, item, item_size, the_slot);

    TOS_CPU_INT_ENABLE();
    return K_ERR_NONE;
}

__API__ k_err_t tos_prio_q_flush(k_prio_q_t *prio_q)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(prio_q);
    TOS_OBJ_VERIFY(prio_q, KNL_OBJ_TYPE_PRIORITY_QUEUE);

    TOS_CPU_INT_DISABLE();

    prio_q_pool_mgr_reset(&prio_q->pool_mgr, prio_q->item_cnt);
    prio_q_prio_mgr_reset(&prio_q->prio_mgr);
    prio_q->total = 0;

    TOS_CPU_INT_ENABLE();
    return K_ERR_NONE;
}

__API__ int tos_prio_q_is_empty(k_prio_q_t *prio_q)
{
    TOS_CPU_CPSR_ALLOC();
    int is_empty = K_FALSE;

    TOS_PTR_SANITY_CHECK_RC(prio_q, K_FALSE);
    TOS_OBJ_VERIFY_RC(prio_q, KNL_OBJ_TYPE_PRIORITY_QUEUE, K_FALSE);

    TOS_CPU_INT_DISABLE();
    is_empty = (prio_q->total == 0);
    TOS_CPU_INT_ENABLE();

    return is_empty;
}

__API__ int tos_prio_q_is_full(k_prio_q_t *prio_q)
{
    TOS_CPU_CPSR_ALLOC();
    int is_full = K_FALSE;

    TOS_PTR_SANITY_CHECK_RC(prio_q, K_FALSE);
    TOS_OBJ_VERIFY_RC(prio_q, KNL_OBJ_TYPE_PRIORITY_QUEUE, K_FALSE);

    TOS_CPU_INT_DISABLE();
    is_full = (prio_q->total == prio_q->item_cnt);
    TOS_CPU_INT_ENABLE();

    return is_full;
}

