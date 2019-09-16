#ifndef __USBD_DESC_H_
#define	__USBD_DESC_H_



/* Descriptors */
#define DESCRIPTOR_TYPE(wValue)  (wValue >> 8)
#define DESCRIPTOR_INDEX(wValue) (wValue & 0xff)

/* Standard descriptor types */
#define DEVICE_DESCRIPTOR        (1)
#define CONFIGURATION_DESCRIPTOR (2)
#define STRING_DESCRIPTOR        (3)
#define INTERFACE_DESCRIPTOR     (4)
#define ENDPOINT_DESCRIPTOR      (5)
#define QUALIFIER_DESCRIPTOR     (6)
#define REPORT_DESCRIPTOR        (0x22)

/* Standard descriptor lengths */
#define DEVICE_DESCRIPTOR_LENGTH        (0x12)
#define CONFIGURATION_DESCRIPTOR_LENGTH (0x09)
#define INTERFACE_DESCRIPTOR_LENGTH     (0x09)
#define ENDPOINT_DESCRIPTOR_LENGTH      (0x07)


/*string offset*/
#define STRING_OFFSET_LANGID            (0)
#define STRING_OFFSET_IMANUFACTURER     (1)
#define STRING_OFFSET_IPRODUCT          (2)
#define STRING_OFFSET_ISERIAL           (3)
#define STRING_OFFSET_ICONFIGURATION    (4)
#define STRING_OFFSET_IINTERFACE        (5)

/* USB Specification Release Number */
#define USB_VERSION_2_0 (0x0200)

/* Least/Most significant byte of short integer */
#define LSB(n)  ((n)&0xff)
#define MSB(n)  (((n)&0xff00)>>8)


#endif

