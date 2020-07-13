#include "ll/ll_i2s.h"
#include "hal/hal_i2s.h"
#include "hal/hal_common.h"

// TODO: add annotation.

void HAL_I2S_Enable(I2S_En en)
{
    LL_I2S_Enable(en);
}

void HAL_I2S_Tx_Init(I2S_Chan_Enum_t chan, I2S_Resolution resolution)
{
    LL_I2S_TxResolution_Set(chan, resolution);
    LL_I2S_TX_Enable(chan, I2S_TX_ENABLE);

    // block tx enable
    LL_I2S_TxBlock_Enable(I2S_TX_ENABLE);
    LL_I2S_Enable(I2S_ENABLE);
}

void HAL_I2S_Rx_Init(I2S_Chan_Enum_t chan, I2S_Resolution resolution)
{
    LL_I2S_RxResolution_Set(chan, resolution);
    LL_I2S_Rx_Enable(chan, I2S_RX_ENABLE);

    LL_I2S_RxBlock_Enable(I2S_RX_ENABLE);
    LL_I2S_Enable(I2S_ENABLE);
}

void HAL_I2S_Write(I2S_Chan_Enum_t chan, uint32_t *left_data, uint32_t *right_data, uint32_t length)
{
    uint32_t i = 0;

    for (i = 0; i < length; i++) {
        LL_I2S_LeftRxBuf_Set(chan, *(left_data + i) );
        LL_I2S_RightRxBuf_Set(chan, *(right_data + i) );
    }
}

void HAL_I2S_Read(I2S_Chan_Enum_t chan, uint32_t *left_data, uint32_t *right_data, uint32_t length)
{
    uint32_t i = 0;

    for (i = 0; i < length; i++) {
        *(left_data + i) = LL_I2S_LeftRxBuf_Get(chan);
        *(right_data + i) = LL_I2S_RightRxBuf_Get(chan);
    }
}

void HAL_I2S_InterruptEnable(I2S_Chan_Enum_t chan, uint8_t mask_map)
{
    uint8_t mask = 0;

    mask = LL_I2S_IntMask_Get(chan);
    mask &= ~mask_map;
    LL_I2S_IntMask_Set(chan, mask);
}

void HAL_I2S_InterruptDisable(I2S_Chan_Enum_t chan, uint8_t mask_map)
{
    uint8_t mask = 0;

    mask = LL_I2S_IntMask_Get(chan);
    mask |= mask_map;
    LL_I2S_IntMask_Set(chan, mask);
}

uint8_t HAL_I2S_IntStatus(I2S_Chan_Enum_t chan)
{
    return LL_I2S_IntStatus(chan);
}

void HAL_I2S_RxFIFO_TrigLvl_Set(I2S_Chan_Enum_t chan, uint8_t rx_lvl)
{
    LL_I2S_RxFIFO_TrigLvl_Set(chan, rx_lvl);
}

void HAL_I2S_TxFIFO_TrigLvl_Set(I2S_Chan_Enum_t chan, uint8_t tx_lvl)
{
    LL_I2S_TxFIFO_TrigLvl_Set(chan, tx_lvl);
}

void HAL_I2S_TxFIFO_Flush(I2S_Chan_Enum_t chan)
{
    LL_I2S_TX_Enable(chan, I2S_TX_DISABLE);
    LL_I2S_TxFIFO_Flush(chan);
}

void HAL_I2S_RxFIFO_Flush(I2S_Chan_Enum_t chan)
{
    LL_I2S_Rx_Enable(chan, I2S_RX_DISABLE);
    LL_I2S_RxFIFO_Flush(chan);
}

uint8_t HAL_I2S_IntClr_RxOverrun(I2S_Chan_Enum_t chan)
{
    return LL_I2S_IntClr_RxOverrun(chan);
}

uint8_t HAL_I2S_IntClr_TxOverrun(I2S_Chan_Enum_t chan)
{
    return LL_I2S_IntClr_TxOverrun(chan);
}

void HAL_I2S_Reset_TxDMA(void)
{
    LL_I2S_Reset_TxDMA();
}

void HAL_I2S_Reset_RxDMA(void)
{
    LL_I2S_Reset_RxDMA();
}

void HAL_I2S_Tx_MultiData(uint32_t *ch0_left_data, uint32_t *ch0_right_data, uint32_t *ch1_left_data, uint32_t *ch1_right_data, uint16_t length)
{
    uint16_t i = 0;

    HAL_I2S_Reset_TxDMA();

    for (i = 0; i < length; i++) {
        LL_I2S_TxDMA_Set( *(ch0_left_data + i) );
        LL_I2S_TxDMA_Set( *(ch0_right_data + i) );
        LL_I2S_TxDMA_Set( *(ch1_left_data + i) );
        LL_I2S_TxDMA_Set( *(ch1_right_data + i) );
    }
}

void HAL_I2S_Rx_MultiData(uint32_t *ch0_left_data, uint32_t *ch0_right_data, uint32_t *ch1_left_data, uint32_t *ch1_right_data, uint16_t length)
{
    uint16_t i = 0;

    LL_I2S_Reset_RxDMA();

    for (i = 0; i < length; i++) {
        *(ch0_left_data + i) = LL_I2S_RxDMA_Get();
        *(ch0_right_data + i) = LL_I2S_RxDMA_Get();
        *(ch1_left_data + i) = LL_I2S_RxDMA_Get();
        *(ch1_right_data + i) = LL_I2S_RxDMA_Get();
    }
}
