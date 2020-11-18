
#include <string.h>
#include <stdlib.h>

#include "user_usb_uart.h"
#include "usb_host.h"
#include "usbh_cdc.h"

uint8_t da_flag =0;

uint8_t aUsbRxBuffer[MAX_USBRX_BUFFER_LENGTH];
uint8_t bUsbRxBuffer[CUSB_BUFFER_NUMBER][MAX_USBRX_BUFFER_LENGTH+1];
uint8_t bUsbBufLength[CUSB_BUFFER_NUMBER];

uint8_t CurrentUsbBufNumPoint = 0;

extern USBH_HandleTypeDef hUsbHostFS;
extern uint8_t Recv_buf[MAX_USBRX_BUFFER_LENGTH];


void Usb_Buff_Init(void)
{
	for(int ti=0; ti< CUSB_BUFFER_NUMBER; ti++)
	{
		bUsbBufLength[ti] = 0;
		memset(bUsbRxBuffer[ti],0,sizeof(bUsbRxBuffer[ti]));
	}
}

uint8_t ReadUsbBuffData(uint8_t *dBuffer, uint8_t len)
{
	uint8_t buflen =0;
	if(bUsbBufLength[CurrentUsbBufNumPoint]>0)
	{
	  memset(dBuffer,0,len);
		buflen = bUsbBufLength[CurrentUsbBufNumPoint];
		memcpy(dBuffer,(const char *)bUsbRxBuffer[CurrentUsbBufNumPoint],buflen);
		memset(bUsbRxBuffer[CurrentUsbBufNumPoint],0,sizeof(bUsbRxBuffer[CurrentUsbBufNumPoint]));
		bUsbBufLength[CurrentUsbBufNumPoint] = 0;
		CurrentUsbBufNumPoint++;
		if(CurrentUsbBufNumPoint >= CUSB_BUFFER_NUMBER)
				CurrentUsbBufNumPoint = 0;
	}	
	return buflen;
}
//
void Check_empty_buffer()
{
   uint8_t i; 
	for(i=0;i<CUSB_BUFFER_NUMBER;i++)
	{
		if(bUsbBufLength[CurrentUsbBufNumPoint]>0)
		{
			CurrentUsbBufNumPoint++;
			if(CurrentUsbBufNumPoint >= CUSB_BUFFER_NUMBER)
				CurrentUsbBufNumPoint=0;
		}
		else
			break;		
	}
}
//
void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef *phost)
{
	uint32_t i,rec_len=0;
	USBH_CDC_GetLastReceivedDataSize(phost);
	rec_len = strlen(aUsbRxBuffer);
	if(rec_len>0)
	{				
		Check_empty_buffer();
		memcpy(bUsbRxBuffer[CurrentUsbBufNumPoint],aUsbRxBuffer,rec_len);
		bUsbBufLength[CurrentUsbBufNumPoint] = rec_len;		
		memset(aUsbRxBuffer,0,sizeof(aUsbRxBuffer));
	}
}


