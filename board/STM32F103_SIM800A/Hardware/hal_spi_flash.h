/**@defgroup hal_spi_flash SPI Flash Interface
 * @ingroup hal
 */

#ifndef _HAL_SPI_FLASH_H_
#define _HAL_SPI_FLASH_H_

#include <stdint.h>
#define SPI_FLASH_PAGESIZE                         256
#define SPI_FLASH_SECTOR                           4096

#define SPI_FLASH_TOTAL_SIZE                       0x4000000
//#define  SPI_FLASH_ID                       0xEF3015     //W25X16
//#define  SPI_FLASH_ID                       0xEF4015	    //W25Q16
//#define  SPI_FLASH_ID                        0XEF4017     //W25Q64
//#define  SPI_FLASH_ID                       0XEF4018     //W25Q128
#define SPI_FLASH_ID                               0xEF4017
#if defined(__cplusplus)
extern "C" {
#endif

/**
 *@ingroup hal_spi_flash
 *@brief config the spi flash.
 *
 *@par Description:
 *This API is used to config the spi flash.
 *@attention none.
 *
 *@param none.
 *
 *@retval none.
 *@par Dependency: none.
 *@see none.
 */
void hal_spi_flash_config(void);

/**
 *@ingroup hal_spi_flash
 *@brief erase data of spi flash.
 *
 *@par Description:
 *This API is used to erase data of spi flash.
 *@attention none.
 *
 *@param addr           [IN] the address of the spi flash to erase.
 *@param len            [IN] the number of bytes to be erased from addr.
                             Note that at lease 4K byte will be erased by once.
 *
 *@retval #int          0 if succeed or -1 if failed.
 *@par Dependency: none.
 *@see none.
 */
int hal_spi_flash_erase(uint32_t addr, int32_t len);

/**
 *@ingroup hal_spi_flash
 *@brief write data to spi flash.
 *
 *@par Description:
 *This API is used to write data to spi flash. You should call @hal_spi_flash_erase before this.
 *location is updated by each call so that you don't need to care about write address if you do
 *sequential write.
 *@attention none.
 *
 *@param buf            [IN]     the data to be wrote to spi flash.
 *@param len            [IN]     the length of the buffer.
 *@param location       [IN/OUT] the address of the spi flash to write.
 *
 *@retval #int          0 if succeed or -1 if failed.
 *@par Dependency: none.
 *@see hal_spi_flash_erase.
 */
int hal_spi_flash_write(const void* buf, int32_t len, uint32_t* location);

/**
 *@ingroup hal_spi_flash
 *@brief write data to spi flash.
 *
 *@par Description:
 *This API is used to write data to spi flash. You don't need to erase flash by this interface.
 *@attention none.
 *
 *@param buf            [IN] the data to be wrote to spi flash.
 *@param len            [IN] the length of the buffer.
 *@param location       [IN] the address of the spi flash to write.
 *
 *@retval #int          0 if succeed or -1 if failed.
 *@par Dependency: none.
 *@see none.
 */
int hal_spi_flash_erase_write(const void* buf, int32_t len, uint32_t location);

/**
 *@ingroup hal_spi_flash
 *@brief read data from spi flash.
 *
 *@par Description:
 *This API is used to read data from spi flash.
 *@attention none.
 *
 *@param buf            [OUT] buffer to store the data read from spi flash.
 *@param len            [IN]  the length of the buffer.
 *@param location       [IN]  the address of the spi flash to read.
 *
 *@retval #int          0 if succeed or -1 if failed.
 *@par Dependency: none.
 *@see none.
 */
int hal_spi_flash_read(void* buf, int32_t len, uint32_t location);

/**
 *@ingroup hal_spi_flash
 *@brief get ID of the target spi flash.
 *
 *@par Description:
 *This API is used to get ID of the target spi flash.
 *@attention none.
 *
 *@param none.
 *
 *@retval #int          ID of the spi flash.
 *@par Dependency: none.
 *@see none.
 */
int hal_spi_flash_get_id(void);

/**
 *@ingroup hal_spi_flash
 *@brief power down the spi flash.
 *
 *@par Description:
 *This API is used to power down the spi flash.
 *@attention none.
 *
 *@param none.
 *
 *@retval none.
 *@par Dependency: none.
 *@see none.
 */
void hal_spi_flash_power_down(void);

/**
 *@ingroup hal_spi_flash
 *@brief wake up the spi flash.
 *
 *@par Description:
 *This API is used to wake up the spi flash.
 *@attention none.
 *
 *@param none.
 *
 *@retval none.
 *@par Dependency: none.
 *@see none.
 */
void hal_spi_flash_wake_up(void);

#if defined(__cplusplus)
}
#endif

#endif  /* _HAL_SPI_FLASH_H_ */
