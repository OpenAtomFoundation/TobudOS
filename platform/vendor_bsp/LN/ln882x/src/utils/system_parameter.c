#include <string.h>
#include "net/dflt_param_tab.h"
#include "lwip/ip4_addr.h"
#include "utils/eeprom.h"
#include "utils/system_parameter.h"
#include "utils/debug/log.h"
#include "utils/debug/art_assert.h"

typedef struct {
    wifi_mode_enum_t  mode;
    uint8_t           sta_addr[MAC_ADDRESS_LEN];
    uint8_t           ap_addr[MAC_ADDRESS_LEN];
} wifi_common_config_t;

typedef struct {
    uint8_t sta_hostname[NETIF_HOSTNAME_LEN_MAX+1];
    uint8_t ap_hostname[NETIF_HOSTNAME_LEN_MAX+1];
} hostname_t;

typedef enum {
    WIFI_COMMON_CONFIG = 0,
    WIFI_STA_CONFIG,
    WIFI_AP_CONFIG,
    STA_IP_CONFIG,
    AP_IP_CONFIG,
    DHCP_ENABLE_CONFIG,
    HOSTNAME_CONFIG,
    DHCPD_CONFIG,
    WIFI_PSK_INFO,
} system_parameter_item_id_enum_t;

typedef struct system_parameter{
    wifi_common_config_t    wifi_common_config;
    wifi_sta_config_t       sta_config;
    wifi_ap_config_t        ap_config;
    tcpip_ip_info_t         sta_ip_config;
    tcpip_ip_info_t         ap_ip_config;
    uint8_t                 dhcp_sw_config;
    hostname_t              hostname_cfg;
    server_config_t         dhcpd_config;
    OS_Mutex_t              lock;
}system_parameter_t;

typedef struct {
    uint32_t                    item_id;
    union
    {
        wifi_common_config_t    wifi_common_config;
        wifi_sta_config_t       sta_config;
        wifi_ap_config_t        ap_config;
        tcpip_ip_info_t         sta_ip_config;
        tcpip_ip_info_t         ap_ip_config;
        uint8_t                 dhcp_sw_config;          
        hostname_t              hostname_cfg;
	    server_config_t         dhcpd_config;
    }item;
} system_param_item_t;


static system_parameter_t g_system_parameter;
static system_param_item_t g_item;

static void system_parameter_sync_to_flash(system_param_item_t *param_item);
static system_param_item_t *get_param_item_cache(void)
{
    return &g_item;
}

static system_parameter_t *get_system_parameter_handle(void)
{
    return &(g_system_parameter);
}

int system_parameter_init(void)
{
    OS_Status ret = OS_OK;
    system_parameter_t *system_parameter = get_system_parameter_handle();
    uint16_t *system_parameter_buffer = (uint16_t *)system_parameter;
    uint8_t addr[MAC_ADDRESS_LEN] = {0,};
    wifi_config_t config = {0,};
    server_config_t dhcp_server_config = {0,};
    tcpip_ip_info_t  ip_config = {0,};

    memset(system_parameter, 0, sizeof(system_parameter_t));
    ret = OS_MutexCreate(&system_parameter->lock);
    if (ret != OS_OK) {
        LOG(LOG_LVL_ERROR, "[%s, %d]OS_MutexCreate system_parameter->lock fail.\r\n", __func__, __LINE__);
        return -1;
    }

#if (FLASH_XIP == DISABLE) // run in RAM.
    FLASH_Init();
#endif

    if(EE_Init(system_parameter_buffer, sizeof(system_parameter_t) - sizeof(OS_Mutex_t)) < 0){
        //mode
        system_parameter_set_wifi_mode(system_parameter_get_wifi_mode_default());

        //sta_macaddr
        system_parameter_get_wifi_macaddr_default(STATION_IF, addr);
        system_parameter_set_macaddr(STATION_IF, addr);

        //ap_macaddr
        system_parameter_get_wifi_macaddr_default(SOFT_AP_IF, addr);
        system_parameter_set_macaddr(SOFT_AP_IF, addr);

        //sta config
        system_parameter_get_wifi_config_default(STATION_IF, &config);
        system_parameter_set_config(STATION_IF, &config);

        //ap config
        system_parameter_get_wifi_config_default(SOFT_AP_IF, &config);
        system_parameter_set_config(SOFT_AP_IF, &config);

        //sta ip
        system_parameter_get_ip_config_default(STATION_IF, &ip_config);
        system_parameter_set_ip_config(STATION_IF, &ip_config);

        //ap ip
        system_parameter_get_ip_config_default(SOFT_AP_IF, &ip_config);
        system_parameter_set_ip_config(SOFT_AP_IF, &ip_config);

        //dhcp server config
        system_parameter_get_dhcpd_config_default(&dhcp_server_config);
        system_parameter_set_dhcpd_config(&dhcp_server_config);
    }
    return 0;
}
int system_parameter_deinit(void)
{
    OS_Status ret = OS_OK;
    system_parameter_t *system_parameter = get_system_parameter_handle();

    ART_ASSERT(system_parameter);

    ret = OS_MutexDelete(&system_parameter->lock);
    if (ret != OS_OK) {
        LOG(LOG_LVL_ERROR, "[%s, %d]OS_MutexDelete system_parameter->lock fail.\r\n", __func__, __LINE__);
        return -1;
    }

#if (FLASH_XIP == DISABLE)
    FLASH_Deinit();
#endif
    return 0;
}

void system_parameter_sync_to_flash(system_param_item_t *param_item)
{
    uint32_t start = 0, len = 0, i = 0, flag = 0;
    uint8_t *cache_content = NULL, *system_parameter_base = NULL;
    system_parameter_t *system_parameter = get_system_parameter_handle();
    uint16_t *system_parameter_base_u16 = NULL;
    system_parameter_item_id_enum_t item_id;

    ART_ASSERT(system_parameter && param_item);
    system_parameter_base = (uint8_t *)system_parameter;
    system_parameter_base_u16 = (uint16_t *)system_parameter;
    item_id = (system_parameter_item_id_enum_t)param_item->item_id;
    switch(item_id) {

        case WIFI_COMMON_CONFIG: {
            cache_content = (uint8_t *)&(param_item->item.wifi_common_config);
            len = sizeof(wifi_common_config_t);
            start = (uint8_t *)&(system_parameter->wifi_common_config) - system_parameter_base;
        }
        break;
        
        case WIFI_STA_CONFIG: {
            cache_content = (uint8_t *)&(param_item->item.sta_config);
            len = sizeof(wifi_sta_config_t);
            start = (uint8_t *)&(system_parameter->sta_config) - system_parameter_base;
        }
        break;

        case WIFI_AP_CONFIG: {
            cache_content = (uint8_t *)&(param_item->item.ap_config);
            len = sizeof(wifi_ap_config_t);
            start = (uint8_t *)&(system_parameter->ap_config) - system_parameter_base;
        }
        break;

        case STA_IP_CONFIG: {
            cache_content = (uint8_t *)&(param_item->item.sta_ip_config);
            len = sizeof(tcpip_ip_info_t);
            start = (uint8_t *)&(system_parameter->sta_ip_config) - system_parameter_base;
        }
        break;

        case AP_IP_CONFIG: {
            cache_content = (uint8_t *)&(param_item->item.ap_ip_config);
            len = sizeof(tcpip_ip_info_t);
            start = (uint8_t *)&(system_parameter->ap_ip_config) - system_parameter_base;
        }
        break;
        
        case DHCP_ENABLE_CONFIG: {
            cache_content = (uint8_t *)&(param_item->item.dhcp_sw_config);
            len = sizeof(uint8_t);
            start = (uint8_t *)&(system_parameter->dhcp_sw_config) - system_parameter_base;
        }
        break;

        case HOSTNAME_CONFIG: {
            cache_content = (uint8_t *)&(param_item->item.hostname_cfg);
            len = sizeof(hostname_t);
            start = (uint8_t *)&(system_parameter->hostname_cfg) - system_parameter_base;
        }
        break;
        
	    case DHCPD_CONFIG: {
            cache_content = (uint8_t *)&(param_item->item.dhcpd_config);
            len = sizeof(server_config_t);
            start = (uint8_t *)&(system_parameter->dhcpd_config) - system_parameter_base;
        }
        break;

        default:
            break;
    }

    for(i = start; i < (start + len); i++){
        if(system_parameter_base[i] != cache_content[i - start]){
            system_parameter_base[i] = cache_content[i - start];
            if(i%2){
                EE_WriteVariable(i/2, system_parameter_base_u16[i/2]);
                flag = 0;
            }else{
                flag = 1;
            }
        }else{
            if(i%2 && flag == 1){
                EE_WriteVariable(i/2, system_parameter_base_u16[i/2]);
                flag = 0;
            }
        }
    }
    if(flag == 1){
        EE_WriteVariable(i/2, system_parameter_base_u16[i/2]);
        flag = 0;
    }
    
}

//mac config
int system_parameter_set_macaddr(wifi_interface_enum_t if_index, uint8_t *macaddr)
{
    uint8_t *mac = NULL;
    system_parameter_t *system_parameter = get_system_parameter_handle();
    system_param_item_t *param_item = get_param_item_cache();

    ART_ASSERT(system_parameter && macaddr);
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    memcpy(&param_item->item.wifi_common_config, &system_parameter->wifi_common_config, sizeof(wifi_common_config_t));
    param_item->item_id = (uint32_t)WIFI_COMMON_CONFIG;
    if(STATION_IF == if_index){
        mac = param_item->item.wifi_common_config.sta_addr;
    }else{
        mac = param_item->item.wifi_common_config.ap_addr;
    }
    memcpy(mac, macaddr, MAC_ADDRESS_LEN);
    system_parameter_sync_to_flash(param_item);
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}
int system_parameter_get_macaddr(wifi_interface_enum_t if_index, uint8_t *macaddr)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();

    ART_ASSERT(macaddr);
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    if(STATION_IF == if_index){
        memcpy(macaddr, system_parameter->wifi_common_config.sta_addr, MAC_ADDRESS_LEN);
    }else{
        memcpy(macaddr, system_parameter->wifi_common_config.ap_addr, MAC_ADDRESS_LEN);
    }
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}

//wifi config
int system_parameter_set_config(wifi_interface_enum_t if_index, wifi_config_t *config)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();
    system_param_item_t *param_item = get_param_item_cache();

    ART_ASSERT(config);
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    if(STATION_IF == if_index){
        param_item->item_id = (uint32_t)WIFI_STA_CONFIG;
        memcpy(&param_item->item.sta_config, config, sizeof(wifi_config_t));
    }else{
        param_item->item_id = (uint32_t)WIFI_AP_CONFIG;
        memcpy(&param_item->item.ap_config, config, sizeof(wifi_config_t));
    }
    system_parameter_sync_to_flash(param_item);
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}
int system_parameter_get_config(wifi_interface_enum_t if_index, wifi_config_t *config)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();

    ART_ASSERT(config);
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    if(STATION_IF == if_index){
        memcpy(config, &(system_parameter->sta_config), sizeof(wifi_config_t));
    }else{
        memcpy(config, &(system_parameter->ap_config), sizeof(wifi_config_t));
    }
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}

//ip config
int system_parameter_set_ip_config(wifi_interface_enum_t if_index, tcpip_ip_info_t *ip_config)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();
    system_param_item_t *param_item = get_param_item_cache();

    ART_ASSERT(system_parameter && ip_config);
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    if(STATION_IF == if_index){
        param_item->item_id = (uint32_t)STA_IP_CONFIG;
        memcpy(&param_item->item.sta_ip_config, ip_config, sizeof(tcpip_ip_info_t));
    }else{
        param_item->item_id = (uint32_t)AP_IP_CONFIG;
        memcpy(&param_item->item.ap_ip_config, ip_config, sizeof(tcpip_ip_info_t));
    }
    system_parameter_sync_to_flash(param_item);
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}
int system_parameter_get_ip_config(wifi_interface_enum_t if_index, tcpip_ip_info_t *ip_config)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();

    ART_ASSERT(system_parameter && ip_config);
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    if(STATION_IF == if_index){
        memcpy(ip_config, &(system_parameter->sta_ip_config), sizeof(tcpip_ip_info_t));
    }else{
        memcpy(ip_config, &(system_parameter->ap_ip_config), sizeof(tcpip_ip_info_t));
    }
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}

//dhcp enable config
int system_parameter_set_dhcp_enable(uint8_t en)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();
    system_param_item_t *param_item = get_param_item_cache();

    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    param_item->item_id             = (uint32_t)DHCP_ENABLE_CONFIG;
    param_item->item.dhcp_sw_config = en;
    system_parameter_sync_to_flash(param_item);
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}

uint8_t system_parameter_get_dhcp_enable(void)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();

    return system_parameter->dhcp_sw_config;
}

//wifi mode config
int system_parameter_set_wifi_mode(wifi_mode_enum_t wifi_mode)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();
    system_param_item_t *param_item = get_param_item_cache();

    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    memcpy(&param_item->item.wifi_common_config, &system_parameter->wifi_common_config, sizeof(wifi_common_config_t));
    param_item->item_id = (uint32_t)WIFI_COMMON_CONFIG;
    param_item->item.wifi_common_config.mode = wifi_mode;
    system_parameter_sync_to_flash(param_item);
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}

wifi_mode_enum_t system_parameter_get_wifi_mode(void)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();

    return system_parameter->wifi_common_config.mode;
}

//hostname config
int system_parameter_set_hostname(wifi_interface_enum_t if_index, uint8_t *hostname)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();
    system_param_item_t *param_item = get_param_item_cache();

    ART_ASSERT(hostname);
    
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    param_item->item_id = (uint32_t)HOSTNAME_CONFIG;
    memcpy(&param_item->item.hostname_cfg, &system_parameter->hostname_cfg, sizeof(hostname_t));
    if(STATION_IF == if_index){
        memcpy(&param_item->item.hostname_cfg.sta_hostname, hostname, (strlen((char *)hostname) < NETIF_HOSTNAME_LEN_MAX) ? strlen((char *)hostname) : NETIF_HOSTNAME_LEN_MAX);
    }else{
        memcpy(&param_item->item.hostname_cfg.ap_hostname, hostname, (strlen((char *)hostname) < NETIF_HOSTNAME_LEN_MAX) ? strlen((char *)hostname) : NETIF_HOSTNAME_LEN_MAX);
    }
    system_parameter_sync_to_flash(param_item);
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}
int system_parameter_get_hostname(wifi_interface_enum_t if_index, uint8_t *hostname)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();
    uint8_t *name;
    
    ART_ASSERT(hostname);
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    if(STATION_IF == if_index){
        name = system_parameter->hostname_cfg.sta_hostname;
    }else{
        name = system_parameter->hostname_cfg.ap_hostname;
    }
    memcpy(hostname, name, (strlen((char *)name) < NETIF_HOSTNAME_LEN_MAX) ? strlen((char *)name) : NETIF_HOSTNAME_LEN_MAX);
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}

int system_parameter_set_dhcpd_config(server_config_t *server_config)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();
    system_param_item_t *param_item = get_param_item_cache();

    ART_ASSERT(system_parameter && server_config);
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    param_item->item_id = (uint32_t)DHCPD_CONFIG;
    memcpy(&param_item->item.dhcpd_config, server_config, sizeof(server_config_t));
    system_parameter_sync_to_flash(param_item);
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}
int system_parameter_get_dhcpd_config(server_config_t *server_config)
{
    system_parameter_t *system_parameter = get_system_parameter_handle();
    ART_ASSERT(server_config);
    OS_MutexLock(&system_parameter->lock, OS_WAIT_FOREVER);
    memcpy(server_config, &(system_parameter->dhcpd_config), sizeof(server_config_t));
    OS_MutexUnlock(&system_parameter->lock);
    return 0;
}

wifi_mode_enum_t system_parameter_get_wifi_mode_default(void)
{
    return WIFI_MODE_DEFAULT;
}
int system_parameter_get_wifi_config_default (wifi_interface_enum_t if_index, wifi_config_t *config)
{
    wifi_ap_config_t  *ap = NULL;
    wifi_sta_config_t *sta = NULL;
    int len = 0;
    
    ART_ASSERT(config);
    memset(config, 0, sizeof(wifi_config_t));

    //从出厂默认参数中获取
    if(if_index == STATION_IF){
        sta = &config->sta;
        //memset(sta->ssid, SSID_MAX_LEN, 0);
        //memset(sta->password, PASSWORD_MAX_LEN, 0);
        //memset(sta->bssid, BSSID_LEN, 0);
        sta->channel = 1;
        sta->bssid_set = 0;
    }else{
        ap = &config->ap;
        ap->ssid_len = strlen(WIFI_AP_SSID_DEFAULT);
        memcpy(ap->ssid, WIFI_AP_SSID_DEFAULT, ap->ssid_len);
        len = (strlen(WIFI_AP_PASSWORD_DEFAULT) < PASSWORD_MAX_LEN) ? strlen(WIFI_AP_PASSWORD_DEFAULT) : PASSWORD_MAX_LEN;
        memcpy(ap->password, WIFI_AP_PASSWORD_DEFAULT, len);
        if(strlen((const char *)ap->password) > 0){
            ap->authmode = WIFI_AUTH_WPA2_PSK;
        }else{
            ap->authmode = WIFI_AP_AUTH_MODE_DEFAULT;
        }
        ap->channel = WIFI_AP_CHANNEL_DEFAULT;
        ap->ssid_hidden = WIFI_AP_SSID_HIDDEN_DEFAULT;
        ap->max_connection = WIFI_AP_MAX_CONNECTION_DEFAULT;
        ap->beacon_interval = WIFI_AP_BEACON_INTERVAL_DEFAULT;
    }

    return 0;
}
int system_parameter_get_wifi_macaddr_default (wifi_interface_enum_t if_index, uint8_t *macaddr)
{
    ART_ASSERT(macaddr);
    memset(macaddr, 0, MAC_ADDRESS_LEN);
    if(if_index == STATION_IF){
        macaddr[0] = STA_MAC_ADDR0;
        macaddr[1] = STA_MAC_ADDR1;
        macaddr[2] = STA_MAC_ADDR2;
        macaddr[3] = STA_MAC_ADDR3;
        macaddr[4] = STA_MAC_ADDR4;
        macaddr[5] = STA_MAC_ADDR5;
    }else{
        macaddr[0] = AP_MAC_ADDR0;
        macaddr[1] = AP_MAC_ADDR1;
        macaddr[2] = AP_MAC_ADDR2;
        macaddr[3] = AP_MAC_ADDR3;
        macaddr[4] = AP_MAC_ADDR4;
        macaddr[5] = AP_MAC_ADDR5;
    }
    return 0;
}

int system_parameter_get_ip_config_default (wifi_interface_enum_t if_index, tcpip_ip_info_t *ip_config)
{
    ART_ASSERT(ip_config);
    memset(ip_config, 0, sizeof(tcpip_ip_info_t));
    if(if_index == STATION_IF){
        IP_ADDR4(&ip_config->ip, STA_IP_ADDR0, STA_IP_ADDR1, STA_IP_ADDR2, STA_IP_ADDR3);
        IP_ADDR4(&ip_config->netmask, STA_NETMASK_ADDR0, STA_NETMASK_ADDR1, STA_NETMASK_ADDR2, STA_NETMASK_ADDR3);
        IP_ADDR4(&ip_config->gw, STA_GW_ADDR0, STA_GW_ADDR1, STA_GW_ADDR2, STA_GW_ADDR3);
    }else{
        IP_ADDR4(&ip_config->ip, AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
        IP_ADDR4(&ip_config->netmask, AP_NETMASK_ADDR0, AP_NETMASK_ADDR1, AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
        IP_ADDR4(&ip_config->gw, AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
    }
    return 0;
}

int system_parameter_get_dhcpd_config_default (server_config_t *server_config)
{
    ART_ASSERT(server_config);
    memset(server_config, 0, sizeof(server_config_t));
    ip4_addr_set_u32(&(server_config->server), AP_IP_ADDR0|(AP_IP_ADDR1 << 8)|(AP_IP_ADDR2 << 16)|(AP_IP_ADDR3 << 24));
    server_config->port = DHCPD_LISTEN_PORT;
    server_config->lease = DHCPD_IP_LEASE_TIME;
    server_config->renew = DHCPD_IP_RENEW_TIME;
    ip4_addr_set_u32(&(server_config->ip_start), DHCPD_IP_START);
    ip4_addr_set_u32(&(server_config->ip_end), DHCPD_IP_END);
    server_config->client_max = DHCPD_CLIENT_MAX;
    return 0;
}

int system_parameter_get_hostname_default (wifi_interface_enum_t if_index, char *hostname)
{
    ART_ASSERT(hostname);
    if(if_index == STATION_IF){
        memcpy(hostname, STA_HOSTNAME, strlen(STA_HOSTNAME));
    }else{
        memcpy(hostname, AP_HOSTNAME, strlen(AP_HOSTNAME));
    }
    return 0;
}

