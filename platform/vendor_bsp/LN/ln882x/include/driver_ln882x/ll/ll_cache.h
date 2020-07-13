#ifndef __LL_CACHE_H__
#define __LL_CACHE_H__

#include "types.h"

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

/****************************  Data types and Macros  ************************/

#define FLASH_QUAD_READ_CMD             0x6b
#define FLASH_STANDARD_READ_CMD         0x03
#define CACHE_LINE_SIZE_IN_BYTES        32

typedef enum
{
    CACHE_STATE_IDLE = 0,
    CACHE_STATE_FILLING
} Cache_State_t;

typedef enum
{
    CACHE_DISABLE = 0,
    CACHE_ENABLE
} Cache_Enable_t;

typedef enum
{
    CACHE_FLUSH_DISABLE = 0,
    CACHE_FLUSH_ENABLE
} Cache_Flush_Enable_t;

typedef enum
{
    CACHE_FLUSH_STATE_DONE = 0,
    CACHE_FLUSH_STATE_INPROGRESS = 1,
} Cache_Flush_State_t;

typedef enum
{
	FLASH_LITTLE_ENDIAN =0,//!< LITTLE_ENDIAN
	FLASH_BIG_ENDIAN       //!< BIG_ENDIAN
} Flash_Output_Data_Format;

typedef enum
{
	RIGHT_JUSTIFIED = 0,
	LEFT_JUSTIFIED
} Cmd_Addr_Alignment;

/****************************  Function Declaratioin  ************************/

/**
 * @brief Get cache state.
 *
 * @return uint8_t cache state: 0 -- idle; 1 -- filling.
 */
uint8_t LL_CACHE_StateGet(void);

/**
 * @brief enable/disable cache function.
 *
 * @param enable 0 -- disable; 1 -- enable
 */
void LL_CACHE_Enable(uint8_t enable);

/**
 * @brief Set FLASH base address.
 *
 * @param addr
 */
void LL_CACHE_FlashBaseAddrSet(uint32_t addr);

/**
 * @brief Get FLASH base address.
 *
 * @return uint32_t
 */
uint32_t LL_CACHE_FlashBaseAddrGet(void);

/**
 * @brief Set QSPI DR register address.
 *
 * @param addr QSPI DR register address.
 */
void LL_CACHE_QspiDRAddrSet(uint32_t addr);

/**
 * @brief Get QSPI DR register address.
 *
 * @return uint32_t
 */
uint32_t LL_CACHE_QspiDRAddrGet(void);

/**
 * @brief Set FLASH cmd/data format.
 *
 * @param tagram_emaw tag ram parameter
 * @param tagram_ema  tag ram parameter
 * @param flash_rd_cmd_format 8bit read command format: 0 -- {24'd0, cmd}, 1 -- {cmd, 24'd0}
 * @param flash_rd_data_format flash output data format: 0 -- little endian. 1 -- big endian.
 * @param flash_rd_addr_format 24bit read address format: 0 -- {8'd0, addr}, 1 -- {addr, 8'd0}
 * @param flash_rd_cmd SPI FLASH read command.
 */
void LL_CACHE_FlashCmdSet(uint8_t tagram_emaw, uint8_t tagram_ema, uint8_t flash_rd_cmd_format,
                            uint8_t flash_rd_data_format, uint8_t flash_rd_addr_format, uint8_t flash_rd_cmd);

/**
 * @brief flush address LOW range
 *
 * @param addrl
 */
void LL_CACHE_FlushAddrLow(uint32_t addrl);

/**
 * @brief flush address high range
 *
 * @param addrh
 */
void LL_CACHE_FlushAddrHigh(uint32_t addrh);

/**
 * @brief flush all data in tag ram.
 *
 * @param isAll
 */
void LL_CACHE_FlushAll(uint8_t isAll);

/**
 * @brief Start flush the range of cache data or all the cache data.
 *
 * @param en
 */
void LL_CACHE_FlushEnable(uint8_t en);

/**
 * @brief Check if the flush operation is done or not.
 *
 * @return uint8_t 1 -- flush in progress; 0 -- flush done.
 */
uint8_t LL_CACHE_IsFlushing(void);

#ifdef __cplusplus
    }
#endif // __cplusplus
#endif // __LL_CACHE_H__
