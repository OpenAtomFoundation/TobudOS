/**
  ******************************************************************************
  * @file    USART/USART_HyperTerminal_Interrupts/stm8l15x_conf.h
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    30-September-2014
  * @brief   Library configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */  

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_CONF_H
#define __STM8L15x_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/* Uncomment the line below to enable peripheral header file inclusion */
#include "stm8l15x_adc.h"
#include "stm8l15x_aes.h"
#include "stm8l15x_beep.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_comp.h"
#include "stm8l15x_dac.h"
#include "stm8l15x_dma.h"
#include "stm8l15x_exti.h"
#include "stm8l15x_flash.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_i2c.h"
#include "stm8l15x_irtim.h"
#include "stm8l15x_itc.h"
#include "stm8l15x_iwdg.h"
#include "stm8l15x_lcd.h"
#include "stm8l15x_pwr.h"
#include "stm8l15x_rst.h"
#include "stm8l15x_rtc.h"
#include "stm8l15x_spi.h"
#include "stm8l15x_syscfg.h"
#include "stm8l15x_tim1.h"
#include "stm8l15x_tim2.h"
#include "stm8l15x_tim3.h"
#include "stm8l15x_tim4.h"
#include "stm8l15x_tim5.h"
#include "stm8l15x_usart.h"
#include "stm8l15x_wfe.h"
#include "stm8l15x_wwdg.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    (1) */ 

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed. 
  *   If expr is true, it returns no value.
  * @retval : None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __STM8L15x_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
