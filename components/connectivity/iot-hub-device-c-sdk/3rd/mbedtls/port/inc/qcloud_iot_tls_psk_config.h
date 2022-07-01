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
 * @file qcloud_iot_tls_psk_config.h
 * @brief set config for tls psk
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

#ifndef IOT_HUB_DEVICE_C_SDK_3RD_MBEDTLS_PORT_INC_QCLOUD_IOT_TLS_PSK_CONFIG_H
#define IOT_HUB_DEVICE_C_SDK_3RD_MBEDTLS_PORT_INC_QCLOUD_IOT_TLS_PSK_CONFIG_H

#if defined(__cplusplus)
extern "C" {
#endif

/* System support */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_TIMING_ALT
// #define MBEDTLS_ENTROPY_HARDWARE_ALT
// #define MBEDTLS_NO_PLATFORM_ENTROPY

/* mbed TLS feature support */
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_PADDING_PKCS7
#define MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#define MBEDTLS_CIPHER_PADDING_ZEROS
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_MD_C
#define MBEDTLS_MD5_C
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ENTROPY_C

#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_SESSION_TICKETS
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_MAX_CONTENT_LEN 3584
#define MBEDTLS_SSL_CIPHERSUITES    MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA

#define MBEDTLS_TLS_DEFAULT_ALLOW_SHA1_IN_KEY_EXCHANGE

/* For testing with compat.sh */
#include "mbedtls/check_config.h"

#if defined(__cplusplus)
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_3RD_MBEDTLS_PORT_INC_QCLOUD_IOT_TLS_PSK_CONFIG_H
