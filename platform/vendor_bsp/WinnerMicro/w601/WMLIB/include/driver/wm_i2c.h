/**************************************************************************//**
 * @file     wm_i2c.h
 * @author
 * @version
 * @date
 * @brief
 * @copyright (c) 2014 Winner Microelectronics Co., Ltd. All rights reserved.
 *****************************************************************************/

#ifndef __WM_I2C_H
#define __WM_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wm_regs.h"
#include "wm_type_def.h"
typedef struct 
{	
	__IO uint32_t PRER_LO;	
	__IO uint32_t PRER_HI;	
	__IO uint32_t CTRL;	
	__O  uint32_t TX_RX;	
	__O  uint32_t CR_SR;	
	__I  uint32_t TXR;	
	__I  uint32_t CR;
}I2C_T;
#define I2C		((I2C_T *)(HR_I2C_BASE_ADDR))

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup I2C_Driver_APIs I2C Driver APIs
 * @brief I2C driver APIs
 */

/**
 * @addtogroup I2C_Driver_APIs
 * @{
 */

/**
 * @brief	Init the I2C module
 * @param freq   I2C reference clock frequency in Hz that will be used
 * @note
 *   the value must be between 100k and 400k
 */
void tls_i2c_init(u32 freq);

#define  wm_i2c_int_mask(bl)   do { tls_bitband_write(HR_I2C_CTRL, 6, bl);}while(0);

/**
 * @brief	send stop signal
 *
 */
void tls_i2c_stop(void);

/**
 * @brief	Waiting for ack signal
 * @retval
 *	- \ref WM_FAILED
 *	- \ref WM_SUCCESS
 */
int tls_i2c_wait_ack(void);

/**
 * @brief	Writes the data to data register of I2C module
 * when ifstart one the start signal will be sent followed by the data
 * when ifstart zero only the data will be send
 * @param[in] data	the data will be write to the data register of I2C module
 * @param[in] ifstart	when one send start signal, when zero don't
 * @retval
 *
 */
void tls_i2c_write_byte(u8 data, u8 ifstart);

/**
 * @brief	Get the data stored in data register of I2C module
 * @param[in] ifack	when one send ack after reading the data register,when zero don't
 * @param[in] ifstop	when one send stop signal after read, when zero do not send stop
 * @retval
 *	the received data
 */
u8 tls_i2c_read_byte(u8 ifack, u8 ifstop);

/**
 * @brief          This function is used to register i2c transfer done callback function.
 * @param[in]      done  is the i2c transfer done callback function.
 * @retval         None
 * @note           None
 */
void wm_i2c_transfer_done_register(void (*done)(void));

/**
 * @brief	Start writing through int mode
 * @param[in] devaddr	the device address 
 * @param[in] wordaddr when one send stop signal after read, when zero do not send stop
 * @param[in] buf	the address point where data shoule be stored
 * @param[in] len	the length of data will be received 
 * @retval	
 *	- \ref WM_FAILED
 *	- \ref WM_SUCCESS
 */
int wm_i2c_start_write_it(uint8_t devaddr, uint8_t wordaddr, uint8_t * buf, uint16_t len);

/**
 * @brief	Get the data stored in data register of I2C module
 * @param[in] ifack	when one send ack after reading the data register,when zero don't
 * @param[in] ifstop when one send stop signal after read, when zero do not send stop
 * @retval	the received data 
 */
int wm_i2c_start_read_it(uint8_t devaddr, uint8_t wordaddr, uint8_t * buf, uint16_t len);
/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/
