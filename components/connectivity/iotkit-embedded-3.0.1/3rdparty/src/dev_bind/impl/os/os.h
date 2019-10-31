/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_OS_H__
#define __AWSS_OS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <ctype.h>
#include "infra_defs.h"
#include <stdio.h>
#include <string.h>
#include "infra_compat.h"

#ifndef _IN_OPT_
#define _IN_OPT_
#endif
#ifndef _OUT_
#define _OUT_
#endif
#ifndef _OUT_OPT_
#define _OUT_OPT_
#endif
#ifndef _INOUT_
#define _INOUT_
#endif
#ifndef _INOUT_OPT_
#define _INOUT_OPT_
#endif

/** @defgroup group_os os
 *  @{
 */
#define OS_MAC_LEN    HAL_MAC_LEN
#define OS_ETH_ALEN   ETH_ALEN
#define OS_IP_LEN (NETWORK_ADDR_LEN)
#define OS_MAX_PASSWD_LEN  HAL_MAX_PASSWD_LEN
#define OS_MAX_SSID_LEN HAL_MAX_SSID_LEN
#define OS_PRODUCT_KEY_LEN      IOTX_PRODUCT_KEY_LEN
#define OS_PRODUCT_SECRET_LEN   IOTX_PRODUCT_SECRET_LEN
#define OS_DEVICE_NAME_LEN      IOTX_DEVICE_NAME_LEN
#define OS_DEVICE_SECRET_LEN    IOTX_DEVICE_SECRET_LEN
#define PLATFORM_AES_DECRYPTION HAL_AES_DECRYPTION
#define PLATFORM_AES_ENCRYPTION HAL_AES_ENCRYPTION
#define PLATFORM_MAX_SSID_LEN HAL_MAX_SSID_LEN
#define PLATFORM_MAX_PASSWD_LEN HAL_MAX_PASSWD_LEN
typedef void *p_HAL_Aes128_t;
#define p_aes128_t p_HAL_Aes128_t

char *os_wifi_get_mac_str(char mac_str[HAL_MAC_LEN]);
char *os_wifi_str2mac(char mac_str[HAL_MAC_LEN], char mac[ETH_ALEN]);
uint8_t *os_wifi_get_mac(uint8_t mac[ETH_ALEN]);
uint32_t os_get_time_ms(void);

int os_is_big_endian(void);
uint16_t os_htobe16(uint16_t data);
uint16_t os_htole16(uint16_t data);
uint16_t os_be16toh(uint16_t data);
uint16_t os_le16toh(uint16_t data);
uint32_t os_le32toh(uint32_t data);
uint16_t os_get_unaligned_be16(uint8_t *ptr);
uint16_t os_get_unaligned_le16(uint8_t *ptr);
uint32_t os_get_unaligned_be32(uint8_t *ptr);
uint32_t os_get_unaligned_le32(uint8_t *ptr);
void *os_zalloc(uint32_t size);
uint32_t time_elapsed_ms_since(uint32_t start_timestamp);
#ifdef __cplusplus
}
#endif

#endif /* SRC_OSA_ABSTRAC_H_ */
