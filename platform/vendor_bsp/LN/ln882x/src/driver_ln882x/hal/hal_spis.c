#include "types.h"
#include "ll/ll_spis.h"
#include "hal/hal_spis.h"

void HAL_SPIS_Init(SPIS_InitTypeDef config)
{
    LL_SPIS_Enable(SSI_Disabled);
    LL_SPIS_CtrlR0(config.dfs, config.cfs, 0, config.oe, config.tmod, config.scpol, config.scph, config.format);
}

void HAL_SPIS_ENABLE(SPI_En en)
{
    LL_SPIS_Enable(en);
}

void HAL_SPIS_Microwire_Setting(uint8_t handshake_en, uint8_t mode, uint8_t transfer_mode)
{
    LL_SPIS_MW_HandShake_Enable(handshake_en);
    LL_SPIS_MW_Mode_Set(mode);
    LL_SPIS_MW_TxMode_Set(transfer_mode);
}

void HAL_SPIS_FIFO_Threshold_Set(uint8_t tx_thd, uint8_t rx_thd)
{
    LL_SPIS_TxFIFO_Threshold_Set(tx_thd);
    LL_SPIS_RxFIFO_Threshold_Set(rx_thd);
}

uint8_t HAL_SPIS_Current_TxFIFO_Level()
{
    return LL_SPIS_Current_TxFIFO_Level();
}

uint8_t HAL_SPIS_Current_RxFIFO_Level()
{
    return LL_SPIS_Current_RxFIFO_Level();
}

uint8_t HAL_SPIS_Normal_Status(void)
{
    return LL_SPIS_Normal_Status();
}

uint8_t HAL_SPIS_Is_DataCollision(void)
{
    return LL_SPIS_Is_DataCollision();
}

uint8_t HAL_SPIS_Is_TxError(void)
{
    return LL_SPIS_Is_TxError();
}

uint8_t HAL_SPIS_Is_RxFIFO_Full(void)
{
    return LL_SPIS_Is_RxFIFO_Full();
}

uint8_t HAL_SPIS_Is_RxFIFO_NotEmpty(void)
{
    return LL_SPIS_Is_RxFIFO_NotEmpty();
}

uint8_t HAL_SPIS_Is_TxFIFO_Empty(void)
{
    return LL_SPIS_Is_TxFIFO_Empty();
}

uint8_t HAL_SPIS_Is_TxFIFO_NotFull(void)
{
    return LL_SPIS_Is_TxFIFO_NotFull();
}

uint8_t HAL_SPIS_Is_Busy(void)
{
    return LL_SPIS_Is_Busy();
}

void HAL_SPIS_IntMask_Set(uint8_t mask)
{
    LL_SPIS_IntMask_Set(mask);
}

uint8_t HAL_SPIS_IntMask_Get()
{
    return LL_SPIS_IntMask_Get();
}

uint8_t HAL_SPIS_Int_Status()
{
    return LL_SPIS_Int_Status();
}

uint8_t HAL_SPIS_RawInt_Status()
{
    return LL_SPIS_RawInt_Status();
}

uint8_t HAL_SPIS_IntClr_TxOverflow()
{
    return LL_SPIS_IntClr_TxOverflow();
}

uint8_t HAL_SPIS_IntClr_RxOverflow()
{
    return LL_SPIS_IntClr_RxOverflow();
}

uint8_t HAL_SPIS_IntClr_RxUnderflow()
{
    return LL_SPIS_IntClr_RxUnderflow();
}

uint8_t HAL_SPIS_IntClr_MultiMaster()
{
    return LL_SPIS_IntClr_MultiMaster();
}

//clear ssi_txo_intr, ssi_rxu_intr, ssi_rxo_intr, and the ssi_mst_intr interrupts.
uint8_t HAL_SPIS_IntClr_All()       // get the status and clear int at the same time
{
    return LL_SPIS_IntClr_All();
}


void HAL_SPIS_DMA_Config(uint8_t tx_dma_en, uint8_t rx_dma_en, uint8_t tx_data_level, uint8_t rx_data_level)
{
    LL_SPIS_DMA_TxDataLevel(tx_data_level);
    LL_SPIS_DMA_RXDataLevel(rx_data_level);
    LL_SPIS_DMA_Ctrl(tx_dma_en, rx_dma_en);
}


uint32_t HAL_SPIS_Data_Read()
{
    return LL_SPIS_DataReg_Get();
}

void HAL_SPIS_Data_Write(uint32_t data)
{
    LL_SPIS_DataReg_Set(data);
}


uint32_t HAL_SPIS_Read_RxFIFO(void)
{
    return LL_SPIS_DataReg_Get();
}

void HAL_SPIS_Write_FIFO(uint32_t data)
{
    LL_SPIS_DataReg_Set(data);
}

