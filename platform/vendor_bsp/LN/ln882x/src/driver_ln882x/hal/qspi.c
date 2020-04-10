#include "hal/qspi.h"
#include "hal/spi_type.h"
#include "hal/hal_dma.h"
#include "hal/hal_syscon.h"
#include "ll/ll_qspi.h"

#pragma push
#pragma O2

#define QSPI_SPIF_SCLK              (GPIOB_0)

/**
 * @brief QSPI init.
 * 
 * @param clk_divider 
 * @param rx_sample_dly 
 */
void qspi_init(uint16_t clk_divider, uint8_t rx_sample_dly)
{
    //set QSPI_SPIF_SCLK pin to pull down
    HAL_SYSCON_GPIO_PullDown(QSPI_SPIF_SCLK);

    //select spif IO as pad
    HAL_SYSCON_SPIFlashEnable(1);

    LL_QSPI_RxSampleDlySet(rx_sample_dly);

    //reset chip
    LL_QSPI_Enable(SSI_Disabled);
    LL_QSPI_IntMaskAll();
    LL_QSPI_SckDivSet(clk_divider);

    HAL_DMA_Init(DMA_CHANNEL_0, NULL);
}

/**
 * @brief QSPI deinit.
 */
void qspi_deinit()
{
    LL_QSPI_IntMaskAll(); // disable all QSPI interrupts.
    LL_QSPI_Enable(SSI_Disabled);
    HAL_DMA_DeInit(DMA_CHANNEL_0);
}

/**
 * @brief Send a cmd, which is represented by `wr_ptr`, and wait for data, which is represented by `rd_ptr`.
 * 
 * @param rd_ptr read buffer.
 * @param rd_len expected read length in bytes.
 * @param wr_ptr cmd buffer.
 * @param wr_len cmd buffer length.
 */
void qspi_standard_read_byte(uint8_t *rd_ptr, uint32_t rd_len, uint8_t *wr_ptr, uint8_t wr_len)
{
    uint16_t i;
    DMA_InitTypeDef config;
    config.device = DMA_DEVICE_QSPI_RX;
    config.type = DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER;
    config.msize = DMA_BURST_TRANSACTION_LENGTH_8;
    config.src_inc = DMA_ADDRESS_UNCHANGE;
    config.dst_inc = DMA_ADDRESS_INCREMENT;
    config.width = DMA_TRANSFER_WIDTH_8_BITS;
    config.int_en = DMA_INTERRUPT_DISABLE;

    LL_QSPI_SlaveSelect(0);
    LL_QSPI_CtrlR0Set(Standard_SPI_Format, DFS_32_8_bits, 0, 0, 0, EEPROM_Read, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI);
    LL_QSPI_CtrlR1Set( rd_len - 1 );
    LL_QSPI_Enable(SSI_Enabled);

    for( i = 0; i < wr_len; i++) {
        LL_QSPI_DataRegSet(*wr_ptr++);
    }

    LL_QSPI_SlaveSelect(QSPI_SLAVE_INDEX);

    while( !LL_QSPI_IsTxFIFOEmpty() );

    if( rd_len <= QSPI_RX_FIFO_DEPTH ) {
        while( !LL_QSPI_IsRxFIFONotEmpty());
        while( LL_QSPI_IsBusy() == SSI_Busy );

        for( i = 0; i < rd_len; i++)
        {
            *rd_ptr++ = LL_QSPI_DataRegGet();
        }
    } else {
        HAL_DMA_Config(DMA_CHANNEL_0, &config);
        LL_QSPI_DMARxDataLvlSet( QSPI_DMA_Receive_Data_Level - 1 );
        LL_QSPI_DMACtrl( Transmit_DMA_Disabled, Receive_DMA_Enabled );
        HAL_DMA_StartTransfer(DMA_CHANNEL_0, (void *)LL_QSPI_DataRegAddrGet(), rd_ptr, rd_len);
        HAL_DMA_WaitDone(DMA_CHANNEL_0);
        LL_QSPI_DMACtrl( Transmit_DMA_Disabled, Receive_DMA_Disabled );
    }

    LL_QSPI_Enable( SSI_Disabled );

}

/**
 * @brief Read data from FLASH via Standard SPI format.
 * 
 * @param rd_ptr read buffer. 
 * @param rd_len_in_word expected read length in WORD size.
 * @param instruction cmd.
 * @param addr address on FLASH.
 */
void qspi_standard_read_word(uint32_t *rd_ptr, uint32_t rd_len_in_word, uint8_t instruction, uint32_t addr)
{
    uint16_t cycles = rd_len_in_word / MAX_DMAC_TRANSFER_SIZE;
    uint16_t tailing_length = rd_len_in_word % MAX_DMAC_TRANSFER_SIZE;
    uint16_t i;
    DMA_InitTypeDef config;

    config.device = DMA_DEVICE_QSPI_RX;
    config.type = DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER;
    config.msize = DMA_BURST_TRANSACTION_LENGTH_8;
    config.src_inc = DMA_ADDRESS_UNCHANGE;
    config.dst_inc = DMA_ADDRESS_INCREMENT;
    config.width = DMA_TRANSFER_WIDTH_32_BITS;
    config.int_en = DMA_INTERRUPT_DISABLE;

    HAL_DMA_Config(DMA_CHANNEL_0, &config);
    LL_QSPI_SlaveSelect(0);
    LL_QSPI_CtrlR0Set(Standard_SPI_Format, DFS_32_32_bits, 0, 0, 0, EEPROM_Read, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI);
    LL_QSPI_CtrlR1Set( rd_len_in_word - 1);
    LL_QSPI_DMARxDataLvlSet( QSPI_DMA_Receive_Data_Level - 1 );
    LL_QSPI_DMACtrl( Transmit_DMA_Disabled, Receive_DMA_Enabled );

    LL_QSPI_SlaveSelect( QSPI_SLAVE_INDEX );

    for( i = 0; i < cycles; i++) {
        LL_QSPI_Enable(SSI_Enabled);
        HAL_DMA_StartTransfer(DMA_CHANNEL_0, (void *)LL_QSPI_DRReversedAddrGet(), rd_ptr, MAX_DMAC_TRANSFER_SIZE);
        LL_QSPI_DataRegSet( (addr & 0x00ffffff) | (instruction << 24) );
        HAL_DMA_WaitDone(DMA_CHANNEL_0);
        LL_QSPI_Enable(SSI_Disabled);
        rd_ptr += MAX_DMAC_TRANSFER_SIZE;
        addr += sizeof(uint32_t)*MAX_DMAC_TRANSFER_SIZE;
    }

    if(tailing_length > 0){
        LL_QSPI_Enable(SSI_Enabled);
        HAL_DMA_StartTransfer(DMA_CHANNEL_0, (void *)LL_QSPI_DRReversedAddrGet(), rd_ptr, tailing_length);
        LL_QSPI_DataRegSet( (addr & 0x00ffffff) | (instruction << 24) );
        HAL_DMA_WaitDone(DMA_CHANNEL_0);
    }
    LL_QSPI_DMACtrl( Transmit_DMA_Disabled, Receive_DMA_Disabled );
    LL_QSPI_Enable(SSI_Disabled);
}

/**
 * @brief Read data via Dual SPI format.
 * 
 * @param rd_ptr 
 * @param rd_len_in_word 
 * @param instruction 
 * @param addr 
 * @param wait_cycles 
 */
void qspi_flash_dual_read_word(uint32_t *rd_ptr, uint32_t rd_len_in_word, uint8_t instruction, uint32_t addr, uint8_t wait_cycles)
{
    uint16_t cycles = rd_len_in_word / MAX_DMAC_TRANSFER_SIZE;
    uint16_t tailing_length = rd_len_in_word % MAX_DMAC_TRANSFER_SIZE;
    uint16_t i;
    DMA_InitTypeDef config;

    config.device = DMA_DEVICE_QSPI_RX;
    config.type = DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER;
    config.msize = DMA_BURST_TRANSACTION_LENGTH_8;
    config.src_inc = DMA_ADDRESS_UNCHANGE;
    config.dst_inc = DMA_ADDRESS_INCREMENT;
    config.width = DMA_TRANSFER_WIDTH_32_BITS;
    config.int_en = DMA_INTERRUPT_DISABLE;

    HAL_DMA_Config(DMA_CHANNEL_0, &config);
    LL_QSPI_SlaveSelect(0);
    LL_QSPI_CtrlR0Set( Dual_SPI_Format, DFS_32_32_bits, 0, 0, 0, Receive_Only, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI );
    LL_QSPI_CtrlR1Set( rd_len_in_word - 1 );
    LL_QSPI_SPICtrl( wait_cycles, INSTRUCTION_LENGTH_8_BITS, ADDR_WIDTH_24_BITS, BOTH_STANDARD_SPI_MODE );
    LL_QSPI_DMARxDataLvlSet( QSPI_DMA_Receive_Data_Level - 1 );
    LL_QSPI_DMACtrl( Transmit_DMA_Disabled, Receive_DMA_Enabled );
    LL_QSPI_Enable( SSI_Enabled );
    LL_QSPI_SlaveSelect( QSPI_SLAVE_INDEX );

    for( i = 0; i < cycles; i++) {
        HAL_DMA_StartTransfer(DMA_CHANNEL_0, (void *)LL_QSPI_DRReversedAddrGet(), rd_ptr, MAX_DMAC_TRANSFER_SIZE);
        LL_QSPI_DataRegSet( instruction );
        LL_QSPI_DataRegSet( addr );
        HAL_DMA_WaitDone(DMA_CHANNEL_0);
        rd_ptr += MAX_DMAC_TRANSFER_SIZE;
        addr += sizeof(uint32_t)*MAX_DMAC_TRANSFER_SIZE;
    }

    if(tailing_length > 0){
        HAL_DMA_StartTransfer(DMA_CHANNEL_0, (void *)LL_QSPI_DRReversedAddrGet(), rd_ptr, tailing_length);
        LL_QSPI_DataRegSet( instruction );
        LL_QSPI_DataRegSet( addr );
        HAL_DMA_WaitDone(DMA_CHANNEL_0);
    }
    LL_QSPI_DMACtrl( Transmit_DMA_Disabled, Receive_DMA_Disabled );
    LL_QSPI_Enable( SSI_Disabled );

}

/**
 * @brief Read data via Quad SPI format.
 * 
 * @param rd_ptr 
 * @param rd_len_in_word 
 * @param instruction 
 * @param addr 
 * @param wait_cycles 
 */
void qspi_flash_quad_read_word(uint32_t *rd_ptr, uint32_t rd_len_in_word, uint8_t instruction, uint32_t addr, uint8_t wait_cycles)
{
    uint16_t cycles = rd_len_in_word / MAX_DMAC_TRANSFER_SIZE;
    uint16_t tailing_length = rd_len_in_word % MAX_DMAC_TRANSFER_SIZE;
    uint16_t i;
    DMA_InitTypeDef config;
    config.device = DMA_DEVICE_QSPI_RX;
    config.type = DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER;
    config.msize = DMA_BURST_TRANSACTION_LENGTH_8;
    config.src_inc = DMA_ADDRESS_UNCHANGE;
    config.dst_inc = DMA_ADDRESS_INCREMENT;
    config.width = DMA_TRANSFER_WIDTH_32_BITS;
    config.int_en = DMA_INTERRUPT_DISABLE;

    HAL_DMA_Config(DMA_CHANNEL_0, &config);
    LL_QSPI_SlaveSelect( 0 );
    LL_QSPI_CtrlR0Set( Quad_SPI_Format, DFS_32_32_bits, 0, 0, 0, Receive_Only, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI );
    LL_QSPI_CtrlR1Set( rd_len_in_word - 1 );
    LL_QSPI_SPICtrl( 8, INSTRUCTION_LENGTH_8_BITS, ADDR_WIDTH_24_BITS, BOTH_STANDARD_SPI_MODE );
    LL_QSPI_DMARxDataLvlSet( QSPI_DMA_Receive_Data_Level - 1 );
    LL_QSPI_DMACtrl( Transmit_DMA_Disabled, Receive_DMA_Enabled );
    LL_QSPI_Enable( SSI_Enabled );
    LL_QSPI_SlaveSelect( QSPI_SLAVE_INDEX );

    for( i = 0; i < cycles; i++ ) {
        HAL_DMA_StartTransfer(DMA_CHANNEL_0, (void *)LL_QSPI_DRReversedAddrGet(), rd_ptr, MAX_DMAC_TRANSFER_SIZE);
        LL_QSPI_DataRegSet( instruction );
        LL_QSPI_DataRegSet( addr );
        HAL_DMA_WaitDone(DMA_CHANNEL_0);
        rd_ptr += MAX_DMAC_TRANSFER_SIZE;
        addr += sizeof(uint32_t)*MAX_DMAC_TRANSFER_SIZE;
    }

    if(tailing_length > 0){
        HAL_DMA_StartTransfer(DMA_CHANNEL_0, (void *)LL_QSPI_DRReversedAddrGet(), rd_ptr, tailing_length);
        LL_QSPI_DataRegSet( instruction );
        LL_QSPI_DataRegSet( addr );
        HAL_DMA_WaitDone(DMA_CHANNEL_0);
    }
    LL_QSPI_DMACtrl( Transmit_DMA_Disabled, Receive_DMA_Disabled );
    LL_QSPI_Enable( SSI_Disabled );

}


/**
 * @brief Write data via Standard SPI format.
 * 
 * @param bufptr 
 * @param length 
 */
void qspi_standard_write(uint8_t *bufptr, uint32_t length)
{
    DMA_InitTypeDef config;
    config.device = DMA_DEVICE_QSPI_TX;
    config.type = DMA_TRANS_TYPE_MEMORY_TO_PERIPHERAL_DMAC_FLOW_CONTROLLER;
    config.msize = DMA_BURST_TRANSACTION_LENGTH_8;
    config.src_inc = DMA_ADDRESS_INCREMENT;
    config.dst_inc = DMA_ADDRESS_UNCHANGE;
    config.width = DMA_TRANSFER_WIDTH_8_BITS;
    config.int_en = DMA_INTERRUPT_DISABLE;

    LL_QSPI_SlaveSelect( 0 );
    LL_QSPI_CtrlR0Set( Standard_SPI_Format, DFS_32_8_bits, 0, 0, 0, Transmit_Only, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI );
    LL_QSPI_Enable( SSI_Enabled );

    if( length <= QSPI_TX_FIFO_DEPTH ) {
        while(length-->0) {
            LL_QSPI_DataRegSet(*bufptr++);
        }
        LL_QSPI_SlaveSelect( QSPI_SLAVE_INDEX );
    } else {
        HAL_DMA_Config(DMA_CHANNEL_0, &config);
        LL_QSPI_DMATxDataLvlSet( QSPI_DMA_Transmit_Data_Level );
        LL_QSPI_DMACtrl( Transmit_DMA_Enabled, Receive_DMA_Disabled );
        LL_QSPI_SlaveSelect( QSPI_SLAVE_INDEX );
        HAL_DMA_StartTransfer(DMA_CHANNEL_0, bufptr, (void *)LL_QSPI_DataRegAddrGet(), length);
        HAL_DMA_WaitDone(DMA_CHANNEL_0);
        LL_QSPI_DMACtrl( Transmit_DMA_Disabled, Receive_DMA_Disabled );
    }
    while( LL_QSPI_IsTxFIFOEmpty() != Transmit_FIFO_Empty );
    while( LL_QSPI_IsBusy() == SSI_Busy );

    LL_QSPI_Enable ( SSI_Disabled );

}

void QSPI_IRQHandler(void)
{
    ;               // we do not use qspi interrupt
}


#pragma pop

