#ifndef __HAL_SPI_FLASH_H__
#define __HAL_SPI_FLASH_H__

#include <stdint.h>

void hal_spi_flash_config(void);

int hal_spi_flash_erase(uint32_t addr, int32_t len);

int hal_spi_flash_write(const void* buf, int32_t len, uint32_t* location);

int hal_spi_flash_erase_write(const void* buf, int32_t len, uint32_t location);

int hal_spi_flash_read(void* buf, int32_t len, uint32_t location);

int hal_spi_flash_get_id(void);

void hal_spi_flash_power_down(void);

void hal_spi_flash_wake_up(void);

#endif  /* _HAL_SPI_FLASH_H_ */

