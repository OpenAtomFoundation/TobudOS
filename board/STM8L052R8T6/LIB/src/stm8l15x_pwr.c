/**
  ******************************************************************************
  * @file    stm8l15x_pwr.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Power Controller (PWR) peripheral:           
  *           - PVD configuration
  *           - Ultra Low Power mode configuration
  *           - Interrupts and flags management
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
#include "stm8l15x_pwr.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup PWR 
  * @brief PWR driver modules
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup PWR_Private_Functions
  * @{
  */

/** @defgroup PWR_Group1 PVD configuration functions
 *  @brief   PVD configuration functions 
 *
@verbatim   
 ===============================================================================
                           PVD configuration functions
 ===============================================================================  

 - The PVD is used to monitor the VDD power supply by comparing it to a threshold
   selected by the PVD Level (PLS[2:0] bits in the PWR_CSR1).
 - The PVD can use an external input analog voltage (PVD_IN) which is compared 
   internally to VREFINT. The PVD_IN (PE6) has to be configured in Analog mode 
   when PWR_PVDLevel_PVDIn is selected (PLS[2:0] = 111).
 - A PVDO flag is available to indicate if VDD/VDDA is higher or lower than the 
   PVD threshold. An interrupt is generated in case enabled for PVD. 
 - The PVD interrup event has the capability to wake up the CPU from Halt/Active Halt modes.

@endverbatim
  * @{
  */
    
/**
  * @brief  Deinitializes the PWR peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void PWR_DeInit(void)
{
  PWR->CSR1 = PWR_CSR1_PVDIF;
  PWR->CSR2 = PWR_CSR2_RESET_VALUE;
}

/**
  * @brief  Configures the voltage threshold detected by the Power Voltage Detector(PVD).
  * @param  PWR_PVDLevel: specifies the PVD detection level
  *          This parameter can be one of the following values:
  *            @arg PWR_PVDLevel_1V85: PVD detection level set to 1.85V
  *            @arg PWR_PVDLevel_2V05: PVD detection level set to 2.05V
  *            @arg PWR_PVDLevel_2V26: PVD detection level set to 2.26V
  *            @arg PWR_PVDLevel_2V45: PVD detection level set to 2.45V
  *            @arg PWR_PVDLevel_2V65: PVD detection level set to 2.65V
  *            @arg PWR_PVDLevel_2V85: PVD detection level set to 2.85V
  *            @arg PWR_PVDLevel_3V05: PVD detection level set to 3.05V
  *            @arg PWR_PVDLevel_PVDIn: External input analog voltage (Compare internally to VREFINT)
  * @retval None
  */
void PWR_PVDLevelConfig(PWR_PVDLevel_TypeDef PWR_PVDLevel)
{
  /* Check the parameters */
  assert_param(IS_PWR_PVD_LEVEL(PWR_PVDLevel));

  /* Clear the PVD level */
  PWR->CSR1 &= (uint8_t)(~PWR_CSR1_PLS);

  /* Configure the PVD level */
  PWR->CSR1 |= PWR_PVDLevel;

}

/**
  * @brief  Enables or disables the Power Voltage Detector(PVD).
  * @param  NewState: new state of the PVD.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_PVDCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the PWR PVD */
    PWR->CSR1 |= PWR_CSR1_PVDE;
  }
  else
  {
    /* Disable the PWR PVD */
    PWR->CSR1 &= (uint8_t)(~PWR_CSR1_PVDE);
  }
}

/**
  * @}
  */

/** @defgroup PWR_Group2 Ultra Low Power mode configuration functions
 *  @brief   Ultra Low Power mode configuration functions 
 *
@verbatim   
 ===============================================================================
                    Ultra Low Power mode configuration functions
 ===============================================================================  

 - The internal voltage reference consumption is not negligible, in particular 
   in Halt and Active Halt modes. To reduce power consumption, use the PWR_UltraLowPowerCmd()
   function (ULP bit (Ultra low power) in the PWR_CSR2 register) to disable the 
   internal voltage reference. However, in this case, when exiting from the 
   Halt/Active Halt modes, the functions managed through the internal voltage reference 
   are not reliable during the internal voltage reference startup time (up to 3 ms).
   To reduce the wakeup time, the device can exit from Halt/Active Halt modes without 
   waiting for the internal voltage reference startup time. This is performed 
   by using the PWR_FastWakeUpCmd() function (setting the FWU bit (Fast
   wakeup) in the PWR_CSR2 register) before entering Halt/Active Halt mode.

@endverbatim
  * @{
  */
  
/**
  * @brief  Enables or disables the Fast WakeUp from Ultra Low Power mode.
  * @param  NewState: new state of the Fast WakeUp  functionality.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_FastWakeUpCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the PWR FWU */
    PWR->CSR2 |= PWR_CSR2_FWU;
  }
  else
  {
    /* Disable the PWR FWU */
    PWR->CSR2 &= (uint8_t)(~PWR_CSR2_FWU);
  }
}

/**
  * @brief  Enables or disables the Ultra Low Power mode.
  * @param  NewState: new state of the Ultra Low Power mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_UltraLowPowerCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the PWR ULP */
    PWR->CSR2 |= PWR_CSR2_ULP;
  }
  else
  {
    /* Disable the PWR ULP */
    PWR->CSR2 &= (uint8_t)(~PWR_CSR2_ULP);
  }
}

/**
  * @}
  */

/** @defgroup PWR_Group3 Interrupts and Flags management functions
 *  @brief   Interrupts and Flags management functions 
 *
@verbatim   
 ===============================================================================
                 Interrupts and Flags management functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Configures the Programmable Voltage Detector (PVD) Interrupt .
  * @param  NewState : Indicates the new state of the PVD interrupt.
  * @retval None
  */
void PWR_PVDITConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the PVD interrupt */
    PWR->CSR1 |= PWR_CSR1_PVDIEN;
  }
  else
  {
    /* Disable the PVD interrupt */
    PWR->CSR1 &= (uint8_t)(~PWR_CSR1_PVDIEN);
  }
}

/**
  * @brief  Checks whether the specified PWR flag is set or not.
  * @param  PWR_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg PWR_FLAG_PVDOF: PVD Output. This flag is valid only if PVD is enabled 
  *                                 by the PWR_PVDCmd() function.
  *            @arg PWR_FLAG_VREFINTF: Internal Voltage Reference Ready flag. This 
  *                                    flag indicates the state of the internal voltage reference, VREFINT.
  *            @arg PWR_FLAG_PVDIF: PVD Interrupt Flag, it's set by hardware when a PVD
  *       event occurs  
  * @retval The new state of PWR_FLAG (SET or RESET).
  */
FlagStatus PWR_GetFlagStatus(PWR_FLAG_TypeDef PWR_FLAG)
{
  FlagStatus bitstatus = RESET;

  /* Check the parameters */
  assert_param(IS_PWR_FLAG(PWR_FLAG));

  if ((PWR_FLAG & PWR_FLAG_VREFINTF) != 0)
  {
    if ((PWR->CSR2 & PWR_CR2_VREFINTF) != (uint8_t)RESET )
    {
      bitstatus = SET;
    }
    else
    {
      bitstatus = RESET;
    }
  }
  else
  {
    if ((PWR->CSR1 & PWR_FLAG) != (uint8_t)RESET )
    {
      bitstatus = SET;
    }
    else
    {
      bitstatus = RESET;
    }
  }

  /* Return the flag status */
  return((FlagStatus)bitstatus);
}

/**
  * @brief  Clears the PWR PVDIF Flag.
  * @param  None
  * @retval None
  */
void PWR_PVDClearFlag(void)
{
  /* Set the PVDIF to clear it */
  PWR->CSR1 |= PWR_CSR1_PVDIF;
}

/**
  * @brief  Checks whether the PVD interrupt has occurred or not.
  * @param  None
  * @retval The new state of the PVD Interrupt (SET or RESET).
  */
ITStatus PWR_PVDGetITStatus(void)
{
  ITStatus bitstatus = RESET;

  uint8_t PVD_itStatus = 0x0, PVD_itEnable = 0x0;

  PVD_itStatus = (uint8_t)(PWR->CSR1 & (uint8_t)PWR_CSR1_PVDIF);
  PVD_itEnable = (uint8_t)(PWR->CSR1 & (uint8_t)PWR_CSR1_PVDIEN);

  if ((PVD_itStatus != (uint8_t)RESET ) && (PVD_itEnable != (uint8_t)RESET))
  {
    bitstatus = (ITStatus)SET;
  }
  else
  {
    bitstatus = (ITStatus)RESET;
  }
  return ((ITStatus)bitstatus);
}

/**
  * @brief  Clears the PWR interrupt pending bit.
  * @param  None
  * @retval None
  */
void PWR_PVDClearITPendingBit(void)
{
  /* Set the PVDIF to clear it */
  PWR->CSR1 |= PWR_CSR1_PVDIF;
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
