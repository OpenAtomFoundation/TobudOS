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

/*
** Two Level Segregated Fit memory allocator, version 3.1.
** Written by Matthew Conte
** http://tlsf.baisoku.org
**
** Based on the original documentation by Miguel Masmano:
** http://www.gii.upv.es/tlsf/main/docs
**
** This implementation was written to the specification
** of the document, therefore no GPL restrictions apply.
**
** Copyright (c) 2006-2016, Matthew Conte
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the copyright holder nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL MATTHEW CONTE BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TOS_MMHEAP_H_
#define  _TOS_MMHEAP_H_

__CDECLS_BEGIN

#if TOS_CFG_MMHEAP_EN > 0u

/**
 * log2 of number of linear subdivisions of block sizes. Larger
 * values require more memory in the control structure. Values of
 * 4 or 5 are typical.
 */
#define K_MMHEAP_SL_INDEX_COUNT_LOG2    5

/* All allocation sizes and addresses are aligned to 4 bytes. */
#define K_MMHEAP_ALIGN_SIZE_LOG2        2
#define K_MMHEAP_ALIGN_SIZE             (1 << K_MMHEAP_ALIGN_SIZE_LOG2)

/*
 * We support allocations of sizes up to (1 << K_MMHEAP_FL_INDEX_MAX) bits.
 * However, because we linearly subdivide the second-level lists, and
 * our minimum size granularity is 4 bytes, it doesn't make sense to
 * create first-level lists for sizes smaller than K_MMHEAP_SL_INDEX_COUNT * 4,
 * or (1 << (K_MMHEAP_SL_INDEX_COUNT_LOG2 + 2)) bytes, as there we will be
 * trying to split size ranges into more slots than we have available.
 * Instead, we calculate the minimum threshold size, and place all
 * blocks below that size into the 0th first-level list.
 */
#define K_MMHEAP_FL_INDEX_MAX           30
#define K_MMHEAP_SL_INDEX_COUNT         (1 << K_MMHEAP_SL_INDEX_COUNT_LOG2)
#define K_MMHEAP_FL_INDEX_SHIFT         (K_MMHEAP_SL_INDEX_COUNT_LOG2 + K_MMHEAP_ALIGN_SIZE_LOG2)
#define K_MMHEAP_FL_INDEX_COUNT         (K_MMHEAP_FL_INDEX_MAX - K_MMHEAP_FL_INDEX_SHIFT + 1)

#define K_MMHEAP_SMALL_BLOCK_SIZE       (1 << K_MMHEAP_FL_INDEX_SHIFT)

#define K_MMHEAP_BLOCK_CURR_FREE        (1 << 0)
#define K_MMHEAP_BLOCK_PREV_FREE        (1 << 1)
#define K_MMHEAP_BLOCK_SIZE_MASK        ~(K_MMHEAP_BLOCK_CURR_FREE | K_MMHEAP_BLOCK_PREV_FREE)
#define K_MMHEAP_BLOCK_STATE_MASK       (K_MMHEAP_BLOCK_CURR_FREE | K_MMHEAP_BLOCK_PREV_FREE)

typedef struct k_mmheap_information_st {
    uint32_t    used; /* space is used */
    uint32_t    free; /* space is free */
} k_mmheap_info_t;

/**
 * Block structure.
 *
 * There are several implementation subtleties involved:
 * - The prev_phys_block field is only valid if the previous block is free.
 * - The prev_phys_block field is actually stored at the end of the
 *   previous block. It appears at the beginning of this structure only to
 *   simplify the implementation.
 * - The next_free / prev_free fields are only valid if the block is free.
 */
typedef struct mmheap_blk_st {
    struct mmheap_blk_st *prev_phys_blk;

    size_t size;

    struct mmheap_blk_st *next_free;
    struct mmheap_blk_st *prev_free;
} mmheap_blk_t;

/**
 * A free block must be large enough to store its header minus the size of
 * the prev_phys_block field, and no larger than the number of addressable
 * bits for FL_INDEX.
 */
#define K_MMHEAP_BLK_SIZE_MIN           (sizeof(mmheap_blk_t) - sizeof(mmheap_blk_t *))
#define K_MMHEAP_BLK_SIZE_MAX           (1 << K_MMHEAP_FL_INDEX_MAX)

#define K_MMHEAP_BLK_HEADER_OVERHEAD    (sizeof(size_t))
#define K_MMHEAP_BLK_START_OFFSET       (TOS_OFFSET_OF_FIELD(mmheap_blk_t, size) + sizeof(size_t))

#define K_MMHEAP_POOL_MAX               3

/**
 * memory heap control
 */
typedef struct k_mmheap_control_st {
    int             pool_cnt;
    void           *pool_start[K_MMHEAP_POOL_MAX];

    mmheap_blk_t    block_null; /**< Empty lists point at this block to indicate they are free. */

    uint32_t        fl_bitmap; /**< Bitmaps for free lists. */
    uint32_t        sl_bitmap[K_MMHEAP_FL_INDEX_COUNT];

    mmheap_blk_t   *blocks[K_MMHEAP_FL_INDEX_COUNT][K_MMHEAP_SL_INDEX_COUNT]; /**< Head of free lists. */
} k_mmheap_ctl_t;

/**
 * @brief Add a pool.
 * Add addtional pool to the heap.
 *
 * @attention None
 *
 * @param[in]   pool_start  start address of the pool.
 * @param[in]   pool_size   size of the pool.
 *
 * @return  errcode
 * @retval  #K_ERR_MMHEAP_INVALID_POOL_ADDR     start address of the pool is invalid.
 * @retval  #K_ERR_MMHEAP_INVALID_POOL_SIZE     size of the pool is invalid.
 * @retval  #K_ERR_MMHEAP_POOL_OVERFLOW         too many pools are added.
 * @retval  #K_ERR_MMHEAP_POOL_ALREADY_EXIST    the pool is already exist.
 * @retval  #K_ERR_NONE                         return successfully.
 */
__API__ k_err_t tos_mmheap_pool_add(void *pool_start, size_t pool_size);

/**
 * @brief Remove a pool.
 * Remove a pool from the heap.
 *
 * @attention None
 *
 * @param[in]   pool_start  start address of the pool.
 *
 * @return  errcode
 * @retval  #K_ERR_OBJ_PTR_NULL             start address of the pool is NULL
 * @retval  #K_ERR_MMHEAP_POOL_NOT_EXIST    the pool is not exist
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_mmheap_pool_rmv(void *pool_start);

/**
 * @brief Alloc memory.
 * Allocate size bytes and returns a pointer to the allocated memory.
 *
 * @attention size should no bigger than K_MMHEAP_BLK_SIZE_MAX.
 *
 * @param[in]   size    size of the memory.
 *
 * @return  the pointer to the allocated memory.
 */
__API__ void   *tos_mmheap_alloc(size_t size);

__API__ void   *tos_mmheap_calloc(size_t num, size_t size);

/**
 * @brief Alloc start address aligned memory from the heap.
 * Alloc aligned address and specified size memory from the heap.
 *
 * @attention
 *
 * @param[in]   size    size of the memory.
 * @param[in]   align   address align mask of the memory.
 *
 * @return  the pointer to the allocated memory.
 */
__API__ void   *tos_mmheap_aligned_alloc(size_t size, size_t align);

/**
 * @brief Realloc memory from the heap.
 * Change the size of the memory block pointed to by ptr to size bytes.
 *
 * @attention
 * <ul>
 * <li> if ptr is K_NULL, then the call is equivalent to tos_mmheap_alloc(size), for all values of size.
 * <li> if ptr is if size is equal to zero, and ptr is not K_NULL, then the call is equivalent to tos_mmheap_free(ptr).
 * </ul>
 *
 * @param[in]   ptr     old pointer to the memory space.
 * @param[in]   size    new size of the memory space.
 *
 * @return  the new pointer to the allocated memory.
 */
__API__ void   *tos_mmheap_realloc(void *ptr, size_t size);

/**
 * @brief Free the memory.
 * Free the memory space pointed to by ptr, which must have been returned by a previous call to tos_mmheap_alloc(), tos_mmheap_aligned_alloc(), or tos_mmheap_realloc().
 *
 * @attention
 *
 * @param[in]   ptr     pointer to the memory.
 *
 * @return  None.
 */
__API__ void    tos_mmheap_free(void *ptr);

/**
 * @brief Check the pool.
 *
 * @attention
 *
 * @param[in]   pool_start  start address of the pool.
 * @param[out]  info        pointer to the information struct.
 *
 * @return  errcode.
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_mmheap_pool_check(void *pool_start, k_mmheap_info_t *info);

/**
 * @brief Check the heap.
 *
 * @attention
 *
 * @param[out]  info        pointer to the information struct.
 *
 * @return  errcode.
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_mmheap_check(k_mmheap_info_t *info);

__KNL__ k_err_t mmheap_init(void);

__KNL__ k_err_t mmheap_init_with_pool(void *pool_start, size_t pool_size);

#endif

__CDECLS_END

#endif /* _TOS_MMHEAP_H_ */

