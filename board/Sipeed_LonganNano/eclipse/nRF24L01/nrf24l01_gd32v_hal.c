#include "nrf24l01_gd32v_hal.h"
#include "gd32vf103_gpio.h"
#include <string.h>

static nrf_hal_init_t g_nrf_hal;

int nrf_hal_init(void *private) {

    memcpy(&g_nrf_hal, private, sizeof(nrf_hal_init_t));

    gpio_init(g_nrf_hal.ce_port,  GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, g_nrf_hal.ce_pin);
    gpio_init(g_nrf_hal.csn_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, g_nrf_hal.csn_pin);

    nrf_hal_ce(1);
    nrf_hal_csn(1);

    return 0;
}

void nrf_hal_csn(uint8_t mode) {
    gpio_bit_write(g_nrf_hal.csn_port, g_nrf_hal.csn_pin, mode == 0 ? RESET : SET);
}

void nrf_hal_ce(uint8_t mode) {
	gpio_bit_write(g_nrf_hal.ce_port, g_nrf_hal.ce_pin, mode == 0 ? RESET : SET);
}

uint8_t _spi_transfer(uint32_t spi, uint8_t data) {
    while(RESET == spi_i2s_flag_get(spi, SPI_FLAG_TBE));
    spi_i2s_data_transmit(spi, data);

    while(RESET == spi_i2s_flag_get(spi, SPI_FLAG_RBNE));
    data = spi_i2s_data_receive(spi);

    return data;
}


void nrf_hal_spi_send(uint8_t *buf, uint8_t len) {
    for(uint8_t i=0; i<len; i++) {
        _spi_transfer(g_nrf_hal.spi, buf[i]);
    }
}
void nrf_hal_spi_recv(uint8_t *buf, uint8_t len) {
    for(uint8_t i=0; i<len; i++) {
        buf[i] = _spi_transfer(g_nrf_hal.spi, 0xFF);
    }
}
