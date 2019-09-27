/**
 * @file wm_adc.h
 *
 * @brief ADC Driver Module
 *
 * @author 
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */


#ifndef WM_ADC_H
#define WM_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wm_type_def.h"

/** ADC MACRO */
//每次启动dma之后，需要一段稳定时间，所以采集到的数据前面的12个byte不稳定，要舍去
#define ADC_DEST_BUFFER_DMA     		(u32)0x20028000
#define ADC_DEST_BUFFER_SIZE			65532	
#define SAMPLE_NUM_PER_CHANNEL  		20



#define CONFIG_ADC_CAL_OFFSET_TEMP12   	(1<<23)
#define CONFIG_ADC_G_CTRL12   			(1<<22)
#define CONFIG_ADC_CMP_POL				(1<<21) 
#define CONFIG_ADC_REF_SEL     			(1<<20)
#define CONFIG_ADC_BUF_BYPASS			(1<<19)
#define CONFIG_ADC_DMA_MASK 			(0xFF<<11)
#define CONFIG_ADC_EN_CAL				(1<<10)
#define CONFIG_ADC_INT_CMP				(1<<9)
#define CONFIG_ADC_INT					(1<<8)
#define CONFIG_ADC_CMP_INT_MASK     	(1<<7)
#define CONFIG_ADC_INT_MASK     		(1<<6)
#define CONFIG_ADC_TEMP_ON				(1<<5)
#define CONFIG_ADC_START  				(1<<4)
#define CONFIG_ADC_CHL_MASK 			(0xF)

#define CONFIG_ADC_CHL_OFFSET			(0x0E)
#define CONFIG_ADC_CHL_VOLT				(0x0D)
#define CONFIG_ADC_CHL_TEMP				(0x0C)




#define CONFIG_ADC_INPUT_CMP_VAL(n)		((n&0x3FFF)<<14)
#define CONFIG_ADC_VCM(n)				(((u32)n&0x3F)<<26)
#define CONFIG_ADC_G_TEMP12(n)			((n&0x03)<<24)



#define ADC_INT_TYPE_ADC				0
#define ADC_INT_TYPE_DMA				1
#define ADC_INT_TYPE_ADC_COMP 			2

#define ADC_REFERENCE_EXTERNAL  		0       //外部参考
#define ADC_REFERENCE_INTERNAL  		1       //内部参考

typedef struct adc_st{
	u8 dmachannel;
	void (*adc_cb)(u16 *buf, u16 len);
	void (*adc_bigger_cb)(u16 *buf, u16 len);
	void (*adc_dma_cb)(u16 *buf,u16 len);
	u16 valuelen;		/*dma 采样数据长度*/
	u16 offset;
}ST_ADC;

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup ADC_Driver_APIs ADC Driver APIs
 * @brief ADC driver APIs
 */

/**
 * @addtogroup ADC_Driver_APIs
 * @{
 */

/**
 * @brief		This function is used to init ADC
 *
 * @param[in]	ifusedma		if use dma
 * @param[in]	dmachannel	dma channel
 *
 * @return		None
 *
 * @note			If the requested dma channel is already used by other task, system will auto use other dma channel.	
 */
void tls_adc_init(u8 ifusedma,u8 dmachannel);


/**
 * @brief		This function is used to register interrupt callback function
 *
 * @param[in]	inttype	interrupt type
 *				 ADC_INT_TYPE_ADC		 adc interrupt,user get adc result from the callback function.
 *				 ADC_INT_TYPE_DMA		 dma interrupt,dma transfer the adc result to the user's buffer.
 * @param[in]	callback	interrupt callback function
 *
 * @return		None
 *
 * @note			None
 */
void tls_adc_irq_register(int inttype, void (*callback)(u16 *buf, u16 len));

/**
 * @brief		This function is used to clear the interrupt source
 *
 * @param[in]	inttype 	interrupt type
 *				ADC_INT_TYPE_ADC		 adc interrupt,user get adc result from the callback function.
 *				ADC_INT_TYPE_DMA		 dma interrupt,dma transfer the adc result to the user's buffer.
 *				ADC_INT_TYPE_ADC_COMP adc compare with setting data
 *
 * @return		None
 *
 * @note			None
 */
void tls_adc_clear_irq(int inttype);

/**
 * @brief	     This function is used to start adc,use dma for transfer data
 *
 * @param[in]    channel	adc channel,from 0 to 3 is single input;4 and 5 is differential input
 *
 * @param[in]    length 	byte data length,is an integer multiple of half word,need <= 0x500
 *
 * @return	     None
 *
 * @note		     None
 */
void tls_adc_start_with_dma(int Channel, int Length);

/**
 * @brief		 This function is used to start adc,use cpu
 *
 * @param[in]	 channel	adc channel,from 0 to 3 is single input;4 and 5 is differential input
 *
 * @return		 None
 *
 * @note			 None
 */
void tls_adc_start_with_cpu(int Channel);

/**
 * @brief		 This function is used to enable input buffer calibration
 *
 * @param[in]	 None
 *
 * @return		 None
 *
 * @note			 None
 */
void tls_adc_enable_calibration_buffer_offset(void);

/**
 * @brief		 This function is used to sample voltage using cpu
 *
 * @param[in]	 None
 *
 * @return		 None
 *
 * @note			 None
 */
void tls_adc_voltage_start_with_cpu(void);

/**
 * @brief		 This function is used to sample temperature using cpu
 *
 * @param[in]	 calenflag 1:enable calibration , 0: disable calibration
 *
 * @return		 None
 *
 * @note			 None
 */
void tls_adc_temp_offset_with_cpu(u8 calenflag);

/**
 * @brief		 This function is used to read adc result
 *
 * @param[in]	 None
 *
 * @return		 None
 *
 * @note			 None
 */
u16 tls_read_adc_result(void);

/**
 * @brief		 This function is used to stop the adc
 *
 * @param[in]	 ifusedma 1:use dma, 0:not use dma
 *
 * @return		 None
 *
 * @note			 None
 */
void tls_adc_stop(int ifusedma);

/**
 * @brief		 This function is used to config adc compare register
 *
 * @param[in]	 cmp_data compare data
 *
 * @param[in]	 cmp_pol compare polarity
 *
 * @return		 None
 *
 * @note			 None
 */
void tls_adc_config_cmp_reg(int cmp_data, int cmp_pol);

/**
 * @brief		 This function is used to set adc reference source
 *
 * @param[in]	 ref
 *                       ADC_REFERENCE_EXTERNAL
 *				ADC_REFERENCE_INTERNAL 
 *
 * @return		 None
 *
 * @note			 None
 */
void tls_adc_reference_sel(int ref);

/**
 * @brief		 This function is used to set clock division
 *
 * @param[in]	 div
 *
 * @return		 None
 *
 * @note			 None
 */
void tls_adc_set_clk(int div);

void signedToUnsignedData(u16 *adcValue, u16 *offset);

/**
 * @brief		 This function is used to set buffer bypass
 *
 * @param[in]	 isset  1: buffer bypass, 0: buffer work
 *
 * @return		 None
 *
 * @note			 None
 */
void tls_adc_buffer_bypass_set(u8 isset);

/**
 * @brief		 This function is used to start compare
 *
 * @param[in]	 Channel   sample channel
 *
 * @param[in]	 cmp_data compare data
 *
 * @param[in]	 cmp_pol  compare polarity
 *
 * @return		 None
 *
 * @note			 None
 */
 void tls_adc_cmp_start(int Channel, int cmp_data, int cmp_pol);

u16 adc_get_offset(void);
u32 adc_get_interTemp(void);
u16 adc_get_inputVolt(u8 channel);
u16 adc_get_interVolt(void);
u32 adc_temp(void);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* end of WM_ADC_H */

/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/
