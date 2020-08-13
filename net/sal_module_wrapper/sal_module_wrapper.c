#include "sal_module_wrapper.h"

static sal_module_t *cur_sal_module = NULL;
static k_list_t sal_module_device_list;

int tos_sal_module_register(sal_module_t *module)
{
    if (!module) {
        return -1;
    }
    
    if (sal_module_device_list.next == NULL || sal_module_device_list.prev == NULL) {\
        tos_list_init(&sal_module_device_list);
    }

    tos_list_add_tail(&module->sal_module_list, &sal_module_device_list);    

    return 0;
}

int tos_sal_module_unregister(sal_module_t *module)
{
    if (!module) {
        return -1;
    }

    tos_list_del(&module->sal_module_list);    

    return 0;
}

int tos_sal_module_set_status(sal_module_t *module, sal_device_status_t status)
{
    if (module) {
        module->sal_module_status = status;
        return 0;
    }

    return -1;
}

void tos_sal_module_set_local_ip(sal_module_t *module, char* ip)
{
    if (module && ip) {
        strncpy(module->sal_module_ip, ip, sizeof(module->sal_module_ip));
    }

    return;
}

int tos_sal_module_set_default(char *sal_module_device_name)
{
    k_list_t     *cur = NULL;
    sal_module_t *sal_module = NULL;

    TOS_LIST_FOR_EACH(cur, &sal_module_device_list) {
        sal_module = TOS_LIST_ENTRY(cur, sal_module_t, sal_module_list);
        if (sal_module == NULL) {
            continue;
        }
        else {
            if (strcmp(sal_module->sal_module_name, sal_module_device_name) == 0) {
//                tos_list_del(cur);
//                tos_list_add(cur, &sal_module_device_list);
                cur_sal_module = sal_module;
                return 0;
            }
        }
    }

    return -1;
}

int tos_sal_module_get_type(void)
{
    if (cur_sal_module) {
        return cur_sal_module->sal_module_type;
    }

    return -1;
}

// int tos_sal_module_init(void)
// {
//     if (cur_sal_module && cur_sal_module->init) {
//         return cur_sal_module->init();
//     }
//     return -1;
// }

int tos_sal_module_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    if (cur_sal_module && cur_sal_module->parse_domain) {
        return cur_sal_module->parse_domain(host_name, host_ip, host_ip_len);
    }
    return -1;
}

int tos_sal_module_connect(const char *ip, const char *port, sal_proto_t proto)
{
    if (cur_sal_module && cur_sal_module->connect) {
        return cur_sal_module->connect(ip, port, proto);
    }
    return -1;
}

int tos_sal_module_send(int sock, const void *buf, size_t len)
{
    if (cur_sal_module && cur_sal_module->send) {
        return cur_sal_module->send(sock, buf, len);
    }
    return -1;
}

int tos_sal_module_recv(int sock, void *buf, size_t len)
{
    if (cur_sal_module && cur_sal_module->recv) {
        return cur_sal_module->recv(sock, buf, len);
    }
    return -1;
}

int tos_sal_module_recv_timeout(int sock, void *buf, size_t len, uint32_t timeout)
{
    if (cur_sal_module && cur_sal_module->recv_timeout) {
        return cur_sal_module->recv_timeout(sock, buf, len, timeout);
    }
    return -1;
}

int tos_sal_module_sendto(int sock, char *ip, char *port, const void *buf, size_t len)
{
    if (cur_sal_module && cur_sal_module->sendto) {
        return cur_sal_module->sendto(sock, ip, port, buf, len);
    }
    return -1;
}

int tos_sal_module_recvfrom(int sock, void *buf, size_t len)
{
    if (cur_sal_module && cur_sal_module->recvfrom) {
        return cur_sal_module->recvfrom(sock, buf, len);
    }
    return -1;
}

int tos_sal_module_recvfrom_timeout(int sock, void *buf, size_t len, uint32_t timeout)
{
    if (cur_sal_module && cur_sal_module->recvfrom_timeout) {
        return cur_sal_module->recvfrom_timeout(sock, buf, len, timeout);
    }
    return -1;
}

int tos_sal_module_close(int sock)
{
    if (cur_sal_module && cur_sal_module->close) {
        return cur_sal_module->close(sock);
    }
    return -1;
}

