#ifndef __USB_CDC_H__
#define __USB_CDC_H__
#include <stdint.h>

//USB_CDC设备状态
#define WAITING_FOR_ENUMERATION  0x00
//USB setup接口请求参数
#define SET_LINE_CODING          0x20
#define GET_LINE_CODING          0x21
#define SET_CONTROL_LINE_STATE   0x22
#define LOADER_MODE              0xAA
#define GET_INTERFACE            0x0A
#define SERIAL_STATE             0x20

/* Defines */
#define EP_OUT          3
#define EP_IN           2   


#define CDC_INPointer   BufferPointer[bEP02IN_ODD]
#define CDC_OUTPointer  BufferPointer[bEP3OUT_ODD]
//定义CDC直线通信结构体
typedef struct
{
    uint32_t  DTERate;
    uint8_t   CharFormat;
    uint8_t   ParityType;
    uint8_t   Databits;
}CDC_Line_Coding;




//本构件实现的接口函数
void USB_CDC_Init(void);
#endif

