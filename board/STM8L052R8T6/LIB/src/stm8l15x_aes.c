/**
  ******************************************************************************
  * @file    stm8l15x_aes.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Advanced Encryption Standard (AES) peripheral:           
  *           - Configuration
  *           - Read/Write operations
  *           - DMA transfers management  
  *           - Interrupts and flags management
  *
  *  @verbatim  
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================  
  *          1- Enable AES clock to get write access to AES registers
  *             using CLK_PeripheralClockConfig(CLK_Peripheral_AES, ENABLE);
  *
  *          2- Configure the AES operation mode using AES_OperationModeConfig()
  *
  *          3- If required, enable interrupt source using AES_ITConfig()
  *
  *          4- If required, when using the DMA mode 
  *               - Configure the DMA using DMA_Init()
  *               - Enable DMA requests using AES_DMAConfig()
  *
  *          5- Enable the AES peripheral using AES_Cmd()
  *
  *          6- Write data/key using AES_WriteSubData() / AES_WriteSubKey()
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
#include "stm8l15x_aes.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @defgroup AES 
  * @brief AES driver modules
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup AES_Private_Functions
  * @{
  */ 

/** @defgroup AES_Group1 Configuration
 *  @brief   Configuration
 *
@verbatim   
 ===============================================================================
                           Configuration
 ===============================================================================  

@endverbatim
  * @{
  */  
  
/**
  * @brief   Deinitializes the AES peripheral.
  * @param   None.
  * @retval  None
  */
void AES_DeInit(void)
{
  /* Set AES_CR to reset value 0x00, AES_SR is reset by setting ERRC and CCFC bits in CR */
  AES->CR = AES_CR_ERRC | AES_CR_CCFC;
  AES->DINR = AES_DINR_RESET_VALUE;       /* Set AES_DINR to reset value 0x00 */
  AES->DOUTR = AES_DOUTR_RESET_VALUE;     /* Set AES_DOUTR to reset value 0x00 */
}

/**
  * @brief   Configures the AES operation mode.
  * @param   AES_Operation : the selected AES operation mode.
  *          This parameter can be one of the following values:
  *            @arg AES_Operation_Encryp: AES in Encryption mode
  *            @arg AES_Operation_KeyDeriv: AES in Key Derivation mode
  *            @arg AES_Operation_Decryp: AES in Decryption mode
  *            @arg AES_Operation_KeyDerivAndDecryp: AES in Key Derivation and Decryption mode
  * @note   The operation mode must be configured when AES peripheral is disabled.
  * @retval  None
  */
void AES_OperationModeConfig(AES_Operation_TypeDef AES_Operation)
{
  /* Check the parameter */
  assert_param(IS_AES_MODE(AES_Operation));

  /* Reset the operation mode bits in CR register */
  AES->CR &= (uint8_t) (~AES_CR_MODE);

  /* Set the new operation mode bits in CR register */
  AES->CR |= (uint8_t) (AES_Operation);
}

/**
  * @brief  Enable the AES peripheral.
  * @param  NewState : The new state of the AES peripheral.
    *         This parameter can be: ENABLE or DISABLE.
  * @note   AES peripheral can be enabled once operation mode is configured using
  *         AES_OperationModeConfig()  
  * @retval None
  */
void AES_Cmd(FunctionalState NewState)
{
  /* Check the parameter */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    AES->CR |= (uint8_t) AES_CR_EN;   /**< AES Enable */
  }
  else
  {
    AES->CR &= (uint8_t)(~AES_CR_EN);  /**< AES Disable */
  }
}

/**
  * @}
  */

/** @defgroup AES_Group2 AES Read and Write
 *  @brief   AES Read and Write
 *
@verbatim   
 ===============================================================================
                        AES Read and Write operations
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Write data in DINR register to be processed by AES peripheral.
  * @param  Data: The data to be processed.
  * @note   When an unexpected write to DINR register is detected, WRERR flag is
  *         set.
  * @retval None
  */
void AES_WriteSubData(uint8_t Data)
{
  /* Write Data */
  AES->DINR = Data;
}

/**
  * @brief  Write key in DINR register.
  * @param  Key: The key to be used for encryption/decryption.
  * @note   When an unexpected write to DINR register is detected, WRERR flag is
  *         set.
  * @retval None
  */
void AES_WriteSubKey(uint8_t Key)
{
  /* Write key */
  AES->DINR = Key;
}

/**
  * @brief  Returns the data in DOUTR register processed by AES peripheral.
  * @note   When an unexpected read of DOUTR register is detected, RDERR flag is
  *         set
  * @retval The processed data.
  */
uint8_t AES_ReadSubData(void)
{
  return AES->DOUTR;
}

/**
  * @brief  Returns the DOUTR register content.
  * @retval The derivation key.
  * @note   When an unexpected read of DOUTR register is detected, RDERR flag is
  *         set.  
  */
uint8_t AES_ReadSubKey(void)
{
  return AES->DOUTR;
}

/**
  * @}
  */

/** @defgroup AES_Group3 DMA transfers management functions
 *  @brief   DMA transfers management function
 *
@verbatim   
 ===============================================================================
                      DMA transfers management functions
 ===============================================================================  

@endverbatim
  * @{
  */
  
/**
  * @brief  Configures the AES DMA interface.
  * @param  AES_DMATransfer: Specifies the AES DMA transfer.
  *          This parameter can be one of the following values:
  *            @arg AES_DMATransfer_InOut: DMA requests enabled for input/Output transfer phase
  * @param  NewState Indicates the new state of the AES DMA interface.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  * @note   CCF bit has no meaning when DMA requests are enabled (DMAEN = 1).
  */
void AES_DMAConfig(AES_DMATransfer_TypeDef AES_DMATransfer, FunctionalState NewState)
{
  /* Check the parameter */
  assert_param(IS_AES_DMATRANSFER(AES_DMATransfer));

  if (NewState != DISABLE)
  {
    /* Enable the DMA transfer */
    AES->CR |= (uint8_t) AES_DMATransfer;
  }
  else
  {
    /* Disable the DMA transfer */
    AES->CR &= (uint8_t)(~AES_DMATransfer);
  }
}


/**
  * @}
  */

/** @defgroup AES_Group4 Interrupts and flags management functions
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
  * @brief  Enables or disables the specified AES interrupt.
  * @param  AES_IT: Specifies the AES interrupt source to enable/disable.
  *          This parameter can be one of the following values:
  *            @arg AES_IT_CCIE: Computation Complete interrupt enable
  *            @arg AES_IT_ERRIE: Error interrupt enable
  * @param  NewState : The new state of the AES peripheral.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void AES_ITConfig(AES_IT_TypeDef AES_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_AES_IT(AES_IT));

  if (NewState != DISABLE)
  {
    AES->CR |= (uint8_t) AES_IT;    /**< AES_IT Enable */
  }
  else
  {
    AES->CR &= (uint8_t)(~AES_IT);  /**< AES_IT Disable */
  }
}

/**
  * @brief  Checks whether the specified AES flag is set or not.
  * @param  AES_FLAG specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg AES_FLAG_CCF: Computation Complete Flag
  *            @arg AES_FLAG_RDERR: Read Error Flag
  *            @arg AES_FLAG_WRERR: Write Error Flag 
  * @retval FlagStatus (SET or RESET)
  * @note   CCF bit has a meaning only when DMA requests are disabled (DMAEN bit = 0).
  */
FlagStatus AES_GetFlagStatus(AES_FLAG_TypeDef AES_FLAG)
{
  FlagStatus status = RESET;

  /* Check parameters */
  assert_param(IS_AES_FLAG(AES_FLAG));

  if (AES_FLAG == AES_FLAG_CCF)
  {
    if ((AES->SR & (uint8_t)AES_FLAG_CCF) != (uint8_t)0x00)
    {
      /* Computation Complete Flag CCF is set */
      status = (FlagStatus) SET;
    }
    else
    {
      /* Computation Complete Flag CCF is reset */
      status = (FlagStatus) RESET;
    }
  }
  else if (AES_FLAG == AES_FLAG_RDERR)
  {
    if ((AES->SR & (uint8_t)AES_FLAG_RDERR) != (uint8_t)0x00)
    {
      /* Read Error Flag RDERR is set */
      status = (FlagStatus) SET;
    }
    else
    {
      /* Read Error Flag RDERR is reset */
      status = (FlagStatus) RESET;
    }
  }
  else
  {
    if ((AES->SR & (uint8_t)AES_FLAG_WRERR) != (uint8_t)0x00)
    {
      /* Write Error Flag WRERR is set */
      status = (FlagStatus) SET;
    }
    else
    {
      /* Write Error Flag WRERR is reset */
      status = (FlagStatus) RESET;
    }
  }
  /* Return the AES_FLAG status */
  return ((FlagStatus) status);
}

/**
  * @brief  Clears the AES flags.
  * @param  AES_FLAG: specifies the flag to clear.
  *          This parameter can be one of the following values:
  *            @arg AES_FLAG_CCF: Computation Complete Flag
  *            @arg AES_FLAG_RDERR: Read Error Flag
  *            @arg AES_FLAG_WRERR: Write Error Flag 
  * @retval None
  */
void AES_ClearFlag(AES_FLAG_TypeDef AES_FLAG)
{
  /* Check the parameters */
  assert_param(IS_AES_FLAG(AES_FLAG));

  /* Check if AES_FLAG is AES_FLAG_CCF */
  if (AES_FLAG == AES_FLAG_CCF)
  {
    /* Clear CCF flag by setting CCFC bit */
    AES->CR |= (uint8_t) AES_CR_CCFC;
  }
  else /* AES_FLAG is AES_FLAG_RDERR or AES_FLAG_WRERR */
  {
    /* Clear RDERR and WRERR flags by setting ERRC bit */
    AES->CR |= (uint8_t) AES_CR_ERRC;
  }
}

/**
  * @brief  Checks whether the specified AES interrupt has occurred or not.
  * @param  AES_IT: Specifies the AES interrupt pending bit to check.
  *          This parameter can be one of the following values:
  *            @arg AES_IT_CCIE: Computation Complete interrupt enable
  *            @arg AES_IT_ERRIE: Error interrupt enable
  * @retval ITStatus The new state of AES_IT (SET or RESET).
  */
ITStatus AES_GetITStatus(AES_IT_TypeDef AES_IT)
{
  ITStatus itstatus = RESET;
  BitStatus cciebitstatus, ccfbitstatus = RESET;

  /* Check parameters */
  assert_param(IS_AES_IT(AES_IT));

  cciebitstatus = (BitStatus) (AES->CR & AES_CR_CCIE);
  ccfbitstatus =  (BitStatus) (AES->SR & AES_SR_CCF);

  /* Check if AES_IT is AES_IT_CCIE */
  if (AES_IT == AES_IT_CCIE)
  {
    /* Check the status of the specified AES interrupt */
    if (((cciebitstatus) != RESET) && ((ccfbitstatus) != RESET))
    {
      /* Interrupt occurred */
      itstatus = (ITStatus) SET;
    }
    else
    {
      /* Interrupt not occurred */
      itstatus = (ITStatus) RESET;
    }
  }
  else /* AES_IT is AES_IT_ERRIE */
  {
    /* Check the status of the specified AES interrupt */
    if ((AES->CR & AES_CR_ERRIE) != RESET)
    {
      /* Check if WRERR or RDERR flags are set */
      if ((AES->SR & (uint8_t)(AES_SR_WRERR | AES_SR_RDERR)) != RESET)
      {
        /* Interrupt occurred */
        itstatus = (ITStatus) SET;
      }
      else
      {
        /* Interrupt not occurred */
        itstatus = (ITStatus) RESET;
      }
    }
    else
    {
      /* Interrupt not occurred */
      itstatus = (ITStatus) RESET;
    }
  }

  /* Return the AES_IT status */
  return ((ITStatus)itstatus);
}

/**
  * @brief  Clears the AES's interrupt pending bits.
  * @param  AES_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be one of the following values:
  *            @arg AES_IT_CCIE: Computation Complete interrupt enable
  *            @arg AES_IT_ERRIE: Error interrupt enable
  * @retval None
  */
void AES_ClearITPendingBit(AES_IT_TypeDef AES_IT)
{
  /* Check the parameters */
  assert_param(IS_AES_IT(AES_IT));

  /* Check if AES_IT is AES_IT_CCIE */
  if (AES_IT == AES_IT_CCIE)
  {
    /* Clear CCF flag by setting CCFC bit */
    AES->CR |= (uint8_t) AES_CR_CCFC;
  }
  else /* AES_IT is AES_IT_ERRIE */
  {
    /* Clear RDERR and WRERR flags by setting ERRC bit */
    AES->CR |= (uint8_t) AES_CR_ERRC;
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
