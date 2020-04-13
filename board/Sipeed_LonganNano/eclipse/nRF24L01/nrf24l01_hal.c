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
    gpio_bit_write(nhi.csn_port, nhi.csn_pin, mode == 0 ? RESET : SET);
    //mode == 0 ? gpio_bit_reset(nhi.csn_port, nhi.csn_pin) : gpio_bit_set(nhi.csn_port, nhi.csn_pin);
}

void nrf_hal_ce(uint8_t mode) {
	gpio_bit_write(nhi.ce_port, nhi.ce_pin, mode == 0 ? RESET : SET);
    //mode == 0 ? gpio_bit_reset(nhi.ce_port, nhi.ce_pin) : gpio_bit_set(nhi.ce_port, nhi.ce_pin);
}


void _spi_send(uint32_t spi, uint8_t *buf, uint8_t len) {
    if(buf == 0) {
        return;
    }

    int cnt = 0;
    while(cnt < len) {
        if(SET == spi_i2s_flag_get(spi, SPI_FLAG_TBE)) {
            spi_i2s_data_transmit(spi, buf[cnt]);
            cnt++;
        }
    }
}

void _spi_recv(uint32_t spi, uint8_t *buf, uint8_t len) {
    if(buf == 0) {
        return ;
    }

    int cnt = 0;
    while(cnt < len) {
        FlagStatus ret = spi_i2s_flag_get(spi, SPI_FLAG_RBNE);
        if(SET == ret) {
            buf[cnt] = (uint8_t)spi_i2s_data_receive(spi);
            cnt++;
       }
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

