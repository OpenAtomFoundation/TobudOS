#include "ping/ping.h"
#include "osal/osal.h"

#include "lwip/opt.h"

#define PING_SENDER_ENABLE 1

#if (PING_SENDER_ENABLE && LWIP_RAW)
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/inet_chksum.h"
#include "netif/ethernetif.h"

#include "utils/debug/log.h"
#include "utils/debug/art_assert.h"

#if PING_USE_SOCKETS
#include "lwip/sockets.h"
#include "lwip/inet.h"
#endif /* PING_USE_SOCKETS */


/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LOG_LVL_INFO
#endif

/** ping receive timeout - in milliseconds */
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 2000
#endif

/** ping delay - in milliseconds */
#ifndef PING_DELAY
#define PING_DELAY     1000
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 1400
#endif

/** ping result action - no default action */
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

/* ping variables */
static u16_t ping_seq_num;
static u32_t ping_time;
//TaskHandle_t ping_sender_task_handle;
OS_Thread_t g_ping_sender_thread;

/** Prepare a echo ICMP request */
static void
ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for(i = 0; i < data_len; i++) {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

/* Ping using the socket ip */
static err_t
ping_send(int s, ip_addr_t *addr)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
    LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

    iecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
    if (!iecho) {
        return ERR_MEM;
    }

    ping_prepare_echo(iecho, (u16_t)ping_size);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
    inet_addr_from_ip4addr(&to.sin_addr, addr);

    err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));

    mem_free(iecho);

    return (err ? ERR_OK : ERR_VAL);
}

#define ping_printf(format, ...)     log_printf(format, ##__VA_ARGS__)
static void ping_recv(int s)
{
    char buf[64];
    int fromlen = sizeof(struct sockaddr_in), len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;

    while(1)
    {
        if((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0)
        {
            if (len >= (int)(sizeof(struct ip_hdr)+sizeof(struct icmp_echo_hdr))) 
            {
                ip_addr_t fromaddr;
                inet_addr_to_ip4addr(&fromaddr, &from.sin_addr);
                LOG(PING_DEBUG, "ping recv: %s  %"U32_F" ms.\r\n", ipaddr_ntoa(&fromaddr), (OS_GetTicks() - ping_time));

                ping_printf("+%"U32_F"ms\r\n", (OS_GetTicks() - ping_time));                
                ping_printf("OK\r\n");

                iphdr = (struct ip_hdr *)buf;
                iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
                if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) 
                {
                    /* do some ping result processing */
                    PING_RESULT((ICMPH_TYPE(iecho) == ICMP_ER));
                    return;
                }
                else
                {
                    if(PING_RCV_TIMEO > (OS_GetTicks() - ping_time)){
                        LOG(PING_DEBUG, "ping: recive icmp packet mismatch\r\n");
                    } else {
                        return;
                    }
                }
            }
        }
        else
        {
            LOG(PING_DEBUG, "ping: recv - %"U32_F" ms - timeout\n", (OS_GetTicks() - ping_time));
            ping_printf("+timeout\r\n");
            ping_printf("ERROR\r\n");
            return;
        }        
    }

    /* do some ping result processing */
    PING_RESULT(0);
}

static OS_Thread_t g_ping_thread;
#define USR_PING_TASK_STACK_SIZE  4*256 //Byte    
uint8_t ping_status = 0;

void
ping_sender(void *arg)
{
    int s;

    struct timeval timeout = { (PING_RCV_TIMEO / OS_MSEC_PER_SEC), (PING_RCV_TIMEO % OS_MSEC_PER_SEC)};

    ip_addr_t target_ip;
    uint32_t  pind_times;
    uint32_t  pind_interval;
    struct netif *netif = ethernetif_get_netif(ethernetif_get_active());
    
    if(! ip4addr_aton(((ping_param_t *)arg)->host, &target_ip)) {
        ping_status = 0;
        OS_ThreadDelete(&g_ping_thread);
        //ping stop
    }

    pind_times    = ((ping_param_t *)arg)->cnt;
    pind_interval = ((ping_param_t *)arg)->interval_ms;
    
    if ((s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) {
        OS_ThreadDelete(&g_ping_thread);
    }

    struct sockaddr name;
    socklen_t namelen = sizeof(struct sockaddr);
    name.sa_len       = sizeof(struct sockaddr);
    name.sa_family    = AF_INET;
    name.sa_data[0] = 0;
    name.sa_data[1] = 0;
    name.sa_data[2] = (uint8_t)(netif->ip_addr.addr);
    name.sa_data[3] = (uint8_t)(netif->ip_addr.addr >> 8 );
    name.sa_data[4] = (uint8_t)(netif->ip_addr.addr >> 16);
    name.sa_data[5] = (uint8_t)(netif->ip_addr.addr >> 24);

    lwip_bind(s, &name, namelen);
    
    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    while (1) 
    {
        if(!netif_is_link_up(netif))
        {
            OS_MsDelay(1000);
            continue;
        }
        
        if (ping_send(s, &target_ip) == ERR_OK) 
        {
            ping_time = OS_GetTicks();
            LOG(PING_DEBUG, "ping send: %s success.\r\n", ipaddr_ntoa(&target_ip));
            ping_recv(s);
        }
        else 
        {
            LOG(PING_DEBUG, "ping send: %s error.\r\n", ipaddr_ntoa(&target_ip));
        }
        pind_times--;
        
        if(pind_times < 1)
        {
            lwip_close(s);
            ping_status = 0;
            OS_ThreadDelete(&g_ping_thread);
        }

        OS_MsDelay(pind_interval);
    }
}

bool creat_ping_task(ping_param_t * ping_param)
{
    static ping_param_t m_ping_param; //must be static param
    static char host_str[64];
    
    if(ping_status){
        return false;
    }
    
    memset(host_str, 0, sizeof(host_str));
    memcpy(host_str, ping_param->host, (strlen(ping_param->host) > sizeof(host_str) ? sizeof(host_str) : strlen(ping_param->host)));
    
    m_ping_param.interval_ms = ping_param->interval_ms;
    m_ping_param.cnt         = ping_param->cnt;
    m_ping_param.host        = host_str;
    
    if(OS_OK != OS_ThreadCreate(&g_ping_thread, "ping", ping_sender, (void *)&m_ping_param, OS_PRIORITY_NORMAL, USR_PING_TASK_STACK_SIZE))
    {
        return false;
    }
    ping_status = 1;
    return true;
}

#endif /* PING_SENDER_ENABLE */

