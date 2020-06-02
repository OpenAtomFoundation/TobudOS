#include "ota_flash.h"
#include "onchip_flash.h"

#define ONCHIP_FLASH_ADDR_START     0x08000000  // start address for onchip flash for stm32l431RCTX
#define ONCHIP_FLASH_ADDR_MAX       0x0803FFFF  // 256K flash addr for stm32l431RCTX

#define SECTOR_SIZE                 2048         // sector size for stm32l431RCTX
#define SECTOR_SIZE_LOG2            11  // 2 ^ 11 = 2048

#define FOR_OTA_FLASH_SIZE           (2 * SECTOR_SIZE)  // storage for OTA
#define FOR_OTA_FLASH_START          0x801e000

ota_flash_drv_t stm32l4_norflash_onchip_drv_ota = {
    .write  = stm32l4_norflash_onchip_write,
    .read   = stm32l4_norflash_onchip_read,
    .erase  = stm32l4_norflash_onchip_erase,
};

ota_flash_prop_t stm32l4_norflash_onchip_prop_ota = {
    .sector_size_log2   = SECTOR_SIZE_LOG2,
    .pgm_type           = OTA_FLASH_PROGRAM_TYPE_DOUBLEWORD,
};

