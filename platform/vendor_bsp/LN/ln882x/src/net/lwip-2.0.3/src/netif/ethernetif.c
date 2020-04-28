#include "proj_config.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "netif/ethernetif.h"
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>
#include "netif/etharp.h"
#include "wifi/wifi.h"
#include "dhcpd/dhcpd.h"
#include "utils/debug/art_assert.h"
#include "utils/debug/log.h"
#include "wifi_manager/wifi_manager.h"


#define ETHERNETIF_RX_THR_STACK_SIZE                (1024)
#define ETHERNETIF_RX_THR_PRIO                      (OS_PRIORITY_NORMAL)
#define ETHERNETIF_RX_QUEUE_DEPTH                   (30)

#define TCPIP_PKT_SIZE_MAX    (NETIF_MTU + PBUF_LINK_HLEN)//max in/output pocket size = MTU + LINK_HEAD_LEN

const uint8_t if_name_sta[2] = {'S','T'};
const uint8_t if_name_ap[2]  = {'A','P'};
const uint8_t if_name_monitor[2]  = {'M','O'};

typedef struct {
	struct netif            nif;
    dhcp_mode_enum_t        dhcp_mode;
    tcpip_ip_info_t         ip_info;
	network_state_enum_t	link;
}netdev_t;

typedef struct {
	netdev_t                netdev[WIFI_IF_NUM];   /* 0 -- STA; 1 -- AP; 2 -- MONITOR */
	wifi_interface_enum_t   active;
    OS_Queue_t              netif_rx_queue;
    OS_Thread_t             netif_rx_task;
}ethernetif_t;

typedef struct {
    struct pbuf                 *pkt;
    wifi_interface_enum_t       nif_idx;
} ethernetif_rx_queue_item_t;

ethernetif_t g_ethernetif = {0};

#define netdev2netif(ndev)	((struct netif *)(ndev))
#define netif2netdev(nif)	((netdev_t *)(nif))

static ethernetif_t *ethernetif_get_handle(void)
{
    return &g_ethernetif;
}
static netdev_t *ethernetif_get_netdev(wifi_interface_enum_t nif_idx)
{
    ethernetif_t *eth_if = ethernetif_get_handle();
    netdev_t *netdev = NULL;

    if(nif_idx >= WIFI_IF_NUM){
        LOG(LOG_LVL_ERROR, "nif_idx(%d) invalid!\r\n", nif_idx);
        return NULL;
    }
    netdev = &(eth_if->netdev[nif_idx]);
    return netdev;
}
struct netif *ethernetif_get_netif(wifi_interface_enum_t nif_idx)
{
    netdev_t *netdev = ethernetif_get_netdev(nif_idx);

    if(netdev){
        return &(netdev->nif);
    }

    return (struct netif *)NULL;
}

/**
  * @brief This function should do the actual transmission of the packet. The packet is
  * contained in the pbuf that is passed to the function. This pbuf
  * might be chained.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
  * @return ERR_OK if the packet could be sent
  *         an err_t value if the packet couldn't be sent
  *
  * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
  *       strange results. You might consider waiting for space in the DMA queue
  *       to become available since the stack doesn't retry to send a packet
  *       dropped because of memory failure (except for the TCP timers).
  */
static err_t low_level_output(struct netif *netif, struct pbuf *pkt)
{
    err_enum_t ret = ERR_OK;

    struct pbuf *q = NULL;
    uint8_t * frame_buf  = NULL;
    uint16_t payload_offset = 0;
    
    if(!netif_is_link_up(netif)) {
        LOG(LOG_LVL_INFO, "[%s, %d] netif is not link up.\r\n", __func__, __LINE__);
        ret = ERR_IF;
        return ret;
    }
    
    frame_buf = OS_Malloc(pkt->tot_len);
    
    //Copy the packet payload
    for(q = pkt; q != NULL; q = q->next) {
        memcpy(frame_buf + payload_offset, (u8_t*)q->payload, q->len );
        payload_offset += q->len;
    }

    //Give it to WiFi firmware
    wifi_if_send_ethernet_pkt(frame_buf, pkt->tot_len);
    
    OS_Free(frame_buf);
    return ret;
}

/**
  * @brief Should allocate a pbuf and transfer the bytes of the incoming
  * packet from the interface into the pbuf.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return a pbuf filled with the received packet (including MAC header)
  *         NULL on memory error
  */
void ethernetif_low_level_input(wifi_interface_enum_t nif_idx, uint8_t *data, uint16_t len)
{
    struct pbuf *pkt = NULL, *q = NULL;
    ethernetif_t *eth_if = ethernetif_get_handle();
    ethernetif_rx_queue_item_t item;

    //alloc pbuf
    pkt = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if(pkt == NULL) {
        LOG(LOG_LVL_ERROR, "[%s, %d] Can not malloc pbuff(request len=%d).\r\n", __func__, __LINE__, len);
        return;
    }

    for(q = pkt; q != NULL; q = q->next) {
		memcpy(q->payload, data, q->len);
		data += q->len;
    }

    item.pkt = pkt;
    item.nif_idx = nif_idx;
    OS_QueueSend(&(eth_if->netif_rx_queue), &item, OS_WAIT_FOREVER);
}

/**
  * @brief This function is the ethernetif_input task, it is processed when a packet 
  * is ready to be read from the interface. It uses the function ethernetif_low_level_input() 
  * that should handle the actual reception of bytes from the network
  * interface. Then the type of the received packet is determined and
  * the appropriate input function is called.
  *
  * @param netif the lwip network interface structure for this ethernetif
  */
void ethernetif_input( void *argument )
{
    struct pbuf *p;
    struct netif *nif;
    ethernetif_rx_queue_item_t item;
    ethernetif_t *eth_if = (ethernetif_t *)argument;
  
    for( ;; )
    {
        if(OS_QueueReceive(&(eth_if->netif_rx_queue), &item, OS_WAIT_FOREVER) == OS_OK){
            p = item.pkt;
            nif = ethernetif_get_netif(item.nif_idx);
            if(p != NULL){
                if(nif->input(p, nif) != ERR_OK)
                {
                    pbuf_free(p);
                }
            }
        }
    }
}

static err_t sta_low_level_output(struct netif *nif, struct pbuf *p)
{
    return low_level_output(nif, p);
}
static err_t ap_low_level_output(struct netif *nif, struct pbuf *p)
{
    return low_level_output(nif, p);
}
#if LWIP_IPV4
static err_t ethernetif_null_ip4output(struct netif *nif, struct pbuf *p, const ip4_addr_t *ip4addr)
{
	LOG(LOG_LVL_ERROR, "%s() called\r\n", __func__);
	return ERR_IF;
}
#else
static err_t ethernetif_null_ip6output(struct netif *nif, struct pbuf *p, const ip6_addr_t *ip6addr)
{
	LOG(LOG_LVL_ERROR, "%s() called\r\n", __func__);
	return ERR_IF;
}
#endif

static err_t ethernetif_null_linkoutput(struct netif *nif, struct pbuf *p)
{
	LOG(LOG_LVL_ERROR, "%s() called\r\n", __func__);
	return ERR_IF;
}
static err_t tcpip_null_input(struct pbuf *p, struct netif *nif)
{
	LOG(LOG_LVL_ERROR, "%s() called\r\n", __func__);
	pbuf_free(p);
	return ERR_OK;
}
/**
  * @brief Should be called at the beginning of the program to set up the
  * network interface. It calls the function low_level_init() to do the
  * actual setup of the hardware.
  *
  * This function should be passed as a parameter to netif_add().
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return ERR_OK if the loopif is initialized
  *         ERR_MEM if private data couldn't be allocated
  *         any other err_t on error
  */
static err_t ethernetif_init(struct netif *netif, wifi_interface_enum_t nif_idx)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    if(nif_idx == STATION_IF){
#if LWIP_NETIF_HOSTNAME
        /* Initialize interface hostname */
        netif->hostname = "wifi_sta";
#endif /* LWIP_NETIF_HOSTNAME */
		memcpy(netif->name, if_name_sta, sizeof(netif->name));
#if LWIP_IPV4
        netif->output = etharp_output;
#else
        netif->output_ip6 = ethip6_output;
#endif
        netif->linkoutput = sta_low_level_output;
    }else if(nif_idx == SOFT_AP_IF){
#if LWIP_NETIF_HOSTNAME
        /* Initialize interface hostname */
        netif->hostname = "wifi_ap";
#endif /* LWIP_NETIF_HOSTNAME */
		memcpy(netif->name, if_name_ap, sizeof(netif->name));
#if LWIP_IPV4
        netif->output = etharp_output;
#else
        netif->output_ip6 = ethip6_output;
#endif
        netif->linkoutput = ap_low_level_output;
    }else if(nif_idx == MONITOR_IF){
#if LWIP_NETIF_HOSTNAME
        /* Initialize interface hostname */
        netif->hostname = "wifi_monitor";
#endif /* LWIP_NETIF_HOSTNAME */
		memcpy(netif->name, if_name_monitor, sizeof(netif->name));
#if LWIP_IPV4
        netif->output = ethernetif_null_ip4output;
#else
        netif->output_ip6 = ethernetif_null_ip6output;
#endif
        netif->linkoutput = ethernetif_null_linkoutput;
    }else{
        LOG(LOG_LVL_ERROR, "ethernetif_init: nif_idx input error\n");
        return ERR_ARG;
    }

    /* set netif MAC hardware address length */
    netif->hwaddr_len = NETIF_MAX_HWADDR_LEN;

    /* set netif maximum transfer unit */
    netif->mtu = NETIF_MTU;

    /* Accept broadcast address and ARP traffic */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    return ERR_OK;
}

static err_t ethernetif_sta_init(struct netif *nif)
{
	return ethernetif_init(nif, STATION_IF);
}
static err_t ethernetif_hostap_init(struct netif *nif)
{
	return ethernetif_init(nif, SOFT_AP_IF);
}

static err_t ethernetif_monitor_init(struct netif *nif)
{
	return ethernetif_init(nif, MONITOR_IF);
}

static void post_ip_info(struct netif *nif)
{
    netdev_t *ndev = netif2netdev(nif);
    tcpip_ip_info_t *ip_info = &(ndev->ip_info);
    
    /* post ip, mask and gateway in dhcp mode */
    LOG(LOG_LVL_INFO, "************************************************** \r\n");
	LOG(LOG_LVL_INFO, "netif hostname: %s \r\n", nif->hostname);
    LOG(LOG_LVL_INFO, "netif name: %s \r\n", nif->name);
	if(ndev->dhcp_mode == WLAN_DHCP_CLIENT){
    	LOG(LOG_LVL_INFO, "DHCP mode: %s \r\n", "Client");
	}else if(ndev->dhcp_mode == WLAN_DHCP_SERVER){
		LOG(LOG_LVL_INFO, "DHCP mode: %s \r\n", "Server");
	}else{
		LOG(LOG_LVL_INFO, "DHCP mode: %s \r\n", "Static IP");
	}
    LOG(LOG_LVL_INFO, "ip = %s \r\n", ip4addr_ntoa(&ip_info->ip));
    LOG(LOG_LVL_INFO, "mask = %s \r\n", ip4addr_ntoa(&ip_info->netmask));
    LOG(LOG_LVL_INFO, "gateway = %s \r\n", ip4addr_ntoa(&ip_info->gw));
    LOG(LOG_LVL_INFO, "************************************************** \r\n");

}


static void tcpip_dhcpc_cb(struct netif *nif)
{
    netdev_t *ndev = netif2netdev(nif);
    tcpip_ip_info_t *ip_info = &(ndev->ip_info);
    wifi_msg_t wifi_msg;

    if (!ip4_addr_cmp(ip_2_ip4(&nif->ip_addr), IP4_ADDR_ANY4)) {
        //check whether IP is changed
        if (!ip4_addr_cmp(ip_2_ip4(&nif->ip_addr), &ip_info->ip) ||
                !ip4_addr_cmp(ip_2_ip4(&nif->netmask), &ip_info->netmask) ||
                !ip4_addr_cmp(ip_2_ip4(&nif->gw), &ip_info->gw)) {
            ip4_addr_set(&ip_info->ip, ip_2_ip4(&nif->ip_addr));
            ip4_addr_set(&ip_info->netmask, ip_2_ip4(&nif->netmask));
            ip4_addr_set(&ip_info->gw, ip_2_ip4(&nif->gw));

            /* post the dhcp ip address */
            post_ip_info(nif);
			ndev->link = LINK_UP;

			wifi_msg.msg_id  = WIFI_MSG_ID_STA_DHCP_GOT_IP;
            wifi_msg.msg_len = 0;
            notify_wifi_manager_task(&wifi_msg);
        }
    }
    return;
}


static err_t tcpip_dhcpc_start(struct netif *nif)
{
    if (NULL == nif){
        LOG(LOG_LVL_ERROR, "input nif is NULL \r\n");
        return -1;
    }

    if (dhcp_start(nif) != ERR_OK) {
        LOG(LOG_LVL_ERROR, "dhcp client start failed\r\n");
        return -1;
    }

    netif_set_status_callback(nif, tcpip_dhcpc_cb);
    return 0;
}

static void tcpip_init_done(void *arg)
{
    ethernetif_t *eth_if = ethernetif_get_handle();
    netdev_t *ndev = NULL;
    struct netif *nif = NULL;
    tcpip_ip_info_t *ip_info = NULL;
    wifi_interface_enum_t nif_idx;

    //memset(eth_if, 0, sizeof(ethernetif_t));

    //for STA
    nif_idx = STATION_IF;
    ndev = ethernetif_get_netdev(nif_idx);
	ndev->link = LINK_DOWN;
    ip_info = &(ndev->ip_info);
    nif = ethernetif_get_netif(nif_idx);
#if (LWIP_IPV4)
    netif_add(nif, &(ip_info->ip), &(ip_info->netmask), &(ip_info->gw), NULL, ethernetif_sta_init, tcpip_input);
#else
    netif_add(nif, NULL, ethernetif_sta_init, tcpip_input);
#endif
    netif_set_default(nif);

    //for AP
    nif_idx = SOFT_AP_IF;
    ndev = ethernetif_get_netdev(nif_idx);
	ndev->link = LINK_DOWN;
    ip_info = &(ndev->ip_info);
    nif = ethernetif_get_netif(nif_idx);
#if (LWIP_IPV4)
    netif_add(nif, &(ip_info->ip), &(ip_info->netmask), &(ip_info->gw), NULL, ethernetif_hostap_init, tcpip_input);
#else
    netif_add(nif, NULL, ethernetif_hostap_init, tcpip_input);
#endif

    //for Monitor
    nif_idx = MONITOR_IF;
    ndev = ethernetif_get_netdev(nif_idx);
	ndev->link = LINK_DOWN;
    ip_info = &(ndev->ip_info);
    nif = ethernetif_get_netif(nif_idx);
#if (LWIP_IPV4)
    netif_add(nif, &(ip_info->ip), &(ip_info->netmask), &(ip_info->gw), NULL, ethernetif_monitor_init, tcpip_null_input);
#else
    netif_add(nif, NULL, ethernetif_monitor_init, tcpip_null_input);
#endif

    OS_QueueCreate(&(eth_if->netif_rx_queue), ETHERNETIF_RX_QUEUE_DEPTH, sizeof(ethernetif_rx_queue_item_t));
    /* create the ethernetif_input task that handles the WiFi MAC input */
    OS_ThreadCreate(&eth_if->netif_rx_task, "ethernetif_input", ethernetif_input, eth_if, ETHERNETIF_RX_THR_PRIO, ETHERNETIF_RX_THR_STACK_SIZE);

    LOG(LOG_LVL_INFO, "TCP/IP initialized.\r\n");
}

int lwip_tcpip_init(void)
{
    tcpip_init(tcpip_init_done, NULL);

    return 0;
}

int ethernetif_set_dhcp_mode(wifi_interface_enum_t nif_idx, dhcp_mode_enum_t dhcp_mode)
{
	ethernetif_t *eth_if = ethernetif_get_handle();
    netdev_t *netdev = ethernetif_get_netdev(nif_idx);

	eth_if->active = nif_idx;
    if(netdev->dhcp_mode != dhcp_mode){
        netdev->dhcp_mode = dhcp_mode;
    }
    return 0;
}

dhcp_mode_enum_t ethernetif_get_dhcp_mode(wifi_interface_enum_t nif_idx)
{
    netdev_t *netdev = ethernetif_get_netdev(nif_idx);
    return netdev->dhcp_mode;
}

int ethernetif_set_mac_addr(wifi_interface_enum_t nif_idx, uint8_t *mac_addr)
{
    struct netif *nif = ethernetif_get_netif(nif_idx);

    if(nif && mac_addr){
        memcpy(nif->hwaddr, mac_addr, NETIF_MAX_HWADDR_LEN);
        return 0;
    }
    return -1;
}
int ethernetif_get_mac_addr(wifi_interface_enum_t nif_idx, uint8_t *mac_addr)
{
    struct netif *nif = ethernetif_get_netif(nif_idx);

    if(nif && mac_addr){
        memcpy(mac_addr, nif->hwaddr, NETIF_MAX_HWADDR_LEN);
        return 0;
    }
    return -1;
}

int ethernetif_set_ip_info(wifi_interface_enum_t nif_idx, tcpip_ip_info_t *ip_info)
{
    netdev_t *ndev = ethernetif_get_netdev(nif_idx);

    if(ndev && ip_info){
        memcpy(&(ndev->ip_info), ip_info, sizeof(tcpip_ip_info_t));
        netif_set_addr(netdev2netif(ndev), &ip_info->ip, &ip_info->netmask, &ip_info->gw);
        return 0;
    }
    return -1;
}
int ethernetif_get_ip_info(wifi_interface_enum_t nif_idx, tcpip_ip_info_t *ip_info)
{
    netdev_t *ndev = ethernetif_get_netdev(nif_idx);

    if(ndev && ip_info){
        memcpy(ip_info, &(ndev->ip_info), sizeof(tcpip_ip_info_t));
        return 0;
    }
    return -1;
}

int ethernetif_set_state(wifi_interface_enum_t nif_idx, netif_state_enum_t up)
{
    netdev_t *ndev = NULL;
    struct netif *nif = NULL;
    tcpip_ip_info_t *ip_info = NULL;
	wifi_msg_t wifi_msg;

    if(nif_idx >= WIFI_IF_NUM){
        LOG(LOG_LVL_ERROR, "nif_idx(%d) invalid!\r\n", nif_idx);
        return -1;
    }
    ndev = ethernetif_get_netdev(nif_idx);
    ip_info = &(ndev->ip_info);
    nif = ethernetif_get_netif(nif_idx);

    //for STA
    if(nif_idx == STATION_IF){
        if(up == NETIF_UP){
            netif_set_up(nif);
            netif_set_link_up(nif);
            if(ndev->dhcp_mode == WLAN_DHCP_CLIENT){
                memset(ip_info, 0, sizeof(tcpip_ip_info_t));
                tcpip_dhcpc_start(nif);
            }else{
                netif_set_addr(nif, &ip_info->ip, &ip_info->netmask, &ip_info->gw);
				ndev->link = LINK_UP;
				wifi_msg.msg_id  = WIFI_MSG_ID_STA_USE_STATIC_IP;
		        wifi_msg.msg_len = 0;
		        notify_wifi_manager_task(&wifi_msg);
            }
        }else{
            dhcp_release(nif);
            dhcp_stop(nif);
            dhcp_cleanup(nif);
            netif_set_status_callback(nif, NULL);
            netif_set_link_down(nif);
            netif_set_down(nif);
			ndev->link = LINK_DOWN;
        }
    }else if(nif_idx == SOFT_AP_IF){//for AP
        if(up == NETIF_UP){
            netif_set_up(nif);
            netif_set_link_up(nif);
			ndev->link = LINK_UP;
            dhcpd_start();
        }else{
            dhcpd_stop();
            netif_set_status_callback(nif, NULL);
            netif_set_link_down(nif);
            netif_set_down(nif);
			ndev->link = LINK_DOWN;
        }
    }else if(nif_idx == MONITOR_IF){//for Monitor
        if(up == NETIF_UP){
            netif_set_up(nif);
            netif_set_link_up(nif);
        }else{
            netif_set_status_callback(nif, NULL);
            netif_set_link_down(nif);
            netif_set_down(nif);
			ndev->link = LINK_DOWN;
        }
    }else{
        LOG(LOG_LVL_ERROR, "nif_idx(%d) invalid!\r\n", nif_idx);
    }

    return 0;
}

wifi_interface_enum_t ethernetif_get_active(void)
{
    ethernetif_t *eth_if = ethernetif_get_handle();
    return eth_if->active;
}

network_state_enum_t ethernetif_get_link_state(void)
{
	wifi_interface_enum_t nif_idx = ethernetif_get_active();
	netdev_t *ndev = ethernetif_get_netdev(nif_idx);
	return ndev->link;
}

void ethernetif_remove_station(uint8_t *mac)
{
    dhcpd_ip_release((char *)mac);
}


