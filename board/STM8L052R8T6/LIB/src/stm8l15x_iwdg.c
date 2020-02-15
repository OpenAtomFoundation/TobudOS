/**
  ********************************************************************************
  * @file    stm8l15x_iwdg.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Independent watchdog (IWDG) peripheral:           
  *           - Prescaler and Counter configuration
  *           - IWDG activation
  *
  *  @verbatim  
  *  
  *          ===================================================================
  *                                     IWDG features
  *          ===================================================================
  *    
  *          The IWDG can be started by either software or hardware (configurable
  *          through option byte).
  *            
  *          The IWDG is clocked by its own dedicated low-speed clock (LSI) and
  *          thus stays active even if the main clock fails.
  *          Once the IWDG is started, the LSI is forced ON and cannot be disabled
  *          (LSI cannot be disabled too), and the counter starts counting down from 
  *          the reset value of 0xFF. When it reaches the end of count value (0x00)
  *          a system reset is generated.
  *          The IWDG counter should be reloaded at regular intervals to prevent
  *          an MCU reset.
  *                           
  *          The IWDG is implemented in the VDD voltage domain that is still functional
  *          in Halt and Active Halt mode depending on the configuration of the IWDG_HALT
  *          option byte. (IWDG reset can wake-up from Halt).          
  *            
  *          IWDGF flag in RST_SR register can be used to inform when a IWDG
  *          reset occurs.
  *            
  *          Min-max timeout value @38KHz (LSI): 0.10 ms / ~1.724s
  *          The IWDG timeout may vary due to LSI frequency dispersion. STM8L15x
  *          devices provide the capability to measure the LSI frequency (LSI clock
  *          connected internally to TIM2 CH1 input capture). The measured value
  *          can be used to have an IWDG timeout with an acceptable accuracy. 
  *          For more information, please refer to the STM8L15x Reference manual
  *          
  *                            
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *          1. Enable write access to IWDG_PR and IWDG_RLR registers using
  *             IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable) function
  *               
  *          2. Configure the IWDG prescaler using IWDG_SetPrescaler() function
  *            
  *          3. Configure the IWDG counter value using IWDG_SetReload() function.
  *             This value will be loaded in the IWDG counter each time the counter
  *             is reloaded, then the IWDG will start counting down from this value.
  *            
  *          4. Start the IWDG using IWDG_Enable() function, when the IWDG is used
  *             in software mode (no need to enable the LSI, it will be enabled
  *             by hardware)
  *             
  *          5. Then the application program must reload the IWDG counter at regular
  *             intervals during normal operation to prevent an MCU reset, using
  *             IWDG_ReloadCounter() function.      
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
#include "stm8l15x_iwdg.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @defgroup IWDG 
  * @brief IWDG driver modules
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup IWDG_Private_Functions
  * @{
  */

/** @defgroup IWDG_Group1 Prescaler and Counter configuration functions
 *  @brief   Prescaler and Counter configuration functions
 *
@verbatim   
 ===============================================================================
                  Prescaler and Counter configuration functions
 ===============================================================================  

@endverbatim
  * @{
  */
  
/**
  * @brief  Enables or disables write access to IWDG_PR and IWDG_RLR registers.
  * @param  IWDG_WriteAccess: new state of write access to IWDG_PR and IWDG_RLR registers.
  *          This parameter can be one of the following values:
  *            @arg IWDG_WriteAccess_Enable: Enable write access to IWDG_PR and IWDG_RLR registers
  *            @arg IWDG_WriteAccess_Disable: Disable write access to IWDG_PR and IWDG_RLR registers
  * @retval None
  */
void IWDG_WriteAccessCmd(IWDG_WriteAccess_TypeDef IWDG_WriteAccess)
{
  /* Check the parameters */
  assert_param(IS_IWDG_WRITE_ACCESS_MODE(IWDG_WriteAccess));
  IWDG->KR = IWDG_WriteAccess; /* Write Access */
}

/**
  * @brief  Sets IWDG Prescaler value.
  * @param  IWDG_Prescaler: specifies the IWDG Prescaler value.
  *          This parameter can be one of the following values:
  *            @arg IWDG_Prescaler_4: IWDG prescaler set to 4
  *            @arg IWDG_Prescaler_8: IWDG prescaler set to 8
  *            @arg IWDG_Prescaler_16: IWDG prescaler set to 16
  *            @arg IWDG_Prescaler_32: IWDG prescaler set to 32
  *            @arg IWDG_Prescaler_64: IWDG prescaler set to 64
  *            @arg IWDG_Prescaler_128: IWDG prescaler set to 128
  *            @arg IWDG_Prescaler_256: IWDG prescaler set to 256
  * @retval None
  */
void IWDG_SetPrescaler(IWDG_Prescaler_TypeDef IWDG_Prescaler)
{
  /* Check the parameters */
  assert_param(IS_IWDG_PRESCALER_VALUE(IWDG_Prescaler));
  IWDG->PR = IWDG_Prescaler;
}

/**
  * @brief  Sets IWDG Reload value.
  * @note   Write access should be enabled
  * @param  IWDG_Reload : Reload register value.
  *         This parameter must be a number between 0 and 0xFF.
  * @retval None
  */
void IWDG_SetReload(uint8_t IWDG_Reload)
{
  IWDG->RLR = IWDG_Reload;
}

/**
  * @brief  Reloads IWDG counter with value defined in the reload register
  *         (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @param  None
  * @retval None
  */
void IWDG_ReloadCounter(void)
{
  IWDG->KR = IWDG_KEY_REFRESH;
}

/**
  * @}
  */

/** @defgroup IWDG_Group2 IWDG activation function
 *  @brief   IWDG activation function 
 *
@verbatim   
 ===============================================================================
                          IWDG activation function
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables IWDG (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @param  None
  * @retval None
  */
void IWDG_Enable(void)
{
  IWDG->KR = IWDG_KEY_ENABLE;
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