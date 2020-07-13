#ifndef __QSPI_H__
#define __QSPI_H__

#include <stdint.h>
#include "proj_config.h"

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

/***************************  Data types and Macros  *************************/

#define QSPI_READ_MAX_LENGTH            0x10000
#define QSPI_TX_FIFO_DEPTH              16
#define QSPI_RX_FIFO_DEPTH              16
#define QSPI_SLAVE_INDEX                1
#define QSPI_DMA_Receive_Data_Level     8  //match with DMAC.CTL.SRC_MSIZE
#define QSPI_DMA_Transmit_Data_Level    8  //match with DMAC.CTL.DST_MSIZE

typedef enum
{
    INSTRUCTION_LENGTH_0_BIT = 0,
    INSTRUCTION_LENGTH_4_BITS,
    INSTRUCTION_LENGTH_8_BITS,
    INSTRUCTION_LENGTH_16_BITS,
} SPI_CTRLR0_INST_L_FIELD;

typedef enum
{
    ADDR_WIDTH_0_BIT = 0,
    ADDR_WIDTH_4_BITS,
    ADDR_WIDTH_8_BITS,
    ADDR_WIDTH_12_BITS,
    ADDR_WIDTH_16_BITS,
    ADDR_WIDTH_20_BITS,
    ADDR_WIDTH_24_BITS,
    ADDR_WIDTH_28_BITS,
    ADDR_WIDTH_32_BITS,
    ADDR_WIDTH_36_BITS,
    ADDR_WIDTH_40_BITS,
    ADDR_WIDTH_44_BITS,
    ADDR_WIDTH_48_BITS,
    ADDR_WIDTH_52_BITS,
    ADDR_WIDTH_56_BITS,
    ADDR_WIDTH_60_BITS
} SPI_CTRLR0_ADDR_L_FIELD;

typedef enum
{
    BOTH_STANDARD_SPI_MODE = 0,
    INSTRUCTION_STANDARD_ADDRESS_SPECIFIC,
    BOTH_SPECIFIC_MODE,
} SPI_CTRLR0_TRANS_TYPE_FIELD;

/********************************  sanity check  *****************************/
// TODO: add sanity check here.

/****************************  Function Declaration  *************************/

/**
 * @brief QSPI init.
 * 
 * @param clk_divider 
 * @param rx_sample_dly 
 */
void qspi_init(uint16_t clk_divider, uint8_t rx_sample_dly);

/**
 * @brief QSPI deinit.
 */
void qspi_deinit(void);

/**
 * @brief Write data via Standard SPI format.
 * 
 * @param bufptr 
 * @param length 
 */
void qspi_standard_write(uint8_t *bufptr, uint32_t length);

/**
 * @brief Send a cmd, which is represented by `wr_ptr`, and wait for data, which is represented by `rd_ptr`.
 * 
 * @param rd_ptr read buffer.
 * @param rd_len expected read length in bytes.
 * @param wr_ptr cmd buffer.
 * @param wr_len cmd buffer length.
 */
void qspi_standard_read_byte(uint8_t *rd_ptr, uint32_t rd_len, uint8_t *wr_ptr, uint8_t wr_len);

/**
 * @brief Read data from FLASH via Standard SPI format.
 * 
 * @param rd_ptr read buffer. 
 * @param rd_len_in_word expected read length in WORD size.
 * @param instruction cmd.
 * @param addr address on FLASH.
 */
void qspi_standard_read_word(uint32_t *rd_ptr, uint32_t rd_len_in_word, uint8_t instruction, uint32_t addr);

/**
 * @brief Read data via Dual SPI format.
 * 
 * @param rd_ptr 
 * @param rd_len_in_word 
 * @param instruction 
 * @param addr 
 * @param wait_cycles 
 */
void qspi_flash_dual_read_word(uint32_t *rd_ptr, uint32_t rd_len_in_word, uint8_t instruction, uint32_t addr, uint8_t wait_cycles);

/**
 * @brief Read data via Quad SPI format.
 * 
 * @param rd_ptr 
 * @param rd_len_in_word 
 * @param instruction 
 * @param addr 
 * @param wait_cycles 
 */
void qspi_flash_quad_read_word(uint32_t *rd_ptr, uint32_t rd_len_in_word, uint8_t instruction, uint32_t addr, uint8_t wait_cycles);

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus
#endif // __QSPI_H__
