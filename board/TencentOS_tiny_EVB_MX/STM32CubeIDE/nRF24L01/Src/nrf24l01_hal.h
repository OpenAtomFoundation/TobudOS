#ifndef NRF24L01_HAL_H_
#define NRF24L01_HAL_H_

#define __SI24R1__

#ifdef STM32L431xx
#include "stm32l4xx_hal.h"
#endif

int nrf_hal_init(SPI_HandleTypeDef* spi, GPIO_TypeDef* csn_gpio_port, uint16_t csn_pin, GPIO_TypeDef* ce_gpio_port, uint16_t ce_pin);

void nrf_hal_csn(uint8_t mode);

void nrf_hal_ce(uint8_t mode);


int nrf_hal_read_reg(uint8_t reg, uint8_t *buf, uint8_t len);

int nrf_hal_read_reg_byte(uint8_t reg, uint8_t *v);

int nrf_hal_write_reg(uint8_t reg, uint8_t *buf, uint8_t len);


int nrf_hal_write_reg_byte(uint8_t reg, uint8_t byte);

int nrf_hal_set_reg_bit(uint8_t reg, uint8_t bit);


int nrf_hal_clear_reg_bit(uint8_t reg, uint8_t bit);

int nrf_hal_cmd_read(uint8_t cmd, uint8_t *data, uint8_t len);

int nrf_hal_cmd_read_byte(uint8_t cmd, uint8_t *data);

int nrf_hal_cmd_write(uint8_t cmd, uint8_t *data, uint8_t len);

int nrf_hal_write_cmd(uint8_t cmd);


#endif /* NRF24L01_HAL_H_ */
