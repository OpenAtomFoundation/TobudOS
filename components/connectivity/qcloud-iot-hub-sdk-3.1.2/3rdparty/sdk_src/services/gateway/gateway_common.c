
#include "lite-utils.h"
#include "mqtt_client.h"
#include "gateway_common.h"


static char cloud_rcv_buf[GATEWAY_RECEIVE_BUFFER_LEN];


static bool get_json_type(char *json, char **v) {
	*v= LITE_json_value_of("type", json);
	return *v == NULL ? false : true;
}

static bool get_json_devices(char *json, char **v) {
	*v = LITE_json_value_of("payload.devices", json);
	return *v == NULL ? false: true;
}

static bool get_json_result(char *json, int32_t* res) {
	char *v = LITE_json_value_of("result", json);
	if (v == NULL) {
		return false;
	}
	if (LITE_get_int32(res, v) != QCLOUD_RET_SUCCESS) {
        HAL_Free(v);
		return false;
	}
    HAL_Free(v);
	return true;
}

static bool get_json_product_id(char *json, char **v) {
	*v = LITE_json_value_of("product_id", json);
	return *v == NULL ? false: true;
}


static bool get_json_device_name(char *json, char **v) {
	*v = LITE_json_value_of("device_name", json);
	return *v == NULL ? false: true;
}

static void _gateway_message_handler(void *client, MQTTMessage *message, void *user_data)
{
	Qcloud_IoT_Client *mqtt = NULL;
	Gateway *gateway = NULL;
	char *topic = NULL;
	size_t topic_len = 0;
	int cloud_rcv_len = 0;
	char *type = NULL;
	char *devices = NULL, *devices_strip = NULL;
	char *product_id = NULL;
	char *device_name = NULL;
	int32_t result = 0;
	char client_id[MAX_SIZE_OF_CLIENT_ID+1] = {0};
	int size = 0;	

	POINTER_SANITY_CHECK_RTN(client);
	POINTER_SANITY_CHECK_RTN(message);

	mqtt = (Qcloud_IoT_Client *)client;
	gateway = (Gateway*)mqtt->event_handle.context;
	POINTER_SANITY_CHECK_RTN(gateway);

	topic = (char *)message->ptopic;
	topic_len = message->topic_len;
	if (NULL == topic || topic_len <= 0) {
		Log_e("topic == NULL or topic_len <= 0.");
		return;
	}

	if (message->payload_len > GATEWAY_RECEIVE_BUFFER_LEN) {
		Log_e("message->payload_len > GATEWAY_RECEIVE_BUFFER_LEN.");
		return;
	}

	cloud_rcv_len = Min(GATEWAY_RECEIVE_BUFFER_LEN - 1, message->payload_len);
	memcpy(cloud_rcv_buf, message->payload, cloud_rcv_len + 1);
	cloud_rcv_buf[cloud_rcv_len] = '\0';    // jsmn_parse relies on a string

	if (!get_json_type(cloud_rcv_buf, &type))
	{
		Log_e("Fail to parse type from msg: %s", cloud_rcv_buf);
		return;
	}

	if (!get_json_devices(cloud_rcv_buf, &devices))
	{
		Log_e("Fail to parse devices from msg: %s", cloud_rcv_buf);
        HAL_Free(type);
		return;
	}

	if(devices[0] == '[') {
		devices_strip = devices+1;
	} else {
		devices_strip = devices;
	}

	if (!get_json_result(devices_strip, &result)) {
		Log_e("Fail to parse result from msg: %s", cloud_rcv_buf);
        HAL_Free(type);
        HAL_Free(devices);
		return;
	}
	if (!get_json_product_id(devices_strip, &product_id)) {
		Log_e("Fail to parse product_id from msg: %s", cloud_rcv_buf);
        HAL_Free(type);
        HAL_Free(devices);        
		return;
	}
	if (!get_json_device_name(devices_strip, &device_name)) {
		Log_e("Fail to parse device_name from msg: %s", cloud_rcv_buf);
        HAL_Free(type);
        HAL_Free(devices);        
        HAL_Free(product_id);
		return;
	}

	size = HAL_Snprintf(client_id, MAX_SIZE_OF_CLIENT_ID+1, GATEWAY_CLIENT_ID_FMT, product_id, device_name);
	if (size < 0 || size > MAX_SIZE_OF_CLIENT_ID){
		Log_e("generate client_id fail.");
		HAL_Free(type);
        HAL_Free(devices);
        HAL_Free(product_id);
        HAL_Free(device_name);
		return;
	}


	if (strncmp(type, "online", sizeof("online")-1) == 0) {
		if(strncmp(client_id,  gateway->gateway_data.online.client_id, size) == 0) {
			Log_i("client_id(%s), online success. result %d", client_id, result);
			gateway->gateway_data.online.result = result;
		}
	} else if(strncmp(type, "offline", sizeof("offline")-1) == 0) {
		if(strncmp(client_id,  gateway->gateway_data.offline.client_id, size) == 0) {
			Log_i("client_id(%s), offline success. result %d", client_id, result);
			gateway->gateway_data.offline.result = result;
		}
	}	
	
    HAL_Free(type);
    HAL_Free(devices);
    HAL_Free(product_id);
    HAL_Free(device_name);
    return;

}

int gateway_subscribe_unsubscribe_topic(Gateway *gateway, char *topic_filter, SubscribeParams *params, int is_subscribe)
{
	int rc = 0;
	int loop_count = 0;
	uint32_t status = -1;

	POINTER_SANITY_CHECK(gateway, QCLOUD_ERR_INVAL);                                          
	POINTER_SANITY_CHECK(params, QCLOUD_ERR_INVAL);

	STRING_PTR_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);

	params->qos = QOS1;
	gateway->gateway_data.sync_status = status;

	if (is_subscribe) {
		/* subscribe */
		rc = IOT_MQTT_Subscribe(gateway->mqtt, topic_filter, params);
	} else {
		/* unsubscribe */
		rc = IOT_MQTT_Unsubscribe(gateway->mqtt, topic_filter);
	}
	
	if (rc < 0) {
		Log_e("subscribe or un(%d), result(%d)", is_subscribe, rc);
		IOT_FUNC_EXIT_RC(rc);
	}
	
	gateway->gateway_data.sync_status = status = rc;
	while (status == gateway->gateway_data.sync_status) {
		if (loop_count > GATEWAY_LOOP_MAX_COUNT) {
			Log_i("loop max count, time out");
			IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
		}

		IOT_Gateway_Yield(gateway, 200);
		loop_count++;
	}

	if(gateway->gateway_data.sync_status != 0) {
		Log_e("gateway->gateway_data.sync_status(%u) != 0", gateway->gateway_data.sync_status);
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}

	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

int gateway_subscribe_unsubscribe_default(Gateway *gateway, GatewayParam *param)
{
	int rc = 0;
	int size = 0;
	char topic_filter[MAX_SIZE_OF_CLOUD_TOPIC+1] = {0};
	SubscribeParams subscribe_params = DEFAULT_SUB_PARAMS;  
	
	POINTER_SANITY_CHECK(param, QCLOUD_ERR_INVAL);                                                                                               

	STRING_PTR_SANITY_CHECK(param->product_id, QCLOUD_ERR_INVAL);
	STRING_PTR_SANITY_CHECK(param->device_name, QCLOUD_ERR_INVAL);

	//subscribe  online/offline operation reslut 
	size = HAL_Snprintf(topic_filter, MAX_SIZE_OF_CLOUD_TOPIC+1, GATEWAY_TOPIC_OPERATION_RESULT_FMT, param->product_id, param->device_name); 
	if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC)                                                                                          
	{                                                                                                                                            
		Log_e("buf size < topic length!");                                                                                                   
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);                                                                                                
	}  
	subscribe_params.on_message_handler = _gateway_message_handler;
	rc = gateway_subscribe_unsubscribe_topic(gateway, topic_filter, &subscribe_params, IOT_TRUE);
	if (QCLOUD_RET_SUCCESS != rc) {
		IOT_FUNC_EXIT_RC(rc);
	}

	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

SubdevSession* subdev_find_session(Gateway *gateway, char *product_id, char *device_name)
{
    SubdevSession* session = NULL;

    POINTER_SANITY_CHECK(gateway, NULL);
    STRING_PTR_SANITY_CHECK(product_id, NULL);
    STRING_PTR_SANITY_CHECK(device_name, NULL);

    session = gateway->session_list;

    /* session is exist */
    while(session) {
        if (0 == strncmp(session->product_id, product_id, strlen(product_id)) &&
           0 == strncmp(session->device_name, device_name, strlen(device_name))) {
            IOT_FUNC_EXIT_RC(session);
        }
        session = session->next;
    }

    IOT_FUNC_EXIT_RC(NULL);
}

SubdevSession* subdev_add_session(Gateway *gateway, char *product_id, char *device_name)
{
    SubdevSession* session = NULL;

    POINTER_SANITY_CHECK(gateway, NULL);
    STRING_PTR_SANITY_CHECK(product_id, NULL);
    STRING_PTR_SANITY_CHECK(device_name, NULL);

    session = HAL_Malloc(sizeof(SubdevSession));
    if(session == NULL) {
	Log_e("Not enough memory");
    	IOT_FUNC_EXIT_RC(NULL);
    }

    /* add session to list */
    session->next = gateway->session_list;
    gateway->session_list = session;

    int size = strlen(product_id);
    strncpy(session->product_id, product_id, size);
    session->product_id[size] = '\0';
    size = strlen(device_name);
    strncpy(session->device_name, device_name, size);
    session->device_name[size] = '\0';
    session->session_status = SUBDEV_SEESION_STATUS_INIT;

    IOT_FUNC_EXIT_RC(session);
}

int subdev_remove_session(Gateway *gateway, char *product_id, char *device_name)
{
    SubdevSession* cur_session = NULL;
    SubdevSession* pre_session = NULL;

    POINTER_SANITY_CHECK(gateway, QCLOUD_ERR_FAILURE);
    STRING_PTR_SANITY_CHECK(product_id, QCLOUD_ERR_FAILURE);
    STRING_PTR_SANITY_CHECK(device_name, QCLOUD_ERR_FAILURE);

    pre_session = cur_session = gateway->session_list;

    if (NULL == cur_session) {
        Log_e("session list is empty");
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
    }

    /* session is exist */
    while(cur_session) {
        if (0 == strncmp(cur_session->product_id, product_id, strlen(product_id)) &&
            0 == strncmp(cur_session->device_name, device_name, strlen(device_name)) ) {
            if (cur_session == gateway->session_list) {
                gateway->session_list = cur_session->next;
            } else {
                pre_session->next = cur_session->next;
            }
            HAL_Free(cur_session);
            IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
        }
        pre_session = cur_session;
        cur_session = cur_session->next;
    }

    IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
}


int gateway_publish_sync(Gateway *gateway, char *topic, PublishParams *params, int32_t *result)
{
	int rc = 0;
	int loop_count = 0;
	int32_t res = *result;

	POINTER_SANITY_CHECK(gateway, QCLOUD_ERR_INVAL);

	rc = IOT_Gateway_Publish(gateway, topic, params);
	if(rc < 0) {
		Log_e("publish fail.");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}

	/* wait for response */
	while (res == *result) {
		if (loop_count > GATEWAY_LOOP_MAX_COUNT) {
			Log_i("loop max count, time out.");
			IOT_FUNC_EXIT_RC(QCLOUD_ERR_GATEWAY_SESSION_TIMEOUT);
		}

		IOT_Gateway_Yield(gateway, 200);
		loop_count++;
	}

	if(*result != 0) {
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}
	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

