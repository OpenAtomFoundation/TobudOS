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

#ifndef _TOS_RING_QUEUE_H_
#define  _TOS_RING_QUEUE_H_

typedef struct k_ring_queue_st {
    knl_obj_t   knl_obj;

    uint16_t    head;
    uint16_t    tail;
    size_t      total;

    uint8_t    *pool;

    size_t      item_size;
    size_t      item_cnt;
} k_ring_q_t;

#define RING_HEAD_ITEM(ring_q)      (uint8_t *)(&ring_q->pool[ring_q->head * ring_q->item_size])
#define RING_TAIL_ITEM(ring_q)      (uint8_t *)(&ring_q->pool[ring_q->tail * ring_q->item_size])
#define RING_NEXT(ring_q, index)    ((index + 1) % ring_q->item_cnt)

/**
 * @brief Create a ring queue.
 * create a ring queue.
 *
 * @attention None
 *
 * @param[in]   ring_q      pointer to the handler of the ring queue.
 * @param[in]   pool        pool buffer of the ring queue.
 * @param[in]   item_cnt    item count of the ring queue.
 * @param[in]   item_size   size of each item of the ring queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_ring_q_create(k_ring_q_t *ring_q, void *pool, size_t item_cnt, size_t item_size);

/**
 * @brief Destroy a ring queue.
 * destroy a ring queue.
 *
 * @attention None
 *
 * @param[in]   ring_q      pointer to the handler of the ring queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_OBJ_INVALID_ALLOC_TYPE   invalid alloc type(is dynamic allocated not static)
 */
__API__ k_err_t tos_ring_q_destroy(k_ring_q_t *ring_q);

#if TOS_CFG_MMHEAP_EN > 0u

/**
 * @brief Create a ring queue with dynamic allocated pool.
 * create a ring queue with dynamic allocated pool.
 *
 * @attention pool inside is dynamic allocated.
 *
 * @param[in]   ring_q      pointer to the handler of the ring queue.
 * @param[in]   item_cnt    item count of the ring queue.
 * @param[in]   item_size   size of each item of the ring queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                 return successfully.
 * @retval  #K_ERR_OUT_OF_MEMORY        out of memory
 */
__API__ k_err_t tos_ring_q_create_dyn(k_ring_q_t *ring_q, size_t item_cnt, size_t item_size);

/**
 * @brief Destroy a ring queue with a dynamic allocated pool.
 * destroy a ring queue with a dynamic allocated pool.
 *
 * @attention None
 *
 * @param[in]   ring_q      pointer to the handler of the ring queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_OBJ_INVALID_ALLOC_TYPE   invalid alloc type(is static allocated not dynamic)
 */
__API__ k_err_t tos_ring_q_destroy_dyn(k_ring_q_t *ring_q);

#endif


/**
 * @brief Enqueue an item.
 * enqueue an item into the ring queue.
 *
 * @attention None
 *
 * @param[in]   ring_q      pointer to the handler of the ring queue.
 * @param[in]   item        the item to be enqueued.
 * @param[in]   item_size   size of the item(should be consistent with the item_size passed to tos_ring_q_create).
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                         return successfully.
 * @retval  #K_ERR_RING_Q_ITEM_SIZE_NOT_MATCH   the item_size is not consistent with the item_size passed to tos_ring_q_create.
 * @retval  #K_ERR_RING_Q_FULL                  the ring queue is full.
 */
__API__ k_err_t tos_ring_q_enqueue(k_ring_q_t *ring_q, void *item, size_t item_size);

/**
 * @brief Dequeue an item.
 * dequeue an item from the ring queue.
 *
 * @attention None
 *
 * @param[in]   ring_q      pointer to the handler of the ring queue.
 * @param[out]  item        buffer to hold the item dequeued.
 * @param[out]  item_size   size of the item dequeued(should be consistent with the item_size passed to tos_ring_q_create).
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 * @retval  #K_ERR_RING_Q_EMPTY     the ring queue is empty.
 */
__API__ k_err_t tos_ring_q_dequeue(k_ring_q_t *ring_q, void *item, size_t *item_size);

/**
 * @brief Flush the ring queue.
 * flush the ring queue.
 *
 * @attention None
 *
 * @param[in]   ring_q      pointer to the handler of the ring queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_ring_q_flush(k_ring_q_t *ring_q);

/**
 * @brief Whether the ring queue is empty.
 * Whether the ring queue is empty.
 *
 * @attention None
 *
 * @param[in]   ring_q      pointer to the handler of the ring queue.
 *
 * @return  whether the ring queue is emtpy.
 * @retval  #0      the ring queue is not empty.
 * @retval  #Not 0  the ring queue is empty.
 */
__API__ int     tos_ring_q_is_empty(k_ring_q_t *ring_q);

/**
 * @brief Whether the ring queue is full.
 * Whether the ring queue is full.
 *
 * @attention None
 *
 * @param[in]   ring_q      pointer to the handler of the ring queue.
 *
 * @return  whether the ring queue is full.
 * @retval  #0      the ring queue is not full.
 * @retval  #Not 0  the ring queue is full.
 */
__API__ int     tos_ring_q_is_full(k_ring_q_t *ring_q);

#endif

