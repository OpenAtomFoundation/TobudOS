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

#ifndef UTILS_METHOD_H_
#define UTILS_METHOD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#define min(a,b) (a) < (b) ? (a) : (b)

/* Max number of requests in appending state */
#define MAX_APPENDING_REQUEST_AT_ANY_GIVEN_TIME                     (10)

/* Size of buffer to receive JSON document from server */
#define CLOUD_IOT_JSON_RX_BUF_LEN                                   (QCLOUD_IOT_MQTT_RX_BUF_LEN + 1)

/* Max size of clientToken */
#define MAX_SIZE_OF_CLIENT_TOKEN                                    (MAX_SIZE_OF_CLIENT_ID + 10)

/* Max size of JSON string which only contain clientToken field */
#define MAX_SIZE_OF_JSON_WITH_CLIENT_TOKEN                          (MAX_SIZE_OF_CLIENT_TOKEN + 20)


#define CLIENT_TOKEN_FIELD     		"clientToken"
#define METHOD_FIELD	         	"method"
#define TYPE_FIELD	         		"type"
#define ACTION_ID_FIELD     		"actionId"
#define TIME_STAMP_FIELD     		"timestamp"

#define REPLY_CODE					"code"
#define REPLY_STATUS				"status"

#define GET_STATUS					"get_status"			//method field for get_status
#define GET_STATUS_REPLY			"get_status_reply"		//method field for get_status reply

#define CONTROL_CMD					"control"				//method field for control msg
#define CONTROL_CMD_REPLY			"control_reply"			//method field for control msg reply

#define CLEAR_CONTROL				"clear_control"			//method field for clear control

#define REPORT_CMD					"report"				//method field for report
#define REPORT_CMD_REPLY			"report_reply"			//method field for report reply

#define INFO_CMD					"report_info"			//method field for report system informaiton
#define INFO_CMD_REPLY				"report_info_reply"		//method field for report system informaiton reply


#define GET_CONTROL_PARA			"data.control"
#define CMD_CONTROL_PARA			"params"


/**
 * @brief define type of request parameters
 */
typedef struct _RequestParam {

    Method               	method;              	// method type: GET, REPORT, RINFO, REPLY, CLEAR

    uint32_t             	timeout_sec;         	// request timeout in second

    OnReplyCallback    		request_callback;    	// request callback

    void                 	*user_context;          // user context for callback

} RequestParams;


#define DEFAULT_REQUEST_PARAMS {GET, 4, NULL, NULL};

/**
 * @brief type for document request
 */
typedef struct {
    char                   client_token[MAX_SIZE_OF_CLIENT_TOKEN];          // clientToken
    Method                 method;                                          // method type

    void                   *user_context;                                   // user context
    Timer                  timer;                                           // timer for timeout

    OnReplyCallback      callback;                                        // request response callback
} Request;


/**
 * @brief for property and it's callback
 */
typedef struct {

    void *property;							

    OnPropRegCallback callback;      		

} PropertyHandler;


/**
 * @brief save the action registed and its callback
 */
typedef struct {

    void *action;							

    OnActionHandleCallback callback;      

} ActionHandler;


/**
 * @brief check return value of snprintf
 *
 * @param returnCode       return value of snprintf
 * @param maxSizeOfWrite   max size of write buffer
 * @return                 QCLOUD_RET_SUCCESS for success, or err code for failure
 */

int check_snprintf_return(int32_t returnCode, size_t maxSizeOfWrite);

/**
 * @brief insert source string to the dest string with specified position
 *
 * @param pSourceStr   source string
 * @param pDestStr     dest string
 * @param pos          dest string positon
 */
void insert_str(char *pDestStr, char *pSourceStr, int pos);


/**
 * add a JSON node to JSON string
 *
 * @param jsonBuffer   	JSON string buffer
 * @param sizeOfBuffer  size of buffer
 * @param pKey          key of JSON node
 * @param pData         value of JSON node
 * @param type          value type of JSON node
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type);

/**
 * add a JSON node to JSON string, data_template's bool type not the same to put_json_node
 *
 * @param jsonBuffer   	JSON string buffer
 * @param sizeOfBuffer  size of buffer
 * @param pKey          key of JSON node
 * @param pData         value of JSON node
 * @param type          value type of JSON node
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */ 
int template_put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type);

/**
 * @brief generate a ClientToken
 *
 * @param pStrBuffer    string buffer
 * @param sizeOfBuffer  size of buffer
 * @param tokenNumber   shadow token number, increment every time
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int generate_client_token(char *pStrBuffer, size_t sizeOfBuffer, uint32_t *tokenNumber);

/**
 * @brief generate an empty JSON with only clientToken
 *
 * @param tokenNumber   shadow token number, increment every time
 * @param pJsonBuffer   JSON string buffer
 */
void build_empty_json(uint32_t *tokenNumber, char *pJsonBuffer);

/**
 * @brief parse field of clientToken from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pClientToken   pointer to field of ClientToken
 * @return               true for success
 */
bool parse_client_token(char *pJsonDoc, char **pClientToken);

/**
 * @brief parse field of aciont_id from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pActionID   	 pointer to field of action_id
 * @return               true for success
 */
bool parse_action_id(char *pJsonDoc, char **pActionID);

/**
 * @brief parse field of timestamp from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pTimestamp     pointer to field of timestamp
 * @return               true for success
 */
bool parse_time_stamp(char *pJsonDoc, int32_t *pTimestamp);


/**
 * @brief parse field of input from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pActionInput   filed of params as action input parameters
 * @return               true for success
 */ 
bool parse_action_input(char *pJsonDoc, char **pActionInput);

/**
 * @brief parse field of status from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pStatus   	 pointer to field of status
 * @return               true for success
 */
bool parse_status_return(char *pJsonDoc, char **pStatus);

/**
 * @brief parse field of code from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pCode   		 pointer to field of Code
 * @return               true for success
 */
bool parse_code_return(char *pJsonDoc, int32_t *pCode);


/**
 * @brief update value in JSON if key is matched, not for OBJECT type
 *
 * @param pJsonDoc       JSON string
 * @param pProperty      device property
 * @return               true for success
 */
bool update_value_if_key_match(char *pJsonDoc, DeviceProperty *pProperty);


/**
 * @brief parse field of method from JSON string
 *
 * @param pJsonDoc		 source JSON string
 * @param pMethod 		 pointer to field of method
 * @return				 true for success
 */
bool parse_template_method_type(char *pJsonDoc, char **pMethod);
 
/**
 * @brief parse field of control from get_status_reply JSON string 
 *
 * @param pJsonDoc		 source JSON string
 * @param control 		 pointer to field of control
 * @return				 true for success
 */ 
bool parse_template_get_control(char *pJsonDoc, char **control);

/**
 * @brief parse field of control from control JSON string 
 *
 * @param pJsonDoc		 source JSON string
 * @param control 		 pointer to field of control
 * @return				 true for success
 */ 
bool parse_template_cmd_control(char *pJsonDoc, char **control);

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_SHADOW_H_ */
