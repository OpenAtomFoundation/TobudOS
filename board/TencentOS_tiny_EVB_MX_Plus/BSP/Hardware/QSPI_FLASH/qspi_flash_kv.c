#include "tos_kv.h"
#include "hal_qspi_flash.h"

#define SECTOR_SIZE                 4096    // sector size for qspiflash
#define SECTOR_SIZE_LOG2            12  // 2 ^ 12 = 4096

#define FOR_KV_FLASH_SIZE           (2 * SECTOR_SIZE)
#define FOR_KV_FLASH_START          0x0

int stm32l4_qspiflash_read(uint32_t addr, void *buf, size_t len)
{
    return hal_spi_flash_read(buf, len, addr);
}

int stm32l4_qspiflash_write(uint32_t addr, const void *buf, size_t len)
{
    uint32_t location = addr;

    return hal_spi_flash_write(buf, len, &location);
}

int stm32l4_qspiflash_erase(uint32_t addr, size_t size)
{
    return hal_spi_flash_erase(addr, size);
}

kv_flash_drv_t stm32l4_qspiflash_drv = {
    .write  = stm32l4_qspiflash_write,
    .read   = stm32l4_qspiflash_read,
    .erase  = stm32l4_qspiflash_erase,
};

kv_flash_prop_t stm32l4_qspiflash_prop = {
    .sector_size_log2   = SECTOR_SIZE_LOG2,
    .pgm_type           = KV_FLASH_PROGRAM_TYPE_BYTE,
    .flash_start        = FOR_KV_FLASH_START,
    .flash_size         = FOR_KV_FLASH_SIZE,
};


