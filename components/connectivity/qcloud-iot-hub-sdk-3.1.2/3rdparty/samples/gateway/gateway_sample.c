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
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "utils_getopt.h"
#include "qcloud_iot_export.h"

#define MAX_SIZE_OF_TOPIC (128)
#define MAX_SIZE_OF_DATA (128)

static int sg_sub_packet_id = -1;

#ifdef AUTH_MODE_CERT
    static char sg_cert_file[PATH_MAX + 1];      // full path of device cert file
    static char sg_key_file[PATH_MAX + 1];       // full path of device key file
#endif


void _event_handler(void *client, void *context, MQTTEventMsg *msg) 
{
	MQTTMessage* mqtt_messge = (MQTTMessage*)msg->msg;
	uintptr_t packet_id = (uintptr_t)msg->msg;

	switch(msg->event_type) {
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
					mqtt_messge->topic_len,
					mqtt_messge->ptopic,
					mqtt_messge->payload_len,
					mqtt_messge->payload);
			break;
		case MQTT_EVENT_SUBCRIBE_SUCCESS:
			Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
			sg_sub_packet_id = packet_id;
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
			Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			sg_sub_packet_id = packet_id;
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
			Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
			sg_sub_packet_id = packet_id;
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

static void _message_handler(void *client, MQTTMessage *message, void *user_data)
{
	if (message == NULL) {
		return;
	}

	Log_i("Receive Message With topicName:%.*s, payload:%.*s",
		  (int) message->topic_len, message->ptopic, (int) message->payload_len, (char *) message->payload);
}

static int sg_loop_count = 1;
static int parse_arguments(int argc, char **argv)
{
    int c;
    while ((c = utils_getopt(argc, argv, "c:l:")) != EOF)
        switch (c) 
        {
            case 'c':
                if (HAL_SetDevInfoFile(utils_optarg))
                    return -1;
                break;

            case 'l':
                sg_loop_count = atoi(utils_optarg);
                if (sg_loop_count > 10000)
                    sg_loop_count = 10000;
                else if (sg_loop_count < 0)
                    sg_loop_count = 1;
                break;

            default:
                HAL_Printf("usage: %s [options]\n"
                    "  [-c <config file for DeviceInfo>] \n"
                    "  [-l <loop count>] \n"
                    , argv[0]);
            return -1;
        }
    return 0;
}

int main(int argc, char **argv) 
{
	int rc = QCLOUD_ERR_FAILURE;	
	void* client = NULL;

	IOT_Log_Set_Level(eLOG_DEBUG);

    // parse arguments for device info file and loop test;
    rc = parse_arguments(argc, argv);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("parse arguments error, rc = %d", rc);
        return rc;
    } 

	GatewayDeviceInfo GWdevInfo;
	rc = HAL_GetGwDevInfo((void *)&GWdevInfo);	
	if(QCLOUD_RET_SUCCESS != rc){
		Log_e("Get gateway dev info err,rc:%d",rc);
		return rc;
	}

	GatewayInitParam init_params = DEFAULT_GATEWAY_INIT_PARAMS;
	init_params.init_param.product_id = GWdevInfo.gw_info.product_id;
	init_params.init_param.device_name = GWdevInfo.gw_info.device_name;

#ifdef AUTH_MODE_CERT
	char certs_dir[PATH_MAX + 1] = "certs";
	char current_path[PATH_MAX + 1];
	char *cwd = getcwd(current_path, sizeof(current_path));
	if (cwd == NULL)
	{
		Log_e("getcwd return NULL");
		return QCLOUD_ERR_FAILURE;
	}
	sprintf(sg_cert_file, "%s/%s/%s", current_path, certs_dir, GWdevInfo.gw_info.dev_cert_file_name);
	sprintf(sg_key_file, "%s/%s/%s", current_path, certs_dir, GWdevInfo.gw_info.dev_key_file_name);

	init_params.init_param.cert_file = sg_cert_file;
	init_params.init_param.key_file = sg_key_file;
#else
	init_params.init_param.device_secret = GWdevInfo.gw_info.device_secret;
#endif

	init_params.init_param.command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	init_params.init_param.auto_connect_enable = 1;
    init_params.init_param.event_handle.h_fp = _event_handler;
	client = IOT_Gateway_Construct(&init_params);
	if (client == NULL) {
		Log_e("client constructed failed.");
		return QCLOUD_ERR_FAILURE;
	}

	// make sub-device online
	GatewayParam param = DEFAULT_GATEWAY_PARAMS;;
	param.product_id = GWdevInfo.gw_info.product_id;
	param.device_name = GWdevInfo.gw_info.device_name;

	DeviceInfo *subDevInfo;
	subDevInfo = GWdevInfo.sub_dev_info;

	param.subdev_product_id = subDevInfo->product_id;
	param.subdev_device_name = subDevInfo->device_name;
	
	rc = IOT_Gateway_Subdev_Online(client, &param);
	if(rc != QCLOUD_RET_SUCCESS) {
		Log_e("IOT_Gateway_Subdev_Online fail.");
		return rc;
	}
	
	// subscribe sub-device topic
	char topic_filter[MAX_SIZE_OF_TOPIC+1] = {0};
	SubscribeParams sub_param = DEFAULT_SUB_PARAMS;
	int size = HAL_Snprintf(topic_filter, MAX_SIZE_OF_TOPIC+1, "%s/%s/data", subDevInfo->product_id, subDevInfo->device_name);
	if (size < 0 || size > MAX_SIZE_OF_TOPIC)
	{
		Log_e("buf size < topic length!");
		return QCLOUD_ERR_FAILURE;
	}
	sub_param.on_message_handler = _message_handler;
	rc = IOT_Gateway_Subscribe(client, topic_filter, &sub_param);
	if(rc < 0) {
		Log_e("IOT_Gateway_Subscribe fail.");
		return rc;
	}

	rc = IOT_Gateway_Yield(client, 200);

	// publish msg to sub-device topic
	char topic_name[MAX_SIZE_OF_TOPIC+1] = {0};
	PublishParams pub_param = DEFAULT_PUB_PARAMS;
	size = HAL_Snprintf(topic_name, MAX_SIZE_OF_TOPIC+1, "%s/%s/data", subDevInfo->product_id, subDevInfo->device_name);
	if (size < 0 || size > MAX_SIZE_OF_TOPIC)
	{
		Log_e("buf size < topic length!");
		return QCLOUD_ERR_FAILURE;
	}

	pub_param.qos = QOS1;
	pub_param.payload = "{\"data\":\"test gateway\"}";
	pub_param.payload_len = sizeof("{\"data\":\"test gateway\"}");

	do {
		if(sg_sub_packet_id > 0) {
			rc = IOT_Gateway_Publish(client, topic_name, &pub_param);
			if(rc < 0) {
				Log_e("IOT_Gateway_Publish fail.");
			}
		}
		rc = IOT_Gateway_Yield(client, 200);

		if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
			HAL_SleepMs(1000);
			continue;
		}
		else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED){
			Log_e("exit with error: %d", rc);
			break;
		}

		HAL_SleepMs(1000);

	} while (--sg_loop_count > 0);

	// make sub-device offline
	rc = IOT_Gateway_Subdev_Offline(client, &param);
	if(rc != QCLOUD_RET_SUCCESS) {
		Log_e("IOT_Gateway_Subdev_Offline fail.");
		return rc;
	}
	
	rc = IOT_Gateway_Destroy(client);

	return rc;
}
