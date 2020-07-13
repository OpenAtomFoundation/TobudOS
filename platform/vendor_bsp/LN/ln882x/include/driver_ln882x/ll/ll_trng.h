#ifndef __LL_TRNG_H__
#define __LL_TRNG_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "types.h"
#include "ln88xx.h"

typedef     void    TRNG_Instance;
#define     TRNG     ((TRNG_Instance *) REG_TRNG_BASE)

///////////////////////////////////////////////////////////////////////////////
//                                 TRNG_CONFIG                               //
//                              Configuration Register                       //
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Select the length of the inverter chain in the ring oscillator.
 *
 * @param TRNGx
 * @param src
 * 0 -- shortest inverter chain length. (reset value)
 * 1 -- short inverter chain length.
 * 2 -- long inverter chain length.
 * 3 -- longset inverter chain length.
 */
void LL_TRNG_SrcLengthCfg(TRNG_Instance *TRNGx, uint8_t srcLen);

///////////////////////////////////////////////////////////////////////////////
//                                 TRNG_VALID                                //
//                                Valid Register                             //
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Check whether the EHR_DATA[0-5] registers contain 192 bits of valid data.
 *
 * @param TRNGx
 * @return uint8_t
 */
uint8_t LL_TRNG_isDataReady(TRNG_Instance *TRNGx);

///////////////////////////////////////////////////////////////////////////////
//                                 EHR_DATA[0~5]                             //
//                     Entropy Holding Register Data Registers               //
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Get TRNG data register value.
 *
 * @param TRNGx
 * @param index valid index is 0~5.
 * @return uint32_t
 */
uint32_t LL_TRNG_GetRandomNumber(TRNG_Instance * TRNGx, uint8_t index);

///////////////////////////////////////////////////////////////////////////////
//                               RND_SOURCE_ENABLE                           //
//                         Random Source Enable Register                     //
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Choose the entropy source.
 *
 * @param TRNGx
 * @param en
 * 1 -- ring oscillator.
 * 0 -- entropy source is disabled, this is the reset value.
 */
void LL_TRNG_Start(TRNG_Instance * TRNGx);

void LL_TRNG_Stop(TRNG_Instance * TRNGx);

///////////////////////////////////////////////////////////////////////////////
//                                 SAMPLE_CNT1                               //
//                             Sample Count Register                         //
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Config how often the TRNG samples the single output bit of the ring oscillator.
 *
 * @param TRNGx
 * @param sample
 */
void LL_TRNG_SampleCnt_Cfg(TRNG_Instance *TRNGx, uint32_t sample);

///////////////////////////////////////////////////////////////////////////////
//                                 TRNG_SW_RESET                             //
//                                 Reset Register                            //
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Requires an internal TRNG reset.
 *
 * @param TRNGx
 */
void LL_TRNG_Reset(TRNG_Instance *TRNGx);

///////////////////////////////////////////////////////////////////////////////
//                                 TRNG_BUSY                                 //
//                                Busy Register                              //
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Indicates when the TRING is busy.
 *
 * @param TRNGx
 * @return uint8_t
 */
uint8_t LL_TRNG_isBusy(TRNG_Instance *TRNGx);

/**
 * @brief [NOTE] This function must be called in order to behave normally.
 *
 * @param TRNGx
 */
void LL_TRNG_DebugDisable(TRNG_Instance *TRNGx);

#ifdef __cplusplus
    }
#endif
#endif // !__LL_TRNG_H__
