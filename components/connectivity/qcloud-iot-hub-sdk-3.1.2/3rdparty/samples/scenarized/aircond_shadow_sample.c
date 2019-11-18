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

#include "utils_getopt.h"
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "lite-utils.h"

#ifdef AUTH_MODE_CERT
    static char sg_cert_file[PATH_MAX + 1];      // full path of device cert file
    static char sg_key_file[PATH_MAX + 1];       // full path of device key file
#endif


static DeviceInfo sg_devInfo;


#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 200

#define ROOM_TEMPERATURE 32.0f
static float sg_desire_temperature = 20.0f;
static float sg_report_temperature = ROOM_TEMPERATURE;

static float sg_energy_consumption = 0.0f;
static bool sg_airconditioner_openned = false;

static MQTTEventType sg_subscribe_event_result = MQTT_EVENT_UNDEF;

#define MAX_RECV_LEN (512 + 1)


// compare float value
bool _is_value_equal(float left, float right)
{
    if((left<right+0.01) && (left>right-0.01))
        return true;
    else
        return false;
}


// simulate room temperature change
static void _simulate_room_temperature(float *roomTemperature) {
    float delta_change = 0;

    if (!sg_airconditioner_openned) {
        if(!_is_value_equal(*roomTemperature, ROOM_TEMPERATURE)) {
            delta_change = (*roomTemperature)>ROOM_TEMPERATURE ? -0.5: 0.5;
        }
    } else {
        sg_energy_consumption += 1;
        if (!_is_value_equal(*roomTemperature, sg_desire_temperature)) {
            delta_change = (*roomTemperature)>sg_desire_temperature ? -1.0: 1.0;
        }
    }

    *roomTemperature += delta_change;
}

// MQTT event callback
static void event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg) 
{	
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

		case MQTT_EVENT_SUBCRIBE_SUCCESS:
            sg_subscribe_event_result = msg->event_type;
			Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            sg_subscribe_event_result = msg->event_type;
			Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
            sg_subscribe_event_result = msg->event_type;
			Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
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

// callback when MQTT msg arrives
static void on_message_callback(void *pClient, MQTTMessage *message, void *userData) 
{
    if (message == NULL)
        return;

    const char *topicName = message->ptopic; 
    size_t topicNameLen = message->topic_len;

    if (topicName == NULL || topicNameLen == 0) {
        return;
    }

    Log_i("Receive Message With topicName:%.*s, payload:%.*s",
          (int) topicNameLen, topicName, (int) message->payload_len, (char *) message->payload);


	static char cloud_rcv_buf[MAX_RECV_LEN + 1];
	size_t len = (message->payload_len > MAX_RECV_LEN)?MAX_RECV_LEN:(message->payload_len);

	if(message->payload_len > MAX_RECV_LEN){
		Log_e("paload len exceed buffer size");
	}

	memcpy(cloud_rcv_buf, message->payload, len);
	cloud_rcv_buf[len] = '\0';    // jsmn_parse relies on a string

    char* value = LITE_json_value_of("action", cloud_rcv_buf);
    if (value != NULL) {
        if(strcmp(value, "come_home") == 0)
        {
            sg_airconditioner_openned = true;
        }
        else if(strcmp(value, "leave_home") == 0)
        {
            sg_airconditioner_openned = false;
        }
    }

    HAL_Free(value);
}

// Setup MQTT construct parameters
static int _setup_connect_init_params(ShadowInitParams* initParams)
{
	int ret;

	ret = HAL_GetDevInfo((void *)&sg_devInfo);	
	if(QCLOUD_RET_SUCCESS != ret){
		return ret;
	}
	
	initParams->device_name = sg_devInfo.device_name;
	initParams->product_id = sg_devInfo.product_id;

#ifdef AUTH_MODE_CERT
	char certs_dir[PATH_MAX + 1] = "certs";
	char current_path[PATH_MAX + 1];
	char *cwd = getcwd(current_path, sizeof(current_path));
	if (cwd == NULL)
	{
		Log_e("getcwd return NULL");
		return QCLOUD_ERR_FAILURE;
	}
	sprintf(sg_cert_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_cert_file_name);
	sprintf(sg_key_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_key_file_name);

	initParams->cert_file = sg_cert_file;
	initParams->key_file = sg_key_file;
#else
	initParams->device_secret = sg_devInfo.device_secret;
#endif

    initParams->auto_connect_enable = 1;
    initParams->event_handle.h_fp = event_handler;

    return QCLOUD_RET_SUCCESS;
}

// subscrib MQTT topic
static int _register_subscribe_topics(void *client)
{
    static char topic_name[128] = {0};
    int size = HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", sg_devInfo.product_id, sg_devInfo.device_name, "control");
    if (size < 0 || size > sizeof(topic_name) - 1)
    {
        Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
        return QCLOUD_ERR_FAILURE;
    }
    SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = on_message_callback;
    return IOT_Shadow_Subscribe(client, topic_name, &sub_params);
}

int main(int argc, char **argv)
{
    int c;
    while ((c = utils_getopt(argc, argv, "c:")) != EOF)
        switch (c) 
        {
            case 'c':
                if (HAL_SetDevInfoFile(utils_optarg))
                    return -1;
                break;

            default:
                HAL_Printf("usage: %s [options]\n"
                    "  [-c <config file for DeviceInfo>] \n"
                    , argv[0]);
                
            return -1;
        }


    int rc;

    //init log level
    IOT_Log_Set_Level(eLOG_DEBUG);

    //init connection
    ShadowInitParams init_params = DEFAULT_SHAWDOW_INIT_PARAMS;
    rc = _setup_connect_init_params(&init_params);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("init params err,rc=%d", rc);
		return rc;
	}

    void *client = IOT_Shadow_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("Cloud Device Construct Failed");
        return QCLOUD_ERR_FAILURE;
    }

    //register subscribe topics here
    rc = _register_subscribe_topics(client);
    if (rc < 0) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        return rc;
    }

    while (IOT_Shadow_IsConnected(client) || rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT || rc == QCLOUD_RET_MQTT_RECONNECTED) {

        rc = IOT_Shadow_Yield(client, 200);

        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            HAL_SleepMs(1000);
            continue;
        }
		else if (rc != QCLOUD_RET_SUCCESS) {
			Log_e("Exit loop caused of errCode: %d", rc);
		}

        if (sg_subscribe_event_result != MQTT_EVENT_SUBCRIBE_SUCCESS &&
            sg_subscribe_event_result != MQTT_EVENT_SUBCRIBE_TIMEOUT &&
            sg_subscribe_event_result != MQTT_EVENT_SUBCRIBE_NACK)
        {
            Log_i("Wait for subscribe result. sg_subscribe_event_result = %d", sg_subscribe_event_result);
            HAL_SleepMs(1000);
            continue;
        }

        _simulate_room_temperature(&sg_report_temperature);
        Log_i("airConditioner state: %s", sg_airconditioner_openned ? "open" : "close");
        Log_i("currentTemperature: %f, energyConsumption: %f", sg_report_temperature, sg_energy_consumption);

        HAL_SleepMs(1000);
    }

    rc = IOT_Shadow_Destroy(client);

    return rc;
}
