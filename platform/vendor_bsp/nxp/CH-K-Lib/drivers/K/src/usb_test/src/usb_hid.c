#include "usb_hid.h"
#include "usb.h"
#include "string.h"




uint8_t USB_HID_RecFlag = 0;
uint8_t USB_HID_SendBuffer[32];
uint8_t USB_HID_SendLen = 0;
static  MessageType_t* pMsg;


extern USB_DEVICE device;


uint8_t USBD_HID_ClassSetup(SETUP_PACKET *packet)
{
    printf("USBD_HID_ClassCallback\r\n");
    return 1;
}

void USBD_HID_Init(void)
{
    device.ClassSetup = USBD_HID_ClassSetup;
}


void USB_HID_SendData(uint8_t* buf,uint8_t len)
{
	memcpy(USB_HID_SendBuffer,buf,len);
	USB_HID_SendLen = len;
}



void USB_HID_SetMouse(uint8_t HOffset,uint8_t VOffset,uint8_t SOffset,uint8_t Key)
{
	uint8_t buf[4];
	buf[1] = HOffset;
	buf[2] = VOffset;
	buf[3] = SOffset;
	buf[0] = Key;
	USB_HID_SendData(buf,4);
}


uint8_t USB_HID_SetKeyBoard(uint8_t FnKey,uint8_t *Keybuf)
{
	uint8_t i;
	if(Keybuf == NULL) return 1;
	USB_HID_SendBuffer[0] = FnKey;
	USB_HID_SendLen = 8;
	for(i=2;i<8;i++) 
	{
		USB_HID_SendBuffer[i] = Keybuf[i-2]; 
	}
}

void HID_Proc(void)
{
	uint8_t i;
	pMsg = NULL;
	if(fn_msg_exist())
	{
			pMsg = fn_msg_pop();
			if(pMsg->m_Command == USB_DEVICE_CLASS_HID)
			{
				if(pMsg->m_MessageType == kUSB_IN) //需要发送数据
				{
					USBD_EPWrite(EP2, USB_HID_SendBuffer, USB_HID_SendLen);
					memset(USB_HID_SendBuffer, 0, USB_HID_SendLen); 
				}
				else if(pMsg->m_MessageType == kUSB_OUT)
				{
					 USB_HID_RecFlag = 1;
				}
			}
	}
}


uint8_t USB_HID_RecData(uint8_t* buf)
{
	if(USB_HID_RecFlag == 1)
	{
		memcpy(buf,pMsg->pMessage,pMsg->m_MsgLen); //拷贝数据
		USB_HID_RecFlag = 0;
		return pMsg->m_MsgLen;
	}
	else return 0;
}
