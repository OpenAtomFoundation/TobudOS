#include "nrf24l01.h"

static SPI_HandleTypeDef *nrf_spi;
static GPIO_TypeDef* nrf_csn_gpio_port;
static uint16_t      nrf_csn_pin;
static GPIO_TypeDef* nrf_ce_gpio_port;
static uint16_t      nrf_ce_pin;


int nrf_hal_init(SPI_HandleTypeDef* spi, GPIO_TypeDef* csn_gpio_port, uint16_t csn_pin, GPIO_TypeDef* ce_gpio_port, uint16_t ce_pin) {
	nrf_spi = spi;
	nrf_csn_gpio_port = csn_gpio_port;
	nrf_csn_pin = csn_pin;
	nrf_ce_gpio_port = ce_gpio_port;
	nrf_ce_pin = ce_pin;
	return 0;
}

void nrf_hal_csn(uint8_t mode) {
	HAL_GPIO_WritePin(nrf_csn_gpio_port, nrf_csn_pin, mode == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void nrf_hal_ce(uint8_t mode) {
	HAL_GPIO_WritePin(nrf_ce_gpio_port, nrf_ce_pin, mode == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}


int nrf_hal_read_reg(uint8_t reg, uint8_t *buf, uint8_t len) {
	uint8_t cmd  = CMD_R_REGISTER | reg;

	nrf_hal_csn(0);


	if(HAL_OK != HAL_SPI_Transmit(nrf_spi, &cmd, 1, HAL_MAX_DELAY)) {
		return -1;
	}

	if(HAL_OK != HAL_SPI_Receive(nrf_spi, buf, len, HAL_MAX_DELAY)){
		return -1;
	}

	nrf_hal_csn(1);

	return 0;
}

int nrf_hal_read_reg_byte(uint8_t reg, uint8_t *v) {
	return nrf_hal_read_reg(reg, v, 1);
}

int nrf_hal_write_reg(uint8_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t cmd = CMD_W_REGISTER | reg;

	nrf_hal_csn(0);

	if(HAL_OK != HAL_SPI_Transmit(nrf_spi, &cmd, 1,   HAL_MAX_DELAY)) {
		return -1;
	}

	if(HAL_OK != HAL_SPI_Transmit(nrf_spi,  buf, len, HAL_MAX_DELAY)) {
		return -1;
	}

	nrf_hal_csn(1);

	return 0;
}


int nrf_hal_write_reg_byte(uint8_t reg, uint8_t byte)
{
	return nrf_hal_write_reg(reg, &byte, 1);
}

int nrf_hal_set_reg_bit(uint8_t reg, uint8_t bit) {
	uint8_t v = 0;

	if(0 != nrf_hal_read_reg_byte(reg, &v)) {
		return -1;
	}

	v |= _BV(bit);

	if(0 != nrf_hal_write_reg_byte(reg, v)) {
		return -1;
	}

	return 0;
}


int nrf_hal_clear_reg_bit(uint8_t reg, uint8_t bit) {
	uint8_t v = 0;

	if(0 != nrf_hal_read_reg_byte(reg, &v)) {
		return -1;
	}

	v &= ~_BV(bit);

	if(0 != nrf_hal_write_reg_byte(reg, v)) {
		return -1;
	}

	return 0;
}

int nrf_hal_cmd_read(uint8_t cmd, uint8_t *data, uint8_t len) {
	nrf_hal_csn(0);

	if(HAL_OK != HAL_SPI_Transmit(nrf_spi, &cmd, 1, HAL_MAX_DELAY)) {
		return -1;
	}

	if(HAL_OK != HAL_SPI_Receive(nrf_spi, data, len, HAL_MAX_DELAY)) {
		return -1;
	}

	nrf_hal_csn(1);

	return 0;
}

int nrf_hal_cmd_read_byte(uint8_t cmd, uint8_t *data) {
	return nrf_hal_cmd_read(cmd, data, 1);
}

int nrf_hal_write_cmd(uint8_t cmd) {
	nrf_hal_csn(0);

	if(HAL_OK != HAL_SPI_Transmit(nrf_spi, &cmd, 1, HAL_MAX_DELAY)) {
		return -1;
	}

	nrf_hal_csn(1);

	return 0;
}

