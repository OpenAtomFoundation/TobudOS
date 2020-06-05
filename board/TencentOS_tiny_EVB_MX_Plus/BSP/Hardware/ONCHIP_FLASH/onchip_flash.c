#include "tos_k.h"
#include "stm32l4xx.h"

#define SECTOR_SIZE                 2048         // sector size for stm32l431RCTX

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

