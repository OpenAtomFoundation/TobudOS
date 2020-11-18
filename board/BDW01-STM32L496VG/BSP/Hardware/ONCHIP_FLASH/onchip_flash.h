#ifndef __ONCHIP_FLASH_H__
#define __ONCHIP_FLASH_H__

#include "stdint.h"

int stm32l4_norflash_onchip_read(uint32_t addr, void *buf, size_t len);

int stm32l4_norflash_onchip_write(uint32_t addr, const void *buf, size_t len);

int stm32l4_norflash_onchip_erase(uint32_t addr, size_t size);

#endif /* __ONCHIP_FLASH_H__ */

