/************************************************************************************
 * Copyright @ 1995-2005 Freescale Semiconductor, Inc. All rights reserved          *
 *                                                                                  *
 *                                                                                  *
 *                                                                                  *
 * DESCRIPTION                                                                      *
 *   USB Descriptors definitions.                                                   *
 *                                                                                  *
 *                                                                                  *
 *                                                                                  *
 * NOTE                                                                             *
 *                                                                                  *
 *                                                                                  *
 *                                                                                  *
 * HISTORY                                                                          *
 *                                                                                  *	
 ************************************************************************************/


#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

/********************************************************************/

/* USB descriptor types */
#define DEVICE					1
#define CONFIGURATION			2
#define STRING					3
#define INTERFACE				4
#define ENDPOINT				5

/* USB Standard Request Types - bRequestType */
#define IN						0x80
#define OUT						0x00
#define STANDARD				0x00
#define CLASS					0x20
#define VENDOR					0x40
#define RQ_DEVICE				0x00
#define RQ_INTERFACE			0x01
#define RQ_ENDPOINT			0x02

/* USB Standard Request Codes - bRequest */
#define GET_STATUS			0
#define CLEAR_FEATURE		1
#define SET_FEATURE			3
#define SET_ADDRESS			5
#define GET_DESCRIPTOR		6
#define SET_DESCRIPTOR		7
#define GET_CONFIGURATION	8
#define SET_CONFIGURATION	9
#define GET_INTERFACE		10
#define SET_INTERFACE		11
#define SYNCH_FRAME			12

/* Configuration bmAttributes fields */
#define BUS_POWERED			0x80
#define SELF_POWERED			0xC0
#define REMOTE_WAKEUP		0xA0

/* Endpoint bmAttributes fields */
#define CONTROL				0x00
#define ISOCHRONOUS			0x01
#define BULK					0x02
#define INTERRUPT				0x03
#define DISABLED				0xFF

/* Standard Feature Selectors */
#define DEVICE_REMOTE_WAKEUP	1
#define ENDPOINT_HALT			0

/* Structure for USB Device Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 bcdUSBL;
	uint8 bcdUSBH;
	uint8 bDeviceClass;
	uint8 bDeviceSubClass;
	uint8 bDeviceProtocol;
	uint8 bMaxPacketSize0;
	uint8 idVendorL;
	uint8 idVendorH;
	uint8 idProductL;
	uint8 idProductH;
	uint8 bcdDeviceL;
	uint8 bcdDeviceH;
	uint8 iManufacturer;
	uint8 iProduct;
	uint8 iSerialNumber;
	uint8 bNumConfigurations;
} USB_DEVICE_DESC;

/* Structure for USB Configuration Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 wTotalLengthL;
	uint8 wTotalLengthH;
	uint8 bNumInterfaces;
	uint8 bConfigurationValue;
	uint8 iConfiguration;
	uint8 bmAttributes;
	uint8 maxPower;
} USB_CONFIG_DESC;

/* Structure for USB Interface Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 bInterfaceNumber;
	uint8 bAlternateSetting;
	uint8 bNumEndpoints;
	uint8 bInterfaceClass;
	uint8 bInterfaceSubClass;
	uint8 bInterfaceProtocol;
	uint8 iInterface;
} USB_INTERFACE_DESC;

/* Structure for USB Endpoint Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 bEndpointAddress;
	uint8 bmAttributes;
	uint8 wMaxPacketSizeL;
	uint8 wMaxPacketSizeH;
	uint8 bInterval;
} USB_ENDPOINT_DESC;

/* Structure for USB String Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 * bString;
} USB_STRING_DESC;

/********************************************************************/

#endif /* DESCRIPTOR_H */

