/**
  ******************************************************************************
  * @file    stm8l15x_rst.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the RST peripheral:
  *           - Flag management
  *           - NRST Pin configuration
  *
  *  @verbatim
  *               
  *          ===================================================================
  *                               RST specific features
  *          ===================================================================
  *
  *           When a reset occurs, there is a reset phase from the external pin 
  *           pull-down to the internal reset signal release. During this phase,
  *           the microcontroller sets some hardware configurations before going
  *           to the reset vector.
  *           At the end of this phase, most of the registers are configured with
  *           their “reset state” values. 
  *           During the reset phase, some pin configurations may be different from
  *           their “reset state” configuration.
  *           
  *           The NRST pin is an input and can be configured as open-drain output
  *           using the RST_GPOutputEnable() function 
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

#include "stm8l15x_rst.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup RST 
  * @brief RST driver modules
  * @{
  */ 
/* Private define ------------------------------------------------------------*/
#define RST_CR_MASK  0xD0 /*!< Enable the GPIO */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup RST_Private_Functions
  * @{
  */

/** @defgroup RST_Group1 Flag management functions
 *  @brief   Flag management functions 
 *
@verbatim   
 ===============================================================================
                       Flag management functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief   Checks whether the specified RST flag is set or not.
  * @param   RST_Flag : specify the reset flag to check.
  *          This parameter can be one of the following values:
  *            @arg RST_FLAG_PORF: POR reset flag
  *            @arg RST_FLAG_SWIMF: SWIM reset flag
  *            @arg RST_FLAG_ILLOPF: Illegal opcode reset flag
  *            @arg RST_FLAG_IWDGF: Independent watchdog reset flag 
  *            @arg RST_FLAG_WWDGF: Window watchdog reset flag
  *            @arg RST_FLAG_BORF: BOR reset flag
  * @retval The new state of RST_Flag (SET or RESET).
  */
FlagStatus RST_GetFlagStatus(RST_FLAG_TypeDef RST_Flag)
{
  /* Check the parameters */
  assert_param(IS_RST_FLAG(RST_Flag));

  /* Get flag status */
  return((FlagStatus)(((uint8_t)(RST->SR & RST_Flag) == (uint8_t)0x00) ? RESET : SET));
}

/**
  * @brief  Clears the specified RST flag.
  * @param   RST_Flag : specify the reset flag to check.
  *          This parameter can be one of the following values:
  *            @arg RST_FLAG_PORF: POR reset flag
  *            @arg RST_FLAG_SWIMF: SWIM reset flag
  *            @arg RST_FLAG_ILLOPF: Illegal opcode reset flag
  *            @arg RST_FLAG_IWDGF: Independent watchdog reset flag 
  *            @arg RST_FLAG_WWDGF: Window watchdog reset flag
  *            @arg RST_FLAG_BORF: BOR reset flag
  * @retval None
  */
void RST_ClearFlag(RST_FLAG_TypeDef RST_Flag)
{
  /* Check the parameters */
  assert_param(IS_RST_FLAG(RST_Flag));

  RST->SR = (uint8_t)RST_Flag;
}

/**
  * @}
  */
  
/** @defgroup RST_Group2 NRST Pin configuration function
 *  @brief   NRST Pin configuration function 
 *
@verbatim   
 ===============================================================================
                      NRST Pin configuration function
 ===============================================================================  

@endverbatim
  * @{
  */
  
/**
  * @brief  Configures the reset pad as GP output.
  * @param  None
  * @retval None
  */
void RST_GPOutputEnable(void)
{

  RST->CR = RST_CR_MASK;
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
