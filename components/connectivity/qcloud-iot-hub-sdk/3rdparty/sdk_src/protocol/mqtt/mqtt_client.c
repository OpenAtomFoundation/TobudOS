/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "lite-utils.h"
#include "log_upload.h"
#include "mqtt_client.h"
#include "qcloud_iot_ca.h"
#include "qcloud_iot_common.h"
#include "qcloud_iot_device.h"
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "utils_base64.h"
#include "utils_list.h"

static uint16_t _get_random_start_packet_id(void)
{
    srand((unsigned)HAL_GetTimeMs());
    return rand() % 65536 + 1;
}

DeviceInfo *IOT_MQTT_GetDeviceInfo(void *pClient)
{
    POINTER_SANITY_CHECK(pClient, NULL);
    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;
    return &mqtt_client->device_info;
}

// currently return a constant value
int IOT_MQTT_GetErrCode(void)
{
    return QCLOUD_ERR_FAILURE;
}

void *IOT_MQTT_Construct(MQTTInitParams *pParams)
{
    POINTER_SANITY_CHECK(pParams, NULL);
    STRING_PTR_SANITY_CHECK(pParams->product_id, NULL);
    STRING_PTR_SANITY_CHECK(pParams->device_name, NULL);

    Qcloud_IoT_Client *mqtt_client = NULL;

    // create and init MQTTClient
    if ((mqtt_client = (Qcloud_IoT_Client *)HAL_Malloc(sizeof(Qcloud_IoT_Client))) == NULL) {
        Log_e("malloc MQTTClient failed");
        return NULL;
    }

    int rc = qcloud_iot_mqtt_init(mqtt_client, pParams);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("mqtt init failed: %d", rc);
        HAL_Free(mqtt_client);
        return NULL;
    }

    MQTTConnectParams connect_params = DEFAULT_MQTTCONNECT_PARAMS;
    connect_params.client_id         = mqtt_client->device_info.client_id;
    // Upper limit of keep alive interval is (11.5 * 60) seconds
    connect_params.keep_alive_interval = Min(pParams->keep_alive_interval_ms / 1000, 690);
    connect_params.clean_session       = pParams->clean_session;
    connect_params.auto_connect_enable = pParams->auto_connect_enable;
#if defined(AUTH_WITH_NOTLS) && defined(AUTH_MODE_KEY)
    if (pParams->device_secret == NULL) {
        Log_e("Device secret is null!");
        qcloud_iot_mqtt_fini(mqtt_client);
        HAL_Free(mqtt_client);
        return NULL;
    }
    size_t src_len = strlen(pParams->device_secret);
    size_t len;
    memset(mqtt_client->psk_decode, 0x00, DECODE_PSK_LENGTH);
    rc                               = qcloud_iot_utils_base64decode(mqtt_client->psk_decode, DECODE_PSK_LENGTH, &len,
                                       (unsigned char *)pParams->device_secret, src_len);
    connect_params.device_secret     = (char *)mqtt_client->psk_decode;
    connect_params.device_secret_len = len;
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("Device secret decode err, secret:%s", pParams->device_secret);
        qcloud_iot_mqtt_fini(mqtt_client);
        HAL_Free(mqtt_client);
        return NULL;
    }
#endif

    rc = qcloud_iot_mqtt_connect(mqtt_client, &connect_params);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("mqtt connect with id: %s failed: %d", mqtt_client->options.conn_id, rc);
        qcloud_iot_mqtt_fini(mqtt_client);
        HAL_Free(mqtt_client);
        return NULL;
    } else {
        Log_i("mqtt connect with id: %s success", mqtt_client->options.conn_id);
    }

#ifdef LOG_UPLOAD
    // log subscribe topics
    if (is_log_uploader_init()) {
        int log_level;
        rc = qcloud_get_log_level(mqtt_client, &log_level);
        // rc = qcloud_log_topic_subscribe(mqtt_client);
        if (rc < 0) {
            Log_e("client get log topic failed: %d", rc);
        }
        set_log_mqtt_client((void *)mqtt_client);

        IOT_Log_Upload(true);
    }
#endif
    return mqtt_client;
}

int IOT_MQTT_Destroy(void **pClient)
{
    POINTER_SANITY_CHECK(*pClient, QCLOUD_ERR_INVAL);

    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)(*pClient);

    int rc = qcloud_iot_mqtt_disconnect(mqtt_client);
    // disconnect network stack by force
    if (rc != QCLOUD_RET_SUCCESS) {
        mqtt_client->network_stack.disconnect(&(mqtt_client->network_stack));
        set_client_conn_state(mqtt_client, NOTCONNECTED);
    }

    int i = 0;
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        /* notify this event to topic subscriber */
        if (NULL != mqtt_client->sub_handles[i].topic_filter && NULL != mqtt_client->sub_handles[i].sub_event_handler)
            mqtt_client->sub_handles[i].sub_event_handler(mqtt_client, MQTT_EVENT_CLIENT_DESTROY,
                                                          mqtt_client->sub_handles[i].handler_user_data);

        if (NULL != mqtt_client->sub_handles[i].topic_filter) {
            HAL_Free((void *)mqtt_client->sub_handles[i].topic_filter);
            mqtt_client->sub_handles[i].topic_filter = NULL;
        }
    }

#ifdef MQTT_RMDUP_MSG_ENABLED
    reset_repeat_packet_id_buffer(mqtt_client);
#endif

    HAL_MutexDestroy(mqtt_client->lock_generic);
    HAL_MutexDestroy(mqtt_client->lock_write_buf);

    HAL_MutexDestroy(mqtt_client->lock_list_sub);
    HAL_MutexDestroy(mqtt_client->lock_list_pub);

    list_destroy(mqtt_client->list_pub_wait_ack);
    list_destroy(mqtt_client->list_sub_wait_ack);

    HAL_Free(*pClient);
    *pClient = NULL;
#ifdef LOG_UPLOAD
    set_log_mqtt_client(NULL);
#endif
    Log_i("mqtt release!");

    return rc;
}

int IOT_MQTT_Yield(void *pClient, uint32_t timeout_ms)
{
    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;

#ifdef MULTITHREAD_ENABLED
    /* only one instance of yield is allowed in running state*/
    if (mqtt_client->thread_running) {
        HAL_SleepMs(timeout_ms);
        return QCLOUD_RET_SUCCESS;
    }
#endif

    int rc = qcloud_iot_mqtt_yield(mqtt_client, timeout_ms);

#ifdef LOG_UPLOAD
    /* do instant log uploading if MQTT communication error */
    if (rc == QCLOUD_RET_SUCCESS)
        IOT_Log_Upload(false);
    else
        IOT_Log_Upload(true);
#endif

    return rc;
}

int IOT_MQTT_Publish(void *pClient, char *topicName, PublishParams *pParams)
{
    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;

    return qcloud_iot_mqtt_publish(mqtt_client, topicName, pParams);
}

int IOT_MQTT_Subscribe(void *pClient, char *topicFilter, SubscribeParams *pParams)
{
    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;

    return qcloud_iot_mqtt_subscribe(mqtt_client, topicFilter, pParams);
}

int IOT_MQTT_Unsubscribe(void *pClient, char *topicFilter)
{
    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;

    return qcloud_iot_mqtt_unsubscribe(mqtt_client, topicFilter);
}

bool IOT_MQTT_IsSubReady(void *pClient, char *topicFilter)
{
    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;

    return qcloud_iot_mqtt_is_sub_ready(mqtt_client, topicFilter);
}

bool IOT_MQTT_IsConnected(void *pClient)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;

    IOT_FUNC_EXIT_RC(get_client_conn_state(mqtt_client) == 1)
}

#ifdef MULTITHREAD_ENABLED

static void _mqtt_yield_thread(void *ptr)
{
    int                rc          = QCLOUD_RET_SUCCESS;
    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)ptr;

    Log_i("MQTT client %s start loop", mqtt_client->device_info.client_id);
    while (mqtt_client->thread_running) {
        int rc = qcloud_iot_mqtt_yield(mqtt_client, 500);

#ifdef LOG_UPLOAD
        /* do instant log uploading if MQTT communication error */
        if (rc == QCLOUD_RET_SUCCESS)
            IOT_Log_Upload(false);
        else
            IOT_Log_Upload(true);
#endif

        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            HAL_SleepMs(500);
            continue;
        } else if (rc == QCLOUD_RET_MQTT_MANUALLY_DISCONNECTED || rc == QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT) {
            Log_e("MQTT Yield thread exit with error: %d", rc);
            break;
        } else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
            Log_e("MQTT Yield thread error: %d", rc);
        }

        HAL_SleepMs(200);
    }

    mqtt_client->thread_running   = false;
    mqtt_client->thread_exit_code = rc;

#ifdef LOG_UPLOAD
    IOT_Log_Upload(true);
#endif

    Log_i("MQTT client %s stop loop", mqtt_client->device_info.client_id);
}

int IOT_MQTT_StartLoop(void *pClient)
{
    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    Qcloud_IoT_Client *mqtt_client   = (Qcloud_IoT_Client *)pClient;
    ThreadParams       thread_params = {0};
    thread_params.thread_func        = _mqtt_yield_thread;
    thread_params.thread_name        = "MQTT_yield_thread";
    thread_params.user_arg           = pClient;
    thread_params.stack_size         = 4096;
    thread_params.priority           = 1;
    mqtt_client->thread_running      = true;

    int rc = HAL_ThreadCreate(&thread_params);
    if (rc) {
        Log_e("create mqtt yield thread fail: %d", rc);
        return QCLOUD_ERR_FAILURE;
    }

    HAL_SleepMs(500);
    return QCLOUD_RET_SUCCESS;
}

void IOT_MQTT_StopLoop(void *pClient)
{
    POINTER_SANITY_CHECK_RTN(pClient);

    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;
    mqtt_client->thread_running    = false;
    HAL_SleepMs(1000);
    return;
}

bool IOT_MQTT_GetLoopStatus(void *pClient, int *exit_code)
{
    POINTER_SANITY_CHECK(pClient, false);
    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;
    *exit_code                     = mqtt_client->thread_exit_code;
    return mqtt_client->thread_running;
}

#endif

#if 0
static inline  void _strlowr(char *str)
{
    while (*str != '\0') {
        *str = tolower(*str);
        str++;
    }
}
#endif

int qcloud_iot_mqtt_init(Qcloud_IoT_Client *pClient, MQTTInitParams *pParams)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pParams, QCLOUD_ERR_INVAL);

    memset(pClient, 0x0, sizeof(Qcloud_IoT_Client));

    int rc = iot_device_info_set(&(pClient->device_info), pParams->product_id, pParams->device_name);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("failed to set device info: %d", rc);
        return rc;
    }

    int size =
        HAL_Snprintf(pClient->host_addr, HOST_STR_LENGTH, "%s.%s", pParams->product_id, QCLOUD_IOT_MQTT_DIRECT_DOMAIN);
    if (size < 0 || size > HOST_STR_LENGTH - 1) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    // enable below code for some special platform
    //_strlowr(s_qcloud_iot_host);

    int i = 0;
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {
        pClient->sub_handles[i].topic_filter      = NULL;
        pClient->sub_handles[i].message_handler   = NULL;
        pClient->sub_handles[i].sub_event_handler = NULL;
        pClient->sub_handles[i].qos               = QOS0;
        pClient->sub_handles[i].handler_user_data = NULL;
    }

    if (pParams->command_timeout < MIN_COMMAND_TIMEOUT)
        pParams->command_timeout = MIN_COMMAND_TIMEOUT;
    if (pParams->command_timeout > MAX_COMMAND_TIMEOUT)
        pParams->command_timeout = MAX_COMMAND_TIMEOUT;
    pClient->command_timeout_ms = pParams->command_timeout;

    // packet id, random from [1 - 65536]
    pClient->next_packet_id               = _get_random_start_packet_id();
    pClient->write_buf_size               = QCLOUD_IOT_MQTT_TX_BUF_LEN;
    pClient->read_buf_size                = QCLOUD_IOT_MQTT_RX_BUF_LEN;
    pClient->is_ping_outstanding          = 0;
    pClient->was_manually_disconnected    = 0;
    pClient->counter_network_disconnected = 0;

    pClient->event_handle = pParams->event_handle;

    pClient->lock_generic = HAL_MutexCreate();
    if (NULL == pClient->lock_generic) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    set_client_conn_state(pClient, NOTCONNECTED);

    if ((pClient->lock_write_buf = HAL_MutexCreate()) == NULL) {
        Log_e("create write buf lock failed.");
        goto error;
    }
    if ((pClient->lock_list_sub = HAL_MutexCreate()) == NULL) {
        Log_e("create sub list lock failed.");
        goto error;
    }
    if ((pClient->lock_list_pub = HAL_MutexCreate()) == NULL) {
        Log_e("create pub list lock failed.");
        goto error;
    }

    if ((pClient->list_pub_wait_ack = list_new()) == NULL) {
        Log_e("create pub wait list failed.");
        goto error;
    }
    pClient->list_pub_wait_ack->free = HAL_Free;

    if ((pClient->list_sub_wait_ack = list_new()) == NULL) {
        Log_e("create sub wait list failed.");
        goto error;
    }
    pClient->list_sub_wait_ack->free = HAL_Free;

#ifndef AUTH_WITH_NOTLS
    // device param for TLS connection
#ifdef AUTH_MODE_CERT
    Log_d("cert file: %s", pParams->cert_file);
    Log_d("key file: %s", pParams->key_file);

    strncpy(pClient->cert_file_path, pParams->cert_file, FILE_PATH_MAX_LEN - 1);
    strncpy(pClient->key_file_path, pParams->key_file, FILE_PATH_MAX_LEN - 1);

    pClient->network_stack.ssl_connect_params.cert_file  = pClient->cert_file_path;
    pClient->network_stack.ssl_connect_params.key_file   = pClient->key_file_path;
    pClient->network_stack.ssl_connect_params.ca_crt     = iot_ca_get();
    pClient->network_stack.ssl_connect_params.ca_crt_len = strlen(pClient->network_stack.ssl_connect_params.ca_crt);
#else
    if (pParams->device_secret != NULL) {
        size_t src_len = strlen(pParams->device_secret);
        size_t len;
        memset(pClient->psk_decode, 0x00, DECODE_PSK_LENGTH);
        qcloud_iot_utils_base64decode(pClient->psk_decode, DECODE_PSK_LENGTH, &len,
                                      (unsigned char *)pParams->device_secret, src_len);
        pClient->network_stack.ssl_connect_params.psk        = (char *)pClient->psk_decode;
        pClient->network_stack.ssl_connect_params.psk_length = len;
    } else {
        Log_e("psk is empty!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }
    if (strnlen(pClient->device_info.client_id, MAX_SIZE_OF_CLIENT_ID) == 0) {
        Log_e("psk id is empty!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }
    pClient->network_stack.ssl_connect_params.psk_id     = pClient->device_info.client_id;
    pClient->network_stack.ssl_connect_params.ca_crt     = NULL;
    pClient->network_stack.ssl_connect_params.ca_crt_len = 0;
#endif

    pClient->network_stack.host = pClient->host_addr;
    pClient->network_stack.port = MQTT_SERVER_PORT_TLS;
    pClient->network_stack.ssl_connect_params.timeout_ms =
        pClient->command_timeout_ms > QCLOUD_IOT_TLS_HANDSHAKE_TIMEOUT ? pClient->command_timeout_ms
                                                                       : QCLOUD_IOT_TLS_HANDSHAKE_TIMEOUT;

#else
    pClient->network_stack.host = pClient->host_addr;
    pClient->network_stack.port = MQTT_SERVER_PORT_NOTLS;
#endif

    // init network stack
    qcloud_iot_mqtt_network_init(&(pClient->network_stack));

    // ping timer and reconnect delay timer
    InitTimer(&(pClient->ping_timer));
    InitTimer(&(pClient->reconnect_delay_timer));

#ifdef SYSTEM_COMM
    pClient->sys_state.result_recv_ok = false;
    pClient->sys_state.topic_sub_ok   = false;
    pClient->sys_state.time           = 0;
#endif

#ifdef MULTITHREAD_ENABLED
    pClient->thread_running = false;
#endif

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);

error:
    if (pClient->list_pub_wait_ack) {
        pClient->list_pub_wait_ack->free(pClient->list_pub_wait_ack);
        pClient->list_pub_wait_ack = NULL;
    }
    if (pClient->list_sub_wait_ack) {
        pClient->list_sub_wait_ack->free(pClient->list_sub_wait_ack);
        pClient->list_sub_wait_ack = NULL;
    }
    if (pClient->lock_generic) {
        HAL_MutexDestroy(pClient->lock_generic);
        pClient->lock_generic = NULL;
    }
    if (pClient->lock_list_sub) {
        HAL_MutexDestroy(pClient->lock_list_sub);
        pClient->lock_list_sub = NULL;
    }
    if (pClient->lock_list_pub) {
        HAL_MutexDestroy(pClient->lock_list_pub);
        pClient->lock_list_pub = NULL;
    }
    if (pClient->lock_write_buf) {
        HAL_MutexDestroy(pClient->lock_write_buf);
        pClient->lock_write_buf = NULL;
    }

    IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
}

int qcloud_iot_mqtt_fini(Qcloud_IoT_Client *mqtt_client)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(mqtt_client, QCLOUD_ERR_INVAL);

    HAL_MutexDestroy(mqtt_client->lock_generic);
    HAL_MutexDestroy(mqtt_client->lock_write_buf);

    HAL_MutexDestroy(mqtt_client->lock_list_sub);
    HAL_MutexDestroy(mqtt_client->lock_list_pub);

    list_destroy(mqtt_client->list_pub_wait_ack);
    list_destroy(mqtt_client->list_sub_wait_ack);

    Log_i("release mqtt client resources");

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

int qcloud_iot_mqtt_set_autoreconnect(Qcloud_IoT_Client *pClient, bool value)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    pClient->options.auto_connect_enable = (uint8_t)value;

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

bool qcloud_iot_mqtt_is_autoreconnect_enabled(Qcloud_IoT_Client *pClient)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    bool is_enabled = false;
    if (pClient->options.auto_connect_enable == 1) {
        is_enabled = true;
    }

    IOT_FUNC_EXIT_RC(is_enabled);
}

int qcloud_iot_mqtt_get_network_disconnected_count(Qcloud_IoT_Client *pClient)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    IOT_FUNC_EXIT_RC(pClient->counter_network_disconnected);
}

int qcloud_iot_mqtt_reset_network_disconnected_count(Qcloud_IoT_Client *pClient)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    pClient->counter_network_disconnected = 0;

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

#ifdef __cplusplus
}
#endif
