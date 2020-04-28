/**
  ******************************************************************************
  * @file    stm8l15x_beep.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the BEEPER (BEEP) peripheral:           
  *           - Initialization and Configuration
  *           - Low Speed Internal Clock(LSI) Calibration
  *
  *  @verbatim  
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================  
  *          1- Make sure that the LS RC clock calibration is performed by the following 
  *            steps:
  *               - Connect internally the LS clock source to TIM2 channel 1 input
  *                 capture for measurement using BEEP_LSClockToTIMConnectCmd() function
  *               - Update the BEEP_CSR register by the measured LSI frequency 
  *                  --> During this phase the BEEPER must be disabled to avoid 
  *                      unwanted interrupts  
  *
  *          2- Configure the output beeper frequency using the BEEP_Init() function
  *
  *          3- Enable the beeper using the BEEP_Cmd() function
  *
  *  @endverbatim   
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
#include "stm8l15x_beep.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
/** @defgroup BEEP 
  * @brief BEEP driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup BEEP_Private_Functions
  * @{
  */

/** @defgroup BEEP_Group1 Initialization and Configuration functions
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
                      Initialization and Configuration functions
 ===============================================================================  
  This section provides functions allowing to:
   - Initialize and configure the Beeper frequency
   - Enable and Disable the Beeper output
   
@endverbatim
  * @{
  */
  
/**
  * @brief  Deinitializes the BEEP peripheral registers to their default
  *         reset values.
  * @param  None
  * @retval None
  */
void BEEP_DeInit(void)
{
  BEEP->CSR1 = BEEP_CSR1_RESET_VALUE;
  BEEP->CSR2 = BEEP_CSR2_RESET_VALUE;
}

/**
  * @brief  Initializes the BEEP function according to the specified parameters.
  * @note   The LS RC calibration must be performed before calling this function.
  * @param  BEEP_Frequency Frequency selection.
  *         This parameter can be one of the values of @ref BEEP_Frequency_TypeDef.
  * @retval None
  */
void BEEP_Init(BEEP_Frequency_TypeDef BEEP_Frequency)
{

  /* Check parameter */
  assert_param(IS_BEEP_FREQUENCY(BEEP_Frequency));

  /* Set a default calibration value if no calibration is done */
  if ((BEEP->CSR2 & BEEP_CSR2_BEEPDIV) == BEEP_CSR2_BEEPDIV)
  {
    BEEP->CSR2 &= (uint8_t)(~BEEP_CSR2_BEEPDIV); /* Clear bits */
    BEEP->CSR2 |= BEEP_CALIBRATION_DEFAULT;
  }

  /* Select the output frequency */
  BEEP->CSR2 &= (uint8_t)(~BEEP_CSR2_BEEPSEL);
  BEEP->CSR2 |= (uint8_t)(BEEP_Frequency);

}

/**
  * @brief  Enable or disable the BEEP function.
  * @note   Initialisation of BEEP and LS RC calibration must be done before.
  * @param  NewState Indicates the new state of the BEEP function.
  * @retval None
  */
void BEEP_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the BEEP peripheral */
    BEEP->CSR2 |= BEEP_CSR2_BEEPEN;
  }
  else
  {
    /* Disable the BEEP peripheral */
    BEEP->CSR2 &= (uint8_t)(~BEEP_CSR2_BEEPEN);
  }
}

/**
  * @}
  */

/** @defgroup BEEP_Group2 Low Speed Internal Clock(LSI) Calibration functions
 *  @brief    Low Speed Internal Clock(LSI) Calibration functions 
 *
@verbatim   
 ===============================================================================
              Low Speed Internal Clock(LSI) Calibration functions
 ===============================================================================  

  This section provides functions allowing to measure and calibrate the internal 
  low speed clock source to ensure better BEEPER output frequency .
  
  A typical configuration for LSI calibration is done following these steps :
   1. Disable the Beeper to avoid any unwanted interrupt using BEEP_Cmd() function 
   2. Measure the LSI clock frequency by connecting it internally to TIM2 input capture  
      using BEEP_LSClockToTIMConnectCmd() function.
   3. Calibrate the beeper frequency with the measured LSI clock frequency using 
      BEEP_LSICalibrationConfig() function.
   
@endverbatim
  * @{
  */
  
/**
  * @brief  Enable or disable the LS clock source connection to TIM for measurement.
  * @param  NewState Indicates the new state of the LS clock to TIM connection
  * @retval None
  */
void BEEP_LSClockToTIMConnectCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Connect LS clock to TIM for measurement */
    BEEP->CSR1 |= BEEP_CSR1_MSR;
  }
  else
  {
    /* Disconnect LS clock to TIM */
    BEEP->CSR1 &= (uint8_t)(~BEEP_CSR1_MSR);
  }
}
/**
  * @brief  Update CSR register with the measured LSI frequency.
  * @note   BEEP must be disabled to avoid unwanted interrupts.
  * @note   Prescaler calculation:
  *         A is the integer part of LSIFreqkHz/4 and x the decimal part.
  *         x <= A/(1+2A) is equivalent to A >= x(1+2A)
  *         and also to 4A >= 4x(1+2A) [F1]
  *         but we know that A + x = LSIFreqkHz/4 ==> 4x = LSIFreqkHz-4A
  *         so [F1] can be written :
  *         4A >= (LSIFreqkHz-4A)(1+2A)
  * @param  LSIFreqHz Low Speed RC frequency measured by timer (in Hz).
  * @retval None
  */
void BEEP_LSICalibrationConfig(uint32_t LSIFreqHz)
{
  uint16_t lsifreqkhz;
  uint16_t A;

  /* Check parameter */
  assert_param(IS_LSI_FREQUENCY(LSIFreqHz));

  lsifreqkhz = (uint16_t)(LSIFreqHz / 1000); /* Converts value in kHz */

  /* Calculation of BEEPER calibration value */

  BEEP->CSR2 &= (uint8_t)(~BEEP_CSR2_BEEPDIV); /* Clear bits */

  A = (uint16_t)(lsifreqkhz >> 3U); /* Division by 8, keep integer part only */

  if ((8U * A) >= ((lsifreqkhz - (8U * A)) * (1U + (2U * A))))
  {
    BEEP->CSR2 |= (uint8_t)(A - 2U);
  }
  else
  {
    BEEP->CSR2 |= (uint8_t)(A - 1U);
  }
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
