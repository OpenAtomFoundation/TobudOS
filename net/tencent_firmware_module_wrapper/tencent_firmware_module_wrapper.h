/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _TENCENT_FIRMWARE_MODULE_WRAPPER_H_
#define  _TENCENT_FIRMWARE_MODULE_WRAPPER_H_

#include <stdint.h>
#include <stdio.h>
#include "tos_k.h"

#define AUTH_MODE_KEY       1
#define AUTH_MODE_CERT      2

#define CFG_AUTH_MODE       AUTH_MODE_KEY


#define MQTT_COMMAND_TIMEOUT                (5 * 1000)

#define PRODUCT_ID_MAX_SIZE                 10

#define DEVICE_NAME_MAX_SIZE                64

#define DEVICE_SERC_MAX_SIZE                25

#define DEVICE_CERT_FILE_NAME_MAX_SIZE      128

#define DEVICE_KEY_FILE_NAME_MAX_SIZE       128

#define TOPIC_NAME_MAX_SIZE                      ((DEVICE_NAME_MAX_SIZE) + (PRODUCT_ID_MAX_SIZE) + 64)

#define PUB_PAYLOAD_MAX_SIZE                200

#define MQTT_MESSAGE_HANDLE_TASK_STACK_SIZE 2048
#define MQTT_MESSAGE_HANDLE_TASK_PRIO       3
#define MQTT_MESSAGE_NUM_MAX                3
#define MQTT_MESSAGE_POOL_SIZE              MQTT_MESSAGE_NUM_MAX*sizeof(mqtt_message_t)

#define MQTT_SUB_TOPIC_MAX                  5
#define MQTT_SUB_TOPIC_HANDLES_POOL_SIZE    MQTT_SUB_TOPIC_MAX * sizeof(mqtt_message_handlers_t)

typedef enum mqtt_state_en {
    MQTT_STATE_DISCONNECTED,
    MQTT_STATE_CONNECTED,
} mqtt_state_t;

typedef enum tls_mode_en {
    TLS_MODE_NO     = 0,
    TLS_MODE_PSK    = 1,
    TLS_MODE_CERT   = 2,
} tls_mode_t;

typedef enum quality_of_service_en {
    QOS0 = 0,
    QOS1 = 1,
    QOS2 = 2,
} qos_t;

typedef struct mqtt_param_st {
    tls_mode_t  tls_mode;
    uint32_t    command_timeout;
    uint32_t    keep_alive_interval_ms;
    uint8_t     clean_session;
    uint8_t     auto_connect_enable;
} mqtt_param_t;

typedef struct mqtt_message_st {
    char topic[64];
    char payload[256];
} mqtt_message_t;

typedef void (*message_handler_t)(mqtt_message_t* msg);

typedef struct mqtt_message_handlers_st {
    k_list_t            list;
    qos_t               qos;
    const char*         topic_filter;
    message_handler_t   handler;
} mqtt_message_handlers_t;

extern k_mail_q_t mqtt_message_mail;

#define DEFAULT_MQTT_PARAMS { TLS_MODE_PSK, MQTT_COMMAND_TIMEOUT, 240, 1, 0 }

typedef struct device_info_st {
    char product_id[PRODUCT_ID_MAX_SIZE + 1];
    char device_name[DEVICE_NAME_MAX_SIZE + 1];

    char device_serc[DEVICE_SERC_MAX_SIZE + 1];
} device_info_t;

typedef enum ota_mode_en {
    OTA_DISABLE,
    OTA_ENABLE
} ota_mode_t;

typedef struct ota_fw_info_st {
    char     fw_version[10];
    uint32_t  fw_size;
    uint8_t  fw_md5[50];
} ota_fw_info_t;

typedef struct tencent_firmware_module_st {
    int (*init)(void);

    int (*info_set)(device_info_t *info, tls_mode_t tls_mode);

    int (*mqtt_conn)(mqtt_param_t init_params);

    int (*mqtt_discon)(void);

    int (*mqtt_pub)(const char *topic, qos_t qos, char *payload);

    int (*mqtt_publ)(const char *topic, qos_t qos, char *payload);

    int (*mqtt_sub)(char *topic, qos_t qos);

    int (*mqtt_unsub)(const char *topic);

    int (*mqtt_state_get)(mqtt_state_t *state);

    int (*debug_level_set)(int log_level);
    
    int (*ota_set)(ota_mode_t mode, char *version);
    
    int (*ota_read_fwinfo)(ota_fw_info_t *ota_fw_info);
    
    int (*ota_read_fwdata)(uint8_t *ota_fw_data_buffer,uint16_t buf_size);
    
    int (*start_smartconfig)(void);
    
    int (*stop_smartconfig)(void);
} tencent_firmware_module_t;

/**
 * @brief Register a tencent firmware module.
 *
 * @attention None
 *
 * @param[in]   module      network module with tencent firmware
 *
 * @return  errcode
 */
int tos_tf_module_register(tencent_firmware_module_t *module);

/**
 * @brief Initialize the module.
 *
 * @attention None
 *
 * @return  errcode
 */
int tos_tf_module_init(void);

/**
 * @brief Set tencent firmware module information.
 *
 * @attention None
 *
 * @param[in]   info        device information
 * @param[in]   tls_mode    tls mode
 *
 * @return  errcode
 */
int tos_tf_module_info_set(device_info_t *info, tls_mode_t tls_mode);

/**
 * @brief Connect to the mqtt server.
 *
 * @attention None
 *
 * @param[in]   init_params mqtt connect parameter.
 *
 * @return  errcode
 */
int tos_tf_module_mqtt_conn(mqtt_param_t init_params);

/**
 * @brief Disconnect from the mqtt server.
 *
 * @attention None
 *
 * @return  errcode
 */
int tos_tf_module_mqtt_discon(void);

/**
 * @brief Publish a mqtt topic.
 *
 * @attention None
 *
 * @param[in]   topic       mqtt topic
 * @param[in]   qos         quality of service
 * @param[in]   payload     payload of the topic
 *
 * @return  errcode
 */
int tos_tf_module_mqtt_pub(const char *topic, qos_t qos, char *payload);

/**
 * @brief Publish mqtt topic with a long(> 200) payload.
 *
 * @attention None
 *
 * @param[in]   topic       mqtt topic
 * @param[in]   qos         quality of service
 * @param[in]   payload     payload of the topic
 *
 * @return  errcode
 */
int tos_tf_module_mqtt_publ(const char *topic, qos_t qos, char *payload);

/**
 * @brief Subscribe a mqtt topic.
 *
 * @attention None
 *
 * @param[in]   topic       mqtt topic
 * @param[in]   qos         quality of service
 * @param[in]   handle      will be callback when topic arrive
 *
 * @return  errcode
 */
int tos_tf_module_mqtt_sub(char *topic, qos_t qos, message_handler_t handle);

/**
 * @brief Unsubscribe a mqtt topic.
 *
 * @attention None
 *
 * @param[in]   topic       mqtt topic
 *
 * @return  errcode
 */
int tos_tf_module_mqtt_unsub(const char *topic);

/**
 * @brief Get module mqtt state.
 *
 * @attention None
 *
 * @param[out]  state       mqtt connection state
 *
 * @return  errcode
 */
int tos_tf_module_mqtt_state_get(mqtt_state_t *state);

/**
 * @brief Set tencent firmware module debug level.
 *
 * @attention None
 *
 * @param[in]   log_level   device information
 *
 * @return  errcode
 */
int tos_tf_module_debug_level_set(int log_level);

/**
 * @brief Set tencent firmware module OTA mode
 *
 * @attention None
 *
 * @param[in]   mode       OTA_DISABLE or OTA_ENABLE
 * @param[in]   version    version string
 *
 * @return  errcode
 */
int tos_tf_module_ota_set(ota_mode_t mode, char *version);

/**
 * @brief Read fw info from tencent firmware module
 *
 * @attention None
 *
 * @param[in]   ota_fw_info   fw information struct
 *
 * @return  errcode
 */
int tos_tf_module_ota_read_fwinfo(ota_fw_info_t *ota_fw_info);

/**
 * @brief Read fw data from tencent firmware module
 *
 * @attention None
 *
 * @param[in]   ota_fw_data_buffer   fw data buffer
 *
 * @param[in]   buf_size   fw data buffer size
 *
 * @return  errcode
 */
int tos_tf_module_ota_read_fwdata(uint8_t *ota_fw_data_buffer,uint16_t buf_size);

/**
 * @brief Start smartconfig from tencent firmware module
 *
 * @attention None
 *
 * @return  errcode
 */
int tos_tf_module_smartconfig_start(void);

/**
 * @brief Stop smartconfig from tencent firmware module
 *
 * @attention None
 *
 * @return  errcode
 */
int tos_tf_module_smartconfig_stop(void);

#endif

