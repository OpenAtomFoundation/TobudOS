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

#ifdef __cplusplus
extern "C" {
#endif

#include "shadow_client.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils_param_check.h"
#include "shadow_client_json.h"
#include "shadow_client_common.h"


static void _init_request_params(RequestParams *pParams, Method method, OnRequestCallback callback, void *userContext, uint8_t timeout_sec) {
	pParams->method 			=		method;
	pParams->user_context 		= 		userContext;
	pParams->timeout_sec 		= 		timeout_sec;
	pParams->request_callback 	= 		callback;
}

/**
 * @brief check return value of snprintf
 *
 * @param returnCode       return value of snprintf
 * @param maxSizeOfWrite   max size of write buffer
 * @return                 QCLOUD_RET_SUCCESS for success, or err code for failure
 */
static inline int _check_snprintf_return(int32_t returnCode, size_t maxSizeOfWrite) {

    if (returnCode >= maxSizeOfWrite) {
        return QCLOUD_ERR_JSON_BUFFER_TRUNCATED;
    } else if (returnCode < 0) {
        return QCLOUD_ERR_JSON;
    }

    return QCLOUD_RET_SUCCESS;
}

static void _shadow_event_handler(void *pclient, void *context, MQTTEventMsg *msg) {
	uintptr_t packet_id = (uintptr_t)msg->msg;
	Qcloud_IoT_Shadow *shadow_client = (Qcloud_IoT_Shadow *)context;
	MQTTMessage* topic_info = (MQTTMessage*)msg->msg;

	switch (msg->event_type) {
		case MQTT_EVENT_SUBCRIBE_SUCCESS:
			Log_d("shadow subscribe success, packet-id=%u", (unsigned int)packet_id);
			if (shadow_client->inner_data.sync_status > 0)
				shadow_client->inner_data.sync_status = 0;
			break;
		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
			Log_d("shadow subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			if (shadow_client->inner_data.sync_status > 0)
				shadow_client->inner_data.sync_status = -1;
			break;
		case MQTT_EVENT_SUBCRIBE_NACK:
			Log_d("shadow subscribe nack, packet-id=%u", (unsigned int)packet_id);
			if (shadow_client->inner_data.sync_status > 0)
				shadow_client->inner_data.sync_status = -1;
			break;
		case MQTT_EVENT_PUBLISH_RECVEIVED:
			Log_d("shadow topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
					 topic_info->topic_len,
					 topic_info->ptopic,
					 topic_info->payload_len,
					 topic_info->payload);
			break;
		default:
			/* Log_i("Should NOT arrive here."); */
			break;
	}
	if (shadow_client->event_handle.h_fp != NULL)
	{
		shadow_client->event_handle.h_fp(shadow_client, shadow_client->event_handle.context, msg);
	}
}

static void _copy_shadow_init_params_to_mqtt(MQTTInitParams *pMqttInitParams, ShadowInitParams *shadowInitParams)
{
	pMqttInitParams->device_name = shadowInitParams->device_name;
    pMqttInitParams->product_id = shadowInitParams->product_id;

#ifdef AUTH_MODE_CERT
	pMqttInitParams->cert_file = shadowInitParams->cert_file;
	pMqttInitParams->key_file = shadowInitParams->key_file;
#else
	pMqttInitParams->device_secret = shadowInitParams->device_secret;
#endif

	pMqttInitParams->command_timeout = shadowInitParams->command_timeout;
    pMqttInitParams->keep_alive_interval_ms = shadowInitParams->keep_alive_interval_ms;
    pMqttInitParams->clean_session = shadowInitParams->clean_session;
    pMqttInitParams->auto_connect_enable = shadowInitParams->auto_connect_enable;
}

static void _update_ack_cb(void *pClient, Method method, RequestAck requestAck, const char *pReceivedJsonDocument, void *pUserdata) 
{
	Log_d("requestAck=%d", requestAck);

    if (NULL != pReceivedJsonDocument) {
        Log_d("Received Json Document=%s", pReceivedJsonDocument);
    } else {
        Log_d("Received Json Document is NULL");
    }

    *((RequestAck *)pUserdata) = requestAck;
}

void* IOT_Shadow_Construct(ShadowInitParams *pParams)
{
	POINTER_SANITY_CHECK(pParams, NULL);

	Qcloud_IoT_Shadow *shadow_client = NULL;
	if ((shadow_client = (Qcloud_IoT_Shadow *)HAL_Malloc(sizeof(Qcloud_IoT_Shadow))) == NULL) {
		Log_e("memory not enough to malloc ShadowClient");
	}

	MQTTInitParams mqtt_init_params;
	_copy_shadow_init_params_to_mqtt(&mqtt_init_params, pParams);

	mqtt_init_params.event_handle.h_fp = _shadow_event_handler;
	mqtt_init_params.event_handle.context = shadow_client;

	void *mqtt_client = NULL;
	if ((mqtt_client = IOT_MQTT_Construct(&mqtt_init_params)) == NULL) {
		HAL_Free(shadow_client);
		goto End;
	}

	shadow_client->mqtt = mqtt_client;
	shadow_client->shadow_type = pParams->shadow_type;
	shadow_client->event_handle = pParams->event_handle;
	shadow_client->inner_data.result_topic = NULL;
	shadow_client->inner_data.token_num = 0;
	
	int rc;

	rc = qcloud_iot_shadow_init(shadow_client);
	if (rc != QCLOUD_RET_SUCCESS) {
		IOT_MQTT_Destroy(&(shadow_client->mqtt));
		HAL_Free(shadow_client);
		goto End;
	}
	
	rc = subscribe_operation_result_to_cloud(shadow_client);
	if (rc < 0)
	{
		Log_e("Subcribe $shadow/operation/results fail!");
	}
	else {
		shadow_client->inner_data.sync_status = rc;
		while (rc == shadow_client->inner_data.sync_status) {
	        IOT_Shadow_Yield(shadow_client, 100);
	    }
		if (0 == shadow_client->inner_data.sync_status) {
	        Log_i("Sync device data successfully");
	    } else {
	        Log_e("Sync device data failed");
	    }
	}

	return shadow_client;

End:
	return NULL;
}

int IOT_Shadow_Publish(void *handle, char *topicName, PublishParams *pParams) {
	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow   *shadow = (Qcloud_IoT_Shadow *)handle;

	return qcloud_iot_mqtt_publish(shadow->mqtt, topicName, pParams);
}

int IOT_Shadow_Subscribe(void *handle, char *topicFilter, SubscribeParams *pParams) {
	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow   *shadow = (Qcloud_IoT_Shadow *)handle;

	return qcloud_iot_mqtt_subscribe(shadow->mqtt, topicFilter, pParams);
}

int IOT_Shadow_Unsubscribe(void *handle, char *topicFilter) {
	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow   *shadow = (Qcloud_IoT_Shadow *)handle;

	return qcloud_iot_mqtt_unsubscribe(shadow->mqtt, topicFilter);
}

bool IOT_Shadow_IsConnected(void *handle)
{
	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow* pshadow = (Qcloud_IoT_Shadow*)handle;

	IOT_FUNC_EXIT_RC(IOT_MQTT_IsConnected(pshadow->mqtt))
}

int IOT_Shadow_Destroy(void *handle)
{
	IOT_FUNC_ENTRY;

	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow* shadow_client = (Qcloud_IoT_Shadow*)handle;
	qcloud_iot_shadow_reset(handle);

	IOT_MQTT_Destroy(&shadow_client->mqtt);

    if (NULL != shadow_client->mutex) {
        HAL_MutexDestroy(shadow_client->mutex);
    }

	if (NULL != shadow_client->inner_data.result_topic) {
        HAL_Free(shadow_client->inner_data.result_topic);
		shadow_client->inner_data.result_topic = NULL;
    }

    HAL_Free(handle);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS)
}

int IOT_Shadow_Yield(void *handle, uint32_t timeout_ms) {

    IOT_FUNC_ENTRY;
    int rc;

	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	NUMBERIC_SANITY_CHECK(timeout_ms, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow *pshadow = (Qcloud_IoT_Shadow *)handle;
	POINTER_SANITY_CHECK(pshadow, QCLOUD_ERR_INVAL);

    handle_expired_request(pshadow);

    rc = IOT_MQTT_Yield(pshadow->mqtt, timeout_ms);

    IOT_FUNC_EXIT_RC(rc);
}

int IOT_Shadow_Register_Property(void *handle, DeviceProperty *pProperty, OnPropRegCallback callback) {

    IOT_FUNC_ENTRY;
	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow* pshadow = (Qcloud_IoT_Shadow*)handle;
	int rc;

    if (IOT_MQTT_IsConnected(pshadow->mqtt) == false) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

	if (shadow_common_check_property_existence(pshadow, pProperty)) 
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_SHADOW_PROPERTY_EXIST);

    rc = shadow_common_register_property_on_delta(pshadow, pProperty, callback);

    IOT_FUNC_EXIT_RC(rc);
}

int IOT_Shadow_UnRegister_Property(void *handle, DeviceProperty *pProperty) {
	IOT_FUNC_ENTRY;
	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	Qcloud_IoT_Shadow* pshadow = (Qcloud_IoT_Shadow*)handle;

	if (IOT_MQTT_IsConnected(pshadow->mqtt) == false) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

	if (!shadow_common_check_property_existence(pshadow, pProperty)) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_SHADOW_NOT_PROPERTY_EXIST);
    }
	int rc =  shadow_common_remove_property(pshadow, pProperty);
	IOT_FUNC_EXIT_RC(rc);
}

int IOT_Shadow_Update(void *handle, char *pJsonDoc, size_t sizeOfBuffer, OnRequestCallback callback, void *userContext, uint32_t timeout_ms) {

    IOT_FUNC_ENTRY;
	int rc;

	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(pJsonDoc, QCLOUD_ERR_INVAL);
	NUMBERIC_SANITY_CHECK(timeout_ms, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow* shadow = (Qcloud_IoT_Shadow*)handle;

	if (IOT_MQTT_IsConnected(shadow->mqtt) == false) {
		Log_e("shadow is disconnected");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
	}

	// subscribe topic $shadow/operation/result if not subscribed yet
	if (shadow->inner_data.sync_status < 0) {
		subscribe_operation_result_to_cloud(shadow);
	}

	Log_d("UPDATE Request Document: %s", pJsonDoc);

	RequestParams request_params = DEFAULT_REQUEST_PARAMS;
	_init_request_params(&request_params, UPDATE, callback, userContext, timeout_ms/1000);

	rc = do_shadow_request(shadow, &request_params, pJsonDoc, sizeOfBuffer);
	IOT_FUNC_EXIT_RC(rc);
}

int IOT_Shadow_Update_Sync(void *handle, char *pJsonDoc, size_t sizeOfBuffer, uint32_t timeout_ms) {

    IOT_FUNC_ENTRY;
	int rc = QCLOUD_RET_SUCCESS;

	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(pJsonDoc, QCLOUD_ERR_INVAL);
	NUMBERIC_SANITY_CHECK(timeout_ms, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow* shadow = (Qcloud_IoT_Shadow*)handle;

	if (IOT_MQTT_IsConnected(shadow->mqtt) == false) {
		Log_e("shadow is disconnected");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
	}

	RequestAck ack_update = ACK_NONE;
	rc = IOT_Shadow_Update(handle, pJsonDoc, sizeOfBuffer, _update_ack_cb, &ack_update, timeout_ms);
	if (rc != QCLOUD_RET_SUCCESS) IOT_FUNC_EXIT_RC(rc);

	while (ACK_NONE == ack_update) {
        IOT_Shadow_Yield(handle, 200);
    }

	if (ACK_ACCEPTED == ack_update) {
        rc = QCLOUD_RET_SUCCESS;
    } else if (ACK_TIMEOUT == ack_update) {
        rc = QCLOUD_ERR_SHADOW_UPDATE_TIMEOUT;
    } else if (ACK_REJECTED == ack_update) {
        rc = QCLOUD_ERR_SHADOW_UPDATE_REJECTED;
    }
	
	IOT_FUNC_EXIT_RC(rc);
}

int IOT_Shadow_Get(void *handle, OnRequestCallback callback, void *userContext, uint32_t timeout_ms) {

    IOT_FUNC_ENTRY;
	int rc;

	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(callback, QCLOUD_ERR_INVAL);
	NUMBERIC_SANITY_CHECK(timeout_ms, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow* shadow = (Qcloud_IoT_Shadow*)handle;

	if (IOT_MQTT_IsConnected(shadow->mqtt) == false) {
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
	}

	// subscribe topic $shadow/operation/result if not subscribed yet
	if (shadow->inner_data.sync_status < 0) {
		subscribe_operation_result_to_cloud(shadow);
	}

	char getRequestJsonDoc[MAX_SIZE_OF_JSON_WITH_CLIENT_TOKEN];
	build_empty_json(&(shadow->inner_data.token_num), getRequestJsonDoc);
	Log_d("GET Request Document: %s", getRequestJsonDoc);

	RequestParams request_params = DEFAULT_REQUEST_PARAMS;
	_init_request_params(&request_params, GET, callback, userContext, timeout_ms/1000);

	rc = do_shadow_request(shadow, &request_params, getRequestJsonDoc, MAX_SIZE_OF_JSON_WITH_CLIENT_TOKEN);
	IOT_FUNC_EXIT_RC(rc);
}

int IOT_Shadow_Get_Sync(void *handle, uint32_t timeout_ms) {

    IOT_FUNC_ENTRY;
	int rc = QCLOUD_RET_SUCCESS;

	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	NUMBERIC_SANITY_CHECK(timeout_ms, QCLOUD_ERR_INVAL);

	Qcloud_IoT_Shadow* shadow = (Qcloud_IoT_Shadow*)handle;

	if (IOT_MQTT_IsConnected(shadow->mqtt) == false) {
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
	}

	RequestAck ack_update = ACK_NONE;
	rc = IOT_Shadow_Get(handle, _update_ack_cb, &ack_update, timeout_ms);
	if (rc != QCLOUD_RET_SUCCESS) IOT_FUNC_EXIT_RC(rc);
	
	while (ACK_NONE == ack_update) {
        IOT_Shadow_Yield(handle, 200);
    }

	if (ACK_ACCEPTED == ack_update) {
        rc = QCLOUD_RET_SUCCESS;
    } else if (ACK_TIMEOUT == ack_update) {
        rc = QCLOUD_ERR_SHADOW_GET_TIMEOUT;
    } else if (ACK_REJECTED == ack_update) {
        rc = QCLOUD_ERR_SHADOW_GET_REJECTED;
    }

	IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief Init a shadow JSON string, add the initial field of "{\"state\":{"
 *
 * @param jsonBuffer   JSON string buffer 
 * @param sizeOfBuffer buffer size
 * @return             QCLOUD_RET_SUCCESS for success, or err code for failure
 */
static int IOT_Shadow_JSON_Init(Qcloud_IoT_Shadow *pShadow, char *jsonBuffer, size_t sizeOfBuffer, bool overwrite) {

    if (jsonBuffer == NULL) {
        return QCLOUD_ERR_INVAL;
    }
	
    int32_t rc_of_snprintf = 0;
    if (overwrite) {
    	rc_of_snprintf = HAL_Snprintf(jsonBuffer, sizeOfBuffer, "{\"overwriteUpdate\":true, \"state\":{");
    }
    else {    	
    	rc_of_snprintf = HAL_Snprintf(jsonBuffer, sizeOfBuffer, "{\"state\":{");
    }

    return _check_snprintf_return(rc_of_snprintf, sizeOfBuffer);
}

/**
 * @brief Finish a shadow JSON string, append the tail fields of clientToken and version
 *
 * @param jsonBuffer   JSON string buffer 
 * @param sizeOfBuffer buffer size
 * @return             QCLOUD_RET_SUCCESS for success, or err code for failure
 */
static int IOT_Shadow_JSON_Finalize(Qcloud_IoT_Shadow *pShadow, char *jsonBuffer, size_t sizeOfBuffer) 
{
	int rc;
	size_t remain_size = 0;
	int32_t rc_of_snprintf = 0;

	if (jsonBuffer == NULL) {
		return QCLOUD_ERR_INVAL;
	}

	if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
		return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
	}

	rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer) - 1, remain_size, "}, \"%s\":\"", CLIENT_TOKEN_FIELD);
	rc = _check_snprintf_return(rc_of_snprintf, remain_size);
	if (rc != QCLOUD_RET_SUCCESS) {
		return rc;
	}

	if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
		return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
	}

	rc_of_snprintf = generate_client_token(jsonBuffer + strlen(jsonBuffer), remain_size, &(pShadow->inner_data.token_num));
	rc = _check_snprintf_return(rc_of_snprintf, remain_size);

	if (rc != QCLOUD_RET_SUCCESS) {
		return rc;
	}

	if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
		return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
	}

	rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"}");
	rc = _check_snprintf_return(rc_of_snprintf, remain_size);

	return rc;
}

int IOT_Shadow_JSON_ConstructReport(void *handle, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, ...) 
{
	Qcloud_IoT_Shadow* pshadow = (Qcloud_IoT_Shadow*)handle;
	POINTER_SANITY_CHECK(pshadow, QCLOUD_ERR_INVAL);
	
	int rc = IOT_Shadow_JSON_Init(pshadow, jsonBuffer, sizeOfBuffer, false);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json init failed: %d", rc);
		return rc;
	}

    size_t remain_size = 0;
    int32_t rc_of_snprintf = 0;
    int8_t i;

    if (jsonBuffer == NULL) {
        return QCLOUD_ERR_INVAL;
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"reported\":{");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

	va_list pArgs;
    va_start(pArgs, count);

    for (i = 0; i < count; i++) {
		DeviceProperty *pJsonNode;
        pJsonNode = va_arg(pArgs, DeviceProperty *);
        if (pJsonNode != NULL && pJsonNode->key != NULL) {
            rc = put_json_node(jsonBuffer, remain_size, pJsonNode->key, pJsonNode->data, pJsonNode->type);

            if (rc != QCLOUD_RET_SUCCESS) {
                va_end(pArgs);
                return rc;
            }
        } else {
            va_end(pArgs);
            return QCLOUD_ERR_INVAL;
        }
    }

    va_end(pArgs);
    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }
    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer) - 1, remain_size, "},");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json add report failed: %d", rc);
		return rc;
	}

	rc = IOT_Shadow_JSON_Finalize(pshadow, jsonBuffer, sizeOfBuffer);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json finalize failed: %d", rc);
	}

	return rc;
}


int IOT_Shadow_JSON_ConstructReportArray(void *handle, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, DeviceProperty *pDeviceProperties[]) 
{
	Qcloud_IoT_Shadow* pshadow = (Qcloud_IoT_Shadow*)handle;
	POINTER_SANITY_CHECK(pshadow, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(pDeviceProperties, QCLOUD_ERR_INVAL);
	
	int rc = IOT_Shadow_JSON_Init(pshadow, jsonBuffer, sizeOfBuffer, false);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json init failed: %d", rc);
		return rc;
	}

    size_t remain_size = 0;
    int32_t rc_of_snprintf = 0;
    int8_t i;

    if (jsonBuffer == NULL) {
        return QCLOUD_ERR_INVAL;
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"reported\":{");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

    for (i = 0; i < count; i++) {
		DeviceProperty *pJsonNode = pDeviceProperties[i];
        if (pJsonNode != NULL && pJsonNode->key != NULL) {
            rc = put_json_node(jsonBuffer, remain_size, pJsonNode->key, pJsonNode->data, pJsonNode->type);

            if (rc != QCLOUD_RET_SUCCESS) {
                return rc;
            }
        } else {
            return QCLOUD_ERR_INVAL;
        }
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }
    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer) - 1, remain_size, "},");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json add report failed: %d", rc);
		return rc;
	}

	rc = IOT_Shadow_JSON_Finalize(pshadow, jsonBuffer, sizeOfBuffer);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json finalize failed: %d", rc);
	}

	return rc;
}


int IOT_Shadow_JSON_Construct_OverwriteReport(void *handle, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, ...)
{
	Qcloud_IoT_Shadow* pshadow = (Qcloud_IoT_Shadow*)handle;
	POINTER_SANITY_CHECK(pshadow, QCLOUD_ERR_INVAL);

	int rc = IOT_Shadow_JSON_Init(pshadow, jsonBuffer, sizeOfBuffer, true);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json init failed: %d", rc);
		return rc;
	}

    size_t remain_size = 0;
    int32_t rc_of_snprintf = 0;
    int8_t i;

    if (jsonBuffer == NULL) {
        return QCLOUD_ERR_INVAL;
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"reported\":{");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

	va_list pArgs;
    va_start(pArgs, count);

    for (i = 0; i < count; i++) {
		DeviceProperty *pJsonNode;
        pJsonNode = va_arg(pArgs, DeviceProperty *);
        if (pJsonNode != NULL && pJsonNode->key != NULL) {
            rc = put_json_node(jsonBuffer, remain_size, pJsonNode->key, pJsonNode->data, pJsonNode->type);

            if (rc != QCLOUD_RET_SUCCESS) {
                va_end(pArgs);
                return rc;
            }
        } else {
            va_end(pArgs);
            return QCLOUD_ERR_INVAL;
        }
    }

    va_end(pArgs);
    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }
    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer) - 1, remain_size, "},");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json add report failed: %d", rc);
		return rc;
	}

	rc = IOT_Shadow_JSON_Finalize(pshadow, jsonBuffer, sizeOfBuffer);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json finalize failed: %d", rc);
	}

	return rc;
}

int IOT_Shadow_JSON_ConstructReportAndDesireAllNull(void *handle, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, ...)
{
	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	Qcloud_IoT_Shadow* pshadow = (Qcloud_IoT_Shadow*)handle;
	
	int rc = IOT_Shadow_JSON_Init(pshadow, jsonBuffer, sizeOfBuffer, false);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json init failed: %d", rc);
		return rc;
	}

    size_t remain_size = 0;
    int32_t rc_of_snprintf = 0;
    int8_t i;

    if (jsonBuffer == NULL) {
        return QCLOUD_ERR_INVAL;
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"reported\":{");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

	va_list pArgs;
    va_start(pArgs, count);

    for (i = 0; i < count; i++) {
		DeviceProperty *pJsonNode;
        pJsonNode = va_arg(pArgs, DeviceProperty *);
        if (pJsonNode != NULL && pJsonNode->key != NULL) {
            rc = put_json_node(jsonBuffer, remain_size, pJsonNode->key, pJsonNode->data, pJsonNode->type);

            if (rc != QCLOUD_RET_SUCCESS) {
                va_end(pArgs);
                return rc;
            }
        } else {
            va_end(pArgs);
            return QCLOUD_ERR_INVAL;
        }
    }

    va_end(pArgs);
    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }
    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer) - 1, remain_size, "},");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json add report failed: %d", rc);
		return rc;
	}

	rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"desired\": null ");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

	rc = IOT_Shadow_JSON_Finalize(pshadow, jsonBuffer, sizeOfBuffer);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json finalize failed: %d", rc);
	}

	return rc;
}

int IOT_Shadow_JSON_ConstructDesireAllNull(void *handle, char *jsonBuffer, size_t sizeOfBuffer) 
{
	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	Qcloud_IoT_Shadow* shadow = (Qcloud_IoT_Shadow*)handle;

	int rc = IOT_Shadow_JSON_Init(shadow, jsonBuffer, sizeOfBuffer, false);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json init failed: %d", rc);
		return rc;
	}

	size_t remain_size = 0;
	int32_t rc_of_snprintf = 0;

	if (jsonBuffer == NULL) {
		return QCLOUD_ERR_INVAL;
	}

	if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
		return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
	}

	rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"desired\": null ");
	rc = _check_snprintf_return(rc_of_snprintf, remain_size);

	if (rc != QCLOUD_RET_SUCCESS) {
		return rc;
	}

	rc = IOT_Shadow_JSON_Finalize(shadow, jsonBuffer, sizeOfBuffer);
	return rc;
}

int IOT_Shadow_JSON_ConstructDesirePropNull(void *handle, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, ...) 
{
	POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
	Qcloud_IoT_Shadow* shadow = (Qcloud_IoT_Shadow*)handle;

	int rc = IOT_Shadow_JSON_Init(shadow, jsonBuffer, sizeOfBuffer, false);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json init failed: %d", rc);
		return rc;
	}

    size_t remain_size = 0;
    int32_t rc_of_snprintf = 0;
    int8_t i;

    if (jsonBuffer == NULL) {
        return QCLOUD_ERR_INVAL;
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"desired\":{");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

	va_list pArgs;
    va_start(pArgs, count);

    for (i = 0; i < count; i++) {
		DeviceProperty *pJsonNode;
        pJsonNode = va_arg (pArgs, DeviceProperty *);
        if (pJsonNode != NULL && pJsonNode->key != NULL) {
            rc = put_json_node(jsonBuffer, remain_size, pJsonNode->key, pJsonNode->data, pJsonNode->type);
            if (rc != QCLOUD_RET_SUCCESS) {
				va_end(pArgs);
                return rc;
            }
        } else {
			va_end(pArgs);
            return QCLOUD_ERR_INVAL;
        }
    }

    va_end(pArgs);
    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }
    // strlen(jsonBuffer) - 1 to remove last comma
    rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer) - 1, remain_size, "},");
    rc = _check_snprintf_return(rc_of_snprintf, remain_size);

	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("shadow json add desired failed: %d", rc);
		return rc;
	}

	rc = IOT_Shadow_JSON_Finalize(shadow, jsonBuffer, sizeOfBuffer);
	return rc;
}

#ifdef __cplusplus
}
#endif

