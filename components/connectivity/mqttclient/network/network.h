/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:02
 * @LastEditTime : 2020-01-14 03:38:32
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "mqtt_config.h"

#if MQTT_NETWORK_TYPE_TLS
typedef struct network_ssl_params {
    const char		            *ca_crt;
    size_t 		                ca_crt_len;
#if defined(MBEDTLS_FS_IO)
    const char                  *cert_file;            // public certificate file
    const char                  *key_file;             // pravite certificate file
#else
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    const char                  *psk;                  // PSK string
    const char                  *psk_id;               // PSK ID
    size_t                      psk_length;            // PSK length
#endif
#endif
    unsigned int                timeout_ms;            // SSL handshake timeout in millisecond
} network_ssl_params_t;
#endif /* MQTT_NETWORK_TYPE_TLS */

typedef struct network_params {
    char                        *addr;
    char                        *port;
#if MQTT_NETWORK_TYPE_TLS
    network_ssl_params_t        network_ssl_params;
    void                        *nettype_tls_params;
#endif /* MQTT_NETWORK_TYPE_TLS */
} network_params_t;

typedef struct network {
    int                     socket;
    network_params_t        network_params;
    int                     (*connect)(struct network *);
    void                    (*disconnect)(struct network *);
    int                     (*read)(struct network *, unsigned char *, int, int);
    int                     (*write)(struct network *, unsigned char *, int, int);
} network_t;

int network_init(network_t* n, network_params_t* network_params);
int network_read(network_t* n, unsigned char* buf, int len, int timeout);
int network_write(network_t* n, unsigned char* buf, int len, int timeout);
int network_connect(network_t* n);
void network_release(network_t* n);

#endif
