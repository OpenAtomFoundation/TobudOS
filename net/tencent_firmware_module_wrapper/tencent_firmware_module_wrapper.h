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

#define AUTH_MODE_KEY       1
#define AUTH_MODE_CERT      2

#define CFG_AUTH_MODE       AUTH_MODE_KEY


#define MQTT_COMMAND_TIMEOUT                (5 * 1000)

#define PRODUCT_ID_MAX_SIZE                 10

#define DEVICE_NAME_MAX_SIZE                64

#define DEVICE_SERC_MAX_SIZE                25

#define DEVICE_CERT_FILE_NAME_MAX_SIZE      128

#define DEVICE_KEY_FILE_NAME_MAX_SIZE       128

#define TOPIC_MAX_SIZE                      ((DEVICE_NAME_MAX_SIZE) + (PRODUCT_ID_MAX_SIZE) + 64)

#define PUB_PAYLOAD_MAX_SIZE                200

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

#define DEFAULT_MQTT_PARAMS { TLS_MODE_PSK, MQTT_COMMAND_TIMEOUT, 240, 1, 1 }

typedef struct device_info_st {
    char product_id[PRODUCT_ID_MAX_SIZE + 1];
    char device_name[DEVICE_NAME_MAX_SIZE + 1];

    char device_serc[DEVICE_SERC_MAX_SIZE + 1];
} device_info_t;

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
 *
 * @return  errcode
 */
int tos_tf_module_mqtt_sub(char *topic, qos_t qos);

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
 * @param[in]   tls_mode    tls mode
 *
 * @return  errcode
 */
int tos_tf_module_debug_level_set(int log_level);

#endif

