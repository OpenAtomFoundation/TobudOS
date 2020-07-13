#include "nrf24l01.h"
#include "tos_k.h"

static nrf_init_t g_nrf;

int nrf_init(nrf_init_t *ni) {

	memcpy(&g_nrf, ni, sizeof(nrf_init_t));

	return g_nrf.init(g_nrf.private);
}

void nrf_flush_rx() {
	nrf_write_cmd(CMD_FLUSH_RX);
}

void nrf_flush_tx() {
	nrf_write_cmd(CMD_FLUSH_TX);
}


void nrf_delay(uint32_t delay) {
	tos_task_delay(delay);
}


int nrf_powerup() {
	return nrf_set_reg_bit(REG_CONFIG, PWR_UP);
}

int nrf_powerdown() {
	return nrf_clear_reg_bit(REG_CONFIG, PWR_UP);
}

void nrf_enable_rx_irq() {
	nrf_clear_reg_bit(REG_CONFIG, MASK_RX_DR);
}

void nrf_disable_rx_irq() {
	nrf_set_reg_bit(REG_CONFIG, MASK_RX_DR);
}

void nrf_enable_tx_irq() {
	nrf_clear_reg_bit(REG_CONFIG, MASK_TX_DS);
}

void nrf_disable_tx_irq() {
	nrf_set_reg_bit(REG_CONFIG, MASK_TX_DS);
}

void nrf_enable_max_rt_irq() {
	nrf_clear_reg_bit(REG_CONFIG, MASK_MAX_RT);
}

void nrf_disable_max_rt_irq() {
	nrf_set_reg_bit(REG_CONFIG, MASK_MAX_RT);
}

void nrf_set_rf_channel(uint8_t channel) {
	channel &= 0x7F;
	nrf_write_reg_byte(REG_RF_CH, channel);
}

int nrf_set_rxaddr(uint8_t pipe, uint8_t *addr, uint8_t addrlen) {
	if(addrlen >= 6 || pipe >= 6) {
		return -1;
	}

	if(pipe >= 2) {
		addrlen = 1;
	}

	uint8_t reg = REG_RX_ADDR_P0 + pipe;


	return nrf_write_reg(reg, addr, addrlen);
}

int nrf_get_addrlen() {
    uint8_t v = 0;
    uint8_t addrlen = 0;
    if(0 != nrf_read_reg_byte(REG_SETUP_AW, &v)) {
        return 0;
    }

    v &= 0x03;

    switch(v) {
    case AW_3BYTES:
        addrlen = 3;
        break;
    case AW_4BYTES:
        addrlen = 4;
        break;
    case AW_5BYTES:
        addrlen = 5;
        break;
    default:
        break;
    }

    return addrlen;
}

int nrf_get_rxaddr(uint8_t pipe, uint8_t *addr, uint8_t *addrlen) {

    *addrlen = nrf_get_addrlen();

    uint8_t reg = REG_RX_ADDR_P0 + pipe;

    return nrf_read_reg(reg, addr, *addrlen);
}

int nrf_get_txaddr(uint8_t *addr, uint8_t *addrlen) {
    *addrlen = nrf_get_addrlen();
    return nrf_read_reg(REG_TX_ADDR, addr, *addrlen);
}

int nrf_set_txaddr(uint8_t *addr, uint8_t addrlen) {
	if(addrlen >= 6) {
		return -1;
	}
	return nrf_write_reg(REG_TX_ADDR, addr, addrlen);
}

int nrf_enable_rxaddr(uint8_t pipe) {
	if(pipe >= 6) {
		return -1;
	}

	nrf_set_reg_bit(REG_EN_RXADDR, pipe);

	return 0;
}

void nrf_reset_registers() {
	nrf_write_reg_byte(REG_CONFIG,     _BV(EN_CRC));
	nrf_write_reg_byte(REG_EN_AA,      _BV(ENAA_P0) | _BV(ENAA_P1) | _BV(ENAA_P2) | _BV(ENAA_P3) | _BV(ENAA_P4) | _BV(ENAA_P5));
	nrf_write_reg_byte(REG_EN_RXADDR,  _BV(ERX_P0) | _BV(ERX_P1));
	nrf_write_reg_byte(REG_SETUP_AW,   _VV(AW_5BYTES, AW));
	nrf_write_reg_byte(REG_SETUP_RETR, _VV(ARD_250us, ARD) | _VV(ARC_3, ARC));
	nrf_write_reg_byte(REG_RF_CH,		0b00000010);
	nrf_write_reg_byte(REG_RF_SETUP,	_BV(RF_DR) | _VV(RF_PWR_0dBm, RF_PWR));

	uint8_t status = 0;
	nrf_read_reg_byte(REG_STATUS, &status);
	if(status & _BV(RX_DR)) {
		nrf_set_reg_bit(REG_STATUS, _BV(RX_DR));
	}
	if(status & _BV(TX_DS)) {
		nrf_set_reg_bit(REG_STATUS, _BV(TX_DS));
	}
	if(status & _BV(MAX_RT)) {
		nrf_set_reg_bit(REG_STATUS, _BV(MAX_RT));
	}

	nrf_write_reg_byte(REG_RX_PW_P0,	0);
	nrf_write_reg_byte(REG_RX_PW_P1,	0);
	nrf_write_reg_byte(REG_RX_PW_P2,	0);
	nrf_write_reg_byte(REG_RX_PW_P3,	0);
	nrf_write_reg_byte(REG_RX_PW_P4,	0);
	nrf_write_reg_byte(REG_RX_PW_P5,	0);
	nrf_write_reg_byte(REG_DYNPD,		0);
	nrf_write_reg_byte(REG_FEATURE,	0);

	uint8_t addrp0[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	uint8_t addrp1[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
	nrf_write_reg(REG_TX_ADDR,    addrp0, 5);
	nrf_write_reg(REG_RX_ADDR_P0, addrp0, 5);
	nrf_write_reg(REG_RX_ADDR_P1, addrp1, 5);
	nrf_write_reg_byte(REG_RX_ADDR_P2, 0xC3);
	nrf_write_reg_byte(REG_RX_ADDR_P3, 0xC4);
	nrf_write_reg_byte(REG_RX_ADDR_P4, 0xC5);
	nrf_write_reg_byte(REG_RX_ADDR_P5, 0xC6);

	nrf_flush_rx();
	nrf_flush_tx();
}

void nrf_set_standby_mode() {
	nrf_ce(0);
	nrf_powerdown();
	nrf_reset_registers();
	nrf_delay(10);

	nrf_powerup();
	nrf_delay(10);	// 10m > 1.5~2ms
}



void nrf_set_receive_mode() {
	nrf_set_reg_bit(REG_CONFIG, PRIM_RX);

	nrf_ce(1);

	nrf_delay(1); // 1ms > 120~130us
}

void nrf_set_send_mode() {
	nrf_clear_reg_bit(REG_CONFIG, PRIM_RX);

	nrf_ce(1);

	nrf_delay(1); // 1ms > 120~130us
}

void nrf_enable_autoack(uint8_t pipe) {
	if(pipe >= 6) {
		return ;
	}

	nrf_set_reg_bit(REG_EN_AA, pipe);
}

void nrf_disable_autoack(uint8_t pipe) {
	if(pipe >= 6) {
		return ;
	}

	nrf_clear_reg_bit(REG_EN_AA, pipe);
}


void nrf_set_datarate(uint8_t dr) {

	if(NRF_1Mbps == dr) {
		dr = 0;
	} else if(NRF_2Mbps == dr) {
		nrf_write_reg_byte(REG_RF_SETUP,   0b00001110);
		nrf_write_reg_byte(REG_SETUP_RETR, 0b00010011);
	} else {

	}

}


int nrf_enable_dynamic_payload(uint8_t pipe) {
	if(pipe >= 6) {
		return -1;
	}

	uint8_t feature = 0;
	uint8_t dynpd = 0;

	nrf_read_reg_byte(REG_FEATURE, &feature);
	nrf_read_reg_byte(REG_DYNPD, &dynpd);

	feature |= _BV(EN_DPL);
	dynpd   |= _BV(pipe);

	nrf_write_reg_byte(REG_DYNPD, dynpd);
	nrf_write_reg_byte(REG_FEATURE, feature);

	return 0;
}

int nrf_read_payload(uint8_t *buf, uint8_t *len, uint8_t *pipe) {
    // 读数据通道
    uint8_t status = 0;
    nrf_read_reg_byte(REG_STATUS, &status);
    *pipe = ((status>>1) & 0x07);

    // 读数据长度
	nrf_cmd_read_byte(CMD_R_RX_PL_WID, len);

	// 读数据
	nrf_cmd_read(CMD_R_RX_PAYLOAD, buf, *len);

	// 清除数据标志位
	nrf_set_reg_bit(REG_STATUS, _BV(RX_DR));

	// 清空接收缓冲区
	nrf_flush_rx();

	if(*pipe >= 6) {
	    *len = 0;
	}

	return 0;
}


int nrf_poll_read_payload(uint8_t *buf, uint8_t *len, uint8_t *pipe) {
    while(1) {
        // 读数据通道
        uint8_t status = 0;
        nrf_read_reg_byte(REG_STATUS, &status);
        if((status & _BV(RX_DR)) == 0) {
            nrf_delay(1);
            continue;
        }

        *pipe = ((status>>1) & 0x07);

        break;
    }

    // 读数据长度
    nrf_cmd_read_byte(CMD_R_RX_PL_WID, len);

    // 读数据
    nrf_cmd_read(CMD_R_RX_PAYLOAD, buf, *len);

    // 清除数据标志位
    nrf_set_reg_bit(REG_STATUS, _BV(RX_DR));

    // 清空接收缓冲区
    nrf_flush_rx();

    if(*pipe >= 6) {
        *len = 0;
    }

    return 0;
}

int nrf_write_payload(uint8_t *buf, uint8_t len) {
	nrf_write_cmd_and_data(CMD_W_TX_PAYLOAD_NOACK, buf, len);

    while(1) {
        uint8_t status = 0;
        nrf_read_reg_byte(REG_STATUS, &status);
        if(status &  _BV(TX_DS)) {
            nrf_delay(1);
        }
        nrf_set_reg_bit(REG_STATUS, _BV(MAX_RT));
        nrf_set_reg_bit(REG_STATUS, _BV(TX_DS));
        break;
    }

    return 0;
}


void nrf_ce(uint8_t mode) {
    g_nrf.ce(mode);
}

void nrf_csn(uint8_t mode) {
    g_nrf.csn(mode);
}






int nrf_read_reg(uint8_t reg, uint8_t *buf, uint8_t len) {
	uint8_t cmd  = CMD_R_REGISTER | reg;

	nrf_csn(0);

    g_nrf.spi_send(&cmd, 1);

    g_nrf.spi_recv(buf, len);

	nrf_csn(1);

	return 0;
}


int nrf_write_reg(uint8_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t cmd = CMD_W_REGISTER | reg;

	nrf_csn(0);

	g_nrf.spi_send(&cmd, 1);

	g_nrf.spi_send(buf, len);

	nrf_csn(1);

	return 0;
}


int nrf_cmd_read(uint8_t cmd, uint8_t *data, uint8_t len) {
	nrf_csn(0);

	g_nrf.spi_send(&cmd, 1);

	g_nrf.spi_recv(data, len);

	nrf_csn(1);

	return 0;
}

int nrf_write_cmd_and_data(uint8_t cmd, uint8_t *data, uint8_t len) {
	nrf_csn(0);

	g_nrf.spi_send(&cmd, 1);

	g_nrf.spi_send(data, len);

	nrf_csn(1);

	return 0;
}


int nrf_cmd_read_byte(uint8_t cmd, uint8_t *data) {
	return nrf_cmd_read(cmd, data, 1);
}

int nrf_write_cmd(uint8_t cmd) {
	nrf_csn(0);

	g_nrf.spi_send(&cmd, 1);

	nrf_csn(1);

	return 0;
}


int nrf_read_reg_byte(uint8_t reg, uint8_t *v) {
	return nrf_read_reg(reg, v, 1);
}

int nrf_write_reg_byte(uint8_t reg, uint8_t byte) {
	return nrf_write_reg(reg, &byte, 1);
}

int nrf_clear_reg_bit(uint8_t reg, uint8_t bit) {
    uint8_t v = 0;

    if(0 != nrf_read_reg_byte(reg, &v)) {
        return -1;
    }

    v &= ~_BV(bit);

    if(0 != nrf_write_reg_byte(reg, v)) {
        return -1;
    }

    return 0;
}

int nrf_set_reg_bit(uint8_t reg, uint8_t bit) {
    uint8_t v = 0;

    if(0 != nrf_read_reg_byte(reg, &v)) {
        return -1;
    }

    v |= _BV(bit);

    if(0 != nrf_write_reg_byte(reg, v)) {
        return -1;
    }

    return 0;
}





