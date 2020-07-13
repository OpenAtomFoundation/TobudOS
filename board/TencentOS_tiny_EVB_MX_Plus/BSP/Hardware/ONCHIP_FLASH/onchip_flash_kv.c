#include "tos_kv.h"
#include "onchip_flash.h"

#define ONCHIP_FLASH_ADDR_START     0x08000000  // start address for onchip flash for stm32l431RCTX
#define ONCHIP_FLASH_ADDR_MAX       0x0803FFFF  // 256K flash addr for stm32l431RCTX

#define SECTOR_SIZE                 2048         // sector size for stm32l431RCTX
#define SECTOR_SIZE_LOG2            11  // 2 ^ 11 = 2048

kv_flash_drv_t stm32l4_norflash_onchip_drv = {
    .write  = stm32l4_norflash_onchip_write,
    .read   = stm32l4_norflash_onchip_read,
    .erase  = stm32l4_norflash_onchip_erase,
};

kv_flash_prop_t stm32l4_norflash_onchip_prop = {
    .sector_size_log2   = SECTOR_SIZE_LOG2,
    .pgm_type           = KV_FLASH_PROGRAM_TYPE_DOUBLEWORD,
};

