#ifndef __HAL_TRNG_H__
#define __HAL_TRNG_H__

#include "ll/ll_trng.h"

#define TRNG_INTMASK_VN_ERR         (0x08)
#define TRNG_INTMASK_CRNGT_ERR      (0x04)
#define TRNG_INTMASK_AUTOCORR_ERR   (0x02)
#define TRNG_INTMASK_EHR_VALID      (0x01)

typedef enum
{
    Length_Shortest = 0,
    Length_Short = 1,
    Length_Long = 2,
    Length_Longest = 3
} TrngSrcLength_t;

typedef struct
{
    TrngSrcLength_t srcLength; /*!< reset value is shortest. */
    uint32_t sampleCnt; /*!< how often the TRNG samples the single output bit of the ring oscillator. */
} TRNG_InitStruct;


void HAL_TRNG_Reset(TRNG_Instance *TRNGx);

void HAL_TRNG_Init(TRNG_Instance *TRNGx, TRNG_InitStruct initStruct);

void HAL_TRNG_Start(TRNG_Instance *TRNGx);

void HAL_TRNG_Stop(TRNG_Instance *TRNGx);

uint8_t HAL_TRNG_isDataReady(TRNG_Instance *TRNGx);

uint8_t HAL_TRNG_isBusy(TRNG_Instance *TRNGx);

/**
 * @brief Get random number form a EHR data register.
 *
 * @param TRNGx
 * @param index EHR register index, 0~5 is valid.
 * @return uint32_t
 */
uint32_t HAL_TRNG_GetRandomNumber(TRNG_Instance *TRNGx, uint8_t index);

#endif // !__HAL_TRNG_H__
