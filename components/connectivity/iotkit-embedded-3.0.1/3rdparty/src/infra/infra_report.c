/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "infra_config.h"

#ifdef INFRA_REPORT

#include <stdio.h>
#include <string.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_string.h"
#include "infra_report.h"

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt, ...);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN]);
uint64_t HAL_UptimeMs(void);
int HAL_GetFirmwareVersion(char *version);

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define SYS_REPORT_MALLOC(size) LITE_malloc(size, MEM_MAGIC, "sys.report")
    #define SYS_REPORT_FREE(ptr)    LITE_free(ptr)
#else
    #define SYS_REPORT_MALLOC(size) HAL_Malloc(size)
    #define SYS_REPORT_FREE(ptr)    HAL_Free(ptr)
#endif

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define VERSION_DEBUG(...)  log_debug("version", __VA_ARGS__)
    #define VERSION_ERR(...)    log_err("version", __VA_ARGS__)
#else
    #define VERSION_DEBUG(...)  do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
    #define VERSION_ERR(...)    do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#endif

static unsigned int g_report_id = 0;

int iotx_report_id(void)
{
    return g_report_id++;
}

static info_report_func_pt info_report_func = NULL;

void iotx_set_report_func(info_report_func_pt func)
{
    info_report_func = func;
}
/*  aos will implement this function */
#if defined(BUILD_AOS)
extern void aos_get_version_hex(unsigned char version[VERSION_NUM_SIZE]);
#else
void aos_get_version_hex(unsigned char version[VERSION_NUM_SIZE])
{
    const char *p_version = IOTX_SDK_VERSION;
    int i = 0, j = 0;
    unsigned char res = 0;

    for (j = 0; j < 3; j++) {
        for (res = 0; p_version[i] <= '9' && p_version[i] >= '0'; i++) {
            res = res * 10 + p_version[i] - '0';
        }
        version[j] = res;
        i++;
    }
    version[3] = 0x00;
}
#endif



/*  aos will implement this function */
#if defined(BUILD_AOS)
extern void aos_get_mac_hex(unsigned char mac[MAC_ADDRESS_SIZE]);
#else
void aos_get_mac_hex(unsigned char mac[MAC_ADDRESS_SIZE])
{
    memcpy(mac, "\x01\x02\x03\x04\x05\x06\x07\x08", MAC_ADDRESS_SIZE);
}
#endif

/*  aos will implement this function */
#if defined(BUILD_AOS)
extern void aos_get_chip_code(unsigned char chip_code[CHIP_CODE_SIZE]);
#else
void aos_get_chip_code(unsigned char chip_code[CHIP_CODE_SIZE])
{
    memcpy(chip_code, "\x01\x02\x03\x04", CHIP_CODE_SIZE);
}
#endif

const char *DEVICE_INFO_UPDATE_FMT = "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":["
                                     "{\"attrKey\":\"SYS_LP_SDK_VERSION\",\"attrValue\":\"%s\",\"domain\":\"SYSTEM\"},"
                                     "{\"attrKey\":\"SYS_SDK_LANGUAGE\",\"attrValue\":\"C\",\"domain\":\"SYSTEM\"}"
                                     "],\"method\":\"thing.deviceinfo.update\"}";

int iotx_report_devinfo(void *pclient)
{
    int ret = 0;
    char topic_name[IOTX_URI_MAX_LEN + 1] = {0};
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char *msg = NULL;
    int  msg_len = 0;


    if (info_report_func == NULL) {
        VERSION_ERR("report func not register!");
        return -1;
    }

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    VERSION_DEBUG("devinfo report");

    /* devinfo update topic name */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/sys/%s/%s/thing/deviceinfo/update",
                       product_key,
                       device_name);
    if (ret <= 0) {
        VERSION_ERR("topic generate err");
        return FAIL_RETURN;
    }
    VERSION_DEBUG("devinfo report topic: %s", topic_name);

    msg_len = strlen(DEVICE_INFO_UPDATE_FMT) + 10 + strlen(IOTX_SDK_VERSION) + 1;
    msg = (char *)SYS_REPORT_MALLOC(msg_len);
    if (msg == NULL) {
        VERSION_ERR("malloc err");
        return FAIL_RETURN;
    }
    memset(msg, 0, msg_len);

    /* devinfo update message */
    ret = HAL_Snprintf(msg,
                       msg_len,
                       DEVICE_INFO_UPDATE_FMT,
                       iotx_report_id(),
                       IOTX_SDK_VERSION
                      );
    if (ret <= 0) {
        VERSION_ERR("topic msg generate err");
        SYS_REPORT_FREE(msg);
        return FAIL_RETURN;
    }
    VERSION_DEBUG("devinfo report data: %s", msg);

    if (info_report_func != NULL) {
        info_report_func(pclient, topic_name, 1, msg, strlen(msg));
    }

    SYS_REPORT_FREE(msg);
    if (ret < 0) {
        VERSION_ERR("publish failed, ret = %d", ret);
        return FAIL_RETURN;
    }
    VERSION_DEBUG("devinfo report succeed");

    return SUCCESS_RETURN;
}

/* report Firmware version */
int iotx_report_firmware_version(void *pclient)
{
    int ret;
    char topic_name[IOTX_URI_MAX_LEN + 1] = {0};
    char msg[FIRMWARE_VERSION_MSG_LEN] = {0};
    char version[IOTX_FIRMWARE_VERSION_LEN + 1] = {0};
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};

    if (info_report_func == NULL) {
        VERSION_ERR("report func not register!");
        return -1;
    }

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

    ret = HAL_GetFirmwareVersion(version);
    if (ret <= 0) {
        VERSION_ERR("firmware version does not implement");
        return FAIL_RETURN;
    }

    VERSION_DEBUG("firmware version report start in MQTT");

    /* firmware report topic name generate */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/ota/device/inform/%s/%s",
                       product_key,
                       device_name
                      );
    if (ret <= 0) {
        VERSION_ERR("firmware report topic generate err");
        return FAIL_RETURN;
    }
    VERSION_DEBUG("firmware report topic: %s", topic_name);

    /* firmware report message json data generate */
    ret = HAL_Snprintf(msg,
                       FIRMWARE_VERSION_MSG_LEN,
                       "{\"id\":\"%d\",\"params\":{\"version\":\"%s\"}}",
                       iotx_report_id(),
                       version
                      );
    if (ret <= 0) {
        VERSION_ERR("firmware report message json data generate err");
        return FAIL_RETURN;
    }
    VERSION_DEBUG("firmware report data: %s", msg);

    ret = info_report_func(pclient, topic_name, 1, msg, strlen(msg));

    if (ret < 0) {
        VERSION_ERR("publish failed, ret = %d", ret);
        return ret;
    }

    VERSION_DEBUG("firmware version report finished, iotx_publish() = %d", ret);
    return SUCCESS_RETURN;
}

/* report ModuleID */
int iotx_report_mid(void *pclient)
{
    return SUCCESS_RETURN;
}

#ifndef BUILD_AOS
unsigned int aos_get_version_info(unsigned char version_num[VERSION_NUM_SIZE],
                                  unsigned char random_num[RANDOM_NUM_SIZE], unsigned char mac_address[MAC_ADDRESS_SIZE],
                                  unsigned char chip_code[CHIP_CODE_SIZE], unsigned char *output_buffer, unsigned int output_buffer_size)
{
    char *p = (char *)output_buffer;

    if (output_buffer_size < AOS_ACTIVE_INFO_LEN) {
        return 1;
    }

    memset(p, 0, output_buffer_size);

    infra_hex2str(version_num, VERSION_NUM_SIZE, p);
    p += VERSION_NUM_SIZE * 2;
    infra_hex2str(random_num, RANDOM_NUM_SIZE, p);
    p += RANDOM_NUM_SIZE * 2;
    infra_hex2str(mac_address, MAC_ADDRESS_SIZE, p);
    p += MAC_ADDRESS_SIZE * 2;
    infra_hex2str(chip_code, CHIP_CODE_SIZE, p);
    p += CHIP_CODE_SIZE * 2;
    strcat(p, "1111111111222222222233333333334444444444");

    return 0;
}
#endif
#endif

