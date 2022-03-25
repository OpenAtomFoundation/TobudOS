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
 * @file qcloud_iot_device.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_DEVICE_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief MAX size of client ID.
 *
 */
#define MAX_SIZE_OF_CLIENT_ID (80)

/**
 * @brief MAX size of product ID.
 *
 */
#define MAX_SIZE_OF_PRODUCT_ID (10)

/**
 * @brief MAX size of product secret.
 *
 */
#define MAX_SIZE_OF_PRODUCT_SECRET (32)

/**
 * @brief MAX size of device name.
 *
 */
#define MAX_SIZE_OF_DEVICE_NAME (48)

/**
 * @brief MAX size of device secret.
 *
 */
#define MAX_SIZE_OF_DEVICE_SECRET (64)

/**
 * @brief MAX size of device cert file name.
 *
 */
#define MAX_SIZE_OF_DEVICE_CERT_FILE_NAME (128)

/**
 * @brief MAX size of device key file name.
 *
 */
#define MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME (128)

/**
 * @brief Max size of base64 encoded PSK = 64, after decode: 64/4*3 = 48.
 *
 */
#define MAX_SIZE_OF_DECODE_PSK_LENGTH 48

/**
 * @brief Device info needed to connect mqtt server.
 *
 */
typedef struct {
    char product_id[MAX_SIZE_OF_PRODUCT_ID + 1];
    char device_name[MAX_SIZE_OF_DEVICE_NAME + 1];
    char client_id[MAX_SIZE_OF_CLIENT_ID + 1];

#ifdef AUTH_MODE_CERT
    char dev_cert_file_name[MAX_SIZE_OF_DEVICE_CERT_FILE_NAME + 1];
    char dev_key_file_name[MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME + 1];
#else
    char    device_secret[MAX_SIZE_OF_DEVICE_SECRET + 1];
    uint8_t device_secret_decode[MAX_SIZE_OF_DECODE_PSK_LENGTH];
    size_t  device_secret_decode_len;
#endif

#ifdef DEV_DYN_REG_ENABLED
    char product_secret[MAX_SIZE_OF_PRODUCT_SECRET + 1];
#endif
} DeviceInfo;

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_DEVICE_H_
