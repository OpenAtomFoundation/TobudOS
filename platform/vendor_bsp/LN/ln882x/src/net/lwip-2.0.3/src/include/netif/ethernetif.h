#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__
#include "lwip/ip4_addr.h"
#include "wifi/wifi.h"

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

typedef enum{
    NETIF_DOWN = 0,
    NETIF_UP
}netif_state_enum_t;

typedef enum{
    LINK_DOWN = 0,
    LINK_UP
}network_state_enum_t;

typedef struct {
    ip4_addr_t ip; 
    ip4_addr_t netmask;
    ip4_addr_t gw;
    //ip4_addr_t dns;
} tcpip_ip_info_t;


int lwip_tcpip_init(void);
int ethernetif_set_dhcp_mode(wifi_interface_enum_t nif_idx, dhcp_mode_enum_t dhcp_mode);
dhcp_mode_enum_t ethernetif_get_dhcp_mode(wifi_interface_enum_t nif_idx);
int ethernetif_set_ip_info(wifi_interface_enum_t nif_idx, tcpip_ip_info_t *ip_info);
int ethernetif_get_ip_info(wifi_interface_enum_t nif_idx, tcpip_ip_info_t *ip_info);
int ethernetif_set_mac_addr(wifi_interface_enum_t nif_idx, uint8_t *mac_addr);
int ethernetif_get_mac_addr(wifi_interface_enum_t nif_idx, uint8_t *mac_addr);
int ethernetif_set_state(wifi_interface_enum_t nif_idx, netif_state_enum_t up);
wifi_interface_enum_t ethernetif_get_active(void);
struct netif *ethernetif_get_netif(wifi_interface_enum_t nif_idx);
network_state_enum_t ethernetif_get_link_state(void);
void ethernetif_low_level_input(wifi_interface_enum_t nif_idx, uint8_t *data, uint16_t len);
void ethernetif_remove_station(uint8_t *mac);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ETHERNETIF_H__ */

