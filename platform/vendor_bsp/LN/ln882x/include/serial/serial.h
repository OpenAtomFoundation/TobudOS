#ifndef __SERIAL_H__
#define __SERIAL_H__
#include "proj_config.h"

#include "serial_hw.h"
#include "utils/fifo/fifobuf.h"

/** Human-readable serial error descriptions */
extern const char * const serial_errors[8];
typedef void (* rx_callbcak)(void);

/** Serial handle structure */
typedef struct Serial
{
    /** Physical port id */
    SerialPortID port_id;
    bool is_open;

    FIFOBuffer txfifo;
    FIFOBuffer rxfifo;

    int32_t rxtimeout;
    int32_t txtimeout;

    /** Holds the flags defined above.  Will be 0 when no errors have occurred. */
    volatile uint32_t status;
    rx_callbcak callback;
    /** Low-level interface to hardware. */
    struct SerialHardware* hw;
} Serial_t;

int serial_getchar_nowait(Serial_t *fd);

void serial_purgeRx(Serial_t *fd);
void serial_purgeTx(Serial_t *fd);
void serial_purge(Serial_t *fd);
void serial_init(Serial_t *fd,SerialPortID port_id, uint32_t baudrate, rx_callbcak cb);
void serial_deinit(Serial_t *fd);
size_t serial_read(Serial_t *fd, void *_buf, size_t size);
size_t serial_write(Serial_t *fd, const void *_buf, size_t size);
int serial_putchar(Serial_t *port, int c);

int serial_flush(Serial_t *fd);
//int serial_error(Serial_t *fd);
//void serial_clearerr(Serial_t *fd);
bool serial_setbaudrate(struct Serial *fd, uint32_t baudrate);


#define serial_getstatus(serial)    ((serial)->status)
#define serial_setstatus(serial, new_status) ((serial)->status = (new_status))

#endif /* __SERIAL_H__ */
