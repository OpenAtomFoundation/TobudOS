#include "tos_kv.h"
#include "spi_flash.h"

#define SECTOR_SIZE                 4096    // sector size for spiflash
#define SECTOR_SIZE_LOG2            12      // 2 ^ 12 = 4096

int ch32v30x_spiflash_read(uint32_t addr, void *buf, size_t len)
{
    SPI_Flash_Read(buf, addr, len);

    return 0;
}

int ch32v30x_spiflash_write(uint32_t addr, const void *buf, size_t len)
{
    SPI_Flash_Write((void*)buf, addr, len);

    return 0;
}

int ch32v30x_spiflash_erase(uint32_t addr, size_t size)
{
    return SPI_Flash_Erase(addr, size);
}

kv_flash_drv_t ch32v30x_spiflash_drv = {
    .write  = ch32v30x_spiflash_write,
    .read   = ch32v30x_spiflash_read,
    .erase  = ch32v30x_spiflash_erase,
};

kv_flash_prop_t ch32v30x_spiflash_prop = {
    .sector_size_log2   = SECTOR_SIZE_LOG2,
    .pgm_type           = KV_FLASH_PROGRAM_TYPE_BYTE,
};
