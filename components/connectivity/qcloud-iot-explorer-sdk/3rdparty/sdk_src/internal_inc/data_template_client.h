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

/**
 * @brief API of data_template
 *
 */
#ifndef IOT_TEMPLATE_CLIENT_H_
#define IOT_TEMPLATE_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "utils_param_check.h"
#include "data_template_client_json.h"
#include "qcloud_iot_device.h"
#include "mqtt_client.h"


#define MAX_CLEAE_DOC_LEN		256

typedef struct _TemplateInnerData {
    uint32_t token_num;
    int32_t sync_status;
	uint32_t eventflags;
	List *event_list;
    List *reply_list;
	List *action_handle_list;
    List *property_handle_list;   
	char *upstream_topic;		//upstream topic
    char *downstream_topic;		//downstream topic
} TemplateInnerData;

typedef struct _Template {
    void *mqtt;
    void *mutex;
    MQTTEventHandler event_handle;
    TemplateInnerData inner_data;
} Qcloud_IoT_Template;


/**
 * @brief init data template client
 * 
 * @param pTemplate   handle to data_template client
 */
int qcloud_iot_template_init(Qcloud_IoT_Template *pTemplate);

/**
 * @brief deinit data template client list and topics
 * 
 * @param pClient   data template client
 */

void qcloud_iot_template_reset(void *pClient);

/**
 * @brief deal upstream msg wait for reply timeout
 * 
 * @param pTemplate   data template client
 */
void handle_template_expired_reply(Qcloud_IoT_Template *pTemplate);

/**
 * @brief get the clientToken of control message for control_reply
 * 
 * @param void
 * @return clientToken
 */
char *  get_control_clientToken(void);

/**
 * @brief all the upstream data by the way of request
 *
 * @param pTemplate     handle to data_template client
 * @param pParams  		request params
 * @param pJsonDoc	  	  data buffer for request
 * @param sizeOfBuffer    length of data buffer
 * @return				QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int send_template_request(Qcloud_IoT_Template *pTemplate, RequestParams *pParams, char *pJsonDoc, size_t sizeOfBuffer);

/**
 * @brief subscribe data_template topic $thing/down/property/%s/%s
 *
 * @param pShadow       shadow client
 * @return         		QCLOUD_RET_SUCCESS for success, or err code for failure
 */ 
int subscribe_template_downstream_topic(Qcloud_IoT_Template *pTemplate);

#ifdef __cplusplus
}
#endif

#endif /* IOT_TEMPLATE_CLIENT_H_ */
