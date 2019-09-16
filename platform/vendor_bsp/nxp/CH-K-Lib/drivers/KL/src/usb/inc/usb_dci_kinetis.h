/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 ******************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************//*!
 *
 * @file usb_dci_kinetis.h
 *
 * @author
 *
 * @version
 *
 * @date
 *
 * @brief The file contains Macro's and functions needed by the DCI layer.
 *
 *****************************************************************************/

#ifndef _USB_DCI_H
#define _USB_DCI_H
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "types.h"
#include "derivative.h"
#include "usb_devapi.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define BYTES_1024              (1024)
#define BYTES_512               (512)
#define ENDPT_EP_STALL_MASK     (0x02)
#define FRAME_HIGH_BYTE_SHIFT    (8)

#ifdef USB_LOWPOWERMODE
typedef enum _stopmode
{
    STOP_MODE1 = 1, /* STOP MODE 1 */
    STOP_MODE2 = 2, /* STOP MODE 2 */
    STOP_MODE3 = 3, /* STOP MODE 3 */
    STOP_MODE4 = 4  /* STOP MODE 4 */
}STOP_MODE;
#endif

/* USBTRC0 Initialization Parameters */
#define _USBPHYEN           (0x01)       /* Use internal transceiver */
#define _USBPUEN            (0x40)       /* Use internal pull-up resistor */
#define _USBREGEN           (0x04)       /* Use the internal regulator */
#define _USBRESET           (0x80)

#define UCFG_VAL            (_USBPUEN|_USBREGEN)

#define CTL_RESET_VAL (0)             /* value programmed to the CTL
                                         register in RESET */

#define EP_CTRL     (0x0C)            /* Cfg Control pipe for this endpoint */
// HIGH_SPEED_DEVICE
#if !HIGH_SPEED_DEVICE
#define EP_OUT      (0x08)            /* Cfg OUT only pipe for this endpoint*/
#define EP_IN       (0x04)            /* Cfg IN only pipe for this endpoint */
#endif
#define HSHK_EN     (0x01)            /* Enable handshake packet            */
                                      /* Handshake should be disable for
                                         isochorous transfer                */
#define EP_CTL_DIS  (0x10)

#define EP_DISABLE  (0)

#define TRANSFER_INDEX(x)  (x>>1)

#define MAX_EP_BUFFER_SIZE  USB_MAX_EP_BUFFER_SIZE /*Max Endpoint Buffer Size*/

/* Macro's to check whether corresponding INT_STAT bit is set */
#define BUS_RESET_FLAG(x)               ((x) & 1)
#define ERROR_FLAG(x)                   ((x) & 2)
#define SOF_TOKEN_FLAG(x)               ((x) & 4)
#define SLEEP_FLAG(x)                   ((x) & 0x10)
#define RESUME_FLAG(x)                  ((x) & 0x20)
#define STALL_FLAG(x)                   ((x) & 0x80)
#define TOKEN_COMPL_FLAG(x)             ((x) & 8)

/* Setup the controller for Remote Wakeup */
#define USB_DCI_WAKEUP				\
{									\
	USB0->ISTAT |= USB_ISTAT_RESUME_MASK; \
	USB0->INTEN &= ~USB_INTEN_RESUMEEN_MASK; \
	USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;	\
}

/* control endpoint transfer types */
#define USB_TRF_UNKNOWN      			(0xFF)


#define BDT_MIN_BUFFER_SIZE             (16)  /* space occupied by smallest
                                                           buffer in BDT */

#define BDT_MIN_BUFFER_ADDR_INC         (4)   /* min offset increment
                                                 correspoding to min buffer
                                                 size */

#define BDT_OFFSET_SHIFT                (4)   /* bdt offset shift */

#define INVALID_BDT_INDEX               (0xff)/* invalid bdt index */

#define ENDPOINT_NUMBER_SHIFT           (4)   /* endpoint shift & mask to  */
#define ENDPOINT_NUMBER_MASK            (0xf0)/* use in setting and getting
                                                 status */

#define ENDPOINT_DIRECTION_SHIFT        (3)   /* direction shift & mask to */
#define ENDPOINT_DIRECTION_MASK         (0x08)/* be used for STAT byte
                                                 in BDT */

#define SEND_CONTROL_ENDPOINT_BDT_INDEX (2)   /* BDT Index for Control Endpoint
                                                 SEND direction */
#define RECV_CONTROL_ENDPOINT_BDT_INDEX (0)   /* BDT Index for Control Endpoint
                                                 RECV direction */

#define EPCTL_STALL                     (0x02)/* Stall bit in Endpoint
                                                  Control Reg */

#define USB_SETUP_TOKEN                 (0x0d)/* Setup Token PID */
#define USB_SETUP_DIRECTION             (0x80)/* Data xfer direction
                                                 for Setup packet */

#define INT_STAT_CLEAR_ALL              (0xbf)/* Value to clear
                                                 all Interrupts */
#define ERR_STAT_CLEAR_ALL              (0xbf)/* Value to clear
                                                 all Errors */
#define ERR_ENB_ENABLE_ALL              (0xbf)/* Value to enable
                                                 all Error Interrupts */
#define INTENB_BUS_RESET_VAL            (0x9f)/* Value to enable
                                                 Interrupts in Bus Reset */
#define INTENB_DISABLE_ALL_VAL          (0x00)/* Value to disable all
                                                 Interrupts */

#define MAX_USB_RAM_BUFFER_INDEX        (14)  /* MAX possible RAM buffer
                                                 Index */

#define EP_START_BUFFER_ADDR            (0x08)/* First USB_RAM buffer offset*/

#define ASSERT_RESUME_DELAY_COUNT       (20000)/* Delay for assert resume, 48MHz clock */

#define NO_ERRORS                       (0)   /* Init value for error */

#define USB_RAM_EVEN_BUFFER             (0)
#define USB_RAM_ODD_BUFFER              (1)

#define SWAP16(val)			(val)

#define SWAP32(val)			(val)

/******************************************************************************
 * Types
 *****************************************************************************/
#ifdef LONG_SEND_TRANSACTION
  #define LONG_TRANSACTION
#endif

#ifdef LONG_RECEIVE_TRANSACTION
  #define LONG_TRANSACTION
#endif

typedef union {
  uint_8 Byte;
  struct {
    uint_8 EP_HSHK     :1;                                       /* When set this bet enables an endpoint to perform handshaking during a transaction to this endpoint. This bit will generally be set unless the endpoint is Isochronous */
    uint_8 EP_STALL    :1;                                       /* When set this bit indicates that the endpoint is stalled */
    uint_8 EP_TX_EN    :1;                                       /* This bit, when set, enables the endpoint for TX transfers */
    uint_8 EP_RX_EN    :1;                                       /* This bit, when set, enables the endpoint for RX transfers */
    uint_8 EP_CTRL_DIS  :1;                                       /* This bit, when set, disables control (SETUP) transfers. When cleared, control transfers are enabled. This applies if and only if the EP_RX_EN and EP_TX_EN bits are also set */
    uint_8             :1;
    uint_8 RETRY_DIS   :1;                                       /* This is a Host mode only bit and is only present in the control register for endpoint 0 (ENDPT0) */
    uint_8 HOST_WO_HUB :1;                                       /* This is a Host mode only bit and is only present in the control register for endpoint 0 (ENDPT0) */
  } Bits;
} ENDPT0STR;

#pragma pack(push)
#pragma pack(1)
/* This structure is used to hold endpoint paramaetes and the
   transaction parameters on the IO's happening on them */
typedef struct  _BDT_ELEM
{
	uint_16            len;             /* endpoint max buffer len */
	uint_32            addr;            /* endpoint buffer addr in USB_RAM */
#ifdef LONG_TRANSACTION
    uint_8_ptr         app_buffer;      /* application buffer pointer */
    USB_PACKET_SIZE    app_len;         /* application buffer len */
    USB_PACKET_SIZE    curr_offset;     /* current offset for long transactions */
#endif
    uint_8         flag;                /* zero termination flag */
    uint_8         bdtmap_index;        /* Corresponding to the buffer */
    uint_8         direction;           /* Direction (Send/Receive) */
    uint_8         type;                /* Type of Endpoint */
} BDT_ELEM, *P_BDT_ELEM;
#if defined(__CWCC__)
	#pragma options align = reset
#elif defined(__IAR_SYSTEMS_ICC__)
	#pragma pack()
#else /* gcc */
#pragma pack(pop)
#endif


uint_8 USB_DCI_DeInit(void);

 /*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern uint_8 USB_Device_Call_Service(
    uint_8                    type,
    PTR_USB_DEV_EVENT_STRUCT  event
);

#ifndef OTG_BUILD
void USB_ISR(void);
#endif

#if HIGH_SPEED_DEVICE
	/* Device Queue Head and Device Transfer Descriptor Related Defination */
	#define SIZE_OF_QHD     0x40
	#define SIZE_OF_DTD0    0x20
	#define SIZE_OF_DTD1    0x20
	#define dTD_SIZE_EPIN   (SIZE_OF_DTD0 + SIZE_OF_DTD1)   //0x40
	#define dTD_SIZE_EPOUT  (SIZE_OF_DTD0 + SIZE_OF_DTD1)   //0x40
	#define BUFFER_USED_PER_EP ((SIZE_OF_QHD + dTD_SIZE_EPIN) +(SIZE_OF_QHD + dTD_SIZE_EPOUT))  //0x100
	#define ZLT_ENABLE      0
	#define ZLT_DISABLE     1
	#define IOS_NOTSET      0
	#define IOS_SET         1
	#define IOC_NOTSET      0
	#define IOC_SET         1
	#define TERMINATE       1
	#define NOT_TERMINATE   0
	#define NO_STATUS       0
	#define ACTIVE          0x00000080
	#define EPOUT_COMPLETE  0x00000001
	#define EPIN_COMPLETE   0x00010000
	#define EPOUT_PRIME     0x00000001
	#define EPIN_PRIME      0x00010000
	#define EPOUT_ENABLE    0x00000080
	#define EPIN_ENABLE     0x00800000
	#define STALL_RX        0x00000001
	#define STALL_TX        0x00010000

	/* Maximum packet size defination */
	#define MPS_8	8
	#define MPS_64	64
	
	/* enum for endpoint numbers */
	enum {
		EP0,
		EP1,
		EP2,
		EP3,
		EP4,
		EP5
	};

	enum {
		OUT,
		IN
	};

	/* enum for data transfer type on endpoints */
	enum {
		CONTROL,
		ISOCHRONOUS,
		BULK,
		INTERRUPT
	};

	/* Status of all transaction on USB */
	typedef enum {
		USB_SUCCESS,
		USB_FAILURE,
		USB_INVALID = -1            /* Always Keep this entry in last */
	} usb_status_t;

	/* USB Device State which are handled by DCD */
	typedef enum {
		USB_DEV_DUMMY_STATE,
		USB_DEV_DEFAULT_STATE,
		USB_DEV_ADDRESSED_STATE,
		USB_DEV_CONFIGURED_STATE
	} usb_state_t;
#endif // HIGH_SPEED_DEVICE

#endif
