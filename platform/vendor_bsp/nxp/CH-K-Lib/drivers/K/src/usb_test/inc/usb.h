#ifndef __USB_H_
#define	__USB_H_
#include "common.h"
#include "message_manage.h"
#include <stdio.h>

#ifdef USB0

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ALIGN
/* Compiler Related Definitions */
#ifdef __CC_ARM                         /* ARM Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #define PRAGMA(x)                   _Pragma(#x)
    #define ALIGN(n)                    PRAGMA(data_alignment=n)
#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#endif /* Compiler Related Definitions */
#endif

#define USB_DEBUG

#ifdef USB_DEBUG

/* Turn on some of these (set to non-zero) to debug kernel */
#ifndef USB_DEBUG_MIN
#define USB_DEBUG_MIN                   1
#endif

#ifndef USB_DEBUG_EP0
#define USB_DEBUG_EP0                   1
#endif

#ifndef USB_DEBUG_DESCRIPTOR
#define USB_DEBUG_DESCRIPTOR            1
#endif




#define USB_DEBUG_LOG(type, message)                                            \
do                                                                              \
{                                                                               \
    if (type)                                                                   \
        printf message;                                                         \
}                                                                               \
while (0)

#define USB_ASSERT(EX)                                                      \
if (!(EX))                                                                  \
{                                                                           \
    volatile char dummy = 0;                                                \
    printf("(%s) assert failed at %s:%d \n", #EX, __FUNCTION__, __LINE__);  \
    while (dummy == 0);                                                     \
}

    
#else
#define USB_ASSERT(EX)
#define USB_DEBUG_LOG(type, message)
#endif


#define USB_DEVICE_CLASS USB_DEVICE_CLASS_HID




//DBT由MCU控制
#define kMCU      0x00
//DBT由SIE(USB)模块控制
#define kSIE      0x80

//BDT缓冲区DATA0格式
#define kUDATA0   0x88
//BDT缓冲区DATA1格式
#define kUDATA1   0xC8

//USB事务中，包的PID类型
#define mSETUP_TOKEN    0x0D
#define mOUT_TOKEN      0x01
#define mIN_TOKEN       0x09


/* Standard requests */
#define GET_STATUS        (0)
#define CLEAR_FEATURE     (1)
#define SET_FEATURE       (3)
#define SET_ADDRESS       (5)
#define GET_DESCRIPTOR    (6)
#define SET_DESCRIPTOR    (7)
#define GET_CONFIGURATION (8)
#define SET_CONFIGURATION (9)
#define GET_INTERFACE     (10)
#define SET_INTERFACE     (11)

/* bmRequestType.dataTransferDirection */
#define HOST_TO_DEVICE (0)
#define DEVICE_TO_HOST (1)

/* bmRequestType.Type*/
#define STANDARD_TYPE  (0)
#define CLASS_TYPE     (1)
#define VENDOR_TYPE    (2)
#define RESERVED_TYPE  (3)

/* bmRequestType.Recipient */
#define DEVICE_RECIPIENT    (0)
#define INTERFACE_RECIPIENT (1)
#define ENDPOINT_RECIPIENT  (2)
#define OTHER_RECIPIENT     (3)




#define USB_DEVICE_CLASS_AUDIO        1
#define USB_DEVICE_CLASS_CDC          2
#define USB_DEVICE_CLASS_HID          3
#define USB_DEVICE_CLASS_PHY          4
#define USB_DEVICE_CLASS_IMAGE        5
#define USB_DEVICE_CLASS_MASS_STORAGE 6
#define USB_DEVICE_CLASS_HUB          7
#define USB_DEVICE_CLASS_CDC_DATA     8
#define USB_DEVICE_CLASS_SMARTCARD    9




#define NUMBER_OF_LOGICAL_ENDPOINTS (16)
#define NUMBER_OF_PHYSICAL_ENDPOINTS (NUMBER_OF_LOGICAL_ENDPOINTS * 2)

/* Define physical endpoint numbers */

/*      Endpoint    No.   */
/*      ----------------  */
#define EP0OUT      (0)
#define EP0IN       (1)
#define EP1OUT      (2)
#define EP1IN       (3)
#define EP2OUT      (4)
#define EP2IN       (5)
#define EP3OUT      (6)
#define EP3IN       (7)
#define EP4OUT      (8)
#define EP4IN       (9)
#define EP5OUT      (10)
#define EP5IN       (11)
#define EP6OUT      (12)
#define EP6IN       (13)
#define EP7OUT      (14)
#define EP7IN       (15)
#define EP8OUT      (16)
#define EP8IN       (17)
#define EP9OUT      (18)
#define EP9IN       (19)
#define EP10OUT     (20)
#define EP10IN      (21)
#define EP11OUT     (22)
#define EP11IN      (23)
#define EP12OUT     (24)
#define EP12IN      (25)
#define EP13OUT     (26)
#define EP13IN      (27)
#define EP14OUT     (28)
#define EP14IN      (29)
#define EP15OUT     (30)
#define EP15IN      (31)

#define MAX_PACKET_SIZE_EP0  (64)
#define MAX_PACKET_SIZE_EP1  (64)
#define MAX_PACKET_SIZE_EP2  (64)
#define MAX_PACKET_SIZE_EP3  (1023)
#define MAX_PACKET_SIZE_EP4  (64)
#define MAX_PACKET_SIZE_EP5  (64)
#define MAX_PACKET_SIZE_EP6  (64)
#define MAX_PACKET_SIZE_EP7  (64)
#define MAX_PACKET_SIZE_EP8  (64)
#define MAX_PACKET_SIZE_EP9  (64)
#define MAX_PACKET_SIZE_EP10 (64)
#define MAX_PACKET_SIZE_EP11 (64)
#define MAX_PACKET_SIZE_EP12 (64)
#define MAX_PACKET_SIZE_EP13 (64)
#define MAX_PACKET_SIZE_EP14 (64)
#define MAX_PACKET_SIZE_EP15 (64)

enum
{
    EP0,
    EP1,
    EP2,
    EP3,
    EP4,
    EP5,
    DUMMY,
    LOADER
};

typedef enum {ATTACHED, POWERED, DEFAULT, ADDRESS, CONFIGURED} DEVICE_STATE;

typedef struct {
    struct {
        uint8_t dataTransferDirection;
        uint8_t Type;
        uint8_t Recipient;
    } bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} SETUP_PACKET;

typedef struct {
    SETUP_PACKET setup;
    uint8_t *ptr;
    uint32_t remaining;
    uint8_t direction;
    bool zlp;
    bool notify;
} CONTROL_TRANSFER;


typedef struct
{
    uint8_t (*ClassSetup)(SETUP_PACKET *packet);
    uint8_t (*ClassData)(uint8_t event, void* args);
    uint8_t (*AppCallback)(uint8_t event, void* args);
    SETUP_PACKET setup_pkt;
    volatile DEVICE_STATE state;
    uint8_t configuration;
    bool suspended;
} USB_DEVICE;

enum
{
    kUSB_IN,
    kUSB_OUT
};

enum
{
    bEP0OUT_ODD,
    bEP0OUT_EVEN,
    bEP0IN_ODD,
    bEP0IN_EVEN,
    bEP1OUT_ODD,
    bEP1OUT_EVEN,
    bEP1IN_ODD,
    bEP1IN_EVEN,
    bEP2OUT_ODD,
    bEP2OUT_EVEN,
    bEP2IN_ODD,
    bEP2IN_EVEN,
    bEP3OUT_ODD,
    bEP3OUT_EVEN,
    bEP3IN_ODD,
    bEP3IN_EVEN
};

typedef union _tBDT_STAT
{
    uint8_t info;
	//发送的MCU控制字段
    struct{
        uint8_t :1;
        uint8_t :1;
        uint8_t BSTALL:1;              //OTG模块发出一个握手协议
        uint8_t DTS:1;                 //
        uint8_t NINC:1;                //访问数据缓冲区时，DMA引擎不会增加它的地址
        uint8_t KEEP:1;                //
        uint8_t DATA:1;                //发送或接受了DATA0/DATA1包，USB模块不改变该位
        uint8_t UOWN:1;                //BDT所有权 UOWN=1 USB模块拥有，UOWN=0 微处理器拥有
    }McuCtlBit;
    //接受控制字段 
    struct{
        uint8_t    :2;
        uint8_t PID:4;                 //包标志
        uint8_t    :2;
    }RecPid;
} tBDT_STAT,*ptBDT_STAT;                            //缓冲区描述符表结构体


typedef struct _tBDT
{
    tBDT_STAT Stat;         // BD[0:7]
    uint8_t  dummy;         // RSVD: BD[8:15]
    uint16_t byte_count;    // BD[16:32]
    uint32_t address;       // Addr
} tBDT,*ptBDT;


void USBD_EPWrite(uint8_t ep, uint8_t *buf, uint8_t len);
void USBD_EPRead(uint8_t ep, uint8_t *buf, uint32_t *bytesRead);

uint16_t USB_EP_OUT_SizeCheck(uint8_t ep);
void USB_EnableInterface(void);
uint8_t USB_Init(void);
uint8_t* USBD_GetStdDesc(SETUP_PACKET *packet, uint32_t *len);


#endif

#endif
