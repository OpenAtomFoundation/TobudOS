#include "sal_module_wrapper.h"

static sal_module_t *g_sal_module = NULL;

int tos_sal_module_register(sal_module_t *module)
{
    if (!g_sal_module) {
        g_sal_module = module;
        return 0;
    }

    return -1;
}

int tos_sal_module_init(void)
{
    if (g_sal_module && g_sal_module->init) {
        return g_sal_module->init();
    }
    return -1;
}

int tos_sal_module_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    if (g_sal_module && g_sal_module->parse_domain) {
        return g_sal_module->parse_domain(host_name, host_ip, host_ip_len);
    }
    return -1;
}

int tos_sal_module_connect(const char *ip, const char *port, sal_proto_t proto)
{
    if (g_sal_module && g_sal_module->connect) {
        return g_sal_module->connect(ip, port, proto);
    }
    return -1;
}

int tos_sal_module_send(int sock, const void *buf, size_t len)
{
    if (g_sal_module && g_sal_module->send) {
        return g_sal_module->send(sock, buf, len);
    }
    return -1;
}

int tos_sal_module_recv(int sock, void *buf, size_t len)
{
    if (g_sal_module && g_sal_module->recv) {
        return g_sal_module->recv(sock, buf, len);
    }
    return -1;
}

int tos_sal_module_recv_timeout(int sock, void *buf, size_t len, uint32_t timeout)
{
    if (g_sal_module && g_sal_module->recv_timeout) {
        return g_sal_module->recv_timeout(sock, buf, len, timeout);
    }
    return -1;
}

int tos_sal_module_sendto(int sock, char *ip, char *port, const void *buf, size_t len)
{
    if (g_sal_module && g_sal_module->sendto) {
        return g_sal_module->sendto(sock, ip, port, buf, len);
    }
    return -1;
}

int tos_sal_module_recvfrom(int sock, void *buf, size_t len)
{
    if (g_sal_module && g_sal_module->recvfrom) {
        return g_sal_module->recvfrom(sock, buf, len);
    }
    return -1;
}

int tos_sal_module_recvfrom_timeout(int sock, void *buf, size_t len, uint32_t timeout)
{
    if (g_sal_module && g_sal_module->recvfrom_timeout) {
        return g_sal_module->recvfrom_timeout(sock, buf, len, timeout);
    }
    return -1;
}

int tos_sal_module_close(int sock)
{
    if (g_sal_module && g_sal_module->close) {
        return g_sal_module->close(sock);
    }
    return -1;
}

