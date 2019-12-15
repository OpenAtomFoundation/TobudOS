/**
 * @file    wm_fls_gd25qxx.h
 *
 * @brief  wm gd25qxx flash driver
 *
 * @author  dave
 *
 * @copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#ifndef TLS_EXSPIFLS_H
#define TLS_EXSPIFLS_H

#define SPI_SCLK							(10000000)  /** 10MHz. */
#define FLASH_TOTAL_SIZE            (1024*1024)
#define PAGE_SIZE						256
#define PROGRAM_SIZE					256
#define PAGE_ADDR_OFFSET				8
#define SECTOR_SIZE						4096


/**
 *  command code define.
 */
#define EXSPIFLASH_WRITE_ENABLE      (0x06)    /** Global write enable */
#define EXSPIFLASH_WRITE_DISABLE      (0x04)   /** Global write disable */
#define EXSPIFLASH_READ_SR1      (0x05)    /** Read flash status register s0~s7 */
#define EXSPIFLASH_READ_SR2      (0x35)    /** Read flash status register s8~s15 */
#define EXSPIFLASH_WRITE_SR      (0x01)    /** Write flash status register s0~s15 */
#define EXSPIFLASH_PAGE_PROGRAM      (0x02)    /** program one page */
#define EXSPIFLASH_DATA_READ      (0x03)   /** read data from specified address */
#define EXSPIFLASH_DATA_FAST_READ      (0x0b)  /** fast read data from specified address */
#define EXSPIFLASH_SECTOR_ERASE      (0x20)    /** Sector erase */
#define EXSPIFLASH_BLOCK32_ERASE      (0x52)   /** 32KB Block erase(128 pages) */
#define EXSPIFLASH_BLOCK64_ERASE      (0xd8)   /** 64kb Block erase(256 pages) */
#define EXSPIFLASH_CHIP_ERASE      (0xc7)  /** Chip erase */
#define EXSPIFLASH_FLASH_DEVICEID      (0x90)  /** Read flash manufacturer/device ID */
#define EXSPIFLASH_FLASH_ID      (0x9f)    /** Read flash ID */


#define FLASH_STATUS_BUSY      (1 << 0)
#define FLASH_STATUS_WEL      (1 << 1)

/**
 * @brief          This function is used to install gd25qxx driver.
 *
 * @param[in]      None
 *
 * @retval         TLS_FLS_STATUS_OK	         if write flash success
 * @retval         TLS_FLS_STATUS_EPERM     if flash struct point is null
 * @retval         TLS_FLS_STATUS_ENODRV	  if flash driver is not installed
 * @retval         TLS_FLS_STATUS_EINVAL	  if argument is invalid
 * @retval         TLS_FLS_STATUS_EIO	         if io error
 * @retval         TLS_FLS_STATUS_EEXIST	  if driver is already existed
 *
 * @note           None
 */
int tls_spifls_drv_install(void);

#endif /* TLS_FLS_GD25QXX_H */
