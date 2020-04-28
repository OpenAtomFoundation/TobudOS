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

#ifndef _TOS_BINARY_HEAP_H_
#define  _TOS_BINARY_HEAP_H_

__CDECLS_BEGIN

typedef int (*k_bin_heap_cmp)(void *first, void *second);

typedef struct k_binary_heap_st {
    knl_obj_t           knl_obj;

    size_t              total;

    k_bin_heap_cmp      cmp;
    size_t              item_size;
    size_t              item_cnt;
    uint8_t            *pool;
} k_bin_heap_t;

#define BIN_HEAP_FIRST_ITEM(bin_heap)   (void *)(&bin_heap->pool[0])
#define BIN_HEAP_LAST_ITEM(bin_heap)    (void *)(&bin_heap->pool[bin_heap->total * bin_heap->item_size])
#define BIN_HEAP_THE_ITEM(bin_heap, index)  (void *)(&bin_heap->pool[index * bin_heap->item_size])

#define BIN_HEAP_PARENT(index)          ((index - 1) / 2)
#define BIN_HEAP_RCHILD(index)          (2 * (index + 1))
#define BIN_HEAP_LSIBLING(index)        (index - 1)

/**
 * @brief Create a binary heap.
 * create a binary heap.
 *
 * @attention None
 *
 * @param[in]   bin_heap    pointer to the handler of the binary heap.
 * @param[in]   pool        pool buffer of the binary heap.
 * @param[in]   item_cnt    item count of the binary heap.
 * @param[in]   item_size   size of each item of the binary heap.
 * @param[in]   cmp         compare function to determine two items which is bigger or smaller.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_bin_heap_create(k_bin_heap_t *bin_heap, void *pool, size_t item_cnt, size_t item_size, k_bin_heap_cmp cmp);

/**
 * @brief Destroy a binary heap.
 * destroy a binary heap.
 *
 * @attention None
 *
 * @param[in]   bin_heap    pointer to the handler of the binary heap.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_bin_heap_destroy(k_bin_heap_t *bin_heap);

#if TOS_CFG_MMHEAP_EN > 0u

/**
 * @brief Create a binary heap with a dynamic allocated pool.
 * create a binary heap with a dynamic allocated pool.
 *
 * @attention None
 *
 * @param[in]   bin_heap    pointer to the handler of the binary heap.
 * @param[in]   item_cnt    item count of the binary heap.
 * @param[in]   item_size   size of each item of the binary heap.
 * @param[in]   cmp         compare function to determine two items which is bigger or smaller.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_bin_heap_create_dyn(k_bin_heap_t *bin_heap, size_t item_cnt, size_t item_size, k_bin_heap_cmp cmp);

/**
 * @brief Destroy a binary heap with a dynamic allocated pool.
 * destroy a binary heap with a dynamic allocated pool.
 *
 * @attention None
 *
 * @param[in]   bin_heap    pointer to the handler of the binary heap.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_bin_heap_destroy_dyn(k_bin_heap_t *bin_heap);

#endif

/**
 * @brief Push an item.
 * push an item into the binary heap.
 *
 * @attention None
 *
 * @param[in]   bin_heap    pointer to the handler of the binary heap.
 * @param[in]   item        the item to be pushed.
 * @param[in]   item_size   size of the item(should be consistent with the item_size passed to tos_bin_heap_create).
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                         return successfully.
 * @retval  #K_ERR_BIN_HEAP_ITEM_SIZE_NOT_MATCH the item_size is not consistent with the item_size passed to tos_bin_heap_create.
 * @retval  #K_ERR_BIN_HEAP_FULL                the binary heap is full.
 */
__API__ k_err_t tos_bin_heap_push(k_bin_heap_t *bin_heap, void *item, size_t item_size);

/**
 * @brief Pop an item.
 * pop an item from the binary heap.
 *
 * @attention None
 *
 * @param[in]   bin_heap    pointer to the handler of the binary heap.
 * @param[out]  item        buffer to hold the item poped.
 * @param[out]  item_size   size of the item poped(should be consistent with the item_size passed to tos_bin_heap_create).
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 * @retval  #K_ERR_BIN_HEAP_EMPTY   the ring queue is empty.
 */
__API__ k_err_t tos_bin_heap_pop(k_bin_heap_t *bin_heap, void *item, size_t *item_size);

/**
 * @brief Flush the binary heap.
 * flush the binary heap.
 *
 * @attention None
 *
 * @param[in]   bin_heap    pointer to the handler of the binary heap.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_bin_heap_flush(k_bin_heap_t *bin_heap);

/**
 * @brief Whether the binary heap is empty.
 * Whether the binary heap is empty.
 *
 * @attention None
 *
 * @param[in]   bin_heap    pointer to the handler of the binary heap.
 *
 * @return  whether the binary heap is emtpy.
 * @retval  #0      the binary heap is not empty.
 * @retval  #Not 0  the binary heap is empty.
 */
__API__ int tos_bin_heap_is_empty(k_bin_heap_t *bin_heap);

/**
 * @brief Whether the binary heap is full.
 * Whether the binary heap is full.
 *
 * @attention None
 *
 * @param[in]   bin_heap    pointer to the handler of the binary heap.
 *
 * @return  whether the binary hea is full.
 * @retval  #0      the binary hea is not full.
 * @retval  #Not 0  the binary hea is full.
 */
__API__ int tos_bin_heap_is_full(k_bin_heap_t *bin_heap);

__CDECLS_END

#endif /* _TOS_BINARY_HEAP_H_ */

