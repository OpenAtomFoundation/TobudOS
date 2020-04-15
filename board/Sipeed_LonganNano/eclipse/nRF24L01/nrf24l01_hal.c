#include "nrf24l01.h"


static nrf_hal_init_t nhi;


int nrf_hal_init(nrf_hal_init_t *private) {
    if(private == 0) {
        return -1;
    }

    nhi = *private;

    return 0;
}

void nrf_hal_csn(uint8_t mode) {
    //gpio_bit_write(nhi.csn_port, nhi.csn_pin, mode == 0 ? RESET : SET);
    mode == 0 ? gpio_bit_reset(nhi.csn_port, nhi.csn_pin) : gpio_bit_set(nhi.csn_port, nhi.csn_pin);
}

void nrf_hal_ce(uint8_t mode) {
	//gpio_bit_write(nhi.ce_port, nhi.ce_pin, mode == 0 ? RESET : SET);
    mode == 0 ? gpio_bit_reset(nhi.ce_port, nhi.ce_pin) : gpio_bit_set(nhi.ce_port, nhi.ce_pin);
}

uint8_t _spi_transfer(uint32_t spi, uint8_t data) {
    while(RESET == spi_i2s_flag_get(spi, SPI_FLAG_TBE));
    spi_i2s_data_transmit(spi, data);

    while(RESET == spi_i2s_flag_get(spi, SPI_FLAG_RBNE));
    data = spi_i2s_data_receive(spi);

    return data;
}


void _spi_send(uint32_t spi, uint8_t *buf, uint8_t len) {
    for(uint8_t i=0; i<len; i++) {
        _spi_transfer(spi, buf[i]);
    }
}
void _spi_recv(uint32_t spi, uint8_t *buf, uint8_t len) {
    for(uint8_t i=0; i<len; i++) {
        buf[i] = _spi_transfer(spi, 0xFF);
    }
}


int nrf_hal_read_reg(uint8_t reg, uint8_t *buf, uint8_t len) {
	uint8_t cmd  = CMD_R_REGISTER | reg;

	nrf_hal_csn(0);

    _spi_send(nhi.spi, &cmd, 1);

    _spi_recv(nhi.spi, buf, len);

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

	_spi_send(nhi.spi, &cmd, 1);

	_spi_send(nhi.spi, buf, len);

	nrf_hal_csn(1);

	return 0;
}


int nrf_hal_write_reg_byte(uint8_t reg, uint8_t byte)
{
	return nrf_hal_write_reg(reg, &byte, 1);
}


int nrf_hal_cmd_read(uint8_t cmd, uint8_t *data, uint8_t len) {
	nrf_hal_csn(0);

	_spi_send(nhi.spi, &cmd, 1);

	_spi_recv(nhi.spi, data, len);

	nrf_hal_csn(1);

	return 0;
}

int nrf_hal_cmd_write(uint8_t cmd, uint8_t *data, uint8_t len) {
	nrf_hal_csn(0);

	_spi_send(nhi.spi, &cmd, 1);

	_spi_send(nhi.spi, data, len);

	nrf_hal_csn(1);

	return 0;
}


int nrf_hal_cmd_read_byte(uint8_t cmd, uint8_t *data) {
	return nrf_hal_cmd_read(cmd, data, 1);
}

int nrf_hal_write_cmd(uint8_t cmd) {
	nrf_hal_csn(0);

	_spi_send(nhi.spi, &cmd, 1);

	nrf_hal_csn(1);

	return 0;
}

