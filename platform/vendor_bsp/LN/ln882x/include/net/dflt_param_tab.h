
#ifndef _DFLT_PARAM_TAB_H_
#define _DFLT_PARAM_TAB_H_
#include "wifi/wifi.h"

//TODO: Get these static parameters from flash.
#define WIFI_MODE_DEFAULT               WIFI_MODE_STATION
#define WIFI_AP_SSID_DEFAULT            "LN8826"
#define WIFI_AP_PASSWORD_DEFAULT        "12345678"
#define WIFI_AP_CHANNEL_DEFAULT         7
#define WIFI_AP_AUTH_MODE_DEFAULT       WIFI_AUTH_OPEN
#define WIFI_AP_SSID_HIDDEN_DEFAULT     0
#define WIFI_AP_MAX_CONNECTION_DEFAULT  3
#define WIFI_AP_BEACON_INTERVAL_DEFAULT 100


/**<  Config sta modle network card default parameters*/
/**<  static IP*/
#define STA_IP_ADDR0         192
#define STA_IP_ADDR1         168
#define STA_IP_ADDR2         1
#define STA_IP_ADDR3         20

/**< Subnet mask*/
#define STA_NETMASK_ADDR0    255
#define STA_NETMASK_ADDR1    255
#define STA_NETMASK_ADDR2    255
#define STA_NETMASK_ADDR3    0

/**<  GateWay*/
#define STA_GW_ADDR0         192
#define STA_GW_ADDR1         168
#define STA_GW_ADDR2         1
#define STA_GW_ADDR3         1

/**<  MAC Addr*/
#define STA_MAC_ADDR0        0x00
#define STA_MAC_ADDR1        0x50
#define STA_MAC_ADDR2        0xC2
#define STA_MAC_ADDR3        0x5E
#define STA_MAC_ADDR4        0x10
#define STA_MAC_ADDR5        0x8D


/**<  Config softAP modle network card default parameters*/
/**<  static IP*/
#define AP_IP_ADDR0          192
#define AP_IP_ADDR1          168
#define AP_IP_ADDR2          1
#define AP_IP_ADDR3          1
                             
/**< Subnet mask*/           
#define AP_NETMASK_ADDR0     255
#define AP_NETMASK_ADDR1     255
#define AP_NETMASK_ADDR2     255
#define AP_NETMASK_ADDR3     0
                             
/**<  GateWay*/              
#define AP_GW_ADDR0          192
#define AP_GW_ADDR1          168
#define AP_GW_ADDR2          1
#define AP_GW_ADDR3          1
                             
/**<  MAC Addr*/             
#define AP_MAC_ADDR0         STA_MAC_ADDR0
#define AP_MAC_ADDR1         STA_MAC_ADDR1
#define AP_MAC_ADDR2         STA_MAC_ADDR2
#define AP_MAC_ADDR3         STA_MAC_ADDR3
#define AP_MAC_ADDR4         STA_MAC_ADDR4
#define AP_MAC_ADDR5         0x8B



/* Config DHCP server default param*/
#define DHCPD_IP_LEASE_TIME             (2880)
#define DHCPD_IP_RENEW_TIME             (2880)
#define DHCPD_LISTEN_PORT               (67)
#define DHCPD_CLIENT_MAX                (WIFI_AP_MAX_CONNECTION_DEFAULT)
#define DHCPD_IP_START                  ((100 << 24)|(AP_IP_ADDR2 << 16)|(AP_IP_ADDR1 << 8)|AP_IP_ADDR0)
#define DHCPD_IP_END                    ((102 << 24)|(AP_IP_ADDR2 << 16)|(AP_IP_ADDR1 << 8)|AP_IP_ADDR0)


#define STA_IFNAME                      "st"
#define AP_IFNAME                       "ap"
#define STA_HOSTNAME                    "wifi_sta"
#define AP_HOSTNAME                     "wifi_ap"

#endif
