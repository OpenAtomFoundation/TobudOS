/**
  ******************************************************************************
  * @file    stm8l15x_gpio.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the GPIO peripheral:
  *           - Initialization and Configuration
  *           - GPIO Read and Write
  *
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================       
  *           1. Configure the GPIO pin(s) using GPIO_Init()
  *              Two main configuration are available for each pin:
  *                - Input: Floating 
  *                         Pull-up.
  *                  In Input mode, external interrupt can be enabled or disabled
  *                - Output: Push-Pull
  *                          Open Drain.
  *                  In output mode, the GPIO pin speed is configurable: 
  *                  Slow (2 MHz) or Fast (10MHz).
  *  
  *           2. To get the level of a pin configured in input mode use GPIO_ReadInputDataBit()
  * 
  *           3. To set/reset GPIO pins sharing the same GPIO port use
  *              GPIO_SetBits() / GPIO_ResetBits()
  *
  *           4. To enable external interrupt, the GPIO pin must be configured
  *              in input mode with interrupt enabled. Interrupt sensitivity
  *              (rising, falling...) is configurable using 
  *              EXTI_SetPinSensitivity() in the EXTI peripheral driver "stm8l15x_exti.c"
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
#include "stm8l15x_gpio.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup CLK 
  * @brief CLK driver modules
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup GPIO_Private_Functions
  * @{
  */ 


/** @defgroup GPIO_Group1 Initialization and Configuration
 *  @brief   Initialization and Configuration
 *
@verbatim   
 ===============================================================================
                        Initialization and Configuration
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the GPIOx peripheral registers to their default reset values.
  * @param  GPIOx: Select the GPIO peripheral number (x = A to I).
  * @retval None
  */
void GPIO_DeInit(GPIO_TypeDef* GPIOx)
{
  GPIOx->CR2 = GPIO_CR2_RESET_VALUE; /* Reset Control Register 2 */
  GPIOx->ODR = GPIO_ODR_RESET_VALUE; /* Reset Output Data Register */
  GPIOx->DDR = GPIO_DDR_RESET_VALUE; /* Reset Data Direction Register */
  GPIOx->CR1 = GPIO_CR1_RESET_VALUE; /* Reset Control Register 1 */
}

/**
  * @brief  Initializes the GPIOx according to the specified parameters.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @param  GPIO_Pin : This parameter contains the pin number
  *           This parameter can be one of the following values:
  *            @arg GPIO_Pin_0: Pin 0
  *            @arg GPIO_Pin_1: Pin 1
  *            @arg GPIO_Pin_2: Pin 2
  *            @arg GPIO_Pin_3: Pin 3
  *            @arg GPIO_Pin_4: Pin 4
  *            @arg GPIO_Pin_5: Pin 5
  *            @arg GPIO_Pin_6: Pin 6
  *            @arg GPIO_Pin_7: Pin 7              
  * @param  GPIO_Mode : This parameter can be a value of the
  *           This parameter can be one of the following values:
  *            @arg GPIO_Mode_In_FL_No_IT: Input floating, no external interrupt
  *            @arg GPIO_Mode_In_PU_No_IT: Input pull-up, no external interrupt
  *            @arg GPIO_Mode_In_FL_IT: Input pull-up, external interrupt
  *            @arg GPIO_Mode_Out_OD_Low_Fast: Output open-drain, low level, 10MHz
  *            @arg GPIO_Mode_Out_PP_Low_Fast: Output push-pull, low level, 10MHz
  *            @arg GPIO_Mode_Out_OD_Low_Slow: Output open-drain, low level, 2MHz
  *            @arg GPIO_Mode_Out_PP_Low_Slow: Output push-pull, low level, 2MHz
  *            @arg GPIO_Mode_Out_OD_HiZ_Fast: Output open-drain, high-impedance level, 10MHz
  *            @arg GPIO_Mode_Out_PP_High_Fast: Output push-pull, high level, 10MHz
  *            @arg GPIO_Mode_Out_OD_HiZ_Slow: Output open-drain, high-impedance level, 2MHz
  *            @arg GPIO_Mode_Out_PP_High_Slow: Output push-pull, high level, 2MHz
  * @retval None
  */

void GPIO_Init(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, GPIO_Mode_TypeDef GPIO_Mode)
{
  /*----------------------*/
  /* Check the parameters */
  /*----------------------*/

  assert_param(IS_GPIO_MODE(GPIO_Mode));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* Reset corresponding bit to GPIO_Pin in CR2 register */
  GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));

  /*-----------------------------*/
  /* Input/Output mode selection */
  /*-----------------------------*/

  if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x80) != (uint8_t)0x00) /* Output mode */
  {
    if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x10) != (uint8_t)0x00) /* High level */
    {
      GPIOx->ODR |= GPIO_Pin;
    } else /* Low level */
    {
      GPIOx->ODR &= (uint8_t)(~(GPIO_Pin));
    }
    /* Set Output mode */
    GPIOx->DDR |= GPIO_Pin;
  } else /* Input mode */
  {
    /* Set Input mode */
    GPIOx->DDR &= (uint8_t)(~(GPIO_Pin));
  }

  /*------------------------------------------------------------------------*/
  /* Pull-Up/Float (Input) or Push-Pull/Open-Drain (Output) modes selection */
  /*------------------------------------------------------------------------*/

  if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x40) != (uint8_t)0x00) /* Pull-Up or Push-Pull */
  {
    GPIOx->CR1 |= GPIO_Pin;
  } else /* Float or Open-Drain */
  {
    GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
  }

  /*-----------------------------------------------------*/
  /* Interrupt (Input) or Slope (Output) modes selection */
  /*-----------------------------------------------------*/

  if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x20) != (uint8_t)0x00) /* Interrupt or Slow slope */
  {
    GPIOx->CR2 |= GPIO_Pin;
  } else /* No external interrupt or No slope control */
  {
    GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));
  }

}

/**
  * @brief  Configures the external pull-up on GPIOx pins.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @param  GPIO_Pin : Specifies the pin number
  *           This parameter can be one of the following values:
  *            @arg GPIO_Pin_0: Pin 0
  *            @arg GPIO_Pin_1: Pin 1
  *            @arg GPIO_Pin_2: Pin 2
  *            @arg GPIO_Pin_3: Pin 3
  *            @arg GPIO_Pin_4: Pin 4
  *            @arg GPIO_Pin_5: Pin 5
  *            @arg GPIO_Pin_6: Pin 6
  *            @arg GPIO_Pin_7: Pin 7     
  * @param  NewState : The new state of the pull up pin.
  *           Can be ENABLE or DISABLE  
  * @retval None
  */
void GPIO_ExternalPullUpConfig(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE) /* External Pull-Up Set*/
  {
    GPIOx->CR1 |= GPIO_Pin;
  } else /* External Pull-Up Reset*/
  {
    GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
  }
}

/**
  * @}
  */

/** @defgroup GPIO_Group2 GPIO Read and Write
 *  @brief   GPIO Read and Write
 *
@verbatim   
 ===============================================================================
                              GPIO Read and Write
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Writes data to the specified GPIO data port.
  * @note   The port must be configured in output mode.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @param  GPIO_PortVal : Specifies the value to be written to the port output
  *         data register.
  * @retval None
  */
void GPIO_Write(GPIO_TypeDef* GPIOx, uint8_t GPIO_PortVal)
{
  GPIOx->ODR = GPIO_PortVal;
}

/**
  * @brief  Sets or clears the selected data port bit.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @param  GPIO_Pin: Specifies the port bit to be written.
  *           This parameter can be one of the following values:
  *            @arg GPIO_Pin_0: Pin 0
  *            @arg GPIO_Pin_1: Pin 1
  *            @arg GPIO_Pin_2: Pin 2
  *            @arg GPIO_Pin_3: Pin 3
  *            @arg GPIO_Pin_4: Pin 4
  *            @arg GPIO_Pin_5: Pin 5
  *            @arg GPIO_Pin_6: Pin 6
  *            @arg GPIO_Pin_7: Pin 7   
  * @param  GPIO_BitVal: specifies the desired status to be written.
  *         This parameter can be SET or RESET
  * @retval None
  */
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin, BitAction GPIO_BitVal)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_STATE_VALUE(GPIO_BitVal));

  if (GPIO_BitVal != RESET)
  {
    GPIOx->ODR |= GPIO_Pin;

  }
  else
  {
    GPIOx->ODR &= (uint8_t)(~GPIO_Pin);
  }
}

/**
  * @brief  Writes high level to the specified GPIO pins.
  * @note   The port must be configured in output mode.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @param  GPIO_Pin : Specifies the pins to be turned high.
  *           This parameter can be one of the following values:
  *            @arg GPIO_Pin_0: Pin 0
  *            @arg GPIO_Pin_1: Pin 1
  *            @arg GPIO_Pin_2: Pin 2
  *            @arg GPIO_Pin_3: Pin 3
  *            @arg GPIO_Pin_4: Pin 4
  *            @arg GPIO_Pin_5: Pin 5
  *            @arg GPIO_Pin_6: Pin 6
  *            @arg GPIO_Pin_7: Pin 7   
  * @retval None
  */
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin)
{
  GPIOx->ODR |= GPIO_Pin;
}

/**
  * @brief  Writes low level to the specified GPIO pins.
  * @note   The port must be configured in output mode.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @param  GPIO_Pin : Specifies the pins to be turned low
  *           This parameter can be one of the following values:
  *            @arg GPIO_Pin_0: Pin 0
  *            @arg GPIO_Pin_1: Pin 1
  *            @arg GPIO_Pin_2: Pin 2
  *            @arg GPIO_Pin_3: Pin 3
  *            @arg GPIO_Pin_4: Pin 4
  *            @arg GPIO_Pin_5: Pin 5
  *            @arg GPIO_Pin_6: Pin 6
  *            @arg GPIO_Pin_7: Pin 7 
  * @retval None
  */
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin)
{
  GPIOx->ODR &= (uint8_t)(~GPIO_Pin);
}

/**
  * @brief  Toggles the specified GPIO pins.
  * @note   The port must be configured in output mode.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @param  GPIO_Pin : Specifies the pins to be toggled.
  * @retval None
  */
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin)
{
  GPIOx->ODR ^= GPIO_Pin;
}

/**
  * @brief  Reads the specified GPIO input data port.
  * @note   The port must be configured in input mode.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @retval The GPIOx input data port value.
  */
uint8_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
{
  return ((uint8_t)GPIOx->IDR);
}

/**
  * @brief  Reads the specified GPIO output data port.
  * @note   The port must be configured in input mode.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @retval The GPIOx  output data port value.
  */
uint8_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx)
{
  return ((uint8_t)GPIOx->ODR);
}

/**
  * @brief  Reads the specified GPIO input data pin.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @param  GPIO_Pin : Specifies the pin number.
  *           This parameter can be one of the following values:
  *            @arg GPIO_Pin_0: Pin 0
  *            @arg GPIO_Pin_1: Pin 1
  *            @arg GPIO_Pin_2: Pin 2
  *            @arg GPIO_Pin_3: Pin 3
  *            @arg GPIO_Pin_4: Pin 4
  *            @arg GPIO_Pin_5: Pin 5
  *            @arg GPIO_Pin_6: Pin 6
  *            @arg GPIO_Pin_7: Pin 7 
  * @retval BitStatus : GPIO input pin status.
  */
BitStatus GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
{
  return ((BitStatus)(GPIOx->IDR & (uint8_t)GPIO_Pin));
}

/**
  * @brief  Reads the specified GPIO Output data pin.
  * @param  GPIOx : Select the GPIO peripheral number (x = A to I).
  * @param  GPIO_Pin : Specifies the pin number
  * @retval BitStatus : GPIO output pin status.
  */
BitStatus GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
{
  return ((BitStatus)(GPIOx->ODR & (uint8_t)GPIO_Pin));
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
