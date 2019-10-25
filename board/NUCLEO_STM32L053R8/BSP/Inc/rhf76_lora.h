#ifndef __RHF76_LORA_H__
#define __RHF76_LORA_H__

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "usart.h"
#define  UART_RXBUF_MAXSIZE           256

typedef enum _class
{
	CLASS_A = 1,
	CLASS_B ,
	CLASS_C ,
}ENUM_CLASS_TypeDef;

typedef enum _mode
{
	LWOTAA = 1,
	LWABP ,
}ENUM_MODE_TypeDef;

typedef enum _id
{
	devaddr = 1,
	deveui ,
	appeui,
}ENUM_ID_TypeDef;

typedef enum _key
{
	appkey = 1,
	appskey ,
	nwkskey,
}ENUM_KEY_TypeDef;


void rhf76_init(void);
int rhf76_send(const void  *buf, uint32_t len);
#endif /* __RHF76_LORA_H__ */

