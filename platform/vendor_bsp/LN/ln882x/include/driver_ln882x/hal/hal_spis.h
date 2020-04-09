#ifndef __HAL_SPIS_H__
#define __HAL_SPIS_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"
#include "hal/spi_type.h"

#define RX_FIFO_DEPTH       16
#define TX_FIFO_DEPTH       16

/**
 * @brief This is the struct to initialize the spi slave module.
 */
typedef struct
{
    SPI_Protocol_Type  format;          /**< format: the protocol type of spi */
    SPI_Transmit_Mode tmod;             /**< tmod: the transmit mode of spi */
    SPI_Dataframe_Size dfs;             /**< dfs: set the dataframe size */
    SPI_Controlframe_Size cfs;          /**< cfs: set the control frame size, it is useful when only format = National_Semiconductors_Microwire*/
    SPI_Slave_Output_Enable oe;         /**< oe: spi output enable when work as a slave*/
    SPI_Clock_Polarity scpol;           /**< scpol: set the clock polarity when spi is inactive */
    SPI_Clock_Phase scph;               /**< scph: set the spi clock phase */
} SPIS_InitTypeDef;

/**
 * @brief enum of spi slave int status.
 */
typedef enum
{
    SPIS_INT_TXFIFO_EMPTY     = (1<<0),
    SPIS_INT_TXFIFO_OVERFLOW  = (1<<1),
    SPIS_INT_RXFIFO_UNDERFLOW = (1<<2),
    SPIS_INT_RXFIFO_OVERFLOW  = (1<<3),
    SPIS_INT_RXFIFO_FULL      = (1<<4)
}SPIS_Int_Status;

/**
* @brief enum of spi slave int mask.
*/
typedef enum
{
    SPIS_MASK_TXFIFO_EMPTY     = (1<<0),
    SPIS_MASK_TXFIFO_OVERFLOW  = (1<<1),
    SPIS_MASK_RXFIFO_UNDERFLOW = (1<<2),
    SPIS_MASK_RXFIFO_OVERFLOW  = (1<<3),
    SPIS_MASK_RXFIFO_FULL      = (1<<4)
} SPIS_Int_Mask;

/**
* @brief enum of spi slave status.
*/
typedef enum
{
    SPIS_STATUS_BUSY              = (1<<0),
    SPIS_STATUS_TXFIFO_NOTFULL    = (1<<1),
    SPIS_STATUS_TXFIFO_EMPTY      = (1<<2),
    SPIS_STATUS_RXFIFO_NOTEMPTY   = (1<<3),
    SPIS_STATUS_RXFIFO_FULL       = (1<<4),
    SPIS_STATUS_TRANSMIT_ERROR    = (1<<5),
    SPIS_STATUS_DATA_COLLISION    = (1<<6)
}SPIS_Normal_Status;


/**
 * @brief Initialize spi slave module.
 * @param config: input the spi configure structure to initialize the module.
 * @return This function has no return value.
 */
void HAL_SPIS_Init(SPIS_InitTypeDef config);
/**
 * @brief Enable the spi module.
 * @param en: set to enable or disable spi module
 * @return This function has no return value.
 */
void HAL_SPIS_ENABLE(SPI_En en);
/**
 * @brief Settings of microwire.
 * @param handshake_en: set to enable or disable handshaking interface.
 * @param mode: Defines the direction of the data word when the Microwire serial protocol is used.
 * @param transfer_mode: Defines whether the Microwire transfer is sequential or non-sequential. 0 ï¿½C non-sequential transfer 1 ï¿½C sequential transfer
 * @return This function has no return value.
 */
void HAL_SPIS_Microwire_Setting(uint8_t handshake_en, uint8_t mode, uint8_t transfer_mode);
/**
 * @brief Set the spi fifo threshold.
 * @param tx_thd: tx threshold, the FIFO depth is configurable in the range 2-16
 * @param rx_thd: rx threshold, the FIFO depth is configurable in the range 2-16
 * @return This function has no return value.
 */
void HAL_SPIS_FIFO_Threshold_Set(uint8_t tx_thd, uint8_t rx_thd);
/**
 * @brief Read current tx fifo level.
 * @return return tx fifo level.
 */
uint8_t HAL_SPIS_Current_TxFIFO_Level(void);
/**
 * @brief Read current rx fifo level.
 * @return return rx fifo level.
 */
uint8_t HAL_SPIS_Current_RxFIFO_Level(void);
/**
 * @brief Get normal status(not interrupt status).
 * @return return the status of spim.
 */
uint8_t HAL_SPIS_Normal_Status(void);
uint8_t HAL_SPIS_Is_DataCollision(void);
uint8_t HAL_SPIS_Is_TxError(void);
uint8_t HAL_SPIS_Is_RxFIFO_Full(void);
uint8_t HAL_SPIS_Is_RxFIFO_NotEmpty(void);
uint8_t HAL_SPIS_Is_TxFIFO_Empty(void);
uint8_t HAL_SPIS_Is_TxFIFO_NotFull(void);
uint8_t HAL_SPIS_Is_Busy(void);

/**
 * @brief Set interrupt mask.
 * @param mask: bit map of mask. The input mask is ored from enum SPIM_Int_Mask.
 * @return This function has no return value.
 */
void HAL_SPIS_IntMask_Set(uint8_t mask);
/**
 * @brief Get interrupt mask.
 * @return return the current int mask.
 */
uint8_t HAL_SPIS_IntMask_Get(void);
/**
 * @brief Get interrupt status (after int mask).
 * @return return the int status after int mask.
 */
uint8_t HAL_SPIS_Int_Status(void);
/**
 * @brief Get interrupt raw status (before int mask).
 * @return return the raw int status before int mask.
 */
uint8_t HAL_SPIS_RawInt_Status(void);
/**
 * @brief Get interrupt tx overflow.
 * @return return the tx overflow int status.
 */
uint8_t HAL_SPIS_IntClr_TxOverflow(void);
/**
 * @brief Clear Receive FIFO Overflow Interrupt.
 * @return return the int status of rx overflow
 */
uint8_t HAL_SPIS_IntClr_RxOverflow(void);
/**
 * @brief Clear Receive FIFO Underflow Interrupt.
 * @return return the int status of rx underflow
 */
uint8_t HAL_SPIS_IntClr_RxUnderflow(void);
/**
 * @brief Clear Multi-Master Contention Interrupt.
 * @return return the int status of  Multi-Master Contention Interrupt.
 */
uint8_t HAL_SPIS_IntClr_MultiMaster(void);
/**
 * @brief Get the status and clear int at the same time.  A read clears the ssi_txo_intr, ssi_rxu_intr, ssi_rxo_intr, and the ssi_mst_intr interrupts.
 * @return return the int status of combined Interrupt.
 */
uint8_t HAL_SPIS_IntClr_All(void);       // get the status and clear int at the same time
/**
 * @brief Config DMA of spi, enable or disable them
 * @param tx_dma_en: enable tx dma
 * @param rx_dma_en: enable rx dma
 * @param tx_data_level: This bit field controls the level at which a DMA request is made by the transmit logic. It is equal to the watermark level
 * @param rx_data_level: This bit field controls the level at which a DMA request is made by the transmit logic. It is equal to the watermark level
 * @return This function has no return value.
 */
void HAL_SPIS_DMA_Config(uint8_t tx_dma_en, uint8_t rx_dma_en, uint8_t tx_data_level, uint8_t rx_data_level);
/**
 * @brief Read 1 data
 * @return return the data read
 */
uint32_t HAL_SPIS_Data_Read(void);
/**
 * @brief write 1 data
 * @return This function has no return value.
 */
void HAL_SPIS_Data_Write(uint32_t data);
/**
 * @brief Read specific length of data from rx fifo
 * @return return value.
 */
uint32_t HAL_SPIS_Read_RxFIFO(void);


/**
 * @brief Write specific length of data from tx fifo
 * @param data: data to the write data buffer.
 * @return This function has no return value.
 */
void HAL_SPIS_Write_FIFO(uint32_t data);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __HAL_SPIS_H__
