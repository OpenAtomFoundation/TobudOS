#include "tos_kv.h"
#include "stm32l4xx.h"

#define ONCHIP_FLASH_ADDR_START     0x08000000  // start address for onchip flash for stm32l431RCTX
#define ONCHIP_FLASH_ADDR_MAX       0x0803FFFF  // 256K flash addr for stm32l431RCTX

#define SECTOR_SIZE                 2048         // sector size for stm32l431RCTX
#define SECTOR_SIZE_LOG2            11  // 2 ^ 11 = 2048

#define FOR_KV_FLASH_SIZE           (2 * SECTOR_SIZE)  // storage for kv
#define FOR_KV_FLASH_START          0x803d000

int stm32l4_norflash_onchip_read(uint32_t addr, void *buf, size_t len)
{
    memcpy(buf, (void *)addr, len);
    return 0;
}

int stm32l4_norflash_onchip_write(uint32_t addr, const void *buf, size_t len)
{
    int i = 0;
    uint8_t *array = (uint8_t *)buf;
    HAL_StatusTypeDef hal_status;

    HAL_FLASH_Unlock();
    tos_cpu_int_disable();

    for (i = 0; i < len; i += 8) {
        hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                                        addr + i,
                                        *(uint64_t *)&array[i]);
        if (hal_status != HAL_OK) {
            return -1;
        }
    }

    HAL_FLASH_Lock();
    tos_cpu_int_enable();

    return 0;
}

int stm32l4_norflash_onchip_erase(uint32_t addr, size_t size)
{
    uint32_t page_err = 0;
    HAL_StatusTypeDef hal_status;
	FLASH_EraseInitTypeDef erase_para;

	erase_para.TypeErase    = FLASH_TYPEERASE_PAGES;
    erase_para.Banks        = FLASH_BANK_1;
	erase_para.Page         = addr / SECTOR_SIZE;
	erase_para.NbPages      = size / SECTOR_SIZE;
    HAL_FLASH_Unlock();
    tos_cpu_int_disable();
	hal_status = HAL_FLASHEx_Erase(&erase_para, &page_err);
	HAL_FLASH_Lock();
    tos_cpu_int_enable();

    if (hal_status != HAL_OK) {
        return -1;
    }

    return 0;
}

kv_flash_drv_t stm32l4_norflash_onchip_drv = {
    .write  = stm32l4_norflash_onchip_write,
    .read   = stm32l4_norflash_onchip_read,
    .erase  = stm32l4_norflash_onchip_erase,
};

kv_flash_prop_t stm32l4_norflash_onchip_prop = {
    .sector_size_log2   = SECTOR_SIZE_LOG2,
    .pgm_type           = KV_FLASH_PROGRAM_TYPE_DOUBLEWORD,
    .flash_start        = FOR_KV_FLASH_START,
    .flash_size         = FOR_KV_FLASH_SIZE,
};

