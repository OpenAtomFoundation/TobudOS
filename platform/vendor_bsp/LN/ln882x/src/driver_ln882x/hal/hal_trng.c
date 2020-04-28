#include "ll/ll_trng.h"
#include "hal/hal_trng.h"
#include "hal/hal_common.h"

#define IS_TRNG_ALL_INSTANCE(trng) ((trng) == TRNG)

void HAL_TRNG_Reset(TRNG_Instance *TRNGx)
{
    assert_param(IS_TRNG_ALL_INSTANCE(TRNGx));

    LL_TRNG_Reset(TRNGx);
}

void HAL_TRNG_Init(TRNG_Instance *TRNGx, TRNG_InitStruct initStruct)
{
    assert_param(IS_TRNG_ALL_INSTANCE(TRNGx));

    // reset first.
    LL_TRNG_Reset(TRNGx);

    LL_TRNG_SrcLengthCfg(TRNGx, initStruct.srcLength);
    LL_TRNG_SampleCnt_Cfg(TRNGx, initStruct.sampleCnt);
    // NOTE: just do it.
    LL_TRNG_DebugDisable(TRNGx);
}

void HAL_TRNG_Start(TRNG_Instance *TRNGx)
{
    assert_param(IS_TRNG_ALL_INSTANCE(TRNGx));

    LL_TRNG_Start(TRNGx);
}

void HAL_TRNG_Stop(TRNG_Instance *TRNGx)
{
    assert_param(IS_TRNG_ALL_INSTANCE(TRNGx));

    LL_TRNG_Stop(TRNGx);
}

uint8_t HAL_TRNG_isDataReady(TRNG_Instance *TRNGx)
{
    assert_param(IS_TRNG_ALL_INSTANCE(TRNGx));

    return LL_TRNG_isDataReady(TRNGx);
}

uint8_t HAL_TRNG_isBusy(TRNG_Instance *TRNGx)
{
    assert_param(IS_TRNG_ALL_INSTANCE(TRNGx));

    return LL_TRNG_isBusy(TRNGx);
}

/**
 * @brief Get random number form a EHR data register.
 *
 * @param TRNGx
 * @param index EHR register index, 0~5 is valid.
 * @return uint32_t
 */
uint32_t HAL_TRNG_GetRandomNumber(TRNG_Instance *TRNGx, uint8_t index)
{
    assert_param(IS_TRNG_ALL_INSTANCE(TRNGx));
    assert_param(index < 6);

    uint32_t data = 0;

    data = LL_TRNG_GetRandomNumber(TRNGx, index);
    return data;
}

