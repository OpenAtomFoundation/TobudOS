#ifndef __PING_H__
#define __PING_H__

#include "osal/osal.h"
#include "types.h"

/**
 * PING_USE_SOCKETS: Set to 1 to use sockets, otherwise the raw api is used
 */
#ifndef PING_USE_SOCKETS
#define PING_USE_SOCKETS    LWIP_SOCKET
#endif

typedef struct {
    uint32_t ip;
    uint32_t timers;
}ping_info_t;

typedef struct{
    char * host;
    uint32_t interval_ms;/* ms */
    uint32_t cnt;        /* -t */
}ping_param_t;

bool creat_ping_task(ping_param_t * ping_param);


#endif /* __PING_H__ */
