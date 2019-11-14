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



#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"


#ifdef SYSTEM_COMM 

#include "qcloud_iot_export_system.h"
#include "mqtt_client.h"
#include "lite-utils.h"
#include "device.h"

typedef struct _sys_mqtt_state {
    bool topic_sub_ok;
    bool result_recv_ok;
    long  time;    
}SysMQTTState;

static SysMQTTState sg_state = {
            .topic_sub_ok = false, 
            .result_recv_ok = false,
            .time = 0};


static void _system_mqtt_message_callback(void *pClient, MQTTMessage *message, void *pUserData)
{
#define MAX_RECV_LEN (512)

    POINTER_SANITY_CHECK_RTN(message);	

    static char rcv_buf[MAX_RECV_LEN + 1];
	size_t len = (message->payload_len > MAX_RECV_LEN)?MAX_RECV_LEN:(message->payload_len);

	if(message->payload_len > MAX_RECV_LEN){
		Log_e("payload len oversize");
	}
    memcpy(rcv_buf, message->payload, len);
    rcv_buf[len] = '\0';    // jsmn_parse relies on a string
    SysMQTTState *state = (SysMQTTState *)pUserData;

    Log_d("Recv Msg Topic:%s, payload:%s", message->ptopic, rcv_buf);

    char* value = LITE_json_value_of("time", rcv_buf);
    if (value != NULL)
        state->time = atol(value);

    state->result_recv_ok = true;
    HAL_Free(value);
    return;
}

static void _system_mqtt_sub_event_handler(void *pclient, MQTTEventType event_type, void *pUserData)
{
    SysMQTTState *state = (SysMQTTState *)pUserData;

    switch(event_type) {
        case MQTT_EVENT_SUBCRIBE_SUCCESS:
            Log_d("mqtt sys topic subscribe success");
            state->topic_sub_ok = true;         
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            Log_i("mqtt sys topic subscribe timeout");
            state->topic_sub_ok = false;            
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            Log_i("mqtt sys topic subscribe NACK");
            state->topic_sub_ok = false;            
            break;
        case MQTT_EVENT_UNSUBSCRIBE:
            Log_i("mqtt sys topic has been unsubscribed");
            state->topic_sub_ok = false;;           
            break;
        case MQTT_EVENT_CLIENT_DESTROY:
            Log_i("mqtt client has been destroyed");
            state->topic_sub_ok = false;;           
            break;
        default:
            return;
    }
}

static int _iot_system_info_get_publish(void *pClient)
{
    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    Qcloud_IoT_Client   *mqtt_client = (Qcloud_IoT_Client *)pClient;
    DeviceInfo          *dev_info = iot_device_info_get();
    POINTER_SANITY_CHECK(dev_info, QCLOUD_ERR_INVAL);

    char topic_name[128] = {0};
    char payload_content[128] = {0};

    HAL_Snprintf(topic_name, sizeof(topic_name), "$sys/operation/%s/%s", dev_info->product_id, dev_info->device_name);
    HAL_Snprintf(payload_content, sizeof(payload_content), "{\"type\": \"get\", \"resource\": [\"time\"]}");

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos = QOS0;
    pub_params.payload = payload_content;
    pub_params.payload_len = strlen(payload_content);

	return IOT_MQTT_Publish(mqtt_client, topic_name, &pub_params);
}

static int _iot_system_info_result_subscribe(void *pClient)
{
    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    DeviceInfo          *dev_info = iot_device_info_get();
    POINTER_SANITY_CHECK(dev_info, QCLOUD_ERR_INVAL);

    char topic_name[128] = {0};
    int size = HAL_Snprintf(topic_name, sizeof(topic_name), "$sys/operation/result/%s/%s", dev_info->product_id, dev_info->device_name);
    if (size < 0 || size > sizeof(topic_name) - 1)
    {
        Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
        return QCLOUD_ERR_FAILURE;
    }
    SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = _system_mqtt_message_callback;
    sub_params.on_sub_event_handler = _system_mqtt_sub_event_handler;
    sub_params.user_data = (void *)&sg_state;
    sub_params.qos = QOS0;

    return IOT_MQTT_Subscribe(pClient, topic_name, &sub_params);
}

int IOT_Get_SysTime(void* pClient, long *time)
{
    int ret = 0;
    int cntSub = 0;
    int cntRev = 0;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);
    Qcloud_IoT_Client   *mqtt_client = (Qcloud_IoT_Client *)pClient;

    // subscribe sys topic: $sys/operation/get/${productid}/${devicename}
    // skip this if the subscription is done and valid
    if(!sg_state.topic_sub_ok){
        for(cntSub = 0; cntSub < 3; cntSub++){
            ret = _iot_system_info_result_subscribe(mqtt_client);
            if (ret < 0) {
                Log_w("_iot_system_info_result_subscribe failed: %d, cnt: %d", ret, cntSub);
                continue;
            }

            /* wait for sub ack */
            ret = qcloud_iot_mqtt_yield((Qcloud_IoT_Client *)pClient, 100);
            if(sg_state.topic_sub_ok) {                
                break;
            }
        }
    }

    // return failure if subscribe failed
    if(!sg_state.topic_sub_ok){
        Log_e("Subscribe sys topic failed!");
        return QCLOUD_ERR_FAILURE;
    }

    sg_state.result_recv_ok = false;
    // publish msg to get system timestamp
	ret = _iot_system_info_get_publish(mqtt_client);
	if (ret < 0) {
		Log_e("client publish sys topic failed :%d.", ret);
        return ret;
	}

    do {
        ret = qcloud_iot_mqtt_yield((Qcloud_IoT_Client *)pClient, 100);
        cntRev++;
    } while(!sg_state.result_recv_ok && cntRev < 20);

    
    if (sg_state.result_recv_ok) {        
        *time = sg_state.time;
        ret = QCLOUD_RET_SUCCESS;        
    } else {
        *time = 0;
        ret = QCLOUD_ERR_FAILURE;
    }  

    return ret;
}

#ifdef __cplusplus
}
#endif

#endif
