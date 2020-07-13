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

#include "lite-utils.h"
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

/*
 * Notes for using SDK in multi-thread programing
 * 1. IOT_MQTT_Yield, IOT_MQTT_Construct and IOT_MQTT_Destroy are NOT thread-safe, only calling them in the same thread
 * 2. IOT_MQTT_Publish, IOT_MQTT_Subscribe and IOT_MQTT_Unsubscribe are thread-safe, and be executed in multi-threads
 * simultaneously
 * 3. IOT_MQTT_Yield is the only entry to read from socket and should not be hand up for long
 * 4. Using IOT_MQTT_StartLoop to start a default thread for IOT_MQTT_Yield is recommended
 */

/*
 * This sample test MQTT multi-thread performance for one device.
 */

#define MAX_PUB_THREAD_COUNT      5
#define PUBLISH_COUNT             10
#define THREAD_SLEEP_INTERVAL_MS  1000
#define RX_RECEIVE_PERCENTAGE     99.0f
#define MAX_SIZE_OF_TOPIC_CONTENT 100

// device info
static DeviceInfo sg_device_info;

static bool         sg_sub_unsub_thread_quit;
static unsigned int sg_rx_count_array[MAX_PUB_THREAD_COUNT]
                                     [PUBLISH_COUNT];  // record the times when msg from subscribed topic is received
static unsigned int sg_rx_msg_buf_too_big_count;       // record the times when msg is oversize
static unsigned int sg_rx_unexpected_count;            // record the times when unexpected msg is received
static unsigned int sg_republish_count;                // record the times of re-publish
static char         sg_pub_sub_test_topic[MAX_SIZE_OF_TOPIC_CONTENT];  // topic for sub/pub

typedef struct AppThreadData {
    void *client;
    int   thread_id;
    int   thread_status;
} AppThreadData;

void event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg)
{
    MQTTMessage *mqtt_messge = (MQTTMessage *)msg->msg;
    uintptr_t    packet_id   = (uintptr_t)msg->msg;

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
            Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            Log_i("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
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

static int _setup_connect_init_params(MQTTInitParams *initParams)
{
    int ret;

    ret = HAL_GetDevInfo((void *)&sg_device_info);
    if (QCLOUD_RET_SUCCESS != ret) {
        return ret;
    }

    initParams->device_name = sg_device_info.device_name;
    initParams->product_id  = sg_device_info.product_id;

#ifdef AUTH_MODE_CERT
    char  certs_dir[16] = "certs";
    char  current_path[128];
    char *cwd = getcwd(current_path, sizeof(current_path));

    if (cwd == NULL) {
        Log_e("getcwd return NULL");
        return QCLOUD_ERR_FAILURE;
    }

    HAL_Snprintf(initParams->cert_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 sg_device_info.dev_cert_file_name);
    HAL_Snprintf(initParams->key_file, FILE_PATH_MAX_LEN, "%s/%s/%s", current_path, certs_dir,
                 sg_device_info.dev_key_file_name);
#else
    initParams->device_secret = sg_device_info.device_secret;
#endif

    memset(sg_pub_sub_test_topic, 0, MAX_SIZE_OF_TOPIC_CONTENT);
    HAL_Snprintf(sg_pub_sub_test_topic, MAX_SIZE_OF_TOPIC_CONTENT, "%s/%s/data", sg_device_info.product_id,
                 sg_device_info.device_name);

    initParams->command_timeout        = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
    initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

    initParams->auto_connect_enable = 1;
    initParams->event_handle.h_fp   = event_handler;

    return QCLOUD_RET_SUCCESS;
}

static void _mqtt_message_handler(void *pClient, MQTTMessage *message, void *userData)
{
    if (message == NULL) {
        return;
    }

    if (MAX_SIZE_OF_TOPIC_CONTENT >= message->payload_len) {
        /* parsing payload */
        char         tempBuf[MAX_SIZE_OF_TOPIC_CONTENT + 1] = {0};
        unsigned int tempRow = 0, tempCol = 0;
        char *       temp = NULL;

        HAL_Snprintf(tempBuf, message->payload_len + 1, "%s", (char *)message->payload);
        Log_d("Message received : %s", tempBuf);

        char *count_value = LITE_json_value_of("count", tempBuf);
        if (count_value != NULL) {
            tempCol = atoi(count_value);
            HAL_Free(count_value);
        } else {
            Log_e("count value not found!");
            sg_rx_unexpected_count++;
            return;
        }

        char *action_value = LITE_json_value_of("action", tempBuf);
        if (action_value != NULL) {
            temp = strstr(action_value, "-");
            if (NULL != temp) {
                tempRow = atoi(temp + 1);
                HAL_Free(action_value);
            } else {
                HAL_Free(action_value);
                Log_e("invalid action value: %s", action_value);
                sg_rx_unexpected_count++;
                return;
            }
        } else {
            Log_e("action value not found!");
            sg_rx_unexpected_count++;
            return;
        }

        if (((tempRow - 1) < MAX_PUB_THREAD_COUNT) && (tempCol < PUBLISH_COUNT)) {
            sg_rx_count_array[tempRow - 1][tempCol]++;
        } else {
            Log_e(" Unexpected Thread : %d, Message : %d ", tempRow, tempCol);
            sg_rx_unexpected_count++;
        }
    } else {
        sg_rx_msg_buf_too_big_count++;
    }
}

/**
 * subscribe/unsubscribe test thread runner
 * subscribed and unsubscribe
 */
static void _mqtt_sub_unsub_thread_runner(void *ptr)
{
    int   rc      = QCLOUD_RET_SUCCESS;
    void *pClient = ptr;
    char  testTopic[128];
    HAL_Snprintf(testTopic, 128, "%s/%s/control", sg_device_info.product_id, sg_device_info.device_name);

    while (QCLOUD_RET_SUCCESS == rc && false == sg_sub_unsub_thread_quit) {
        do {
            HAL_SleepMs(THREAD_SLEEP_INTERVAL_MS);
            SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
            sub_params.qos                = QOS1;
            sub_params.on_message_handler = _mqtt_message_handler;
            rc                            = IOT_MQTT_Subscribe(pClient, testTopic, &sub_params);

        } while (QCLOUD_ERR_MQTT_NO_CONN == rc || QCLOUD_ERR_MQTT_REQUEST_TIMEOUT == rc);

        if (rc < 0) {
            Log_e("Subscribe failed. Ret : %d ", rc);
        }

        HAL_SleepMs(1000);

        do {
            HAL_SleepMs(THREAD_SLEEP_INTERVAL_MS);
            rc = IOT_MQTT_Unsubscribe(pClient, testTopic);
        } while (QCLOUD_ERR_MQTT_NO_CONN == rc || QCLOUD_ERR_MQTT_REQUEST_TIMEOUT == rc);

        if (rc < 0) {
            Log_e("Unsubscribe failed. Returned : %d ", rc);
        }
    }
}

/**
 * do subscribe in the thread
 */
static int _mqtt_subscribe_to_test_topic(void *pClient)
{
    SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = _mqtt_message_handler;
    sub_params.qos                = QOS1;
    return IOT_MQTT_Subscribe(pClient, sg_pub_sub_test_topic, &sub_params);
}

/**
 * do publish in the thread
 * loop for PUBLISH_COUNT times
 * If publish failed in 1st time, do it again
 */
static void _mqtt_publish_thread_runner(void *ptr)
{
    int  itr                                          = 0;
    char topic_content[MAX_SIZE_OF_TOPIC_CONTENT + 1] = {0};

    PublishParams  params;
    int            rc          = QCLOUD_RET_SUCCESS;
    AppThreadData *thread_data = (AppThreadData *)ptr;
    void *         pClient     = thread_data->client;
    int            thread_id   = thread_data->thread_id;

    for (itr = 0; itr < PUBLISH_COUNT; itr++) {
        int size = HAL_Snprintf(topic_content, sizeof(topic_content), "{\"action\": \"thread-%d\", \"count\": \"%d\"}",
                                thread_id, itr);
        if (size < 0 || size > sizeof(topic_content) - 1) {
            Log_e("payload content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_content));
        }

        params.payload     = (void *)topic_content;
        params.payload_len = strlen(topic_content);
        params.qos         = QOS1;
        Log_d("Msg being published: %s", topic_content);

        do {
            rc = IOT_MQTT_Publish(pClient, sg_pub_sub_test_topic, &params);
            HAL_SleepMs(THREAD_SLEEP_INTERVAL_MS);
        } while (QCLOUD_ERR_MQTT_NO_CONN == rc || QCLOUD_ERR_MQTT_REQUEST_TIMEOUT == rc);

        // 1st publish failed, re-publish and update sg_republish_count
        if (rc < 0) {
            Log_e("Failed attempt 1 Publishing Thread : %d, Msg : %d, cs : %d ", thread_id, itr, rc);
            do {
                rc = IOT_MQTT_Publish(pClient, sg_pub_sub_test_topic, &params);
                HAL_SleepMs(THREAD_SLEEP_INTERVAL_MS);
            } while (QCLOUD_ERR_MQTT_NO_CONN == rc);
            sg_republish_count++;
            if (rc < 0) {
                Log_e("Failed attempt 2 Publishing Thread : %d, Msg : %d, cs : %d Second Attempt ", thread_id, itr, rc);
            }
        }
    }

    thread_data->thread_status = 1;
}

/**
 * thread safety performance test
 */
static int _mqtt_multi_thread_test(void *client)
{
    float percent_of_rx_msg     = 0.0;  // record the success percentage of every pub send and recv
    int   finished_thread_count = 0;    // record the number of finished publish threads
    int   rx_msg_count          = 0;    // record the times of successfull subscribe
    // thread data passed to publish thread
    AppThreadData thread_data[MAX_PUB_THREAD_COUNT]       = {0};
    ThreadParams  pub_thread_params[MAX_PUB_THREAD_COUNT] = {0};

    int rc          = QCLOUD_RET_SUCCESS;
    int test_result = 0;
    int i = 0, j = 0;

    // init the global variables
    sg_rx_msg_buf_too_big_count = 0;
    sg_rx_unexpected_count      = 0;
    sg_republish_count          = 0;

    if (client == NULL) {
        Log_e("MQTT client is invalid!");
        return QCLOUD_ERR_FAILURE;
    }

    /* create a thread to test subscribe and unsubscribe another topic */
    ThreadParams sub_thread_params = {0};
    sub_thread_params.thread_func  = _mqtt_sub_unsub_thread_runner;
    sub_thread_params.user_arg     = client;

    rc = HAL_ThreadCreate(&sub_thread_params);
    if (rc < 0) {
        Log_e("Create Sub_unsub thread failed: %d", rc);
        return rc;
    }

    /* subscribe the same test topic as publish threads */
    rc = _mqtt_subscribe_to_test_topic(client);
    if (rc < 0) {
        Log_e("Client subscribe failed: %d", rc);
        return rc;
    }

    /* setup the thread info for pub-threads */
    for (j = 0; j < MAX_PUB_THREAD_COUNT; j++) {
        thread_data[j].client = client;
        // self defined thread ID: 1 - MAX_PUB_THREAD_COUNT
        thread_data[j].thread_id     = j + 1;
        thread_data[j].thread_status = 0;
        for (i = 0; i < PUBLISH_COUNT; i++) {
            sg_rx_count_array[j][i] = 0;
        }
    }

    /* create multi threads to test IOT_MQTT_Publish() */
    for (i = 0; i < MAX_PUB_THREAD_COUNT; i++) {
        pub_thread_params[i].thread_func = _mqtt_publish_thread_runner;
        pub_thread_params[i].user_arg    = (void *)&thread_data[i];

        rc = HAL_ThreadCreate(&pub_thread_params[i]);
        if (rc < 0) {
            Log_e("Create publish thread(ID: %d) failed: %d", thread_data[i].thread_id, rc);
        }
    }

    /* wait for all pub-threads to finish their jobs */
    do {
        finished_thread_count = 0;
        for (i = 0; i < MAX_PUB_THREAD_COUNT; i++) {
            finished_thread_count += thread_data[i].thread_status;
        }

        Log_i(">>>>>>>>Finished thread count : %d", finished_thread_count);

        int exit_code;
        if (!IOT_MQTT_GetLoopStatus(client, &exit_code))
            Log_e("MQTT Loop thread quit with code: %d", exit_code);

        HAL_SleepMs(1000);
    } while (finished_thread_count < MAX_PUB_THREAD_COUNT);

    Log_i("Publishing is finished");

    sg_sub_unsub_thread_quit = true;

    /* Allow time for sub_sunsub thread to exit */
    HAL_SleepMs(1000);

    /* all threads should have terminated gracefully at this point. If they haven't,
     * which should not be possible, something below will fail. */

    /* Calculating Test Results */
    for (i = 0; i < PUBLISH_COUNT; i++) {
        for (j = 0; j < MAX_PUB_THREAD_COUNT; j++) {
            if (sg_rx_count_array[j][i] > 0) {
                rx_msg_count++;
            }
        }
    }
    percent_of_rx_msg = (float)rx_msg_count * 100 / (PUBLISH_COUNT * MAX_PUB_THREAD_COUNT);

    HAL_Printf("\n\nMQTT Multi-thread Test Result : \n");
    HAL_Printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    if (RX_RECEIVE_PERCENTAGE <= percent_of_rx_msg && 0 == sg_rx_msg_buf_too_big_count && 0 == sg_rx_unexpected_count) {
        // test success
        HAL_Printf("Success! PercentOfRxMsg: %f %%\n", percent_of_rx_msg);
        HAL_Printf("Published Messages: %d , Received Messages: %d \n", PUBLISH_COUNT * MAX_PUB_THREAD_COUNT,
                   rx_msg_count);
        HAL_Printf("QoS 1 re publish count %u\n", sg_republish_count);
        test_result = 0;
    } else {
        // test fail
        HAL_Printf("\nFailure! PercentOfRxMsg: %f %%\n", percent_of_rx_msg);
        HAL_Printf("Published Messages: %d , Received Messages: %d \n", PUBLISH_COUNT * MAX_PUB_THREAD_COUNT,
                   rx_msg_count);
        HAL_Printf("\"Received message was too big than anything sent\" count: %u\n", sg_rx_msg_buf_too_big_count);
        HAL_Printf("\"The number received is out of the range\" count: %u\n", sg_rx_unexpected_count);
        test_result = -1;
    }
    HAL_Printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");

    return test_result;
}

int main(int argc, char **argv)
{
    int rc;

    // Init log level
    IOT_Log_Set_Level(eLOG_DEBUG);

    // Init connection
    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
    rc                         = _setup_connect_init_params(&init_params);
    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

    // MQTT client create and connect
    void *client = IOT_MQTT_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("Cloud Device Construct Failed");
        return QCLOUD_ERR_FAILURE;
    }

    // Start the default loop thread to read and handle MQTT packet
    rc = IOT_MQTT_StartLoop(client);
    if (rc) {
        Log_e("MQTT start loop failed: %d", rc);
        rc = IOT_MQTT_Destroy(&client);
        return rc;
    }

    // Start application
    rc = _mqtt_multi_thread_test(client);
    if (0 != rc) {
        Log_e("MQTT multi-thread test FAILED! RC: %d", rc);
    } else {
        Log_i("MQTT multi-thread test SUCCESS");
    }

    // Finish and destroy
    IOT_MQTT_StopLoop(client);
    rc = IOT_MQTT_Destroy(&client);
    return rc;
}
