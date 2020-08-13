/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _SAL_MODULE_WRAPPER_H_
#define  _SAL_MODULE_WRAPPER_H_

#include <stdint.h>
#include <stdio.h>
#include "tos_k.h"

#define SAL_MODULE_DEVICE_NAME_MAX  10

typedef enum sal_protocol_en {
    TOS_SAL_PROTO_TCP,
    TOS_SAL_PROTO_UDP,
} sal_proto_t;

typedef enum sal_device_status_en {
    TOS_SAL_MODULE_AVAILABLE,
    TOS_SAL_MODULE_UNAVAILABLE,
} sal_device_status_t;

typedef enum sal_device_type_en {
    TOS_SAL_MODULE_GPRS,
    TOS_SAL_MODULE_WIFI,
    TOS_SAL_MODULE_4G_LTE,
    TOS_SAL_MODULE_4G_CAT1,
    TOS_SAL_MODULE_NB_IOT,
} sal_device_type_t;

typedef struct sal_module_st {
    char                    sal_module_name[SAL_MODULE_DEVICE_NAME_MAX];

    char                    sal_module_ip[15];

    k_list_t                sal_module_list;

    sal_device_type_t       sal_module_type;

    sal_device_status_t     sal_module_status;
    
    int (*init)(void);

    int (*get_local_mac)(char *mac);

    int (*get_local_ip)(char *ip, char *gw, char *mask);

    int (*parse_domain)(const char *host_name, char *host_ip, size_t host_ip_len);

    int (*connect)(const char *ip, const char *port, sal_proto_t proto);

    int (*send)(int sock, const void *buf, size_t len);

    int (*recv_timeout)(int sock, void *buf, size_t len, uint32_t timeout);

    int (*recv)(int sock, void *buf, size_t len);

    int (*sendto)(int sock, char *ip, char *port, const void *buf, size_t len);

    int (*recvfrom)(int sock, void *buf, size_t len);

    int (*recvfrom_timeout)(int sock, void *buf, size_t len, uint32_t timeout);

    int (*close)(int sock);
} sal_module_t;

/**
 * @brief Register a sal module.
 *
 * @attention None
 *
 * @param[in]   module      network module
 *
 * @return  errcode
 */
int tos_sal_module_register(sal_module_t *module);

int tos_sal_module_unregister(sal_module_t *module);

/**
 * @brief Register a default sal module.
 *
 * @attention None
 *
 * @param[in] None
 *
 * @return  errcode
 */


int tos_sal_module_set_status(sal_module_t *module, sal_device_status_t status);

void tos_sal_module_set_local_ip(sal_module_t *module, char* ip);

int tos_sal_module_set_default(char *sal_module_device_name);

int tos_sal_module_get_type(void);

// /**
//  * @brief Initialize the module.
//  *
//  * @attention None
//  *
//  * @return  errcode
//  */
// int tos_sal_module_init(void);

/**
 * @brief Convert domain to ip address.
 *
 * @attention None
 *
 * @param[in]   host_name   domain name of the host
 * @param[out]  host_ip     ip address of the host
 * @param[out]  host_ip_len ip address buffer length
 *
 * @return  errcode
 */
int tos_sal_module_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len);

/**
 * @brief Connect to remote host.
 *
 * @attention None
 *
 * @param[in]   ip      ip address of the remote host
 * @param[in]   port    port number of the remote host
 * @param[in]   proto   protocol of the connection(TCP/UDP)
 *
 * @return  socket id if succuss, -1 if failed.
 */
int tos_sal_module_connect(const char *ip, const char *port, sal_proto_t proto);

/**
 * @brief Send data to the remote host(TCP).
 *
 * @attention None
 *
 * @param[in]   sock    socket id
 * @param[in]   buf     data to send
 * @param[in]   len     data length
 *
 * @return  data length sent
 */
int tos_sal_module_send(int sock, const void *buf, size_t len);

/**
 * @brief Receive data from the remote host(TCP).
 *
 * @attention None
 *
 * @param[in]   sock    socket id
 * @param[in]   buf     data buffer to hold the data received
 * @param[in]   len     data buffer length
 *
 * @return  data length received
 */
int tos_sal_module_recv(int sock, void *buf, size_t len);

/**
 * @brief Receive data from the remote host(TCP).
 *
 * @attention None
 *
 * @param[in]   sock    socket id
 * @param[in]   buf     data buffer to hold the data received
 * @param[in]   len     data buffer length
 * @param[in]   timeout timeout
 *
 * @return  data length received
 */
int tos_sal_module_recv_timeout(int sock, void *buf, size_t len, uint32_t timeout);

/**
 * @brief Send data to the remote host(UDP).
 *
 * @attention None
 *
 * @param[in]   sock    socket id
 * @param[in]   ip      ip address of the remote host
 * @param[in]   port    port number of the remote host
 * @param[in]   buf     data to send
 * @param[in]   len     data length
 *
 * @return  data length sent
 */
int tos_sal_module_sendto(int sock, char *ip, char *port, const void *buf, size_t len);

/**
 * @brief Receive data from the remote host(UDP).
 *
 * @attention None
 *
 * @param[in]   sock    socket id
 * @param[in]   buf     data buffer to hold the data received
 * @param[in]   len     data buffer length
 *
 * @return  data length received
 */
int tos_sal_module_recvfrom(int sock, void *buf, size_t len);

/**
 * @brief Receive data from the remote host(UDP).
 *
 * @attention None
 *
 * @param[in]   sock    socket id
 * @param[in]   buf     data buffer to hold the data received
 * @param[in]   len     data buffer length
 * @param[in]   timeout timeout
 *
 * @return  data length received
 */
int tos_sal_module_recvfrom_timeout(int sock, void *buf, size_t len, uint32_t timeout);

/**
 * @brief Close the connection.
 *
 * @attention None
 *
 * @param[in]   sock    socket id
 *
 * @return  errcode
 */
int tos_sal_module_close(int sock);

#endif

