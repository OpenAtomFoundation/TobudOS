#ifndef NRF24L01_GD32V_HAL_H_
#define NRF24L01_GD32V_HAL_H_

#include "nrf24l01.h"

typedef struct {
    uint32_t spi;
    uint32_t csn_port;
    uint32_t csn_pin;
    uint32_t ce_port;
    uint32_t ce_pin;
} nrf_hal_init_t;

int  nrf_hal_init(void *nhi);

void nrf_hal_csn(uint8_t mode);

void nrf_hal_ce(uint8_t mode);

void nrf_hal_spi_send(uint8_t *buf, uint8_t len);

void nrf_hal_spi_recv(uint8_t *buf, uint8_t len);

#endif /* NRF24L01_GD32V_HAL_H_ */
