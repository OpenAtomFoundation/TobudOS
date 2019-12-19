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
**	http://tlsf.baisoku.org
**
** Based on the original documentation by Miguel Masmano:
**	http://www.gii.upv.es/tlsf/main/docs
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

#include <tos.h>

#if TOS_CFG_MMHEAP_EN > 0u

#if defined(TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT) && (TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT == 0u)
__STATIC__ int generic_fls(uint32_t x)
{
    int r = 32;
    if (!x)
        return 0;

    if (!(x & 0xffff0000u)) {
        x <<= 16;
        r -= 16;
    }
    if (!(x & 0xff000000u)) {
        x <<= 8;
        r -= 8;
    }
    if (!(x & 0xf0000000u)) {
        x <<= 4;
        r -= 4;
    }
    if (!(x & 0xc0000000u)) {
        x <<= 2;
        r -= 2;
    }
    if (!(x & 0x80000000u)) {
        x <<= 1;
        r -= 1;
    }
    return r;
}
#else
__STATIC__ int generic_fls(uint32_t x)
{
    return 32 - tos_cpu_clz(x);
}
#endif

__STATIC__ int __ffs(uint32_t word)
{
    return generic_fls(word & (~word + 1)) - 1;
}

__STATIC__ int __fls(uint32_t word)
{
    return generic_fls(word) - 1;
}

/*
** TLSF utility functions. In most cases, these are direct translations of
** the documentation found in the white paper.
*/
__STATIC__ void mapping_insert(size_t size, int *fli, int *sli)
{
    int fl, sl;

    if (size < K_MMHEAP_SMALL_BLOCK_SIZE) {
        /* Store small blocks in first list. */
        fl = 0;
        sl = (int)size / (K_MMHEAP_SMALL_BLOCK_SIZE / K_MMHEAP_SL_INDEX_COUNT);
    } else {
        fl = __fls(size);
        sl = ((int)size >> (fl - K_MMHEAP_SL_INDEX_COUNT_LOG2)) ^ (1 << K_MMHEAP_SL_INDEX_COUNT_LOG2);
        fl -= (K_MMHEAP_FL_INDEX_SHIFT - 1);
    }
    *fli = fl;
    *sli = sl;
}

/* This version rounds up to the next block size (for allocations) */
__STATIC__ void mapping_search(size_t size, int *fli, int *sli)
{
    size_t round;

    if (size >= K_MMHEAP_SMALL_BLOCK_SIZE) {
        round = (1 << (__fls(size) - K_MMHEAP_SL_INDEX_COUNT_LOG2)) - 1;
        size += round;
    }
    mapping_insert(size, fli, sli);
}

__STATIC_INLINE__ size_t blk_size(const mmheap_blk_t *blk)
{
    return blk->size & K_MMHEAP_BLOCK_SIZE_MASK;
}

__STATIC_INLINE__ int blk_is_last(const mmheap_blk_t* blk)
{
	return blk_size(blk) == 0;
}

__STATIC_INLINE__ void blk_set_size(mmheap_blk_t *blk, size_t size)
{
    blk->size = size | (blk->size & K_MMHEAP_BLOCK_STATE_MASK);
}

__STATIC_INLINE__ int blk_is_free(const mmheap_blk_t *blk)
{
    return blk->size & K_MMHEAP_BLOCK_CURR_FREE;
}

__STATIC_INLINE__ void blk_set_free(mmheap_blk_t *blk)
{
    blk->size |= K_MMHEAP_BLOCK_CURR_FREE;
}

__STATIC_INLINE__ void blk_set_used(mmheap_blk_t *blk)
{
    blk->size &= ~K_MMHEAP_BLOCK_CURR_FREE;
}

__STATIC_INLINE__ int blk_is_prev_free(const mmheap_blk_t *blk)
{
    return blk->size & K_MMHEAP_BLOCK_PREV_FREE;
}

__STATIC_INLINE__ void blk_set_prev_free(mmheap_blk_t *blk)
{
    blk->size |= K_MMHEAP_BLOCK_PREV_FREE;
}

__STATIC__ void blk_set_prev_used(mmheap_blk_t *blk)
{
    blk->size &= ~K_MMHEAP_BLOCK_PREV_FREE;
}

__STATIC_INLINE__ mmheap_blk_t *blk_from_ptr(const void *ptr)
{
    return (mmheap_blk_t *)((cpu_addr_t)ptr - K_MMHEAP_BLK_START_OFFSET);
}

__STATIC_INLINE__ void *blk_to_ptr(const mmheap_blk_t *blk)
{
    return (void *)((cpu_addr_t)blk + K_MMHEAP_BLK_START_OFFSET);
}

/* Return location of next block after block of given size. */
__STATIC_INLINE__ mmheap_blk_t *offset_to_blk(const void *ptr, int diff)
{
    return (mmheap_blk_t *)((cpu_addr_t)ptr + diff);
}

/* Return location of previous block. */
__STATIC_INLINE__ mmheap_blk_t *blk_prev(const mmheap_blk_t *blk)
{
    return blk->prev_phys_blk;
}

/* Return location of next existing block. */
__STATIC__ mmheap_blk_t *blk_next(const mmheap_blk_t *blk)
{
    mmheap_blk_t *next_blk;

    next_blk = offset_to_blk(blk_to_ptr(blk), blk_size(blk) - K_MMHEAP_BLK_HEADER_OVERHEAD);
    return next_blk;
}

/* Link a new block with its physical neighbor, return the neighbor. */
__STATIC__ mmheap_blk_t *blk_link_next(mmheap_blk_t *blk)
{
    mmheap_blk_t *next_blk;

    next_blk = blk_next(blk);
    next_blk->prev_phys_blk = blk;
    return next_blk;
}

__STATIC__ void blk_mark_as_free(mmheap_blk_t *blk)
{
    mmheap_blk_t *next_blk;

    /* Link the block to the next block, first. */
    next_blk = blk_link_next(blk);
    blk_set_prev_free(next_blk);
    blk_set_free(blk);
}

__STATIC__ void blk_mark_as_used(mmheap_blk_t *blk)
{
    mmheap_blk_t *next_blk;

    next_blk = blk_next(blk);
    blk_set_prev_used(next_blk);
    blk_set_used(blk);
}

__STATIC_INLINE__ size_t align_up(size_t x, size_t align)
{
    return (x + (align - 1)) & ~(align - 1);
}

__STATIC_INLINE__ size_t align_down(size_t x, size_t align)
{
    return x - (x & (align - 1));
}

__STATIC_INLINE__ void *align_ptr(const void *ptr, size_t align)
{
    return (void *)(((cpu_addr_t)ptr + (align -1)) & ~(align -1));
}

/* Insert a free block into the free block list. */
__STATIC__ void insert_free_block(mmheap_blk_t *blk, int fl, int sl)
{
    mmheap_blk_t *curr;

    curr = k_mmheap_ctl.blocks[fl][sl];
    blk->next_free = curr;
    blk->prev_free = &k_mmheap_ctl.block_null;
    curr->prev_free = blk;

	/*
	** Insert the new block at the head of the list, and mark the first-
	** and second-level bitmaps appropriately.
	*/
    k_mmheap_ctl.blocks[fl][sl] = blk;
    k_mmheap_ctl.fl_bitmap |= (1 << fl);
    k_mmheap_ctl.sl_bitmap[fl] |= (1 << sl);
}

/* Remove a free block from the free list.*/
__STATIC__ void remove_free_block(mmheap_blk_t *blk, int fl, int sl)
{
    mmheap_blk_t *prev_blk;
    mmheap_blk_t *next_blk;

    prev_blk = blk->prev_free;
    next_blk = blk->next_free;
    next_blk->prev_free = prev_blk;
    prev_blk->next_free = next_blk;

    /* If this block is the head of the free list, set new head. */
    if (k_mmheap_ctl.blocks[fl][sl] == blk) {
        k_mmheap_ctl.blocks[fl][sl] = next_blk;

        /* If the new head is null, clear the bitmap. */
        if (next_blk == &k_mmheap_ctl.block_null) {
            k_mmheap_ctl.sl_bitmap[fl] &= ~(1 << sl);

            /* If the second bitmap is now empty, clear the fl bitmap. */
            if (!k_mmheap_ctl.sl_bitmap[fl]) {
                k_mmheap_ctl.fl_bitmap &= ~(1 << fl);
            }
        }
    }
}

/* Remove a given block from the free list. */
__STATIC__ void blk_remove(mmheap_blk_t *blk)
{
    int fl, sl;

    mapping_insert(blk_size(blk), &fl, &sl);
    remove_free_block(blk, fl, sl);
}

/* Insert a given block into the free list. */
__STATIC__ void blk_insert(mmheap_blk_t *blk)
{
    int fl, sl;

    mapping_insert(blk_size(blk), &fl, &sl);
    insert_free_block(blk, fl, sl);
}

__STATIC__ int blk_can_split(mmheap_blk_t *blk, size_t size)
{
    return blk_size(blk) >= sizeof(mmheap_blk_t) + size;
}

/* Split a block into two, the second of which is free. */
__STATIC__ mmheap_blk_t *blk_split(mmheap_blk_t *blk, size_t size)
{
    mmheap_blk_t   *remaining;
    size_t          remain_size;

    /* Calculate the amount of space left in the remaining block. */
    remaining   = offset_to_blk(blk_to_ptr(blk), size - K_MMHEAP_BLK_HEADER_OVERHEAD);
    remain_size = blk_size(blk) - (size + K_MMHEAP_BLK_HEADER_OVERHEAD);

    blk_set_size(remaining, remain_size);

    blk_set_size(blk, size);
    blk_mark_as_free(remaining);

    return remaining;
}

/* Absorb a free block's storage into an adjacent previous free block. */
__STATIC__ mmheap_blk_t *blk_absorb(mmheap_blk_t *prev_blk, mmheap_blk_t *blk)
{
    prev_blk->size += blk_size(blk) + K_MMHEAP_BLK_HEADER_OVERHEAD;
    blk_link_next(prev_blk);
    return prev_blk;
}

/* Merge a just-freed block with an adjacent previous free block. */
__STATIC__ mmheap_blk_t *blk_merge_prev(mmheap_blk_t *blk)
{
    mmheap_blk_t *prev_blk;

    if (blk_is_prev_free(blk)) {
        prev_blk = blk_prev(blk);
        blk_remove(prev_blk);
        blk = blk_absorb(prev_blk, blk);
    }

    return blk;
}

/* Merge a just-freed block with an adjacent free block. */
__STATIC__ mmheap_blk_t *blk_merge_next(mmheap_blk_t *blk)
{
    mmheap_blk_t *next_blk;

    next_blk = blk_next(blk);
    if (blk_is_free(next_blk)) {
        blk_remove(next_blk);
        blk = blk_absorb(blk, next_blk);
    }

    return blk;
}

/* Trim any trailing block space off the end of a block, return to pool. */
__STATIC__ void blk_trim_free(mmheap_blk_t *blk, size_t size)
{
    mmheap_blk_t *remaining_blk;

    if (blk_can_split(blk, size)) {
        remaining_blk = blk_split(blk, size);
        blk_link_next(blk);
        blk_set_prev_free(remaining_blk);
        blk_insert(remaining_blk);
    }
}

/* Trim any trailing block space off the end of a used block, return to pool. */
__STATIC__ void blk_trim_used(mmheap_blk_t *blk, size_t size)
{
    mmheap_blk_t *remaining_blk;

    if (blk_can_split(blk, size)) {
        /* If the next block is free, we must coalesce. */
        remaining_blk = blk_split(blk, size);
        blk_set_prev_used(remaining_blk);

        remaining_blk = blk_merge_next(remaining_blk);
        blk_insert(remaining_blk);
    }
}

__STATIC__ mmheap_blk_t *blk_trim_free_leading(mmheap_blk_t *blk, size_t size)
{
    mmheap_blk_t *remaining_blk;

    remaining_blk = blk;
    if (blk_can_split(blk, size)) {
        /* We want the 2nd block. */
        remaining_blk = blk_split(blk, size - K_MMHEAP_BLK_HEADER_OVERHEAD);
        blk_set_prev_free(remaining_blk);

        blk_link_next(blk);
        blk_insert(blk);
    }

    return remaining_blk;
}

__STATIC__ mmheap_blk_t *blk_search_suitable(int *fli, int *sli)
{
    int fl, sl;
    uint32_t sl_map, fl_map;

    fl = *fli;
    sl = *sli;

	/*
	** First, search for a block in the list associated with the given
	** fl/sl index.
	*/
    sl_map = k_mmheap_ctl.sl_bitmap[fl] & (~0U << sl);
    if (!sl_map) {
        /* No block exists. Search in the next largest first-level list. */
        fl_map = k_mmheap_ctl.fl_bitmap & (~0U << (fl + 1));
        if (!fl_map) {
            /* No free blocks available, memory has been exhausted. */
            return 0;
        }

        fl = __ffs(fl_map);
        *fli = fl;
        sl_map = k_mmheap_ctl.sl_bitmap[fl];
    }
    sl = __ffs(sl_map);
    *sli = sl;

    /* Return the first block in the free list. */
    return k_mmheap_ctl.blocks[fl][sl];
}

__STATIC__ mmheap_blk_t *blk_locate_free(size_t size)
{
    int fl = 0, sl = 0;
    mmheap_blk_t *blk = K_NULL;

    if (!size) {
        return K_NULL;
    }

    mapping_search(size, &fl, &sl);

    /*
    ** mapping_search can futz with the size, so for excessively large sizes it can sometimes wind up
    ** with indices that are off the end of the block array.
    ** So, we protect against that here, since this is the only callsite of mapping_search.
    ** Note that we don't need to check sl, since it comes from a modulo operation that guarantees it's always in range.
    */
    if (fl < K_MMHEAP_FL_INDEX_COUNT) {
        blk = blk_search_suitable(&fl, &sl);
    }

    if (blk) {
        remove_free_block(blk, fl, sl);
    }

    return blk;
}

/*
** Adjust an allocation size to be aligned to word size, and no smaller
** than internal minimum.
*/
__STATIC__ size_t adjust_request_size(size_t size, size_t align)
{
    size_t adjust_size = 0;

    if (!size) {
        return 0;
    }

    adjust_size = align_up(size, align);
    if (adjust_size > K_MMHEAP_BLK_SIZE_MAX) {
        return 0;
    }

    /* aligned sized must not exceed block_size_max or we'll go out of bounds on sl_bitmap */
    return adjust_size > K_MMHEAP_BLK_SIZE_MIN ? adjust_size : K_MMHEAP_BLK_SIZE_MIN;
}

__STATIC__ void *blk_prepare_used(mmheap_blk_t *blk, size_t size)
{
    if (!blk) {
        return K_NULL;
    }
    blk_trim_free(blk, size);
    blk_mark_as_used(blk);
    return blk_to_ptr(blk);
}

__STATIC_INLINE__ int mmheap_pool_is_full(void)
{
    return k_mmheap_ctl.pool_cnt == K_MMHEAP_POOL_MAX;
}

__STATIC__ int mmheap_pool_is_exist(void *pool_start)
{
    int i = 0;

    for (i = 0; i < k_mmheap_ctl.pool_cnt; ++i) {
        if (k_mmheap_ctl.pool_start[i] == pool_start) {
            return K_TRUE;
        }
    }
    return K_FALSE;
}

__STATIC_INLINE__ void mmheap_pool_record(void *pool_start)
{
    k_mmheap_ctl.pool_start[k_mmheap_ctl.pool_cnt++] = pool_start;
}

__STATIC__ void mmheap_pool_unrecord(void *pool_start)
{
    int i = 0;

    for (i = 0; i < k_mmheap_ctl.pool_cnt; ++i) {
        if (k_mmheap_ctl.pool_start[i] == pool_start) {
            break;
        }
    }
    if (i != k_mmheap_ctl.pool_cnt - 1) {
        k_mmheap_ctl.pool_start[i] = k_mmheap_ctl.pool_start[k_mmheap_ctl.pool_cnt - 1];
    }
    --k_mmheap_ctl.pool_cnt;
}

__STATIC__ void mmheap_ctl_init(void)
{
    int i, j;

    k_mmheap_ctl.pool_cnt = 0u;
    for (i = 0; i < K_MMHEAP_POOL_MAX; ++i) {
        k_mmheap_ctl.pool_start[i] = (void *)K_NULL;
    }

    k_mmheap_ctl.block_null.next_free = &k_mmheap_ctl.block_null;
    k_mmheap_ctl.block_null.prev_free = &k_mmheap_ctl.block_null;

    k_mmheap_ctl.fl_bitmap = 0;
    for (i = 0; i < K_MMHEAP_FL_INDEX_COUNT; ++i) {
        k_mmheap_ctl.sl_bitmap[i] = 0;
        for (j = 0; j < K_MMHEAP_SL_INDEX_COUNT; ++j) {
            k_mmheap_ctl.blocks[i][j] = &k_mmheap_ctl.block_null;
        }
    }
}

__KERNEL__ k_err_t mmheap_init(void)
{
    mmheap_ctl_init();
    return K_ERR_NONE;
}

__KERNEL__ k_err_t mmheap_init_with_pool(void *pool_start, size_t pool_size)
{
    mmheap_ctl_init();

    return tos_mmheap_pool_add(pool_start, pool_size);
}

__API__ void *tos_mmheap_alloc(size_t size)
{
    size_t          adjust_size;
    mmheap_blk_t   *blk;

    adjust_size     = adjust_request_size(size, K_MMHEAP_ALIGN_SIZE);
    blk             = blk_locate_free(adjust_size);
    if (!blk) {
        return K_NULL;
    }

    return blk_prepare_used(blk, adjust_size);
}

__API__ void *tos_mmheap_calloc(size_t num, size_t size)
{
    void *ptr;

    ptr = tos_mmheap_alloc(num * size);
    if (ptr) {
        memset(ptr, 0, num * size);
    }

    return ptr;
}

__API__ void *tos_mmheap_aligned_alloc(size_t size, size_t align)
{
    mmheap_blk_t *blk;
    void *ptr, *aligned, *next_aligned;
    size_t adjust_size, aligned_size;
    size_t gap_minimum, size_with_gap, gap, gap_remain, offset;

    adjust_size     = adjust_request_size(size, K_MMHEAP_ALIGN_SIZE);
    gap_minimum     = sizeof(mmheap_blk_t);
    size_with_gap   = adjust_request_size(adjust_size + align + gap_minimum, align);
    aligned_size    = (adjust_size && align > K_MMHEAP_ALIGN_SIZE) ? size_with_gap : adjust_size;

    blk = blk_locate_free(aligned_size);
    if (!blk) {
        return K_NULL;
    }

    ptr = blk_to_ptr(blk);
    aligned = align_ptr(ptr, align);
    gap = (size_t)((cpu_addr_t)aligned - (cpu_addr_t)ptr);

    if (gap && gap < gap_minimum) {
        gap_remain = gap_minimum - gap;
        offset = gap_remain > align ? gap_remain : align;
        next_aligned = (void *)((cpu_data_t)aligned + offset);

        aligned = align_ptr(next_aligned, align);
        gap = (size_t)((cpu_addr_t)aligned - (cpu_addr_t)ptr);
    }

    if (gap) {
        blk = blk_trim_free_leading(blk, gap);
    }

    return blk_prepare_used(blk, adjust_size);
}

__API__ void tos_mmheap_free(void *ptr)
{
    mmheap_blk_t *blk;

    if (!ptr) {
        return;
    }

    blk = blk_from_ptr(ptr);
    blk_mark_as_free(blk);
    blk = blk_merge_prev(blk);
    blk = blk_merge_next(blk);
    blk_insert(blk);
}

__API__ void *tos_mmheap_realloc(void *ptr, size_t size)
{
    void *p = 0;
    mmheap_blk_t *curr_blk, *next_blk;
    size_t curr_size, combined_size, adjust_size, min_size;

    if (ptr && size == 0) {
        tos_mmheap_free(ptr);
        return K_NULL;
    }

    if (!ptr) {
        return tos_mmheap_alloc(size);
    }

    curr_blk = blk_from_ptr(ptr);
    next_blk = blk_next(curr_blk);

    curr_size = blk_size(curr_blk);
    combined_size = curr_size + blk_size(next_blk) + K_MMHEAP_BLK_HEADER_OVERHEAD;
    adjust_size = adjust_request_size(size, K_MMHEAP_ALIGN_SIZE);

    if (adjust_size > curr_size && (!blk_is_free(next_blk) || adjust_size > combined_size)) {
        p = tos_mmheap_alloc(size);
        if (p) {
            min_size = curr_size < size ? curr_size : size;
            memcpy(p, ptr, min_size);
            tos_mmheap_free(ptr);
        }
    } else {
        if (adjust_size > curr_size) {
            blk_merge_next(curr_blk);
            blk_mark_as_used(curr_blk);
        }

        blk_trim_used(curr_blk, adjust_size);
        p = ptr;
    }

    return p;
}

__API__ k_err_t tos_mmheap_pool_add(void *pool_start, size_t pool_size)
{
    mmheap_blk_t   *curr_blk;
    mmheap_blk_t   *next_blk;
    size_t          size_aligned;

    if (mmheap_pool_is_full()) {
        return K_ERR_MMHEAP_POOL_OVERFLOW;
    }

    if (mmheap_pool_is_exist(pool_start)) {
        return K_ERR_MMHEAP_POOL_ALREADY_EXIST;
    }

    size_aligned = align_down(pool_size - 2 * K_MMHEAP_BLK_HEADER_OVERHEAD, K_MMHEAP_ALIGN_SIZE);

    if (((cpu_addr_t)pool_start % K_MMHEAP_ALIGN_SIZE) != 0u) {
        return K_ERR_MMHEAP_INVALID_POOL_ADDR;
    }

    if (size_aligned < K_MMHEAP_BLK_SIZE_MIN ||
        size_aligned > K_MMHEAP_BLK_SIZE_MAX) {
        return K_ERR_MMHEAP_INVALID_POOL_SIZE;
    }

    /*
     ** Create the main free block. Offset the start of the block slightly
     ** so that the prev_phys_block field falls outside of the pool -
     ** it will never be used.
     */
    curr_blk = offset_to_blk(pool_start, -K_MMHEAP_BLK_HEADER_OVERHEAD);
    blk_set_size(curr_blk, size_aligned);
    blk_set_free(curr_blk);
    blk_set_prev_used(curr_blk);
    blk_insert(curr_blk);

    /* Split the block to create a zero-size sentinel block. */
    next_blk = blk_link_next(curr_blk);
    blk_set_size(next_blk, 0);
    blk_set_used(next_blk);
    blk_set_prev_free(next_blk);

    mmheap_pool_record(pool_start);

    return K_ERR_NONE;
}

__API__ k_err_t tos_mmheap_pool_rmv(void *pool_start)
{
    int fl = 0, sl = 0;
    mmheap_blk_t *blk;

    TOS_PTR_SANITY_CHECK(pool_start);

    if (!mmheap_pool_is_exist(pool_start)) {
        return K_ERR_MMHEAP_POOL_NOT_EXIST;
    }

    blk = offset_to_blk(pool_start, -K_MMHEAP_BLK_HEADER_OVERHEAD);
    mapping_insert(blk_size(blk), &fl, &sl);
    remove_free_block(blk, fl, sl);

    mmheap_pool_unrecord(pool_start);
    return K_ERR_NONE;
}

__API__ k_err_t tos_mmheap_pool_check(void *pool_start, k_mmheap_info_t *info)
{
    mmheap_blk_t* blk;

    TOS_PTR_SANITY_CHECK(pool_start);
    TOS_PTR_SANITY_CHECK(info);

    memset(info, 0, sizeof(k_mmheap_info_t));

    blk = offset_to_blk(pool_start, -K_MMHEAP_BLK_HEADER_OVERHEAD);

    while (blk && !blk_is_last(blk)) {
        if (blk_is_free(blk)) {
            info->free += blk_size(blk);
        } else {
            info->used += blk_size(blk);
        }
        blk = blk_next(blk);
    }

    return K_ERR_NONE;
}

__API__ k_err_t tos_mmheap_check(k_mmheap_info_t *info)
{
    int i;
    k_err_t err;
    k_mmheap_info_t pool_info;

    TOS_PTR_SANITY_CHECK(info);

    memset(info, 0, sizeof(k_mmheap_info_t));

    for (i = 0; i < k_mmheap_ctl.pool_cnt; ++i) {
        err = tos_mmheap_pool_check(k_mmheap_ctl.pool_start[i], &pool_info);
        if (err != K_ERR_NONE) {
            return err;
        }

        info->free += pool_info.free;
        info->used += pool_info.used;
    }

    return K_ERR_NONE;
}

#endif

