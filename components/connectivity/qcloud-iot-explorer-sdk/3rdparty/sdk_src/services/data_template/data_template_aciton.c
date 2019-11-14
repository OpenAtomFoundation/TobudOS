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

#include "config.h"

#if defined(ACTION_ENABLED)
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "utils_param_check.h"
#include "lite-utils.h"
#include "data_template_client.h"
#include "data_template_action.h"
#include "data_template_client_json.h"
#include "qcloud_iot_export_data_template.h"

//Action Subscribe 
static int _parse_action_input(DeviceAction *pAction, char *pInput) 
{	
	int i;
	char *temp;
	DeviceProperty *pActionInput = pAction->pInput;

	//check and copy
	for (i = 0; i < pAction->input_num; i++) {		
		if (JSTRING == pActionInput[i].type) {
			pActionInput[i].data = LITE_json_value_of(pActionInput[i].key, pInput);	
			if(NULL == pActionInput[i].data) {
				Log_e("action input data [%s] not found!", pActionInput[i].key);
				return -1;
			}
		} else {
			temp = LITE_json_value_of(pActionInput[i].key, pInput);
			if(NULL == temp) {
				Log_e("action input data [%s] not found!", pActionInput[i].key);
				return -1;
			}
			if(JINT32 == pActionInput[i].type) {
				if (sscanf(temp, "%" SCNi32, (int32_t *)pActionInput[i].data) != 1) {
					HAL_Free(temp);
					Log_e("parse code failed, errCode: %d", QCLOUD_ERR_JSON_PARSE);
					return -1;
				}
			} else if( JFLOAT == pActionInput[i].type) {
				if (sscanf(temp, "%f", (float *)pActionInput[i].data) != 1) {
					HAL_Free(temp);
					Log_e("parse code failed, errCode: %d", QCLOUD_ERR_JSON_PARSE);
					return -1;
				}
			} else if( JUINT32 == pActionInput[i].type) {
				if (sscanf(temp, "%" SCNu32, (uint32_t *) pActionInput[i].data) != 1) {
					HAL_Free(temp);
					Log_e("parse code failed, errCode: %d", QCLOUD_ERR_JSON_PARSE);
					return -1;
				}
			}
			HAL_Free(temp);
		}
	}	

	return 0;
}

static void _handle_aciton(Qcloud_IoT_Template *pTemplate, List *list, const char *pClientToken, const char *pActionId, uint32_t timestamp, char *pInput)
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
			
			ActionHandler *pActionHandle =  (ActionHandler *)node->val;						

			// check action id and call callback
			if(0 == strcmp(pActionId,((DeviceAction*)pActionHandle->action)->pActionId)){						
				if(NULL != pActionHandle->callback) {
					if(!_parse_action_input(pActionHandle->action, pInput)) {
						((DeviceAction*)pActionHandle->action)->timestamp = timestamp;
						pActionHandle->callback(pTemplate, pClientToken, pActionHandle->action);
					}
				}						
			}
        }
        list_iterator_destroy(iter);
    }
    HAL_MutexUnlock(pTemplate->mutex);
    IOT_FUNC_EXIT;
}

static void _on_action_handle_callback(void *pClient, MQTTMessage *message, void *pUserData) 
{
	POINTER_SANITY_CHECK_RTN(message);
	Qcloud_IoT_Client *mqtt_client = (Qcloud_IoT_Client *)pClient;
	Qcloud_IoT_Template *template_client = (Qcloud_IoT_Template*)mqtt_client->event_handle.context;

 	char *type_str = NULL;
    char* client_token = NULL;
    char* action_id = NULL;
	char *pInput = NULL;
	int timestamp = 0;
	   
	Log_d("recv:%.*s",(int) message->payload_len, (char *) message->payload);

	// prase_method
    if (!parse_template_method_type((char *) message->payload, &type_str))
    {
        Log_e("Fail to parse method!");
        goto EXIT;
    }

    if (strcmp(type_str, CALL_ACTION)) {
		goto EXIT;
	}

	// prase client Token
    if (!parse_client_token((char *) message->payload, &client_token)) {
        Log_e("fail to parse client token!");
        goto EXIT;
    }

	// prase action ID
	if (!parse_action_id((char *) message->payload, &action_id)) {
        Log_e("fail to parse action id!");
        goto EXIT;
    }

	// prase timestamp
	if (!parse_time_stamp((char *) message->payload, &timestamp)) {
        Log_e("fail to parse timestamp!");
        goto EXIT;
    }

	// prase action input
	if (!parse_action_input((char *) message->payload, &pInput)) {
        Log_e("fail to parse action input!");
        goto EXIT;
    }	

	//find action ID in register list and call handle
	if (template_client != NULL)
        _handle_aciton(template_client, template_client->inner_data.action_handle_list, client_token, action_id, timestamp, pInput);

EXIT:
	HAL_Free(type_str);
	HAL_Free(client_token);
	HAL_Free(pInput);
	return;	
}

int IOT_Action_Init(void *c)
{
	Qcloud_IoT_Template *pTemplate = (Qcloud_IoT_Template *)c;
	static char topic_name[MAX_SIZE_OF_CLOUD_TOPIC] = {0};

	int size = HAL_Snprintf(topic_name, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/down/action/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);
	
	if (size < 0 || size > sizeof(topic_name) - 1)
	{
		Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
		return QCLOUD_ERR_FAILURE;
	}

	SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
	sub_params.on_message_handler = _on_action_handle_callback;
	
	return IOT_MQTT_Subscribe(pTemplate->mqtt, topic_name, &sub_params);
}

//Action register 
static int _add_action_handle_to_template_list(Qcloud_IoT_Template *pTemplate, DeviceAction *pAction, OnActionHandleCallback callback)
{
    IOT_FUNC_ENTRY;

    ActionHandler *action_handle = (ActionHandler *)HAL_Malloc(sizeof(ActionHandler));
    if (NULL == action_handle)
    {
        Log_e("run memory malloc is error!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    action_handle->callback = callback;
    action_handle->action = pAction;

    ListNode *node = list_node_new(action_handle);
    if (NULL == node) {
        Log_e("run list_node_new is error!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }
    list_rpush(pTemplate->inner_data.action_handle_list, node);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

static int _check_action_existence(Qcloud_IoT_Template *ptemplate, DeviceAction *pAction)
{
    ListNode *node;
    HAL_MutexLock(ptemplate->mutex);
    node = list_find(ptemplate->inner_data.action_handle_list, pAction);
    HAL_MutexUnlock(ptemplate->mutex);
    return (NULL != node);
}

int IOT_Action_Register(void *pTemplate, DeviceAction *pAction, OnActionHandleCallback callback)
{
    IOT_FUNC_ENTRY;

	Qcloud_IoT_Template *ptemplate = (Qcloud_IoT_Template *)pTemplate;

    POINTER_SANITY_CHECK(pTemplate, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(callback, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pAction, QCLOUD_ERR_INVAL);

	if (_check_action_existence(ptemplate, pAction)) 
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_ACTION_EXIST);

    int rc;
    HAL_MutexLock(ptemplate->mutex);
    rc = _add_action_handle_to_template_list(ptemplate, pAction, callback);
    HAL_MutexUnlock(ptemplate->mutex);

    IOT_FUNC_EXIT_RC(rc);
}

int IOT_Action_Remove(void *pTemplate, DeviceAction *pAction)
{
	Qcloud_IoT_Template *ptemplate = (Qcloud_IoT_Template *)pTemplate;
	if (!_check_action_existence(ptemplate, pAction)) 
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_NOT_ACTION_EXIST);

    int rc = QCLOUD_RET_SUCCESS;

    ListNode *node;
    HAL_MutexLock(ptemplate->mutex);
    node = list_find(ptemplate->inner_data.action_handle_list, pAction);
    if (NULL == node) {
        rc = QCLOUD_ERR_NOT_PROPERTY_EXIST;
        Log_e("Try to remove a non-existent action.");
    } else {
        list_remove(ptemplate->inner_data.action_handle_list, node);
    }
    HAL_MutexUnlock(ptemplate->mutex);
    
    return rc;
}

//Action post to server
static int _iot_action_json_init(void *handle, char *jsonBuffer, size_t sizeOfBuffer, const char *pClientToken, DeviceAction *pAction) 
{
	POINTER_SANITY_CHECK(jsonBuffer, QCLOUD_ERR_INVAL);
	int32_t rc_of_snprintf;
	memset(jsonBuffer, 0, sizeOfBuffer);
	rc_of_snprintf = HAL_Snprintf(jsonBuffer, sizeOfBuffer, "{\"method\":\"%s\", \"clientToken\":\"%s\", ", REPORT_ACTION, pClientToken);

    return check_snprintf_return(rc_of_snprintf, sizeOfBuffer);
}

static int _iot_construct_action_json(void *handle, char *jsonBuffer, size_t sizeOfBuffer, const char *pClientToken, DeviceAction *pAction, sReplyPara *replyPara) 
{	 
	 size_t remain_size = 0;
	 int32_t rc_of_snprintf = 0;
	 uint8_t i;
	 Qcloud_IoT_Template* ptemplate = (Qcloud_IoT_Template *)handle;
	 
	 POINTER_SANITY_CHECK(ptemplate, QCLOUD_ERR_INVAL);
	 POINTER_SANITY_CHECK(jsonBuffer, QCLOUD_ERR_INVAL);
	 POINTER_SANITY_CHECK(pClientToken, QCLOUD_ERR_INVAL);
	 POINTER_SANITY_CHECK(pAction, QCLOUD_ERR_INVAL);

	 int rc = _iot_action_json_init(ptemplate, jsonBuffer, sizeOfBuffer, pClientToken, pAction);
	 if (rc != QCLOUD_RET_SUCCESS) {
		 Log_e("event json init failed: %d", rc);
		 return rc;
	 }

	 if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
		 return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
	 }


	rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"code\":%d, \"status\":\"%s\", \"response\":{",\
									replyPara->code, replyPara->status_msg);

	rc = check_snprintf_return(rc_of_snprintf, remain_size);
	if (rc != QCLOUD_RET_SUCCESS) {
			return rc;
	}

	if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
		return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
	}

	DeviceProperty *pJsonNode = pAction->pOutput;
	for (i = 0; i < pAction->output_num; i++) {			
		if (pJsonNode != NULL && pJsonNode->key != NULL) {
			rc = template_put_json_node(jsonBuffer, remain_size, pJsonNode->key, pJsonNode->data, pJsonNode->type);

			if (rc != QCLOUD_RET_SUCCESS) {
				return rc;
			}
		} else {
			Log_e("%dth/%d null event property data", i, pAction->output_num);
			return QCLOUD_ERR_INVAL;
		}
		pJsonNode++;
	}	
	if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
		return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
	}
	
	rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer) - 1, remain_size, "}" );

	rc = check_snprintf_return(rc_of_snprintf, remain_size);
	if (rc != QCLOUD_RET_SUCCESS) {
			return rc;
	}

	//finish json
	 if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) < 1) {
		 return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
	 }
	 rc_of_snprintf = HAL_Snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "}");
	 
	 return check_snprintf_return(rc_of_snprintf, remain_size);
}	

static int _publish_action_to_cloud(void *c, char *pJsonDoc)
{
	IOT_FUNC_ENTRY;
	int rc = QCLOUD_RET_SUCCESS;
	char topic[MAX_SIZE_OF_CLOUD_TOPIC] = {0};
	Qcloud_IoT_Template *ptemplate = (Qcloud_IoT_Template *)c;

	int size = HAL_Snprintf(topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/up/action/%s/%s", iot_device_info_get()->product_id, iot_device_info_get()->device_name);	
	if (size < 0 || size > sizeof(topic) - 1)
	{
		Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic));
		return QCLOUD_ERR_FAILURE;
	}

	PublishParams pubParams = DEFAULT_PUB_PARAMS;
	pubParams.qos = QOS1; 
	pubParams.payload_len = strlen(pJsonDoc);
	pubParams.payload = (char *) pJsonDoc;

	rc = IOT_MQTT_Publish(ptemplate->mqtt, topic, &pubParams);

	IOT_FUNC_EXIT_RC(rc);
}

int IOT_ACTION_REPLY(void *pClient, const char *pClientToken, char *pJsonDoc, size_t sizeOfBuffer, DeviceAction *pAction, sReplyPara *replyPara) 									                                           
{
	int rc;
	
	rc = _iot_construct_action_json(pClient, pJsonDoc, sizeOfBuffer, pClientToken, pAction, replyPara);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("construct action json fail, %d",rc);
		return rc;
	}
		
	rc = _publish_action_to_cloud(pClient, pJsonDoc);	
	if (rc < 0) {
		Log_e("publish action to cloud fail, %d",rc);
	}

	return rc;
}

#endif 
