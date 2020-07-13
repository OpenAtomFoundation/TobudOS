#include "arch/lwip_hooks.h"


struct netif * hook_ip4_route_src(const ip4_addr_t *dest, const ip4_addr_t *src)
{
    struct netif *netif;

#if LWIP_MULTICAST_TX_OPTIONS
    /* Use administratively selected interface for multicast by default */
    if (ip4_addr_ismulticast(src) && ip4_default_multicast_netif) 
    {
        return ip4_default_multicast_netif;
    }
#endif /* LWIP_MULTICAST_TX_OPTIONS */

    /* iterate through netifs */
    for (netif = netif_list; netif != NULL; netif = netif->next) 
    {
        /* is the netif up, does it have a link and a valid address? */
        if (netif_is_up(netif) && netif_is_link_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) 
        {
            /* network mask matches? */
            if (ip4_addr_netcmp(src, netif_ip4_addr(netif), netif_ip4_netmask(netif))) {
                /* return netif on which to forward IP packet */
                return netif;
            }
            
            /* gateway matches on a non broadcast interface? (i.e. peer in a point to point interface) */
            if (((netif->flags & NETIF_FLAG_BROADCAST) == 0) && ip4_addr_cmp(src, netif_ip4_gw(netif))) {
                /* return netif on which to forward IP packet */
                return netif;
            }
        }
    }
    
    if ((netif_default == NULL) || !netif_is_up(netif_default) || !netif_is_link_up(netif_default) ||
        ip4_addr_isany_val(*netif_ip4_addr(netif_default))) 
    {
        /* No matching netif found and default netif is not usable.
        If this is not good enough for you, use LWIP_HOOK_IP4_ROUTE() */
        LWIP_DEBUGF(IP_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip4_route: No route to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
        ip4_addr1_16(src), ip4_addr2_16(src), ip4_addr3_16(src), ip4_addr4_16(src)));
        
        IP_STATS_INC(ip.rterr);
        MIB2_STATS_INC(mib2.ipoutnoroutes);
        return NULL;
    }

    return netif_default;
}






