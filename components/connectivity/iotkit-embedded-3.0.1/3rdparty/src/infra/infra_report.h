/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "infra_defs.h"

#ifndef _INFRA_REPORT_H_
#define _INFRA_REPORT_H_

#ifndef VERSION_NUM_SIZE
    #define VERSION_NUM_SIZE    4
#endif

#ifndef RANDOM_NUM_SIZE
    #define RANDOM_NUM_SIZE     4
#endif

#ifndef MAC_ADDRESS_SIZE
    #define MAC_ADDRESS_SIZE    8
#endif

#ifndef CHIP_CODE_SIZE
    #define CHIP_CODE_SIZE      4
#endif

#define AOS_ACTIVE_INFO_LEN (81)

/*  activation device type */
typedef enum {
    ACTIVE_SUBDEV,              /* it's a subDevice */
    ACTIVE_SINGLE_GW            /* it s a single or gateway device */
} active_device_type_t;

/*  activation system type */
typedef enum {
    ACTIVE_LINKKIT_ONLY,        /* only linkkit implement */
    ACTIVE_LINKKIT_AOS,         /* both linkkit and AOS implement */
    ACTIVE_LINKKIT_OTHERS       /* linkkit and 3-party OS implement */
} active_system_type_t;

typedef int (*info_report_func_pt)(void *handle, const char *topic_name,int req_ack,void *data, int len); 

#define MIDREPORT_PAYLOAD_LEN             (62 + IOTX_PARTNER_ID_LEN + IOTX_MODULE_ID_LEN + 32 +1)
#define MIDREPORT_REQID_LEN               (IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 6)
#define AOS_VERSON_MSG_LEN                (256)
#define LINKKIT_VERSION_MSG_LEN           (192)
#define FIRMWARE_VERSION_MSG_LEN          (64)
#define DEBUG_REPORT_MID_DEVINFO_FIRMWARE (1)

int     iotx_report_id(void);
int     iotx_midreport_reqid(char *requestId, char *product_key, char *device_name);
int     iotx_midreport_payload(char *msg, char *requestId, char *mid, char *pid);
int     iotx_midreport_topic(char *topic_name, char *topic_head, char *product_key, char *device_name);

/* AOS version report API */
int     iotx_gen_aos_report_topic(char *topic_name, char *product_key, char *device_name);
int     iotx_gen_aos_report_payload(char *msg, int requestId, char *versionData);

void aos_get_version_hex(unsigned char version[VERSION_NUM_SIZE]);

#ifndef BUILD_AOS
unsigned int aos_get_version_info(unsigned char version_num[VERSION_NUM_SIZE],
                                  unsigned char random_num[RANDOM_NUM_SIZE], unsigned char mac_address[MAC_ADDRESS_SIZE],
                                  unsigned char chip_code[CHIP_CODE_SIZE], unsigned char *output_buffer, unsigned int output_buffer_size);
#endif

void iotx_set_report_func(info_report_func_pt func);
int iotx_report_devinfo(void *pclient);
int iotx_report_mid(void *pclient);
int iotx_report_firmware_version(void *pclient);

#endif

