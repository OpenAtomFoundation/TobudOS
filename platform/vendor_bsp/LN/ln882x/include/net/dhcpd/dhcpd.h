#ifndef __DHCPD_H__
#define __DHCPD_H__

#include <stdint.h>
#include "lwip/sockets.h"
#include "lwip/ip4_addr.h"
#include "dhcpd/dhcp_packet.h"

#define BOOTP_REPLAY_PORT    68
#define DHCPD_IP_POOL_SIZE   3

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

typedef struct server_config {
    ip4_addr_t          server;
    uint16_t            port;
    uint32_t            lease;
    uint32_t            renew;
    ip4_addr_t          ip_start;
    ip4_addr_t          ip_end;
    uint8_t             client_max;
}server_config_t;

typedef struct raw_msg {
    char       *buff;
    uint32_t    length;
    struct sockaddr_in address;
    int socket_fd;
}raw_msg_t;

typedef struct network_config {
    char            hardware_address[6];
    ip4_addr_t      ip_address;
    ip4_addr_t      router;
    ip4_addr_t      netmask;
    ip4_addr_t      dns1;
    ip4_addr_t      dns2;
}network_config_t;

struct dhcpd_ctrl;
typedef struct dhcpd_ctrl dhcpd_ctrl_t;

typedef int (*dhcpd_ip_allocator_t)(dhcpd_ctrl_t *, network_config_t *, bool pre_allocated);

typedef struct{
    bool                allocted;
    char                sta_id[6];
    ip4_addr_t          ip_addr;
}dhcpd_ip_item_t;

struct dhcpd_ctrl{
    OS_Thread_t                 dhcpd_thr;
    int                         dhcp_socket;
    bool                        flag;
    server_config_t             server_config;
    dhcpd_ip_allocator_t        ip_allocator;
    dhcpd_ip_item_t             ip_pool[DHCPD_IP_POOL_SIZE];
    struct raw_msg              msg_receive;
    char                        request_buff[DHCP_MAX_MTU];
    char                        response_buff[DHCP_MAX_MTU];
};
struct dhcp_packet *do_discover(struct dhcp_packet *request);

struct dhcp_packet *do_request(struct dhcp_packet *request);

struct dhcp_packet *do_release(struct dhcp_packet *request);

struct dhcp_packet *do_inform(struct dhcp_packet *request);

struct dhcp_packet *do_decline(struct dhcp_packet *request);

struct dhcp_packet_handler {
    struct dhcp_packet *(*do_discover)(struct dhcp_packet *);
    struct dhcp_packet *(*do_inform)(struct dhcp_packet *);
    struct dhcp_packet *(*do_request)(struct dhcp_packet *);
    struct dhcp_packet *(*do_release)(struct dhcp_packet *);
    struct dhcp_packet *(*do_decline)(struct dhcp_packet *);
};


dhcpd_ctrl_t *dhcpd_get_handle(void);
int dhcpd_start(void);
int dhcpd_stop(void);
bool dhcpd_is_running(void);
void dhcpd_handle_msg(struct raw_msg *msg);
struct dhcp_packet *dhcpd_dispatch(struct dhcp_packet *request);
int dhcpd_ip_allocate(dhcpd_ctrl_t *dhcpd, network_config_t *network_config, bool pre_allocated);
int dhcpd_ip_release(char *sta_id);
void dhcpd_close_socket(int *dhcp_socket);
bool wifi_softap_set_dhcpd_config(server_config_t *server_config, bool write_flash);
bool wifi_softap_get_dhcpd_config(server_config_t *server_config, bool from_flash);
bool wifi_softap_dhcpd_config_reset(bool cfg_from_flash);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif
