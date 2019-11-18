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

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "shadow_client.h"

#include "utils_param_check.h"
#include "qcloud_iot_import.h"

#include "shadow_client_json.h"

#include "utils_list.h"



/**
 * @brief type for document request
 */
typedef struct {
    char                   client_token[MAX_SIZE_OF_CLIENT_TOKEN];          // clientToken
    Method                 method;                                          // method type

    void                   *user_context;                                   // user context
    Timer                  timer;                                           // timer for timeout

    OnRequestCallback      callback;                                        // request response callback
} Request;



static char cloud_rcv_buf[CLOUD_IOT_JSON_RX_BUF_LEN];

bool discard_old_delta_flag = true;

typedef void (*TraverseHandle)(Qcloud_IoT_Shadow *pShadow, ListNode **node, List *list, const char *pClientToken, const char *pType);

static void _on_operation_result_handler(void *pClient, MQTTMessage *message, void *pUserdata);

static void _handle_delta(Qcloud_IoT_Shadow *pShadow, char* delta_str);

static int _set_shadow_json_type(char *pJsonDoc, size_t sizeOfBuffer, Method method);

static int _publish_operation_to_cloud(Qcloud_IoT_Shadow *pShadow, Method method, char *pJsonDoc);

static int _add_request_to_list(Qcloud_IoT_Shadow *pShadow, const char *pClientToken, RequestParams *pParams);

static int _unsubscribe_operation_result_to_cloud(void *pClient);

static void _traverse_list(Qcloud_IoT_Shadow *pShadow, List *list, const char *pClientToken, const char *pType, TraverseHandle traverseHandle);

static void _handle_request_callback(Qcloud_IoT_Shadow *pShadow, ListNode **node, List *list, const char *pClientToken, const char *pType);

static void _handle_expired_request_callback(Qcloud_IoT_Shadow *pShadow, ListNode **node, List *list, const char *pClientToken, const char *pType);

int qcloud_iot_shadow_init(Qcloud_IoT_Shadow *pShadow) {
	IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pShadow, QCLOUD_ERR_INVAL);

    pShadow->mutex = HAL_MutexCreate();
    if (pShadow->mutex == NULL)
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);

    pShadow->inner_data.property_handle_list = list_new();
    if (pShadow->inner_data.property_handle_list)
    {
        pShadow->inner_data.property_handle_list->free = HAL_Free;
    }
    else {
    	Log_e("no memory to allocate property_handle_list");
    	IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

	pShadow->inner_data.request_list = list_new();
	if (pShadow->inner_data.request_list)
	{
		pShadow->inner_data.request_list->free = HAL_Free;
	} else {
		Log_e("no memory to allocate request_list");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}

	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

void qcloud_iot_shadow_reset(void *pClient) {
    POINTER_SANITY_CHECK_RTN(pClient);

    Qcloud_IoT_Shadow *shadow_client = (Qcloud_IoT_Shadow *)pClient;
    if (shadow_client->inner_data.property_handle_list) {
        list_destroy(shadow_client->inner_data.property_handle_list);
    }

    _unsubscribe_operation_result_to_cloud(shadow_client->mqtt);

    if (shadow_client->inner_data.request_list)
    {
        list_destroy(shadow_client->inner_data.request_list);
    }
}

void handle_expired_request(Qcloud_IoT_Shadow *pShadow) {
    IOT_FUNC_ENTRY;

    _traverse_list(pShadow, pShadow->inner_data.request_list, NULL, NULL, _handle_expired_request_callback);

    IOT_FUNC_EXIT;
}

int do_shadow_request(Qcloud_IoT_Shadow *pShadow, RequestParams *pParams, char *pJsonDoc, size_t sizeOfBuffer)
{
    IOT_FUNC_ENTRY;
    int rc = QCLOUD_RET_SUCCESS;

    POINTER_SANITY_CHECK(pShadow, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pJsonDoc, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pParams, QCLOUD_ERR_INVAL);

    char* client_token = NULL;

    if (!parse_client_token(pJsonDoc, &client_token)) {
        Log_e("fail to parse client token!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    if (rc != QCLOUD_RET_SUCCESS)
        IOT_FUNC_EXIT_RC(rc);

    rc = _set_shadow_json_type(pJsonDoc, sizeOfBuffer, pParams->method);
    if (rc != QCLOUD_RET_SUCCESS)
        IOT_FUNC_EXIT_RC(rc);

    if (rc == QCLOUD_RET_SUCCESS) {
        rc = _publish_operation_to_cloud(pShadow, pParams->method, pJsonDoc);
    }

    if (rc >= 0) {
        rc = _add_request_to_list(pShadow, client_token, pParams);
    }

    HAL_Free(client_token);

    IOT_FUNC_EXIT_RC(rc);
}

int subscribe_operation_result_to_cloud(Qcloud_IoT_Shadow *pShadow)
{
    IOT_FUNC_ENTRY;

    int rc;
	int size;
	
    if (pShadow->inner_data.result_topic == NULL) {
        char *operation_result_topic = (char *)HAL_Malloc(MAX_SIZE_OF_CLOUD_TOPIC * sizeof(char));
        if (operation_result_topic == NULL) IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);

        memset(operation_result_topic, 0x0, MAX_SIZE_OF_CLOUD_TOPIC);
		if(eTEMPLATE == pShadow->shadow_type){
			size = HAL_Snprintf(operation_result_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$template/operation/result/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);	
		}else{
			size = HAL_Snprintf(operation_result_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$shadow/operation/result/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);	
		}
		if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
        {
            Log_e("buf size < topic length!");
            HAL_Free(operation_result_topic);
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
        }
        pShadow->inner_data.result_topic = operation_result_topic;
    }

    SubscribeParams subscribe_params = DEFAULT_SUB_PARAMS;
    subscribe_params.on_message_handler = _on_operation_result_handler;
    subscribe_params.qos = QOS0;

    rc = IOT_MQTT_Subscribe(pShadow->mqtt, pShadow->inner_data.result_topic, &subscribe_params);
    if (rc < 0) {
        Log_e("subscribe topic: %s failed: %d.", pShadow->inner_data.result_topic, rc);
    }

    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief publish operation to server
 *
 * @param pClient                   Qcloud_IoT_Client
 * @param method                    method type
 * @param pJsonDoc                  JSON to publish
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
static int _publish_operation_to_cloud(Qcloud_IoT_Shadow *pShadow, Method method, char *pJsonDoc)
{
    IOT_FUNC_ENTRY;
    int rc = QCLOUD_RET_SUCCESS;

    char topic[MAX_SIZE_OF_CLOUD_TOPIC] = {0};
	int size;
		
	if(eTEMPLATE == pShadow->shadow_type){
		size = HAL_Snprintf(topic, MAX_SIZE_OF_CLOUD_TOPIC, "$template/operation/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);	
	}else{
		size = HAL_Snprintf(topic, MAX_SIZE_OF_CLOUD_TOPIC, "$shadow/operation/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);	
	}

	if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
    {
        Log_e("buf size < topic length!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    PublishParams pubParams = DEFAULT_PUB_PARAMS;
    pubParams.qos = QOS0;
    pubParams.payload_len = strlen(pJsonDoc);
    pubParams.payload = (char *) pJsonDoc;

    rc = IOT_MQTT_Publish(pShadow->mqtt, topic, &pubParams);

    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief callback when msg of operation result arrives
 */
static void _on_operation_result_handler(void *pClient, MQTTMessage *message, void *pUserdata)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK_RTN(pClient);
    POINTER_SANITY_CHECK_RTN(message);

    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;
    Qcloud_IoT_Shadow *shadow_client = (Qcloud_IoT_Shadow*)mqtt_client->event_handle.context;

    const char *topic = message->ptopic;
    size_t topic_len = message->topic_len;
    if (NULL == topic || topic_len <= 0) {
        IOT_FUNC_EXIT;
    }

    char *client_token = NULL;
    char *type_str = NULL;

    if (message->payload_len > CLOUD_IOT_JSON_RX_BUF_LEN) {
        Log_e("The length of the received message exceeds the specified length!");
        goto End;
    }

    int cloud_rcv_len = Min(CLOUD_IOT_JSON_RX_BUF_LEN - 1, message->payload_len);
    memcpy(cloud_rcv_buf, message->payload, cloud_rcv_len + 1);
    cloud_rcv_buf[cloud_rcv_len] = '\0';    // jsmn_parse relies on a string

    if (!parse_shadow_operation_type(cloud_rcv_buf, &type_str))
    {
        Log_e("Fail to parse type!");
        goto End;
    }
	Log_d("type:%s", type_str);
    // non-delta msg push is triggered by device side, parse client token first
    if (strcmp(type_str, OPERATION_DELTA) && !parse_client_token(cloud_rcv_buf, &client_token)) {
		Log_e("Fail to parse client token! Json=%s", cloud_rcv_buf);
		goto End;
    }

    if (!strcmp(type_str, OPERATION_DELTA)) {
        HAL_MutexLock(shadow_client->mutex);
        char* delta_str = NULL;
        if (parse_shadow_operation_delta(cloud_rcv_buf, &delta_str)) {
			Log_d("dlta:%s", delta_str);
        	_handle_delta(shadow_client, delta_str);
        	HAL_Free(delta_str);
        }

        HAL_MutexUnlock(shadow_client->mutex);
        goto End;
    }
    
    if (shadow_client != NULL)
        _traverse_list(shadow_client, shadow_client->inner_data.request_list, client_token, type_str, _handle_request_callback);

End:
    HAL_Free(type_str);
    HAL_Free(client_token);

    IOT_FUNC_EXIT;
}


static void _handle_delta(Qcloud_IoT_Shadow *pShadow, char* delta_str)
{
    IOT_FUNC_ENTRY;
    if (pShadow->inner_data.property_handle_list->len) {
        ListIterator *iter;
        ListNode *node = NULL;
        PropertyHandler *property_handle = NULL;

        if (NULL == (iter = list_iterator_new(pShadow->inner_data.property_handle_list, LIST_TAIL))) {
            HAL_MutexUnlock(pShadow->mutex);
            IOT_FUNC_EXIT;
        }

        for (;;) {
            node = list_iterator_next(iter);
            if (NULL == node) {
                break;
            }

            property_handle = (PropertyHandler *)(node->val);
            if (NULL == property_handle) {
                Log_e("node's value is invalid!");
                continue;
            }

            if (property_handle->property != NULL) {

                if (update_value_if_key_match(delta_str, property_handle->property))
                {
                    if (property_handle->callback != NULL)
                    {
                        property_handle->callback(pShadow, delta_str, strlen(delta_str), property_handle->property);
                    }
                    node = NULL;
                }
            }

        }

        list_iterator_destroy(iter);
    }

    IOT_FUNC_EXIT;
}

static void _insert(char *str, char *pch, int pos) {
    int len = strlen(str);
    int nlen = strlen(pch);
    int i;
    for (i = len - 1; i >= pos; --i) {
        *(str + i + nlen) = *(str + i);
    }

    int n;
    for (n = 0; n < nlen; n++)
        *(str + pos + n) = *pch++;
    *(str + len + nlen) = 0;
}


static int _set_shadow_json_type(char *pJsonDoc, size_t sizeOfBuffer, Method method)
{
    IOT_FUNC_ENTRY;

    int rc = QCLOUD_RET_SUCCESS;

    POINTER_SANITY_CHECK(pJsonDoc, QCLOUD_ERR_INVAL);
    char *type_str = NULL;
    switch (method) {
      case GET:
        type_str = OPERATION_GET;
        break;
      case UPDATE:
        type_str = OPERATION_UPDATE;
        break;
      default:
        Log_e("unexpected method!");
        rc = QCLOUD_ERR_INVAL;
        break;
    }
    if (rc != QCLOUD_RET_SUCCESS)
        IOT_FUNC_EXIT_RC(rc);

    size_t json_len = strlen(pJsonDoc);
    size_t remain_size = sizeOfBuffer - json_len;

    char json_node_str[64] = {0};
    HAL_Snprintf(json_node_str, 64, "\"type\":\"%s\", ", type_str);

    size_t json_node_len = strlen(json_node_str);
    if (json_node_len >= remain_size - 1) {
        rc = QCLOUD_ERR_INVAL;
    } else {
        _insert(pJsonDoc, json_node_str, 1);
    }

    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief unsubsribe topic: $shadow/operation/result/{ProductId}/{DeviceName}
 */
static int _unsubscribe_operation_result_to_cloud(void* pClient)
{
    IOT_FUNC_ENTRY;
    int rc = QCLOUD_RET_SUCCESS;

    char operation_result_topic[MAX_SIZE_OF_CLOUD_TOPIC] = {0};
    int size = HAL_Snprintf(operation_result_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$shadow/operation/result/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);

    if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
    {
        Log_e("buf size < topic length!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    IOT_MQTT_Unsubscribe(pClient, operation_result_topic);
    if (rc < 0) {
        Log_e("unsubscribe topic: %s failed: %d.", operation_result_topic, rc);
    }

    IOT_FUNC_EXIT_RC(rc);
}


static int _add_request_to_list(Qcloud_IoT_Shadow *pShadow, const char *pClientToken, RequestParams *pParams)
{
    IOT_FUNC_ENTRY;

    HAL_MutexLock(pShadow->mutex);
    if (pShadow->inner_data.request_list->len >= MAX_APPENDING_REQUEST_AT_ANY_GIVEN_TIME)
    {
        HAL_MutexUnlock(pShadow->mutex);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MAX_APPENDING_REQUEST);
    }

    Request *request = (Request *)HAL_Malloc(sizeof(Request));
    if (NULL == request) {
        HAL_MutexUnlock(pShadow->mutex);
        Log_e("run memory malloc is error!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    request->callback = pParams->request_callback;
    strncpy(request->client_token, pClientToken, MAX_SIZE_OF_CLIENT_TOKEN);

    request->user_context = pParams->user_context;
    request->method = pParams->method;

    InitTimer(&(request->timer));
    countdown(&(request->timer), pParams->timeout_sec);

    ListNode *node = list_node_new(request);
    if (NULL == node) {
        HAL_MutexUnlock(pShadow->mutex);
        Log_e("run list_node_new is error!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    list_rpush(pShadow->inner_data.request_list, node);

    HAL_MutexUnlock(pShadow->mutex);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief iterator list and call traverseHandle for each node
 */
static void _traverse_list(Qcloud_IoT_Shadow *pShadow, List *list, const char *pClientToken, const char *pType, TraverseHandle traverseHandle)
{
    IOT_FUNC_ENTRY;

    HAL_MutexLock(pShadow->mutex);

    if (list->len) {
        ListIterator *iter;
        ListNode *node = NULL;

        if (NULL == (iter = list_iterator_new(list, LIST_TAIL))) {
            HAL_MutexUnlock(pShadow->mutex);
            IOT_FUNC_EXIT;
        }

        for (;;) {
            node = list_iterator_next(iter);
            if (NULL == node) {
                break;
            }

            if (NULL == node->val) {
                Log_e("node's value is invalid!");
                continue;
            }

            traverseHandle(pShadow, &node, list, pClientToken, pType);
        }

        list_iterator_destroy(iter);
    }
    HAL_MutexUnlock(pShadow->mutex);

    IOT_FUNC_EXIT;
}


static void _handle_request_callback(Qcloud_IoT_Shadow *pShadow, ListNode **node, List *list, const char *pClientToken, const char *pType)
{
    IOT_FUNC_ENTRY;

    Request *request = (Request *)(*node)->val;
    if (NULL == request)
        IOT_FUNC_EXIT;

    if (strcmp(request->client_token, pClientToken) == 0)
    {
        RequestAck status = ACK_NONE;

        // result field in payload tell us if operation success or not
        // result = 0 for success, result != 0 for fail
        int16_t result_code = 0;
        
        bool parse_success = parse_shadow_operation_result_code(cloud_rcv_buf, &result_code);
        if (parse_success) {
        	if (result_code == 0) {
				status = ACK_ACCEPTED;
			} else {
				status = ACK_REJECTED;
			}

			if ((strcmp(pType, "get") == 0 && status == ACK_ACCEPTED) ||
				(strcmp(pType, "update") && status == ACK_REJECTED))
			{
				char* delta_str = NULL;
				if (parse_shadow_operation_get(cloud_rcv_buf, &delta_str)) {
					_handle_delta(pShadow, delta_str);
					HAL_Free(delta_str);
				}
			}

			if (request->callback != NULL) {
				request->callback(pShadow, request->method, status, cloud_rcv_buf, request->user_context);
			}
        }
        else {
        	Log_e("parse shadow operation result code failed.");
        }
        
        list_remove(list, *node);
        *node = NULL;

    }

    IOT_FUNC_EXIT;
}


static void _handle_expired_request_callback(Qcloud_IoT_Shadow *pShadow, ListNode **node, List *list, const char *pClientToken, const char *pType)
{
    IOT_FUNC_ENTRY;

    Request *request = (Request *)(*node)->val;
    if (NULL == request)
        IOT_FUNC_EXIT;

    if (expired(&request->timer))
    {
        if (request->callback != NULL) {
            request->callback(pShadow, request->method, ACK_TIMEOUT, cloud_rcv_buf, request->user_context);
        }

        list_remove(list, *node);
        *node = NULL;
    }

    IOT_FUNC_EXIT;
}

#ifdef __cplusplus
}
#endif
