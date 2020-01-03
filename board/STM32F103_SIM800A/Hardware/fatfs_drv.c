/* Includes ------------------------------------------------------------------*/

//#include "fatfs.h"
#include "fatfs_drv.h"
#include "tos_k.h"
#include <hal_spi_flash.h>
/* Defines ------------------------------------------------------------------*/

#define SPI_FLASH_SECTOR_SIZE   (4 * 1024)
#define SPI_FLASH_PAGE_SIZE     256
 DSTATUS w25q64_fatfs_status(BYTE lun)
{
    DSTATUS status = STA_NOINIT;

    if(SPI_FLASH_ID == hal_spi_flash_get_id())
    {
        status &= ~STA_NOINIT;
    }
    return status;
}

 DSTATUS w25q64_fatfs_initialize(BYTE lun)
{
    DSTATUS status = STA_NOINIT;

    hal_spi_flash_config();
    hal_spi_flash_wake_up();
    status = w25q64_fatfs_status(lun);
    return status;
}

DRESULT w25q64_fatfs_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    int ret;
    ret = hal_spi_flash_read(buff, count * SPI_FLASH_SECTOR_SIZE,
            FF_PHYS_ADDR + sector * SPI_FLASH_SECTOR_SIZE);
    if(ret != 0)
        return RES_ERROR;
    return RES_OK;
}

DRESULT w25q64_fatfs_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    int ret;
    ret = hal_spi_flash_erase_write(buff, count * SPI_FLASH_SECTOR_SIZE,
            FF_PHYS_ADDR + sector * SPI_FLASH_SECTOR_SIZE);
    if(ret != 0)
        return RES_ERROR;
    return RES_OK;
}

 DRESULT w25q64_fatfs_ioctl(BYTE lun, BYTE cmd, void *buff)
{
    DRESULT res = RES_PARERR;

    switch (cmd)
    {
    case GET_SECTOR_COUNT:
        *(DWORD *)buff = FF_PHYS_SIZE / SPI_FLASH_SECTOR_SIZE;
        break;
    case GET_SECTOR_SIZE:
        *(WORD *)buff = SPI_FLASH_SECTOR_SIZE;
        break;
    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 1;
        break;
    }
    res = RES_OK;
    return res;
}




DWORD get_fattime (void)
{
    return 0;
}

/* Private functions --------------------------------------------------------*/
