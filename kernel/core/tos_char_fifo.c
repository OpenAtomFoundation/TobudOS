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

__API__ k_err_t tos_chr_fifo_create(k_chr_fifo_t *chr_fifo, void *buffer, size_t size)
{
    TOS_PTR_SANITY_CHECK(chr_fifo);
    TOS_PTR_SANITY_CHECK(buffer);
    k_err_t err;

    err = tos_ring_q_create(&chr_fifo->ring_q, buffer, size, sizeof(uint8_t));
    if (err != K_ERR_NONE) {
        return err;
    }

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_init(&chr_fifo->knl_obj, KNL_OBJ_TYPE_CHAR_FIFO);
#endif

    return K_ERR_NONE;
}

__API__ k_err_t tos_chr_fifo_destroy(k_chr_fifo_t *chr_fifo)
{
    TOS_PTR_SANITY_CHECK(chr_fifo);
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&chr_fifo->knl_obj, KNL_OBJ_TYPE_CHAR_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    err = tos_ring_q_destroy(&chr_fifo->ring_q);
    if (err != K_ERR_NONE) {
        return err;
    }

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_deinit(&chr_fifo->knl_obj);
#endif

    return K_ERR_NONE;
}

__API__ k_err_t tos_chr_fifo_push(k_chr_fifo_t *chr_fifo, uint8_t data)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&chr_fifo->knl_obj, KNL_OBJ_TYPE_CHAR_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    err = tos_ring_q_enqueue(&chr_fifo->ring_q, &data, sizeof(uint8_t));
    if (err != K_ERR_NONE) {
        return err;
    }

    return K_ERR_NONE;
}

__API__ int tos_chr_fifo_push_stream(k_chr_fifo_t *chr_fifo, uint8_t *stream, size_t size)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;
    int i = 0;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&chr_fifo->knl_obj, KNL_OBJ_TYPE_CHAR_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    while (i < size) {
        err = tos_ring_q_enqueue(&chr_fifo->ring_q, &stream[i], sizeof(uint8_t));
        if (err != K_ERR_NONE) {
            TOS_CPU_INT_ENABLE();
            return i;
        }
        ++i;
    }

    TOS_CPU_INT_ENABLE();
    return i;
}

__API__ k_err_t tos_chr_fifo_pop(k_chr_fifo_t *chr_fifo, uint8_t *out)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&chr_fifo->knl_obj, KNL_OBJ_TYPE_CHAR_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    err = tos_ring_q_dequeue(&chr_fifo->ring_q, (void *)out, K_NULL);
    if (err != K_ERR_NONE) {
        return err;
    }

    return K_ERR_NONE;
}

__API__ int tos_chr_fifo_pop_stream(k_chr_fifo_t *chr_fifo, uint8_t *buffer, size_t size)
{
    TOS_CPU_CPSR_ALLOC();
    int i = 0;
    uint8_t data;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&chr_fifo->knl_obj, KNL_OBJ_TYPE_CHAR_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    while (i < size) {
        if (tos_ring_q_dequeue(&chr_fifo->ring_q, &data, K_NULL) != K_ERR_NONE) {
            TOS_CPU_INT_ENABLE();
            return i;
        }
        buffer[i++] = data;
    }

    TOS_CPU_INT_ENABLE();
    return i;
}

__API__ k_err_t tos_chr_fifo_flush(k_chr_fifo_t *chr_fifo)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&chr_fifo->knl_obj, KNL_OBJ_TYPE_CHAR_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    return tos_ring_q_flush(&chr_fifo->ring_q);
}

__API__ int tos_chr_fifo_is_empty(k_chr_fifo_t *chr_fifo)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&chr_fifo->knl_obj, KNL_OBJ_TYPE_CHAR_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    return tos_ring_q_is_empty(&chr_fifo->ring_q);
}

__API__ int tos_chr_fifo_is_full(k_chr_fifo_t *chr_fifo)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&chr_fifo->knl_obj, KNL_OBJ_TYPE_CHAR_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    return tos_ring_q_is_full(&chr_fifo->ring_q);
}

