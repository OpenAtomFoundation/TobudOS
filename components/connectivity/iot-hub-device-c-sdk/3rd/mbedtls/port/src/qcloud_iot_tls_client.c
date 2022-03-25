/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file qcloud_iot_tls_client.c
 * @brief implements tls client with mbedtls
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-12
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-12 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_tls_client.h"

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"

#ifdef AUTH_MODE_KEY
/**
 * @brief Only tls psk is supportted when using psk
 *
 */
static const int ciphersuites[] = {MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA, 0};
#endif

/**
 * @brief Data structure for mbedtls SSL connection
 *
 */
typedef struct {
    mbedtls_net_context      socket_fd;
    mbedtls_entropy_context  entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context      ssl;
    mbedtls_ssl_config       ssl_conf;
#ifdef AUTH_MODE_CERT
    mbedtls_x509_crt ca_cert;
    mbedtls_x509_crt client_cert;
#endif
    mbedtls_pk_context private_key;
} TLSHandle;

#ifdef MBEDTLS_DEBUG_C
#define DEBUG_LEVEL 0
static void _ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    Log_i("[mbedTLS]:[%s]:[%d]: %s\r\n", STRING_PTR_PRINT_SANITY_CHECK(file), line, STRING_PTR_PRINT_SANITY_CHECK(str));
}
#endif

/**
 * @brief mbedtls SSL client init
 *
 * 1. call a series of mbedtls init functions
 * 2. init and set seed for random functions
 * 3. load CA file, cert files or PSK
 *
 * @param[in,out] tls_handle mbedtls TLS handle
 * @param[in] connect_params device info for TLS connection
 * @return @see IotReturnCode
 */
static int _mbedtls_tls_client_init(TLSHandle *tls_handle, const TLSConnectParams *connect_params)
{
    int rc;

    mbedtls_net_init(&tls_handle->socket_fd);
    mbedtls_ssl_init(&tls_handle->ssl);
    mbedtls_ssl_config_init(&tls_handle->ssl_conf);
    mbedtls_ctr_drbg_init(&tls_handle->ctr_drbg);
    mbedtls_entropy_init(&tls_handle->entropy);
#ifdef AUTH_MODE_CERT
    mbedtls_x509_crt_init(&tls_handle->ca_cert);
    mbedtls_x509_crt_init(&tls_handle->client_cert);
    mbedtls_pk_init(&tls_handle->private_key);
#endif
#ifdef MBEDTLS_DEBUG_C
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
    mbedtls_ssl_conf_dbg(&tls_handle->ssl_conf, _ssl_debug, NULL);
#endif

    rc = mbedtls_ctr_drbg_seed(&tls_handle->ctr_drbg, mbedtls_entropy_func, &tls_handle->entropy, NULL, 0);
    if (rc) {
        Log_e("mbedtls_ctr_drbg_seed failed returned 0x%04x", -rc);
        return QCLOUD_ERR_SSL_INIT;
    }

#ifdef AUTH_MODE_CERT
    if (!connect_params->cert_file || !connect_params->key_file || !connect_params->ca_crt) {
        Log_d("cert_file/key_file/ca is empty!|cert_file=%s|key_file=%s|ca=%s",
              STRING_PTR_PRINT_SANITY_CHECK(connect_params->cert_file),
              STRING_PTR_PRINT_SANITY_CHECK(connect_params->key_file),
              STRING_PTR_PRINT_SANITY_CHECK(connect_params->ca_crt));
        return QCLOUD_ERR_SSL_CERT;
    }

    rc = mbedtls_x509_crt_parse(&tls_handle->ca_cert, (const unsigned char *)connect_params->ca_crt,
                                (connect_params->ca_crt_len + 1));
    if (rc) {
        Log_e("parse ca crt failed returned 0x%04x", -rc);
        return QCLOUD_ERR_SSL_CERT;
    }

    rc = mbedtls_x509_crt_parse_file(&tls_handle->client_cert, connect_params->cert_file);
    if (rc) {
        Log_e("load client cert file failed returned 0x%04x", -rc);
        return QCLOUD_ERR_SSL_CERT;
    }

    rc = mbedtls_pk_parse_keyfile(&tls_handle->private_key, connect_params->key_file, "");
    if (rc) {
        Log_e("load client key file failed returned 0x%04x", -rc);
        return QCLOUD_ERR_SSL_CERT;
    }
#else
    if (!connect_params->psk || !connect_params->psk_id) {
        Log_d("psk/psk_id is empty!");
        return QCLOUD_ERR_SSL_INIT;
    }

    rc = mbedtls_ssl_conf_psk(&tls_handle->ssl_conf, (unsigned char *)connect_params->psk, connect_params->psk_length,
                              (const unsigned char *)connect_params->psk_id, strlen(connect_params->psk_id));
    if (rc) {
        Log_e("mbedtls_ssl_conf_psk fail 0x%04x", -rc);
        return rc;
    }
#endif
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Free memory/resources allocated by mbedtls
 *
 * @param[in,out] tls_handle @see TLSHandle
 */
static void _mbedtls_tls_client_free(TLSHandle *tls_handle)
{
    mbedtls_net_free(&(tls_handle->socket_fd));
#ifdef AUTH_MODE_CERT
    mbedtls_x509_crt_free(&tls_handle->client_cert);
    mbedtls_x509_crt_free(&tls_handle->ca_cert);
    mbedtls_pk_free(&tls_handle->private_key);
#endif
    mbedtls_ssl_free(&tls_handle->ssl);
    mbedtls_ssl_config_free(&tls_handle->ssl_conf);
    mbedtls_ctr_drbg_free(&tls_handle->ctr_drbg);
    mbedtls_entropy_free(&tls_handle->entropy);
    HAL_Free(tls_handle);
}

#ifdef AUTH_MODE_CERT
/**
 * @brief verify server certificate
 *
 * mbedtls has provided similar function mbedtls_x509_crt_verify_with_profile
 */
int _mbedtls_tls_client_certificate_verify(void *hostname, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    return *flags;
}
#endif

/**
 * @brief Tls setup and sharkhand
 *
 * @param[in] connect_params connect params of tls
 * @param[in] host server host
 * @param[in] port server port
 * @return tls handle, 0 for fail
 */
uintptr_t qcloud_iot_tls_client_connect(const TLSConnectParams *connect_params, const char *host, const char *port)
{
    int rc = 0;

    TLSHandle *tls_handle = (TLSHandle *)HAL_Malloc(sizeof(TLSHandle));
    if (!tls_handle) {
        return 0;
    }

    rc = _mbedtls_tls_client_init(tls_handle, connect_params);
    if (rc) {
        goto error;
    }

    Log_d("Setting up the SSL/TLS structure...");
    rc = mbedtls_ssl_config_defaults(&tls_handle->ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                     MBEDTLS_SSL_PRESET_DEFAULT);
    if (rc) {
        Log_e("mbedtls_ssl_config_defaults failed returned 0x%04x", -rc);
        goto error;
    }

    mbedtls_ssl_conf_rng(&tls_handle->ssl_conf, mbedtls_ctr_drbg_random, &tls_handle->ctr_drbg);
#ifdef AUTH_MODE_CERT
    mbedtls_ssl_conf_verify(&(tls_handle->ssl_conf), _mbedtls_tls_client_certificate_verify, (void *)host);
    mbedtls_ssl_conf_authmode(&(tls_handle->ssl_conf), MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&tls_handle->ssl_conf, &tls_handle->ca_cert, NULL);
    rc = mbedtls_ssl_conf_own_cert(&tls_handle->ssl_conf, &tls_handle->client_cert, &tls_handle->private_key);
    if (rc) {
        Log_e("mbedtls_ssl_conf_own_cert failed returned 0x%04x", -rc);
        goto error;
    }
#endif
    mbedtls_ssl_conf_read_timeout(&tls_handle->ssl_conf, connect_params->timeout_ms);
    rc = mbedtls_ssl_setup(&tls_handle->ssl, &tls_handle->ssl_conf);
    if (rc) {
        Log_e("mbedtls_ssl_setup failed returned 0x%04x", -rc);
        goto error;
    }
#ifdef AUTH_MODE_CERT
    // Set the hostname to check against the received server certificate and sni
    rc = mbedtls_ssl_set_hostname(&tls_handle->ssl, host);
    if (rc) {
        Log_e("mbedtls_ssl_set_hostname failed returned 0x%04x", -rc);
        goto error;
    }
#else
    // ciphersuites selection for PSK device
    if (connect_params->psk) {
        mbedtls_ssl_conf_ciphersuites(&(tls_handle->ssl_conf), ciphersuites);
    }
#endif
    mbedtls_ssl_set_bio(&tls_handle->ssl, &tls_handle->socket_fd, mbedtls_net_send, mbedtls_net_recv,
                        mbedtls_net_recv_timeout);
    Log_d("Performing the SSL/TLS handshake...");
    Log_d("Connecting to /%s/%s...", STRING_PTR_PRINT_SANITY_CHECK(host), STRING_PTR_PRINT_SANITY_CHECK(port));
    rc = mbedtls_net_connect(&tls_handle->socket_fd, host, port, MBEDTLS_NET_PROTO_TCP);
    if (rc < 0) {
        goto error;
    }

    do {
        rc = mbedtls_ssl_handshake(&tls_handle->ssl);
        if (rc && rc != MBEDTLS_ERR_SSL_WANT_READ && rc != MBEDTLS_ERR_SSL_WANT_WRITE) {
            Log_e("mbedtls_ssl_handshake failed returned 0x%04x", -rc);

#ifdef AUTH_MODE_CERT
            if (rc == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
                Log_e("Unable to verify the server's certificate");
            }
#endif
            goto error;
        }
    } while (rc);

    rc = mbedtls_ssl_get_verify_result(&(tls_handle->ssl));
    if (rc) {
        Log_e("mbedtls_ssl_get_verify_result failed returned 0x%04x", -rc);
        goto error;
    }

    mbedtls_ssl_conf_read_timeout(&tls_handle->ssl_conf, 200);

    Log_d("connected with /%s/%s...", STRING_PTR_PRINT_SANITY_CHECK(host), port);
    return (uintptr_t)tls_handle;

error:
    _mbedtls_tls_client_free(tls_handle);
    return 0;
}

/**
 * @brief Disconect and free
 *
 * @param[in,out] handle tls handle
 */
void qcloud_iot_tls_client_disconnect(uintptr_t handle)
{
    int rc = 0;

    TLSHandle *tls_handle = (TLSHandle *)handle;
    if (!tls_handle) {
        Log_d("handle is NULL");
        return;
    }

    do {
        rc = mbedtls_ssl_close_notify(&tls_handle->ssl);
    } while (rc == MBEDTLS_ERR_SSL_WANT_READ || rc == MBEDTLS_ERR_SSL_WANT_WRITE);
    _mbedtls_tls_client_free(tls_handle);
}

/**
 * @brief Write msg with tls
 *
 * @param[in,out] handle tls handle
 * @param[in] msg msg to write
 * @param[in] total_len number of bytes to write
 * @param[in] timeout_ms timeout millsecond
 * @param[out] written_len number of bytes writtern
 * @return @see IotReturnCode
 */
int qcloud_iot_tls_client_write(uintptr_t handle, unsigned char *msg, size_t total_len, uint32_t timeout_ms,
                                size_t *written_len)
{
    Timer  timer;
    size_t written_so_far;
    int    write_rc = 0;

    TLSHandle *tls_handle = (TLSHandle *)handle;
    HAL_Timer_CountdownMs(&timer, (unsigned int)timeout_ms);

    for (written_so_far = 0; written_so_far < total_len && !HAL_Timer_Expired(&timer); written_so_far += write_rc) {
        do {
            write_rc = mbedtls_ssl_write(&tls_handle->ssl, msg + written_so_far, total_len - written_so_far);
            if (write_rc < 0 && write_rc != MBEDTLS_ERR_SSL_WANT_READ && write_rc != MBEDTLS_ERR_SSL_WANT_WRITE) {
                Log_e("HAL_TLS_write failed 0x%04x", -write_rc);
                return QCLOUD_ERR_SSL_WRITE;
            }

            if (HAL_Timer_Expired(&timer)) {
                break;
            }
        } while (write_rc <= 0);
    }

    *written_len = written_so_far;
    if (HAL_Timer_Expired(&timer) && written_so_far != total_len) {
        return QCLOUD_ERR_SSL_WRITE_TIMEOUT;
    }
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Read msg with tls
 *
 * @param[in,out] handle tls handle
 * @param[out] msg msg buffer
 * @param[in] total_len buffer len
 * @param[in] timeout_ms timeout millsecond
 * @param[out] read_len number of bytes read
 * @return @see IotReturnCode
 */
int qcloud_iot_tls_client_read(uintptr_t handle, unsigned char *msg, size_t total_len, uint32_t timeout_ms,
                               size_t *read_len)
{
    Timer timer;
    int   read_rc;

    TLSHandle *tls_handle = (TLSHandle *)handle;
    HAL_Timer_CountdownMs(&timer, timeout_ms);

    *read_len = 0;

    do {
        read_rc = mbedtls_ssl_read(&tls_handle->ssl, msg + *read_len, total_len - *read_len);
        if (read_rc <= 0 && read_rc != MBEDTLS_ERR_SSL_WANT_WRITE && read_rc != MBEDTLS_ERR_SSL_WANT_READ &&
            read_rc != MBEDTLS_ERR_SSL_TIMEOUT) {
            Log_e("cloud_iot_network_tls_read failed: 0x%04x", -read_rc);
            return QCLOUD_ERR_SSL_READ;
        }
        *read_len += read_rc > 0 ? read_rc : 0;

        if (HAL_Timer_Expired(&timer)) {
            break;
        }
    } while (*read_len < total_len);

    if (*read_len > 0) {
        return QCLOUD_RET_SUCCESS;
    }

    return *read_len == 0 ? QCLOUD_ERR_SSL_NOTHING_TO_READ : QCLOUD_ERR_SSL_READ_TIMEOUT;
}

#ifdef __cplusplus
}
#endif
