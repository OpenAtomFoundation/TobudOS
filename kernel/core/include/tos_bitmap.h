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

#ifndef _TOS_BITMAP_H_
#define  _TOS_BITMAP_H_

typedef uint32_t                        k_bmtbl_t;

typedef struct k_bitmap_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t       knl_obj;
#endif

    uint32_t        bit_ndx_max;
    uint32_t        bit_max;
    k_bmtbl_t      *bitmap_tbl;
} k_bitmap_t;

#define K_BITMAP_SLOT_SIZE              (sizeof(k_bmtbl_t) * 8) /* in bits */

#define K_BITMAP_TBL_SIZE(bit_max)      ((bit_max + K_BITMAP_SLOT_SIZE - 1) / K_BITMAP_SLOT_SIZE)

#define K_BITMAP_NDX(bit)               ((bit) >> 5u)   /* bit / K_BITMAP_SLOT_SIZE */

#define K_BITMAP_BIT(bit)               ((uint32_t)1u << (K_BITMAP_SLOT_SIZE - 1u - ((bit) & (K_BITMAP_SLOT_SIZE - 1u))))

#define TOS_BITMAP_SIZE(bit_max)        (K_BITMAP_TBL_SIZE(bit_max))

/**
 * @brief Create a bitmap with all bit are set to 0.
 *
 * @attention the size of bitmap_tabl can be caculated by the macro TOS_BITMAP_SIZE
 *
 * @param[in]   bitmap          pointer to the handler of the bitmap.
 * @param[in]   bitmap_tbl      bitmap table buffer.
 * @param[in]   bit_max         maximal bit.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 * @retval  #K_ERR_OBJ_PTR_NULL     bitmap is NULL.
 */
__API__ k_err_t     tos_bitmap_create_empty(k_bitmap_t *bitmap, k_bmtbl_t *bitmap_tbl, uint32_t bit_max);

/**
 * @brief Create a bitmap with all bit are set to 1.
 *
 * @attention the size of bitmap_tabl can be caculated by the macro TOS_BITMAP_SIZE
 *
 * @param[in]   bitmap          pointer to the handler of the bitmap.
 * @param[in]   bitmap_tbl      bitmap table buffer.
 * @param[in]   bit_max         maximal bit.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 * @retval  #K_ERR_OBJ_PTR_NULL     bitmap is NULL.
 */
__API__ k_err_t     tos_bitmap_create_full(k_bitmap_t *bitmap, k_bmtbl_t *bitmap_tbl, uint32_t bit_max);

/**
 * @brief Destroy the bitmap.
 *
 * @attention
 *
 * @param[in]   bitmap          pointer to the handler of the bitmap.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 * @retval  #K_ERR_OBJ_PTR_NULL     bitmap is NULL.
 */
__API__ k_err_t     tos_bitmap_destroy(k_bitmap_t *bitmap);

/**
 * @brief Set a certain bit of the bitmap to 1.
 *
 * @attention
 *
 * @param[in]   bitmap          pointer to the handler of the bitmap.
 * @param[in]   bit             the bit to set.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 * @retval  #K_ERR_BITMAP_EXCEED    bit is larger than the bit_max passed to tos_bitmap_create_*.
 */
__API__ k_err_t     tos_bitmap_set(k_bitmap_t *bitmap, uint32_t bit);

/**
 * @brief Set a certain bit of the bitmap to 0.
 *
 * @attention
 *
 * @param[in]   bitmap          pointer to the handler of the bitmap.
 * @param[in]   bit             the bit to set.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 * @retval  #K_ERR_BITMAP_EXCEED    bit is larger than the bit_max passed to tos_bitmap_create_*.
 */
__API__ k_err_t     tos_bitmap_reset(k_bitmap_t *bitmap, uint32_t bit);

/**
 * @brief Test whether a certain bit of the bitmap is 1.
 *
 * @attention
 *
 * @param[in]   bitmap          pointer to the handler of the bitmap.
 * @param[in]   bit             the bit to set.
 *
 * @return  whether the bit is 1
 * @retval  #K_TRUE             the certain bit is 1.
 * @retval  #K_FALSE            the certain bit is not 1(that means is 0).
 */
__API__ int         tos_bitmap_is_set(k_bitmap_t *bitmap, uint32_t bit);

/**
 * @brief Test whether a certain bit of the bitmap is 0.
 *
 * @attention
 *
 * @param[in]   bitmap          pointer to the handler of the bitmap.
 * @param[in]   bit             the bit to set.
 *
 * @return  whether the bit is 0
 * @retval  #K_TRUE             the certain bit is 0.
 * @retval  #K_FALSE            the certain bit is not 0(that means is 1).
 */
__API__ int         tos_bitmap_is_reset(k_bitmap_t *bitmap, uint32_t bit);

/**
 * @brief Get the lowest significant bit of the bitmap.
 *
 * @attention The very first bit which is set to 1.
 *
 * @param[in]   bitmap          pointer to the handler of the bitmap.
 * @param[in]   bit             the bit to set.
 *
 * @return  the lowest significant bit of the bitmap.
 */
__API__ int         tos_bitmap_lsb(k_bitmap_t *bitmap);

#endif /* _TOS_BITMAP_H_ */

