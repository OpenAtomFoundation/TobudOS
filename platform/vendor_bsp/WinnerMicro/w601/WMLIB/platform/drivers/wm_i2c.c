/**************************************************************************//**
 * @file     wm_i2c.c
 * @author
 * @version  
 * @date  
 * @brief
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd. All rights reserved.
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_regs.h"
#include "wm_i2c.h"
#include "wm_cpu.h"
#include "wm_irq.h"

#define I2C_FREQ_MAX			(400000)
#define I2C_FREQ_MIN			(100000)
#define I2C_WRITE				(0x80)
#define I2C_READ				(0x00)
typedef struct {
	uint8_t addr;
	uint8_t dev_addr;
	uint8_t state;
	uint8_t *buf;
	uint16_t len;
	uint16_t cnt;
	uint8_t cmd;
	void (*transfer_done)(void);
} i2c_desc;
enum {
	START,
	RESTART,
	TRANSMIT,
	PRERECEIVE,
	RECEIVE,	
	STOP,
	DONE,
	IDLE,
};
static i2c_desc i2c_transfer;

void tls_i2c_init(u32 freq)
{	
	u32 div = 0;
	tls_sys_clk clk;
	
	if (freq < I2C_FREQ_MIN)
	{
		freq = I2C_FREQ_MIN;
	}		
	else if (freq > I2C_FREQ_MAX)
	{
		freq = I2C_FREQ_MAX;
	}
	tls_sys_clk_get(&clk);	
	
	div = (clk.apbclk * 1000000)/(5 * freq) - 1;
	tls_reg_write32(HR_I2C_PRER_LO, div & 0xff);
	tls_reg_write32(HR_I2C_PRER_HI, (div>>8) & 0xff);

	/** enable I2C | Disable Int*/
	tls_reg_write32(HR_I2C_CTRL, I2C_CTRL_INT_DISABLE | I2C_CTRL_ENABLE);
	tls_irq_enable(I2C_IRQn);
}

/**
 * @brief	send stop signal
 * 
 */
void tls_i2c_stop(void)
{
	tls_reg_write32(HR_I2C_CR_SR, I2C_CR_STO);
	while(tls_reg_read32(HR_I2C_CR_SR) & I2C_SR_TIP);
}

/**
 * @brief	waiting for ack signal
 * @retval 
 *	- \ref WM_FAILED
 *	- \ref WM_SUCCESS
 */
int tls_i2c_wait_ack(void)
{
	u16 errtime=0;
	u32 value;

	while(tls_reg_read32(HR_I2C_CR_SR) & I2C_SR_TIP);
	value = tls_reg_read32(HR_I2C_CR_SR);
	while(value & I2C_SR_NAK)
	{
		errtime ++;
		if(errtime > 512)
		{
			printf("wait ack err\n");
			tls_i2c_stop();
			return WM_FAILED;
		}
		value = tls_reg_read32(HR_I2C_CR_SR);
	}

	return WM_SUCCESS;
}


/**
 * @brief	writes the data to data register of I2C module 
 * when \ifstart one the start signal will be sent followed by the \data
 * when \ifstart zero only the \data will be send
 * @param[in] data	the data will be write to the data register of I2C module
 * @param[in] ifstart	when one send start signal, when zero don't 
 * @retval 
 *
 */
void tls_i2c_write_byte(u8 data,u8 ifstart)
{
	tls_reg_write32(HR_I2C_TX_RX, data);
	if(ifstart)
		tls_reg_write32(HR_I2C_CR_SR, I2C_CR_STA | I2C_CR_WR);
	else
		tls_reg_write32(HR_I2C_CR_SR, I2C_CR_WR);
	while(tls_reg_read32(HR_I2C_CR_SR) & I2C_SR_TIP);
}


/**
 * @brief	get the data stored in data register of I2C module
 * @param[in] ifack	when one send ack after reading the data register,when zero don't
 * @param[in] ifstop when one send stop signal after read, when zero do not send stop
 * @retval	the received data 
 */
u8 tls_i2c_read_byte(u8 ifack,u8 ifstop)
{
	u8 data;
	u32 value = I2C_CR_RD;

	if(!ifack)
		value |= I2C_CR_NAK;
	if(ifstop)
		value |= I2C_CR_STO;
	
	tls_reg_write32(HR_I2C_CR_SR, value);
	/** Waiting finish */
	while(tls_reg_read32(HR_I2C_CR_SR) & I2C_SR_TIP);
	data = tls_reg_read32(HR_I2C_TX_RX);

	return data;
}

/**
 * @brief	start write through int mode
 * @param[in] devaddr	the device address 
 * @param[in] wordaddr when one send stop signal after read, when zero do not send stop
 * @param[in] buf	the address point where data shoule be stored
 * @param[in] len	the length of data will be received 
 * @retval	
 *	- \ref WM_FAILED
 *	- \ref WM_SUCCESS
 */
int wm_i2c_start_write_it(uint8_t devaddr, uint8_t wordaddr, uint8_t * buf, uint16_t len)
{
	if (buf == NULL)
	{
		return WM_FAILED;
	}
	I2C->TX_RX = devaddr;
	i2c_transfer.dev_addr = devaddr;
	i2c_transfer.state = START;
	i2c_transfer.cmd = I2C_WRITE;
	i2c_transfer.buf = buf;
	i2c_transfer.len = len;
	i2c_transfer.cnt = 0;
	i2c_transfer.addr = wordaddr;
	I2C->CR_SR = I2C_CR_STA | I2C_CR_WR;
	return WM_SUCCESS;
}

/**
 * @brief	start read through int mode
 * @param[in] devaddr	the device address 
 * @param[in] wordaddr when one send stop signal after read, when zero do not send stop
 * @param[in] buf	the address point where data shoule be stored
 * @param[in] len	the length of data will be received 
 * @retval	
 *	- \ref WM_FAILED
 *	- \ref WM_SUCCESS
 */
int wm_i2c_start_read_it(uint8_t devaddr, uint8_t wordaddr, uint8_t * buf, uint16_t len)
{
	if (buf == NULL)
	{
		return WM_FAILED;
	}
	I2C->TX_RX = devaddr;
	i2c_transfer.dev_addr = devaddr;
	i2c_transfer.state = START;
	i2c_transfer.cmd = I2C_READ;
	i2c_transfer.buf = buf;
	i2c_transfer.len = len;
	i2c_transfer.cnt = 0;
	i2c_transfer.addr = wordaddr;
	I2C->CR_SR = I2C_CR_STA | I2C_CR_WR;
	
	return WM_SUCCESS;
}

/**
 * @brief          This function is used to register i2c transfer done callback function.
 * @param[in]      done  is the i2c transfer done callback function.
 * @retval         None
 * @note           None
 */
void wm_i2c_transfer_done_register(void (*done)(void))
{
	i2c_transfer.transfer_done = done;
}


void I2C_IRQHandler(void)
{
	int i2c_sr;
	i2c_sr = I2C->CR_SR;	
	I2C->CR_SR = 1;
	if (i2c_sr & 0x20)
	{
		printf("I2C AL lost\r\n");
	}
	if (i2c_sr & 0x01)
	{
		if ((i2c_sr & 0x80) == 0)
		{
			switch(i2c_transfer.state)
			{
				case START:
					I2C->TX_RX = i2c_transfer.addr;
					I2C->CR_SR = I2C_CR_WR;
					if ((i2c_transfer.cmd & I2C_WRITE) == I2C_WRITE)
					{
						i2c_transfer.state = TRANSMIT;
					}
					else
					{
						i2c_transfer.state = RESTART;
					}
					break;
					
				case RESTART:
					I2C->TX_RX = (i2c_transfer.dev_addr | 0x01);
					I2C->CR_SR = (I2C_CR_STA | I2C_CR_WR);
					i2c_transfer.state = PRERECEIVE;
					break;
				
				case TRANSMIT:
					I2C->TX_RX = i2c_transfer.buf[i2c_transfer.cnt++];
					I2C->CR_SR = I2C_CR_WR;
					if (i2c_transfer.cnt == i2c_transfer.len)
					{
						i2c_transfer.state = STOP;
					}
					break;
				
				case PRERECEIVE:
					i2c_transfer.state = RECEIVE;
					I2C->CR_SR = I2C_CR_RD;					
					break;	
				case RECEIVE:
					i2c_transfer.buf[i2c_transfer.cnt++] = I2C->TX_RX;					
					if (i2c_transfer.cnt == (i2c_transfer.len - 1))
					{
						I2C->CR_SR = (I2C_CR_STO | I2C_CR_NAK | I2C_CR_RD);
						i2c_transfer.state = STOP;
					}
					else if (i2c_transfer.len == 1)
					{
						I2C->CR_SR = (I2C_CR_STO | I2C_CR_NAK | I2C_CR_RD);
						i2c_transfer.state = DONE;
						if (i2c_transfer.transfer_done)
						{
							i2c_transfer.transfer_done();
						}
					}
					else 
					{
						I2C->CR_SR = I2C_CR_RD;
					}
					break;
				
				case STOP:
					I2C->CR_SR = I2C_CR_STO;
					i2c_transfer.state = DONE;
					if (i2c_transfer.transfer_done)
					{
						i2c_transfer.transfer_done();
					}
					break;				
			}
		}
		else 
		{
			if ((i2c_transfer.state == STOP) && i2c_transfer.cmd != I2C_WRITE)
			{
				i2c_transfer.buf[i2c_transfer.cnt] = I2C->TX_RX;
				i2c_transfer.state = DONE;
				if (i2c_transfer.transfer_done)
				{
					i2c_transfer.transfer_done();
				}
			}
		}
	}
//	if ((i2c_sr & 0x40) == 0)
//	{
//		i2c_transfer.state = IDLE;
//	}
}

/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/
