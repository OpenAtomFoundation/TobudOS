#ifndef __LL_EFUSE_H__
#define __LL_EFUSE_H__

typedef     void        EFUSE_Instance;
#define     EFUSE       ((EFUSE_Instance *) REG_EFUSE_BASE)

void LL_EFUSE_WriteEnable(EFUSE_Instance * EFUSEx);

/**
 * @brief Write R0~R7.
 *
 * @param EFUSEx
 * @param regIndex index, [0,7].
 * @param regValue
 */
void LL_EFUSE_WriteShadowReg(EFUSE_Instance *EFUSEx, uint8_t regIndex, uint32_t regValue);

/**
 * @brief Read from shawdow register R0~R7.
 *
 * @param EFUSEx
 * @param regIndex 0~7 is valid.
 * @return uint32_t
 */
uint32_t LL_EFUSE_ReadShadowReg(EFUSE_Instance * EFUSEx, uint8_t regIndex);

/**
 * @brief The effective value is stored in correct registers.
 *
 * @param EFUSEx
 * @param regIndex 0~7 is valid.
 * @return uint32_t
 */
uint32_t LL_EFUSE_ReadCorrectReg(EFUSE_Instance * EFUSEx, uint8_t regIndex);

#endif // !__LL_EFUSE_H__
