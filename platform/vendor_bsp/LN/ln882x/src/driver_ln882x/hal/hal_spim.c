#include "hal/hal_spim.h"
#include "ll/ll_spim.h"

void HAL_SPIM_Init(uint32_t src_clk, SPIM_InitTypeDef config)
{
    LL_SPIM_Enable(SSI_Disabled);
    LL_SPIM_CLK_Div(src_clk / config.speed);

    LL_SPIM_CtrlR0(config.dfs, config.cfs, 0, 0, 0, config.scpol, config.scph, config.format);
}

void HAL_SPIM_Read_Register_Polling(SPIM_Slave_Id id, uint32_t *wr_ptr, uint16_t wr_len, uint32_t *rd_ptr, uint16_t rd_len)
{
    uint16_t i;
    LL_SPIM_TransferMode_Set(EEPROM_Read);
    LL_SPIM_CtrlR1(rd_len - 1);
    LL_SPIM_Enable(SSI_Enabled);
    LL_SPIM_Slave_Select(0);

    if(wr_len <= TX_FIFO_DEPTH) {
        for( i = 0; i < wr_len; i++) {
            LL_SPIM_DataReg_Set(*wr_ptr++);
        }
        LL_SPIM_Slave_Select(id);
    } else {
        for( i = 0; i < TX_FIFO_DEPTH; i++) {
            LL_SPIM_DataReg_Set(*wr_ptr++);
        }

        LL_SPIM_Slave_Select(id);

        for( i = TX_FIFO_DEPTH; i < wr_len; i++) {
            while( LL_SPIM_Is_TxFIFONotFull() == Transmit_FIFO_Full);
            LL_SPIM_DataReg_Set(*wr_ptr++);
        }
    }

    while( LL_SPIM_Is_TxFIFOEmpty() != Transmit_FIFO_Empty);

    for(i = 0; i < rd_len; i++) {
        while( LL_SPIM_Is_RxFIFONotEmpty() == Receive_FIFO_Empty );
        *rd_ptr++ = LL_SPIM_DataReg_Get();
    }
    while( LL_SPIM_Is_Busy() == SSI_Busy);
    LL_SPIM_Enable(SSI_Disabled);
}

void HAL_SPIM_Read_Only_Polling(SPIM_Slave_Id id, uint32_t * rd_ptr, uint16_t rd_len)
{
    uint16_t i;

    LL_SPIM_TransferMode_Set(Receive_Only);
    LL_SPIM_CtrlR1(rd_len - 1);
    LL_SPIM_Enable(SSI_Enabled);

    LL_SPIM_Slave_Select(0);
    LL_SPIM_Slave_Select(id);

    LL_SPIM_DataReg_Set(0xffffffff);

    for(i = 0; i < rd_len; i++) {
        while( LL_SPIM_Is_RxFIFONotEmpty() == Receive_FIFO_Empty );
        *rd_ptr++ = LL_SPIM_DataReg_Get();
    }

    while( LL_SPIM_Is_Busy() == SSI_Busy );
    LL_SPIM_Enable(SSI_Disabled);
}

void HAL_SPIM_Write_Polling(SPIM_Slave_Id id, uint32_t * wr_ptr, uint16_t wr_len)
{
    uint16_t i;

    LL_SPIM_TransferMode_Set(Transmit_Only);
    LL_SPIM_Enable(SSI_Enabled);
    LL_SPIM_Slave_Select(0);

    if(wr_len <= TX_FIFO_DEPTH) {
        for( i = 0; i < wr_len; i++) {
            LL_SPIM_DataReg_Set(*wr_ptr++);
        }
        LL_SPIM_Slave_Select(id);
    } else {
        for( i = 0; i < TX_FIFO_DEPTH; i++) {
            LL_SPIM_DataReg_Set(*wr_ptr++);
        }
        LL_SPIM_Slave_Select(id);
        for( i = TX_FIFO_DEPTH; i < wr_len; i++) {
            while( LL_SPIM_Is_TxFIFONotFull() == Transmit_FIFO_Full );
            LL_SPIM_DataReg_Set(*wr_ptr++);
        }
    }
    while( LL_SPIM_Is_Busy() == SSI_Busy );
    LL_SPIM_Enable(SSI_Disabled);
}


void HAL_SPIM_Transmit_And_Receive(SPIM_Slave_Id id)
{
    LL_SPIM_TransferMode_Set(Transmit_and_Receive);
    LL_SPIM_Enable(SSI_Enabled);
    LL_SPIM_Slave_Select(0);
    LL_SPIM_Slave_Select(id);
}

void HAL_SPIM_Read_Register(SPIM_Slave_Id id, uint16_t rd_len)
{
    LL_SPIM_TransferMode_Set(EEPROM_Read);
    LL_SPIM_CtrlR1(rd_len - 1);
    LL_SPIM_Enable(SSI_Enabled);
    LL_SPIM_Slave_Select(0);
    LL_SPIM_Slave_Select(id);
}

void HAL_SPIM_Write(SPIM_Slave_Id id)
{
    LL_SPIM_TransferMode_Set(Transmit_Only);
    LL_SPIM_Enable(SSI_Enabled);
    LL_SPIM_Slave_Select(0);
    LL_SPIM_Slave_Select(id);
}

void HAL_SPIM_Read_Only(SPIM_Slave_Id id, uint16_t rd_len, uint32_t dummy_data)
{
    LL_SPIM_TransferMode_Set(Receive_Only);
    LL_SPIM_CtrlR1(rd_len - 1);
    LL_SPIM_Enable(SSI_Enabled);

    LL_SPIM_Slave_Select(0);
    LL_SPIM_Slave_Select(id);

    LL_SPIM_DataReg_Set(dummy_data);
}

void HAL_SPIM_Set_Read_Len(uint16_t rd_len)
{
    LL_SPIM_CtrlR1(rd_len - 1);
}

void HAL_SPIM_Read_RxFIFO(uint32_t * rd_ptr, uint16_t rd_len)
{
    uint16_t i;
    for( i = 0; i < rd_len; i++) {
        while( LL_SPIM_Is_RxFIFONotEmpty() == Receive_FIFO_Empty );
        *rd_ptr++ = LL_SPIM_DataReg_Get();
    }
}

void HAL_SPIM_Write_TxFIFO(uint32_t * wr_ptr, uint16_t wr_len)
{
    uint16_t i;
    for( i = 0; i < wr_len; i++) {
        while( LL_SPIM_Is_TxFIFONotFull() == Transmit_FIFO_Full );
        LL_SPIM_DataReg_Set(*wr_ptr++);
    }
}

void HAL_SPIM_RxSample_Dly(uint8_t dly)
{
    LL_SPIM_RxSampleDly_Set(dly);
}

void HAL_SPIM_Set_DMA(uint8_t tx_dma_en, uint8_t rx_dma_en, uint8_t tx_data_level, uint8_t rx_data_level)
{
    LL_SPIM_DAM_DataLvl_Set(tx_data_level, rx_data_level);
    LL_SPIM_DMA_Ctrl(tx_dma_en, rx_dma_en);
}

//clear ssi_txo_intr, ssi_rxu_intr, ssi_rxo_intr, and the ssi_mst_intr interrupts.
uint8_t HAL_SPIM_IntClr()       // get the status and clear int at the same time
{
    return LL_SPIM_IntClr_All();
}

uint8_t HAL_SPIM_IntClr_MST()
{
    return LL_SPIM_IntClr_MultiMaster();
}

uint8_t HAL_SPIM_IntClr_RXU()
{
    return LL_SPIM_IntClr_RxFIFOUnderflow();
}

uint8_t HAL_SPIM_IntClr_RXO()
{
    return LL_SPIM_IntClr_RxFIFOOverflow();
}

uint8_t HAL_SPIM_IntClr_TXO()
{
    return LL_SPIM_IntClr_TxFIFOOverflow();
}

uint8_t HAL_SPIM_RawInt_Status()
{
    return LL_SPIM_RawInt_Status();
}

uint8_t HAL_SPIM_Int_Status()
{
    return LL_SPIM_Int_Status();
}

void HAL_SPIM_Set_Mask(uint8_t mask)
{
    LL_SPIM_IntMask_Set(mask);
}

uint8_t HAL_SPIM_Get_Mask()
{
    return LL_SPIM_IntMask_Get();
}

uint8_t HAL_SPIM_Normal_Status()
{
    return LL_SPIM_Normal_Status();
}

uint8_t HAL_SPIM_Current_TxFIFO_Level()
{
    return LL_SPIM_Current_TxFIFO_Level();
}

uint8_t HAL_SPIM_Current_RxFIFO_Level()
{
    return LL_SPIM_Current_RxFIFO_Level();
}

void HAL_SPIM_Set_FIFO_Threshold(uint8_t tx_thd, uint8_t rx_thd)
{
    LL_SPIM_FIFO_Threshold_Set(tx_thd, rx_thd);
}

void HAL_SPIM_CLK_Div(uint16_t div)
{
    LL_SPIM_CLK_Div(div);
}

void HAL_SPIM_Slave_Select_En(SPIM_Slave_Id id)
{
    LL_SPIM_Slave_Select(id);
}

void HAL_SPIM_Microwire_Setting(uint8_t handshake_en, uint8_t mode, uint8_t transfer_mode)
{
    LL_SPIM_Microwire_Setting(handshake_en, mode, transfer_mode);
}

void HAL_SPIM_Enable(SPI_En en)
{
    LL_SPIM_Enable(en);
}

void HAL_SPIM_Ctrl0(uint8_t dfs,uint8_t cfs,uint8_t srl,uint8_t slv_oe,uint8_t tmod,uint8_t scpol,uint8_t scph,uint8_t frf)
{
    LL_SPIM_CtrlR0(dfs, cfs, srl, slv_oe, tmod, scpol, scph, frf);
}

void HAL_SPIM_Ctrl1(uint16_t ndf)
{
    LL_SPIM_CtrlR1(ndf);
}

uint32_t HAL_SPIM_Data_Read()
{
    return LL_SPIM_DataReg_Get();
}

void HAL_SPIM_Data_Write(uint32_t data)
{
    LL_SPIM_DataReg_Set(data);
}

bool HAL_SPIM_Is_Busy()
{
    return LL_SPIM_Is_Busy();
}

void HAL_SPIM_Mode_Set(SPI_Transmit_Mode mode)
{
    LL_SPIM_TransferMode_Set(mode);
}
