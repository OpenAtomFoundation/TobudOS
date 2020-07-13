#ifndef __LL_QSPI_H__
#define __LL_QSPI_H__

#include "types.h"

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

/***********************************  Macros  ********************************/

// Bitmap for SR (Status Register)
#define LL_QSPI_STATUS_DCOL    0x40
#define LL_QSPI_STATUS_TXE     0x20
#define LL_QSPI_STATUS_RFF     0x10
#define LL_QSPI_STATUS_FRNE    0x08
#define LL_QSPI_STATUS_TFE     0x04
#define LL_QSPI_STATUS_TFNF    0x02
#define LL_QSPI_STATUS_BUSY    0x01

// Bitmap for IMR (Interrupt Mask Register)
#define LL_QSPI_INT_MASK_MSTIM     0x20
#define LL_QSPI_INT_MASK_RXFIM     0x10
#define LL_QSPI_INT_MASK_RXOIM     0x08
#define LL_QSPI_INT_MASK_RXUIM     0x04
#define LL_QSPI_INT_MASK_TXOIM     0x02
#define LL_QSPI_INT_MASK_TXEIM     0x01

// Bitmap for ISR (Interrupt Status Register)
#define LL_QSPI_INT_STATUS_MSTIS   0x20
#define LL_QSPI_INT_STATUS_RXFIS   0x10
#define LL_QSPI_INT_STATUS_RXOIS   0x08
#define LL_QSPI_INT_STATUS_RXUIS   0x04
#define LL_QSPI_INT_STATUS_TXOIS   0x02
#define LL_QSPI_INT_STATUS_TXEIS   0x01

// Bitmap for RISR (Raw Interrupt Status Register)
#define LL_QSPI_RAWINT_STATUS_MSTIR    0x20
#define LL_QSPI_RAWINT_STATUS_RXFIR    0x10
#define LL_QSPI_RAWINT_STATUS_RXOIR    0x08
#define LL_QSPI_RAWINT_STATUS_RXUIR    0x04
#define LL_QSPI_RAWINT_STATUS_TXOIR    0x02
#define LL_QSPI_RAWINT_STATUS_TXEIR    0x01

// SPI_CTRLR0

/**
 * @brief Dual/Quad mode instruction length in bits.
 */
#define LL_QSPI_INSTRUCTION_LEN_0_BIT      0x00
#define LL_QSPI_INSTRUCTION_LEN_4_BITS     0x01
#define LL_QSPI_INSTRUCTION_LEN_8_BITS     0x02
#define LL_QSPI_INSTRUCTION_LEN_16_BITS    0x03

/**
 * @brief Length of address to be transmitted.
 */
#define LL_QSPI_ADDR_LEN_0_BIT      0x00
#define LL_QSPI_ADDR_LEN_4_BIT      0x01
#define LL_QSPI_ADDR_LEN_8_BIT      0x02
#define LL_QSPI_ADDR_LEN_12_BIT     0x03
#define LL_QSPI_ADDR_LEN_16_BIT     0x04
#define LL_QSPI_ADDR_LEN_20_BIT     0x05
#define LL_QSPI_ADDR_LEN_24_BIT     0x06
#define LL_QSPI_ADDR_LEN_28_BIT     0x07
#define LL_QSPI_ADDR_LEN_32_BIT     0x08
#define LL_QSPI_ADDR_LEN_36_BIT     0x09
#define LL_QSPI_ADDR_LEN_40_BIT     0x0A
#define LL_QSPI_ADDR_LEN_44_BIT     0x0B
#define LL_QSPI_ADDR_LEN_48_BIT     0x0C
#define LL_QSPI_ADDR_LEN_52_BIT     0x0D
#define LL_QSPI_ADDR_LEN_56_BIT     0x0E
#define LL_QSPI_ADDR_LEN_60_BIT     0x0F

/**
 * @brief SPI_CTRLR0_TRANS_TYPE
 */
#define LL_QSPI_BOTH_STANDARD_SPI_MODE                  0
#define LL_QSPI_INSTRUCTION_STANDARD_ADDRESS_SPECIFIC   1
#define LL_QSPI_BOTH_SPECIFIC_MODE                      2


/***************************  Function Declarations  *************************/

/**
 * @brief Control the serial data transfer. It is impossible to write to this
 * register when the QSPI controller is enabled.
 *
 * @param spi_frf SPI Frame Format. @see SPI_Format
 * @param dfs Data Frame Size in 32-bit mode. @see SPI_DataFrame_Size
 * @param cfs Control Frame Size. Selects the length of the control word for
 *          the Microwire frame format. @see SPI_Controlframe_Size
 * @param srl Shift Register Loop. Always set to zero.
 * @param slv_oe Slave Output Enable. Only used when in serial-slave device.
 * @param tmod Transfer Mode. Selects the mode of transfer for serial comunication.
 *          Only indicates whether the receive or transmit data are valid. @see SPI_Transmit_Mode
 * @param scpol Serial Clock Polarity. Valid when the frame format (FRF) is set
 *          to Motorola SPI. @see SPI_Clock_Polarity.
 * @param scph Serial Clock Phase. Valid when the frame format (FRF) is set to
 *          Motorola SPI. @see SPI_Clock_Phase.
 * @param frf Frame Format. Selects which serial protocol transfers the data.
 *          @see SPI_Protocol_Type.
 */
void LL_QSPI_CtrlR0Set( uint8_t spi_frf, uint8_t dfs, uint8_t cfs,
                        uint8_t srl, uint8_t slv_oe, uint8_t tmod,
                        uint8_t scpol, uint8_t scph, uint8_t frf);


/**
 * @brief Control register 1 controls the end of serial transfers when in
 * receive-only mode.
 *
 * @param ndf Number of Data Frames. When TMOD = 10 or 11, this register field
 * sets the number of data frames to be continuously received.
 */
void LL_QSPI_CtrlR1Set(uint16_t ndf);

/**
 * @brief
 *
 * @param ndf
 */
void LL_QSPI_NumOfDataFrameSet(uint16_t ndf);

uint16_t LL_QSPI_NumOfDataFrameGet(void);

/**
 * @brief SSI Enable.
 * When disabled, all serial transfers are halted immediately. Transmit and receive
 * FIFO buffers are cleared when the device is disabled. It is impossible to
 * program some control registers when enabled.
 *
 * @param enable 0 for disable; 1 for enable.
 */
void LL_QSPI_Enable(uint8_t enable);

/**
 * @brief Microwire Control.
 * Controls the direction of the data word for the half-duplex Microwire serial protocol.
 *
 * @param mhs Microwire Handshaking. 0 -- handshaking disabled; 1 -- handshaking enabled.
 * @param mod Microwire Control. 0 -- receive data word; 1 -- send data word.
 * @param mwmod Microwire Transfer Mode. 0 -- non-sequential; 1 -- sequential transfer.
 */
void LL_QSPI_MicrowireCtrl(uint8_t mhs, uint8_t mod, uint8_t mwmod);

/**
 * @brief Slave Select Enable Flag.
 *
 * @param slave_index 1bit for one slave; 0 -- not selected.
 */
void LL_QSPI_SlaveSelect(uint8_t slave_index);

/**
 * @brief SSI Clock Divider.
 *
 * @param sckdiv
 */
void LL_QSPI_SckDivSet(uint16_t sckdiv);

/**
 * @brief Transmit FIFO Threshold Level
 * Controls the level of entries (or below) at which the transmit FIFO controller
 * triggers an interrupt. The FIFO depth is configurable in the range 2~256.
 *
 * @param lvl
 */
void LL_QSPI_TxFIFOThresholdLvlSet(uint8_t lvl);

/**
 * @brief Receive FIFO Threshold Level
 * Controls the level of entries (or above) at which the recieve FIFO controller
 * triggers an interrupt. The FIFO depth is configurable in the range 2~256.
 *
 * @param lvl
 */
void LL_QSPI_RxFIFOThresholdLvlSet(uint8_t lvl);

/**
 * @brief Transmit FIFO Level
 *
 * @return uint8_t
 */
uint8_t LL_QSPI_TxFIFOLvlGet(void);

/**
 * @brief Receive FIFO Level
 *
 * @return uint8_t
 */
uint8_t LL_QSPI_RxFIFOLvlGet(void);

/**
 * @brief Indicates the current transfer status, FIFO status, and any transmission
 * or reception errors that may have occured. The status register may be read at
 * any time. None of the bits in this register request an interrupt.
 *
 * @return uint8_t
 */
uint8_t LL_QSPI_Status(void);

uint8_t LL_QSPI_IsDataCollision(void);

uint8_t LL_QSPI_IsTxError(void);

uint8_t LL_QSPI_IsRxFIFOFull(void);

uint8_t LL_QSPI_IsRxFIFONotEmpty(void);

uint8_t LL_QSPI_IsTxFIFOEmpty(void);

uint8_t LL_QSPI_IsTxFIFONotFull(void);

uint8_t LL_QSPI_IsBusy(void);

/**
 * @brief Disable all interruts.
 *
 */
void LL_QSPI_IntMaskAll(void);

/**
 * @brief Enable all interrupts.
 *
 */
void LL_QSPI_IntUnMaskAll(void);

/**
 * @brief Disable particular interrupt.
 *
 * @param intMaskBit @see LL_QSPI_INT_MASK_MSTIM
 */
void LL_QSPI_IntMask(uint8_t intMaskBit);

/**
 * @brief Enable particular interrupts.
 *
 * @param intMaskBit @see LL_QSPI_INT_MASK_MSTIM
 */
void LL_QSPI_IntUnMask(uint8_t intMaskBit);

/**
 * @brief Get Interrupt status.
 *
 * @return uint8_t interrupt status after they have been masked.
 */
uint8_t LL_QSPI_IntStatus(void);

/**
 * @brief Get raw interrupt status.
 *
 * @return uint8_t raw interrupts prior to masking.
 */
uint8_t LL_QSPI_RawIntStatus(void);

/**
 * @brief Clear Transmit FIFO Overflow Interrupt.
 *
 */
void LL_QSPI_IntClearTxFIFOOverflow(void);

/**
 * @brief Clear Receive FIFO Overflow Interrupt.
 *
 */
void LL_QSPI_IntClearRxFIFOOverflow(void);

/**
 * @brief Clear Receive FIFO Underflow Interrupt.
 *
 */
void LL_QSPI_IntClearRxFIFOUnderflow(void);

/**
 * @brief Clear Multi-Master Contention Interrupt.
 *
 */
void LL_QSPI_IntClearMultiMasterContention(void);

/**
 * @brief Clear Interrupts.
 *
 */
void LL_QSPI_IntClearAll(void);

/**
 * @brief DMA Control
 *
 * @param tdmae Transmit DMA Enable
 * @param rdmae Receive DMA Enable
 */
void LL_QSPI_DMACtrl(uint8_t tdmae, uint8_t rdmae);

/**
 * @brief DMA Transmit Data Level.
 *
 * @param level
 */
void LL_QSPI_DMATxDataLvlSet(uint8_t level);

/**
 * @brief DMA Receive Data Level.
 *
 * @param level
 */
void LL_QSPI_DMARxDataLvlSet(uint8_t level);

/**
 * @brief Data Register
 *
 * @return uint32_t
 */
uint32_t LL_QSPI_DataRegGet(void);

void LL_QSPI_DataRegSet(uint32_t val);

/**
 * @brief Return DR register address.
 * 
 * @return uint32_t* address of DR register.
 */
uint32_t * LL_QSPI_DataRegAddrGet(void);

/**
 * @brief Get address of DR_Reversed Register.
 *
 * @return uint32_t*
 */
uint32_t * LL_QSPI_DRReversedAddrGet(void);

void LL_QSPI_DRReversedSet(uint32_t val);

uint32_t LL_QSPI_DRReversedGet(void);

/**
 * @brief Set Rx Sample Delay.
 *
 * @param rsd
 */
void LL_QSPI_RxSampleDlySet(uint8_t rsd);

/**
 * @brief Control the serial data transfer in SPI mode of operation.
 *
 * @param wait_cycles defines the wait cycles in dual/quad mode between control
 *          frames transmit and data reception.
 * @param inst_len Dual/Quad mode instruction length in bits. @see LL_QSPI_INST_LEN_0_BIT
 * @param addr_len defines length of address to be transmitted. @see LL_QSPI_ADDR_LEN_0_BIT
 * @param trans_type Address and instruction transfer format.
 */
void LL_QSPI_SPICtrl(uint8_t wait_cycles, uint8_t inst_len, uint8_t addr_len, uint8_t trans_type);




#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __LL_QSPI_H__
