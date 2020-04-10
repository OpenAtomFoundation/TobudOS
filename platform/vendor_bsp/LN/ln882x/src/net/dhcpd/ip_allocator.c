#include "proj_config.h"

#include <stdio.h>
#include <string.h>
#include "dhcpd/dhcpd.h"
#include "utils/debug/log.h"

static bool mac_are_all_zero(char *mac)
{
    int i;
    for(i = 0; i < 6; i++){
        if(*(mac + i)){
            return false;
        }
    }
    return true;
}

static dhcpd_ip_item_t *get_next_usable_ip_item(dhcpd_ip_item_t *ip_pool, char *sta_addr)
{
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    server_config_t * server_config = &(dhcpd->server_config);
    int i, usable = -1, usable2 = -1;
    dhcpd_ip_item_t *ip_item;
    for(i = 0; i < MIN(DHCPD_IP_POOL_SIZE, server_config->client_max); i++){
        ip_item = &(ip_pool[i]);
        LOG(LOG_LVL_TRACE, "%s i=%d, mac="MACSTR", ip=%d.%d.%d.%d, allocted=%d\r\n", __func__, i, MAC2STR(ip_item->sta_id), ip4_addr1(&ip_item->ip_addr), ip4_addr2(&ip_item->ip_addr), ip4_addr3(&ip_item->ip_addr), ip4_addr4(&ip_item->ip_addr), ip_item->allocted);
        if(!memcmp(ip_item->sta_id, sta_addr, 6)){
            LOG(LOG_LVL_TRACE, "Got i=%d, mac="MACSTR", ip=%d.%d.%d.%d, allocted=%d\r\n", i, MAC2STR(ip_item->sta_id), ip4_addr1(&ip_item->ip_addr), ip4_addr2(&ip_item->ip_addr), ip4_addr3(&ip_item->ip_addr), ip4_addr4(&ip_item->ip_addr), ip_item->allocted);
            return ip_item;
        }
        if((usable < 0) && mac_are_all_zero(ip_item->sta_id) &&  ip_item->allocted == false){
            usable = i;
            LOG(LOG_LVL_TRACE, "Got i=%d, mac="MACSTR", ip=%d.%d.%d.%d, allocted=%d\r\n", i, MAC2STR(ip_item->sta_id), ip4_addr1(&ip_item->ip_addr), ip4_addr2(&ip_item->ip_addr), ip4_addr3(&ip_item->ip_addr), ip4_addr4(&ip_item->ip_addr), ip_item->allocted);
        }
        if((usable2 < 0) &&  ip_item->allocted == false){
            usable2 = i;
            LOG(LOG_LVL_TRACE, "Got i=%d, mac="MACSTR", ip=%d.%d.%d.%d, allocted=%d\r\n", i, MAC2STR(ip_item->sta_id), ip4_addr1(&ip_item->ip_addr), ip4_addr2(&ip_item->ip_addr), ip4_addr3(&ip_item->ip_addr), ip4_addr4(&ip_item->ip_addr), ip_item->allocted);
        }
    }
    if((usable < 0) && (usable2 >= 0)){
        usable = usable2;
    }
    if((usable >= 0) && usable < MIN(DHCPD_IP_POOL_SIZE, server_config->client_max)){
        return &(ip_pool[usable]);
    }
    return NULL;
}
static dhcpd_ip_item_t *get_ip_item(dhcpd_ip_item_t *ip_pool, char *sta_addr)
{
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    server_config_t * server_config = &(dhcpd->server_config);
    int i;
    dhcpd_ip_item_t *ip_item;
    for(i = 0; i < MIN(DHCPD_IP_POOL_SIZE, server_config->client_max); i++){
        ip_item = &(ip_pool[i]);
        if(!memcmp(ip_item->sta_id, sta_addr, 6)){
            return ip_item;
        }
    }
    return NULL;
}

static int set_ip_item_status(dhcpd_ip_item_t *ip_item, bool st)
{
    ip_item->allocted = st;
    return 0;
}    
static int simple_ip_allocator(dhcpd_ctrl_t *dhcpd, struct network_config *config, bool pre_allocated)
{
    ip4_addr_t asc_gateway, asc_netmask, asc_dns1, asc_dns2, asc_ip_address;
    dhcpd_ip_item_t *ip_item = NULL;
    
    LOG(LOG_LVL_TRACE, "[%s, %d]hw="MACSTR"\r\n", __func__, __LINE__, MAC2STR(config->hardware_address));

    ip_item = get_next_usable_ip_item(dhcpd->ip_pool, config->hardware_address);
    if(NULL == ip_item){
        LOG(LOG_LVL_ERROR, "[%s, %d]NULL == ip_item\r\n", __func__, __LINE__);
        return -1;
    }
    memcpy(ip_item->sta_id, config->hardware_address, 6);
    ip4_addr_set_u32(&asc_ip_address, ip4_addr_get_u32(&(ip_item->ip_addr)));
    ip4_addr_set_u32(&asc_gateway, (1<<24)|(ip4_addr3(&asc_ip_address)<<16)|(ip4_addr2(&asc_ip_address)<<8)|ip4_addr1(&asc_ip_address));
    ip4_addr_set_u32(&asc_netmask, (0<<24)|(255<<16)|(255<<8)|255);
    ip4_addr_set_u32(&asc_dns1, (8<<24)|(8<<16)|(8<<8)|8);
    ip4_addr_set_u32(&asc_dns2, (4<<24)|(4<<16)|(4<<8)|4);
    if(pre_allocated && ip_item->allocted == false){
        set_ip_item_status(ip_item, true);
    }

    LOG(LOG_LVL_TRACE, "ip_addr=%d.%d.%d.%d\r\n", ip4_addr1(&asc_ip_address), ip4_addr2(&asc_ip_address), ip4_addr3(&asc_ip_address), ip4_addr4(&asc_ip_address));
	LOG(LOG_LVL_TRACE, "gateway=%d.%d.%d.%d\r\n", ip4_addr1(&asc_gateway), ip4_addr2(&asc_gateway), ip4_addr3(&asc_gateway), ip4_addr4(&asc_gateway));
    LOG(LOG_LVL_TRACE, "netmask=%d.%d.%d.%d\r\n", ip4_addr1(&asc_netmask), ip4_addr2(&asc_netmask), ip4_addr3(&asc_netmask), ip4_addr4(&asc_netmask));
    LOG(LOG_LVL_TRACE, "dns1=%d.%d.%d.%d\r\n", ip4_addr1(&asc_dns1), ip4_addr2(&asc_dns1), ip4_addr3(&asc_dns1), ip4_addr4(&asc_dns1));
    LOG(LOG_LVL_TRACE, "dns2=%d.%d.%d.%d\r\n", ip4_addr1(&asc_dns2), ip4_addr2(&asc_dns2), ip4_addr3(&asc_dns2), ip4_addr4(&asc_dns2));
    
	memcpy(&(config->ip_address), &asc_ip_address, sizeof(ip4_addr_t));
    memcpy(&(config->router), &asc_gateway, sizeof(ip4_addr_t));
    memcpy(&(config->netmask), &asc_netmask, sizeof(ip4_addr_t));
    memcpy(&(config->dns1), &asc_netmask, sizeof(ip4_addr_t));
    memcpy(&(config->dns2), &asc_netmask, sizeof(ip4_addr_t));

	return 0;
}
static int simple_ip_release(dhcpd_ctrl_t *dhcpd, char *sta_id)
{
    dhcpd_ip_item_t *ip_item = NULL;
    LOG(LOG_LVL_TRACE, "[%s, %d]sta_id="MACSTR"\r\n", __func__, __LINE__, MAC2STR(sta_id));
    ip_item = get_ip_item(dhcpd->ip_pool, sta_id);
    set_ip_item_status(ip_item, false);
    return 0;
}

int dhcpd_ip_allocate(dhcpd_ctrl_t *dhcpd, network_config_t *network_config, bool pre_allocated)
{
    return simple_ip_allocator(dhcpd, network_config, pre_allocated);
}
int dhcpd_ip_release(char *sta_id)
{
    return simple_ip_release(dhcpd_get_handle(), sta_id);
}

