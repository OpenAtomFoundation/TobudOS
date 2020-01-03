/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include "tos_k.h"
#include "qcloud.h"


static int sg_count = 0;
static int sg_sub_packet_id = -1;

void event_handler(void *client, void *context, mqtt_event_t *event) {
    mqtt_incoming_msg_t* mqtt_messge = NULL;
    uint16_t packet_id = *(uint16_t *)event->message;

    switch(event->type) {
        case MQTT_EVENT_UNDEF:
            QCLOUD_LOG_I("undefined event occur.");
            break;

        case MQTT_EVENT_DISCONNECT:
            QCLOUD_LOG_I("MQTT disconnect.");
            break;

        case MQTT_EVENT_RECONNECT:
            QCLOUD_LOG_I("MQTT reconnect.");
            break;

        case MQTT_EVENT_PUBLISH_RECVEIVED:
            mqtt_messge = (mqtt_incoming_msg_t*)event->message;
            QCLOUD_LOG_I("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
            mqtt_messge->topic_len,
            mqtt_messge->topic,
            mqtt_messge->payload_len,
            mqtt_messge->payload);
            break;
        case MQTT_EVENT_SUBCRIBE_SUCCESS:
            QCLOUD_LOG_I("subscribe success, packet-id=%u", (unsigned int)packet_id);
            sg_sub_packet_id = packet_id;
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            QCLOUD_LOG_I("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            sg_sub_packet_id = packet_id;
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            QCLOUD_LOG_I("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            sg_sub_packet_id = packet_id;
            break;

        case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            QCLOUD_LOG_I("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            QCLOUD_LOG_I("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBCRIBE_NACK:
            QCLOUD_LOG_I("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_SUCCESS:
            QCLOUD_LOG_I("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_TIMEOUT:
            QCLOUD_LOG_I("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_NACK:
            QCLOUD_LOG_I("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;
        default:
            QCLOUD_LOG_I("Should NOT arrive here.");
            break;
    }
}

/**
 * MQTT消息接收处理函数
 *
 * @param topicName         topic主题
 * @param topicNameLen      topic长度
 * @param message           已订阅消息的结构
 * @param userData         消息负载
 */
static void on_message_callback(void *client, mqtt_incoming_msg_t *message, void *private_data)
{
    if (!message) {
        return;
    }

    QCLOUD_LOG_I("Receive Message With topicName:%.*s, payload:%.*s",
            (int) message->topic_len, message->topic, (int) message->payload_len, (char *)message->payload);
}

static qcloud_device_t device;
static mqtt_connect_opt_t connect_opt;
static qcloud_mqtt_client_t client;

static char topic_filter[128] = {0};

#define MAX_SIZE_OF_TOPIC_CONTENT 100
static char topic_payload[MAX_SIZE_OF_TOPIC_CONTENT + 1] = {0};

void mqtt_basic_thread(void)
{
    qcloud_err_t rc;
    mqtt_publish_opt_t publish_opt;
    mqtt_subscribe_opt_t subscribe_opt;

    QCLOUD_LOG_I("mqtt_sample start");

    qcloud_device_create(&device, "XC31USKYPL", "dev001", "Pz1wK0fVJHxSojqxDuuvmg==");

    qcloud_mqtt_connect_opt_create(&connect_opt, &device, MQTT_VERSION_3_1_1, 240, MQTT_CLEAN_SESSION_STATE_ENABLED);

    qcloud_mqtt_client_create(&client, &device, event_handler, NULL, QCLOUD_AUTO_CONN_STATE_ENABLED);

    qcloud_mqtt_client_connect(&client, &connect_opt);

    osal_snprintf(topic_filter, sizeof(topic_filter), "%s/%s/%s", device.product_id, device.device_name, "data");

    subscribe_opt.message_handler   = on_message_callback;
    subscribe_opt.private_data      = NULL;
    subscribe_opt.qos               = MQTT_QOS0;

    rc = qcloud_mqtt_client_subscribe(&client, topic_filter, &subscribe_opt);

    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_LOG_E("Subscribe Failed: %d", rc);
    } else {
        QCLOUD_LOG_D("Subscribe success");
    }

    do {
        rc = qcloud_mqtt_client_yield(&client, &connect_opt, 3000);
        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            osal_sleep_ms(1000);
            continue;
        } else if (rc != QCLOUD_ERR_SUCCESS && rc != QCLOUD_ERR_MQTT_RECONNECTED) {
            QCLOUD_LOG_E("exit with error: %d", rc);
            break;
        }

        // 等待订阅结果
        if (sg_sub_packet_id > 0) {
            sprintf(topic_filter, "%s/%s/%s", device.product_id, device.device_name, "data");
            osal_snprintf(topic_payload, sizeof(topic_payload), "{\"action\": \"publish_test\", \"count\": \"%d\"}", sg_count++);

            memset(&publish_opt, 0, sizeof(mqtt_publish_opt_t));
            publish_opt.qos         = MQTT_QOS1;
            publish_opt.payload     = topic_payload;
            publish_opt.payload_len = strlen(topic_payload);

            rc = qcloud_mqtt_client_publish(&client, topic_filter, &publish_opt);
            if (rc != QCLOUD_ERR_SUCCESS) {
                QCLOUD_LOG_E("client publish topic failed :%d.", rc);
            }
        }

        osal_sleep_ms(1000);
    } while (1);

    qcloud_mqtt_client_destroy(&client);

    return;
}

