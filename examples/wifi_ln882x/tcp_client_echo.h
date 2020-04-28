#ifndef __TCP_CLIENT_ECHO_H__
#define __TCP_CLIENT_ECHO_H__

#include "stdint.h"

typedef struct{
    uint8_t * ip;
    uint32_t  port;
}target_ser_t;

void tcp_client_echo_task_creat(uint8_t * ser_ip, uint32_t port);



#endif /* __TCP_CLIENT_ECHO_H__ */

