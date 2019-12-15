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

#ifndef QCLOUD_IOT_EXPORT_METHOD_H_
#define QCLOUD_IOT_EXPORT_METHOD_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Type of request ACK
 */
typedef enum {
    ACK_NONE = -3,      // request timeout
    ACK_TIMEOUT = -2,   // request timeout
    ACK_REJECTED = -1,  // request rejected
    ACK_ACCEPTED = 0    // request accepted
} ReplyAck;

/**
 * @brief Type of interaction with server
 */
typedef enum {
    GET = 0,      // Get data_template state from server
    REPORT = 1,   //  Report property to server
    RINFO = 2,    //  Report system information to server
    REPLY = 3,    //  Reply to control msg
    CLEAR = 4,    //   Clear control msg
} Method;

/**
 * @brief JSON data type
 */
typedef enum {
    JINT32,      
    JINT16,      
    JINT8,      
    JUINT32,     
    JUINT16,     
    JUINT8,      
    JFLOAT,     
    JDOUBLE,     
    JBOOL,       
    JSTRING,     
    JOBJECT      
} JsonDataType;

/**
 * @brief Define a device property, as a JSON document node
 */
typedef struct _JSONNode {
    char   		 *key;    // Key of this JSON node
    void         *data;   // Value of this JSON node
    uint16_t     data_buff_len;  // data buff len, for string type value update
    JsonDataType type;    // Data type of this JSON node
} DeviceProperty;

/**
 * @brief Define a device action
 */
typedef struct {
    char    *pActionId;     // action id
    uint32_t timestamp;	    // action timestamp
    uint8_t  input_num;     // input num
    uint8_t  output_num;    // output mun
    DeviceProperty *pInput;  // input 
    DeviceProperty *pOutput; // output
} DeviceAction;


/**
 * @brief Define MQTT data_template callback when request response arrived
 *
 * @param method         type of request
 * @param requestAck     response type 
 * @param pJsonDocument  JSON document from server
 * @param userContext    User context
 *
 */  
typedef void (*OnReplyCallback)(void *pClient, Method method, ReplyAck replyAck, const char *pJsonDocument, void *userContext);


/**
 * @brief Define callback when device property change
 *
 * @param pJsonValueBuffer property JSON buffer
 * @param valueLength      property length
 * @param DeviceProperty   reference to device property
 */
typedef void (*OnPropRegCallback)(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty);

/**
 * @brief action handle callback 
 *
 * @param pAction   action with input data
 */
typedef void (*OnActionHandleCallback)(void *pClient, const char *pClientToken, DeviceAction *pAction);


#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_SHADOW_H_ */
