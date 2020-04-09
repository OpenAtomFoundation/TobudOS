#ifndef __LL_I2S_H__
#define __LL_I2S_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

/////////////////////////////  Data types and Macros  /////////////////////////

#include "types.h"
/**
 * LN882x only support 1 channel i2s,
 * so I2S_CHAN_1 is not support in fact.
 **/
typedef enum
{
    I2S_CHAN_0 = 0,
    I2S_CHAN_1,
} I2S_Chan_Enum_t;


/////////////////////////////  Function Declarations  /////////////////////////

/**
 * @brief A disable on this bit overrides any other block or channel enables
 * and flushes all FIFOs.
 *
 * @param en 1 -- enable; 0 -- disable.
 */
void LL_I2S_Enable(uint8_t en);

/**
 * @brief A disable on this bit overrides any individual receive channel enables.
 *
 * @param en 1 -- enable receiver; 0 -- disable.
 */
void LL_I2S_RxBlock_Enable(uint8_t en);

/**
 * @brief A disable on this bit overrides any individual transmit channel enables.
 *
 * @param en 1 -- enable transmitter; 0 -- disable.
 */
void LL_I2S_TxBlock_Enable(uint8_t en);

/**
 * @brief Put a data in the Left Receive Buffer Register.
 *
 * @param chan
 * @param data
 */
void LL_I2S_LeftRxBuf_Set(I2S_Chan_Enum_t chan, uint32_t data);

/**
 * @brief Get a data from the Left Receive Buffer Register.
 *
 * @param chan
 * @return uint32_t
 */
uint32_t LL_I2S_LeftRxBuf_Get(I2S_Chan_Enum_t chan);

/**
 * @brief Put a data in the Right Receive Buffer Register.
 *
 * @param chan
 * @param data
 */
void LL_I2S_RightRxBuf_Set(I2S_Chan_Enum_t chan, uint32_t data);

/**
 * @brief Get a data from the Rigt Receive Buffer Register.
 *
 * @param chan
 * @return uint32_t
 */
uint32_t LL_I2S_RightRxBuf_Get(I2S_Chan_Enum_t chan);

/**
 * @brief To enable/disable a receive channel, independently of all other channels.
 *
 * @param chan
 * @param en 1 -- enable; 0 -- disable.
 */
void LL_I2S_Rx_Enable(I2S_Chan_Enum_t chan, uint8_t en);

/**
 * @brief Enable/disable a transmit channel, independently of all other channels.
 * A global disable of IER[0] or Transmitter block ITER[0] overrides this value.
 *
 * @param chan
 * @param en 1 -- enable; 0 -- disable.
 */
void LL_I2S_TX_Enable(I2S_Chan_Enum_t chan, uint8_t en);

/**
 * @brief To program the desired data resolution of the receiver and enables
 * the LSB of the incoming left (or right) word to be placed in the LSB of the
 * LRBRx (or RRBRx) register.
 *
 * @param chan
 * @param resolution
 */
void LL_I2S_RxResolution_Set(I2S_Chan_Enum_t chan, uint8_t resolution);

void LL_I2S_RxResolution_Get(I2S_Chan_Enum_t chan, uint8_t *resolution);

void LL_I2S_TxResolution_Set(I2S_Chan_Enum_t chan, uint8_t resolution);

void LL_I2S_TxResolution_Get(I2S_Chan_Enum_t chan, uint8_t *resolution);

/**
 * @brief Interrupt status of one channel.
 *
 * @param chan
 * @return uint8_t
 */
uint8_t LL_I2S_IntStatus(I2S_Chan_Enum_t chan);

void LL_I2S_IntMask_Set(I2S_Chan_Enum_t chan, uint8_t mask_map);
uint8_t LL_I2S_IntMask_Get(I2S_Chan_Enum_t chan);

uint8_t LL_I2S_IntClr_RxOverrun(I2S_Chan_Enum_t chan);

uint8_t LL_I2S_IntClr_TxOverrun(I2S_Chan_Enum_t chan);

/**
 * @brief To set the trigger level in the RX FIFO at which the Received Data
 * Available interrupt is generated.
 *
 * @param chan
 * @param level
 */
void LL_I2S_RxFIFO_TrigLvl_Set(I2S_Chan_Enum_t chan, uint8_t level);

/**
 * @brief To set the trigger level in the TX FIFO at which the Empty Threshold
 * Reached Interrupt is generated.
 *
 * @param chan
 * @param level
 */
void LL_I2S_TxFIFO_TrigLvl_Set(I2S_Chan_Enum_t chan, uint8_t level);

/**
 * @brief Receive Channel FIFO Reset.
 * Rx channel or block must be disabled prior to writing to this bit.
 *
 * @param chan
 */
void LL_I2S_RxFIFO_Flush(I2S_Chan_Enum_t chan);

/**
 * @brief Transmit Channel FIFO Reset.
 * Tx channel or block must be disabled prior to writing to this bit.
 *
 * @param chan
 */
void LL_I2S_TxFIFO_Flush(I2S_Chan_Enum_t chan);

/**
 * @brief If you enabled chan-0 and chan-1, order of returned read data:
 * 1. ch0 - left data
 * 2. ch0 - right data
 * 3. ch1 - left data
 * 4. ch1 - right data
 *
 * 5. ch0 - left data
 * 6. ch0 - right data
 * 7. ch1 - left data
 * 8. ch1 - right data
 *
 * @return uint32_t
 */
uint32_t LL_I2S_RxDMA_Get(void);

/**
 * @brief Reset Receiver Block DMA.
 *
 */
void LL_I2S_Reset_RxDMA(void);

/**
 * @brief To cycle repeatedly through the enabled Transmit channels (from lowest
 * numbered to highest) to allow writing stereo data pairs.
 *
 * @param data
 */
void LL_I2S_TxDMA_Set(uint32_t data);

/**
 * @brief Reset Transmitter Block DMA.
 *
 */
void LL_I2S_Reset_TxDMA(void);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __LL_I2S_H__
