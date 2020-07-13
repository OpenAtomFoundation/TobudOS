#ifndef __HAL_EFUSE_H__
#define __HAL_EFUSE_H__

#include "ll/ll_efuse.h"

#define IS_EFUSE_ALL_INSTANCES(efusex)  ((efusex) == EFUSE)

typedef enum {
    EFUSE_REG_0 = 0,
    EFUSE_REG_1,
    EFUSE_REG_2,
    EFUSE_REG_3,
    EFUSE_REG_4,
    EFUSE_REG_5,
    EFUSE_REG_6,
    EFUSE_REG_7,
} EFUSE_Reg_Index_t;

#define IS_EFUSE_REG_INDEX(index)   (   ((index) == EFUSE_REG_0) || \
                                        ((index) == EFUSE_REG_1) || \
                                        ((index) == EFUSE_REG_2) || \
                                        ((index) == EFUSE_REG_3) || \
                                        ((index) == EFUSE_REG_4) || \
                                        ((index) == EFUSE_REG_5) || \
                                        ((index) == EFUSE_REG_6) || \
                                        ((index) == EFUSE_REG_7) )

/**
 * @brief This function must be called before any EFUSE register writing operations.
 *
 * @param EFUSEx
 */
void HAL_EFUSE_WriteEnable(EFUSE_Instance * EFUSEx);

/**
 * @brief Write shadow register, but the effective value is stored in correct register.
 *
 * @param EFUSEx
 * @param regIndex valid value is 0~7.
 * @param regValue
 */
void HAL_EFUSE_WriteShadowReg(EFUSE_Instance * EFUSEx, EFUSE_Reg_Index_t regIndex, uint32_t regValue);

/**
 * @brief Read from shadow register.
 *
 * @param EFUSEx
 * @param regIndex
 * @return uint32_t
 */
uint32_t HAL_EFUSE_ReadShadowReg(EFUSE_Instance * EFUSEx, EFUSE_Reg_Index_t regIndex);

/**
 * @brief The effective value is stored in correct register.
 *
 * @param EFUSEx
 * @param regIndex valid value is 0~7.
 * @return uint32_t
 */
uint32_t HAL_EFUSE_ReadCorrectReg(EFUSE_Instance * EFUSEx, EFUSE_Reg_Index_t regIndex);


/**
 * @brief Set one bit to 1.
 *
 * @param EFUSEx
 * @param regIndex valid value is 0~7
 * @param bitIndex valid value is 0~26
 */
void HAL_EFUSE_EnforceBit(EFUSE_Instance * EFUSEx, EFUSE_Reg_Index_t regIndex, uint8_t bitIndex);

#endif // !__HAL_EFUSE_H__
