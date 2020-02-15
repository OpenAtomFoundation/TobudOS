/**
  ******************************************************************************
  * @file    stm8l15x_exti.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the EXTI peripheral:
  *            - Interrupt sensitivity of GPIO ports/pins configuration
  *            - Interrupt status management
  *  @verbatim
  *  
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *          This driver provides functions to configure and initialize the EXTI 
  *          peripheral
  *          These functions are split in 2 groups: 
  *   
  *          1. EXTI configuration: this group includes all needed functions 
  *             to configure the EXTI GPIO ports and pins:
  *                   - Set GPIO pins sensitivity
  *                   - Select GPIO port, GPIO half port and set the GPIO port sensitivity
  *                         
  *          2. EXTI interrupt status management
  *                   - Get the interrupt status: set/reset
  *                   - Clear interrupt pending bits
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
#include "stm8l15x_exti.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup EXTI 
  * @brief EXTI driver modules
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup EXTI_Private_Functions
  * @{
  */

/** @defgroup EXTI_Group1 Interrupt sensitivity of GPIO ports/pins configuration
 *  @brief   Interrupt sensitivity of GPIO ports/pins configuration
 *
@verbatim   
 ===============================================================================
                         EXTI configuration functions
 ===============================================================================  
  
       ===================================================================      
                        EXTI Driver: how to configure EXTI
       =================================================================== 
       To use a GPIO pin as an interrupt source, follow theses steps:
       
         1. Configure the GPIO pin in input mode with interrupt enabled using
            GPIO_Init()
          
         2. Configure the GPIO pin sensitivity (falling, rising...) using 
            EXTI_SetPinSensitivity()
          
         3. Enable global interrupts using enableInterrupts()
       
         4. In the IRQ handler corresponding to the GPIO pin, clear the interrupt
            pending bit using EXTI_ClearITPendingBit()

       To use a GPIO port as an interrupt source, follows theses steps:  
            
         1. Configure the GPIO pins of the same port in input mode with interrupt
            enabled using GPIO_Init()
          
         2. Configure the GPIO port sensitivity (falling, rising...) using 
            EXTI_SetPortSensitivity()
          
         3. Select the GPIO port and the corresponding half port using  
            EXTI_SelectPort() then EXTI_SetHalfPortSelection()
        
         4. Enable global interrupts using enableInterrupts()
       
         5. In the IRQ handler corresponding to the GPIO port, clear the interrupt
            pending bit using EXTI_ClearITPendingBit()

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the EXTI registers to their default reset value.
  * @param  None
  * @retval None
  */
void EXTI_DeInit(void)
{
  EXTI->CR1 = EXTI_CR1_RESET_VALUE;
  EXTI->CR2 = EXTI_CR2_RESET_VALUE;
  EXTI->CR3 = EXTI_CR3_RESET_VALUE;
  EXTI->CR4 = EXTI_CR4_RESET_VALUE;
  EXTI->SR1 = 0xFF; /* Setting SR1 bits in order to clear flags */
  EXTI->SR2 = 0xFF; /* Setting SR2 bits in order to clear flags */
  EXTI->CONF1 = EXTI_CONF1_RESET_VALUE;
  EXTI->CONF2 = EXTI_CONF2_RESET_VALUE;
}

/**
  * @brief  Sets the external interrupt sensitivity of the selected pin.
  * @note   Global interrupts must be disabled before calling this function.
  * @note   The modification of external interrupt sensitivity is only possible
  *         when he interrupts are disabled.
  * @note   The normal behavior is to disable the interrupts before calling this
  *         function, and re-enable them after.
  * @param  EXTI_Pin : The pin to configure.
  *          This parameter can be one of the following values:
  *            @arg EXTI_Pin_0: GPIO Pin 0
  *            @arg EXTI_Pin_1: GPIO Pin 1
  *            @arg EXTI_Pin_2: GPIO Pin 2
  *            @arg EXTI_Pin_3: GPIO Pin 3
  *            @arg EXTI_Pin_4: GPIO Pin 4
  *            @arg EXTI_Pin_5: GPIO Pin 5
  *            @arg EXTI_Pin_6: GPIO Pin 6
  *            @arg EXTI_Pin_7: GPIO Pin 7              
  * @param  EXTI_Trigger : The external interrupt sensitivity value to set.
  *          This parameter can be one of the following values:
  *            @arg EXTI_Trigger_Falling_Low: Interrupt on Falling edge and Low level
  *            @arg EXTI_Trigger_Rising: Interrupt on Rising edge only  
  *            @arg EXTI_Trigger_Falling: Interrupt on Falling edge only 
  *            @arg EXTI_Trigger_Rising_Falling: Interrupt on Rising and Falling edges       
  * @retval None
  */
void EXTI_SetPinSensitivity(EXTI_Pin_TypeDef EXTI_Pin, EXTI_Trigger_TypeDef EXTI_Trigger)
{

  /* Check function parameters */
  assert_param(IS_EXTI_PINNUM(EXTI_Pin));
  assert_param(IS_EXTI_TRIGGER(EXTI_Trigger));

  /* Clear port sensitivity bits */
  switch (EXTI_Pin)
  {
    case EXTI_Pin_0:
      EXTI->CR1 &=  (uint8_t)(~EXTI_CR1_P0IS);
      EXTI->CR1 |= (uint8_t)((uint8_t)(EXTI_Trigger) << EXTI_Pin);
      break;
    case EXTI_Pin_1:
      EXTI->CR1 &=  (uint8_t)(~EXTI_CR1_P1IS);
      EXTI->CR1 |= (uint8_t)((uint8_t)(EXTI_Trigger) << EXTI_Pin);
      break;
    case EXTI_Pin_2:
      EXTI->CR1 &=  (uint8_t)(~EXTI_CR1_P2IS);
      EXTI->CR1 |= (uint8_t)((uint8_t)(EXTI_Trigger) << EXTI_Pin);
      break;
    case EXTI_Pin_3:
      EXTI->CR1 &=  (uint8_t)(~EXTI_CR1_P3IS);
      EXTI->CR1 |= (uint8_t)((uint8_t)(EXTI_Trigger) << EXTI_Pin);
      break;
    case EXTI_Pin_4:
      EXTI->CR2 &=  (uint8_t)(~EXTI_CR2_P4IS);
      EXTI->CR2 |= (uint8_t)((uint8_t)(EXTI_Trigger) << ((uint8_t)EXTI_Pin & (uint8_t)0xEF));
      break;
    case EXTI_Pin_5:
      EXTI->CR2 &=  (uint8_t)(~EXTI_CR2_P5IS);
      EXTI->CR2 |= (uint8_t)((uint8_t)(EXTI_Trigger) << ((uint8_t)EXTI_Pin & (uint8_t)0xEF));
      break;
    case EXTI_Pin_6:
      EXTI->CR2 &=  (uint8_t)(~EXTI_CR2_P6IS);
      EXTI->CR2 |= (uint8_t)((uint8_t)(EXTI_Trigger) << ((uint8_t)EXTI_Pin & (uint8_t)0xEF));
      break;
    case EXTI_Pin_7:
      EXTI->CR2 &=  (uint8_t)(~EXTI_CR2_P7IS);
      EXTI->CR2 |= (uint8_t)((uint8_t)(EXTI_Trigger) << ((uint8_t)EXTI_Pin & (uint8_t)0xEF));
      break;
    default:
      break;
  }
}

/**
  * @brief  Selects the port interrupt selection.
  * @param  EXTI_Port : The port number to access.
  *          This parameter can be one of the following values:
  *            @arg EXTI_Port_B: GPIO Port B
  *            @arg EXTI_Port_D: GPIO Port D
  *            @arg EXTI_Port_E: GPIO Port E
  *            @arg EXTI_Port_F: GPIO Port F
  *            @arg EXTI_Port_G: GPIO Port G
  *            @arg EXTI_Port_H: GPIO Port H   
  * @retval None
  */
void EXTI_SelectPort(EXTI_Port_TypeDef EXTI_Port)
{
  /* Check function parameter */
  assert_param(IS_EXTI_PORT(EXTI_Port));

  if (EXTI_Port == EXTI_Port_B)
  {
    /* Select Port B by resetting PGBS bit in CONF2 register */
    EXTI->CONF2 &= (uint8_t) (~EXTI_CONF2_PGBS);
  }
  else if (EXTI_Port == EXTI_Port_D)
  {
    /* Select Port D by resetting PHDS bit in CONF2 register */
    EXTI->CONF2 &= (uint8_t) (~EXTI_CONF2_PHDS);
  }
  else if (EXTI_Port == EXTI_Port_E)
  {
    /* Select Port E by resetting PFES bit in CONF1 register */
    EXTI->CONF1 &= (uint8_t) (~EXTI_CONF1_PFES);
  }
  else if (EXTI_Port == EXTI_Port_F)
  {
    /* Select Port F by setting PFES bit in CONF1 register */
    EXTI->CONF1 |= (uint8_t) (EXTI_CONF1_PFES);
  }
  else if (EXTI_Port == EXTI_Port_G)
  {
    /* Select Port G by setting PGBS bit in CONF2 register */
    EXTI->CONF2 |= (uint8_t) (EXTI_CONF2_PGBS);
  }
  else /* EXTI_Port is EXTI_Port_H */
  {
    /* Select Port H by setting PHDS bit in CONF2 register */
    EXTI->CONF2 |= (uint8_t) (EXTI_CONF2_PHDS);
  }
}

/**
  * @brief  Configures the half port interrupt selection.
  * @note   This function should be called once the port sensitivity configured,
  *         otherwise it will not have any effect on the port external interrupt.
  * @note   This function should be called after EXTI_SelectPort() function which
  *         selects the port to be used otherwise ports are selected by default
  * @param  EXTI_HalfPort : The port part to access (MSB or LSB).
  *          This parameter can be one of the following values:
  *            @arg EXTI_HalfPort_B_LSB: Interrupt selector PB(3:0)
  *            @arg EXTI_HalfPort_B_MSB: Interrupt selector PB(7:4)
  *            @arg EXTI_HalfPort_D_LSB: Interrupt selector PD(3:0)
  *            @arg EXTI_HalfPort_D_MSB: Interrupt selector PD(7:4)
  *            @arg EXTI_HalfPort_E_LSB: Interrupt selector PE(3:0)
  *            @arg EXTI_HalfPort_E_MSB: Interrupt selector PE(7:4)
  *            @arg EXTI_HalfPort_F_LSB: Interrupt selector PF(3:0)
  *            @arg EXTI_HalfPort_F_MSB: Interrupt selector PF(7:4)
  *            @arg EXTI_HalfPort_G_LSB: Interrupt selector PG(3:0)
  *            @arg EXTI_HalfPort_G_MSB: Interrupt selector PG(7:4)
  *            @arg EXTI_HalfPort_H_LSB: Interrupt selector PH(3:0)
  *            @arg EXTI_HalfPort_H_MSB: Interrupt selector PH(7:4)                      
  * @param  NewState : The external interrupt new state.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void EXTI_SetHalfPortSelection(EXTI_HalfPort_TypeDef EXTI_HalfPort,
                               FunctionalState NewState)
{
  /* Check function parameters */
  assert_param(IS_EXTI_HALFPORT(EXTI_HalfPort));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if ((EXTI_HalfPort & 0x80) == 0x00)
  {
    if (NewState != DISABLE)
    {
      /* Enable port interrupt selector */
      EXTI->CONF1 |= (uint8_t)EXTI_HalfPort;
    }
    else /*NewState == DISABLE */
    {
      /* Disable port interrupt selector */
      EXTI->CONF1 &= (uint8_t)(~(uint8_t)EXTI_HalfPort);
    }
  }
  else
  {
    if (NewState != DISABLE)
    {
      /* Enable port interrupt selector */
      EXTI->CONF2 |= (uint8_t)(EXTI_HalfPort & (uint8_t)0x7F);
    }
    else /*NewState == DISABLE */
    {
      /* Disable port interrupt selector */
      EXTI->CONF2 &= (uint8_t)(~(uint8_t) (EXTI_HalfPort & (uint8_t)0x7F));
    }
  }
}

/**
  * @brief  Sets the external interrupt sensitivity of the selected port.
  * @note   Global interrupts must be disabled before calling this function.
  * @note   The modification of external interrupt sensitivity is only possible
  *         when the interrupts are disabled.
  * @note   The normal behavior is to disable the interrupts before calling this
  *         function, and re-enable them after.
  * @param  EXTI_Port : The port number to access.
  *          This parameter can be one of the following values:
  *            @arg EXTI_Port_B: GPIO Port B
  *            @arg EXTI_Port_D: GPIO Port D
  *            @arg EXTI_Port_E: GPIO Port E
  *            @arg EXTI_Port_F: GPIO Port F
  *            @arg EXTI_Port_G: GPIO Port G
  *            @arg EXTI_Port_H: GPIO Port H  
  * @param  EXTI_Trigger : The external interrupt sensitivity value to set.
  *          This parameter can be one of the following values:
  *            @arg EXTI_Trigger_Falling_Low: Interrupt on Falling edge and Low level
  *            @arg EXTI_Trigger_Rising: Interrupt on Rising edge only  
  *            @arg EXTI_Trigger_Falling: Interrupt on Falling edge only 
  *            @arg EXTI_Trigger_Rising_Falling: Interrupt on Rising and Falling edges 
  * @retval None
  */
void EXTI_SetPortSensitivity(EXTI_Port_TypeDef EXTI_Port,
                             EXTI_Trigger_TypeDef EXTI_Trigger)
{
  /* Check function parameters */
  assert_param(IS_EXTI_PORT(EXTI_Port));
  assert_param(IS_EXTI_TRIGGER(EXTI_Trigger));

  /* Ceck if selected port is in EXTI_CR3 register */
  if ((EXTI_Port & 0xF0) == 0x00)
  {
    /* Reset the trigger bits corresponding to EXTI_Port */
    EXTI->CR3 &= (uint8_t) (~(uint8_t)((uint8_t)0x03 << EXTI_Port));
    /* Write EXTI port trigger */
    EXTI->CR3 |= (uint8_t)((uint8_t)(EXTI_Trigger) << EXTI_Port);
  }
  else /* selected port is in EXTI_CR4 register */
  {
    /* Reset the trigger bits corresponding to EXTI_Port */
    EXTI->CR4 &= (uint8_t) (~(uint8_t)((uint8_t)0x03 << (EXTI_Port & 0x0F)));
    /* Write EXTI port trigger */
    EXTI->CR4 |= (uint8_t)(EXTI_Trigger << (EXTI_Port & 0x0F));
  }
}

/**
  * @brief  Gets the external interrupt sensitivity of the selected pin.
  * @param  EXTI_Pin : The pin number to access.
  *          This parameter can be one of the following values:
  *            @arg EXTI_Pin_0: GPIO Pin 0
  *            @arg EXTI_Pin_1: GPIO Pin 1
  *            @arg EXTI_Pin_2: GPIO Pin 2
  *            @arg EXTI_Pin_3: GPIO Pin 3
  *            @arg EXTI_Pin_4: GPIO Pin 4
  *            @arg EXTI_Pin_5: GPIO Pin 5
  *            @arg EXTI_Pin_6: GPIO Pin 6
  *            @arg EXTI_Pin_7: GPIO Pin 7 
  * @retval The external interrupt sensitivity of the selected port.
  */
EXTI_Trigger_TypeDef EXTI_GetPinSensitivity(EXTI_Pin_TypeDef EXTI_Pin)
{
  uint8_t value = 0;

  /* Check function parameters */
  assert_param(IS_EXTI_PINNUM(EXTI_Pin));

  switch (EXTI_Pin)
  {
    case EXTI_Pin_0:
      value = (uint8_t)(EXTI->CR1 & EXTI_CR1_P0IS);
      break;
    case EXTI_Pin_1:
      value = (uint8_t)((uint8_t)(EXTI->CR1 & EXTI_CR1_P1IS) >> EXTI_Pin_1);
      break;
    case EXTI_Pin_2:
      value = (uint8_t)((uint8_t)(EXTI->CR1 & EXTI_CR1_P2IS) >> EXTI_Pin_2);
      break;
    case EXTI_Pin_3:
      value = (uint8_t)((uint8_t)(EXTI->CR1 & EXTI_CR1_P3IS) >> EXTI_Pin_3);
      break;
    case EXTI_Pin_4:
      value = (uint8_t)(EXTI->CR2 & EXTI_CR2_P4IS);
      break;
    case EXTI_Pin_5:
      value = (uint8_t)((uint8_t)(EXTI->CR2 & EXTI_CR2_P5IS) >> ((uint8_t)EXTI_Pin_5 & (uint8_t)0x0F));
      break;
    case EXTI_Pin_6:
      value = (uint8_t)((uint8_t)(EXTI->CR2 & EXTI_CR2_P6IS) >> ((uint8_t)EXTI_Pin_6 & (uint8_t)0x0F));
      break;
    case EXTI_Pin_7:
      value = (uint8_t)((uint8_t)(EXTI->CR2 & EXTI_CR2_P7IS) >> ((uint8_t)EXTI_Pin_7 & (uint8_t)0x0F));
      break;
    default:
      break;
  }
  return((EXTI_Trigger_TypeDef)value);
}

/**
  * @brief  Gets the external interrupt sensitivity of the selected port.
  * @param  EXTI_Port : The port number to access.
  *          This parameter can be one of the following values:
  *            @arg EXTI_Port_B: GPIO Port B
  *            @arg EXTI_Port_D: GPIO Port D
  *            @arg EXTI_Port_E: GPIO Port E
  *            @arg EXTI_Port_F: GPIO Port F
  *            @arg EXTI_Port_G: GPIO Port G
  *            @arg EXTI_Port_H: GPIO Port H  
  * @retval The external interrupt sensitivity of the selected port.
  */
EXTI_Trigger_TypeDef EXTI_GetPortSensitivity(EXTI_Port_TypeDef EXTI_Port)
{
  uint8_t portsensitivity = 0;

  /* Check function parameters */
  assert_param(IS_EXTI_PORT(EXTI_Port));

  /* Check if selected port is in EXTI_CR3 */
  if ((EXTI_Port & 0xF0) == 0x00)
  {
    /* Get port sensitivity */
    portsensitivity = (uint8_t)((uint8_t)0x03 & (uint8_t)(EXTI->CR3 >> EXTI_Port));
  }
  /* selected port is in EXTI_CR4 */
  else
  {
    /* Get port sensitivity */
    portsensitivity = (uint8_t)((uint8_t)0x03 & (uint8_t)(EXTI->CR4 >> (EXTI_Port & 0x0F)));
  }

  return((EXTI_Trigger_TypeDef)portsensitivity);
}

/**
  * @}
  */

/** @defgroup EXTI_Group2 EXTI Interrupt status management functions
 *  @brief    EXTI Interrupt status management functions
 *
@verbatim   
 ===============================================================================
                   EXTI Interrupt status management functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Gets the external interrupt status.
  * @param  EXTI_IT : Specifies the interrupt to read.
  *          This parameter can be one of the following values:
  *            @arg EXTI_IT_Pin0: GPIO Pin 0
  *            @arg EXTI_IT_Pin1: GPIO Pin 1
  *            @arg EXTI_IT_Pin2: GPIO Pin 2
  *            @arg EXTI_IT_Pin3: GPIO Pin 3
  *            @arg EXTI_IT_Pin4: GPIO Pin 4
  *            @arg EXTI_IT_Pin5: GPIO Pin 5
  *            @arg EXTI_IT_Pin6: GPIO Pin 6
  *            @arg EXTI_IT_Pin7: GPIO Pin 7 
  *            @arg EXTI_IT_PortB: GPIO Port B
  *            @arg EXTI_IT_PortD: GPIO Port D
  *            @arg EXTI_IT_PortE: GPIO Port E
  *            @arg EXTI_IT_PortF: GPIO Port F
  *            @arg EXTI_IT_PortG: GPIO Port G
  *            @arg EXTI_IT_PortH: GPIO Port H           
  * @retval The status of the specified interrupt.
  *         This parameter can be a SET or RESET
  */
ITStatus EXTI_GetITStatus(EXTI_IT_TypeDef EXTI_IT)
{
  ITStatus status = RESET;
  /* Check function parameters */
  assert_param(IS_EXTI_ITPENDINGBIT(EXTI_IT));

  if (((uint16_t)EXTI_IT & (uint16_t)0xFF00) == 0x0100)
  {
    status = (ITStatus)(EXTI->SR2 & (uint8_t)((uint16_t)EXTI_IT & (uint16_t)0x00FF));
  }
  else
  {
    status = (ITStatus)(EXTI->SR1 & ((uint8_t)((uint16_t)EXTI_IT & (uint16_t)0x00FF)));
  }
  return((ITStatus)status);
}

/**
  * @brief  Clears the specified interrupt pending bit
  * @param  EXTI_IT : Specifies the interrupt to clear
  *          This parameter can be one of the following values:
  *            @arg EXTI_IT_Pin0: GPIO Pin 0
  *            @arg EXTI_IT_Pin1: GPIO Pin 1
  *            @arg EXTI_IT_Pin2: GPIO Pin 2
  *            @arg EXTI_IT_Pin3: GPIO Pin 3
  *            @arg EXTI_IT_Pin4: GPIO Pin 4
  *            @arg EXTI_IT_Pin5: GPIO Pin 5
  *            @arg EXTI_IT_Pin6: GPIO Pin 6
  *            @arg EXTI_IT_Pin7: GPIO Pin 7 
  *            @arg EXTI_IT_PortB: GPIO Port B
  *            @arg EXTI_IT_PortD: GPIO Port D
  *            @arg EXTI_IT_PortE: GPIO Port E
  *            @arg EXTI_IT_PortF: GPIO Port F
  *            @arg EXTI_IT_PortG: GPIO Port G
  *            @arg EXTI_IT_PortH: GPIO Port H  
  * @retval None
  */
void EXTI_ClearITPendingBit(EXTI_IT_TypeDef EXTI_IT)
{
  uint16_t tempvalue = 0;

  /* Check function parameters */
  assert_param(IS_EXTI_ITPENDINGBIT(EXTI_IT));

  tempvalue = ((uint16_t)EXTI_IT & (uint16_t)0xFF00);

  if ( tempvalue == 0x0100)
  {
    EXTI->SR2 = (uint8_t)((uint16_t)EXTI_IT & (uint16_t)0x00FF);
  }
  else
  {
    EXTI->SR1 = (uint8_t) (EXTI_IT);
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
