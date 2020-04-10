#include "proj_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "utils/system_parameter.h"
#include "dhcpd/dhcpd.h"

#include "utils/debug/log.h"

#define BUFF_SIZE               5
#define DHCPD_TASK_STACK_SIZE   256*7

static dhcpd_ctrl_t g_dhcpd_ctrl = {0,};

static struct dhcp_packet_handler gobal_packet_handler = {
    .do_discover    = &do_discover,
    .do_inform      = &do_inform,
    .do_request     = &do_request,
    .do_release     = &do_release,
    .do_decline     = &do_decline,
};

dhcpd_ctrl_t *dhcpd_get_handle(void)
{
    return &g_dhcpd_ctrl;
}
static void dhcpd_task( void *pvParameters )
{
    int so_reuseaddr = 1, rcv_size = 0;
    struct sockaddr_in server_address;
    struct raw_msg *msg = NULL;
    dhcpd_ctrl_t *dhcpd = (dhcpd_ctrl_t *)pvParameters;
    server_config_t *server_config = &(dhcpd->server_config);
    fd_set read_set;
    fd_set write_set;
    struct timeval timeout;
    
    if((dhcpd->dhcp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG(LOG_LVL_ERROR, "Cannot open the socket!\r\n");
        dhcpd_close_socket(&dhcpd->dhcp_socket);
        return;
    }

    setsockopt(dhcpd->dhcp_socket, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_config->port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(dhcpd->dhcp_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        LOG(LOG_LVL_ERROR, "Cannot bind the socket with the address!\r\n");
        dhcpd_close_socket(&dhcpd->dhcp_socket);
        return;
    }

    socklen_t addr_len = sizeof(struct sockaddr_in);
    msg = &(dhcpd->msg_receive);
    
    while(1) 
    {
        if(!dhcpd->flag){
            break;
        }
        
        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        FD_SET(dhcpd->dhcp_socket, &read_set);
        FD_SET(dhcpd->dhcp_socket, &write_set);

        timeout.tv_sec  = 0;
        timeout.tv_usec = 50;
        
        if(0 < select(dhcpd->dhcp_socket+1, &read_set, &write_set, 0, &timeout))
        {
            if (FD_ISSET(dhcpd->dhcp_socket, &read_set))
            {
                memset(msg, 0, sizeof(struct raw_msg));
                if(NULL == (msg->buff = OS_Malloc(DHCP_MAX_MTU))) {
                    LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
                    continue;
                }

                msg->socket_fd = dhcpd->dhcp_socket;
                rcv_size = recvfrom(dhcpd->dhcp_socket, msg->buff, DHCP_MAX_MTU, 0, (struct sockaddr*)&msg->address, &addr_len);
                LOG(LOG_LVL_DEBUG, "%d bytes received\r\n", rcv_size);
                if(rcv_size < 0) {
                    LOG(LOG_LVL_WARN, "Receive data error!\r\n");
                    continue;
                }

                msg->length = (uint32_t) rcv_size;
                dhcpd_handle_msg(msg);
            }
        }
        OS_MsDelay(100);
    }
    
    dhcpd_close_socket(&dhcpd->dhcp_socket);
    OS_ThreadDelete(&(dhcpd->dhcpd_thr));
    return;
}
static int _dhcpd_start(bool cfg_from_flash)
{
    OS_Status ret = OS_OK;
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    server_config_t * server_config = &(dhcpd->server_config);
    dhcpd_ip_item_t *ip_item = NULL;
    char ip_addr1, ip_addr2, ip_addr3, ip_addr4;
    int i;

    if(cfg_from_flash){
        memset(dhcpd, 0, sizeof(dhcpd_ctrl_t));
        wifi_softap_get_dhcpd_config(server_config, true);
    }

    if(0 == ip4_addr_get_u32(&(server_config->server)) || 0 == server_config->port || 0 == server_config->lease || 0 == server_config->renew) {
        LOG(LOG_LVL_ERROR, "server_config parameter error!\r\n");
        return -1;
    }

    LOG(LOG_LVL_DEBUG, "-------DUMP DHCP SERVER_CONFIG----------\r\n");
    LOG(LOG_LVL_DEBUG, "server_config.server=%d.%d.%d.%d\r\n", ip4_addr1(&(server_config->server)), ip4_addr2(&(server_config->server)), ip4_addr3(&(server_config->server)), ip4_addr4(&(server_config->server)));
    LOG(LOG_LVL_DEBUG, "server_config.port=%d\r\n", server_config->port);
    LOG(LOG_LVL_DEBUG, "server_config.lease=%d\r\n", server_config->lease);
    LOG(LOG_LVL_DEBUG, "server_config.renew=%d\r\n", server_config->renew);
    LOG(LOG_LVL_DEBUG, "server_config.ip_start=%d.%d.%d.%d\r\n", ip4_addr1(&(server_config->ip_start)), ip4_addr2(&(server_config->ip_start)), ip4_addr3(&(server_config->ip_start)), ip4_addr4(&(server_config->ip_start)));
    LOG(LOG_LVL_DEBUG, "server_config.ip_end=%d.%d.%d.%d\r\n", ip4_addr1(&(server_config->ip_end)), ip4_addr2(&(server_config->ip_end)), ip4_addr3(&(server_config->ip_end)), ip4_addr4(&(server_config->ip_end)));
    LOG(LOG_LVL_DEBUG, "server_config.max=%d\r\n", server_config->client_max);
    LOG(LOG_LVL_DEBUG, "-----------------END--------------\r\n");

    for(i = 0; i < MIN(DHCPD_IP_POOL_SIZE, server_config->client_max); i++){
        ip_item = &(dhcpd->ip_pool[i]);
        ip_item->allocted = false;
        ip_addr1 = ip4_addr1(&(server_config->server));
        ip_addr2 = ip4_addr2(&(server_config->server));
        ip_addr3 = ip4_addr3(&(server_config->server));
        ip_addr4 = ip4_addr4(&(server_config->ip_start)) + i;
        ip4_addr_set_u32(&(ip_item->ip_addr), ip_addr1|(ip_addr2 << 8)|(ip_addr3 << 16)|(ip_addr4 << 24));
    }
    dhcpd->flag = true;
    dhcpd->dhcp_socket = -1;
    dhcpd->ip_allocator = dhcpd_ip_allocate;
    ret = OS_ThreadCreate(&(dhcpd->dhcpd_thr), "dhcpd", dhcpd_task, (void *)dhcpd, OS_PRIORITY_BELOW_NORMAL, DHCPD_TASK_STACK_SIZE);
    if (ret != OS_OK) {
        LOG(LOG_LVL_ERROR, "[%s, %d]OS_ThreadCreate dhcpd->dhcpd_thr fail.\r\n", __func__, __LINE__);
        return ret;
    }

    return 0;
}
int dhcpd_start(void)
{
    return _dhcpd_start(true);
}

int dhcpd_stop(void)
{
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    if(dhcpd->flag == true){
        if(dhcpd->dhcp_socket >= 0){
            dhcpd_close_socket(&dhcpd->dhcp_socket);
        }
        OS_ThreadDelete(&dhcpd->dhcpd_thr);
    }
    dhcpd->flag = false;
    return 0;
}

bool dhcpd_is_running(void)
{
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    return dhcpd->flag;
}

void dhcpd_close_socket(int *dhcp_socket)
{

    if(*dhcp_socket >= 0) {
        close(*dhcp_socket);
        *dhcp_socket = -1;
    }
}

void dhcpd_handle_msg(struct raw_msg *msg)
{
    int broadcast_socket = 0;
    int so_broadcast = 1;
    int so_reuseaddr = 1;
    struct sockaddr_in server_address, broadcast = {0};
    struct dhcp_packet *request = NULL, *response = NULL;
    char buffer[DHCP_MAX_MTU] = {0,};
    int length = 0, send_length;
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    server_config_t * server_config = &(dhcpd->server_config);

    if(msg == NULL){
        LOG(LOG_LVL_ERROR, "handle_msg input parameter error!\r\n");
        return;
    }
    request = dhcpd_marshall(msg->buff, msg->length, dhcpd->request_buff);

    if(NULL != request) {
        response = dhcpd_dispatch(request);

        if(NULL != response) {
            if((broadcast_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                LOG(LOG_LVL_ERROR, "Cannot open the socket!\r\n");
                goto err;
            }

            setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
            setsockopt(broadcast_socket, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));

            memset(&server_address, 0, sizeof(server_address));
            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(server_config->port);
            //server_address.sin_addr.s_addr = inet_addr(server_config->server);
            server_address.sin_addr.s_addr = ip4_addr_get_u32(&server_config->server);

            if(bind(broadcast_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
                LOG(LOG_LVL_ERROR, "Cannot bind the socket with the address!\r\n");
                goto err;
            }

            length = dhcpd_serialize(response, buffer, DHCP_MAX_MTU);


            broadcast.sin_family = AF_INET;
            broadcast.sin_port = htons(BOOTP_REPLAY_PORT);
            broadcast.sin_addr.s_addr = htonl(INADDR_BROADCAST);

            send_length = sendto(broadcast_socket, buffer, length, 0, (struct sockaddr*)&broadcast, sizeof(broadcast));
            if(send_length < 0) {
                LOG(LOG_LVL_ERROR, "Send data error!\r\n");
            } else {
                LOG(LOG_LVL_DEBUG, "Total %d bytes send!\r\n", send_length);
            }
err:
            if(broadcast_socket >= 0) {
                close(broadcast_socket);
            }
        } else {
            LOG(LOG_LVL_DEBUG, "Response packet is NULL.\r\n");
        }

        dhcpd_packet_free(request);
    } else {
        LOG(LOG_LVL_WARN, "Can not marshall request packet from raw bytes.\r\n");
    }
    
    if(msg->buff != NULL) {
        OS_Free(msg->buff);
    }
}

struct dhcp_packet* dhcpd_dispatch(struct dhcp_packet *request)
{
    char type = '\0';
    struct dhcp_option *option = NULL;
    struct dhcp_packet *response = NULL;

    if(NULL == request) {
        LOG(LOG_LVL_ERROR, "Request packet is NULL\r\n");
        return NULL;
    }

    if(BOOT_REQUEST != request->op) {
        LOG(LOG_LVL_WARN, "Packet is not send from dhcp client, ignor!\r\n");
        return NULL;
    }

    //get the dhcp packet type
    option = request->options;
    while(NULL != option) {
        if(DHO_DHCP_MESSAGE_TYPE == option->code) {
            type = *option->value;
            break;
        }

        option = option->next;
    }

    if('\0' == type) {
        LOG(LOG_LVL_ERROR, "No dhcp message type found in the packet!\r\n");
        return NULL;
    }
    LOG(LOG_LVL_DEBUG, "packet type=%d\r\n", type);
    switch(type) {
        case DHCP_DISCOVER:
            response = gobal_packet_handler.do_discover(request);
            break;
        case DHCP_RELEASE:
            response = gobal_packet_handler.do_release(request);
            break;
        case DHCP_INFORM:
            response = gobal_packet_handler.do_inform(request);
            break;
        case DHCP_REQUEST:
            response = gobal_packet_handler.do_request(request);
            break;
        case DHCP_DECLINE:
            response = gobal_packet_handler.do_decline(request);
            break;
        default:
            break;
    }

    return response;
}

struct dhcp_packet *do_discover(struct dhcp_packet *request)
{
    struct network_config config = {0,};
    struct dhcp_packet *response;
    struct dhcp_option *packet_type;
    struct dhcp_option *server_identifier;
    struct dhcp_option *lease_time;
    struct dhcp_option *renew_time;
    struct dhcp_option *router;
    struct dhcp_option *subnet_mask;
    struct dhcp_option *dns_server;
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    server_config_t * server_config = &(dhcpd->server_config);
    char *response_base = dhcpd->response_buff;
    int offset = 0;

    if(NULL == request) {
        LOG(LOG_LVL_ERROR, "Request packet is NULL\r\n");
        return NULL;
    }

    memcpy(config.hardware_address, request->chaddr, 16);

    if(dhcpd->ip_allocator == NULL){
        LOG(LOG_LVL_ERROR, "dhcpd->ip_allocator == NULL!\r\n");
        return NULL;
    }
    if(((dhcpd->ip_allocator)(dhcpd, &config, false)) < 0){
        LOG(LOG_LVL_ERROR, "Cannot assign IP address!\r\n");
        return NULL;
    }

    //response = (struct dhcp_packet*)malloc(sizeof(struct dhcp_packet));
    response = (struct dhcp_packet*)&(response_base[offset]);
    if(NULL == response) {
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_packet);
    memset(response, 0, sizeof(struct dhcp_packet));

    response->op = BOOT_REPLY;
    response->htype = request->htype;
    response->hlen = request->hlen;
    response->hops = 1;
    memcpy(response->xid, request->xid, 4);
    response->yiaddr[0] = ip4_addr1(&(config.ip_address));
    response->yiaddr[1] = ip4_addr2(&(config.ip_address));
    response->yiaddr[2] = ip4_addr3(&(config.ip_address));
    response->yiaddr[3] = ip4_addr4(&(config.ip_address));
    memcpy(response->flags, request->flags, 2);
    memcpy(response->chaddr, request->chaddr, 16);

    //options
    //message type
    //packet_type = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    packet_type = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == packet_type ) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(packet_type, 0, sizeof(struct dhcp_option));
    packet_type->code = DHO_DHCP_MESSAGE_TYPE;
    //packet_type->value = (char *)malloc(1);
    packet_type->value = (char *)&(response_base[offset]);
    if(NULL == packet_type->value && response != NULL) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 1;
    *packet_type->value = DHCP_OFFER;
    packet_type->length = 1;
    response->options = packet_type;

    //server identifier
    //server_identifier = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    server_identifier = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == server_identifier) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(server_identifier, 0, sizeof(struct dhcp_option));
    server_identifier->code = DHO_DHCP_SERVER_IDENTIFIER;
    //server_identifier->value = (char *)malloc(sizeof(ip4_addr_t));
    server_identifier->value = (char *)&(response_base[offset]);
    if(NULL == server_identifier->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(ip4_addr_t);
    ip4_addr_set_u32((ip4_addr_t *)(server_identifier->value), ip4_addr_get_u32(&(server_config->server)));
    server_identifier->length = sizeof(ip4_addr_t);
    packet_type->next = server_identifier;

    //lease time
    //lease_time = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    lease_time = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == lease_time) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(lease_time, 0, sizeof(struct dhcp_option));
    lease_time->code = DHO_DHCP_LEASE_TIME;
    //lease_time->value = (char *)malloc(4);
    lease_time->value = (char *)&(response_base[offset]);
    if(NULL == lease_time->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 4;
    memcpy(lease_time->value, &server_config->lease, 4);
    lease_time->length = 4;
    server_identifier->next = lease_time;

    //renew time
    //renew_time = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    renew_time = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == renew_time) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(renew_time, 0, sizeof(struct dhcp_option));
    renew_time->code = DHO_DHCP_RENEWAL_TIME;
    //renew_time->value = (char *)malloc(4);
    renew_time->value = (char *)&(response_base[offset]);
    if(NULL == renew_time->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 4;
    memcpy(renew_time->value, &server_config->renew, 4);
    renew_time->length = 4;
    lease_time->next = renew_time;

    //router/gateway
    //router = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    router = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == router) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(router, 0, sizeof(struct dhcp_option));
    router->code = DHO_ROUTERS;
    //router->value = (char *)malloc(sizeof(ip4_addr_t));
    router->value = (char *)&(response_base[offset]);
    if(NULL == router->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(ip4_addr_t);
    memcpy((ip4_addr_t *)router->value, &(config.router), sizeof(ip4_addr_t));
    router->length = sizeof(ip4_addr_t);
    renew_time->next = router;

    //netmask
    //subnet_mask = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    subnet_mask = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == subnet_mask) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(subnet_mask, 0, sizeof(struct dhcp_option));
    subnet_mask->code = DHO_SUBNET;
    //subnet_mask->value = (char *)malloc(sizeof(ip4_addr_t));
    subnet_mask->value = (char *)&(response_base[offset]);
    if(NULL == subnet_mask->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(ip4_addr_t);
    memcpy((ip4_addr_t *)subnet_mask->value, &(config.netmask), sizeof(ip4_addr_t));
    subnet_mask->length = sizeof(ip4_addr_t);
    router->next = subnet_mask;

    //dns
    //dns_server = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    dns_server = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == dns_server) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(dns_server, 0, sizeof(struct dhcp_option));
    dns_server->code = DHO_DOMAIN_NAME_SERVERS;
    //dns_server->value = (char *)malloc(2*sizeof(ip4_addr_t));
    dns_server->value = (char *)&(response_base[offset]);
    if(NULL == dns_server->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 2*sizeof(ip4_addr_t);
    memcpy((ip4_addr_t *)(dns_server->value), &(config.dns1), sizeof(ip4_addr_t));
    memcpy((ip4_addr_t *)(dns_server->value + sizeof(ip4_addr_t)), &(config.dns2), sizeof(ip4_addr_t));
    dns_server->length = 2*sizeof(ip4_addr_t);
    subnet_mask->next = dns_server;

    return response;
}

struct dhcp_packet *do_request(struct dhcp_packet *request)
{
    struct network_config config = {0,};
    char type = DHCP_ACK;
    char requested_address[4] = {0,};
    struct dhcp_option *option;
    struct dhcp_packet *response;
    struct dhcp_option *packet_type;
    struct dhcp_option *server_identifier;
    struct dhcp_option *lease_time;
    struct dhcp_option *renew_time;
    struct dhcp_option *router;
    struct dhcp_option *subnet_mask;
    struct dhcp_option *dns_server;
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    server_config_t * server_config = &(dhcpd->server_config);
    char *response_base = dhcpd->response_buff;
    int offset = 0;

    memcpy(config.hardware_address, request->chaddr, 16);

    if(dhcpd->ip_allocator == NULL){
        LOG(LOG_LVL_ERROR, "dhcpd->ip_allocator == NULL!\r\n");
        return NULL;
    }
    if(((dhcpd->ip_allocator)(dhcpd, &config, false)) < 0){
        LOG(LOG_LVL_ERROR, "Cannot assign IP address!\r\n");
        return NULL;
    }

    if(0 != memcmp(requested_address, request->ciaddr, 4)) {
        LOG(LOG_LVL_DEBUG, "request->ciaddr is not 0, copy it to request_address\r\n");
        memcpy(requested_address, request->ciaddr, 4);
    } else {
        LOG(LOG_LVL_DEBUG, "request->ciaddr is 0, get request_address from dhcp option\r\n");
        option = request->options;
        while(NULL != option) {
            if(DHO_DHCP_REQUESTED_ADDRESS == option->code && option->length >= 4) {
                memcpy(requested_address, option->value, 4);
                break;
            }

            option = option->next;
        }
    }

    if(0 != memcmp(&(config.ip_address), requested_address, 4)) {
        LOG(LOG_LVL_DEBUG, "request_address is not the same as IP assigned, change packet type to NAK\r\n");
        type = DHCP_NAK;
        memset(requested_address, 0, 4);
    }else{
        if(((dhcpd->ip_allocator)(dhcpd, &config, true)) < 0){
            LOG(LOG_LVL_ERROR, "Cannot assign IP address!\r\n");
            return NULL;
        }
        memcpy(requested_address, &(config.ip_address), 4);
    }

    //response = (struct dhcp_packet*)malloc(sizeof(struct dhcp_packet));
    response = (struct dhcp_packet*)&(response_base[offset]);
    offset += sizeof(struct dhcp_packet);
    memset(response, 0, sizeof(struct dhcp_packet));

    response->op = BOOT_REPLY;
    response->htype = request->htype;
    response->hlen = request->hlen;
    //response->hops = 1;
    response->hops = request->hops;
    memcpy(response->xid, request->xid, 4);
    memcpy(response->yiaddr, requested_address, 4);
    memcpy(response->flags, request->flags, 2);
    memcpy(response->chaddr, request->chaddr, 16);

    if(DHCP_ACK == type) {
        //options
        //message type
        //packet_type = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
        packet_type = (struct dhcp_option*)&(response_base[offset]);
        if(NULL == packet_type) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += sizeof(struct dhcp_option);
        memset(packet_type, 0, sizeof(struct dhcp_option));
        packet_type->code = DHO_DHCP_MESSAGE_TYPE;
        //packet_type->value = (char *)malloc(1);
        packet_type->value = (char *)&(response_base[offset]);
        if(NULL == packet_type->value) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += 1;
        *packet_type->value = type;
        packet_type->length = 1;
        response->options = packet_type;

        //server identifier
        //server_identifier = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
        server_identifier = (struct dhcp_option*)&(response_base[offset]);
        if(NULL == server_identifier) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += sizeof(struct dhcp_option);
        memset(server_identifier, 0, sizeof(struct dhcp_option));
        server_identifier->code = DHO_DHCP_SERVER_IDENTIFIER;
        //server_identifier->value = (char *)malloc(4);
        server_identifier->value = (char *)&(response_base[offset]);
        if(NULL == server_identifier->value) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += 4;
        memcpy(server_identifier->value, &(server_config->server), sizeof(ip4_addr_t));
        server_identifier->length = 4;
        packet_type->next = server_identifier;

        //lease time
        //lease_time = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
        lease_time = (struct dhcp_option*)&(response_base[offset]);
        if(NULL == lease_time) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += sizeof(struct dhcp_option);
        memset(lease_time, 0, sizeof(struct dhcp_option));
        lease_time->code = DHO_DHCP_LEASE_TIME;
        //lease_time->value = (char *)malloc(4);
        lease_time->value = (char *)&(response_base[offset]);
        if(NULL == lease_time->value) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += 4;
        memcpy(lease_time->value, &server_config->lease, 4);
        lease_time->length = 4;
        server_identifier->next = lease_time;

        //renew time
        //renew_time = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
        renew_time = (struct dhcp_option*)&(response_base[offset]);
        if(NULL == renew_time) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += sizeof(struct dhcp_option);
        memset(renew_time, 0, sizeof(struct dhcp_option));
        renew_time->code = DHO_DHCP_RENEWAL_TIME;
        //renew_time->value = (char *)malloc(4);
        renew_time->value = (char *)&(response_base[offset]);
        if(NULL == renew_time->value) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += 4;
        memcpy(renew_time->value, &server_config->renew, 4);
        renew_time->length = 4;
        lease_time->next = renew_time;

        //router/gateway
        //router = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
        router = (struct dhcp_option*)&(response_base[offset]);
        if(NULL == router) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += sizeof(struct dhcp_option);
        memset(router, 0, sizeof(struct dhcp_option));
        router->code = DHO_ROUTERS;
        //router->value = (char *)malloc(4);
        router->value = (char *)&(response_base[offset]);
        if(NULL == router->value) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += 4;
        memcpy(router->value, &(config.router), 4);
        router->length = 4;
        renew_time->next = router;

        //netmask
        //subnet_mask = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
        subnet_mask = (struct dhcp_option*)&(response_base[offset]);
        if(NULL == subnet_mask) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += sizeof(struct dhcp_option);
        memset(subnet_mask, 0, sizeof(struct dhcp_option));
        subnet_mask->code = DHO_SUBNET;
        //subnet_mask->value = (char *)malloc(4);
        subnet_mask->value = (char *)&(response_base[offset]);
        if(NULL == subnet_mask->value) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += 4;
        memcpy(subnet_mask->value, &(config.netmask), 4);
        subnet_mask->length = 4;
        router->next = subnet_mask;

        //dns
        //dns_server = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
        dns_server = (struct dhcp_option*)&(response_base[offset]);
        if(NULL == dns_server) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += sizeof(struct dhcp_option);
        memset(dns_server, 0, sizeof(struct dhcp_option));
        dns_server->code = DHO_DOMAIN_NAME_SERVERS;
        //dns_server->value = (char *)malloc(8);
        dns_server->value = (char *)&(response_base[offset]);
        if(NULL == dns_server->value) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += 8;
        memcpy(dns_server->value, &(config.dns1), 4);
        memcpy(dns_server->value + 4, &(config.dns2), 4);
        dns_server->length = 8;
        subnet_mask->next = dns_server;
    }else{
        //options
        //message type
        //packet_type = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
        packet_type = (struct dhcp_option*)&(response_base[offset]);
        if(NULL == packet_type) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += sizeof(struct dhcp_option);
        memset(packet_type, 0, sizeof(struct dhcp_option));
        packet_type->code = DHO_DHCP_MESSAGE_TYPE;
        //packet_type->value = (char *)malloc(1);
        packet_type->value = (char *)&(response_base[offset]);
        if(NULL == packet_type->value) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += 1;
        *packet_type->value = type;
        packet_type->length = 1;
        response->options = packet_type;

        //server identifier
        //server_identifier = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
        server_identifier = (struct dhcp_option*)&(response_base[offset]);
        if(NULL == server_identifier) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += sizeof(struct dhcp_option);
        memset(server_identifier, 0, sizeof(struct dhcp_option));
        server_identifier->code = DHO_DHCP_SERVER_IDENTIFIER;
        //server_identifier->value = (char *)malloc(4);
        server_identifier->value = (char *)&(response_base[offset]);
        if(NULL == server_identifier->value) {
            dhcpd_packet_free(response);
            LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
            return NULL;
        }
        offset += 4;
        memcpy(server_identifier->value, &(server_config->server), 4);
        server_identifier->length = 4;
        packet_type->next = server_identifier;

    }
    return response;
}

struct dhcp_packet *do_release(struct dhcp_packet *request)
{
    LOG(LOG_LVL_DEBUG, "[%s, %d]\r\n", __func__, __LINE__);
    return request;
}

struct dhcp_packet* do_inform(struct dhcp_packet *request)
{
    struct network_config config = {0};
    struct dhcp_option *packet_type;
    struct dhcp_packet *response;
    struct dhcp_option *server_identifier;
    struct dhcp_option *lease_time;
    struct dhcp_option *renew_time;
    struct dhcp_option *router;
    struct dhcp_option *subnet_mask;
    struct dhcp_option *dns_server;
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    server_config_t * server_config = &(dhcpd->server_config);
    char *response_base = dhcpd->response_buff;
    int offset = 0;

    memcpy(config.hardware_address, request->chaddr, 16);

    if(dhcpd->ip_allocator == NULL){
        LOG(LOG_LVL_ERROR, "dhcpd->ip_allocator == NULL!\r\n");
        return NULL;
    }
    if(((dhcpd->ip_allocator)(dhcpd, &config, false)) < 0){
        LOG(LOG_LVL_ERROR, "Cannot assign IP address!\r\n");
        return NULL;
    }

    //response = (struct dhcp_packet*)malloc(sizeof(struct dhcp_packet));
    response = (struct dhcp_packet*)&(response_base[offset]);
    offset += sizeof(struct dhcp_packet);
    memset(response, 0, sizeof(struct dhcp_packet));

    response->op = BOOT_REPLY;
    response->htype = request->htype;
    response->hlen = request->hlen;
    response->hops = 1;
    memcpy(response->xid, request->xid, 4);
    memcpy(response->yiaddr, &(config.ip_address), 4);
    memcpy(response->flags, request->flags, 2);
    memcpy(response->chaddr, request->chaddr, 16);

    //options
    //message type
    //packet_type = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    packet_type = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == packet_type) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(packet_type, 0, sizeof(struct dhcp_option));
    packet_type->code = DHO_DHCP_MESSAGE_TYPE;
    //packet_type->value = (char *)malloc(1);
    packet_type->value = (char *)&(response_base[offset]);
    if(NULL == packet_type->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 1;
    *packet_type->value = DHCP_ACK;
    packet_type->length = 1;
    response->options = packet_type;

    //server identifier
    //server_identifier = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    server_identifier = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == server_identifier) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(server_identifier, 0, sizeof(struct dhcp_option));
    server_identifier->code = DHO_DHCP_SERVER_IDENTIFIER;
    //server_identifier->value = (char *)malloc(4);
    server_identifier->value = (char *)&(response_base[offset]);
    if(NULL == server_identifier->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 4;
    memcpy(server_identifier->value, &(server_config->server), 4);
    server_identifier->length = 4;
    packet_type->next = server_identifier;

    //lease time
    //lease_time = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    lease_time = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == lease_time) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(lease_time, 0, sizeof(struct dhcp_option));
    lease_time->code = DHO_DHCP_LEASE_TIME;
    //lease_time->value = (char *)malloc(4);
    lease_time->value = (char *)&(response_base[offset]);
    if(NULL == lease_time->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 4;
    memcpy(lease_time->value, &server_config->lease, 4);
    lease_time->length = 4;
    server_identifier->next = lease_time;

    //renew time
    //renew_time = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    renew_time = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == renew_time) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(renew_time, 0, sizeof(struct dhcp_option));
    renew_time->code = DHO_DHCP_RENEWAL_TIME;
    //renew_time->value = (char *)malloc(4);
    renew_time->value = (char *)&(response_base[offset]);
    if(NULL == renew_time->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 4;
    memcpy(renew_time->value, &server_config->renew, 4);
    renew_time->length = 4;
    lease_time->next = renew_time;

    //router/gateway
    //router = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    router = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == router) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(router, 0, sizeof(struct dhcp_option));
    router->code = DHO_ROUTERS;
    //router->value = (char *)malloc(4);
    router->value = (char *)&(response_base[offset]);
    if(NULL == router->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 4;
    memcpy(router->value, &(config.router), 4);
    router->length = 4;
    renew_time->next = router;

    //netmask
    //subnet_mask = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    subnet_mask = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == subnet_mask) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(subnet_mask, 0, sizeof(struct dhcp_option));
    subnet_mask->code = DHO_SUBNET;
    //subnet_mask->value = (char *)malloc(4);
    subnet_mask->value = (char *)&(response_base[offset]);
    if(NULL == subnet_mask->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 4;
    memcpy(subnet_mask->value, &(config.netmask), 4);
    subnet_mask->length = 4;
    router->next = subnet_mask;

    //dns
    //dns_server = (struct dhcp_option*)malloc(sizeof(struct dhcp_option));
    dns_server = (struct dhcp_option*)&(response_base[offset]);
    if(NULL == dns_server) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += sizeof(struct dhcp_option);
    memset(dns_server, 0, sizeof(struct dhcp_option));
    dns_server->code = DHO_DOMAIN_NAME_SERVERS;
    //dns_server->value = (char *)malloc(8);
    dns_server->value = (char *)&(response_base[offset]);
    if(NULL == dns_server->value) {
        dhcpd_packet_free(response);
        LOG(LOG_LVL_ERROR, "[%s, %d]Allocate memory failed!\r\n", __func__, __LINE__);
        return NULL;
    }
    offset += 8;
    memcpy(dns_server->value, &(config.dns1), 4);
    memcpy(dns_server->value + 4, &(config.dns2), 4);
    dns_server->length = 8;
    subnet_mask->next = dns_server;
    return response;
}

struct dhcp_packet *do_decline(struct dhcp_packet *request)
{
    LOG(LOG_LVL_INFO, "[%s, %d]\r\n", __func__, __LINE__);
    return NULL;
}

bool wifi_softap_set_dhcpd_config(server_config_t *server_config, bool write_flash)
{
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();

    if(write_flash){
        system_parameter_set_dhcpd_config(server_config);
    }
    memcpy(&(dhcpd->server_config), server_config, sizeof(server_config_t));
    if(dhcpd_is_running()){
        dhcpd_stop();
        _dhcpd_start(false);
    }
    return true;
}
bool wifi_softap_get_dhcpd_config(server_config_t *server_config, bool from_flash)
{
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();

    if(from_flash){
        system_parameter_get_dhcpd_config(server_config);
    }else{
        memcpy(server_config, &(dhcpd->server_config), sizeof(server_config_t));
    }
    return true;
}

bool wifi_softap_dhcpd_config_reset(bool cfg_from_flash)
{
    server_config_t dhcp_server_config;
    dhcpd_ctrl_t *dhcpd = dhcpd_get_handle();
    server_config_t *server_config = &(dhcpd->server_config);

    system_parameter_get_dhcpd_config_default(&dhcp_server_config);

    if(cfg_from_flash){
        system_parameter_set_dhcpd_config(&dhcp_server_config);
    }
    memcpy(server_config, &dhcp_server_config, sizeof(server_config_t));
    if(dhcpd_is_running()){
        dhcpd_stop();
        _dhcpd_start(false);
    }

    return true;
}

