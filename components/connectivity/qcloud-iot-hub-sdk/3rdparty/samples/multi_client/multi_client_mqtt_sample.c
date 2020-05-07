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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "utils_getopt.h"

/*
 * This sample test multi MQTT clients in multi-thread runtime.
 * 3 MQTT clients run in each own thread
 * psk/cert_device_info1/2/3.json for each device info are required
 * data topic forward configuration is required
 */

#define MAX_MQTT_THREAD_COUNT 3
// record the status of all the threadss
static unsigned int sg_thread_status[MAX_MQTT_THREAD_COUNT];

#ifdef WIN32
#define OS_PATH ".\\"
#else
#define OS_PATH "./"
#endif

#ifdef AUTH_MODE_CERT
char *device_info_file[MAX_MQTT_THREAD_COUNT] = {OS_PATH "cert_device_info1.json", OS_PATH "cert_device_info2.json",
                                                 OS_PATH "cert_device_info3.json"};
#else
char *device_info_file[MAX_MQTT_THREAD_COUNT] = {OS_PATH "psk_device_info1.json", OS_PATH "psk_device_info2.json",
                                                 OS_PATH "psk_device_info3.json"};
#endif

// sample data structures
typedef struct AppThreadData {
    int      thread_id;
    char *   device_info_file;
    bool     sub_ready;
    uint32_t msg_recv_cnt;
} AppThreadData;

static int sg_loop_cnt = 10;

// MQTT event callback
static void _mqtt_event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg)
{
    MQTTMessage *  mqtt_messge = (MQTTMessage *)msg->msg;
    uintptr_t      packet_id   = (uintptr_t)msg->msg;
    AppThreadData *app_data    = (AppThreadData *)handle_context;

    switch (msg->event_type) {
        case MQTT_EVENT_UNDEF:
            Log_i("undefined event occur.");
            break;

        case MQTT_EVENT_DISCONNECT:
            Log_i("MQTT disconnect.");
            break;

        case MQTT_EVENT_RECONNECT:
            Log_i("MQTT reconnect.");
            break;

        case MQTT_EVENT_PUBLISH_RECVEIVED:
            Log_i("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                  mqtt_messge->topic_len, mqtt_messge->ptopic, mqtt_messge->payload_len, mqtt_messge->payload);
            break;

        case MQTT_EVENT_SUBCRIBE_SUCCESS:
            Log_d("mqtt topic subscribe success");
            app_data->sub_ready = true;
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            Log_i("mqtt topic subscribe timeout");
            app_data->sub_ready = false;
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            Log_i("mqtt topic subscribe NACK");
            app_data->sub_ready = false;
            break;

        case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            Log_i("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            app_data->sub_ready = false;
            break;

        case MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            Log_i("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBCRIBE_NACK:
            Log_i("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_SUCCESS:
            Log_i("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_TIMEOUT:
            Log_i("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_NACK:
            Log_i("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;
        default:
            Log_i("Should NOT arrive here.");
            break;
    }
}

// Setup MQTT construct parameters
static int _setup_connect_init_params(MQTTInitParams *initParams, DeviceInfo *device_info, AppThreadData *app_data)
{
    initParams->product_id  = device_info->product_id;
    initParams->device_name = device_info->device_name;

#ifdef AUTH_MODE_CERT
    char  certs_dir[16] = "certs";
    char  current_path[128];
    char *cwd = getcwd(current_path, sizeof(current_path));

    if (cwd == NULL) {
        Log_e("getcwd return NULL");
        return QCLOUD_ERR_FAILURE;
    }

#ifdef WIN32
    HAL_Snprintf(initParams->cert_file, FILE_PATH_MAX_LEN, "%s\\%s\\%s", current_path, certs_dir,
                 device_info->dev_cert_file_name);
    HAL_Snprintf(initParams->key_file, FILE_PATH_MAX_LEN, "%s\\%s\\%s", current_path, certs_dir,
                 device_info->dev_key_file_name);
#else
    HAL_Snprintf(initParams->cert_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 device_info->dev_cert_file_name);
    HAL_Snprintf(initParams->key_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 device_info->dev_key_file_name);
#endif

#else
    initParams->device_secret = device_info->device_secret;
#endif

    initParams->command_timeout        = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
    initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

    initParams->auto_connect_enable  = 1;
    initParams->event_handle.h_fp    = _mqtt_event_handler;
    initParams->event_handle.context = (void *)app_data;

    return QCLOUD_RET_SUCCESS;
}

#define MAX_SIZE_OF_TOPIC_CONTENT 100
// publish MQTT msg
static int _publish_test_msg(void *client, char *topic_keyword, QoS qos, int count, int thread_id)
{
    char        topic_name[128] = {0};
    DeviceInfo *dev_info        = IOT_MQTT_GetDeviceInfo(client);

    int size = HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", dev_info->product_id, dev_info->device_name,
                            topic_keyword);
    if (size < 0 || size > sizeof(topic_name) - 1) {
        Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
        return QCLOUD_ERR_FAILURE;
    }

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = qos;

    char topic_content[MAX_SIZE_OF_TOPIC_CONTENT + 1] = {0};

    size = HAL_Snprintf(topic_content, sizeof(topic_content), "{\"text\": \"thread-%u\", \"count\": \"%d\"}", thread_id,
                        count);
    if (size < 0 || size > sizeof(topic_content) - 1) {
        Log_e("payload content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_content));
        return -3;
    }

    pub_params.payload     = topic_content;
    pub_params.payload_len = strlen(topic_content);

    return IOT_MQTT_Publish(client, topic_name, &pub_params);
}

// callback when MQTT msg arrives
static void _on_message_callback(void *pClient, MQTTMessage *message, void *user_data)
{
    if (message == NULL) {
        return;
    }

    AppThreadData *app_data = (AppThreadData *)user_data;
    app_data->msg_recv_cnt += 1;
    Log_i("Thread-%d recv msg topic:%.*s, payload:%.*s", app_data->thread_id, (int)message->topic_len, message->ptopic,
          (int)message->payload_len, (char *)message->payload);
}

// subscrib MQTT topic
static int _subscribe_topic_wait_result(void *client, char *topic_keyword, QoS qos, AppThreadData *app_data)
{
    static char topic_name[128] = {0};
    DeviceInfo *dev_info        = IOT_MQTT_GetDeviceInfo(client);
    int size = HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", dev_info->product_id, dev_info->device_name,
                            topic_keyword);

    if (size < 0 || size > sizeof(topic_name) - 1) {
        Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
        return QCLOUD_ERR_FAILURE;
    }
    SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
    sub_params.qos                = qos;
    sub_params.on_message_handler = _on_message_callback;
    sub_params.user_data          = (void *)app_data;
    int rc                        = IOT_MQTT_Subscribe(client, topic_name, &sub_params);
    if (rc < 0) {
        Log_e("MQTT subscribe failed: %d", rc);
        return rc;
    }

    int wait_cnt = 10;
    while (!app_data->sub_ready && (wait_cnt > 0)) {
        // wait for subscription result
        rc = IOT_MQTT_Yield(client, 1000);
        if (rc) {
            Log_e("MQTT error: %d", rc);
            return rc;
        }
        wait_cnt--;
    }

    if (wait_cnt > 0) {
        return QCLOUD_RET_SUCCESS;
    } else {
        Log_e("wait for subscribe result timeout!");
        return QCLOUD_ERR_FAILURE;
    }
}

static uint32_t _get_random_delay(void)
{
    srand((unsigned)HAL_GetTimeMs());
    /* range: 1000 - 5000 ms, in 10ms unit */
    return (rand() % 400 + 100) * 10;
}

static void _mqtt_client_thread_runner(void *ptr)
{
    int            pub_cnt   = 0;
    void *         client    = NULL;
    AppThreadData *app_data  = (AppThreadData *)ptr;
    int            thread_id = app_data->thread_id;

    DeviceInfo dev_info = {0};
    if (HAL_GetDevInfoFromFile(app_data->device_info_file, (void *)&dev_info)) {
        Log_e("invalid dev info file: %s", app_data->device_info_file);
        goto thread_exit;
    }

    Log_i("Thread running. ID: %d; device file: %s", thread_id, app_data->device_info_file);

    // init connection
    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
    int            rc          = _setup_connect_init_params(&init_params, &dev_info, app_data);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("init params error: %d", rc);
        goto thread_exit;
    }

    // create MQTT client and connect with server
    client = IOT_MQTT_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("MQTT Construct failed!");
        goto thread_exit;
    }

#ifdef SYSTEM_COMM
    long time = 0;
    // get system timestamp from server
    rc = IOT_Get_SysTime(client, &time);
    if (QCLOUD_RET_SUCCESS == rc) {
        Log_i("system time is %ld", time);
    } else {
        Log_e("get system time failed!");
    }
#endif

    // subscribe topic first
    rc = _subscribe_topic_wait_result(client, "data", QOS1, app_data);
    if (rc < 0) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        goto thread_exit;
    }

    int test_count = 0;
    do {
        rc = _publish_test_msg(client, "data", QOS1, test_count, thread_id);
        if (rc < 0) {
            Log_e("client publish topic failed :%d.", rc);
        } else {
            pub_cnt++;
        }

        rc = IOT_MQTT_Yield(client, 1000);
        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            HAL_SleepMs(1000);
            continue;
        } else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
            Log_e("exit with error: %d", rc);
            break;
        }

        test_count++;

        if (test_count < sg_loop_cnt)
            HAL_SleepMs(_get_random_delay());

#ifdef SYSTEM_COMM
        long time = 0;
        // get system timestamp from server
        rc = IOT_Get_SysTime(client, &time);
        if (QCLOUD_RET_SUCCESS == rc) {
            Log_i("system time is %ld", time);
        } else {
            Log_e("get system time failed!");
        }
#endif
    } while (test_count < sg_loop_cnt);

thread_exit:
    if (client != NULL)
        IOT_MQTT_Destroy(&client);

    Log_i(">>>>>>>>>>Thread-%d totally pub %d msg and recv %d msg", thread_id, pub_cnt, app_data->msg_recv_cnt);
    sg_thread_status[thread_id] = 1;
}

static int parse_arguments(int argc, char **argv)
{
    int c;
    while ((c = utils_getopt(argc, argv, "l:")) != EOF) switch (c) {
            case 'l':
                sg_loop_cnt = atoi(utils_optarg);
                break;

            default:
                HAL_Printf(
                    "usage: %s [options]\n"
                    "  [-l n] test loop count\n",
                    argv[0]);
                return -1;
        }
    return 0;
}

int main(int argc, char **argv)
{
    // init log level
    IOT_Log_Set_Level(eLOG_DEBUG);

    parse_arguments(argc, argv);

    AppThreadData app_data[MAX_MQTT_THREAD_COUNT];

    int i;
    // init thread app data
    for (i = 0; i < MAX_MQTT_THREAD_COUNT; i++) {
        sg_thread_status[i] = 0;  // init thread status flag

        app_data[i].device_info_file = device_info_file[i];
        app_data[i].thread_id        = i;
        app_data[i].sub_ready        = false;
        app_data[i].msg_recv_cnt     = 0;
    }

    int created_thread_cnt = 0;
    /* create multi threads for multi mqtt client test */
    for (i = 0; i < MAX_MQTT_THREAD_COUNT; i++) {
        sg_thread_status[i] = 0;  // init thread status flag

        ThreadParams thread_params = {0};
        thread_params.thread_func  = _mqtt_client_thread_runner;
        thread_params.user_arg     = &app_data[i];

        int rc = HAL_ThreadCreate(&thread_params);
        if (rc) {
            Log_e("create mqtt thread fail: %d", rc);
        } else {
            created_thread_cnt++;
        }

        HAL_SleepMs(300);
    }

    Log_i("created %d mqtt threads", created_thread_cnt);

    /* wait for all threads to finish their jobs */
    int finished_created_thread_cnt;
    do {
        finished_created_thread_cnt = 0;
        for (i = 0; i < MAX_MQTT_THREAD_COUNT; i++) {
            finished_created_thread_cnt += sg_thread_status[i];
        }
        Log_i(">>>>>>>>Finished thread count : %d", finished_created_thread_cnt);

        HAL_SleepMs(1000);
    } while (finished_created_thread_cnt < created_thread_cnt);

    return 0;
}
