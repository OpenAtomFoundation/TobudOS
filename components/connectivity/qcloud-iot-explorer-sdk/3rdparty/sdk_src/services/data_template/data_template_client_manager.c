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

#include "qcloud_iot_import.h"
#include "utils_param_check.h"
#include "utils_list.h"

#include "data_template_client.h"
#include "data_template_client_json.h"


typedef void (*TraverseTemplateHandle)(Qcloud_IoT_Template *pTemplate, ListNode **node, List *list, const char *pClientToken, const char *pType);

static char sg_template_cloud_rcv_buf[CLOUD_IOT_JSON_RX_BUF_LEN];
static char sg_template_clientToken[MAX_SIZE_OF_CLIENT_TOKEN];

/**
* @brief unsubsribe topic:  $thing/down/property/{ProductId}/{DeviceName}
*/
static int _unsubscribe_template_downstream_topic(void* pClient)
{
    IOT_FUNC_ENTRY;
    int rc = QCLOUD_RET_SUCCESS;

    char downstream_topic[MAX_SIZE_OF_CLOUD_TOPIC] = {0};
    int size = HAL_Snprintf(downstream_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/down/property/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);

    if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
    {
        Log_e("buf size < topic length!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    IOT_MQTT_Unsubscribe(pClient, downstream_topic);
    if (rc < 0) {
        Log_e("unsubscribe topic: %s failed: %d.", downstream_topic, rc);
    }

    IOT_FUNC_EXIT_RC(rc);
}


/**
* @brief add request to data_template request wait for reply list
*/ 
static int _add_request_to_template_list(Qcloud_IoT_Template *pTemplate, const char *pClientToken, RequestParams *pParams)
{
    IOT_FUNC_ENTRY;

    HAL_MutexLock(pTemplate->mutex);
    if (pTemplate->inner_data.reply_list->len >= MAX_APPENDING_REQUEST_AT_ANY_GIVEN_TIME)
    {
        HAL_MutexUnlock(pTemplate->mutex);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MAX_APPENDING_REQUEST);
    }

    Request *request = (Request *)HAL_Malloc(sizeof(Request));
    if (NULL == request) {
        HAL_MutexUnlock(pTemplate->mutex);
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
        HAL_MutexUnlock(pTemplate->mutex);
        Log_e("run list_node_new is error!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    list_rpush(pTemplate->inner_data.reply_list, node);

    HAL_MutexUnlock(pTemplate->mutex);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief publish operation to server
 *
 * @param pClient                   handle to data_template client
 * @param method                    method type
 * @param pJsonDoc                  JSON to publish
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */ 
int _publish_to_template_upstream_topic(Qcloud_IoT_Template *pTemplate, Method method, char *pJsonDoc)
{
    IOT_FUNC_ENTRY;
    int rc = QCLOUD_RET_SUCCESS;

    char topic[MAX_SIZE_OF_CLOUD_TOPIC] = {0};
	int size;
		

	size = HAL_Snprintf(topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/up/property/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);	

	if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
    {
        Log_e("buf size < topic length!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    PublishParams pubParams = DEFAULT_PUB_PARAMS;
    pubParams.qos = QOS0;
    pubParams.payload_len = strlen(pJsonDoc);
    pubParams.payload = (char *) pJsonDoc;

    rc = IOT_MQTT_Publish(pTemplate->mqtt, topic, &pubParams);

    IOT_FUNC_EXIT_RC(rc);
}


/**
 * @brief fill method json filed with the value of RequestParams and Method
 */
static int _set_template_json_type(char *pJsonDoc, size_t sizeOfBuffer, Method method)
{
    IOT_FUNC_ENTRY;

    int rc = QCLOUD_RET_SUCCESS;

    POINTER_SANITY_CHECK(pJsonDoc, QCLOUD_ERR_INVAL);
    char *method_str = NULL;
    switch (method) {
      case GET:
        method_str = GET_STATUS;
        break;
      case REPORT:
        method_str = REPORT_CMD;
        break;
      case RINFO:
        method_str = INFO_CMD;
		break;
      case REPLY:
        method_str = CONTROL_CMD_REPLY;
        break;
	  case CLEAR:
        method_str = CLEAR_CONTROL;
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
    HAL_Snprintf(json_node_str, 64, "\"method\":\"%s\", ", method_str);

    size_t json_node_len = strlen(json_node_str);
    if (json_node_len >= remain_size - 1) {
        rc = QCLOUD_ERR_INVAL;
    } else {
        insert_str(pJsonDoc, json_node_str, 1);
    }

    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief iterator list and call traverseHandle for each node
 */
static void _traverse_template_list(Qcloud_IoT_Template *pTemplate, List *list, const char *pClientToken, const char *pType, TraverseTemplateHandle traverseHandle)
{
    IOT_FUNC_ENTRY;

    HAL_MutexLock(pTemplate->mutex);

    if (list->len) {
        ListIterator *iter;
        ListNode *node = NULL;

        if (NULL == (iter = list_iterator_new(list, LIST_TAIL))) {
            HAL_MutexUnlock(pTemplate->mutex);
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

            traverseHandle(pTemplate, &node, list, pClientToken, pType);
        }

        list_iterator_destroy(iter);
    }
    HAL_MutexUnlock(pTemplate->mutex);

    IOT_FUNC_EXIT;
}

/**
 * @brief handle the timeout request wait for reply
 */
static void _handle_template_expired_reply_callback(Qcloud_IoT_Template *pTemplate, ListNode **node, List *list, const char *pClientToken, const char *pType)
{
    IOT_FUNC_ENTRY;

    Request *request = (Request *)(*node)->val;
    if (NULL == request)
        IOT_FUNC_EXIT;

    if (expired(&request->timer))
    {
        if (request->callback != NULL) {
            request->callback(pTemplate, request->method, ACK_TIMEOUT, sg_template_cloud_rcv_buf, request);
        }

        list_remove(list, *node);
        *node = NULL;
    }

    IOT_FUNC_EXIT;
}

static void _set_control_clientToken(const char *pClientToken)
{
	memset(sg_template_clientToken, '\0', MAX_SIZE_OF_CLIENT_TOKEN);
	strncpy(sg_template_clientToken, pClientToken, MAX_SIZE_OF_CLIENT_TOKEN);
}

char * get_control_clientToken(void)
{
	return sg_template_clientToken;
}

void qcloud_iot_template_reset(void *pClient)
{
    POINTER_SANITY_CHECK_RTN(pClient);

    Qcloud_IoT_Template *template_client = (Qcloud_IoT_Template *)pClient;
    if (template_client->inner_data.property_handle_list) {
        list_destroy(template_client->inner_data.property_handle_list);
    }

    _unsubscribe_template_downstream_topic(template_client->mqtt);

    if (template_client->inner_data.reply_list)
    {
        list_destroy(template_client->inner_data.reply_list);
    }

	if (template_client->inner_data.event_list)
    {
        list_destroy(template_client->inner_data.event_list);
    }
}

int qcloud_iot_template_init(Qcloud_IoT_Template *pTemplate) 
{
	IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pTemplate, QCLOUD_ERR_INVAL);

    pTemplate->mutex = HAL_MutexCreate();
    if (pTemplate->mutex == NULL)
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);

    pTemplate->inner_data.property_handle_list = list_new();
    if (pTemplate->inner_data.property_handle_list)
    {
        pTemplate->inner_data.property_handle_list->free = HAL_Free;
    }
    else {
    	Log_e("no memory to allocate property_handle_list");
    	IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

	pTemplate->inner_data.reply_list = list_new();
	if (pTemplate->inner_data.reply_list)
	{
		pTemplate->inner_data.reply_list->free = HAL_Free;
	} else {
		Log_e("no memory to allocate reply_list");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}
		
	pTemplate->inner_data.event_list = list_new();
	if (pTemplate->inner_data.event_list)
	{
		pTemplate->inner_data.event_list->free = HAL_Free;
	} else {
		Log_e("no memory to allocate event_list");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}

	pTemplate->inner_data.action_handle_list = list_new();
	if (pTemplate->inner_data.action_handle_list)
	{
		pTemplate->inner_data.action_handle_list->free = HAL_Free;
	} else {
		Log_e("no memory to allocate action_handle_list");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}

	IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

void handle_template_expired_reply(Qcloud_IoT_Template *pTemplate) 
{
    IOT_FUNC_ENTRY;

    _traverse_template_list(pTemplate, pTemplate->inner_data.reply_list, NULL, NULL, _handle_template_expired_reply_callback);

    IOT_FUNC_EXIT;
}

int send_template_request(Qcloud_IoT_Template *pTemplate, RequestParams *pParams, char *pJsonDoc, size_t sizeOfBuffer)
{
    IOT_FUNC_ENTRY;
    int rc = QCLOUD_RET_SUCCESS;

    POINTER_SANITY_CHECK(pTemplate, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pJsonDoc, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pParams, QCLOUD_ERR_INVAL);

    char* client_token = NULL;

    // parse clientToken in pJsonDoc, return err if parse failed
    if (!parse_client_token(pJsonDoc, &client_token)) {
        Log_e("fail to parse client token!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    if (rc != QCLOUD_RET_SUCCESS)
        IOT_FUNC_EXIT_RC(rc);

    rc = _set_template_json_type(pJsonDoc, sizeOfBuffer, pParams->method);
    if (rc != QCLOUD_RET_SUCCESS)
        IOT_FUNC_EXIT_RC(rc);

    if (rc == QCLOUD_RET_SUCCESS) {
        rc = _publish_to_template_upstream_topic(pTemplate, pParams->method, pJsonDoc);
    }

    if ((rc == QCLOUD_RET_SUCCESS) && (NULL != pParams->request_callback)) {
        rc = _add_request_to_template_list(pTemplate, client_token, pParams);
    }

    HAL_Free(client_token);

    IOT_FUNC_EXIT_RC(rc);
}

static void _handle_control(Qcloud_IoT_Template *pTemplate, char* control_str)
{
    IOT_FUNC_ENTRY;
    if (pTemplate->inner_data.property_handle_list->len) {
        ListIterator *iter;
        ListNode *node = NULL;
        PropertyHandler *property_handle = NULL;

        if (NULL == (iter = list_iterator_new(pTemplate->inner_data.property_handle_list, LIST_TAIL))) {
            HAL_MutexUnlock(pTemplate->mutex);
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
                if (update_value_if_key_match(control_str, property_handle->property))
                {
                	
                    if (property_handle->callback != NULL)
                    {
                        property_handle->callback(pTemplate, control_str, strlen(control_str), property_handle->property);
                    }
                    node = NULL;
                }
            }

        }

        list_iterator_destroy(iter);
    }

    IOT_FUNC_EXIT;
}



static void _handle_template_reply_callback(Qcloud_IoT_Template *pTemplate, ListNode **node, List *list, const char *pClientToken, const char *pType)
{
    IOT_FUNC_ENTRY;

    Request *request = (Request *)(*node)->val;
    if (NULL == request)
        IOT_FUNC_EXIT;

    if (strcmp(request->client_token, pClientToken) == 0)
    {
        ReplyAck status = ACK_NONE;

        // check operation success or not according to code field of reply message
        int32_t reply_code = 0;
        
        bool parse_success = parse_code_return(sg_template_cloud_rcv_buf, &reply_code);
        if (parse_success) {
        	if (reply_code == 0) {
				status = ACK_ACCEPTED;
			} else {
				status = ACK_REJECTED;
			}

			if (strcmp(pType, GET_STATUS_REPLY) == 0 && status == ACK_ACCEPTED)
			{
				char* control_str = NULL;
				if (parse_template_get_control(sg_template_cloud_rcv_buf, &control_str)) {
					Log_d("control data from get_status_reply");
					_set_control_clientToken(pClientToken);
					_handle_control(pTemplate, control_str); 
					HAL_Free(control_str);
					*((ReplyAck *)request->user_context) = ACK_ACCEPTED; //prepare for clear_control
				}
			}
		
			
			if (request->callback != NULL) {
				request->callback(pTemplate, request->method, status, sg_template_cloud_rcv_buf, request);
			}
        }
        else {
        	Log_e("parse template operation result code failed.");
        }
        
        list_remove(list, *node);
        *node = NULL;

    }

    IOT_FUNC_EXIT;
}

static void _on_template_downstream_topic_handler(void *pClient, MQTTMessage *message, void *pUserdata)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK_RTN(pClient);
    POINTER_SANITY_CHECK_RTN(message);

    Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;
    Qcloud_IoT_Template *template_client = (Qcloud_IoT_Template*)mqtt_client->event_handle.context;

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

    int cloud_rcv_len = min(CLOUD_IOT_JSON_RX_BUF_LEN - 1, message->payload_len);
	memset(sg_template_cloud_rcv_buf, 0, sizeof(sg_template_cloud_rcv_buf));
    memcpy(sg_template_cloud_rcv_buf, message->payload, cloud_rcv_len + 1);
    sg_template_cloud_rcv_buf[cloud_rcv_len] = '\0';    // jsmn_parse relies on a string
	//Log_d("recv:%s", sg_template_cloud_rcv_buf);

	//parse the message type from topic $thing/down/property 
    if (!parse_template_method_type(sg_template_cloud_rcv_buf, &type_str))
    {
        Log_e("Fail to parse method!");
        goto End;
    }

    if (!parse_client_token(sg_template_cloud_rcv_buf, &client_token)) {
		Log_e("Fail to parse client token! Json=%s", sg_template_cloud_rcv_buf);
		goto End;
    }
	
	//handle control message
    if (!strcmp(type_str, CONTROL_CMD)) {
	    HAL_MutexLock(template_client->mutex);
	    char* control_str = NULL;
	    if (parse_template_cmd_control(sg_template_cloud_rcv_buf, &control_str)) {
			Log_d("control_str:%s", control_str);
			_set_control_clientToken(client_token);
	    	_handle_control(template_client, control_str);
	    	HAL_Free(control_str);
	    }

	    HAL_MutexUnlock(template_client->mutex);
	    goto End;
	}
   	
    if (template_client != NULL)
        _traverse_template_list(template_client, template_client->inner_data.reply_list, client_token, type_str, _handle_template_reply_callback);

End:
    HAL_Free(type_str);
    HAL_Free(client_token);

    IOT_FUNC_EXIT;
}

int subscribe_template_downstream_topic(Qcloud_IoT_Template *pTemplate)
{
    IOT_FUNC_ENTRY;

    int rc;
	int size;
	
    if (pTemplate->inner_data.downstream_topic == NULL) {
        char *downstream_topic = (char *)HAL_Malloc(MAX_SIZE_OF_CLOUD_TOPIC * sizeof(char));
        if (downstream_topic == NULL) IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);

        memset(downstream_topic, 0x0, MAX_SIZE_OF_CLOUD_TOPIC);
		size = HAL_Snprintf(downstream_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/down/property/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);	
		if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
        {
            Log_e("buf size < topic length!");
            HAL_Free(downstream_topic);
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
        }
        pTemplate->inner_data.downstream_topic = downstream_topic;
    }

    SubscribeParams subscribe_params = DEFAULT_SUB_PARAMS;
    subscribe_params.on_message_handler = _on_template_downstream_topic_handler;
    subscribe_params.qos = QOS0;

    rc = IOT_MQTT_Subscribe(pTemplate->mqtt, pTemplate->inner_data.downstream_topic, &subscribe_params);
    if (rc < 0) {
        Log_e("subscribe topic: %s failed: %d.", pTemplate->inner_data.downstream_topic, rc);
    }

    IOT_FUNC_EXIT_RC(rc);
}


#ifdef __cplusplus
}
#endif
