/**
  ******************************************************************************
  * @file    stm8l15x_irtim.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to configure the IRTIM peripheral.
  *
  *  @verbatim
  *  
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *          This driver provides functions to:
  *             1. Enable the IRTIM peripheral
  *             2. Enable the high sink mode on the IRTIM pin 
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
#include "stm8l15x_irtim.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @defgroup IRTIM 
  * @brief IRTIM driver modules
  * @{
  */
   
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/** @defgroup IRTIM_Private_Functions
  * @{
  */

/** @defgroup IRTIM_Group1 IRTIM configuration functions
 *  @brief   IRTIM configuration functions 
 *
@verbatim   
 ===============================================================================
                       IRTIM configuration functions
 ===============================================================================  
  
       ===================================================================      
                       IRTIM Driver: how to use it
       =================================================================== 
       To generate the infrared remote control signal, perform the following steps:
          1. Use TIM2 channel 1 to generate the high frequency carrier signal
             by calling TIM2_OC1Init()
          2. Use TIM3 channel 1 to generate the modulation envelope by
             calling TIM3_OC1Init()
          3. Enable the IRTIM peripheral using IRTIM_Cmd()

          Note1: When IRTIM peripheral is enabled, TIM2 channel 1 and TIM3 channel 1
                 become inactive (no signal on output) and can be used as GPIO.
                 
          Note2: The high sink LED driver capability (only available on the IRTIM pin)
                 can be activated using IRTIM_HighSinkODCmd() to sink the high 
                 current needed to directly control an infrared LED

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the IRTIM peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void IRTIM_DeInit(void)
{
  IRTIM->CR = IRTIM_CR_RESET_VALUE;
}

/**
  * @brief  Enables or disables the IRTIM peripheral.
  * @param  NewState : The new state of the IRTIM peripheral.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void IRTIM_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* set or Reset the EN Bit */
  if (NewState ==   DISABLE)
  {
    IRTIM->CR &= (uint8_t)(~IRTIM_CR_EN) ;
  }
  else
  {
    IRTIM->CR |= IRTIM_CR_EN ;
  }
}

/**
  * @brief  Enables or disables the High sink open drain buffer of the IRTIM peripheral.
  * @param  NewState : The new state of the High sink open drain buffer.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void IRTIM_HighSinkODCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* set or Reset the EN Bit */
  if (NewState == DISABLE)
  {
    IRTIM->CR &= (uint8_t)(~IRTIM_CR_HSEN) ;
  }
  else
  {
    IRTIM->CR |= IRTIM_CR_HSEN ;
  }
}

/**
  * @}
  */

/** @defgroup IRTIM_Group2 IRITM status management functions
 *  @brief    IRITM status management functions 
 *
@verbatim   
 ===============================================================================
                     IRITM status management functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Checks whether the IRTIM device is enabled or not.
  * @param  None
  * @retval state of the IRTIM device.
  */

FunctionalState IRTIM_GetStatus(void)
{
  return ((FunctionalState) (IRTIM->CR & IRTIM_CR_EN));
}

/**
  * @brief  Checks whether the IRTIM High Sink Open Drain buffer is Enabled or not.
  * @param  None
  * @retval state of High Sink Open Drain buffer.
  */
FunctionalState IRTIM_GetHighSinkODStatus(void)
{
  return ((FunctionalState)(IRTIM->CR & IRTIM_CR_HSEN));
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
