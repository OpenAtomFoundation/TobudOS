/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-11 19:45:35
 * @LastEditTime: 2020-09-20 14:29:06
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "nettype_tls.h"
#include "platform_net_socket.h"
#include "platform_memory.h"
#include "platform_timer.h"
#include "random.h"

#ifndef MQTT_NETWORK_TYPE_NO_TLS

#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"

#if defined(MBEDTLS_X509_CRT_PARSE_C)
static int server_certificate_verify(void *hostname, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    if (0 != *flags)
        MQTT_LOG_E("%s:%d %s()... server_certificate_verify failed returned 0x%04x\n", __FILE__, __LINE__, __FUNCTION__, *flags);
    return *flags;
}
#endif

static int nettype_tls_entropy_source(void *data, uint8_t *output, size_t len, size_t *out_len)
{
    uint32_t seed;
    (void) data;
    seed = random_number();

    if (len > sizeof(seed)) {
        len = sizeof(seed);
    }

    memcpy(output, &seed, len);
    *out_len = len;

    return 0;
}

static int nettype_tls_init(network_t* n, nettype_tls_params_t* nettype_tls_params)
{
    int rc = MQTT_SUCCESS_ERROR;
    
    mbedtls_platform_set_calloc_free(platform_memory_calloc, platform_memory_free);
    
    mbedtls_net_init(&(nettype_tls_params->socket_fd));
    mbedtls_ssl_init(&(nettype_tls_params->ssl));
    mbedtls_ssl_config_init(&(nettype_tls_params->ssl_conf));
    mbedtls_ctr_drbg_init(&(nettype_tls_params->ctr_drbg));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_init(&(nettype_tls_params->ca_cert));
    mbedtls_x509_crt_init(&(nettype_tls_params->client_cert));
    mbedtls_pk_init(&(nettype_tls_params->private_key));
#endif

    mbedtls_entropy_init(&(nettype_tls_params->entropy));
    mbedtls_entropy_add_source(&(nettype_tls_params->entropy), nettype_tls_entropy_source, NULL, MBEDTLS_ENTROPY_MAX_GATHER, MBEDTLS_ENTROPY_SOURCE_STRONG);

    if ((rc = mbedtls_ctr_drbg_seed(&(nettype_tls_params->ctr_drbg), mbedtls_entropy_func,
                                    &(nettype_tls_params->entropy), NULL, 0)) != 0) {
        MQTT_LOG_E("mbedtls_ctr_drbg_seed failed returned 0x%04x", (rc < 0 )? -rc : rc);
        RETURN_ERROR(rc);
    }

    if ((rc = mbedtls_ssl_config_defaults(&(nettype_tls_params->ssl_conf), MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        MQTT_LOG_E("mbedtls_ssl_config_defaults failed returned 0x%04x", (rc < 0 )? -rc : rc);
        RETURN_ERROR(rc);
    }

    mbedtls_ssl_conf_rng(&(nettype_tls_params->ssl_conf), mbedtls_ctr_drbg_random, &(nettype_tls_params->ctr_drbg));

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (NULL != n->ca_crt) {
        n->ca_crt_len = strlen(n->ca_crt);

        if (0 != (rc = (mbedtls_x509_crt_parse(&(nettype_tls_params->ca_cert), (unsigned char *)n->ca_crt,
                                          (n->ca_crt_len + 1))))) {
            MQTT_LOG_E("%s:%d %s()... parse ca crt failed returned 0x%04x", __FILE__, __LINE__, __FUNCTION__, (rc < 0 )? -rc : rc);
            RETURN_ERROR(rc);
        }
    }

    mbedtls_ssl_conf_ca_chain(&(nettype_tls_params->ssl_conf), &(nettype_tls_params->ca_cert), NULL);

    if ((rc = mbedtls_ssl_conf_own_cert(&(nettype_tls_params->ssl_conf),
                                         &(nettype_tls_params->client_cert), &(nettype_tls_params->private_key))) != 0) {
        MQTT_LOG_E("%s:%d %s()... mbedtls_ssl_conf_own_cert failed returned 0x%04x", __FILE__, __LINE__, __FUNCTION__, (rc < 0 )? -rc : rc);
        RETURN_ERROR(rc);
    }
    
    mbedtls_ssl_conf_verify(&(nettype_tls_params->ssl_conf), server_certificate_verify, (void *)n->host);

    mbedtls_ssl_conf_authmode(&(nettype_tls_params->ssl_conf), MBEDTLS_SSL_VERIFY_REQUIRED);
#endif

    mbedtls_ssl_conf_read_timeout(&(nettype_tls_params->ssl_conf), n->timeout_ms);

    if ((rc = mbedtls_ssl_setup(&(nettype_tls_params->ssl), &(nettype_tls_params->ssl_conf))) != 0) {
        MQTT_LOG_E("mbedtls_ssl_setup failed returned 0x%04x", (rc < 0 )? -rc : rc);
        RETURN_ERROR(rc);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if ((rc = mbedtls_ssl_set_hostname(&(nettype_tls_params->ssl), n->host)) != 0) {
        MQTT_LOG_E("%s:%d %s()... mbedtls_ssl_set_hostname failed returned 0x%04x", __FILE__, __LINE__, __FUNCTION__, (rc < 0 )? -rc : rc);
        RETURN_ERROR(rc);
    }
#endif

    mbedtls_ssl_set_bio(&(nettype_tls_params->ssl), &(nettype_tls_params->socket_fd), mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}


int nettype_tls_connect(network_t* n)
{
    int rc;
    if (NULL == n)
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);
    
    nettype_tls_params_t *nettype_tls_params = (nettype_tls_params_t *) platform_memory_alloc(sizeof(nettype_tls_params_t));

    if (NULL == nettype_tls_params)
        RETURN_ERROR(MQTT_MEM_NOT_ENOUGH_ERROR);


    rc = nettype_tls_init(n, nettype_tls_params);
    if (MQTT_SUCCESS_ERROR != rc)
        goto exit;

    if (0 != (rc = mbedtls_net_connect(&(nettype_tls_params->socket_fd), n->host, n->port, MBEDTLS_NET_PROTO_TCP)))
        goto exit;

    while ((rc = mbedtls_ssl_handshake(&(nettype_tls_params->ssl))) != 0) {
        if (rc != MBEDTLS_ERR_SSL_WANT_READ && rc != MBEDTLS_ERR_SSL_WANT_WRITE) {
            MQTT_LOG_E("%s:%d %s()...mbedtls handshake failed returned 0x%04x", __FILE__, __LINE__, __FUNCTION__, (rc < 0 )? -rc : rc);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
            if (rc == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
                MQTT_LOG_E("%s:%d %s()...unable to verify the server's certificate", __FILE__, __LINE__, __FUNCTION__);
            }
#endif
            goto exit;
        }
    }

    if ((rc = mbedtls_ssl_get_verify_result(&(nettype_tls_params->ssl))) != 0) {
        MQTT_LOG_E("%s:%d %s()...mbedtls_ssl_get_verify_result returned 0x%04x", __FILE__, __LINE__, __FUNCTION__, (rc < 0 )? -rc : rc);
        goto exit;
    }

    n->nettype_tls_params = nettype_tls_params;
    RETURN_ERROR(MQTT_SUCCESS_ERROR)

exit:
    platform_memory_free(nettype_tls_params);
    RETURN_ERROR(rc);
}


void nettype_tls_disconnect(network_t* n) 
{
    int rc = 0;
    if (NULL == n)
        return;
    
    nettype_tls_params_t *nettype_tls_params = (nettype_tls_params_t *) n->nettype_tls_params;

    do {
        rc = mbedtls_ssl_close_notify(&(nettype_tls_params->ssl));
    } while (rc == MBEDTLS_ERR_SSL_WANT_READ || rc == MBEDTLS_ERR_SSL_WANT_WRITE);

    mbedtls_net_free(&(nettype_tls_params->socket_fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free(&(nettype_tls_params->client_cert));
    mbedtls_x509_crt_free(&(nettype_tls_params->ca_cert));
    mbedtls_pk_free(&(nettype_tls_params->private_key));
#endif
    mbedtls_ssl_free(&(nettype_tls_params->ssl));
    mbedtls_ssl_config_free(&(nettype_tls_params->ssl_conf));
    mbedtls_ctr_drbg_free(&(nettype_tls_params->ctr_drbg));
    mbedtls_entropy_free(&(nettype_tls_params->entropy));

    platform_memory_free(nettype_tls_params);
}

int nettype_tls_write(network_t *n, unsigned char *buf, int len, int timeout)
{
    int rc = 0;
    int write_len = 0;
    platform_timer_t timer;

    if (NULL == n)
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);
    
    nettype_tls_params_t *nettype_tls_params = (nettype_tls_params_t *) n->nettype_tls_params;

    platform_timer_cutdown(&timer, timeout);

    do {
        rc = mbedtls_ssl_write(&(nettype_tls_params->ssl), (unsigned char *)(buf + write_len), len - write_len);

        if (rc > 0) {
            write_len += rc;
        } else if ((rc == 0) || ((rc != MBEDTLS_ERR_SSL_WANT_WRITE) && (rc != MBEDTLS_ERR_SSL_WANT_READ) && (rc != MBEDTLS_ERR_SSL_TIMEOUT))) {
            MQTT_LOG_E("%s:%d %s()... mbedtls_ssl_write failed: 0x%04x", __FILE__, __LINE__, __FUNCTION__, (rc < 0 )? -rc : rc);
            break;
        } 
    } while((!platform_timer_is_expired(&timer)) && (write_len < len));

    return write_len;
}

int nettype_tls_read(network_t *n, unsigned char *buf, int len, int timeout)
{
    int rc = 0;
    int read_len = 0;
    platform_timer_t timer;

    if (NULL == n)
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);
    
    nettype_tls_params_t *nettype_tls_params = (nettype_tls_params_t *) n->nettype_tls_params;

    platform_timer_cutdown(&timer, timeout);
    
    do {
        rc = mbedtls_ssl_read(&(nettype_tls_params->ssl), (unsigned char *)(buf + read_len), len - read_len);

        if (rc > 0) {
            read_len += rc;
        } else if ((rc == 0) || ((rc != MBEDTLS_ERR_SSL_WANT_WRITE) && (rc != MBEDTLS_ERR_SSL_WANT_READ) && (rc != MBEDTLS_ERR_SSL_TIMEOUT))) {
            // MQTT_LOG_E("%s:%d %s()... mbedtls_ssl_read failed: 0x%04x", __FILE__, __LINE__, __FUNCTION__, (rc < 0 )? -rc : rc);
            break;
        } 
    } while((!platform_timer_is_expired(&timer)) && (read_len < len));

    return read_len;
}

#endif /* MQTT_NETWORK_TYPE_NO_TLS */
