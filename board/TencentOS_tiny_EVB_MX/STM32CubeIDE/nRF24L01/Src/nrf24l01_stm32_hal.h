#ifndef NRF24L01_STM32_HAL_H_
#define NRF24L01_STM32_HAL_H_

#include <nrf24l01.h>

#ifdef STM32L431xx
#include "stm32l4xx_hal.h"
#endif

typedef struct {
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef* csn_port;
    uint16_t      csn_pin;
    GPIO_TypeDef* ce_port;
    uint16_t      ce_pin;
} nrf_hal_init_t;

int  nrf_hal_init(void *nhi);

void nrf_hal_csn(uint8_t mode);

void nrf_hal_ce(uint8_t mode);

void nrf_hal_spi_send(uint8_t *buf, uint8_t len);

void nrf_hal_spi_recv(uint8_t *buf, uint8_t len);


#endif /* NRF24L01_STM32_HAL_H_ */
