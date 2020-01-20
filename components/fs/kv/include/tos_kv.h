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

#ifndef _TOS_KV_H_
#define  _TOS_KV_H_

#include "tos_k.h"
#include "tos_kv_err.h"
#include "tos_kv_flash.h"

/* the max length of write unit, DOUBLEWORD, 64-bit
   a better way is to change kv_wunit_t according the the flash program type by precompile,
   if to do this, should let user config the flash program type as a macro, and that is not so friendly.
   we typedef kv_wunit_t as uint64_t, so the user can focus on the code rather than how to use kv.
   (althrough there may cause some waste of flash space, just a little, but much more comfortable for coding)
 */
typedef uint64_t    kv_wunit_t;

typedef uint8_t     kv_byte_t;  // byte
typedef uint16_t    kv_hword_t; // half word
typedef uint32_t    kv_word_t;  // word
typedef uint64_t    kv_dword_t; // double word

/*
           add          delete                  no more space
    fresh -----> inuse --------> dirty | inuse ---------------> dirty

           add          no more space
    fresh -----> inuse ---------------> NONE

    if bad, probably the flash block is broken, no more try to use.
    if hanging, maybe another chance to rebuild index.
    there is a situation: dirty | hanging
*/
#define KV_BLK_FLAG_FRESH                           0x01    /* a totally virgin block */
#define KV_BLK_FLAG_INUSE                           0x02    /* in-use */
#define KV_BLK_FLAG_DIRTY                           0x04    /* has discarded item inside */
#define KV_BLK_FLAG_BAD                             0x08    /* bad block, maybe device error */
#define KV_BLK_FLAG_HANGING                         0x10    /* index building failed, we mark a hanging flag here, and will give another chance to do a retry */

#define KV_FLASH_START                              (kv_ctl.flash_ctl.flash_start)
#define KV_FLASH_SIZE                               (kv_ctl.flash_ctl.flash_size)
#define KV_FLASH_END                                (KV_FLASH_START + KV_FLASH_SIZE)
#define KV_FLASH_SECTOR_SIZE_LOG2                   (kv_ctl.flash_ctl.sector_size_log2)
#define KV_FLASH_SECTOR_SIZE                        (1 << KV_FLASH_SECTOR_SIZE_LOG2)
#define KV_FLASH_WRITE_ALIGN                        (kv_ctl.flash_ctl.flash_write_align)

#define KV_FLASH_WRITE                              ((kv_flash_write_t)(kv_ctl.flash_ctl.flash_drv.write))
#define KV_FLASH_READ                               ((kv_flash_read_t)(kv_ctl.flash_ctl.flash_drv.read))
#define KV_FLASH_ERASE                              ((kv_flash_erase_t)(kv_ctl.flash_ctl.flash_drv.erase))

#define KV_IS_ALINGED(v, align)                     ((v) % (align) == 0)
#define KV_IS_ALINGED_LOG2(v, align_log2)           KV_IS_ALINGED(v, (1 << (align_log2)))
#define KV_ALIGN_UP(v, align)                       (((v) + ((align) - 1)) & ~((align) - 1))
#define KV_ALIGN_DOWN(v, align)                     ((v) - ((v) & ((align) - 1)))
#define KV_ADDR_OF_FIELD(addr, type, field)         (addr + TOS_OFFSET_OF_FIELD(type, field))

#define KV_ALIGNED_SIZE(len)                        KV_ALIGN_UP(len, KV_FLASH_WRITE_ALIGN)

#define KV_MGR_LOCK                                 (&kv_ctl.mgr_ctl.kv_lock)
#define KV_MGR_BLK_NUM_FRESH                        (kv_ctl.mgr_ctl.blk_info.num_fresh)
#define KV_MGR_BLK_NUM_INUSE                        (kv_ctl.mgr_ctl.blk_info.num_inuse)
#define KV_MGR_BLK_NUM_HANGING                      (kv_ctl.mgr_ctl.blk_info.num_hanging)
#define KV_MGR_BLK_NUM_TOTAL                        (kv_ctl.mgr_ctl.blk_info.num_total)
#define KV_MGR_BLK_DETAIL                           (kv_ctl.mgr_ctl.blk_info.blk_detail)
#define KV_MGR_WORKSPACE                            (kv_ctl.mgr_ctl.workspace)

#define KV_NO_WRITEABLE_BLK()                       (KV_MGR_BLK_NUM_INUSE == 0 && KV_MGR_BLK_NUM_FRESH == 0)

#define KV_ITEM_HDR_MAGIC                           0xABCD1234DCBA4321
#define KV_ITEM_DISCARDED                           0x0F0F0F0F0F0F0F0F
#define KV_ITEM_IS_DISCARDED(item_hdr)              ((item_hdr)->discarded_flag == KV_ITEM_DISCARDED)
#define KV_ITEM_IS_LEGAL(item_hdr)                  ((item_hdr)->magic == KV_ITEM_HDR_MAGIC)
#define KV_ITEM_IS_FRESH(item_hdr)                  ((item_hdr)->magic == (kv_wunit_t)-1 && \
                                                        (item_hdr)->discarded_flag == (kv_wunit_t)-1 && \
                                                        (item_hdr)->checksum == (uint8_t)-1 && \
                                                        (item_hdr)->k_len == (uint8_t)-1 && \
                                                        (item_hdr)->v_len == (uint16_t)-1 && \
                                                        (item_hdr)->prev_pos == (uint32_t)-1)

#define KV_ITEM_ADDR2BLK(item_start)                (KV_ALIGN_DOWN((item_start) - KV_FLASH_START, KV_BLK_SIZE) + KV_FLASH_START)
#define KV_ITEM_HDR_SIZE                            KV_ALIGNED_SIZE(sizeof(kv_item_hdr_t))
#define KV_ITEM_BODY_SIZE(k_len, v_len)             KV_ALIGNED_SIZE(k_len + v_len)
#define KV_ITEM_SIZE(k_len, v_len)                  (KV_ITEM_HDR_SIZE + KV_ITEM_BODY_SIZE(k_len, v_len))
#define KV_ITEM_SIZE_OF_ITEM(item)                  KV_ITEM_SIZE(item->hdr.k_len, item->hdr.v_len)
#define KV_ITEM_SIZE_OF_BODY(item)                  KV_ITEM_BODY_SIZE(item->hdr.k_len, item->hdr.v_len)
#define KV_ITEM_ADDR_OF_BODY(item)                  (item->pos + KV_ITEM_HDR_SIZE)

#define KV_BLK_HDR_MAGIC                            0x1234ABCD4321DCBA
#define KV_BLK_IS_LEGAL(blk_hdr)                    ((blk_hdr)->magic == KV_BLK_HDR_MAGIC)
#define KV_BLK_INVALID                              ((uint32_t)-1)
#define KV_BLK_HDR_SIZE                             KV_ALIGNED_SIZE(sizeof(kv_blk_hdr_t))
#define KV_BLK_SIZE                                 (KV_FLASH_SECTOR_SIZE)
#define KV_BLK_FRESH_SIZE                           (KV_BLK_SIZE - KV_BLK_HDR_SIZE)
#define KV_BLK_END(blk_start)                       (blk_start + KV_BLK_SIZE)
#define KV_BLK_USABLE_ADDR(blk_start)               (KV_BLK_END(blk_start) - kv_blk_freesz_get(blk_start))
#define KV_BLK_ADDR2IDX(blk_start)                  ((blk_start - KV_FLASH_START) / KV_BLK_SIZE)
#define KV_BLK_FIRST_ITEM(blk_start)                (blk_start + KV_BLK_HDR_SIZE)
#define KV_BLK_NEXT(blk_start)                      (blk_start + KV_BLK_SIZE >= KV_FLASH_END ? KV_FLASH_START : blk_start + KV_BLK_SIZE)

#define KV_BLK_FOR_EACH_FROM(cur_blk, start_blk) \
	for (cur_blk = KV_BLK_NEXT(start_blk); \
            cur_blk != start_blk; \
            cur_blk = KV_BLK_NEXT(cur_blk))

#define KV_BLK_FOR_EACH(cur_blk) \
    for (cur_blk = KV_FLASH_START; \
            cur_blk < KV_FLASH_END; \
            cur_blk += KV_BLK_SIZE)

typedef struct kv_flash_control_st {
    uint8_t         sector_size_log2;
    uint8_t         flash_write_align;
    uint32_t        flash_start;
    uint32_t        flash_size;

    kv_flash_drv_t  flash_drv;
} kv_flash_ctl_t;

typedef struct kv_blk_detail_st {
    uint8_t         blk_flags;      /*< flags indicating the status of the blk, see KV_BLK_FLAG_* */
    uint32_t        free_size;      /*< how many usable flash left */
} kv_blk_detail_t;

typedef struct kv_blk_info_st {
    uint16_t            num_inuse;
    uint16_t            num_fresh;
    uint16_t            num_hanging;
    uint16_t            num_total;

    kv_blk_detail_t    *blk_detail;
} kv_blk_info_t;

typedef struct kv_manager_control_st {
    uint32_t        workspace;
    kv_blk_info_t   blk_info;

    k_mutex_t       kv_lock;
} kv_mgr_ctl_t;

typedef struct kv_control_st {
    kv_flash_ctl_t  flash_ctl;
    kv_mgr_ctl_t    mgr_ctl;
} kv_ctl_t;

typedef struct kv_block_header_st {
    kv_wunit_t      magic;          /*< is this block formatted? */
} __PACKED__ kv_blk_hdr_t;

typedef struct kv_item_header_st {
    kv_wunit_t      discarded_flag; /*< is this item a discarded one, deleted or updated */
    kv_wunit_t      magic;          /*< for item header integrity verification */
    uint8_t         checksum;       /*< checksum for key/value buffer */
    uint8_t         k_len;          /*< key length */
    uint16_t        v_len;          /*< value length */
    uint32_t        prev_pos;       /*< previous position of this item
                                        if meet a power down while updating an item, and we have saved the new item,
                                        but do not get a chance to delete the old one(we have two copy of the item with
                                        the same key on flash), we should do the real delete when system is power on next time */
} __PACKED__ kv_item_hdr_t;

typedef struct kv_item_st {
    kv_item_hdr_t   hdr;        /*< item header */
    uint32_t        pos;        /*< where this item is */
    uint8_t        *body;       /*< item body: key/value buffer */
} kv_item_t;

__STATIC__ kv_ctl_t kv_ctl;

__STATIC_INLINE__ void kv_blk_freesz_set(uint32_t blk_start, uint32_t free_size)
{
    KV_MGR_BLK_DETAIL[KV_BLK_ADDR2IDX(blk_start)].free_size = free_size;
}

__STATIC_INLINE__ uint32_t kv_blk_freesz_get(uint32_t blk_start)
{
    return KV_MGR_BLK_DETAIL[KV_BLK_ADDR2IDX(blk_start)].free_size;
}

__STATIC_INLINE__ void kv_blk_freesz_reduce(uint32_t blk_start, uint32_t size_reduced)
{
    kv_blk_freesz_set(blk_start, kv_blk_freesz_get(blk_start) - size_reduced);
}

__STATIC_INLINE__ int kv_blk_is_full(uint32_t blk_start)
{
    return kv_blk_freesz_get(blk_start) <= KV_ITEM_HDR_SIZE;
}

__STATIC_INLINE__ void kv_blk_flags_set(uint32_t blk_start, uint8_t blk_flags)
{
    KV_MGR_BLK_DETAIL[KV_BLK_ADDR2IDX(blk_start)].blk_flags = blk_flags;
}

__STATIC_INLINE__ uint8_t kv_blk_flags_get(uint32_t blk_start)
{
    return KV_MGR_BLK_DETAIL[KV_BLK_ADDR2IDX(blk_start)].blk_flags;
}

__STATIC_INLINE__ void kv_blk_flags_add(uint32_t blk_start, uint8_t blk_flags)
{
    KV_MGR_BLK_DETAIL[KV_BLK_ADDR2IDX(blk_start)].blk_flags |= blk_flags;
}

__STATIC_INLINE__ void kv_blk_flags_rmv(uint32_t blk_start, uint8_t blk_flags)
{
    KV_MGR_BLK_DETAIL[KV_BLK_ADDR2IDX(blk_start)].blk_flags &= ~blk_flags;
}

__STATIC_INLINE__ int kv_blk_is_fresh(uint32_t blk_start)
{
    return kv_blk_flags_get(blk_start) & KV_BLK_FLAG_FRESH;
}

__STATIC_INLINE__ int kv_blk_is_inuse(uint32_t blk_start)
{
    return kv_blk_flags_get(blk_start) & KV_BLK_FLAG_INUSE;
}

__STATIC_INLINE__ int kv_blk_is_dirty(uint32_t blk_start)
{
    return kv_blk_flags_get(blk_start) & KV_BLK_FLAG_DIRTY;
}

__STATIC_INLINE__ int kv_blk_is_bad(uint32_t blk_start)
{
    return kv_blk_flags_get(blk_start) & KV_BLK_FLAG_BAD;
}

__STATIC_INLINE__ int kv_blk_is_hanging(uint32_t blk_start)
{
    return kv_blk_flags_get(blk_start) & KV_BLK_FLAG_HANGING;
}

__STATIC_INLINE__ void kv_blk_set_fresh(uint32_t blk_start)
{
    if (!kv_blk_is_fresh(blk_start)) {
        ++KV_MGR_BLK_NUM_FRESH;
    }

    kv_blk_freesz_set(blk_start, KV_BLK_FRESH_SIZE);
    kv_blk_flags_set(blk_start, KV_BLK_FLAG_FRESH);
}

__STATIC_INLINE__ void kv_blk_set_inuse(uint32_t blk_start)
{
    if (!kv_blk_is_inuse(blk_start)) {
        ++KV_MGR_BLK_NUM_INUSE;
    }

    kv_blk_flags_add(blk_start, KV_BLK_FLAG_INUSE);
}

__STATIC_INLINE__ void kv_blk_set_dirty(uint32_t blk_start)
{
    kv_blk_flags_add(blk_start, KV_BLK_FLAG_DIRTY);
}

__STATIC_INLINE__ void kv_blk_set_bad(uint32_t blk_start)
{
    kv_blk_flags_set(blk_start, KV_BLK_FLAG_BAD);
}

__STATIC_INLINE__ void kv_blk_set_hanging(uint32_t blk_start)
{
    if (!kv_blk_is_hanging(blk_start)) {
        ++KV_MGR_BLK_NUM_HANGING;
    }

    kv_blk_flags_add(blk_start, KV_BLK_FLAG_HANGING);
}

__STATIC_INLINE__ void kv_blk_reset_fresh(uint32_t blk_start)
{
    if (kv_blk_is_fresh(blk_start)) {
        --KV_MGR_BLK_NUM_FRESH;
    }

    kv_blk_flags_rmv(blk_start, KV_BLK_FLAG_FRESH);
}

__STATIC_INLINE__ void kv_blk_reset_inuse(uint32_t blk_start)
{
    if (kv_blk_is_inuse(blk_start)) {
        --KV_MGR_BLK_NUM_INUSE;
    }

    kv_blk_flags_rmv(blk_start, KV_BLK_FLAG_INUSE);
}

__STATIC_INLINE__ void kv_blk_reset_hanging(uint32_t blk_start)
{
    if (kv_blk_is_hanging(blk_start)) {
        --KV_MGR_BLK_NUM_HANGING;
    }

    kv_blk_flags_rmv(blk_start, KV_BLK_FLAG_HANGING);
}

typedef kv_err_t (*kv_item_walker_t)(kv_item_t *item, const void *patten);

__API__ kv_err_t    tos_kv_init(kv_flash_drv_t *flash_drv, kv_flash_prop_t *flash_prop);

__API__ kv_err_t    tos_kv_deinit(void);

__API__ kv_err_t    tos_kv_set(const char *key, const void *value, size_t v_len);

__API__ kv_err_t    tos_kv_get(const char *key, void *value_buf, size_t value_buf_size, size_t *v_len);

__API__ int         tos_kv_has_key(const char *key);

__API__ kv_err_t    tos_kv_del(const char *key);

__DEBUG__ kv_err_t  tos_kv_walkthru(void);

__STATIC__ kv_err_t kv_gc(void);

#endif /* _TOS_KV_H_ */

