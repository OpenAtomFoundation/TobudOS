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

#include <stdint.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "tos_k.h"
#include "qcloud.h"

#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"

#define DEBUG_LEVEL 0

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_KEY)
static const int ciphersuites[] = {MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA, 0};
#endif

/**
 * @brief 用于保存SSL连接相关数据结构
 */
typedef struct tls_paramter_st {
    mbedtls_net_context         socket_fd;        // socket文件描述符
    mbedtls_entropy_context     entropy;          // 保存熵配置
    mbedtls_ctr_drbg_context    ctr_drbg;         // 随机数生成器
    mbedtls_ssl_context         ssl;              // 保存SSL基本数据
    mbedtls_ssl_config          ssl_conf;         // TSL/TLS配置信息
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt            ca_cert;          // ca证书信息
    mbedtls_x509_crt            client_cert;      // 客户端证书信息
#endif
    mbedtls_pk_context          private_key;      // 客户端私钥信息
} tls_param_t;

/**
 * @brief 释放mbedtls开辟的内存
 */
static void mbedtls_tls_param_free(tls_param_t *tls_paramter)
{
    mbedtls_net_free(&tls_paramter->socket_fd);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free(&tls_paramter->client_cert);
    mbedtls_x509_crt_free(&tls_paramter->ca_cert);
    mbedtls_pk_free(&tls_paramter->private_key);
#endif
    mbedtls_ssl_free(&tls_paramter->ssl);
    mbedtls_ssl_config_free(&tls_paramter->ssl_conf);
    mbedtls_ctr_drbg_free(&tls_paramter->ctr_drbg);
    mbedtls_entropy_free(&tls_paramter->entropy);

    osal_free(tls_paramter);
}

/**
 * @brief mbedtls库初始化
 *
 * 1. 执行mbedtls库相关初始化函数
 * 2. 随机数生成器
 * 3. 加载CA证书, 客户端证书及私钥文件/设置psk
 *
 * @param pDataParams       TLS连接相关数据结构
 * @param pConnectParams    TLS证书密钥相关
 * @return                  返回QCLOUD_ERR_SUCCESS, 表示成功
 */
static void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    QCLOUD_LOG_I("[mbedTLS]:[%s]:[%d]: %s\r\n", file, line, str);
}

static int mbedtls_client_init(tls_param_t *tls_param, qcloud_tls_opt_t *tls_opt)
{
    int ret = QCLOUD_ERR_SUCCESS;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif

    mbedtls_net_init(&tls_param->socket_fd);
    mbedtls_ssl_init(&tls_param->ssl);
    mbedtls_ssl_config_init(&tls_param->ssl_conf);
    mbedtls_ctr_drbg_init(&tls_param->ctr_drbg);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_init(&tls_param->ca_cert);
    mbedtls_x509_crt_init(&tls_param->client_cert);
    mbedtls_pk_init(&tls_param->private_key);
#endif

    mbedtls_entropy_init(&tls_param->entropy);

	mbedtls_ssl_conf_dbg(&tls_param->ssl_conf, mbedtls_debug, NULL);

    // 随机数, 增加custom参数, 目前为NULL
    if ((ret = mbedtls_ctr_drbg_seed(&tls_param->ctr_drbg, mbedtls_entropy_func,
                                        &tls_param->entropy, NULL, 0)) != 0) {
        QCLOUD_LOG_E("mbedtls_ctr_drbg_seed failed returned -0x%04x", -ret);
        return QCLOUD_ERR_SSL_INIT;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (tls_opt->ca_cert != NULL) {
        if ((ret = mbedtls_x509_crt_parse(&tls_param->ca_cert, (const unsigned char *)tls_opt->ca_cert,
            (tls_opt->ca_cert_len + 1)))) {
            QCLOUD_LOG_E("parse ca crt failed returned -0x%04x", -ret);
            return QCLOUD_ERR_SSL_CERT;
        }
    }
#endif

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_CERT)
    if (tls_opt->cert_path != NULL && tls_opt->priv_key_path != NULL) {
#if defined(MBEDTLS_X509_CRT_PARSE_C)
        if ((ret = mbedtls_x509_crt_parse_file(&tls_param->client_cert, tls_opt->cert_path)) != 0) {
            QCLOUD_LOG_E("load client cert file failed returned 0x%x", ret<0?-ret:ret);
            return QCLOUD_ERR_SSL_CERT;
        }
#endif

        if ((ret = mbedtls_pk_parse_keyfile(&tls_param->private_key, tls_opt->priv_key_path, "")) != 0) {
            QCLOUD_LOG_E("load client key file failed returned 0x%x", ret < 0 ? -ret : ret);
            return QCLOUD_ERR_SSL_CERT;
        }
    } else {
        QCLOUD_LOG_E("cert_file/key_file is empty!|cert_file=%s|key_file=%s", tls_opt->cert_path, tls_opt->priv_key_path);
    }
#else
	if (tls_opt->psk != NULL && tls_opt->psk_id != NULL) {
        const char *psk_id = tls_opt->psk_id;
        ret = mbedtls_ssl_conf_psk(&tls_param->ssl_conf, (unsigned char *)tls_opt->psk, tls_opt->psk_len,
                                    (const unsigned char *)psk_id, strlen(psk_id));
    } else {
        QCLOUD_LOG_D("psk/pskid is empty!|psk=%s|psd_id=%s", tls_opt->psk, tls_opt->psk_id);
    }

	if (0 != ret) {
		QCLOUD_LOG_E("mbedtls_ssl_conf_psk fail: -0x%x", -ret);
		return ret;
	}
#endif

    return QCLOUD_ERR_SUCCESS;
}


/**
 * @brief 建立TCP连接
 *
 * @param socket_fd  Socket描述符
 * @param host       服务器主机名
 * @param port       服务器端口地址
 * @return 返回QCLOUD_ERR_SUCCESS, 表示成功
 */
int mbedtls_tcp_connect(mbedtls_net_context *socket_fd, const char *host, int port)
{
    int ret = 0;
    char port_str[6];

    osal_snprintf(port_str, 6, "%d", port);

    if ((ret = mbedtls_net_connect(socket_fd, host, port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
        QCLOUD_LOG_E("tcp connect failed returned -0x%04x", -ret);

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

/**
 * @brief 在该函数中可对服务端证书进行自定义的校验
 *
 * 这种行为发生在握手过程中, 一般是校验连接服务器的主机名与服务器证书中的CN或SAN的域名信息是否一致
 * 不过, mbedtls库已经实现该功能, 可以参考函数 `mbedtls_x509_crt_verify_with_profile`
 *
 * @param hostname 连接服务器的主机名
 * @param crt x509格式的证书
 * @param depth
 * @param flags
 * @return
 */

#if defined(MBEDTLS_X509_CRT_PARSE_C)
static int qcloud_server_certificate_verify(void *hostname, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    return *flags;
}
#endif

__QCLOUD_OSAL__ void *osal_tls_connect(qcloud_tls_opt_t *tls_opt, const char *host, int port)
{
    int ret = 0;
    tls_param_t *tls_param = NULL;

    if (!tls_opt) {
        return NULL;
    }

    tls_param = (tls_param_t *)osal_malloc(sizeof(tls_param_t));
    if (!tls_param) {
        return NULL;
    }

    if ((ret = mbedtls_client_init(tls_param, tls_opt)) != QCLOUD_ERR_SUCCESS) {
        goto error;
    }

    QCLOUD_LOG_D(" Connecting to /%s/%d...", host, port);
    if ((ret = mbedtls_tcp_connect(&tls_param->socket_fd, host, port)) != QCLOUD_ERR_SUCCESS) {
        goto error;
    }

    QCLOUD_LOG_D(" Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(&tls_param->ssl_conf, MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        QCLOUD_LOG_E("mbedtls_ssl_config_defaults failed returned -0x%04x", -ret);
        goto error;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)

    mbedtls_ssl_conf_verify(&tls_param->ssl_conf, qcloud_server_certificate_verify, (void *)host);

    mbedtls_ssl_conf_authmode(&tls_param->ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#endif
    mbedtls_ssl_conf_rng(&tls_param->ssl_conf, mbedtls_ctr_drbg_random, &tls_param->ctr_drbg);


#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&tls_param->ssl_conf, &tls_param->ca_cert, NULL);
    if ((ret = mbedtls_ssl_conf_own_cert(&tls_param->ssl_conf,
                                         &tls_param->client_cert, &tls_param->private_key)) != 0) {
        QCLOUD_LOG_E("mbedtls_ssl_conf_own_cert failed returned 0x%04x", -ret);
        goto error;
    }
#endif

    mbedtls_ssl_conf_read_timeout(&tls_param->ssl_conf, tls_opt->timeout);
    if ((ret = mbedtls_ssl_setup(&tls_param->ssl, &tls_param->ssl_conf)) != 0) {
        QCLOUD_LOG_E("mbedtls_ssl_setup failed returned 0x%04x", -ret);
        goto error;
    }

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_KEY)
    // 选择加密套件代码，以后不通加密方式合并端口的时候可以用到
    if(tls_opt->psk != NULL) {
        mbedtls_ssl_conf_ciphersuites(&tls_param->ssl_conf, ciphersuites);
    }
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    // Set the hostname to check against the received server certificate and sni
    if ((ret = mbedtls_ssl_set_hostname(&tls_param->ssl, host)) != 0) {
        QCLOUD_LOG_E("mbedtls_ssl_set_hostname failed returned 0x%04x", -ret);
        goto error;
    }

#endif

    mbedtls_ssl_set_bio(&tls_param->ssl, &tls_param->socket_fd, mbedtls_net_send, mbedtls_net_recv,
                        mbedtls_net_recv_timeout);

    QCLOUD_LOG_D("Performing the SSL/TLS handshake...");
    while ((ret = mbedtls_ssl_handshake(&tls_param->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            QCLOUD_LOG_E("mbedtls_ssl_handshake failed returned 0x%04x", -ret);

#if defined(MBEDTLS_X509_CRT_PARSE_C)
            if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
                QCLOUD_LOG_E("Unable to verify the server's certificate");
            }
#endif
            goto error;
        }
    }

    if ((ret = mbedtls_ssl_get_verify_result(&tls_param->ssl)) != 0) {
        QCLOUD_LOG_E("mbedtls_ssl_get_verify_result failed returned 0x%04x", -ret);
        goto error;
    }

    mbedtls_ssl_conf_read_timeout(&tls_param->ssl_conf, 100);

    return tls_param;

error:
    mbedtls_tls_param_free(tls_param);
    return NULL;
}

__QCLOUD_OSAL__ void osal_tls_disconnect(void *handle)
{
    int ret = 0;
    tls_param_t *tls_param;

    if (!handle) {
        QCLOUD_LOG_D("handle is NULL");
        return;
    }

    tls_param = (tls_param_t *)handle;

    do {
        ret = mbedtls_ssl_close_notify(&tls_param->ssl);
    } while (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);

	mbedtls_tls_param_free(tls_param);
}

__QCLOUD_OSAL__ qcloud_err_t osal_tls_write(void *handle, const void *buf, size_t len, uint32_t timeout, size_t *write_len)
{
    QCLOUD_POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(write_len, QCLOUD_ERR_INVAL);

    osal_timer_t timer;
    tls_param_t *tls_param;
    int error = QCLOUD_FALSE;
    int write_rc = 0;
    size_t written_so_far;

    tls_param = (tls_param_t *)handle;

    osal_timer_init(&timer);
    osal_timer_countdown_ms(&timer, timeout);

    for (written_so_far = 0; written_so_far < len && !osal_timer_is_expired(&timer); written_so_far += write_rc) {
        while (!osal_timer_is_expired(&timer) &&
                (write_rc = mbedtls_ssl_write(&tls_param->ssl, (uint8_t *)buf + written_so_far, len - written_so_far)) <= 0) {
            if (write_rc != MBEDTLS_ERR_SSL_WANT_READ && write_rc != MBEDTLS_ERR_SSL_WANT_WRITE) {
                QCLOUD_LOG_E("tls_write failed 0x%04x", -write_rc);
                error = QCLOUD_TRUE;
                break;
            }
        }

        if (error) {
            break;
        }
    }

    *write_len = written_so_far;

    if (error) {
        return QCLOUD_ERR_SSL_WRITE;
    } else if (osal_timer_is_expired(&timer) && written_so_far != len) {
        return QCLOUD_ERR_SSL_WRITE_TIMEOUT;
    }

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_OSAL__ qcloud_err_t osal_tls_read(void *handle, void *buf, size_t len, uint32_t timeout, size_t *read_len)
{
    //mbedtls_ssl_conf_read_timeout(&(pParams->ssl_conf), timeout_ms); TODO:每次调用这个方法会导致read阻塞, 超时也不返回
    // 这里使用非阻塞的方式, 具体的超时操作由上层做
    QCLOUD_POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(read_len, QCLOUD_ERR_INVAL);

    osal_timer_t timer;
    tls_param_t *tls_param = NULL;

    *read_len = 0;

    osal_timer_init(&timer);
    osal_timer_countdown_ms(&timer, timeout);

    tls_param = (tls_param_t *)handle;

    do {
        int read_rc = 0;
        read_rc = mbedtls_ssl_read(&tls_param->ssl, (uint8_t *)buf + *read_len, len - *read_len);

        if (read_rc > 0) {
            *read_len += read_rc;
        } else if (read_rc == 0 ||
                    (read_rc != MBEDTLS_ERR_SSL_WANT_WRITE &&
                    read_rc != MBEDTLS_ERR_SSL_WANT_READ &&
                    read_rc != MBEDTLS_ERR_SSL_TIMEOUT)) {
            QCLOUD_LOG_E("tls_read failed: 0x%04x", -read_rc);
            return QCLOUD_ERR_SSL_READ;
        }

        if (osal_timer_is_expired(&timer)) {
            break;
        }
    } while (*read_len < len);

    if (len == *read_len) {
        return QCLOUD_ERR_SUCCESS;
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

