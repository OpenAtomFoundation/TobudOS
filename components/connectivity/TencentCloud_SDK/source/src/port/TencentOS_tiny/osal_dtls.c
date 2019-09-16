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

#include "qcloud.h"

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/timing.h"
#include "mbedtls/ssl_cookie.h"
#include "mbedtls/debug.h"

#define DEBUG_LEVEL 0

void mbedtls_dtls_net_init(mbedtls_net_context *ctx);
int mbedtls_dtls_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto);
void mbedtls_dtls_net_usleep(unsigned long usec);
int mbedtls_dtls_net_recv(void *ctx, unsigned char *buf, size_t len);
int mbedtls_dtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout);
int mbedtls_dtls_net_send(void *ctx, const unsigned char *buf, size_t len);
void mbedtls_dtls_net_free(mbedtls_net_context *ctx);

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_KEY)
static const int ciphersuites[] = { MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA, 0 };
#endif

typedef struct dtls_paramter_st {
    mbedtls_net_context             socket_fd;        // socket文件描述符
    mbedtls_entropy_context         entropy;          // 保存熵配置
    mbedtls_ctr_drbg_context        ctr_drbg;         // 随机数生成器
    mbedtls_ssl_context             ssl;              // 保存SSL基本数据
    mbedtls_ssl_config              ssl_conf;         // TSL/TLS配置信息

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt                ca_cert;          // ca证书信息
    mbedtls_x509_crt                client_cert;      // 客户端证书信息
#endif
    mbedtls_pk_context              private_key;      // 客户端私钥信息

    mbedtls_timing_delay_context    timer;
    mbedtls_ssl_cookie_ctx          cookie_ctx;
} dtls_param_t;

/**
 * @brief 释放mbedtls开辟的内存
 */
static void mbedtls_dtls_param_free(dtls_param_t *dtls_param)
{
    mbedtls_dtls_net_free(&dtls_param->socket_fd);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free(&dtls_param->client_cert);
    mbedtls_x509_crt_free(&dtls_param->ca_cert);
    mbedtls_pk_free(&dtls_param->private_key);
#endif
    mbedtls_ssl_free(&dtls_param->ssl);
    mbedtls_ssl_config_free(&dtls_param->ssl_conf);
    mbedtls_ctr_drbg_free(&dtls_param->ctr_drbg);
    mbedtls_entropy_free(&dtls_param->entropy);
#if 0
    mbedtls_ssl_cookie_free(&(dtls_param->cookie_ctx));
#endif

    osal_free(dtls_param);
}

static void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    QCLOUD_LOG_I("[mbedTLS]:[%s]:[%d]: %s\r\n", file, line, str);
}

static int mbedtls_client_init(dtls_param_t *dtls_param, qcloud_tls_opt_t *tls_opt)
{
	int ret = 0;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif

    mbedtls_dtls_net_init(&dtls_param->socket_fd);
    mbedtls_ssl_init(&dtls_param->ssl);
    mbedtls_ssl_config_init(&dtls_param->ssl_conf);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_init(&dtls_param->ca_cert);
    mbedtls_x509_crt_init(&dtls_param->client_cert);
    mbedtls_pk_init(&dtls_param->private_key);
#endif
    mbedtls_ctr_drbg_init(&dtls_param->ctr_drbg);
    mbedtls_entropy_init(&dtls_param->entropy);

    if((ret = mbedtls_ctr_drbg_seed(&dtls_param->ctr_drbg, mbedtls_entropy_func,
    		&dtls_param->entropy, NULL,0)) != 0) {
        QCLOUD_LOG_E("mbedtls_ctr_drbg_seed failed returned -0x%x", -ret);
        return QCLOUD_ERR_SSL_INIT;
    }

	mbedtls_ssl_conf_authmode(&dtls_param->ssl_conf, MBEDTLS_SSL_VERIFY_NONE );

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (tls_opt->ca_cert != NULL) {
        if ((ret = mbedtls_x509_crt_parse(&dtls_param->ca_cert, (const unsigned char *)tls_opt->ca_cert,
            (tls_opt->ca_cert_len + 1)))) {
            QCLOUD_LOG_E("parse ca crt failed returned -0x%04x", -ret);
            return QCLOUD_ERR_SSL_CERT;
        }
    }
#endif

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_CERT)
    if (tls_opt->cert_path != NULL && tls_opt->priv_key_path != NULL) {
#if defined(MBEDTLS_X509_CRT_PARSE_C)
        if ((ret = mbedtls_x509_crt_parse_file(&dtls_param->client_cert, tls_opt->cert_path)) != 0) {
            QCLOUD_LOG_E("load client cert file failed returned -0x%x", ret);
            return QCLOUD_ERR_SSL_CERT;
        }
#endif

        if ((ret = mbedtls_pk_parse_keyfile(&dtls_param->private_key, tls_opt->priv_key_path, "")) != 0) {
            QCLOUD_LOG_E("load client key file failed returned -0x%x", ret);
            return QCLOUD_ERR_SSL_CERT;
        }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
        if (0 == ret) {
            mbedtls_ssl_conf_ca_chain(&dtls_param->ssl_conf, &dtls_param->ca_cert, NULL);
            if ((ret = mbedtls_ssl_conf_own_cert(&dtls_param->ssl_conf, &dtls_param->client_cert, &dtls_param->private_key)) != 0) {
                QCLOUD_LOG_E("mbedtls_ssl_conf_own_cert failed returned -0x%x", -ret);
                return QCLOUD_ERR_SSL_CERT;
            }
        }
#endif
    } else {
        QCLOUD_LOG_D("cert_file/key_file is empty!|cert_file=%s|key_file=%s", tls_opt->cert_path, tls_opt->priv_key_path);
    }

#else
    if (tls_opt->psk != NULL && tls_opt->psk_id !=NULL) {
        const char *psk_id = tls_opt->psk_id;
        ret = mbedtls_ssl_conf_psk(&dtls_param->ssl_conf, (unsigned char *)tls_opt->psk, tls_opt->psk_len,
                                    (const unsigned char *)psk_id, strlen(psk_id));
    } else {
        QCLOUD_LOG_D("psk/pskid is empty!|psk=%s|psd_id=%s", tls_opt->psk, tls_opt->psk_id);
    }

	if (0 != ret) {
		QCLOUD_LOG_E("mbedtls_ssl_conf_psk fail: -0x%x", -ret);
		return ret;
	}
#endif

    return ret;
}

/**
 * @brief 建立UDP连接
 *
 * @param socket_fd  Socket描述符
 * @param host       服务器主机名
 * @param port       服务器端口地址
 * @return 返回QCLOUD_ERR_SUCCESS, 表示成功
 */
int mbedtls_udp_connect(mbedtls_net_context *socket_fd, const char *host, int port)
{
    int ret = 0;
    char port_str[6];

    osal_snprintf(port_str, 6, "%d", port);
    if ((ret = mbedtls_dtls_net_connect(socket_fd, host, port_str, MBEDTLS_NET_PROTO_UDP)) != 0) {
        QCLOUD_LOG_E("mbedtls_dtls_net_connect host:%s port:%s returned -0x%04x", host, port_str, -ret);
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
        QCLOUD_LOG_E("set block faliled returned -0x%04x", -ret);
        return QCLOUD_ERR_TCP_CONNECT;
    }
#endif

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_OSAL__ void *osal_dtls_connect(qcloud_tls_opt_t *tls_opt, const char *host, int port)
{
	QCLOUD_FUNC_ENTRY;

    int ret = QCLOUD_ERR_SUCCESS;
    dtls_param_t *dtls_param = NULL;

    if (!tls_opt) {
        return NULL;
    }

    dtls_param = (dtls_param_t *)osal_malloc(sizeof(dtls_param_t));
    if (!dtls_param) {
        return NULL;
    }

    if ((ret = mbedtls_client_init(dtls_param, tls_opt)) != QCLOUD_ERR_SUCCESS) {
		goto error;
	}

    QCLOUD_LOG_D(" Connecting to /%s/%d...", host, port);
    if ((ret = mbedtls_udp_connect(&dtls_param->socket_fd, host, port)) != QCLOUD_ERR_SUCCESS) {
		goto error;
	}

    QCLOUD_LOG_D(" Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(&dtls_param->ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                         MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        QCLOUD_LOG_E("mbedtls_ssl_config_defaults result 0x%04x", ret);
        goto error;
    }

    mbedtls_ssl_conf_authmode(&dtls_param->ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);

    mbedtls_ssl_conf_rng(&dtls_param->ssl_conf, mbedtls_ctr_drbg_random, &dtls_param->ctr_drbg);
    mbedtls_ssl_conf_dbg(&dtls_param->ssl_conf, mbedtls_debug, NULL);

#if 0
    if ((ret = mbedtls_ssl_cookie_setup(&pDataParams->cookie_ctx, mbedtls_ctr_drbg_random, &pDataParams->ctr_drbg)) != 0) {
        QCLOUD_LOG_E("mbedtls_ssl_cookie_setup result 0x%04x", ret);
        goto error;
    }
#endif

#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY) && defined(MBEDTLS_SSL_SRV_C)
    mbedtls_ssl_conf_dtls_cookies(&dtls_param->ssl_conf, mbedtls_ssl_cookie_write, mbedtls_ssl_cookie_check, &dtls_param->cookie_ctx);
#endif

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_KEY)
	mbedtls_ssl_conf_ciphersuites(&dtls_param->ssl_conf, ciphersuites);
#endif

#ifdef MBEDTLS_SSL_PROTO_DTLS
    if (dtls_param->ssl_conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
        mbedtls_ssl_conf_min_version(&dtls_param->ssl_conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

        mbedtls_ssl_conf_max_version(&dtls_param->ssl_conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

        mbedtls_ssl_conf_handshake_timeout(&dtls_param->ssl_conf, (MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2),
        		(MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN * 2 * 4));
    }
#endif

    if ((ret = mbedtls_ssl_setup(&dtls_param->ssl, &dtls_param->ssl_conf)) != 0) {
		QCLOUD_LOG_E("mbedtls_ssl_setup failed returned -0x%x", -ret);
		goto error;
	}

    if (dtls_param->ssl_conf.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
		mbedtls_ssl_set_timer_cb(&dtls_param->ssl, (void *)&dtls_param->timer, mbedtls_timing_set_delay,
								 mbedtls_timing_get_delay);
	}

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if ((ret = mbedtls_ssl_set_hostname(&dtls_param->ssl, host)) != 0) {
		QCLOUD_LOG_E("mbedtls_ssl_set_hostname failed returned -0x%x", -ret);
		goto error;
	}
#endif

	mbedtls_ssl_set_bio(&dtls_param->ssl, (void *)&dtls_param->socket_fd, mbedtls_dtls_net_send, mbedtls_dtls_net_recv,
						mbedtls_dtls_net_recv_timeout);

    QCLOUD_LOG_D("Performing the SSL/TLS handshake...");
	while ((ret = mbedtls_ssl_handshake(&dtls_param->ssl)) != 0) {
		if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
			QCLOUD_LOG_E("mbedtls_ssl_handshake failed returned -0x%x", -ret);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
			if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
				QCLOUD_LOG_E("Unable to verify the server's certificate");
			}
#endif
			goto error;
		}
	}

    if ((ret = mbedtls_ssl_get_verify_result(&dtls_param->ssl)) != 0) {
        QCLOUD_LOG_E("mbedtls_ssl_get_verify_result failed returned -0x%x", -ret);
        goto error;
    }

    return dtls_param;

error:
	mbedtls_dtls_param_free(dtls_param);
    return NULL;
}

__QCLOUD_OSAL__ void osal_dtls_disconnect(void *handle)
{
    int rc = 0;
    dtls_param_t *dtls_param = NULL;

    if (!handle) {
        return;
    }

    dtls_param = (dtls_param_t *)handle;

    do {
        rc = mbedtls_ssl_close_notify(&dtls_param->ssl);
    } while (rc == MBEDTLS_ERR_SSL_WANT_READ || rc == MBEDTLS_ERR_SSL_WANT_WRITE);

    mbedtls_dtls_param_free(dtls_param);
}

__QCLOUD_OSAL__ qcloud_err_t osal_dtls_write(void *handle, const void *buf, size_t len, size_t *write_len)
{
	QCLOUD_POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(write_len, QCLOUD_ERR_INVAL);

    int rc;
	dtls_param_t *data_params = NULL;

    data_params = (dtls_param_t *)handle;

    rc = mbedtls_ssl_write(&data_params->ssl, buf, len);
    if (rc < 0) {
        QCLOUD_LOG_E("ssl_write failed %d", rc);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SSL_WRITE);
    }

    *write_len = rc;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_OSAL__ qcloud_err_t osal_dtls_read(void *handle, void *buf, size_t len, uint32_t timeout, size_t *read_len)
{
    QCLOUD_POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(read_len, QCLOUD_ERR_INVAL);

    int rc = 0;
    dtls_param_t *data_params = NULL;

    data_params = (dtls_param_t *)handle;

	mbedtls_ssl_conf_read_timeout(&data_params->ssl_conf, timeout);

	do {
		rc = mbedtls_ssl_read(&data_params->ssl, buf, len);
	} while (rc == MBEDTLS_ERR_SSL_WANT_READ || rc == MBEDTLS_ERR_SSL_WANT_WRITE);

    if (rc > 0) {
        *read_len = rc;
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

	switch (rc) {
		case MBEDTLS_ERR_SSL_TIMEOUT:
			QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SSL_READ_TIMEOUT);

		case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
			QCLOUD_LOG_E("connection closed");
			QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_DTLS_PEER_CLOSE_NOTIFY);

		default:
			QCLOUD_LOG_E("ssl_read returned -0x%x", -rc);
            break;
	}

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
}

#ifdef __cplusplus
}
#endif

