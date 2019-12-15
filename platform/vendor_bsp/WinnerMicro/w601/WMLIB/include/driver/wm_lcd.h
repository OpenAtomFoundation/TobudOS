/**
 * @file    wm_lcd.h
 *
 * @brief   LCD Driver Module
 *
 * @author  dave
 *
 * @copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */

#ifndef __WM_LCD_H
#define __WM_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wm_regs.h"
#include <stdbool.h>

/**
 * @brief LCD Register Declaration
 *
 */
typedef struct
{
	/** Control Register */
	__IO uint32_t CTRL;
	/** Refresh Rate Prescaler Register */
	__IO uint32_t FRAMECNT;
	/** Data Register */
	__IO uint32_t COM01;
	/** Data Register */
	__IO uint32_t COM23;
	/** Data Register */
	__IO uint32_t COM45;
	/** Data Register */
	__IO uint32_t COM67;
	/** LCD IO Control Register */
	__IO uint32_t IO_EN;

} LCD_T;
/** LCD base pointer */
#define LCD 		((LCD_T *)HR_LCD_REG_BASE)

typedef enum
{
	/** Static (2 levels) */
	BIAS_STATIC	   = LCD_BIAS_STATIC,
	/** 1/2 Bias (3 levels) */
	BIAS_ONEHALF   = LCD_BIAS_ONEHALF,
	/** 1/3 Bias (4 levels) */
	BIAS_ONETHIRD  = LCD_BIAS_ONETHIRD,
	/** 1/4 Bias (4 levels) */
	BIAS_ONEFOURTH = LCD_BIAS_ONEFOURTH,
} LCD_BiasDef;

typedef enum
{
	/** VLCD 2.7v */
	VLCD27		= LCD_VLCD_27,
	/** VLCD 2.9v */
	VLCD29		= LCD_VLCD_29,
	/** VLCD 3.1v */
	VLCD31		= LCD_VLCD_31,
	/** VLCD 3.3v */
	VLCD33		= LCD_VLCD_33,
} LCD_VlcdDef;

typedef enum
{
	/** max configuration is 8x16 */
	MODE8x16 = LCD_CR_MODE_8X16,
	/** max configuration is 4x20 */
	MODE4x20 = LCD_CR_MODE_4x20,
} LCD_ModeDef;

typedef enum
{
	/** Static (segments can be multiplexed with LCD_COM[0]) */
	DUTY_STATIC		= LCD_DUTY_STATIC,
	/**  1/2 Duty cycle (segments can be multiplexed with LCD_COM[0:1]) */
	DUTY_ONEHALF	= LCD_DUTY_ONEHALF,
	/**  1/3 Duty cycle (segments can be multiplexed with LCD_COM[0:2]) */
	DUTY_ONETHIRD	= LCD_DUTY_ONETHIRD,
	/**  1/4 Duty cycle (segments can be multiplexed with LCD_COM[0:3]) */
	DUTY_ONEFOURTH	= LCD_DUTY_ONEFOURTH,
	/**  1/5 Duty cycle (segments can be multiplexed with LCD_COM[0:4]) */
	DUTY_ONEFIFTH	= LCD_DUTY_ONEFIFTH,
	/**  1/6 Duty cycle (segments can be multiplexed with LCD_COM[0:5]) */
	DUTY_ONESIXTH	= LCD_DUTY_ONESIXTH,
	/**  1/7 Duty cycle (segments can be multiplexed with LCD_COM[0:6]) */
	DUTY_ONESEVENTH	= LCD_DUTY_ONESEVENTH,
	/**  1/8 Duty cycle (segments can be multiplexed with LCD_COM[0:7]) */
	DUTY_ONEEIGHTH	= LCD_DUTY_ONEEIGHTH,
} LCD_DutyDef;

typedef struct tls_lcd_options
{
	bool enable;
	/** 8x16 or 4x20*/
	LCD_ModeDef  mode;
	/** Bias configuration */
	LCD_BiasDef  bias;
	/** Duty configuration */
	LCD_DutyDef  duty;
	/** Vlcd configuration */
	LCD_VlcdDef  vlcd;
	/** Fresh rate configuration */
	uint16_t	 fresh_rate;
} tls_lcd_options_t;


/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup LCD_Driver_APIs LCD Driver APIs
 * @brief LCD driver APIs
 */

/**
 * @addtogroup LCD_Driver_APIs
 * @{
 */

/**
 * @brief initialize the LCD module
 *
 */
void tls_lcd_init(tls_lcd_options_t *opts);

/**
 * @brief   Initialize LCD Frame Counter
 * @param[in] freq   LCD reference refresh frequency in Hz that will be used
 *
 */
void tls_lcd_fresh_rate(uint16_t freq);

/**
 * @brief   Turn on or clear a segment
 * @param[in] com   Which COM line to update
 * @param[in] bit   Bit index of which field to change
 * @param[in] enable   When one will set segment, when zero will clear segment
 * @note   Before this function be called, the module must have been intialized
 */
void tls_lcd_seg_set(int com, int bit, int on_off);

/**
 * @brief   Select the voltage of LCD module
 * @param[in] vlcd   This parameter can be one of the following values:
 *     - \ref VLCD27
 *     - \ref VLCD29
 *     - \ref VLCD31
 *     - \ref VLCD33
 */
void tls_lcd_vlcd_sel(LCD_VlcdDef vlcd);

/**
 * @brief   Set the duty of LCD module
 * @param[in] duty   This parameter can be one of the following values:
 *     - \ref DUTY_STATIC
 *     - \ref DUTY_ONEHALF
 *     - \ref DUTY_ONETHIRD
 *     - \ref DUTY_ONEFOURTH
 *     - \ref DUTY_ONEFIFTH
 *     - \ref DUTY_ONESIXTH
 *     - \ref DUTY_ONESEVENTH
 *	   - \ref DUTY_ONEEIGHTH
 *
 */
void tls_lcd_duty_set(LCD_DutyDef duty);

/**
 * @brief   Set the bias of LCD module
 * @param[in] duty   This parameter can be one of the following values:
 *     - \ref BIAS_STATIC
 *     - \ref BIAS_ONEHALF
 *     - \ref BIAS_ONETHIRD
 *     - \ref BIAS_ONEFOURTH
 *
 */
void tls_lcd_bias_set(LCD_BiasDef bias);

/**
 * @brief  Enable or disable the LCD module gpio
 *
 * @param[in] the config value per bit for one LCD gpio, set enable gpio output ,clear disabled
 *
 */
void tls_lcd_io_config(unsigned int val);

/**
 * @brief   Enable or disable clock of LCD module
 * @param[in] enable   When one enable the clock of LCD module, when zero disable
 */
#define TLS_LCD_CLK_ENABLE(enable) \
		do { \
			tls_bitband_write(HR_CLK_BASE_ADDR, HR_CLK_LCD_GATE_Pos, enable); \
		} while(0)


/**
 * @brief   Enable or disable the LCD module
 * @param[in] enable   When one enable the LCD module, when zero disable
 *
 */
#define TLS_LCD_ENABLE(enable)	\
		do { \
			tls_bitband_write(HR_LCD_CR, LCD_CR_EN_Pos, enable); \
		} while(0)

/**
 * @brief   Enable or disable the LCD module
 * @param[in] enable   When one close LCD module, when zero open the LCD module
 *
 */
#define TLS_LCD_POWERDOWM(enable)	\
		do { \
			tls_bitband_write(HR_LCD_CR, LCD_CR_PD_Pos, enable); \
		} while(0)		

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
