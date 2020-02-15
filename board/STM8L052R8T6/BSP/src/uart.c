#include "stm8l15x.h"
#include "uart.h"

#include "stdio.h"

const uint8_t HEX_TABLE[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int putchar(int c) {
    if('\n' == (char)c)  {
        USART_SendData8(USART1, '\r');
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
    USART_SendData8(USART1, c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    return (c);
}

void UART1_Send_Byte(uint8_t ucData)
{
	USART_SendData8(USART1, ucData);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0)
		;
}

//send string
void UART1_Send_String(char *Str)
{

	while (*Str != '\0') // "\0" meaning is the end of a string
	{
		UART1_Send_Byte(*Str);
		Str++;
	}
}

void UART1_Send_Dec(unsigned int num, unsigned char ucNumCount)
{
	char disp_buffer1[2];
	char disp_buffer2[3];
	char disp_buffer3[4];
	char disp_buffer4[5];
	char disp_buffer5[6];
	char disp_buffer6[7];

	switch (ucNumCount)
	{
	case 1:

		disp_buffer1[0] = HEX_TABLE[num % 10];
		disp_buffer1[1] = 0;

		UART1_Send_String(disp_buffer1);
		break;

	case 2:
		disp_buffer2[0] = HEX_TABLE[(num % 100) / 10];
		disp_buffer2[1] = HEX_TABLE[num % 10];
		disp_buffer2[2] = 0;
		UART1_Send_String(disp_buffer2);
		break;
	case 3:
		disp_buffer3[0] = HEX_TABLE[num / 100];
		disp_buffer3[1] = HEX_TABLE[(num % 100) / 10];
		disp_buffer3[2] = HEX_TABLE[num % 10];
		disp_buffer3[3] = 0;

		UART1_Send_String(disp_buffer3);
		break;

	case 4:
		disp_buffer4[0] = HEX_TABLE[(num % 10000) / 1000];
		disp_buffer4[1] = HEX_TABLE[(num % 1000) / 100];
		disp_buffer4[2] = HEX_TABLE[(num % 100) / 10];
		disp_buffer4[3] = HEX_TABLE[num % 10];
		disp_buffer4[4] = 0;

		UART1_Send_String(disp_buffer4);
		break;

	case 5:

		disp_buffer5[0] = HEX_TABLE[(num % 100000) / 10000];
		disp_buffer5[1] = HEX_TABLE[(num % 10000) / 1000];
		disp_buffer5[2] = HEX_TABLE[(num % 1000) / 100];
		disp_buffer5[3] = HEX_TABLE[(num % 100) / 10];
		disp_buffer5[4] = HEX_TABLE[num % 10];
		disp_buffer5[5] = 0;

		UART1_Send_String(disp_buffer5);
		break;

	case 6:

		disp_buffer6[0] = HEX_TABLE[num / 100000];
		disp_buffer6[1] = HEX_TABLE[(num % 100000) / 10000];
		disp_buffer6[2] = HEX_TABLE[(num % 10000) / 1000];
		disp_buffer6[3] = HEX_TABLE[(num % 1000) / 100];
		disp_buffer6[4] = HEX_TABLE[(num % 100) / 10];
		disp_buffer6[5] = HEX_TABLE[num % 10];
		disp_buffer6[6] = 0;

		UART1_Send_String(disp_buffer6);
		break;

	default:

		break;
	}
}

void UART1_Init(uint32_t uiBaudRate)
{
    // INIT UART1 PINS
    GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_High_Fast);
    GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT);
    GPIO_SetBits(GPIOC, GPIO_Pin_3);

    // enable UART1 Clock
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);

    // setting the UART1
    USART_Init(USART1, uiBaudRate, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No,
                (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));

    // enable RX interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // enable UART1
    USART_Cmd(USART1, ENABLE);
}
