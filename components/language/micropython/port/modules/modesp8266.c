/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 KY-zhang-X
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mpconfig.h"

#if MICROPY_PY_NETWORK && MICROPY_PY_NETWORK_ESP8266

#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "extmod/modnetwork.h"
#include "machine_uart.h"

#if !(MP_GEN_HDR)
#include "tos_k.h"
#include "tos_at_socket.h"
#include "sal_module_wrapper.h"
#include "esp8266.h"
#endif

#define MAKE_SOCKADDR(addr, ip, port) \
    struct sockaddr addr; \
    addr.sa_family = AF_INET; \
    addr.sa_data[0] = port >> 8; \
    addr.sa_data[1] = port; \
    addr.sa_data[2] = ip[0]; \
    addr.sa_data[3] = ip[1]; \
    addr.sa_data[4] = ip[2]; \
    addr.sa_data[5] = ip[3];

#define UNPACK_SOCKADDR(addr, ip, port) \
    port = (addr.sa_data[0] << 8) | addr.sa_data[1]; \
    ip[0] = addr.sa_data[2]; \
    ip[1] = addr.sa_data[3]; \
    ip[2] = addr.sa_data[4]; \
    ip[3] = addr.sa_data[5];

STATIC int esp8266_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *ip_out) {
    char ip_str[16];
    if (0 != tos_sal_module_parse_domain(name, ip_str, sizeof(ip_str))) {
        return -2;
    }
    int ret = sscanf(ip_str, "%hhu.%hhu.%hhu.%hhu", 
        &ip_out[0], &ip_out[1], &ip_out[2], &ip_out[3]);
    if (ret != 4) {
        return -2;
    }
    return 0;
}

STATIC int socket_socket(struct _mod_network_socket_obj_t *skt, int *_errno) {
    // only support IPv4
    if (skt->domain != MOD_NETWORK_AF_INET) {
        *_errno = MP_EAFNOSUPPORT;
        return -1;
    }
    int domain = AF_INET;
    
    int type;
    switch (skt->type) {
        case MOD_NETWORK_SOCK_STREAM:
            type = SOCK_STREAM;
            break;
        case MOD_NETWORK_SOCK_DGRAM:
            type = SOCK_DGRAM;
            break;
        case MOD_NETWORK_SOCK_RAW:
            // SOCK_RAW is not supported yet
        default:
            *_errno = MP_EINVAL;
            return -1;            
    }

    // open socket
    int fd = socket(domain, type, 0);
    if (fd < 0) {
        // FIXME: no more detailed errno
        *_errno = MP_EMFILE;
        return -1;
    }

    // store state of this socket
    skt->fileno = fd;

    return 0;
}

STATIC void socket_close(struct _mod_network_socket_obj_t *socket) {
    close(socket->fileno);
}

STATIC int socket_bind(struct _mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    // FIXME: not support
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int socket_listen(struct _mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    // FIXME: not support
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int socket_accept(struct _mod_network_socket_obj_t *socket, struct _mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
    // FIXME: not support
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int socket_connect(struct _mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port);
    int ret = connect(socket->fileno, &addr, sizeof(addr));
    if (ret != 0) {
        // FIXME: no more detailed errno
        *_errno = MP_ETIMEDOUT;
        return -1;
    }
    return 0;
}

STATIC mp_uint_t socket_send(struct _mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    int ret = send(socket->fileno, buf, len, 0);
    if (ret < 0) {
        // FIXME: no more detailed errno
        *_errno = MP_EPERM;
        return (mp_uint_t)-1;
    }
    return ret;
}

STATIC mp_uint_t socket_recv(struct _mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    int ret = recv(socket->fileno, buf, len, 0);
    if (ret < 0) {
        // FIXME: no more detailed errno
        *_errno = MP_EPERM;
        return (mp_uint_t)-1;
    }
    return ret;
}

STATIC mp_uint_t socket_sendto(struct _mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port);
    int ret = sendto(socket->fileno, buf, len, 0, &addr, sizeof(addr));
    if (ret < 0) {
        // FIXME: no more detailed errno
        *_errno = MP_EPERM;
        return (mp_uint_t)-1;
    }
    return ret;
}

STATIC mp_uint_t socket_recvfrom(struct _mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    int ret = recvfrom(socket->fileno, buf, len, 0, &addr, &addr_len);
    if (ret < 0) {
        // FIXME
        *_errno = MP_EPERM;
        return (mp_uint_t)-1;
    }
    UNPACK_SOCKADDR(addr, ip, *port);
    return ret;
}

STATIC int socket_setsockopt(struct _mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    // FIXME not support
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int socket_settimeout(struct _mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
    // FIXME
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int socket_ioctl(struct _mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
    // FIXME
    if (request == MP_STREAM_POLL) {
        int ret = 0;
        if ((arg & MP_STREAM_POLL_RD) && 1) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((arg & MP_STREAM_POLL_WR) && 1) {
            ret |= MP_STREAM_POLL_WR;
        }
        return ret;
    } else {
        *_errno = MP_EINVAL;
        return (int)MP_STREAM_ERROR;
    }
}

/********************************************/
typedef struct _esp8266_obj_t {
    mp_obj_base_t base;
    const machine_uart_obj_t *uart;
    uint8_t init;
} esp8266_obj_t;

STATIC esp8266_obj_t esp8266_obj = {
    {(mp_obj_type_t *)&mod_network_nic_type_esp8266}, 
    .uart = NULL,
    .init = 0,
};

extern at_agent_t esp8266_agent;
STATIC mp_obj_t esp8266_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    if (!esp8266_obj.init) {
        machine_uart_obj_t *uart = MP_OBJ_TO_PTR(args[0]);
        machine_uart_set_at_agent(uart, &esp8266_agent);
        machine_uart_rx_start(uart);
        if (0 != esp8266_sal_init(machine_uart_get_port(uart))) {
				    esp8266_sal_deinit();
            machine_uart_set_at_agent(uart, NULL);
            mp_raise_msg(&mp_type_OSError, "can't setup esp8266");
        }

        esp8266_obj.uart = uart;
        esp8266_obj.init = 1;
    }

    mod_network_register_nic(MP_OBJ_FROM_PTR(&esp8266_obj));

    return MP_OBJ_FROM_PTR(&esp8266_obj);
}

STATIC mp_obj_t esp8266_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_pwd };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_pwd, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get ssid
    size_t ssid_len;
    const char *ssid = mp_obj_str_get_data(args[ARG_ssid].u_obj, &ssid_len);

    // get pwd
    size_t pwd_len = 0;
    const char *pwd = "";
    if (args[ARG_pwd].u_obj != mp_const_none) {
        pwd = mp_obj_str_get_data(args[ARG_pwd].u_obj, &pwd_len);
    }

    // connect to AP
    if (esp8266_join_ap(ssid, pwd) != 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("could not connect to ssid=%s, pwd=%s\n"), ssid, pwd);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(esp8266_connect_obj, 1, esp8266_connect);

STATIC const mp_rom_map_elem_t esp8266_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&esp8266_connect_obj) },
};
STATIC MP_DEFINE_CONST_DICT(esp8266_locals_dict, esp8266_locals_dict_table);

const mod_network_nic_type_t mod_network_nic_type_esp8266 = {
    .base = {
        { &mp_type_type },
        .name = MP_QSTR_ESP8266,
        .make_new = esp8266_make_new,
        .locals_dict = (mp_obj_dict_t *)&esp8266_locals_dict,
    },
    .gethostbyname = esp8266_gethostbyname,
    .socket = socket_socket,
    .close = socket_close,
    .bind = socket_bind,
    .listen = socket_listen,
    .accept = socket_accept,
    .connect = socket_connect,
    .send = socket_send,
    .recv = socket_recv,
    .sendto = socket_sendto,
    .recvfrom = socket_recvfrom,
    .setsockopt = socket_setsockopt,
    .settimeout = socket_settimeout,
    .ioctl = socket_ioctl,
};

#endif /* MICROPY_PY_NETWORK && MICROPY_PY_NETWORK_ESP8266*/
