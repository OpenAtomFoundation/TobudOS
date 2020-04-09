#include "ln88xx.h"
#include "ll/ll_efuse.h"
#include "hal/hal_efuse.h"
#include "hal/hal_common.h"

/**
 * @brief This function must be called before any EFUSE register writing operations.
 *
 * @param EFUSEx
 */
void HAL_EFUSE_WriteEnable(EFUSE_Instance * EFUSEx)
{
    assert_param(IS_EFUSE_ALL_INSTANCES(EFUSEx));

    LL_EFUSE_WriteEnable(EFUSEx);
}

/**
 * @brief Write shadow register, but the effective value is stored in correct register.
 *
 * @param EFUSEx
 * @param regIndex valid value is 0~7.
 * @param regValue
 */
void HAL_EFUSE_WriteShadowReg(EFUSE_Instance * EFUSEx, EFUSE_Reg_Index_t regIndex, uint32_t regValue)
{
    assert_param(IS_EFUSE_ALL_INSTANCES(EFUSEx));
    assert_param(IS_EFUSE_REG_INDEX(regIndex));

    LL_EFUSE_WriteShadowReg(EFUSEx, (uint8_t)regIndex, regValue);
}

/**
 * @brief Read from shadow register.
 *
 * @param EFUSEx
 * @param regIndex
 * @return uint32_t
 */
uint32_t HAL_EFUSE_ReadShadowReg(EFUSE_Instance * EFUSEx, EFUSE_Reg_Index_t regIndex)
{
    assert_param(IS_EFUSE_ALL_INSTANCES(EFUSEx));
    assert_param(IS_EFUSE_REG_INDEX(regIndex));

    return LL_EFUSE_ReadShadowReg(EFUSEx, (uint8_t)regIndex);
}

/**
 * @brief The effective value is stored in correct register.
 *
 * @param EFUSEx
 * @param regIndex valid value is 0~7.
 * @return uint32_t
 */
uint32_t HAL_EFUSE_ReadCorrectReg(EFUSE_Instance * EFUSEx, EFUSE_Reg_Index_t regIndex)
{
    assert_param(IS_EFUSE_ALL_INSTANCES(EFUSEx));
    assert_param(IS_EFUSE_REG_INDEX(regIndex));

    return LL_EFUSE_ReadCorrectReg(EFUSEx, regIndex);
}

/**
 * @brief Set one bit to 1.
 *
 * @param EFUSEx
 * @param regIndex valid value is 0~7
 * @param bitIndex valid value is 0~26
 */
void HAL_EFUSE_EnforceBit(EFUSE_Instance * EFUSEx, EFUSE_Reg_Index_t regIndex, uint8_t bitIndex)
{
    assert_param(IS_EFUSE_ALL_INSTANCES(EFUSEx));
    assert_param(IS_EFUSE_REG_INDEX(regIndex));
    assert_param(bitIndex <= 26);

    uint32_t regValue = LL_EFUSE_ReadCorrectReg(EFUSEx, regIndex);
    regValue |= (bitIndex + 1) << 27;
    LL_EFUSE_WriteShadowReg(EFUSEx, regIndex, regValue);
}
