/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/**
  * @brief 板级 usart 硬件初始化
  * @param void:
  * retval N/A.
  */
int board_usart_init(void)
{
    rcu_periph_clock_enable(RCU_USART0);
    usart_deinit(RCU_USART0);
    usart_baudrate_set(USART0, 115200);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    usart_enable(USART0);
    nvic_irq_enable(USART0_IRQn, 0, 0);

    return 0;
}
