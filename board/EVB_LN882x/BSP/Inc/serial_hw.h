#ifndef __SERIAL_HW_H__
#define __SERIAL_HW_H__

#include "hal/hal_uart.h"

struct SerialHardware;
struct Serial;

typedef enum {
    SER_PORT_UART0 = 0,
    SER_PORT_UART1 = 1,
    SER_PORT_NUM   = 2,  /**< Number of serial ports */
    SER_PORT_ID_INVALID = SER_PORT_NUM
}SerialPortID;

struct SerialHardwareVT
{
    void (*init       )(struct SerialHardware *ctx, struct Serial *ser, uint32_t baudrate);
    void (*cleanup    )(struct SerialHardware *ctx);
    void (*txStart    )(struct SerialHardware *ctx);
    bool (*txSending  )(struct SerialHardware *ctx);
    bool (*setBaudrate)(struct SerialHardware *ctx, uint32_t baudrate);
};

struct SerialHardware
{
    const struct SerialHardwareVT *table;
    unsigned char *txbuffer;
    unsigned char *rxbuffer;
    size_t         txbuffer_size;
    size_t         rxbuffer_size;
    void          *hw_device;
    volatile bool  isSending;
};

struct SerialHardware *serial_hw_getdesc(SerialPortID port_id);



#endif /* __SERIAL_HW_H__ */
