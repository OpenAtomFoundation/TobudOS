/**************************************************************************//**
 * @file     wm_7816.h
 * @author
 * @version
 * @date
 * @brief
 * @copyright (c) 2014 Winner Microelectronics Co., Ltd. All rights reserved.
 *****************************************************************************/
#ifndef WM_7816_H_
#define WM_7816_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdbool.h>
#include "wm_regs.h"
#include "wm_type_def.h"
#include "wm_io.h"

#define WM_SC_RST_PIN		WM_IO_PB_23 //(23)    
#define WM_SC_PWR_PIN		WM_IO_PB_29 //(29)	

#define WM_SC_DEFAULT_FD	(372)

typedef struct sc_io_map_ {
	enum tls_io_name  clk_pin_num;
	uint32_t  clk_opt;
	enum tls_io_name io_pin_num;
	uint32_t  io_opt;
	uint8_t	initialed;
} sc_io_map;

extern sc_io_map sc_io;

/**
 * @brief
 *	This function is used to config the pin in gpio or 7816 mode for the 7816 power on timing
 *
 * @param[in] mode : 1--gpio mode ; 0--7816 mode	
 *
 * @retval
 */
void wm_sc_io_clk_config(uint8_t mode);

/**
 * @brief
 *	close af to use as gpio
 * @retval
 */
void wm_sc_powerInit(void);

/**
 * @brief
 *	power on the 7816 device if power is controled by GPIO 
 * @retval
 */
void wm_sc_poweron(void);

/**
 * @brief
 *	power off the 7816 device if power is controled by GPIO 
 * @retval
 */
void wm_sc_poweroff(void);

/**
 * @brief
 *	driver the reset gpio in low level 
 * @retval
 */
void wm_sc_rst_low(void);

/**
 * @brief
 *	driver the reset gpio in high level 
 * @retval
 */
void wm_sc_rst_high(void);

/**
 * @brief
 *	hotrest the 7816 device obey the 7816-3 timing
 * @retval
 */
void wm_sc_hotreset(void);

/**
 * @brief
 *	colreset the 7816 device obey the 7816-3 timing
 * @retval
 */
void wm_sc_colreset(void);

/**
 * @brief
 *	deactive the 7816 device obey the 7816-3 timing
 * @retval
 */
void wm_sc_deactive(void);

/**
 * @brief
 *	This function is used to config the block guard time param in 7816 mode
 * @param[in] bgt : the value of blcok guard time will be set	
 * @retval
 */
void wm_sc_set_bgt(uint8_t bgt);

/**
 * @brief
 *	This function is used to config the tx retry count when detect err signal
 * @param[in] count : the value of retry time will be set 7 for max
 * @retval
 */
void wm_sc_tx_retry_times(uint8_t count);

/**
 * @brief
 *	This function is used to config the rx retry count when detect parity error
 * @param[in] count : the value of retry time will be set 7 for max
 * @retval
 */
void wm_sc_rx_retry_times(uint8_t count);

/**
 * @brief
 *	This function is used to config the etu param
 * @param[in] etu : the value of etu will be set
 * @retval
 */
void wm_sc_set_etu(uint16_t etu);

/**
 * @brief
 *	This function config the module clock freq
 * @param[in] freq : the value of clock freq
 * @retval
 */
void wm_sc_set_frequency(uint32_t freq);

/**
 * @brief
 *	config recv or not when parity error 
 * @param[in] bl :	1--- recv 
 *					0--- don't recv
 * @retval
 */
static inline void wm_sc_parity_recv(bool bl)
{
	tls_bitband_write(HR_UART2_LINE_CTRL, 9, bl);
}

/**
 * @brief
 *	select the model in 7816 or uart function 
 * @param[in] bl :	1---7816 mode 
 *					0---uart mode
 * @retval
 */
static inline void wm_sc_7816_mode(bool bl)
{
	tls_bitband_write(HR_UART2_LINE_CTRL, 24, bl);	
}

/**
 * @brief
 *	This function is used to config the guard time param
 * @param[in] bwt : the value of the guard time will be set
 * @retval
 */
static inline void wm_sc_set_guardtime(uint8_t gt)
{
	tls_reg_write32(HR_UART2_GUARD_TIME, gt);
}

/**
 * @brief
 *	This function is used to config the CWT or BWT param
 * @param[in] bwt : the value of CWT or BWT will be set
 * @retval
 */
static inline void wm_sc_set_bcwt(uint32_t bwt)
{
	bwt = (bwt > 0xFFFFFF) ? 0xFFFFFF : bwt;
	tls_reg_write32(HR_UART2_WAIT_TIME, bwt);	
}

/**
 * @brief
 *	module errsignal int enable or disable 
 * @param[in] bl :	1---enable 
 *					0---disable
 * @retval
 */
static inline void wm_sc_tx_errsignal_mask(bool bl)
{
	tls_bitband_write(HR_UART2_INT_MASK, 9, bl);
}

/**
 * @brief
 *	config the module protol 
 * @param[in] bl :	1--- T1 protocol
 *					0--- T0 protocol
 * @retval
 */
static inline void wm_sc_set_protocol(bool bl)
{
	tls_bitband_write(HR_UART2_LINE_CTRL, 8, bl);	
}

/**
 * @brief
 *	get the module protol 
 * @retval
 *		1--- T1 protocol
 *		0--- T0 protocol
 */
static inline uint8_t wm_sc_get_protocol()
{
	return tls_bitband_read(HR_UART2_LINE_CTRL, 8);
}

/**
 * @brief
 *	smart card clock output enable or disable
 * @param[in] bl : 0---enable;
 *				   1---disable;
 * @retval
 */
static inline void wm_sc_clk_enable(bool bl)
{
	tls_bitband_write(HR_UART2_LINE_CTRL, 10, bl);
}

#ifdef __cplusplus
}
#endif

#endif
