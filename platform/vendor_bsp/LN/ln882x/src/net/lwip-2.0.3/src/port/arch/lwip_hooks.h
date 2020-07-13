
#ifndef __LWIP_HOOKS_H__
#define __LWIP_HOOKS_H__


#ifdef  __cplusplus
extern "C" {
#endif
    
#include "lwip/tcpip.h"

struct netif * hook_ip4_route_src(const ip4_addr_t *dest, const ip4_addr_t *src);



#ifdef  __cplusplus
}
#endif

#endif /* __LWIP_HOOKS_H__ */
