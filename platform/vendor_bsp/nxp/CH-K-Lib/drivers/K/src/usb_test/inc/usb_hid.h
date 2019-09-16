#ifndef __USB_HID_H__
#define __USB_HID_H__



#include <stdint.h>
//本构件实现的接口函数
void USB_HID_SetMouse(uint8_t HOffset,uint8_t VOffset,uint8_t SOffset,uint8_t Key);
void HID_Proc(void);
void USB_HID_SendData(uint8_t* buf,uint8_t len);
uint8_t USB_HID_SetKeyBoard(uint8_t FnKey,uint8_t *Keybuf);
uint8_t USB_HID_RecData(uint8_t* buf);
#endif
