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
#include <signal.h>

#include "utils_getopt.h"
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"


#ifdef AUTH_MODE_CERT
    static char sg_cert_file[PATH_MAX + 1];      // full path of device cert file
    static char sg_key_file[PATH_MAX + 1];       // full path of device key file
#endif


static DeviceInfo sg_devInfo;
static int sg_count = 0;
static int sg_sub_packet_id = -1;


// user's log print callback
static bool log_handler(const char* message)
{	
	// return true if print success
	return false;
}

// MQTT event callback
static void _mqtt_event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg)
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


// Setup MQTT construct parameters
static int _setup_connect_init_params(MQTTInitParams* initParams)
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

#ifdef WIN32
	sprintf(sg_cert_file, "%s\\%s\\%s", current_path, certs_dir, sg_devInfo.dev_cert_file_name);
	sprintf(sg_key_file, "%s\\%s\\%s", current_path, certs_dir, sg_devInfo.dev_key_file_name);
#else 
	sprintf(sg_cert_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_cert_file_name);
	sprintf(sg_key_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_key_file_name);
#endif

	initParams->cert_file = sg_cert_file;
	initParams->key_file = sg_key_file;
#else
	initParams->device_secret = sg_devInfo.device_secret;
#endif

	initParams->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

	initParams->auto_connect_enable = 1;
	initParams->event_handle.h_fp = _mqtt_event_handler;
	initParams->event_handle.context = NULL;

    return QCLOUD_RET_SUCCESS;
}

#define MAX_SIZE_OF_TOPIC_CONTENT 100
// publish MQTT msg
static int _publish_test_msg(void *client, char *topic_keyword, QoS qos)
{
    char topicName[128] = {0};
	sprintf(topicName,"%s/%s/%s", sg_devInfo.product_id, sg_devInfo.device_name, topic_keyword);	

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos = qos;

    char topic_content[MAX_SIZE_OF_TOPIC_CONTENT + 1] = {0};

	int size = HAL_Snprintf(topic_content, sizeof(topic_content), "{\"action\": \"publish_test\", \"count\": \"%d\"}", sg_count++);
	if (size < 0 || size > sizeof(topic_content) - 1) {
		Log_e("payload content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_content));
		return -3;
	}

	pub_params.payload = topic_content;
	pub_params.payload_len = strlen(topic_content);

    return IOT_MQTT_Publish(client, topicName, &pub_params);
}

// callback when MQTT msg arrives
static void on_message_callback(void *pClient, MQTTMessage *message, void *userData)
{
	if (message == NULL) {
		return;
	}

	Log_i("Receive Message With topicName:%.*s, payload:%.*s",
		  (int) message->topic_len, message->ptopic, (int) message->payload_len, (char *) message->payload);
}

// subscrib MQTT topic
static int _subscribe_topic(void *client, char *topic_keyword, QoS qos)
{
    static char topic_name[128] = {0};    
    int size = HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", sg_devInfo.product_id, sg_devInfo.device_name, topic_keyword);
	
	if (size < 0 || size > sizeof(topic_name) - 1) {
        Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
        return QCLOUD_ERR_FAILURE;
    }
    SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
	sub_params.qos = qos;
    sub_params.on_message_handler = on_message_callback;
    return IOT_MQTT_Subscribe(client, topic_name, &sub_params);
}

#ifdef LOG_UPLOAD
// init log upload module
static int _init_log_upload(void)
{
    LogUploadInitParams log_init_params;
    memset(&log_init_params, 0, sizeof(LogUploadInitParams));

    log_init_params.product_id = sg_devInfo.product_id;
    log_init_params.device_name = sg_devInfo.device_name;
#ifdef AUTH_MODE_CERT    
    log_init_params.sign_key = sg_cert_file;
#else
    log_init_params.sign_key = sg_devInfo.device_secret;
#endif

#if defined(__linux__) || defined(WIN32)
    log_init_params.read_func = HAL_Log_Read;
    log_init_params.save_func = HAL_Log_Save;
    log_init_params.del_func  = HAL_Log_Del;
    log_init_params.get_size_func = HAL_Log_Get_Size;
#endif

    return IOT_Log_Init_Uploader(&log_init_params);

}

#endif

int mqtt_basic_thread(void)
{
    int rc;
    //init log level
    IOT_Log_Set_Level(eLOG_DEBUG);
    IOT_Log_Set_MessageHandler(log_handler);    
 
    //init connection
    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
    rc = _setup_connect_init_params(&init_params);
	if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("init params error, rc = %d", rc);
        return rc;
	}	

#ifdef LOG_UPLOAD
    // _init_log_upload should be done after _setup_connect_init_params and before IOT_MQTT_Construct
    rc = _init_log_upload();
    if (rc != QCLOUD_RET_SUCCESS)
        Log_e("init log upload error, rc = %d", rc);        
#endif

    // create MQTT client and connect with server
    void *client = IOT_MQTT_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        rc = IOT_MQTT_GetErrCode();
        Log_e("MQTT Construct failed, rc = %d", rc);
        IOT_Log_Upload(true);
        return QCLOUD_ERR_FAILURE;
    }

#ifdef SYSTEM_COMM
    long time = 0;
    // get system timestamp from server
    rc = IOT_Get_SysTime(client, &time);
    if (QCLOUD_RET_SUCCESS == rc){
        Log_i("system time is %ld", time);
    } else {
        Log_e("get system time failed!");
    }
#endif

	//subscribe normal topics here
    rc = _subscribe_topic(client, "data", QOS0);
    if (rc < 0) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        IOT_Log_Upload(true);
        return rc;
    }	

    // wait for subscription result
    IOT_MQTT_Yield(client, 500);
    
    do {
        
        if (sg_sub_packet_id > 0) {
            rc = _publish_test_msg(client, "data", QOS1);
            if (rc < 0) {
                Log_e("client publish topic failed :%d.", rc);
            }
        }

        rc = IOT_MQTT_Yield(client, 500);
        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            HAL_SleepMs(1000);
            continue;
        } else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED){
            Log_e("exit with error: %d", rc);            
            break;
        }
    } while (1);

    rc = IOT_MQTT_Destroy(&client);
    
    IOT_Log_Upload(true);

    return rc;
}
