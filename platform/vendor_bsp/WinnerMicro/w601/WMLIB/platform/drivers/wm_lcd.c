/**************************************************************************//**
 * @file     wm_lcd.c
 * @author
 * @version  
 * @date  
 * @brief
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd. All rights reserved.
 *****************************************************************************/
 
#include "wm_lcd.h"

#define RTC_CLK		(32000UL)	

/**
 * @brief   Initialize LCD Frame Counter
 * @param[in] freq   LCD reference refresh frequency in Hz that will be used
 */
void tls_lcd_fresh_ratio(uint16_t freq)
{
	uint8_t com_num;

	if (freq == 0)
	{
		freq = 60;
	}
	
	com_num = tls_bitband_read(HR_LCD_CR, LCD_CR_MODE_Pos) ? 4 : 8;
	LCD->FRAMECNT = RTC_CLK/(com_num * freq);	
}

/**
 * @brief
 *   Turn on or clear a segment 
 *
 * @param[in] com
 *   Which COM line to update
 *
 * @param[in] bit
 *   Bit index of which field to change
 *
 * @param[in] enable
 *   When one will set segment, when zero will clear segment
 *
 * @param[in] mode
 *	 when one max configuration is 4x20, when zero max configuration is 8x16
 *
 */
void tls_lcd_seg_set(int com, int bit, int on_off)
{
	int mode;
	
	/** get the mode congfig  when one max configuration is 4x20, when zero max configuration is 8x16 */
	mode = tls_bitband_read(HR_LCD_CR, LCD_CR_MODE_Pos);	
	
	if (!mode && (com < 0 || com > 7) && (bit < 0 || bit > 15))
	{
		return ;
	}
	if (mode && (com < 0 || com > 3) && (bit < 0 || bit > 19))
	{
		return ;
	}
	
	switch(com)
	{
		/** COM0 */
		case 0:
			tls_bitband_write(HR_LCD_COM0_1_SEG, bit, on_off);
			break;
		
		/** COM1 */
		case 1:
			if (mode)
				tls_bitband_write(HR_LCD_COM2_3_SEG, bit, on_off);
			else
			{
				bit += 16;
				tls_bitband_write(HR_LCD_COM0_1_SEG, bit, on_off);
			}				
			break;
		
		/** COM2 */
		case 2:
			if (mode)
				tls_bitband_write(HR_LCD_COM4_5_SEG, bit, on_off);
			else 
				tls_bitband_write(HR_LCD_COM2_3_SEG, bit, on_off);
			break;
		
		/** COM3 */
		case 3:
			if (mode)
				tls_bitband_write(HR_LCD_COM6_7_SEG, bit, on_off);
			else
			{
				bit += 16;
				tls_bitband_write(HR_LCD_COM2_3_SEG, bit, on_off);
			}
			break;
		
		/** COM4 */
		case 4:
			tls_bitband_write(HR_LCD_COM4_5_SEG, bit, on_off);
			break;
		
		/** COM5 */
		case 5:
			bit += 16;
			tls_bitband_write(HR_LCD_COM4_5_SEG, bit, on_off);
			break;
		
		/** COM6 */
		case 6:
			tls_bitband_write(HR_LCD_COM6_7_SEG, bit, on_off);
			break;
		
		/** COM7 */
		case 7:
			bit += 16;
			tls_bitband_write(HR_LCD_COM6_7_SEG, bit, on_off);
			break;
		
		default:
			break;
	}
}

/**
 * @brief  enable or disable the LCD module gpio
 *
 * @param[in] the config value per bit for one LCD gpio, set enable gpio output ,clear disabled
 *
 */
void tls_lcd_io_config(unsigned int val)
{
	tls_reg_write32(HR_LCD_IO_EN, val);
}

/**
 * @brief
 *   select the voltage of LCD module
 *
 */
void tls_lcd_vlcd_sel(LCD_VlcdDef vlcd)
{	
	LCD->CTRL &= ~LCD_VLCD_MASK;
	LCD->CTRL |= vlcd;	
}


/**
 * @brief
 *   set the duty of LCD module
 *  
 */
void tls_lcd_duty_set(LCD_DutyDef duty)
{	
	LCD->CTRL &= ~LCD_DUTY_MASK;
	LCD->CTRL |= duty;
}


/**
 * @brief
 *   set the bias of LCD module
 * 
 */
void tls_lcd_bias_set(LCD_BiasDef bias)
{
	LCD->CTRL &= ~LCD_BIAS_MASK;
	LCD->CTRL |= bias;		
}

/**
 * @brief
 *   initialize the lcd module
 * 
 */
void tls_lcd_init(tls_lcd_options_t *opts)
{
	LCD->CTRL = 0;
	LCD->CTRL = opts->mode | opts->bias | opts->duty | opts->vlcd | (1 << 12);
	tls_lcd_fresh_ratio(opts->fresh_rate);	
	TLS_LCD_ENABLE(opts->enable);
	TLS_LCD_POWERDOWM(0);
}
