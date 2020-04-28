#ifndef __HAL_I2S_H__
#define __HAL_I2S_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"
#include "ll/ll_i2s.h"

////////////////////////////  Data type and Macros  //////////////////////////

#define I2S_RX_FIFO_DEPTH           8
#define I2S_TX_FIFO_DEPTH           8

/**
 * @brief enum of I2S Enable and I2S Disable.
 */
typedef enum
{
    I2S_DISABLE = 0,
    I2S_ENABLE  = 1
}I2S_En;

/**
 * @brief enum of I2S Tx Enable and Tx Disable.
 */
typedef enum
{
    I2S_TX_DISABLE = 0,
    I2S_TX_ENABLE  = 1
}I2S_TX_En;

/**
 * @brief enum of I2S Rx Enable and Rx Disable.
 */
typedef enum
{
    I2S_RX_DISABLE = 0,
    I2S_RX_ENABLE  = 1
}I2S_RX_En;

/**
 * @brief enum of I2S Resolution, can be defined to 12bit, 16bit, 20bit, 24bit and 32bit.
 */
typedef enum
{
    I2S_WORD_LENGTH_NONE  = 0,
    I2S_WORD_LENGTH_12BIT = 1,
    I2S_WORD_LENGTH_16BIT = 2,
    I2S_WORD_LENGTH_20BIT = 3,
    I2S_WORD_LENGTH_24BIT = 4,
    I2S_WORD_LENGTH_32BIT = 5
}I2S_Resolution;

/**
 * @brief enum of I2S Int status.
 */
typedef enum
{
    I2S_RX_DATA_AVALIABLE         = 0x01,
    I2S_RX_FIFO_OVERRUN           = 0x02,
    I2S_TX_FIFO_EMPTY_LVL_REACHED = 0x10,
    I2S_TX_FIFO_OVERRUN           = 0x20
}I2S_Int_Status;

/**
 * @brief enum of I2S Int Mask.
 */
typedef enum{
    I2S_MASK_RX_DATA_AVALIABLE         = 0x01,
    I2S_MASK_RX_FIFO_OVERRUN           = 0x02,
    I2S_MASK_TX_FIFO_EMPTY_LVL_REACHED = 0x10,
    I2S_MASK_TX_FIFO_OVERRUN           = 0x20
}I2S_Int_Mask;

////////////////////////////  Function Declarations  //////////////////////////

void HAL_I2S_Enable(I2S_En en);

void HAL_I2S_Tx_Init(I2S_Chan_Enum_t chan, I2S_Resolution resolution);

void HAL_I2S_Rx_Init(I2S_Chan_Enum_t chan, I2S_Resolution resolution);

void HAL_I2S_Write(I2S_Chan_Enum_t chan, uint32_t *left_data, uint32_t *right_data, uint32_t length);

void HAL_I2S_Read(I2S_Chan_Enum_t chan, uint32_t *left_data, uint32_t *right_data, uint32_t length);

void HAL_I2S_InterruptEnable(I2S_Chan_Enum_t chan, uint8_t mask_map);
void HAL_I2S_InterruptDisable(I2S_Chan_Enum_t chan, uint8_t mask_map);

uint8_t HAL_I2S_IntStatus(I2S_Chan_Enum_t chan);

void HAL_I2S_RxFIFO_TrigLvl_Set(I2S_Chan_Enum_t chan, uint8_t rx_lvl);

void HAL_I2S_TxFIFO_TrigLvl_Set(I2S_Chan_Enum_t chan, uint8_t tx_lvl);

void HAL_I2S_TxFIFO_Flush(I2S_Chan_Enum_t chan);

void HAL_I2S_RxFIFO_Flush(I2S_Chan_Enum_t chan);

uint8_t HAL_I2S_IntClr_RxOverrun(I2S_Chan_Enum_t chan);

uint8_t HAL_I2S_IntClr_TxOverrun(I2S_Chan_Enum_t chan);

void HAL_I2S_Reset_TxDMA(void);

void HAL_I2S_Reset_RxDMA(void);

void HAL_I2S_Tx_MultiData(uint32_t *ch0_left_data, uint32_t *ch0_right_data, uint32_t *ch1_left_data, uint32_t *ch1_right_data, uint16_t length);

void HAL_I2S_Rx_MultiData(uint32_t *ch0_left_data, uint32_t *ch0_right_data, uint32_t *ch1_left_data, uint32_t *ch1_right_data, uint16_t length);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __HAL_I2S_H__
