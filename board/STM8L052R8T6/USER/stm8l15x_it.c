/**
  ******************************************************************************
  * @file    USART/USART_HyperTerminal_Interrupts/stm8l15x_it.c
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    30-September-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service routine.
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
#include "stm8l15x_it.h"
#include "tos_k.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TX_BUFFER_SIZE (countof(TxBuffer) - 1)

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t TxBuffer[] = "\n\rUSART Example: USART-Hyperterminal communication using Interrupt\nEnter your Text\n\r";
uint8_t TxCounter = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef _COSMIC_
/**
 * @brief Dummy interrupt routine
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(NonHandledInterrupt, 0)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
#endif

/**
 * @brief TRAP interrupt routine
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief FLASH Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(FLASH_IRQHandler, 1)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief DMA1 channel0 and channel1 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler, 2)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief DMA1 channel2 and channel3 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler, 3)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief RTC / CSS_LSE Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler, 4)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief External IT PORTE/F and PVD Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler, 5)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PORTB / PORTG Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTIB_G_IRQHandler, 6)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PORTD /PORTH Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTID_H_IRQHandler, 7)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PIN0 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PIN1 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTI1_IRQHandler, 9)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PIN2 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTI2_IRQHandler, 10)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PIN3 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTI3_IRQHandler, 11)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PIN4 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PIN5 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PIN6 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTI6_IRQHandler, 14)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief External IT PIN7 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(EXTI7_IRQHandler, 15)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief LCD /AES Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(LCD_AES_IRQHandler, 16)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief CLK switch/CSS/TIM1 break Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler, 17)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief ADC1/Comparator Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(ADC1_COMP_IRQHandler, 18)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */

    TIM2_ClearITPendingBit(TIM2_IT_Update);

    if (tos_knl_is_running()) {
        tos_knl_irq_enter();
        tos_tick_handler();
        tos_knl_irq_leave();
    }

#ifdef USE_STM8L1528_EVAL
    /* Write one byte to the transmit data register */
    USART_SendData8(EVAL_COM1, TxBuffer[TxCounter++]);
    USART_ClearITPendingBit(EVAL_COM1, USART_IT_TC);

    if (TxCounter == TX_BUFFER_SIZE)
    {
        /* Disable the USART Transmit Complete interrupt */
        USART_ITConfig(EVAL_COM1, USART_IT_TC, DISABLE);
    }
#endif /* USE_STM8L1528_EVAL */
}

/**
 * @brief Timer2 Capture/Compare / USART2 RX Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
#ifdef USE_STM8L1528_EVAL
    uint8_t temp;

    /* Read one byte from the receive data register and send it back */
    temp = (USART_ReceiveData8(EVAL_COM1) & 0x7F);
    USART_SendData8(EVAL_COM1, temp);
#endif /* USE_STM8L1528_EVAL */
}


/**
 * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler, 21)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief Timer3 Capture/Compare /USART3 RX Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler, 22)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler, 23)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief TIM1 Capture/Compare Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(TIM1_CC_IRQHandler, 24)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}
/**
 * @brief SPI1 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(SPI1_IRQHandler, 26)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/**
 * @brief USART1 TX / TIM5 Update/Overflow/Trigger/Break Interrupt  routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler, 27)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
#ifdef USE_STM8L1526_EVAL
    /* Write one byte to the transmit data register */
    USART_SendData8(EVAL_COM1, TxBuffer[TxCounter++]);
    USART_ClearITPendingBit(EVAL_COM1, USART_IT_TC);

    if (TxCounter == TX_BUFFER_SIZE)
    {
        /* Disable the USART Transmit Complete interrupt */
        USART_ITConfig(EVAL_COM1, USART_IT_TC, DISABLE);
    }
#endif /* USE_STM8L1526_EVAL */
}

/**
 * @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler, 28)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */

#ifdef USE_STM8L1526_EVAL
    uint8_t temp;

    /* Read one byte from the receive data register and send it back */
    temp = (USART_ReceiveData8(EVAL_COM1) & 0x7F);
    USART_SendData8(EVAL_COM1, temp);
#endif /* USE_STM8L1526_EVAL */
}

/**
 * @brief I2C1 / SPI2 Interrupt routine.
 * @param  None
 * @retval None
 */
INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

