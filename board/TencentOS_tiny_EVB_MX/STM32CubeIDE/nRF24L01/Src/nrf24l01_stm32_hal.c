#include "nrf24l01_stm32_hal.h"
#include <string.h>

static nrf_hal_init_t g_nrf_hal;

int nrf_hal_init(void *private) {

    memcpy(&g_nrf_hal, private, sizeof(nrf_hal_init_t));

    nrf_hal_ce(1);
    nrf_hal_csn(1);

    return 0;
}

void nrf_hal_csn(uint8_t mode) {
    HAL_GPIO_WritePin(g_nrf_hal.csn_port, g_nrf_hal.csn_pin, mode == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void nrf_hal_ce(uint8_t mode) {
    HAL_GPIO_WritePin(g_nrf_hal.ce_port, g_nrf_hal.ce_pin, mode == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void nrf_hal_spi_send(uint8_t *buf, uint8_t len) {
	HAL_SPI_Transmit(g_nrf_hal.spi, buf, len, HAL_MAX_DELAY);

}

void nrf_hal_spi_recv(uint8_t *buf, uint8_t len) {

	HAL_SPI_Receive(g_nrf_hal.spi, buf, len, HAL_MAX_DELAY);

}
