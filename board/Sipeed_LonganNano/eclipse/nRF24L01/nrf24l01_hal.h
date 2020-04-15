#ifndef NRF24L01_HAL_H_
#define NRF24L01_HAL_H_

#include "gd32vf103.h"

typedef struct {
    uint32_t spi;
    uint32_t csn_port;
    uint32_t csn_pin;
    uint32_t ce_port;
    uint32_t ce_pin;
} nrf_hal_init_t;

int nrf_hal_init(nrf_hal_init_t *private);

void nrf_hal_csn(uint8_t mode);

void nrf_hal_ce(uint8_t mode);


int nrf_hal_read_reg(uint8_t reg, uint8_t *buf, uint8_t len);

int nrf_hal_read_reg_byte(uint8_t reg, uint8_t *v);

int nrf_hal_write_reg(uint8_t reg, uint8_t *buf, uint8_t len);

int nrf_hal_write_reg_byte(uint8_t reg, uint8_t byte);

int nrf_hal_cmd_read(uint8_t cmd, uint8_t *data, uint8_t len);

int nrf_hal_cmd_read_byte(uint8_t cmd, uint8_t *data);

int nrf_hal_cmd_write(uint8_t cmd, uint8_t *data, uint8_t len);

int nrf_hal_write_cmd(uint8_t cmd);


#endif /* NRF24L01_HAL_H_ */
