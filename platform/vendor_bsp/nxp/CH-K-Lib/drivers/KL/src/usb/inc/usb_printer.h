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
 * @file usb_printer.h
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief The file contains USB stack PRINTER class layer API header function.
 *
 *****************************************************************************/
#ifndef _USB_PRINTER_H
#define _USB_PRINTER_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "types.h"
#include "usb_descriptor.h"
#include "usb_class.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/
/******************************************************************************
 * Macro's
 *****************************************************************************/
/* Queue element */
#define MAX_QUEUE_ELEMS                     (4)
#define COMPONENT_PREPARE_SHIFT             (0x07)

#define STRING_ID_SIZE                      (3)

/* class specific requests */
#define USB_PRINTER_GET_DEVICE_ID           (0)
#define USB_PRINTER_GET_PORT_STATUS         (1)
#define USB_PRINTER_SOFT_RESET              (2)

/* define for port status */
#define USB_PRINTER_NOT_ERROR               (0x08)
#define USB_PRINTER_SELECT                  (0x10)
#define USB_PRINTER_PAGE_EMPTY              (0x20)

#define PRINTER_SEND_DATA_BUFF_SIZE         (PRINTER_SEND_BUFF_SIZE)
   
#define USB_Printer_Bulk_Send_Data(a,b,c)  USB_Class_Printer_Send_Data(a,PRINTER_BULK_IN_ENDPOINT,b,c)
#define USB_Printer_Bulk_Recv_Data(a,b,c)  _usb_device_recv_data(a,PRINTER_BULK_OUT_ENDPOINT,b,c)
/******************************************************************************
 * Types
 *****************************************************************************/
  typedef struct _app_printer_data_struct
{
    uint_8_ptr      data_ptr;       /* pointer to buffer */
    USB_PACKET_SIZE data_size;      /* buffer size of endpoint */
}APP_PRINTER_DATA_STRUCT;

 /* structure to hold a request in the endpoint queue */
typedef struct _usb_class_printer_queue
{
    uint_8 controller_ID;   /* Controller ID*/
    uint_8 channel;         /* Endpoint number */
    uint_8_ptr app_buff;    /* Buffer to send */
    USB_PACKET_SIZE size;   /* Size of the transfer */
}USB_CLASS_PRINTER_QUEUE, *PTR_USB_CLASS_PRINTER_QUEUE;

/* USB class printer endpoint data */
typedef struct _usb_class_printer_endpoint
{
    uint_8 endpoint; /* endpoint num */
    uint_8 type;     /* type of endpoint (interrupt, bulk or isochronous) */
    uint_8 bin_consumer;/* the num of queued elements */
    uint_8 bin_producer;/* the num of de-queued elements */
    USB_CLASS_PRINTER_QUEUE queue[MAX_QUEUE_ELEMS]; /* queue data */
}USB_CLASS_PRINTER_ENDPOINT;

typedef struct _PRINTER_APP_STRUCT
{
    uint_32 offset;
    uint_32 size;
    uint_8_ptr buff_ptr;
}PRINTER_APP_STRUCT, * PTR_PRINTER_APP_STRUCT;

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern uint_8 USB_Class_Printer_Init (
    uint_8                          controller_ID,
    USB_CLASS_CALLBACK              printer_class_callback,
    USB_REQ_FUNC                    vendor_req_callback,
    USB_CLASS_SPECIFIC_HANDLER_FUNC              param_callback
);
extern uint_8 USB_Class_Printer_DeInit (uint_8 controller_ID); 
extern uint_8 USB_Class_Printer_Send_Data (
    uint_8 controller_ID,       /* [IN] Controller ID */
    uint_8 ep_num,              /* [IN] Endpoint Number */
    uint_8_ptr app_buff,        /* [IN] Buffer to Send */
    USB_PACKET_SIZE size        /* [IN] Length of the Transfer */
);
extern void USB_Printer_Service_Bulk_In(PTR_USB_DEV_EVENT_STRUCT event);
extern void USB_Printer_Service_Bulk_Out(PTR_USB_DEV_EVENT_STRUCT event);
/*****************************************************************************/

#endif
