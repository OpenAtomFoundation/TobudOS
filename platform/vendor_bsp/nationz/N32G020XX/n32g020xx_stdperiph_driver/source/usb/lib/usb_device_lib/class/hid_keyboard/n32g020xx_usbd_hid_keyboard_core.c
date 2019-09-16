/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32g020xx_usbd_hid_keyboard_core.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */




/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"
#include "n32g020xx_usbd_desc.h"

#include "n32g020xx_usbd_hid_keyboard_core.h"

#ifndef LOG_ENABLE
#include <stdio.h>
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/ 
static uint8_t  USBD_HID_Init (void  *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_HID_DeInit (void  *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_HID_Setup (void  *pdev, 
                                USB_SETUP_REQ *req);

static uint8_t  *USBD_HID_GetCfgDesc (uint8_t speed, uint16_t *length);

USBD_Class_cb_TypeDef  USBD_HID_cb = 
{
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  NULL, /*DataIn*/
  NULL, /*DataOut*/
  NULL, /*SOF */    
  USBD_HID_GetCfgDesc, 
};
      
static uint32_t  USBD_HID_AltSet = 0;
    
static uint32_t  USBD_HID_Protocol = 0;
 
static uint32_t  USBD_HID_IdleState  = 0;

/* USB HID device Configuration Descriptor */
const uint8_t USBD_HID_CfgDesc[USB_HID_KEYBOARD_CONFIG_DESC_SIZ] =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_HID_KEYBOARD_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xC0,//0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  
  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x01,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x21,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  KEYBOARD_SIZ_REPORT_DESC,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /* 27 */
  /******************** Descriptor of keyboard endpoint (IN) ********************/
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
  HID_IN_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_IN_PACKET, /*wMaxPacketSize: 8 Byte max */
  0x00,
  0x0A,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */
  /******************** Descriptor of keyboard endpoint (OUT) ********************/
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
  HID_OUT_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_OUT_PACKET, /*wMaxPacketSize: 8 Byte max */
  0x00,
  0x0A,          /*bInterval: Polling Interval (10 ms)*/
  /* 41 */
} ;

/* HID的报告描述符*/

/*定义了8字节发送：

**  第一字节表示特殊件是否按下：D0:Ctrl D1:Shift D2:Alt

**  第二字节保留，值为0

**  第三~第八字节:普通键键值的数组,最多能同时按下6个键

**定义了1字节接收:对应键盘上的LED灯,这里只用了两个位。

**     D0:Num Lock   D1:Cap Lock   D2:Scroll Lock   D3:Compose   D4:Kana*/

const uint8_t HID_KEYBOARD_ReportDesc[KEYBOARD_SIZ_REPORT_DESC] =

{     

       /*short Item   D7~D4:bTag;D3~D2:bType;D1~D0:bSize

       **bTag ---主条目      1000:输入(Input) 1001:输出(Output) 1011:特性(Feature) 1010:集合(Collection) 1100:关集合(End Collection)

       **              全局条目     0000:用途页(Usage Page) 0001:逻辑最小值(Logical Minimum) 0010:逻辑最大值(Logical Maximum) 0011:物理最小值(Physical Minimum)

       **                                 0100:物理最大值(Physical Maximum) 0101:单元指数(Unit Exponet) 0110:单元(Unit) 0111:数据域大小(Report Size)

       **                                 1000:报告ID(Report ID) 1001:数据域数量(Report Count) 1010:压栈(Push) 1011:出栈(Pop) 1100~1111:保留(Reserved)

       **              局部条目     0000:用途(Usage) 0001:用途最小值(Usage Minimum) 0010:用途最大值(Usage Maximum) 0011:标识符索引(Designator Index)

       **                                 0100:标识符最小值(Designator Minimum) 0101:标识符最大值(Designator Maximum) 0111:字符串索引(String Index) 1000:字符串最小值(String Minimum)  

       **                                 1001:字符串最大值(String Maximum) 1010:分隔符(Delimiter) 其他：保留(Reserved)

       **bType---00:主条目(main)  01:全局条目(globle)  10:局部条目(local)  11:保留(reserved)

       **bSize---00:0字节  01:1字节  10:2字节  11:4字节*/

   
       //0x05:0000 01 01 这是个全局条目，用途页选择为普通桌面页

       0x05, 0x01, // USAGE_PAGE (Generic Desktop)

       //0x09:0000 10 01 这是个全局条目，用途选择为键盘

       0x09, 0x06, // USAGE (Keyboard)

       //0xa1:1010 00 01 这是个主条目，选择为应用集合，

       0xa1, 0x01, // COLLECTION (Application)

       //0x05:0000 01 11 这是个全局条目，用途页选择为键盘/按键

       0x05, 0x07, // USAGE_PAGE (Keyboard/Keypad)

 

       //0x19:0001 10 01 这是个局部条目，用途的最小值为0xe0，对应键盘上的左ctrl键

       0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)

       //0x29:0010 10 01 这是个局部条目，用途的最大值为0xe7，对应键盘上的有GUI(WIN)键

       0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)

       //0x15:0001 01 01 这是个全局条目，说明数据的逻辑值最小值为0

       0x15, 0x00, // LOGICAL_MINIMUM (0)

       //0x25:0010 01 01 这是个全局条目，说明数据的逻辑值最大值为1

       0x25, 0x01, // LOGICAL_MAXIMUM (1)

 

       //0x95:1001 01 01 这是个全局条目，数据域的数量为8个

       0x95, 0x08, // REPORT_COUNT (8)

       //0x75:0111 01 01 这是个全局条目，每个数据域的长度为1位

       0x75, 0x01, // REPORT_SIZE (1)       

       //0x81:1000 00 01 这是个主条目，有8*1bit数据域作为输入，属性为:Data,Var,Abs

       0x81, 0x02, // INPUT (Data,Var,Abs)

 

       //0x95:1001 01 01 这是个全局条目,数据域的数量为1个

       0x95, 0x01, // REPORT_COUNT (1)

       //0x75:0111 01 01 这是个全局条目，每个数据域的长度为8位

       0x75, 0x08, // REPORT_SIZE (8)

       //0x81:1000 00 01 这是个主条目，有1*8bit数据域作为输入，属性为:Cnst,Var,Abs

       0x81, 0x03, // INPUT (Cnst,Var,Abs)

 

       //0x95:1001 01 01 这是个全局条目，数据域的数量为6个

       0x95, 0x06, // REPORT_COUNT (6)

       //0x75:0111 01 01 这是个全局条目，每个数据域的长度为8位

       0x75, 0x08, // REPORT_SIZE (8)

       //0x25:0010 01 01 这是个全局条目，逻辑最大值为255

       0x25, 0xFF, // LOGICAL_MAXIMUM (255)

       //0x19:0001 10 01 这是个局部条目，用途的最小值为0

       0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))

       //0x29:0010 10 01 这是个局部条目，用途的最大值为0x65

       0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)

       //0x81:1000 00 01 这是个主条目，有6*8bit的数据域作为输入，属相为属性为:Data,Var,Abs

       0x81, 0x00, // INPUT (Data,Ary,Abs)

 

       //0x25:0010 01 01 这是个全局条目，逻辑的最大值为1

       0x25, 0x01, // LOGICAL_MAXIMUM (1)

       //0x95:1001 01 01 这是个全局条目，数据域的数量为2

       0x95, 0x02, // REPORT_COUNT (2)

       //0x75:0111 01 01 这是个全局条目，每个数据域的长度为1位

       0x75, 0x01, // REPORT_SIZE (1)

       //0x05:0000 01 01 这是个全局条目，用途页选择为LED页

       0x05, 0x08, // USAGE_PAGE (LEDs)

       //0x19:0001 10 01 这是个局部条目，用途的最小值为0x01,对应键盘上的Num Lock

       0x19, 0x01, // USAGE_MINIMUM (Num Lock)

       //0x29:0010 10 01 这是个局部条目，用途的最大值为0x02,对应键盘上的Caps Lock

       0x29, 0x02, // USAGE_MAXIMUM (Caps Lock)

       //0x91:1001 00 01 这是个主条目，有2*1bit的数据域作为输出，属性为:Data,Var,Abs

       0x91, 0x02, // OUTPUT (Data,Var,Abs)

 

       //0x95:1001 01 01 这是个全局条目，数据域的数量为1个

       0x95, 0x01, // REPORT_COUNT (1)

       //0x75:0111 01 01 这是个全局条目，每个数据域的长度为6bit,正好与前面的2bit组成1字节

       0x75, 0x06, // REPORT_SIZE (6)

       //0x91:1001 00 01 这是个主条目，有1*6bit数据域最为输出，属性为:Cnst,Var,Abs

       0x91, 0x03, // OUTPUT (Cnst,Var,Abs)

       0xc0        // END_COLLECTION

};


/* Private function ----------------------------------------------------------*/ 
/**
  * @brief  USBD_HID_Init
  *         Initialize the HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_Init (void  *pdev, 
                               uint8_t cfgidx)
{ 
  /* Open EP IN */
  DCD_EP_Open(pdev,
              HID_IN_EP,
              HID_IN_PACKET,
              USB_EP_INT);
  
  /* Open EP OUT */
  DCD_EP_Open(pdev,
              HID_OUT_EP,
              HID_OUT_PACKET,
              USB_EP_INT);
  
  return USBD_OK;
}

/**
  * @brief  USBD_HID_Init
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_DeInit (void  *pdev, 
                                 uint8_t cfgidx)
{
  /* Close HID EPs */
  DCD_EP_Close (pdev , HID_IN_EP);
  DCD_EP_Close (pdev , HID_OUT_EP);
  
  
  return USBD_OK;
}

/**
  * @brief  USBD_HID_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_HID_Setup (void  *pdev, 
                                USB_SETUP_REQ *req)
{
  uint16_t len = 0;
  uint8_t  *pbuf = NULL;
  
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (req->bRequest)
    {
    case HID_REQ_SET_PROTOCOL:
      USBD_HID_Protocol = (uint8_t)(req->wValue);
      break;
      
    case HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&USBD_HID_Protocol,
                        1,
                        USB_IN_BUFFER_WAIT);    
      break;
      
    case HID_REQ_SET_IDLE:
      USBD_HID_IdleState = (uint8_t)(req->wValue >> 8);
      break;
      
    case HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&USBD_HID_IdleState,
                        1,
                        USB_IN_BUFFER_WAIT);        
      break;      
      
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
      if( req->wValue >> 8 == HID_REPORT_DESC)
      {
        len = MIN(KEYBOARD_SIZ_REPORT_DESC , req->wLength);
        pbuf = (uint8_t *)HID_KEYBOARD_ReportDesc;
      }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        pbuf = (uint8_t *)USBD_HID_CfgDesc + 0x12;
        len = MIN(USB_HID_DESC_SIZ , req->wLength);
      }
      
      USBD_CtlSendData (pdev, 
                        pbuf,
                        len,
                        USB_IN_BUFFER_WAIT);
      
      break;
      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&USBD_HID_AltSet,
                        1,
                        USB_IN_BUFFER_WAIT);
      break;
      
    case USB_REQ_SET_INTERFACE :
      USBD_HID_AltSet = (uint8_t)(req->wValue);
      break;
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_HID_SendReport 
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_SendReport     (USB_CORE_HANDLE  *pdev, 
                                 uint8_t *report,
                                 uint16_t len)
{
  /* Check if USB is configured */
  if (pdev->dev.device_status == USB_CONFIGURED )
  {
    DCD_EP_Tx (pdev, HID_IN_EP, report, len, USB_IN_BUFFER_WAIT);
  }
  return USBD_OK;
}

/**
  * @brief  USBD_HID_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_HID_GetCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (USBD_HID_CfgDesc);
  return (uint8_t *)USBD_HID_CfgDesc;
}
