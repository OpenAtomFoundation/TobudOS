#include "nrf24l01.h"
#include "tos_k.h"


int nrf_init(void *ni) {
    return nrf_hal_init(ni);
}

void nrf_flush_rx() {
	nrf_hal_write_cmd(CMD_FLUSH_RX);
}

void nrf_flush_tx() {
	nrf_hal_write_cmd(CMD_FLUSH_TX);
}


void nrf_delay(uint32_t delay) {
	tos_task_delay(delay);
}


int nrf_powerup() {
	return nrf_hal_set_reg_bit(REG_CONFIG, PWR_UP);
}

int nrf_powerdown() {
	return nrf_hal_clear_reg_bit(REG_CONFIG, PWR_UP);
}

void nrf_enable_rx_irq() {
	nrf_hal_clear_reg_bit(REG_CONFIG, MASK_RX_DR);
}

void nrf_disable_rx_irq() {
	nrf_hal_set_reg_bit(REG_CONFIG, MASK_RX_DR);
}

void nrf_enable_tx_irq() {
	nrf_hal_clear_reg_bit(REG_CONFIG, MASK_TX_DS);
}

void nrf_disable_tx_irq() {
	nrf_hal_set_reg_bit(REG_CONFIG, MASK_TX_DS);
}

void nrf_enable_max_rt_irq() {
	nrf_hal_clear_reg_bit(REG_CONFIG, MASK_MAX_RT);
}

void nrf_disable_max_rt_irq() {
	nrf_hal_clear_reg_bit(REG_CONFIG, MASK_MAX_RT);
}

void nrf_set_rf_channel(uint8_t channel) {
	channel &= 0x7F;
	nrf_hal_write_reg_byte(REG_RF_CH, channel);
}

int nrf_set_rxaddr(uint8_t pipe, uint8_t *addr, uint8_t addrlen) {
	if(addrlen >= 6 || pipe >= 6) {
		return -1;
	}

	if(pipe >= 2) {
		addrlen = 1;
	}

	uint8_t reg = REG_RX_ADDR_P0 + pipe;


	return nrf_hal_write_reg(reg, addr, addrlen);
}

int nrf_set_txaddr(uint8_t *addr, uint8_t addrlen) {
	if(addrlen >= 6) {
		return -1;
	}
	return nrf_hal_write_reg(REG_TX_ADDR, addr, addrlen);
}

int nrf_enable_rxaddr(uint8_t pipe) {
	if(pipe >= 6) {
		return -1;
	}

	nrf_hal_write_reg_byte(REG_EN_RXADDR, pipe);
	return 0;
}

void nrf_reset_registers() {
	nrf_hal_write_reg_byte(REG_CONFIG,     _BV(EN_CRC));
	nrf_hal_write_reg_byte(REG_EN_AA,      _BV(ENAA_P0) | _BV(ENAA_P1) | _BV(ENAA_P2) | _BV(ENAA_P3) | _BV(ENAA_P4) | _BV(ENAA_P5));
	nrf_hal_write_reg_byte(REG_EN_RXADDR,  _BV(ERX_P0) | _BV(ERX_P1));
	nrf_hal_write_reg_byte(REG_SETUP_AW,   _VV(AW_5BYTES, AW));
	nrf_hal_write_reg_byte(REG_SETUP_RETR, _VV(ARD_250us, ARD) | _VV(ARC_3, ARC));
	nrf_hal_write_reg_byte(REG_RF_CH,		0b00000010);
	nrf_hal_write_reg_byte(REG_RF_SETUP,	_BV(RF_DR_HIGH) | _VV(RF_PWR_0dBm, RF_PWR));

	uint8_t status = 0;
	nrf_hal_read_reg_byte(REG_STATUS, &status);
	if(status & _BV(RX_DR)) {
		nrf_hal_set_reg_bit(REG_STATUS, _BV(RX_DR));
	}
	if(status & _BV(TX_DS)) {
		nrf_hal_set_reg_bit(REG_STATUS, _BV(TX_DS));
	}
	if(status & _BV(MAX_RT)) {
		nrf_hal_set_reg_bit(REG_STATUS, _BV(MAX_RT));
	}

	nrf_hal_write_reg_byte(REG_RX_PW_P0,	0);
	nrf_hal_write_reg_byte(REG_RX_PW_P1,	0);
	nrf_hal_write_reg_byte(REG_RX_PW_P2,	0);
	nrf_hal_write_reg_byte(REG_RX_PW_P3,	0);
	nrf_hal_write_reg_byte(REG_RX_PW_P4,	0);
	nrf_hal_write_reg_byte(REG_RX_PW_P5,	0);
	nrf_hal_write_reg_byte(REG_DYNPD,		0);
	nrf_hal_write_reg_byte(REG_FEATURE,	0);

	uint8_t addrp0[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	uint8_t addrp1[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
	nrf_hal_write_reg(REG_TX_ADDR,    addrp0, 5);
	nrf_hal_write_reg(REG_RX_ADDR_P0, addrp0, 5);
	nrf_hal_write_reg(REG_RX_ADDR_P1, addrp1, 5);
	nrf_hal_write_reg_byte(REG_RX_ADDR_P2, 0xC3);
	nrf_hal_write_reg_byte(REG_RX_ADDR_P3, 0xC4);
	nrf_hal_write_reg_byte(REG_RX_ADDR_P4, 0xC5);
	nrf_hal_write_reg_byte(REG_RX_ADDR_P5, 0xC6);

	nrf_flush_rx();
	nrf_flush_tx();
}

void nrf_set_standby_mode() {
	nrf_hal_ce(0);
	nrf_powerdown();
	nrf_reset_registers();
	nrf_delay(10);

	nrf_powerup();
	nrf_delay(10);	// 10m > 1.5~2ms
}



void nrf_set_receive_mode() {
	nrf_hal_set_reg_bit(REG_CONFIG, PRIM_RX);

	nrf_hal_ce(1);

	nrf_delay(1); // 1ms > 120~130us
}

void nrf_set_send_mode() {
	nrf_hal_clear_reg_bit(REG_CONFIG, PRIM_RX);

	nrf_hal_ce(1);

	nrf_delay(1); // 1ms > 120~130us
}

void nrf_enable_autoack(uint8_t pipe) {
	if(pipe >= 6) {
		return ;
	}

	nrf_hal_set_reg_bit(REG_EN_AA, pipe);
}

void nrf_disable_autoack(uint8_t pipe) {
	if(pipe >= 6) {
		return ;
	}

	nrf_hal_clear_reg_bit(REG_EN_AA, pipe);
}


void nrf_set_datarate(uint8_t dr) {

	if(NRF_1Mbps == dr) {
		dr = 0;
	} else if(NRF_2Mbps == dr) {
		nrf_hal_write_reg_byte(REG_RF_SETUP,   0b00001110);
		nrf_hal_write_reg_byte(REG_SETUP_RETR, 0b00010011);
	} else {

	}

}


int nrf_enable_dynamic_payload(uint8_t pipe) {
	if(pipe >= 6) {
		return -1;
	}

	uint8_t feature = 0;
	uint8_t dynpd = 0;

	nrf_hal_read_reg_byte(REG_FEATURE, &feature);
	nrf_hal_read_reg_byte(REG_DYNPD, &dynpd);

	feature |= _BV(EN_DPL);
	dynpd   |= _BV(pipe);

	nrf_hal_write_reg_byte(REG_DYNPD, dynpd);
	nrf_hal_write_reg_byte(REG_FEATURE, feature);

	return 0;
}


int nrf_read_payload(uint8_t *buf, uint8_t *len) {

	nrf_hal_cmd_read_byte(CMD_R_RX_PL_WID, len);

	nrf_hal_cmd_read(CMD_R_RX_PAYLOAD, buf, *len);

	return 0;
}

int nrf_write_payload(uint8_t *buf, uint8_t len) {
	return nrf_hal_cmd_write(CMD_W_TX_PAYLOAD_NOACK, buf, len);
}

void print_addr(uint8_t pipe) {
	uint8_t addr[5];
	nrf_hal_read_reg(REG_RX_ADDR_P0+pipe, addr, 5);

	printf("pipe %u addr: ", pipe);
	for(int i=0; i<5; i++) {
		printf("%u ", addr[i]);
	}
	printf("\n");
}

uint8_t nrf_received_data = 0;
uint8_t nrf_hal_test_rx() {

	uint8_t data = 0;

	nrf_delay(200);

	nrf_hal_csn(1);
	nrf_hal_ce(0);

	nrf_delay(200);


	nrf_set_standby_mode();

	nrf_set_receive_mode();
	nrf_disable_rx_irq();


	nrf_set_rf_channel(64);
	nrf_set_datarate(NRF_2Mbps);
	uint8_t rxaddr[] = { 1, 2, 3, 4, 1 };
	uint8_t txaddr[] = { 1, 2, 3, 4, 2 };
	nrf_set_rxaddr(0, rxaddr, 5);
	nrf_set_txaddr(txaddr, 5);

	nrf_enable_dynamic_payload(0);
	nrf_enable_dynamic_payload(1);

	nrf_enable_rxaddr(0);
	nrf_enable_rxaddr(1);

	print_addr(0);
	print_addr(1);
	print_addr(2);


	nrf_flush_rx();
	while(1) {
		uint8_t buf[32];
		uint8_t len = 0;
		uint8_t status = 0;
		nrf_hal_read_reg_byte(REG_STATUS, &status);
		nrf_read_payload(buf, &len);

		if(status &  _BV(RX_DR)) {
			nrf_hal_set_reg_bit(REG_STATUS, _BV(RX_DR));
			nrf_flush_rx();

			if(len > 0) {
				uint8_t pipe = status;
				pipe >>= 1;
				pipe &= 0x07;
				printf("received %u bytes from pipe %u: ", len, pipe);
				if(pipe >= 6) {
					printf("\n");
					continue;
				}
				for(int i=0; i<len; i++) {
					printf("%x ", buf[i]);
				}
				nrf_received_data = 1;
				printf("\n");
			}

		} else {
			printf("nodata %x\n", status);
			nrf_delay(100);
		}
	}

	return data;
}

uint8_t nrf_hal_test_tx() {

	uint8_t data = 0;

	nrf_delay(200);
	nrf_hal_csn(1);
	nrf_hal_ce(0);
	nrf_delay(200);

	nrf_set_standby_mode();
	nrf_set_send_mode();

	nrf_set_rf_channel(100);
	nrf_set_datarate(NRF_2Mbps);

	nrf_enable_dynamic_payload(0);
	uint8_t txaddr[] = { 1, 2, 3, 4, 0 };
	nrf_set_txaddr(txaddr, 5);

	nrf_flush_rx();
	nrf_flush_tx();
	uint32_t cnt = 0;
	while(1) {
		nrf_flush_rx();
		nrf_flush_tx();
		uint8_t buf[] = {0x0A, 0x0C, 0x0E, cnt++ };
		nrf_write_payload(buf, sizeof(buf));

		while(1) {
			uint8_t status = 0;
			nrf_hal_read_reg_byte(REG_STATUS, &status);

			printf("status %x\n", status);
			if(status & _BV(MAX_RT)) {
				printf("send error....\n");
				nrf_hal_set_reg_bit(REG_STATUS, _BV(MAX_RT));
				nrf_flush_tx();
				break;
			}else if(status &  _BV(TX_DS)) {
				printf("sended....\n");
				nrf_hal_set_reg_bit(REG_STATUS, _BV(TX_DS));
				break;
			} else {
				printf("sending....\n");
			}
		}

		nrf_delay(100);
	}

	return data;
}

uint8_t nrf_hal_test() {
	//return nrf_hal_test_rx();
	return nrf_hal_test_tx();
}
