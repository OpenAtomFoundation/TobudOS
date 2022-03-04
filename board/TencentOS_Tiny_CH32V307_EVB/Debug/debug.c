/********************************** (C) COPYRIGHT  *******************************
* File Name          : debug.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for UART
*                      Printf , Delay functions.
*******************************************************************************/
#include "debug.h"

static uint8_t  p_us=0;
static uint16_t p_ms=0;

/*******************************************************************************
* Function Name  : Delay_Init
* Description    : Initializes Delay Funcation.
* Input          : None
* Return         : None
*******************************************************************************/
void Delay_Init(void)
{
	p_us=SystemCoreClock/8000000;
	p_ms=(uint16_t)p_us*1000;
}

/*******************************************************************************
* Function Name  : Delay_Us
* Description    : Microsecond Delay Time.
* Input          : n£ºMicrosecond number.
* Return         : None
*******************************************************************************/
void Delay_Us(uint32_t n)
{
    uint32_t i;

    SysTick->CTLR = (1<<4);
    i = (uint32_t)n*p_us;

    SysTick->CMP = i;
    SysTick->CTLR |= (1<<5)|(1<<0);

    while((SysTick->SR & (1<<0)) != (1<<0));
    SysTick->SR &= ~(1<<0);
}

/*******************************************************************************
* Function Name  : Delay_Ms
* Description    : Millisecond Delay Time.
* Input          : n£ºMillisecond number.
* Return         : None
*******************************************************************************/
void Delay_Ms(uint32_t n)
{
    uint32_t i;

    SysTick->CTLR = (1<<4);
    i = (uint32_t)n*p_ms;

    SysTick->CMP = i;
    SysTick->CTLR |= (1<<5)|(1<<0);

    while((SysTick->SR & (1<<0)) != (1<<0));
    SysTick->SR &= ~(1<<0);
}

/*******************************************************************************
* Function Name  : USART_Printf_Init
* Description    : Initializes the USARTx peripheral.
* Input          : baudrate: USART communication baud rate.
* Return         : None
*******************************************************************************/
void USART_Printf_Init(uint32_t baudrate)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

#if (DEBUG == DEBUG_UART1)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART2)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART3)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

#endif

  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;

#if (DEBUG == DEBUG_UART1)
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);

#elif (DEBUG == DEBUG_UART2)
  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2, ENABLE);

#elif (DEBUG == DEBUG_UART3)
  USART_Init(USART3, &USART_InitStructure);
  USART_Cmd(USART3, ENABLE);

#endif
}

/*******************************************************************************
* Function Name  : _write
* Description    : Support Printf Function
* Input          : *buf: UART send Data.
*                  size: Data length
* Return         : size: Data length
*******************************************************************************/
int _write(int fd, char *buf, int size)
{
  int i;

  for(i=0; i<size; i++)
  {
#if (DEBUG == DEBUG_UART1)
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, *buf++);
#elif (DEBUG == DEBUG_UART2)
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    USART_SendData(USART2, *buf++);
#elif (DEBUG == DEBUG_UART3)
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    USART_SendData(USART3, *buf++);
#endif
  }

  return size;
}




