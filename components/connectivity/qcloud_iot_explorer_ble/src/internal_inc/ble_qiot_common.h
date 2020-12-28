/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef QCLOUD_BLE_QIOT_COMMON_H
#define QCLOUD_BLE_QIOT_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_QIOT_PRODUCT_ID_LEN  (10)  // fixed length of product id
#define BLE_QIOT_DEVICE_NAME_LEN (48)  // max length of device name
#define BLE_QIOT_PSK_LEN         (24)  // fixed length of secret key
#define BLE_QIOT_MAC_LEN         (6)   // fixed length of mac

#define SWAP_32(x) \
    ((((x)&0xFF000000) >> 24) | (((x)&0x00FF0000) >> 8) | (((x)&0x0000FF00) << 8) | (((x)&0x000000FF) << 24))
#define SWAP_16(x) ((((x)&0xFF00) >> 8) | (((x)&0x00FF) << 8))
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define HTONL(x) SWAP_32(x)
#define HTONS(x) SWAP_16(x)
#define NTOHL(x) SWAP_32(x)
#define NTOHS(x) SWAP_16(x)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define HTONL(x) (x)
#define HTONS(x) (x)
#define NTOHL(x) (x)
#define NTOHS(x) (x)
#else
#error "undefined byte order"
#endif

#ifdef __cplusplus
}
#endif

#endif  // QCLOUD_BLE_QIOT_COMMON_H
