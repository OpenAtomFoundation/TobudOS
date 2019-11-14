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

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#ifndef AUTH_WITH_NOTLS

#ifdef COAP_COMM_ENABLED

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/timing.h"
#include "mbedtls/ssl_cookie.h"

#include "utils_timer.h"
#include "utils_param_check.h"

#define DEBUG_LEVEL 0

#ifndef AUTH_MODE_CERT
static const int ciphersuites[] = { MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA, 0 };
#endif

/**
 * @brief data structure for mbedtls SSL connection
 */
typedef struct
{
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

    mbedtls_timing_delay_context    timer;
#if 0
    mbedtls_ssl_cookie_ctx          cookie_ctx;
#endif
} DTLSDataParams;

/**
 * @brief free memory/resources allocated by mbedtls
 */
static void _free_mebeddtls(DTLSDataParams *pParams)
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
#if 0
    mbedtls_ssl_cookie_free(&(pParams->cookie_ctx));
#endif

    HAL_Free(pParams);
}

static void _dtls_debug( void *ctx, int level,
                     const char *file, int line,
                     const char *str )
{
    Log_i("[mbedTLS]:[%s]:[%d]: %s\r\n", file, line, str);
}

static int _mbedtls_client_init(DTLSDataParams *pDataParams, DTLSConnectParams *pConnectParams)
{
	int ret = QCLOUD_RET_SUCCESS;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( DEBUG_LEVEL );
#endif

    mbedtls_net_init( &(pDataParams->socket_fd) );
    mbedtls_ssl_init( &(pDataParams->ssl) );
    mbedtls_ssl_config_init( &(pDataParams->ssl_conf) );
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_init( &(pDataParams->ca_cert) );
    mbedtls_x509_crt_init(&(pDataParams->client_cert));
    mbedtls_pk_init(&(pDataParams->private_key));
#endif
    mbedtls_ctr_drbg_init( &(pDataParams->ctr_drbg) );
    mbedtls_entropy_init( &(pDataParams->entropy) );

    if((ret = mbedtls_ctr_drbg_seed(&pDataParams->ctr_drbg, mbedtls_entropy_func,
    		&pDataParams->entropy, NULL,0)) != 0)
    {
        Log_e("mbedtls_ctr_drbg_seed failed returned -0x%x", -ret);
        return QCLOUD_ERR_SSL_INIT;
    }

	mbedtls_ssl_conf_authmode(&pDataParams->ssl_conf, MBEDTLS_SSL_VERIFY_NONE );

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (pConnectParams->ca_crt != NULL)
    {
        if ((ret = mbedtls_x509_crt_parse(&(pDataParams->ca_cert), (const unsigned char *)pConnectParams->ca_crt,
            (pConnectParams->ca_crt_len + 1)))) {
            Log_e("parse ca crt failed returned -0x%04x", -ret);
            return QCLOUD_ERR_SSL_CERT;
        }
    }
#endif

#ifdef AUTH_MODE_CERT
    if (pConnectParams->cert_file != NULL && pConnectParams->key_file != NULL) {
#if defined(MBEDTLS_X509_CRT_PARSE_C)
            if ((ret = mbedtls_x509_crt_parse_file(&(pDataParams->client_cert), pConnectParams->cert_file)) != 0) {
            Log_e("load client cert file failed returned -0x%x", ret);
            return QCLOUD_ERR_SSL_CERT;
        }
#endif

        if ((ret = mbedtls_pk_parse_keyfile(&(pDataParams->private_key), pConnectParams->key_file, "")) != 0) {
            Log_e("load client key file failed returned -0x%x", ret);
            return QCLOUD_ERR_SSL_CERT;
        }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
        if (0 == ret) {
            mbedtls_ssl_conf_ca_chain(&(pDataParams->ssl_conf), &(pDataParams->ca_cert), NULL);
            if ((ret = mbedtls_ssl_conf_own_cert(&(pDataParams->ssl_conf), &(pDataParams->client_cert), &(pDataParams->private_key))) != 0) {
                Log_e("mbedtls_ssl_conf_own_cert failed returned -0x%x", -ret);
                return QCLOUD_ERR_SSL_CERT;
            }
        }
#endif
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
		Log_e("mbedtls_ssl_conf_psk fail: -0x%x", -ret);
		return ret;
	}
#endif

    return ret;
}

/**
 * @brief Setup UDP connection
 *
 * @param socket_fd  socket handle
 * @param host       server address
 * @param port       server port
 * @return QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int _mbedtls_udp_connect(mbedtls_net_context *socket_fd, const char *host, int port) {
    int ret = 0;
    char port_str[6];
    HAL_Snprintf(port_str, 6, "%d", port);
    if ((ret = mbedtls_net_connect(socket_fd, host, port_str, MBEDTLS_NET_PROTO_UDP)) != 0) {
        Log_e("mbedtls_net_connect host:%s port:%s returned -0x%04x errno: %d", host, port_str, -ret, errno);
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
        Log_e("set block faliled returned -0x%04x", -ret);
        return QCLOUD_ERR_TCP_CONNECT;
    }
#endif

    return QCLOUD_RET_SUCCESS;
}

uintptr_t HAL_DTLS_Connect(DTLSConnectParams *pConnectParams, const char *host, int port)
{
	IOT_FUNC_ENTRY;

    int ret = QCLOUD_RET_SUCCESS;

    DTLSDataParams * pDataParams = (DTLSDataParams *)HAL_Malloc(sizeof(DTLSDataParams));

    if ((ret = _mbedtls_client_init(pDataParams, pConnectParams)) != QCLOUD_RET_SUCCESS) {
		goto error;
	}

    if ((ret = _mbedtls_udp_connect(&(pDataParams->socket_fd), host, port)) != QCLOUD_RET_SUCCESS) {
		goto error;
	}

    if ((ret = mbedtls_ssl_config_defaults(&pDataParams->ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                         MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        Log_e("mbedtls_ssl_config_defaults result 0x%04x", ret);
        goto error;
    }

    mbedtls_ssl_conf_authmode(&(pDataParams->ssl_conf), MBEDTLS_SSL_VERIFY_REQUIRED);

    mbedtls_ssl_conf_rng(&pDataParams->ssl_conf, mbedtls_ctr_drbg_random, &pDataParams->ctr_drbg);
    mbedtls_ssl_conf_dbg(&pDataParams->ssl_conf, _dtls_debug, NULL);

#if 0
    if ((ret = mbedtls_ssl_cookie_setup(&pDataParams->cookie_ctx, mbedtls_ctr_drbg_random, &pDataParams->ctr_drbg)) != 0) {
        Log_e("mbedtls_ssl_cookie_setup result 0x%04x", ret);
        goto error;
    }

    mbedtls_ssl_conf_dtls_cookies(&pDataParams->ssl_conf, mbedtls_ssl_cookie_write, mbedtls_ssl_cookie_check, &pDataParams->cookie_ctx);
#endif

#ifndef AUTH_MODE_CERT
	mbedtls_ssl_conf_ciphersuites(&(pDataParams->ssl_conf), ciphersuites);
#endif

#ifdef MBEDTLS_SSL_PROTO_DTLS
    if (pDataParams->ssl_conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM)
    {
        mbedtls_ssl_conf_min_version(&pDataParams->ssl_conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

        mbedtls_ssl_conf_max_version(&pDataParams->ssl_conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

        mbedtls_ssl_conf_handshake_timeout(&pDataParams->ssl_conf, (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2),
        		(MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2 * 4));
    }
#endif

    if ((ret = mbedtls_ssl_setup(&(pDataParams->ssl), &(pDataParams->ssl_conf))) != 0) {
		Log_e("mbedtls_ssl_setup failed returned -0x%x", -ret);
		goto error;
	}

    if (pDataParams->ssl_conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
		mbedtls_ssl_set_timer_cb(&(pDataParams->ssl), (void *)&pDataParams->timer, mbedtls_timing_set_delay,
								 mbedtls_timing_get_delay);
	}

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if ((ret = mbedtls_ssl_set_hostname(&(pDataParams->ssl), host)) != 0) {
		Log_e("mbedtls_ssl_set_hostname failed returned -0x%x", -ret);
		goto error;
	}
#endif

	mbedtls_ssl_set_bio(&(pDataParams->ssl), (void *)&pDataParams->socket_fd, mbedtls_net_send, mbedtls_net_recv,
						mbedtls_net_recv_timeout);

	while ((ret = mbedtls_ssl_handshake(&(pDataParams->ssl))) != 0) {
		if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
			Log_e("mbedtls_ssl_handshake failed returned -0x%x", -ret);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
			if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
				Log_e("Unable to verify the server's certificate");
			}
#endif
			goto error;
		}
	}

    if ((ret = mbedtls_ssl_get_verify_result(&(pDataParams->ssl))) != 0) {
        Log_e("mbedtls_ssl_get_verify_result failed returned -0x%x", -ret);
        goto error;
    }

    return (uintptr_t)pDataParams;

error:
	_free_mebeddtls(pDataParams);
    return 0;
}

void HAL_DTLS_Disconnect(uintptr_t handle)
{
    if ((uintptr_t)NULL == handle) {
        Log_d("handle is NULL");
        return;
    }

    DTLSDataParams *pParams = (DTLSDataParams *)handle;
    int ret = 0;
    do {
        ret = mbedtls_ssl_close_notify(&(pParams->ssl));
    } while (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);

    mbedtls_net_free(&(pParams->socket_fd));
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt_free(&(pParams->client_cert));
    mbedtls_x509_crt_free(&(pParams->ca_cert));
    mbedtls_pk_free(&(pParams->private_key));
#endif
    mbedtls_ssl_free(&(pParams->ssl));
    mbedtls_ssl_config_free(&(pParams->ssl_conf));
    mbedtls_ctr_drbg_free(&(pParams->ctr_drbg));
    mbedtls_entropy_free(&(pParams->entropy));
#if 0
    mbedtls_ssl_cookie_free(&(pParams->cookie_ctx));
#endif

    HAL_Free((void *)handle);
}

int HAL_DTLS_Write(uintptr_t handle, const unsigned char *data, size_t datalen, size_t *written_len)
{
	DTLSDataParams *data_params = (DTLSDataParams *)handle;
	POINTER_SANITY_CHECK(data_params, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(data, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(written_len, QCLOUD_ERR_INVAL);

    int rc = mbedtls_ssl_write(&data_params->ssl, data, datalen);

	if( rc < 0 ) {
		Log_e("failed! mbedtls_ssl_write returned %d", rc);
		rc = QCLOUD_ERR_SSL_WRITE;
	}

	*written_len = rc;
    rc = QCLOUD_RET_SUCCESS;

    IOT_FUNC_EXIT_RC(rc);
}

int HAL_DTLS_Read(uintptr_t handle, unsigned char *data, size_t datalen, unsigned int timeout_ms,
        size_t *read_len)
{
    DTLSDataParams *data_params = (DTLSDataParams *)handle;
    POINTER_SANITY_CHECK(data_params, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(data, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(read_len, QCLOUD_ERR_INVAL);

    int rc = QCLOUD_ERR_SSL_READ;

	mbedtls_ssl_conf_read_timeout(&(data_params->ssl_conf), timeout_ms);

	do {
		rc = mbedtls_ssl_read(&(data_params->ssl), data, datalen);
	} while (rc == MBEDTLS_ERR_SSL_WANT_READ || rc == MBEDTLS_ERR_SSL_WANT_WRITE);

	if( rc <= 0 ) {
		*read_len = 0;
		switch( rc )
		{
			case MBEDTLS_ERR_SSL_TIMEOUT:
				rc = QCLOUD_ERR_SSL_READ_TIMEOUT;
				break;

			case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
				Log_e("connection was closed gracefully");
				rc = QCLOUD_ERR_DTLS_PEER_CLOSE_NOTIFY;
				break;

			default:
				Log_e( " mbedtls_ssl_read returned -0x%x", -rc );
				break;
		}
	} else {
		*read_len = rc;
		rc = QCLOUD_RET_SUCCESS;
	}

    return rc;
}
#endif

#ifdef __cplusplus
}
#endif
#endif