/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "dev_bind_internal.h"

#define AWSS_DEV_RAND_SIGN_FMT  ",\"random\":\"%s\",\"signMethod\":%d,\"sign\":\"%s\""
#define AWSS_DEV_BIND_TOKEN_FMT ",\"token\":\"%s\",\"remainTime\":%d,\"type\":%d"
#define AWSS_SUCCESS_FMT        ",\"type\":%d"
#define AWSS_DEV_INFO_FMT       "\"awssVer\":%s,\"productKey\":\"%s\",\"deviceName\":\"%s\",\"mac\":\"%s\",\"ip\":\"%s\",\"cipherType\":%d"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

static inline int bind_get_encrypt_type()
{
    return 3;
}

static void *awss_get_dev_info(void *dev_info, int len)
{
    char dev_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char mac_str[HAL_MAC_LEN + 1] = {0};
    char pk[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char ip_str[OS_IP_LEN + 1] = {0};

    if (dev_info == NULL || len <= 0) {
        return NULL;
    }

    HAL_GetProductKey(pk);
    HAL_GetDeviceName(dev_name);
    os_wifi_get_mac_str(mac_str);
    HAL_Wifi_Get_IP(ip_str, NULL);
#if 0
    awss_dict_crypt(NOTIFY_ENCODE_TABLE, (uint8_t *)pk, strlen(pk));
    awss_dict_crypt(NOTIFY_ENCODE_TABLE, (uint8_t *)dev_name, strlen(dev_name));
#endif
    HAL_Snprintf(dev_info, len - 1, AWSS_DEV_INFO_FMT, AWSS_VER, pk, dev_name, mac_str, ip_str,
                 bind_get_encrypt_type());

    return dev_info;
}

void *awss_build_dev_info(int type, void *dev_info, int info_len)
{
    int len = 0;
    char *buf = NULL;

    if (dev_info == NULL || info_len <= 0) {
        return NULL;
    }

    buf = os_zalloc(DEV_INFO_LEN_MAX);
    if (buf == NULL) {
        return NULL;
    }

    len += HAL_Snprintf((char *)dev_info + len, info_len - len - 1, "%s", (char *)awss_get_dev_info(buf, DEV_INFO_LEN_MAX));
    HAL_Free(buf);
    buf = NULL;

    switch (type) {
        case AWSS_NOTIFY_DEV_BIND_TOKEN: {
            char rand_str[(RANDOM_MAX_LEN << 1) + 1] = {0};
            utils_hex_to_str(aes_random, RANDOM_MAX_LEN, rand_str, sizeof(rand_str));
            len += HAL_Snprintf((char *)dev_info + len, info_len - len - 1, AWSS_DEV_BIND_TOKEN_FMT, rand_str,
                                awss_token_remain_time(), 0);
            break;
        }
#ifdef WIFI_PROVISION_ENABLED
        case AWSS_NOTIFY_SUCCESS: {
            len += HAL_Snprintf((char *)dev_info + len, info_len - len - 1, AWSS_SUCCESS_FMT, 0);
            break;
        }
        case AWSS_NOTIFY_DEV_RAND_SIGN: {
            char sign_str[DEV_SIGN_SIZE * 2 + 1] = {0};
            char rand_str[(RANDOM_MAX_LEN << 1) + 1] = {0};
            {
                int txt_len = 80;
                char txt[80] = {0};
                char key[IOTX_DEVICE_SECRET_LEN + 1] = {0};
                uint8_t sign[DEV_SIGN_SIZE + 1] = {0};

                if (bind_get_encrypt_type() == 3) { /* aes-key per product */
                    HAL_GetProductSecret(key);
                } else { /* aes-key per device */
                    HAL_GetDeviceSecret(key);
                }
                awss_build_sign_src(txt, &txt_len);
                produce_signature(sign, (uint8_t *)txt, txt_len, key);
                utils_hex_to_str(sign, DEV_SIGN_SIZE, sign_str, sizeof(sign_str));
            }
            utils_hex_to_str(aes_random, RANDOM_MAX_LEN, rand_str, sizeof(rand_str));
            len += HAL_Snprintf((char *)dev_info + len, info_len - len - 1, AWSS_DEV_RAND_SIGN_FMT, rand_str, 0, sign_str);
            break;
        }
#endif
        default:
            break;
    }

    return dev_info;
}

#ifdef WIFI_PROVISION_ENABLED
char *awss_build_sign_src(char *sign_src, int *sign_src_len)
{
    char *pk = NULL, *dev_name = NULL;
    int dev_name_len, pk_len, text_len;

    if (sign_src == NULL || sign_src_len == NULL) {
        goto build_sign_src_err;
    }

    pk = os_zalloc(IOTX_PRODUCT_KEY_LEN + 1);
    dev_name = os_zalloc(IOTX_DEVICE_NAME_LEN + 1);
    if (pk == NULL || dev_name == NULL) {
        goto build_sign_src_err;
    }

    HAL_GetProductKey(pk);
    HAL_GetDeviceName(dev_name);

    pk_len = strlen(pk);
    dev_name_len = strlen(dev_name);

    text_len = RANDOM_MAX_LEN + dev_name_len + pk_len;
    if (*sign_src_len < text_len) {
        goto build_sign_src_err;
    }

    *sign_src_len = text_len;

    memcpy(sign_src, aes_random, RANDOM_MAX_LEN);
    memcpy(sign_src + RANDOM_MAX_LEN, dev_name, dev_name_len);
    memcpy(sign_src + RANDOM_MAX_LEN + dev_name_len, pk, pk_len);

    HAL_Free(pk);
    HAL_Free(dev_name);

    return sign_src;

build_sign_src_err:
    if (pk) {
        HAL_Free(pk);
    }
    if (dev_name) {
        HAL_Free(dev_name);
    }
    return NULL;
}
#endif
const char *awss_build_topic(const char *topic_fmt, char *topic, uint32_t tlen)
{
    char pk[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char dev_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    if (topic == NULL || topic_fmt == NULL || tlen == 0) {
        return NULL;
    }

    HAL_GetProductKey(pk);
    HAL_GetDeviceName(dev_name);

    HAL_Snprintf(topic, tlen - 1, topic_fmt, pk, dev_name);

    return topic;
}

int awss_build_packet(int type, void *id, void *ver, void *method, void *data, int code, void *packet, int *packet_len)
{
    int len;
    if (packet_len == NULL || data == NULL || packet == NULL) {
        return -1;
    }

    len = *packet_len;
    if (len <= 0) {
        return -1;
    }

    if (type == AWSS_CMP_PKT_TYPE_REQ) {
        if (ver == NULL || method == NULL) {
            return -1;
        }

        len = HAL_Snprintf(packet, len - 1, AWSS_REQ_FMT, (char *)id, (char *)ver, (char *)method, (char *)data);
        return 0;
    } else if (type == AWSS_CMP_PKT_TYPE_RSP) {
        len = HAL_Snprintf(packet, len - 1, AWSS_ACK_FMT, (char *)id, code, (char *)data);
        return 0;
    }
    return -1;
}

void produce_random(uint8_t *random, uint32_t len)
{
    int i = 0;
    int time = HAL_UptimeMs();
    HAL_Srandom(time);
    for (i = 0; i < len; i ++) {
        random[i] = HAL_Random(0xFF);
    }
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
