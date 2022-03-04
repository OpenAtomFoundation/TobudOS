/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main Interrupt Service Routines.
*******************************************************************************/
#include "ch32v30x_it.h"
#include "tos_k.h"
#include "tos_at.h"

extern at_agent_t esp8266_tf_agent;

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
    printf("hardfault\r\n");
    printf("mepc   = %08x\r\n",__get_MEPC());
    printf("mcause = %08x\r\n",__get_MCAUSE());
    printf("mtval  = %08x\r\n",__get_MTVAL());
    while (1)
    {

    }
}

/*********************************************************************
 * @fn      USART2_IRQHandler
 *
 * @brief   This function handles USART2 global interrupt request.
 *
 * @return  none
 */
void USART2_IRQHandler(void)
{
  uint8_t data;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
      data= USART_ReceiveData(USART2);
      tos_at_uart_input_byte(&esp8266_tf_agent,data);
  }

}


/*********************************************************************
 * @fn      USART2_IRQHandler
 *
 * @brief   This function handles USART2 global interrupt request.
 *
 * @return  none
 */
void UART6_IRQHandler(void)
{
    uint8_t data;
  if(USART_GetITStatus(UART6, USART_IT_RXNE) != RESET)
  {
      data= USART_ReceiveData(UART6);
      tos_at_uart_input_byte(&esp8266_tf_agent,data);

  }

}
/*********************************************************************
 * @fn      USART2_IRQHandler
 *
 * @brief   This function handles USART2 global interrupt request.
 *
 * @return  none
 */
void UART7_IRQHandler(void)
{
    uint8_t data;
  if(USART_GetITStatus(UART7, USART_IT_RXNE) != RESET)
  {
      data= USART_ReceiveData(UART7);
      tos_at_uart_input_byte(&esp8266_tf_agent,data);
  }

}
