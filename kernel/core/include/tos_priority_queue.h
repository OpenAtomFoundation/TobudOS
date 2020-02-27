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

#ifndef _TOS_PRIORITY_QUEUE_H_
#define  _TOS_PRIORITY_QUEUE_H_

__CDECLS_BEGIN

typedef uint16_t    prio_q_slot_t;

typedef struct prio_q_pool_manager_entry_st {
    prio_q_slot_t           next;
} prio_q_pool_mgr_ent_t;

typedef struct prio_q_pool_manager_st {
    prio_q_slot_t           first_free;
    prio_q_pool_mgr_ent_t  *pool_mgr_ent_array;
} prio_q_pool_mgr_t;

typedef struct prio_q_priority_manager_entry_st {
    k_prio_t        priority;
    prio_q_slot_t   slot;
} prio_q_prio_mgr_ent_t;

typedef struct prio_q_prio_manager_st {
    k_bin_heap_t            prio_mgr_bin_heap;
    prio_q_prio_mgr_ent_t  *prio_mgr_ent_pool;
} prio_q_prio_mgr_t;

typedef struct k_priority_queue_st {
    knl_obj_t           knl_obj;

    prio_q_pool_mgr_t   pool_mgr;
    prio_q_prio_mgr_t   prio_mgr;

    size_t              total;
    size_t              item_size;
    size_t              item_cnt;

    uint8_t            *mgr_pool;
    uint8_t            *data_pool;
} k_prio_q_t;

#define PRIO_Q_POOL_SLOT_INVALID    ((prio_q_slot_t)-1)

#define PRIO_Q_POOL_MGR_ENT_ARRAY_SIZE(item_cnt) \
    (item_cnt * sizeof(prio_q_pool_mgr_ent_t))

#define PRIO_Q_PRIO_MGR_ENT_POOL_SIZE(item_cnt) \
    (item_cnt * sizeof(prio_q_prio_mgr_ent_t))

#define PRIO_Q_THE_ITEM(prio_q, slot)       (void *)(&prio_q->data_pool[slot * prio_q->item_size])

// get the size of mgr_array to create a priority queue
#define TOS_PRIO_Q_MGR_ARRAY_SIZE(item_cnt) \
    (PRIO_Q_POOL_MGR_ENT_ARRAY_SIZE(item_cnt) + PRIO_Q_PRIO_MGR_ENT_POOL_SIZE(item_cnt))

/**
 * @brief Create a priority queue.
 * create a priority queue.
 *
 * @attention if we wanna create a priority queue, we should offer a manager array buffer of which the size can be calculated by TOS_PRIO_Q_MGR_ARRAY_SIZE.
 *
 * @param[in]   prio_q      pointer to the handler of the priority queue.
 * @param[in]   mgr_array   manager array buffer of the priority queue.
 * @param[in]   pool        pool buffer of the priority queue.
 * @param[in]   item_cnt    item count of the priority queue.
 * @param[in]   item_size   size of each item of the priority queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_q_create(k_prio_q_t *prio_q, void *mgr_array, void *pool, size_t item_cnt, size_t item_size);

/**
 * @brief Destroy a priority queue.
 * destroy a priority queue.
 *
 * @attention None
 *
 * @param[in]   prio_q      pointer to the handler of the bpriority queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_q_destroy(k_prio_q_t *prio_q);

#if TOS_CFG_MMHEAP_EN > 0u

/**
 * @brief Create a priority queue with dynamic allocated mgr array and data pool.
 * create a priority queue with dynamic allocated mgr array and data pool.
 *
 * @attention if we wanna create a priority queue, we should offer a manager array buffer of which the size can be calculated by TOS_PRIO_Q_MGR_ARRAY_SIZE.
 *
 * @param[in]   prio_q      pointer to the handler of the priority queue.
 * @param[in]   mgr_array   manager array buffer of the priority queue.
 * @param[in]   pool        pool buffer of the priority queue.
 * @param[in]   item_cnt    item count of the priority queue.
 * @param[in]   item_size   size of each item of the priority queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_q_create_dyn(k_prio_q_t *prio_q, size_t item_cnt, size_t item_size);

/**
 * @brief Destroy a priority queue with dynamic allocated mgr array and data pool.
 * destroy a priority queue with dynamic allocated mgr array and data pool.
 *
 * @attention None
 *
 * @param[in]   prio_q      pointer to the handler of the bpriority queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_q_destroy_dyn(k_prio_q_t *prio_q);

#endif

/**
 * @brief Enqueue an priority queue.
 * enqueue an item into the priority queue.
 *
 * @attention None
 *
 * @param[in]   prio_q      pointer to the handler of priority queue.
 * @param[in]   item        the item to be enqueued.
 * @param[in]   item_size   size of the item(should be consistent with the item_size passed to tos_prio_q_create).
 * @param[in]   prio        priority of the item to be enqueued(should be consistent with the item_size passed to tos_prio_q_create).
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                         return successfully.
 * @retval  #K_ERR_PRIO_Q_ITEM_SIZE_NOT_MATCH   the item_size is not consistent with the item_size passed to tos_prio_q_create.
 * @retval  #K_ERR_PRIO_Q_FULL                  the priority queue is full.
 */
__API__ k_err_t tos_prio_q_enqueue(k_prio_q_t *prio_q, void *item, size_t item_size, k_prio_t prio);

/**
 * @brief Dequeue an item.
 * dequeue an item from the priority queue.
 *
 * @attention None
 *
 * @param[in]   prio_q      pointer to the handler of the priority queue.
 * @param[out]  item        buffer to hold the item dequeued.
 * @param[out]  item_size   size of the item dequeued(should be consistent with the item_size passed to tos_prio_q_create).
 * @param[out]  prio        priority of the item dequeued.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 * @retval  #K_ERR_PRIO_Q_EMPTY     the priority queue is empty.
 */
__API__ k_err_t tos_prio_q_dequeue(k_prio_q_t *prio_q, void *item, size_t *item_size, k_prio_t *prio);

/**
 * @brief Flush the priority queue.
 * flush the priority queue.
 *
 * @attention None
 *
 * @param[in]   prio_q      pointer to the handler of the priority queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_prio_q_flush(k_prio_q_t *prio_q);

/**
 * @brief Whether the priority queue is empty.
 * Whether the priority queue is empty.
 *
 * @attention None
 *
 * @param[in]   prio_q      pointer to the handler of the priority queue.
 *
 * @return  whether the priority queue is emtpy.
 * @retval  #0      the priority queue is not empty.
 * @retval  #Not 0  the priority queue is empty.
 */
__API__ int tos_prio_q_is_empty(k_prio_q_t *prio_q);

/**
 * @brief Whether the priority queue is full.
 * Whether the priority queue is full.
 *
 * @attention None
 *
 * @param[in]   prio_q      pointer to the handler of the priority queue.
 *
 * @return  whether the priority queue is full.
 * @retval  #0      the priority queue is not full.
 * @retval  #Not 0  the priority queue is full.
 */
__API__ int tos_prio_q_is_full(k_prio_q_t *prio_q);

__CDECLS_END

#endif /* _TOS_PRIORITY_QUEUE_H_ */

