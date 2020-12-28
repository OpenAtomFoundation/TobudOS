#ifndef __UART_H
#define __UART_H


#include "stm32l4xx_hal.h"



#define MAX_USBRX_BUFFER_LENGTH 		128
#define CUSB_BUFFER_NUMBER 						6


void Usb_Buff_Init(void);
uint8_t ReadUsbBuffData(uint8_t *dBuffer, uint8_t len);


//uint8_t saveUart2Data(void);




#endif

