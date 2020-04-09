#ifndef __HAL_SPIM__H__
#define __HAL_SPIM__H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"
#include "hal/spi_type.h"

#define RX_FIFO_DEPTH       16
#define TX_FIFO_DEPTH       16

/**
 * @brief enum of SPIM_Slave_Id. We can select from SPI_SLAVE_0 to SPI_SLAVE_2 (We have three cs pin).
 */
typedef enum
{
    SPI_SLAVE_0 = 1,
    SPI_SLAVE_1 = 2,
    SPI_SLAVE_2 = 4
} SPIM_Slave_Id;

/**
 * @brief This is the struct to initialize the spi master module.
 */
typedef struct
{
	uint32_t                speed;      /**< speed: set the spi clock speed */
    SPI_Protocol_Type       format;     /**< format: the protocol type of spi */
    SPI_Dataframe_Size      dfs;        /**< dfs: set the dataframe size */
    SPI_Controlframe_Size   cfs;        /**< cfs: set the control frame size, it is useful when only format = National_Semiconductors_Microwire*/
    SPI_Clock_Polarity      scpol;      /**< scpol: set the clock polarity when spi is inactive */
    SPI_Clock_Phase         scph;       /**< scph: set the spi clock phase */
} SPIM_InitTypeDef;

/**
 * @brief enum of spi master int status.
 */
typedef enum
{
    SPIM_INT_TXFIFO_EMPTY       = (1<<0),
    SPIM_INT_TXFIFO_OVERFLOW    = (1<<1),
    SPIM_INT_RXFIFO_UNDERFLOW   = (1<<2),
    SPIM_INT_RXFIFO_OVERFLOW    = (1<<3),
    SPIM_INT_RXFIFO_FULL        = (1<<4),
    SPIM_INT_MULTI_MASTER       = (1<<5)
} SPIM_Int_Status;

/**
 * @brief enum of spi master int mask.
 */
typedef enum
{
    SPIM_MASK_TXFIFO_EMPTY      = (1<<0),
    SPIM_MASK_TXFIFO_OVERFLOW   = (1<<1),
    SPIM_MASK_RXFIFO_UNDERFLOW  = (1<<2),
    SPIM_MASK_RXFIFO_OVERFLOW   = (1<<3),
    SPIM_MASK_RXFIFO_FULL       = (1<<4),
    SPIM_MASK_MULTI_MASTER      = (1<<5)
} SPIM_Int_Mask;

/**
 * @brief enum of spi master status.
 */
typedef enum
{
    SPIM_STATUS_BUSY            = (1<<0),
    SPIM_STATUS_TXFIFO_NOTFULL  = (1<<1),
    SPIM_STATUS_TXFIFO_EMPTY    = (1<<2),
    SPIM_STATUS_RXFIFO_NOTEMPTY = (1<<3),
    SPIM_STATUS_RXFIFO_FULL     = (1<<4),
    SPIM_STATUS_TRANSMIT_ERROR  = (1<<5),
    SPIM_STATUS_DATA_COLLISION  = (1<<6)
} SPIM_Normal_Status;

/**
 * @brief Initialize spi master module.
 * @param src_clk: input the src_clk of spi module.
 * @param config: input the spi configure structure to initialize the module.
 * @return This function has no return value.
 */
void HAL_SPIM_Init(uint32_t src_clk, SPIM_InitTypeDef config);

/**
 * @brief Read Register through spi interface in polling mode. The function will not return before all data are successfully sent and receive. And then spi is disabled.
          By using this function the spi master work in half-duplex mode(send data and then receive).
 * @param id: select the slave id to operate.
 * @param wr_ptr: pointer to the buffer of output data
 * @param wr_len: data length of output data
 * @param rd_ptr: pointer to the buffer of input data
 * @param rd_len: data length of input data
 * @return This function has no return value.
 */
void HAL_SPIM_Read_Register_Polling(SPIM_Slave_Id id,uint32_t *wr_ptr,uint16_t wr_len,uint32_t *rd_ptr,uint16_t rd_len);

/**
 * @brief Read data from spi interface in polling mode. The function will not return before all data are successfully received. And then spi is disabled.
          By using this function the spi master send dummy data and read specific length of data.
 * @param id: select the slave id to operate.
 * @param rd_ptr: pointer to the buffer of input data
 * @param rd_len: data length of input data
 * @return This function has no return value.
 */
void HAL_SPIM_Read_Only_Polling(SPIM_Slave_Id id, uint32_t *rd_ptr, uint16_t rd_len);

/**
 * @brief write data from spi interface in polling mode. The function will not return before all data are successfully sent. And then spi is disabled.
          By using this function the spi master send specific length of data.
 * @param id: select the slave id to operate.
 * @param wr_ptr: pointer to the buffer of output data
 * @param wr_len: data length of output data
 * @return This function has no return value.
 */
void HAL_SPIM_Write_Polling(SPIM_Slave_Id id, uint32_t *wr_ptr, uint16_t wr_len);

/**
 * @brief Transmit and receive data at the same time. Data are received into fifo and user should enable rx interrupt and get data when interrupt triggered.
          By using this function the spi master send specific length of data and read specific length of data at the same time. Received data can be read when interrupt triggered by using HAL_SPIM_Read_RxFIFO
          It's the users responsible to disable spi when all data flow is finished.
 * @param id: select the slave id to operate.
 * @return This function has no return value.
 */
void HAL_SPIM_Transmit_And_Receive(SPIM_Slave_Id id);

/**
 * @brief Read register and the function return immediately. Data are received into fifo and user should enable rx interrupt and get data when interrupt triggered.
          By using this function the spi master send specific length of data and read specific length of data at the same time. Received data can be read when interrupt triggered by using HAL_SPIM_Read_RxFIFO
          It's the users responsible to disable spi when all data flow is finished.
 * @param id: select the slave id to operate.
 * @param rd_len: data length of input data
 * @return This function has no return value.
 */
void HAL_SPIM_Read_Register(SPIM_Slave_Id id, uint16_t rd_len);

/**
 * @brief Write data and the function return when all data are written into fifo. It's user's responsible to enable the interrupt SPIM_MASK_TXFIFO_EMPTY.
          By using this function the spi master send specific length of data and SPIM_MASK_TXFIFO_EMPTY will trigger. Then user can check status and disable spi.
 * @param id: select the slave id to operate.
 * @return This function has no return value.
 */
void HAL_SPIM_Write(SPIM_Slave_Id id);

/**
 * @brief Read data from spi and the function return immediately. It's user's responsible to enable the interrupt SPIM_INT_RXFIFO_FULL.
          By using this function the spi master read specific length of data and SPIM_INT_RXFIFO_FULL will trigger. Then user can check status and disable spi.
 * @param id: select the slave id to operate.
 * @param rd_len: data length of input data
 * @param dummy_data: value to send as dummy data.
 * @return This function has no return value.
 */
void HAL_SPIM_Read_Only(SPIM_Slave_Id id, uint16_t rd_len, uint32_t dummy_data);

/**
 * @brief Set read data length
 * @param rd_len: number of data to read from rx fifo. rd_len should minus 1. For example, rd_len = 4 means 5 data will receive from spi.
 * @return This function has no return value.
 */
void HAL_SPIM_Set_Read_Len(uint16_t rd_len);

/**
 * @brief Read specific length of data from rx fifo
 * @param rd_ptr: pointer to the receive data buffer.
 * @param rd_len: data length to read in fifo
 * @return This function has no return value.
 */
void HAL_SPIM_Read_RxFIFO(uint32_t *rd_ptr, uint16_t rd_len);

/**
 * @brief Write specific length of data to tx fifo
 * @param wr_ptr: pointer to the transmit data buffer.
 * @param wr_len: data length to write to fifo
 * @return This function has no return value.
 */
void HAL_SPIM_Write_TxFIFO(uint32_t * wr_ptr, uint16_t wr_len);

/**
 * @brief Set Rx Sample delay.
 * @param dly: serveral spi clk to dly
 * @return This function has no return value.
 */
void HAL_SPIM_RxSample_Dly(uint8_t dly);

/**
 * @brief Config DMA of spi, enable or disable them
 * @param tx_dma_en: enable tx dma
 * @param rx_dma_en: enable rx dma
 * @param tx_data_level: This bit field controls the level at which a DMA request is made by the transmit logic. It is equal to the watermark level
 * @param rx_data_level: This bit field controls the level at which a DMA request is made by the transmit logic. It is equal to the watermark level
 * @return This function has no return value.
 */
void HAL_SPIM_Set_DMA(uint8_t tx_dma_en, uint8_t rx_dma_en, uint8_t tx_data_level, uint8_t rx_data_level);

/**
 * @brief Get the status and clear int at the same time.  A read clears the ssi_txo_intr, ssi_rxu_intr, ssi_rxo_intr, and the ssi_mst_intr interrupts.
 * @return return the int status of combined Interrupt.
 */
uint8_t HAL_SPIM_IntClr(void);

/**
 * @brief Clear Multi-Master Contention Interrupt.
 * @return return the int status of Multi-Master Contention Interrupt.
 */
uint8_t HAL_SPIM_IntClr_MST(void);

/**
 * @brief Clear Receive FIFO Underflow Interrupt.
 * @return return the int status of rx underflow.
 */
uint8_t HAL_SPIM_IntClr_RXU(void);

/**
 * @brief Clear Receive FIFO Overflow Interrupt.
 * @return return the int status of rx overflow.
 */
uint8_t HAL_SPIM_IntClr_RXO(void);

/**
 * @brief Clear Transmit FIFO Overflow Interrupt.
 * @return return the int status of tx overflow.
 */
uint8_t HAL_SPIM_IntClr_TXO(void);

/**
 * @brief Get interrupt raw status (before int mask).
 * @return return the raw int status before int mask.
 */
uint8_t HAL_SPIM_RawInt_Status(void);

/**
 * @brief Get interrupt status (after int mask).
 * @return  return the int status after int mask.
 */
uint8_t HAL_SPIM_Int_Status(void);

/**
 * @brief Set interrupt mask.
 * @param mask: bit map of mask. The input mask is ored from enum SPIM_Int_Mask.
 * @return This function has no return value.
 */
void HAL_SPIM_Set_Mask(uint8_t mask);

/**
 * @brief Get interrupt mask.
 * @return return the current int mask.
 */
uint8_t HAL_SPIM_Get_Mask(void);

/**
 * @brief Get normal status(not interrupt status).
 * @return return the status of spim.
 */
uint8_t HAL_SPIM_Normal_Status(void);

/**
 * @brief Read current tx fifo level.
 * @return return tx fifo level.
 */
uint8_t HAL_SPIM_Current_TxFIFO_Level(void);

/**
 * @brief Read current rx fifo level.
 * @return return rx fifo level.
 */
uint8_t HAL_SPIM_Current_RxFIFO_Level(void);

/**
 * @brief Set the spi fifo threshold.
 * @param tx_thd: tx threshold, the FIFO depth is configurable in the range 2-16
 * @param rx_thd: rx threshold, the FIFO depth is configurable in the range 2-16
 * @return This function has no return value.
 */
void HAL_SPIM_Set_FIFO_Threshold(uint8_t tx_thd, uint8_t rx_thd);

/**
 * @brief Set the clock divider of spim
 * @param div: divider of the src clk
 * @return This function has no return value.
 */
void HAL_SPIM_CLK_Div(uint16_t div);

/**
 * @brief Select the slave id.
          When the slave id is select the cs pull low and and data enter tx fifo will send out immediately.
 * @return This function has no return value.
 */
void HAL_SPIM_Slave_Select_En(SPIM_Slave_Id id);

/**
 * @brief Settings of microwire.
 * @param handshake_en: set to enable or disable handshaking interface.
 * @param mode: Defines the direction of the data word when the Microwire serial protocol is used.
 * @param transfer_mode: Defines whether the Microwire transfer is sequential or non-sequential. 0 ï¿½C non-sequential transfer 1 ï¿½C sequential transfer
 * @return This function has no return value.
 */
void HAL_SPIM_Microwire_Setting(uint8_t handshake_en, uint8_t mode, uint8_t transfer_mode);

/**
 * @brief Enable the spi module.
 * @param en: set to enable or disable spi module
 * @return This function has no return value.
 */
void HAL_SPIM_Enable(SPI_En en);

/**
 * @brief Set ctrl0 register of spi master
 * @return This function has no return value.
 */
void HAL_SPIM_Ctrl0(uint8_t dfs,uint8_t cfs,uint8_t srl,uint8_t slv_oe,uint8_t tmod,uint8_t scpol,uint8_t scph,uint8_t frf);

/**
 * @brief Set ctrl1 register of spi master
 * @return This function has no return value.
 */
void HAL_SPIM_Ctrl1(uint16_t ndf);

/**
 * @brief Read 1 data
 * @return return the data read
 */
uint32_t HAL_SPIM_Data_Read(void);

/**
 * @brief write 1 data
 * @return This function has no return value.
 */
void HAL_SPIM_Data_Write(uint32_t data);

/**
 * @brief Get spi busy status.
 * @return return true if spim is busy.
 */
bool HAL_SPIM_Is_Busy(void);

/**
 * @brief Set transfer mode
 * @param mode: transmit mode
 * @return This function has no return value.
 */
void HAL_SPIM_Mode_Set(SPI_Transmit_Mode mode);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __HAL_SPIM__H__
