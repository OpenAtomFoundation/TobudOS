/**
  ******************************************************************************
  * @file    stm8l15x_itc.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionality of the Interrupt controller (ITC) peripheral:           
  *           - Configuration and management
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
#include "stm8l15x_itc.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @defgroup ITC 
  * @brief ITC driver modules
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup ITC_Private_Functions
  * @{
  */

/**
  * @brief  Utility function used to read CC register.
  * @param  None
  * @retval CPU CC register value
  */
uint8_t ITC_GetCPUCC(void)
{
#ifdef _COSMIC_
  _asm("push cc");
  _asm("pop a");
  return; /* Ignore compiler warning, the returned value is in A register */
#elif defined _RAISONANCE_ /* _RAISONANCE_ */
  return _getCC_();
#else /* _IAR_ */
  asm("push cc");
  asm("pop a"); /* Ignore compiler warning, the returned value is in A register */
#endif /* _COSMIC_*/
}

/** @defgroup ITC_Group1 ITC configuration and management functions
 *  @brief   ITC configuration and management functions
 *
@verbatim   
 ===============================================================================
                  ITC configuration and management functions
 ===============================================================================  

@endverbatim
  * @{
  */
  
/**
  * @brief  Deinitializes the ITC registers to their default reset value.
  * @param  None
  * @retval None
 */
void ITC_DeInit(void)
{
  ITC->ISPR1 = ITC_SPRX_RESET_VALUE;
  ITC->ISPR2 = ITC_SPRX_RESET_VALUE;
  ITC->ISPR3 = ITC_SPRX_RESET_VALUE;
  ITC->ISPR4 = ITC_SPRX_RESET_VALUE;
  ITC->ISPR5 = ITC_SPRX_RESET_VALUE;
  ITC->ISPR6 = ITC_SPRX_RESET_VALUE;
  ITC->ISPR7 = ITC_SPRX_RESET_VALUE;
  ITC->ISPR8 = ITC_SPRX_RESET_VALUE;
}

/**
  * @brief  Gets the interrupt software priority bits (I1, I0) value from CPU CC register.
  * @param  None
  * @retval The interrupt software priority bits value.
 */
uint8_t ITC_GetSoftIntStatus(void)
{
  return ((uint8_t)(ITC_GetCPUCC() & CPU_SOFT_INT_DISABLED));
}

/**
  * @brief  Gets the software priority of the specified interrupt source.
  * @param  IRQn : Specifies the peripheral interrupt source.
  * @retval Specifies the software priority of the interrupt source.
 */
ITC_PriorityLevel_TypeDef ITC_GetSoftwarePriority(IRQn_TypeDef IRQn)
{
  uint8_t Value = 0;
  uint8_t Mask = 0;

  /* Check function parameters */
  assert_param(IS_ITC_IRQ(IRQn));

  /* Define the mask corresponding to the bits position in the SPR register */
  Mask = (uint8_t)(0x03U << ((IRQn % 4U) * 2U));

  switch (IRQn)
  {
    case FLASH_IRQn:
    case DMA1_CHANNEL0_1_IRQn:
    case DMA1_CHANNEL2_3_IRQn:
      Value = (uint8_t)(ITC->ISPR1 & Mask); /* Read software priority */
      break;

    case EXTIE_F_PVD_IRQn:
#if defined (STM8L15X_MD) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
    case RTC_IRQn:
    case EXTIB_IRQn:
    case EXTID_IRQn:
#elif defined (STM8L15X_LD) || defined (STM8L05X_LD_VL)
    case RTC_CSSLSE_IRQn:
    case EXTIB_IRQn:
    case EXTID_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case RTC_CSSLSE_IRQn:
    case EXTIB_G_IRQn:
    case EXTID_H_IRQn:
#endif  /* STM8L15X_MD */
      Value = (uint8_t)(ITC->ISPR2 & Mask); /* Read software priority */
      break;

    case EXTI0_IRQn:
    case EXTI1_IRQn:
    case EXTI2_IRQn:
    case EXTI3_IRQn:
      Value = (uint8_t)(ITC->ISPR3 & Mask); /* Read software priority */
      break;

    case EXTI4_IRQn:
    case EXTI5_IRQn:
    case EXTI6_IRQn:
    case EXTI7_IRQn:
      Value = (uint8_t)(ITC->ISPR4 & Mask); /* Read software priority */
      break;

#if defined (STM8L15X_LD) || defined (STM8L05X_LD_VL)
    case SWITCH_CSS_IRQn:
#else
    case SWITCH_CSS_BREAK_DAC_IRQn:
#endif /* STM8L15X_LD */		
    case ADC1_COMP_IRQn:
#if defined (STM8L15X_MD) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
    case LCD_IRQn:
    case TIM2_UPD_OVF_TRG_BRK_IRQn:
#elif defined (STM8L15X_LD) || defined (STM8L05X_LD_VL)
    case TIM2_UPD_OVF_TRG_BRK_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case LCD_AES_IRQn:
    case TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn:
#endif  /* STM8L15X_MD */
      Value = (uint8_t)(ITC->ISPR5 & Mask); /* Read software priority */
      break;
			
#if !defined (STM8L15X_LD) && !defined (STM8L05X_LD_VL)
    case TIM1_UPD_OVF_TRG_IRQn:
#endif /* STM8L15X_LD */		
#if defined (STM8L15X_MD) || defined (STM8L15X_LD) || defined (STM8L05X_MD_VL) ||\
 defined (STM8AL31_L_MD) || defined (STM8L05X_LD_VL)
    case TIM2_CC_IRQn:
    case TIM3_UPD_OVF_TRG_BRK_IRQn :
    case TIM3_CC_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case TIM2_CC_USART2_RX_IRQn:
    case TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn :
    case TIM3_CC_USART3_RX_IRQn:
#endif  /* STM8L15X_MD */
      Value = (uint8_t)(ITC->ISPR6 & Mask); /* Read software priority */
      break;
			
#if !defined (STM8L15X_LD) && !defined (STM8L05X_LD_VL)
    case TIM1_CC_IRQn:
#endif /* STM8L15X_LD */	
    case TIM4_UPD_OVF_TRG_IRQn:
    case SPI1_IRQn:
#if defined (STM8L15X_MD) || defined (STM8L15X_LD) || defined (STM8L05X_MD_VL) ||\
 defined (STM8AL31_L_MD) || defined (STM8L05X_LD_VL)
    case USART1_TX_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn:
#endif  /* STM8L15X_MD || STM8L15X_LD */
      Value = (uint8_t)(ITC->ISPR7 & Mask); /* Read software priority */
      break;

#if defined (STM8L15X_MD) || defined (STM8L15X_LD) || defined (STM8L05X_MD_VL) ||\
 defined (STM8AL31_L_MD) || defined (STM8L05X_LD_VL)
    case USART1_RX_IRQn:
    case I2C1_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case USART1_RX_TIM5_CC_IRQn:
    case I2C1_SPI2_IRQn:
#endif  /* STM8L15X_MD || STM8L15X_LD*/
      Value = (uint8_t)(ITC->ISPR8 & Mask); /* Read software priority */
      break;

    default:
      break;
  }

  Value >>= (uint8_t)((IRQn % 4u) * 2u);

  return((ITC_PriorityLevel_TypeDef)Value);

}

/**
  * @brief  Sets the software priority of the specified interrupt source.
  * @note   The modification of the software priority is only possible when
  *         the interrupts are disabled.
  * @note   The normal behavior is to disable the interrupt before calling
  *         this function, and re-enable it after.
  * @note   The priority level 0 cannot be set (see product specification
  *         for more details).
  * @param  IRQn : Specifies the peripheral interrupt source.
  * @param  ITC_PriorityLevel : Specifies the software priority value to set
  *          This parameter can be one of the following values:
  *            @arg ITC_PriorityLevel_0: Software priority level 0 (cannot be written)
  *            @arg ITC_PriorityLevel_1: Software priority level 1
  *            @arg ITC_PriorityLevel_2: Software priority level 2
  *            @arg ITC_PriorityLevel_3: Software priority level 3     
  * @retval None
  */
void ITC_SetSoftwarePriority(IRQn_TypeDef IRQn, ITC_PriorityLevel_TypeDef ITC_PriorityLevel)
{
  uint8_t Mask = 0;
  uint8_t NewPriority = 0;

  /* Check function parameters */
  assert_param(IS_ITC_IRQ(IRQn));
  assert_param(IS_ITC_PRIORITY(ITC_PriorityLevel));

  /* Check if interrupts are disabled */
  assert_param(IS_ITC_INTERRUPTS_DISABLED);

  /* Define the mask corresponding to the bits position in the SPR register */
  /* The mask is reversed in order to clear the 2 bits after more easily */
  Mask = (uint8_t)(~(uint8_t)(0x03U << ((IRQn % 4U) * 2U)));
  /* Define the new priority to write */
  NewPriority = (uint8_t)((uint8_t)(ITC_PriorityLevel) << ((IRQn % 4U) * 2U));

  switch (IRQn)
  {
    case FLASH_IRQn:
    case DMA1_CHANNEL0_1_IRQn:
    case DMA1_CHANNEL2_3_IRQn:
      ITC->ISPR1 &= Mask;
      ITC->ISPR1 |= NewPriority;
      break;

    case EXTIE_F_PVD_IRQn:
#if defined (STM8L15X_MD) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
    case RTC_IRQn:
    case EXTIB_IRQn:
    case EXTID_IRQn:
#elif defined (STM8L15X_LD) || defined (STM8L05X_LD_VL)
    case RTC_CSSLSE_IRQn:
    case EXTIB_IRQn:
    case EXTID_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case RTC_CSSLSE_IRQn:
    case EXTIB_G_IRQn:
    case EXTID_H_IRQn:
#endif  /* STM8L15X_MD */
      ITC->ISPR2 &= Mask;
      ITC->ISPR2 |= NewPriority;
      break;

    case EXTI0_IRQn:
    case EXTI1_IRQn:
    case EXTI2_IRQn:
    case EXTI3_IRQn:
      ITC->ISPR3 &= Mask;
      ITC->ISPR3 |= NewPriority;
      break;

    case EXTI4_IRQn:
    case EXTI5_IRQn:
    case EXTI6_IRQn:
    case EXTI7_IRQn:
      ITC->ISPR4 &= Mask;
      ITC->ISPR4 |= NewPriority;
      break;
#if !defined (STM8L15X_LD) && !defined (STM8L05X_LD_VL)
    case SWITCH_CSS_BREAK_DAC_IRQn:
#else
    case SWITCH_CSS_IRQn:
#endif /*	STM8L15X_LD	*/
    case ADC1_COMP_IRQn:
#if defined (STM8L15X_MD) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
    case LCD_IRQn:
    case TIM2_UPD_OVF_TRG_BRK_IRQn:
#elif defined (STM8L15X_LD) || defined (STM8L05X_LD_VL)
    case TIM2_UPD_OVF_TRG_BRK_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case LCD_AES_IRQn:
    case TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn:
#endif  /* STM8L15X_MD */
      ITC->ISPR5 &= Mask;
      ITC->ISPR5 |= NewPriority;
      break;
#if !defined (STM8L15X_LD) && !defined (STM8L05X_LD_VL)
    case TIM1_UPD_OVF_TRG_IRQn:
#endif  /* STM8L15X_LD */
#if defined (STM8L15X_MD) || defined (STM8L15X_LD) || defined (STM8L05X_MD_VL) ||\
 defined (STM8AL31_L_MD) || defined (STM8L05X_LD_VL)
    case TIM2_CC_IRQn:
    case TIM3_UPD_OVF_TRG_BRK_IRQn :
    case TIM3_CC_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case TIM2_CC_USART2_RX_IRQn:
    case TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn :
    case TIM3_CC_USART3_RX_IRQn:
#endif  /* STM8L15X_MD */
      ITC->ISPR6 &= Mask;
      ITC->ISPR6 |= NewPriority;
      break;

#if !defined (STM8L15X_LD) && !defined (STM8L05X_LD_VL)
    case TIM1_CC_IRQn:
#endif  /* STM8L15X_LD */
    case TIM4_UPD_OVF_TRG_IRQn:
    case SPI1_IRQn:
#if defined (STM8L15X_MD) || defined (STM8L15X_LD) || defined (STM8L05X_MD_VL) ||\
 defined (STM8AL31_L_MD) || defined (STM8L05X_LD_VL)
    case USART1_TX_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn:
#endif  /* STM8L15X_MD */
      ITC->ISPR7 &= Mask;
      ITC->ISPR7 |= NewPriority;
      break;

#if defined (STM8L15X_MD) || defined (STM8L15X_LD) || defined (STM8L05X_MD_VL) ||\
 defined (STM8AL31_L_MD) || defined (STM8L05X_LD_VL)
    case USART1_RX_IRQn:
    case I2C1_IRQn:
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
    case USART1_RX_TIM5_CC_IRQn:
    case I2C1_SPI2_IRQn:
#endif  /* STM8L15X_MD */
      ITC->ISPR8 &= Mask;
      ITC->ISPR8 |= NewPriority;
      break;

    default:
      break;
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
