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
 * @file qcloud_iot_tls_client.h
 * @brief header file for tls client
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

#ifndef IOT_HUB_DEVICE_C_SDK_3RD_MBEDTLS_PORT_INC_QCLOUD_IOT_TLS_CLIENT_H_
#define IOT_HUB_DEVICE_C_SDK_3RD_MBEDTLS_PORT_INC_QCLOUD_IOT_TLS_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_platform.h"

#ifndef MAX_SIZE_OF_CLIENT_ID
#define MAX_SIZE_OF_CLIENT_ID (80)
#endif

/**
 * @brief Define structure for TLS connection parameters
 *
 */
typedef struct {
    const char *ca_crt;
    uint16_t    ca_crt_len;

#ifdef AUTH_MODE_CERT
    /**
     * Device with certificate
     */
    const char *cert_file;  // public certificate file
    const char *key_file;   // pravite certificate file
#else
    /**
     * Device with PSK
     */
    const char *psk;     // PSK string
    const char *psk_id;  // PSK ID
#endif

    size_t psk_length;  // PSK length

    unsigned int timeout_ms;  // SSL handshake timeout in millisecond

} SSLConnectParams;

typedef SSLConnectParams TLSConnectParams;

/**
 * @brief Tls setup and sharkhand
 *
 * @param[in] connect_params connect params of tls
 * @param[in] host server host
 * @param[in] port server port
 * @return tls handle, 0 for fail
 */
uintptr_t qcloud_iot_tls_client_connect(const TLSConnectParams *connect_params, const char *host, const char *port);

/**
 * @brief Disconect and free
 *
 * @param[in,out] handle tls handle
 */
void qcloud_iot_tls_client_disconnect(uintptr_t handle);

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
                                size_t *written_len);

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
                               size_t *read_len);

#if defined(__cplusplus)
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_3RD_MBEDTLS_PORT_INC_QCLOUD_IOT_TLS_CLIENT_H_
