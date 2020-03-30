/*
 * nrf24l01.c
 *
 *  Created on: Mar 27, 2020
 *      Author: ace
 */

#include "main.h"
#include "nrf24l01.h"
#include "tos_k.h"

extern SPI_HandleTypeDef hspi1;

static SPI_HandleTypeDef *spi;

void nrf_init() {
	spi = &hspi1;
}

void nrf_csn(uint8_t mode) {
	HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, mode == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void nrf_ce(uint8_t mode) {

	HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, mode == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

int _nrf_read_reg(uint8_t reg, uint8_t *buf, uint8_t len) {
	uint8_t cmd  = CMD_R_REGISTER | reg;

	nrf_csn(0);


	if(HAL_OK != HAL_SPI_Transmit(spi, &cmd, 1, HAL_MAX_DELAY)) {
		return -1;
	}

	if(HAL_OK != HAL_SPI_Receive(spi, buf, len, HAL_MAX_DELAY)){
		return -1;
	}

	nrf_csn(1);

	return 0;
}


uint8_t _nrf_read_reg_byte(uint8_t reg, uint8_t *v) {
	return _nrf_read_reg(reg, v, 1);
}

int _nrf_write_reg(uint8_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t cmd = CMD_W_REGISTER | reg;

	nrf_csn(0);

	if(HAL_OK != HAL_SPI_Transmit(spi, &cmd, 1,   HAL_MAX_DELAY)) {
		return -1;
	}

	if(HAL_OK != HAL_SPI_Transmit(spi,  buf, len, HAL_MAX_DELAY)) {
		return -1;
	}

	nrf_csn(1);

	return 0;
}


int _nrf_write_reg_byte(uint8_t reg, uint8_t byte)
{
	return _nrf_write_reg(reg, &byte, 1);
}

void _nrf_set_reg_bit(uint8_t reg, uint8_t bit) {
	uint8_t v = 0;

	_nrf_read_reg_byte(reg, &v);

	v |= _BV(bit);

	_nrf_write_reg_byte(reg, v);
}


void _nrf_clear_reg_bit(uint8_t reg, uint8_t bit) {
	uint8_t v = 0;

	_nrf_read_reg_byte(reg, &v);

	v &= ~_BV(bit);

	_nrf_write_reg_byte(reg, v);
}

int _nrf_cmd_read(uint8_t cmd, uint8_t *data, uint8_t len) {
	nrf_csn(0);

	HAL_SPI_Transmit(spi, &cmd, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(spi, data, len, HAL_MAX_DELAY);

	nrf_csn(1);
	return 0;
}

int _nrf_cmd_read_byte(uint8_t cmd, uint8_t *data) {
	return _nrf_cmd_read(cmd, data, 1);
}

void _nrf_write_cmd(uint8_t cmd) {
	nrf_csn(0);

	HAL_SPI_Transmit(spi, &cmd, 1, HAL_MAX_DELAY);

	nrf_csn(1);
}

void nrf_flush_rx() {
	_nrf_write_cmd(CMD_FLUSH_RX);
}

void nrf_flush_tx() {
	_nrf_write_cmd(CMD_FLUSH_TX);
}


void nrf_delay(uint32_t delay) {
	tos_task_delay(delay);
}


void nrf_powerup() {
	_nrf_set_reg_bit(REG_CONFIG, PWR_UP);
}

void nrf_powerdown() {
	_nrf_clear_reg_bit(REG_CONFIG, PWR_UP);
}

void nrf_enable_rx_irq() {
	_nrf_clear_reg_bit(REG_CONFIG, MASK_RX_DR);
}

void nrf_disable_rx_irq() {
	_nrf_set_reg_bit(REG_CONFIG, MASK_RX_DR);
}

void nrf_enable_tx_irq() {
	_nrf_clear_reg_bit(REG_CONFIG, MASK_TX_DS);
}

void nrf_disable_tx_irq() {
	_nrf_set_reg_bit(REG_CONFIG, MASK_TX_DS);
}

void nrf_enable_max_rt_irq() {
	_nrf_clear_reg_bit(REG_CONFIG, MASK_MAX_RT);
}

void nrf_disable_max_rt_irq() {
	_nrf_clear_reg_bit(REG_CONFIG, MASK_MAX_RT);
}

void nrf_set_rf_channel(uint8_t channel) {
	channel &= 0x7F;
	_nrf_write_reg_byte(REG_RF_CH, channel);
}

int nrf_set_rxaddr(uint8_t pipe, uint8_t *addr, uint8_t addrlen) {
	if(addrlen >= 6 || pipe >= 6) {
		return -1;
	}

	if(pipe >= 2) {
		addrlen = 1;
	}

	uint8_t reg = REG_RX_ADDR_P0 + pipe;


	return _nrf_write_reg(reg, addr, addrlen);
}

int nrf_set_txaddr(uint8_t *addr, uint8_t addrlen) {
	if(addrlen >= 6) {
		return -1;
	}
	return _nrf_write_reg(REG_TX_ADDR, addr, addrlen);
}

int nrf_enable_rxaddr(uint8_t pipe) {
	if(pipe >= 6) {
		return -1;
	}

	_nrf_write_reg_byte(REG_EN_RXADDR, pipe);
	return 0;
}

void nrf_reset_registers() {
	_nrf_write_reg_byte(REG_CONFIG,     _BV(EN_CRC));
	_nrf_write_reg_byte(REG_EN_AA,      _BV(ENAA_P0) | _BV(ENAA_P1) | _BV(ENAA_P2) | _BV(ENAA_P3) | _BV(ENAA_P4) | _BV(ENAA_P5));
	_nrf_write_reg_byte(REG_EN_RXADDR,  _BV(ERX_P0) | _BV(ERX_P1));
	_nrf_write_reg_byte(REG_SETUP_AW,   _VV(AW_5BYTES, AW));
	_nrf_write_reg_byte(REG_SETUP_RETR, _VV(ARD_250us, ARD) | _VV(ARC_3, ARC));
	_nrf_write_reg_byte(REG_RF_CH,		0b00000010);
	_nrf_write_reg_byte(REG_RF_SETUP,	_BV(RF_DR_HIGH) | _VV(RF_PWR_0dBm, RF_PWR));

	uint8_t status = 0;
	_nrf_read_reg_byte(REG_STATUS, &status);
	if(status & _BV(RX_DR)) {
		_nrf_set_reg_bit(REG_STATUS, _BV(RX_DR));
	}
	if(status & _BV(TX_DS)) {
		_nrf_set_reg_bit(REG_STATUS, _BV(TX_DS));
	}
	if(status & _BV(MAX_RT)) {
		_nrf_set_reg_bit(REG_STATUS, _BV(MAX_RT));
	}

	_nrf_write_reg_byte(REG_RX_PW_P0,	0);
	_nrf_write_reg_byte(REG_RX_PW_P1,	0);
	_nrf_write_reg_byte(REG_RX_PW_P2,	0);
	_nrf_write_reg_byte(REG_RX_PW_P3,	0);
	_nrf_write_reg_byte(REG_RX_PW_P4,	0);
	_nrf_write_reg_byte(REG_RX_PW_P5,	0);
	_nrf_write_reg_byte(REG_DYNPD,		0);
	_nrf_write_reg_byte(REG_FEATURE,	0);

	uint8_t addrp0[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	uint8_t addrp1[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
	_nrf_write_reg(REG_TX_ADDR,    addrp0, 5);
	_nrf_write_reg(REG_RX_ADDR_P0, addrp0, 5);
	_nrf_write_reg(REG_RX_ADDR_P1, addrp1, 5);
	_nrf_write_reg_byte(REG_RX_ADDR_P2, 0xC3);
	_nrf_write_reg_byte(REG_RX_ADDR_P3, 0xC4);
	_nrf_write_reg_byte(REG_RX_ADDR_P4, 0xC5);
	_nrf_write_reg_byte(REG_RX_ADDR_P5, 0xC6);

	nrf_flush_rx();
	nrf_flush_tx();
}

void nrf_set_standby_mode() {
	nrf_ce(0);
	nrf_powerdown();
	nrf_reset_registers();
	nrf_delay(10);

	printf("\n");
	for(int i=0; i<=7; i++) {
		uint8_t v = 0;
		_nrf_read_reg_byte(i, &v);
		printf("--reg %u val %x\n", i, v);
	}

	nrf_powerup();
	nrf_delay(10);	// 10m > 1.5~2ms
}



void nrf_set_receive_mode() {

	_nrf_set_reg_bit(REG_CONFIG, PRIM_RX);

	nrf_ce(1);

	nrf_delay(1); // 1ms > 120~130us
}

void nrf_enable_autoack(uint8_t pipe) {
	if(pipe >= 6) {
		return ;
	}

	_nrf_set_reg_bit(REG_EN_AA, pipe);
}

void nrf_disable_autoack(uint8_t pipe) {
	if(pipe >= 6) {
		return ;
	}

	_nrf_clear_reg_bit(REG_EN_AA, pipe);
}


void nrf_set_datarate(uint8_t dr) {

	if(NRF_1Mbps == dr) {
		dr = 0;
	} else if(NRF_2Mbps == dr) {
		_nrf_write_reg_byte(REG_RF_SETUP,   0b00001110);
		_nrf_write_reg_byte(REG_SETUP_RETR, 0b00010011);
	} else {

	}

}


int nrf_enable_dynamic_payload(uint8_t pipe) {
	if(pipe >= 6) {
		return -1;
	}

	uint8_t feature = 0;
	uint8_t dynpd = 0;

	_nrf_read_reg_byte(REG_FEATURE, &feature);
	_nrf_read_reg_byte(REG_DYNPD, &dynpd);

	feature |= _BV(EN_DPL);
	dynpd   |= _BV(pipe);

	_nrf_write_reg_byte(REG_DYNPD, dynpd);
	_nrf_write_reg_byte(REG_FEATURE, feature);

	return 0;
}


int nrf_read_payload(uint8_t *buf, uint8_t *len) {

	_nrf_cmd_read_byte(CMD_R_RX_PL_WID, len);

	_nrf_cmd_read(CMD_R_RX_PAYLOAD, buf, *len);

	return 0;
}

void print_addr(uint8_t pipe) {
	uint8_t addr[5];
	_nrf_read_reg(REG_RX_ADDR_P0+pipe, addr, 5);

	printf("pipe %u addr: ", pipe);
	for(int i=0; i<5; i++) {
		printf("%u ", addr[i]);
	}
	printf("\n");
}

uint8_t nrf_received_data = 0;

uint8_t nrf_hal_test() {

	uint8_t data = 0;
	nrf_init();
#if 1
	nrf_delay(200);

	nrf_csn(1);
	nrf_ce(0);

	nrf_delay(200);


	nrf_set_standby_mode();



	nrf_set_receive_mode();
	//nrf_disable_rx_irq();


	nrf_set_rf_channel(64);
	nrf_set_datarate(NRF_2Mbps);
	uint8_t rxaddr[] = { 1, 2, 3, 4, 1 };
	uint8_t rxaddr1[] = { 1, 2, 3, 4, 2 };
	nrf_set_rxaddr(0, rxaddr, 5);
	nrf_set_rxaddr(1, rxaddr1, 5);
	nrf_set_txaddr(rxaddr, 5);

	nrf_enable_autoack(0);
	//nrf_disable_autoack(0);
	nrf_enable_autoack(1);

	nrf_enable_dynamic_payload(0);
	nrf_enable_dynamic_payload(1);

	nrf_enable_rxaddr(0);
	nrf_enable_rxaddr(1);

	printf("\n");
	for(int i=0; i<=7; i++) {
		uint8_t v = 0;
		_nrf_read_reg_byte(i, &v);
		printf("reg %u val %x\n", i, v);
	}

	print_addr(0);
	print_addr(1);
	print_addr(2);
#endif

	while(1) {

#if 1
		uint8_t buf[32];
		uint8_t len = 0;

		uint8_t status = 0;
		_nrf_read_reg_byte(REG_STATUS, &status);
		nrf_read_payload(buf, &len);

		if(status &  _BV(RX_DR)) {
			if(len > 0) {
				uint8_t pipe = status;
				pipe >>= 1;
				pipe &= 0x07;
				printf("received %u bytes from pipe %u: ", len, pipe);
				if(pipe >= 6) {
					nrf_flush_rx();
					_nrf_read_reg_byte(REG_STATUS, _BV(RX_DR));
					printf("\n");
					continue;
				}
				for(int i=0; i<len; i++) {
					printf("%x ", buf[i]);
				}
				nrf_received_data = 1;
				printf("\n");
			}

			nrf_flush_rx();
			_nrf_set_reg_bit(REG_STATUS, _BV(RX_DR));
		} else {
			printf("nodata %x\n", status);
			nrf_flush_rx();
			_nrf_set_reg_bit(REG_STATUS, _BV(RX_DR));
		}
#endif
		nrf_delay(100);
	}


	return data;
}
