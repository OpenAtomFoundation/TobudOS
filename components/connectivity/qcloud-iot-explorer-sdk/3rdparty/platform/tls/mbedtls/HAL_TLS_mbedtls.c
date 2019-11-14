/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_import.h"

#ifndef AUTH_WITH_NOTLS

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "qcloud_iot_export_error.h"
#include "qcloud_iot_export_log.h"
#include "utils_param_check.h"

#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"

#include "utils_timer.h"

#ifndef AUTH_MODE_CERT
static const int ciphersuites[] = { MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA, 0 };
#endif

/**
 * @brief data structure for mbedtls SSL connection
 */
typedef struct {
    mbedtls_net_context          socket_fd;
    mbedtls_entropy_context      entropy;
    mbedtls_ctr_drbg_context     ctr_drbg;
    mbedtls_ssl_context          ssl;
    mbedtls_ssl_config           ssl_conf;
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt             ca_cert;
    mbedtls_x509_crt             client_cert;
#endif
    mbedtls_pk_context           private_key;
} TLSDataParams;

/**
 * @brief free memory/resources allocated by mbedtls
 */
static void _free_mebedtls(TLSDataParams *pParams)
{
    mbedtls_net_free(&(pParams->socket_fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free(&(pParams->client_cert));
    mbedtls_x509_crt_free(&(pParams->ca_cert));
    mbedtls_pk_free(&(pParams->private_key));
#endif
    mbedtls_ssl_free(&(pParams->ssl));
    mbedtls_ssl_config_free(&(pParams->ssl_conf));
    mbedtls_ctr_drbg_free(&(pParams->ctr_drbg));
    mbedtls_entropy_free(&(pParams->entropy));

    HAL_Free(pParams);
}

/**
 * @brief mbedtls SSL client init
 *
 * 1. call a series of mbedtls init functions
 * 2. init and set seed for random functions
 * 3. load CA file, cert files or PSK
 *
 * @param pDataParams       mbedtls TLS parmaters
 * @param pConnectParams    device info for TLS connection
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */
static int _mbedtls_client_init(TLSDataParams *pDataParams, TLSConnectParams *pConnectParams) {

    int ret = QCLOUD_RET_SUCCESS;
    mbedtls_net_init(&(pDataParams->socket_fd));
    mbedtls_ssl_init(&(pDataParams->ssl));
    mbedtls_ssl_config_init(&(pDataParams->ssl_conf));
    mbedtls_ctr_drbg_init(&(pDataParams->ctr_drbg));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_init(&(pDataParams->ca_cert));
    mbedtls_x509_crt_init(&(pDataParams->client_cert));
    mbedtls_pk_init(&(pDataParams->private_key));
#endif

    mbedtls_entropy_init(&(pDataParams->entropy));
    // custom parameter is NULL for now
    if ((ret = mbedtls_ctr_drbg_seed(&(pDataParams->ctr_drbg), mbedtls_entropy_func,
                                     &(pDataParams->entropy), NULL, 0)) != 0) {
        Log_e("mbedtls_ctr_drbg_seed failed returned 0x%04x", ret<0?-ret:ret);
        return QCLOUD_ERR_SSL_INIT;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (pConnectParams->ca_crt != NULL)
    {
        if ((ret = mbedtls_x509_crt_parse(&(pDataParams->ca_cert), (const unsigned char *)pConnectParams->ca_crt,
            (pConnectParams->ca_crt_len + 1)))) {
            Log_e("parse ca crt failed returned 0x%04x", ret<0?-ret:ret);
            return QCLOUD_ERR_SSL_CERT;
        }
    }
#endif

#ifdef AUTH_MODE_CERT
    if (pConnectParams->cert_file != NULL && pConnectParams->key_file != NULL) {
#if defined(MBEDTLS_X509_CRT_PARSE_C)
            if ((ret = mbedtls_x509_crt_parse_file(&(pDataParams->client_cert), pConnectParams->cert_file)) != 0) {
            Log_e("load client cert file failed returned 0x%x", ret<0?-ret:ret);
            return QCLOUD_ERR_SSL_CERT;
        }
#endif

        if ((ret = mbedtls_pk_parse_keyfile(&(pDataParams->private_key), pConnectParams->key_file, "")) != 0) {
            Log_e("load client key file failed returned 0x%x", ret<0?-ret:ret);
            return QCLOUD_ERR_SSL_CERT;
        }
    } else {
        Log_d("cert_file/key_file is empty!|cert_file=%s|key_file=%s", pConnectParams->cert_file, pConnectParams->key_file);
    }
#else
	if (pConnectParams->psk != NULL && pConnectParams->psk_id !=NULL) {
        const char *psk_id = pConnectParams->psk_id;
        ret = mbedtls_ssl_conf_psk(&(pDataParams->ssl_conf), (unsigned char *)pConnectParams->psk, pConnectParams->psk_length,
                                    (const unsigned char *) psk_id, strlen( psk_id ));
    } else {
        Log_d("psk/pskid is empty!|psk=%s|psd_id=%s", pConnectParams->psk, pConnectParams->psk_id);
    }

	if (0 != ret) {
		Log_e("mbedtls_ssl_conf_psk fail: 0x%x", ret<0?-ret:ret);
		return ret;
	}
#endif

    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Setup TCP connection
 *
 * @param socket_fd  socket handle
 * @param host       server address
 * @param port       server port
 * @return QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int _mbedtls_tcp_connect(mbedtls_net_context *socket_fd, const char *host, int port) {
    int ret = 0;
    char port_str[6];
    HAL_Snprintf(port_str, 6, "%d", port);
    if ((ret = mbedtls_net_connect(socket_fd, host, port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {

        Log_e("tcp connect failed returned 0x%04x errno: %d", ret<0?-ret:ret, errno);

        switch (ret) {
            case MBEDTLS_ERR_NET_SOCKET_FAILED:
                return QCLOUD_ERR_TCP_SOCKET_FAILED;
            case MBEDTLS_ERR_NET_UNKNOWN_HOST:
                return QCLOUD_ERR_TCP_UNKNOWN_HOST;
            default:
                return QCLOUD_ERR_TCP_CONNECT;
        }

    }

#if 0
    if ((ret = mbedtls_net_set_block(socket_fd)) != 0) {
        Log_e("set block faliled returned 0x%04x", ret<0?-ret:ret);
        return QCLOUD_ERR_TCP_CONNECT;
    }
#endif

    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief verify server certificate
 *
 * mbedtls has provided similar function mbedtls_x509_crt_verify_with_profile
 *
 * @return
 */
 #if defined(MBEDTLS_X509_CRT_PARSE_C)
int _qcloud_server_certificate_verify(void *hostname, mbedtls_x509_crt *crt, int depth, uint32_t *flags) {
    return *flags;
}
#endif

uintptr_t HAL_TLS_Connect(TLSConnectParams *pConnectParams, const char *host, int port)
{
    int ret = 0;

    TLSDataParams * pDataParams = (TLSDataParams *)HAL_Malloc(sizeof(TLSDataParams));

    if ((ret = _mbedtls_client_init(pDataParams, pConnectParams)) != QCLOUD_RET_SUCCESS) {
        goto error;
    }



    Log_d("Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(&(pDataParams->ssl_conf), MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        Log_e("mbedtls_ssl_config_defaults failed returned 0x%04x", ret<0?-ret:ret);
        goto error;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_verify(&(pDataParams->ssl_conf), _qcloud_server_certificate_verify, (void *)host);

    mbedtls_ssl_conf_authmode(&(pDataParams->ssl_conf), MBEDTLS_SSL_VERIFY_REQUIRED);
#endif

    mbedtls_ssl_conf_rng(&(pDataParams->ssl_conf), mbedtls_ctr_drbg_random, &(pDataParams->ctr_drbg));

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&(pDataParams->ssl_conf), &(pDataParams->ca_cert), NULL);
    if ((ret = mbedtls_ssl_conf_own_cert(&(pDataParams->ssl_conf),
                                         &(pDataParams->client_cert), &(pDataParams->private_key))) != 0) {
        Log_e("mbedtls_ssl_conf_own_cert failed returned 0x%04x", ret<0?-ret:ret);
        goto error;
    }
#endif

    mbedtls_ssl_conf_read_timeout(&(pDataParams->ssl_conf), pConnectParams->timeout_ms);
    if ((ret = mbedtls_ssl_setup(&(pDataParams->ssl), &(pDataParams->ssl_conf))) != 0) {
        Log_e("mbedtls_ssl_setup failed returned 0x%04x", ret<0?-ret:ret);
        goto error;
    }

#ifndef AUTH_MODE_CERT
    // ciphersuites selection for PSK device
    if(pConnectParams->psk != NULL) {
        mbedtls_ssl_conf_ciphersuites(&(pDataParams->ssl_conf), ciphersuites);
    }
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    // Set the hostname to check against the received server certificate and sni
    if ((ret = mbedtls_ssl_set_hostname(&(pDataParams->ssl), host)) != 0) {
        Log_e("mbedtls_ssl_set_hostname failed returned 0x%04x", ret<0?-ret:ret);
        goto error;
    }
#endif

    mbedtls_ssl_set_bio(&(pDataParams->ssl), &(pDataParams->socket_fd), mbedtls_net_send, mbedtls_net_recv,
                        mbedtls_net_recv_timeout);

    Log_d("Performing the SSL/TLS handshake...");
    Log_d("Connecting to /%s/%d...", host, port);
    if ((ret = _mbedtls_tcp_connect(&(pDataParams->socket_fd), host, port)) != QCLOUD_RET_SUCCESS) {
        goto error;
    }

    while ((ret = mbedtls_ssl_handshake(&(pDataParams->ssl))) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            Log_e("mbedtls_ssl_handshake failed returned 0x%04x", ret<0?-ret:ret);

#if defined(MBEDTLS_X509_CRT_PARSE_C)
            if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
                Log_e("Unable to verify the server's certificate");
            }
#endif
            goto error;
        }
    }

    if ((ret = mbedtls_ssl_get_verify_result(&(pDataParams->ssl))) != 0) {
        Log_e("mbedtls_ssl_get_verify_result failed returned 0x%04x", ret<0?-ret:ret);
        goto error;
    }

    mbedtls_ssl_conf_read_timeout(&(pDataParams->ssl_conf), 100);

    Log_i("connected with /%s/%d...", host, port);

    return (uintptr_t)pDataParams;

error:
    _free_mebedtls(pDataParams);
    return 0;
}

void HAL_TLS_Disconnect(uintptr_t handle) {
    if ((uintptr_t)NULL == handle) {
        Log_d("handle is NULL");
        return;
    }
    TLSDataParams *pParams = (TLSDataParams *)handle;
    int ret = 0;
    do {
        ret = mbedtls_ssl_close_notify(&(pParams->ssl));
    } while (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);

    mbedtls_net_free(&(pParams->socket_fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free(&(pParams->client_cert));
    mbedtls_x509_crt_free(&(pParams->ca_cert));
    mbedtls_pk_free(&(pParams->private_key));
#endif
    mbedtls_ssl_free(&(pParams->ssl));
    mbedtls_ssl_config_free(&(pParams->ssl_conf));
    mbedtls_ctr_drbg_free(&(pParams->ctr_drbg));
    mbedtls_entropy_free(&(pParams->entropy));

    HAL_Free((void *)handle);
}

int HAL_TLS_Write(uintptr_t handle, unsigned char *msg, size_t totalLen, uint32_t timeout_ms,
                                 size_t *written_len)
{

    Timer timer;
    InitTimer(&timer);
    countdown_ms(&timer, (unsigned int) timeout_ms);
    size_t written_so_far;
    bool errorFlag = false;
    int write_rc = 0;

    TLSDataParams *pParams = (TLSDataParams *)handle;

    for (written_so_far = 0; written_so_far < totalLen && !expired(&timer); written_so_far += write_rc) {

        while (!expired(&timer) && (write_rc = mbedtls_ssl_write(&(pParams->ssl), msg + written_so_far, totalLen - written_so_far)) <= 0)
        {
            if (write_rc != MBEDTLS_ERR_SSL_WANT_READ && write_rc != MBEDTLS_ERR_SSL_WANT_WRITE) {
                Log_e("HAL_TLS_write failed 0x%04x", write_rc<0?-write_rc:write_rc);
                errorFlag = true;
                break;
            }
        }

        if (errorFlag) {
            break;
        }
    }

    *written_len = written_so_far;

    if (errorFlag) {
        return QCLOUD_ERR_SSL_WRITE;
    } else if (expired(&timer) && written_so_far != totalLen) {
        return QCLOUD_ERR_SSL_WRITE_TIMEOUT;
    }

    return QCLOUD_RET_SUCCESS;
}

int HAL_TLS_Read(uintptr_t handle, unsigned char *msg, size_t totalLen, uint32_t timeout_ms, size_t *read_len)
{

    //mbedtls_ssl_conf_read_timeout(&(pParams->ssl_conf), timeout_ms); TODO:this cause read blocking and no return even timeout
    // use non-blocking read
    Timer timer;
    InitTimer(&timer);
    countdown_ms(&timer, (unsigned int) timeout_ms);
    *read_len = 0;

    TLSDataParams *pParams = (TLSDataParams *)handle;

    do {
        int read_rc = 0;
        read_rc = mbedtls_ssl_read(&(pParams->ssl), msg + *read_len, totalLen - *read_len);

        if (read_rc > 0) {
            *read_len += read_rc;
        } else if (read_rc == 0 || (read_rc != MBEDTLS_ERR_SSL_WANT_WRITE
                                    && read_rc != MBEDTLS_ERR_SSL_WANT_READ && read_rc != MBEDTLS_ERR_SSL_TIMEOUT)) {
            Log_e("cloud_iot_network_tls_read failed: 0x%04x", read_rc<0?-read_rc:read_rc);
            return QCLOUD_ERR_SSL_READ;
        }

        if (expired(&timer)) {
            break;
        }

    } while (*read_len < totalLen);

    if (totalLen == *read_len) {
        return QCLOUD_RET_SUCCESS;
    }

    if (*read_len == 0) {
        return QCLOUD_ERR_SSL_NOTHING_TO_READ;
    } else {
        return QCLOUD_ERR_SSL_READ_TIMEOUT;
    }
}

#ifdef __cplusplus
}
#endif

#endif
