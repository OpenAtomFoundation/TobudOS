#ifndef __EXFLASH_H__
#define __EXFLASH_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

///////////////////////////////  External FLASH  //////////////////////////////
#define EXFLASH_PAGE_PROGRAM            0x02
#define EXFLASH_READ_DATA_BYTES         0x03
#define EXFLASH_WRITE_DISABLE           0x04
#define EXFLASH_READ_STATUS_REG_LOW     0x05
#define EXFLASH_WRITE_ENABLE            0x06
#define EXFLASH_FAST_READ               0x0B
#define EXFLASH_SECTOR_ERASE            0x20
#define EXFLASH_READ_STATUS_REG_HIGH    0x35
#define EXFLASH_BLOCK_ERASE_32KB        0x52
#define EXFLASH_BLOCK_ERASE_64KB        0xD8
#define EXFLASH_CHIP_ERASE              0x60
#define EXFLASH_READ_MANUFACTURER_ID    0x90
#define EXFLASH_READ_IDENTIFICATION     0x9F

#define EXFLASH_PAGE_SIZE       (256)
#define EXFALSH_SIZE_4K         (4*1024)
#define EXFALSH_SIZE_32K        (32*1024)
#define EXFALSH_SIZE_64K		(64*1024)
#define EXFALSH_SIZE_MAX		(2*1024*1024)

//////////////////////////// Function Declarations  ///////////////////////////

/**
 * @brief Configurate baudrate of the External FLASH interface.
 *
 * @param baud like 1000000
 */
void ExFlash_Init(uint32_t baud);

/**
 * @brief Deinit the interface of FLASH.
 */
void ExFlash_Deinit(void);

/**
 * @brief Read Manufacture ID/Device ID (REMS) (90H)
 *
 * @return uint16_t MSB is Manufacturer ID, LSB is Device ID.
 */
uint16_t ExFlash_Read_ManufactureID(void);

/**
 * @brief Read FLASH Status Register all 16 bits.
 *
 * @return uint16_t FLASH status.
 */
uint16_t ExFlash_Read_Status(void);

/**
 * @brief Read Identification (RDID, 0x9F)
 *
 * @return uint32_t identification
 */
uint32_t ExFlash_Read_Identification(void);

/**
 * @brief The WREN command is for setting the Write Enable Latch bit, which
 * must be set prior to every Page Program, Sector Erase, Block Erase, Chip
 * Erase and Write Status Register command.
 *
 */
void ExFlash_Write_Enable(void);

/**
 * @brief The WRDI command is for setting the Write Enable Latch (WEL) bit.
 * The WEL bit is reset by following condition: Power-up and upon completion
 * of the Write Status Register, Page Program, Sector Erase, Block Erase and
 * Chip Erase commands.
 *
 */
void ExFlash_Write_Disable(void);

/**
 * @brief Read data from FLASH.
 *
 * @param addr start address
 * @param length how many bytes to read
 * @param recv_buf receive buffer
 * @return int8_t return 0 on success; return others on failure.
 */
int8_t ExFlash_Read_Data_Bytes(uint32_t addr, uint8_t *recv_buf, uint32_t length);

/**
 * @brief Erase all data of the chip.
 */
void ExFlash_Chip_Erase(void);

/**
 * @brief Program one page.
 *
 * @param addr  start of address of one page.
 * @param data  data
 * @param length length of data.
 */
void ExFlash_Page_Program(uint32_t addr, uint8_t *data, uint16_t length);

/**
 * @brief Program at any address inside the FLASH address space.
 *
 * @param offset start address
 * @param length how many bytes to program
 * @param buffer start address of the buffer
 */
void ExFlash_Program(uint32_t offset, uint8_t *buffer, uint32_t length);

/**
 * @brief Read Data Bytes at Higher Speed (Fast Read).
 *
 * @param addr start address to read
 * @param length how many bytes to read
 * @param recv_buf receive buffer
 * @return int8_t return 0 on success; return others on failure.
 */
int8_t ExFlash_Fast_Read(uint32_t addr, uint8_t *recv_buf, uint32_t length);

/**
 * @brief Erase an sector at which the addr is located.
 *
 * @param addr Any addrss inside the sector is a valid address for
 *              the Sector Erase command.
 */
void ExFlash_Erase_Sector(uint32_t addr);

/**
 * @brief Block erase by 32KB or 64KB according to type.
 *
 * @param addr Any address inside the block is a valid address for the block erase command.
 * @param type to indicate a 32KB or 64KB block erase.
 */
void ExFlash_Erase_Block(uint32_t addr, uint8_t type);

/**
 * @brief Erase flash start from offset.
 *
 * @param offset start address inside the external FLASH.
 * @param length how many bytes to erase.
 */
void ExFlash_Erase(uint32_t offset, uint32_t length);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __EXFLASH_H__
