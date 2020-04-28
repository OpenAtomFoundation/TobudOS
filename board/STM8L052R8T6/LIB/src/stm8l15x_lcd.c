/**
  ******************************************************************************
  * @file    stm8l15x_lcd.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the LCD controller (LCD) peripheral:           
  *           - Initialization and configuration
  *           - LCD RAM memory write
  *           - Interrupts and flags management   
  *           
  *  @verbatim
  *      
  *          ===================================================================      
  *                                    LCD Clock
  *          ===================================================================    
  *          LCD has tow clock sources:
  *            - RTCCLK divided by 2 used to generate LCD frame rate
  *            - LCDCLK to used for LCD registers read/write access
  *                   
  *          To configure the RTCCLK/LCDCLK, proceed as follows:
  *            - Enable RTC clock using CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE)
  *              function
  *            - Enable LCD clock using CLK_PeripheralClockConfig(CLK_Peripheral_LCD, ENABLE)
  *              function  
  *            - Select the RTC clock source using the CLK_RTCClockConfig() function.    
  *            
  *          ===================================================================      
  *                                  LCD and low power modes
  *          =================================================================== 
  *          The LCD remains active during Active-halt, Wait, Low power run and 
  *          Low power wait modes unless disabled
  *              
  *          ===================================================================
  *                                 How to use this driver
  *          =================================================================== 
  *          1. Enable RTC clock using CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE) function 
  *            
  *          2. Enable LCD clock using CLK_PeripheralClockConfig(CLK_Peripheral_LCD, ENABLE) function
  *            
  *          3. Select the RTC clock source using the CLK_RTCClockConfig() function. 
  *          
  *          4. Configure the LCD prescaler, divider, duty, bias and voltage source
  *             using LCD_Init() function     
  *            
  *          5. Call the LCD_PortMaskConfig() function to assign the unused SEGx
  *             (segment) and COMx (common) pins as standard general purpose IOs
  *
  *          6. Optionally you can enable/configure:
  *              -  LCD High Drive using the LCD_HighDriveCmd() function
  *              -  LCD Pulse ON Duration using the LCD_PulseOnDurationConfig() function
  *              -  LCD Dead Time using the LCD_DeadTimeConfig() function  
  *              -  The LCD Blink mode and frequency using the LCD_BlinkConfig() function
  *              -  The LCD Contrast using the LCD_ContrastConfig() function  
  *
  *          7. Call the LCD_Cmd() to enable the LCD controller
  *          
  *          8. Write to the LCD RAM memory using the LCD_WriteRAM() function.
  *          
  *  @endverbatim
  *           
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

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_lcd.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @defgroup LCD 
  * @brief LCD driver modules
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup LCD_Private_Functions
  * @{
  */

/** @defgroup LCD_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
                    Initialization and Configuration functions
 ===============================================================================  

@endverbatim
  * @{
  */
  
/**
  * @brief  Deinitializes the LCD Controller registers to their default reset values.
  * @param  None
  * @retval None
  */
void LCD_DeInit(void)
{
  uint8_t counter = 0;

  LCD->CR1 = LCD_CR1_RESET_VALUE;
  LCD->CR2 = LCD_CR2_RESET_VALUE;
  LCD->CR3 = LCD_CR3_RESET_VALUE;
  LCD->FRQ = LCD_FRQ_RESET_VALUE;

  for (counter = 0;counter < 0x05; counter++)
  {
    LCD->PM[counter] = LCD_PM_RESET_VALUE;
  }

  for (counter = 0;counter < 0x16; counter++)
  {
    LCD->RAM[counter] =  LCD_RAM_RESET_VALUE;
  }

  LCD->CR4 = LCD_CR4_RESET_VALUE;

}

/**
  * @brief  Initializes the LCD according to the specified parameters.
  * @param  LCD_CLKPrescalerDiv : LCD clock prescaler 
  *          This parameter can be one of the following values:
  *            @arg LCD_Prescaler_1: CLKprescaler = ClKinput 
  *            @arg LCD_Prescaler_2: CLKprescaler = ClKinput/2 
  *            @arg LCD_Prescaler_4: CLKprescaler = ClKinput/4
  *            @arg LCD_Prescaler_8: CLKprescaler = ClKinput/8 
  *            @arg LCD_Prescaler_16: CLKprescaler = ClKinput/16
  *            @arg LCD_Prescaler_32: CLKprescaler = ClKinput/32
  *            @arg LCD_Prescaler_64: CLKprescaler = ClKinput/64 
  *            @arg LCD_Prescaler_128: CLKprescaler = ClKinput/128 
  *            @arg LCD_Prescaler_256: CLKprescaler = ClKinput/256 
  *            @arg LCD_Prescaler_512: CLKprescaler = ClKinput/512 
  *            @arg LCD_Prescaler_1024: CLKprescaler = ClKinput/1024 
  *            @arg LCD_Prescaler_2048: CLKprescaler = ClKinput/2048 
  *            @arg LCD_Prescaler_4096: CLKprescaler = ClKinput/4096 
  *            @arg LCD_Prescaler_8192: CLKprescaler = ClKinput/8192 
  *            @arg LCD_Prescaler_16384: CLKprescaler = ClKinput/16384 
  *            @arg LCD_Prescaler_32768: CLKprescaler = ClKinput/32768              
  * @param  LCD_Divider : LCD clock divider
  *          This parameter can be one of the following values:
  *            @arg LCD_Divider_16: LCD frequency = CLKprescaler/16 
  *            @arg LCD_Divider_17: LCD frequency = CLKprescaler/17
  *            @arg LCD_Divider_18: LCD frequency = CLKprescaler/18
  *            @arg LCD_Divider_19: LCD frequency = CLKprescaler/19
  *            @arg LCD_Divider_20: LCD frequency = CLKprescaler/20
  *            @arg LCD_Divider_21: LCD frequency = CLKprescaler/21
  *            @arg LCD_Divider_22: LCD frequency = CLKprescaler/22
  *            @arg LCD_Divider_23: LCD frequency = CLKprescaler/23
  *            @arg LCD_Divider_24: LCD frequency = CLKprescaler/24
  *            @arg LCD_Divider_25: LCD frequency = CLKprescaler/25
  *            @arg LCD_Divider_26: LCD frequency = CLKprescaler/26
  *            @arg LCD_Divider_27: LCD frequency = CLKprescaler/27
  *            @arg LCD_Divider_28: LCD frequency = CLKprescaler/28
  *            @arg LCD_Divider_29: LCD frequency = CLKprescaler/29
  *            @arg LCD_Divider_30: LCD frequency = CLKprescaler/30
  *            @arg LCD_Divider_31: LCD frequency = CLKprescaler/31
  * @param  LCD_Duty : LCD duty
  *          This parameter can be one of the following values:
  *            @arg LCD_Duty_Static: Static duty 
  *            @arg LCD_Duty_1_2: 1/2 duty
  *            @arg LCD_Duty_1_3: 1/3 duty
  *            @arg LCD_Duty_1_4: 1/4 duty
  *            @arg LCD_Duty_1_8: 1/8 duty   
  * @param  LCD_Bias : LCD bias
  *          This parameter can be one of the following values:
  *            @arg LCD_Bias_1_4: 1/4 bias
  *            @arg LCD_Bias_1_3: 1/3 bias
  *            @arg LCD_Bias_1_2: 1/2 bias
  * @param  LCD_VoltageSource : LCD voltage source
  *          This parameter can be one of the following values:
  *            @arg LCD_VoltageSource_Internal: Internal voltage source
  *            @arg LCD_VoltageSource_External: External voltage source
  * @retval None
  */
void LCD_Init(LCD_Prescaler_TypeDef LCD_Prescaler, LCD_Divider_TypeDef LCD_Divider,
              LCD_Duty_TypeDef LCD_Duty, LCD_Bias_TypeDef LCD_Bias,
              LCD_VoltageSource_TypeDef LCD_VoltageSource)
{
  /* Check function parameters */
  assert_param(IS_LCD_CLOCK_PRESCALER(LCD_Prescaler));
  assert_param(IS_LCD_CLOCK_DIVIDER(LCD_Divider));
  assert_param(IS_LCD_DUTY(LCD_Duty));
  assert_param(IS_LCD_BIAS(LCD_Bias));
  assert_param(IS_LCD_VOLTAGE_SOURCE(LCD_VoltageSource));

  LCD->FRQ &= (uint8_t)(~LCD_FRQ_PS);     /* Clear the prescaler bits */
  LCD->FRQ |= LCD_Prescaler;

  LCD->FRQ &= (uint8_t)(~LCD_FRQ_DIV);     /* Clear the divider bits */
  LCD->FRQ |= LCD_Divider;

  /* Configure the Duty cycle */
  LCD->CR1 &= (uint8_t)(~LCD_CR1_DUTY);    /* Clear the duty bits */
  LCD->CR4 &= (uint8_t)(~LCD_CR4_DUTY8);   /* Clear the DUTY8 bit */

  if (LCD_Duty == LCD_Duty_1_8)
  {
    LCD->CR4 |= (uint8_t)((uint8_t)((uint8_t)LCD_Duty & (uint8_t)0xF0) >> 4);
  }
  else
  {
    LCD->CR1 |= (uint8_t)((uint8_t)LCD_Duty & (uint8_t)0x0F);
  }

  /* Configure the Bias */
  LCD->CR1 &= (uint8_t)(~LCD_CR1_B2);      /* Clear the B2 bit */
  LCD->CR4 &= (uint8_t)(~LCD_CR4_B4);      /* Clear the B4 bit */

  if (LCD_Bias == LCD_Bias_1_4)
  {
    LCD->CR1 |= (uint8_t)((uint8_t)LCD_Bias & (uint8_t)0x0F);
    LCD->CR4 |= (uint8_t)((uint8_t)((uint8_t)LCD_Bias & (uint8_t)0xF0) >> 4);
  }
  else
  {
    LCD->CR1 |= (uint8_t)((uint8_t)LCD_Bias & (uint8_t)0x0F);
  }

  LCD->CR2 &= (uint8_t)(~LCD_CR2_VSEL);    /* Clear the voltage source bit */
  LCD->CR2 |= LCD_VoltageSource;

}

/**
  * @brief  Configures the LCD Port Mask.
  * @param  LCD_PortMaskRegister: The LCD register index in the port mask matrix.
  *          This parameter can be one of the following values:
  *            @arg LCD_PortMaskRegister_0: PortMask Register 0  
  *            @arg LCD_PortMaskRegister_1: PortMask Register 1
  *            @arg LCD_PortMaskRegister_2: PortMask Register 2
  *            @arg LCD_PortMaskRegister_3: PortMask Register 3
  *            @arg LCD_PortMaskRegister_4: PortMask Register 4
  *            @arg LCD_PortMaskRegister_5: PortMask Register 5        
  * @param  LCD_Mask : Value to be written
  * @retval None
  */
void LCD_PortMaskConfig(LCD_PortMaskRegister_TypeDef LCD_PortMaskRegister, uint8_t LCD_Mask)
{
  /* Check function parameters */
  assert_param(IS_LCD_PORT_MASK(LCD_PortMaskRegister));

  /* Copy data bytes to Port mask register */
  LCD->PM[LCD_PortMaskRegister] =  LCD_Mask;

}

/**
  * @brief  Enables or disables the LCD Controller.
  * @param  NewState: new state of the LCD peripheral.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  * @retval None
  */
void LCD_Cmd(FunctionalState NewState)
{
  /* Check function parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    LCD->CR3 |= LCD_CR3_LCDEN; /* Enable the LCD peripheral*/
  }
  else
  {
    LCD->CR3 &= (uint8_t)(~LCD_CR3_LCDEN); /* Disable the LCD peripheral*/
  }
}

/**
  * @brief  Enables or disables the low resistance divider. Displays with high 
  *         internal resistance may need a longer drive time to achieve 
  *         satisfactory contrast. This function is useful in this case if some 
  *         additional power consumption can be tolerated.
  * @note   When this mode is enabled, the PulseOn Duration (PON) have to be 
  *         programmed to 1/CK_PS (LCD_PulseOnDuration_1).    
  * @param  NewState: new state of the low resistance divider.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LCD_HighDriveCmd(FunctionalState NewState)
{
  /* Check function parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    LCD->CR2 |= LCD_CR2_HD; /* Permanently enable low resistance divider */
  }
  else
  {
    LCD->CR2 &= (uint8_t)(~LCD_CR2_HD); /* Permanently disable low resistance divider */
  }
}

/**
  * @brief  Configures the LCD pulses on duration.
  * @param  LCD_PulseOnDuration: specifies the LCD pulse on duration in terms of 
  *         CLKprescaler (prescaled LCD clock period) pulses.
  *          This parameter can be one of the following values:
  *            @arg LCD_PulseOnDuration_0: 0 pulse
  *            @arg LCD_PulseOnDuration_1: Pulse ON duration = 1/CLKprescaler
  *            @arg LCD_PulseOnDuration_2: Pulse ON duration = 2/CLKprescaler
  *            @arg LCD_PulseOnDuration_3: Pulse ON duration = 3/CLKprescaler
  *            @arg LCD_PulseOnDuration_4: Pulse ON duration = 4/CLKprescaler
  *            @arg LCD_PulseOnDuration_5: Pulse ON duration = 5/CLKprescaler
  *            @arg LCD_PulseOnDuration_6: Pulse ON duration = 6/CLKprescaler
  *            @arg LCD_PulseOnDuration_7: Pulse ON duration = 7/CLKprescaler
  * @retval None
  */
void LCD_PulseOnDurationConfig(LCD_PulseOnDuration_TypeDef LCD_PulseOnDuration)
{
  /* Check function parameters */
  assert_param(IS_LCD_PULSE_DURATION(LCD_PulseOnDuration));

  LCD->CR2 &= (uint8_t)(~LCD_CR2_PON); /* Clear the pulses on duration bits */
  LCD->CR2 |= LCD_PulseOnDuration;
}

/**
  * @brief  Configures the LCD dead time.
  * @param  LCD_DeadTime: specifies the LCD dead time.
  *          This parameter can be one of the following values:
  *            @arg LCD_DeadTime_0: No dead Time
  *            @arg LCD_DeadTime_1: One Phase between different couple of Frame
  *            @arg LCD_DeadTime_2: Two Phase between different couple of Frame
  *            @arg LCD_DeadTime_3: Three Phase between different couple of Frame
  *            @arg LCD_DeadTime_4: Four Phase between different couple of Frame
  *            @arg LCD_DeadTime_5: Five Phase between different couple of Frame
  *            @arg LCD_DeadTime_6: Six Phase between different couple of Frame 
  *            @arg LCD_DeadTime_7: Seven Phase between different couple of Frame
  * @retval None
  */
void LCD_DeadTimeConfig(LCD_DeadTime_TypeDef LCD_DeadTime)
{
  /* Check function parameters */
  assert_param(IS_LCD_DEAD_TIME(LCD_DeadTime));

  LCD->CR3 &= (uint8_t)(~LCD_CR3_DEAD);  /* Clear the dead time bits  */

  LCD->CR3 |= LCD_DeadTime;

}

/**
  * @brief  Configures the LCD Blink mode and Blink frequency.
  * @param  LCD_BlinkMode: specifies the LCD blink mode.
  *          This parameter can be one of the following values:
  *            @arg LCD_BlinkMode_Off:           Blink disabled
  *            @arg LCD_BlinkMode_SEG0_COM0:     Blink enabled on SEG[0], COM[0] (1 pixel)
  *            @arg LCD_BlinkMode_SEG0_AllCOM:   Blink enabled on SEG[0], all COM (up to 8 
  *                                       pixels according to the programmed duty)
  *            @arg LCD_BlinkMode_AllSEG_AllCOM: Blink enabled on all SEG and all COM 
  *                                       (all pixels)
  * @param  LCD_BlinkFrequency: specifies the LCD blink frequency.
  *          This parameter can be one of the following values:
  *            @arg LCD_BlinkFrequency_Div8:    The Blink frequency = fLcd/8
  *            @arg LCD_BlinkFrequency_Div16:   The Blink frequency = fLcd/16
  *            @arg LCD_BlinkFrequency_Div32:   The Blink frequency = fLcd/32
  *            @arg LCD_BlinkFrequency_Div64:   The Blink frequency = fLcd/64 
  *            @arg LCD_BlinkFrequency_Div128:  The Blink frequency = fLcd/128
  *            @arg LCD_BlinkFrequency_Div256:  The Blink frequency = fLcd/256
  *            @arg LCD_BlinkFrequency_Div512:  The Blink frequency = fLcd/512
  *            @arg LCD_BlinkFrequency_Div1024: The Blink frequency = fLcd/1024
  * @retval None
  */
void LCD_BlinkConfig(LCD_BlinkMode_TypeDef LCD_BlinkMode, LCD_BlinkFrequency_TypeDef LCD_BlinkFrequency)
{
  /* Check function parameters */
  assert_param(IS_LCD_BLINK_MODE(LCD_BlinkMode));
  assert_param(IS_LCD_BLINK_FREQUENCY(LCD_BlinkFrequency));

  LCD->CR1 &= (uint8_t)(~LCD_CR1_BLINK); /* Clear the blink mode bits */
  LCD->CR1 |= LCD_BlinkMode; /* Config the LCD Blink Mode */

  LCD->CR1 &= (uint8_t)(~LCD_CR1_BLINKF); /* Clear the blink frequency bits */
  LCD->CR1 |= LCD_BlinkFrequency; /* Config the LCD Blink Frequency */

}

/**
  * @brief  Configures the LCD Contrast.
  * @param  LCD_Contrast: specifies the LCD Contrast.
  *          This parameter can be one of the following values:
  *            @arg LCD_Contrast_Level_0: Medium Density / High Density Maximum Voltage = 2.60V / 2.60V 
  *            @arg LCD_Contrast_Level_1: Medium Density / High Density Maximum Voltage = 2.70V / 2.73V
  *            @arg LCD_Contrast_Level_2: Medium Density / High Density Maximum Voltage = 2.80V / 2.86V
  *            @arg LCD_Contrast_Level_3: Medium Density / High Density Maximum Voltage = 2.90V / 2.99V
  *            @arg LCD_Contrast_Level_4: Medium Density / High Density Maximum Voltage = 3.00V / 3.12V
  *            @arg LCD_Contrast_Level_5: Medium Density / High Density Maximum Voltage = 3.10V / 3.25V
  *            @arg LCD_Contrast_Level_6: Medium Density / High Density Maximum Voltage = 3.20V / 3.38V
  *            @arg LCD_Contrast_Level_7: Medium Density / High Density Maximum Voltage = 3.30V / 3.51V
  * @retval None
  */
void LCD_ContrastConfig(LCD_Contrast_TypeDef LCD_Contrast)
{
  /* Check function parameters */
  assert_param(IS_LCD_CONTRAST(LCD_Contrast));

  LCD->CR2 &= (uint8_t)(~LCD_CR2_CC); /* Clear the contrast bits  */
  LCD->CR2 |= LCD_Contrast; /* Select the maximum voltage value Vlcd */

}

/**
  * @}
  */

/** @defgroup LCD_Group2  LCD RAM memory write functions
 *  @brief    LCD RAM memory write functions 
 *
@verbatim   
 ===============================================================================
                           LCD RAM memory write functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Writes a word in the specific LCD RAM.
  * @param  LCD_RAMRegister: specifies the LCD Contrast.
  *          This parameter can be one of the following values:
  *            @arg LCD_RAMRegister_0: LCD RAM Register 0
  *            @arg LCD_RAMRegister_1: LCD RAM Register 1
  *            @arg LCD_RAMRegister_2: LCD RAM Register 2
  *            @arg LCD_RAMRegister_3: LCD RAM Register 3
  *            @arg LCD_RAMRegister_4: LCD RAM Register 4
  *            @arg LCD_RAMRegister_5: LCD RAM Register 5
  *            @arg LCD_RAMRegister_6: LCD RAM Register 6 
  *            @arg LCD_RAMRegister_7: LCD RAM Register 7  
  *            @arg LCD_RAMRegister_8: LCD RAM Register 8
  *            @arg LCD_RAMRegister_9: LCD RAM Register 9
  *            @arg LCD_RAMRegister_10: LCD RAM Register 10
  *            @arg LCD_RAMRegister_11: LCD RAM Register 11
  *            @arg LCD_RAMRegister_12: LCD RAM Register 12 
  *            @arg LCD_RAMRegister_13: LCD RAM Register 13 
  *            @arg LCD_RAMRegister_14: LCD RAM Register 14 
  *            @arg LCD_RAMRegister_15: LCD RAM Register 15
  *            @arg LCD_RAMRegister_15: LCD RAM Register 16 
  *            @arg LCD_RAMRegister_15: LCD RAM Register 17 
  *            @arg LCD_RAMRegister_15: LCD RAM Register 18 
  *            @arg LCD_RAMRegister_15: LCD RAM Register 19 
  *            @arg LCD_RAMRegister_15: LCD RAM Register 20 
  *            @arg LCD_RAMRegister_15: LCD RAM Register 21             
  * @param  LCD_Data: specifies LCD Data Value to be written.
  * @retval None
  */
void LCD_WriteRAM(LCD_RAMRegister_TypeDef LCD_RAMRegister, uint8_t LCD_Data)
{
  /* Check function parameters */
  assert_param(IS_LCD_RAM_REGISTER(LCD_RAMRegister));

  /* Copy data bytes to RAM register */
  LCD->RAM[LCD_RAMRegister] =  LCD_Data;

}

/**
  * @brief  Select the LCD page where the data will be written.
  * @param  LCD_Page: The accessed LCD page.
  *          This parameter can be one of the following values:
  *            @arg LCD_PageSelection_FirstPage: The LCD RAM is selected as the first page
  *            @arg LCD_PageSelection_SecondPage: The LCD RAM is selected as the second page  
  * @retval None
  */
void LCD_PageSelect(LCD_PageSelection_TypeDef LCD_PageSelection)
{
  /* Check function parameters */
  assert_param(IS_LCD_PAGE_SELECT(LCD_PageSelection));

  LCD->CR4 &= (uint8_t)(~LCD_CR4_PAGECOM); /* Clear the PAGE COM bit */
  LCD->CR4 |= LCD_PageSelection; /* Select the LCD page */

}

/**
  * @}
  */

/** @defgroup LCD_Group3 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions 
 *
@verbatim   
 ===============================================================================
                   Interrupts and flags management functions
 ===============================================================================  

@endverbatim
  * @{
  */
  
  
/**
  * @brief  Enables or disables the start of frame interrupt.
  * @param NewState: new state of the specified LCD interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LCD_ITConfig(FunctionalState NewState)
{
  /* Check function parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    LCD->CR3 |= LCD_CR3_SOFIE; /* Enable interrupt*/
  }
  else
  {
    LCD->CR3 &= (uint8_t)(~LCD_CR3_SOFIE); /* Disable interrupt*/
  }

}

/**
  * @brief  Checks whether the LCD start of new frame flag is set or not.
  * @retval The new state of the start of new frame flag.
  *   This returned value can be: SET or RESET.   
  */
FlagStatus LCD_GetFlagStatus(void)
{
  FlagStatus status = RESET;

  /* Check the status of the start of new frame LCD flag */
  if ((LCD->CR3 & (uint8_t)LCD_CR3_SOF) != (uint8_t)RESET)
  {
    status = SET; /* Flag is set */
  }
  else
  {
    status = RESET; /* Flag is reset*/
  }
  /* Return the FLAG status */
  return status;

}

/**
  * @brief  Clears the start of frame LCD flag.
  * @param  None
  * @retval None
  */
void LCD_ClearFlag(void)
{
  /* Clear the flag bit */
  LCD->CR3 |= (uint8_t)(LCD_CR3_SOFC);

}

/**
  * @brief  Checks whether the start of frame interrupt has occurred or not.
  * @param  None
  * @retval The new state of the start of new frame interrupt.
  *   This returned value can be: SET or RESET.   
  */
ITStatus LCD_GetITStatus(void)
{
  ITStatus pendingbitstatus = RESET;
  uint8_t enablestatus = 0;

  enablestatus = (uint8_t)((uint8_t)LCD->CR3 & LCD_CR3_SOFIE);
  /* Check the status of the start of frame interrupt */
  if (((LCD->CR3 & LCD_CR3_SOF) != RESET) && enablestatus)
  {
    /* ITPENDINGBIT is set */
    pendingbitstatus = SET;
  }
  else
  {
    /* ITPENDINGBIT is reset */
    pendingbitstatus = RESET;
  }
  /* Return the ITPENDINGBIT status */
  return  pendingbitstatus;

}

/**
  * @brief  Clears the start of frame interrupt pending bits.
  * @param  None
  * @retval None
  */
void LCD_ClearITPendingBit(void)
{
  /* Clear the flag bit */
  LCD->CR3 |= (uint8_t)(LCD_CR3_SOFC);

}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
