/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_IOT_EXPORT_SHADOW_H_
#define QCLOUD_IOT_EXPORT_SHADOW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_export_mqtt.h"

typedef enum _eShadowType_ {
    eSHADOW   = 0,  // normal shadow
    eTEMPLATE = 1,  // data template
} eShadowType;

/* The structure of MQTT shadow init parameters */
typedef struct {
    /* device info */
    char *product_id;   // product ID
    char *device_name;  // device name

#ifdef AUTH_MODE_CERT
    char cert_file[FILE_PATH_MAX_LEN];  // full path of device cert file
    char key_file[FILE_PATH_MAX_LEN];   // full path of device key file
#else
    char *device_secret;  // device secret
#endif

    uint32_t         command_timeout;         // timeout value (unit: ms) for MQTT connect/pub/sub/yield
    uint32_t         keep_alive_interval_ms;  // MQTT keep alive time interval in millisecond
    uint8_t          clean_session;           // flag of clean session, 1 clean, 0 not clean
    uint8_t          auto_connect_enable;     // flag of auto reconnection, 1 is enable and recommended
    MQTTEventHandler event_handle;            // event callback
    eShadowType      shadow_type;             // shadow type
} ShadowInitParams;

#ifdef AUTH_MODE_CERT
#define DEFAULT_SHAWDOW_INIT_PARAMS                          \
    {                                                        \
        NULL, NULL, {0}, {0}, 5000, 240 * 1000, 1, 1, {0}, 0 \
    }
#else
#define DEFAULT_SHAWDOW_INIT_PARAMS                      \
    {                                                    \
        NULL, NULL, NULL, 5000, 240 * 1000, 1, 1, {0}, 0 \
    }
#endif

/**
 * @brief Type of shadow request ACK
 */
typedef enum {
    ACK_NONE     = -3,  // ACK is init state
    ACK_TIMEOUT  = -2,  // received ACK timeout
    ACK_REJECTED = -1,  // ACK is rejected
    ACK_ACCEPTED = 0,   // ACK is accepted
} RequestAck;

/**
 * @brief Type of interaction with server
 */
typedef enum {
    GET,     // Get device document from server
    UPDATE,  // Update or create device document in server
} Method;

/**
 * @brief JSON data type
 */
typedef enum { JINT32, JINT16, JINT8, JUINT32, JUINT16, JUINT8, JFLOAT, JDOUBLE, JBOOL, JSTRING, JOBJECT } JsonDataType;

/**
 * @brief Define a device property, as a JSON document node
 */
typedef struct _JSONNode {
    char *       key;   // Key of this JSON node
    void *       data;  // Value of this JSON node
    JsonDataType type;  // Data type of this JSON node
    bool         delta_arrived;
} DeviceProperty;

/**
 * @brief Data type of template
 */

#define TYPE_TEMPLATE_INT     JINT32
#define TYPE_TEMPLATEENUM     JINT32
#define TYPE_TEMPLATE_FLOAT   JFLOAT
#define TYPE_TEMPLATE_BOOL    JINT8
#define TYPE_TEMPLATE_STRING  JSTRING
#define TYPE_TEMPLATE_TIME    JUINT32
#define TYPE_TEMPLATE_JOBJECT JOBJECT

typedef int32_t  TYPE_DEF_TEMPLATE_INT;
typedef int32_t  TYPE_DEF_TEMPLATE_ENUM;
typedef float    TYPE_DEF_TEMPLATE_FLOAT;
typedef char     TYPE_DEF_TEMPLATE_BOOL;
typedef char     TYPE_DEF_TEMPLATE_STRING;
typedef uint32_t TYPE_DEF_TEMPLATE_TIME;
typedef void *   TYPE_DEF_TEMPLATE_OBJECT;

/**
 * @brief Define property status in data template
 */
typedef enum _eDataState_ {
    eNOCHANGE = 0,
    eCHANGED  = 1,
} eDataState;

/**
 * @brief Define data point in data template
 */
typedef struct {
    DeviceProperty data_property;
    eDataState     state;
} sDataPoint;

/**
 * @brief Define MQTT shadow callback when request response arrived
 *
 * @param method         type of request
 * @param requestAck     response type
 * @param pJsonDocument  JSON document from server
 * @param userContext    User context
 *
 */
typedef void (*OnRequestCallback)(void *pClient, Method method, RequestAck requestAck, const char *pJsonDocument,
                                  void *userContext);

/**
 * @brief Define callback when device property change
 *
 * @param pJsonValueBuffer property JSON buffer
 * @param valueLength      property length
 * @param DeviceProperty   reference to device property
 */
typedef void (*OnPropRegCallback)(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength,
                                  DeviceProperty *pProperty);

/**
 * @brief Create MQTT Shadow client and connect to MQTT server
 *
 * @param pParams Shadow init parameters
 *
 * @return a valid Shadow client handle when success, or NULL otherwise
 */
void *IOT_Shadow_Construct(ShadowInitParams *pParams);

/**
 * @brief Return the MQTT client dedicated to this shadow handle
 *
 * @param pClient       handle to shadow client
 *
 * @return a valid mqtt client handle when success, or NULL otherwise
 */
void *IOT_Shadow_Get_Mqtt_Client(void *handle);

/**
 * @brief Publish MQTT message
 *
 * @param pClient       handle to shadow client
 * @param topicName     MQTT topic name
 * @param pParams       publish parameters
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_Shadow_Publish(void *pClient, char *topicName, PublishParams *pParams);

/**
 * @brief Subscribe MQTT message
 *
 * @param pClient       handle to shadow client
 * @param topicFilter   MQTT topic filter
 * @param pParams       subscribe parameters
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_Shadow_Subscribe(void *pClient, char *topicFilter, SubscribeParams *pParams);

/**
 * @brief Unsubscribe MQTT message
 *
 * @param pClient       handle to shadow client
 * @param topicFilter   MQTT topic filter
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_Shadow_Unsubscribe(void *pClient, char *topicFilter);

/**
 * @brief Check if MQTT shadow is connected
 *
 * @param pClient       handle to shadow client
 * @return true= connected, false = unconnected
 */
bool IOT_Shadow_IsConnected(void *pClient);

/**
 * @brief Close connection and destroy MQTT shadow client
 *
 * @param pClient    pointer of handle to shadow client
 *
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int IOT_Shadow_Destroy(void *pClient);

/**
 * @brief Check connection and keep alive state, read/handle MQTT message in synchronized way
 *
 * @param pClient    handle to shadow client
 * @param timeout_ms timeout value (unit: ms) for this operation
 *
 * @return QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_Yield(void *pClient, uint32_t timeout_ms);

/**
 * @brief Update device shadow document in asynchronized way
 *
 * @param pClient           handle to shadow client
 * @param pJsonDoc          source JSON document for update
 * @param sizeOfBuffer      length of JSON document
 * @param callback          callback when response arrive
 * @param userContext       user data for callback
 * @param timeout_ms        timeout value for this operation (unit: ms)
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_Update(void *pClient, char *pJsonDoc, size_t sizeOfBuffer, OnRequestCallback callback, void *userContext,
                      uint32_t timeout_ms);

/**
 * @brief Update device shadow document in synchronized way
 *
 * @param pClient           handle to shadow client
 * @param pJsonDoc          source JSON document for update
 * @param sizeOfBuffer      length of JSON document
 * @param timeout_ms        timeout value for this operation (unit: ms)
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure

 */
int IOT_Shadow_Update_Sync(void *pClient, char *pJsonDoc, size_t sizeOfBuffer, uint32_t timeout_ms);

/**
 * @brief Get device shadow document in asynchronized way
 *
 * @param pClient           handle to shadow client
 * @param callback          callback when response arrive
 * @param userContext       user data for callback
 * @param timeout_ms        timeout value for this operation (unit: ms)
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_Get(void *pClient, OnRequestCallback callback, void *userContext, uint32_t timeout_ms);

/**
 * @brief Get device shadow document in asynchronized way
 *
 * @param pClient           handle to shadow client
 * @param timeout_ms        timeout value for this operation (unit: ms)
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_Get_Sync(void *pClient, uint32_t timeout_ms);

/**
 * @brief Register device property
 *
 * @param pClient           handle to shadow client
 * @param pProperty         reference to device property
 * @param callback          callback when property changes
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_Register_Property(void *pClient, DeviceProperty *pProperty, OnPropRegCallback callback);

/**
 * @brief UnRegister device property
 *
 * @param pClient           handle to shadow client
 * @param pProperty         reference to device property
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_UnRegister_Property(void *pClient, DeviceProperty *pProperty);

/**
 * @brief Add reported fields in JSON document, don't overwrite
 *
 * @param pClient       handle to shadow client
 * @param jsonBuffer    string buffer to store JSON document
 * @param sizeOfBuffer  size of string buffer
 * @param count         number of properties
 * @return              QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_JSON_ConstructReport(void *pClient, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, ...);

/**
 * @brief Add reported fields array in JSON document, don't overwrite
 *
 * @param pClient       handle to shadow client
 * @param jsonBuffer    string buffer to store JSON document
 * @param sizeOfBuffer  size of string buffer
 * @param count         number of properties
 * @param pDeviceProperties         array of properties
 * @return              QCLOUD_RET_SUCCESS when success, or err code for failure */

int IOT_Shadow_JSON_ConstructReportArray(void *pClient, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count,
                                         DeviceProperty *pDeviceProperties[]);

/**
 * @brief Add reported fields in JSON document, overwrite
 *
 * @param pClient       handle to shadow client
 * @param jsonBuffer    string buffer to store JSON document
 * @param sizeOfBuffer  size of string buffer
 * @param count         number of properties
 * @return              QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_JSON_Construct_OverwriteReport(void *pClient, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, ...);

/**
 * @brief Add reported fields array in JSON document and set desire field as null
 *
 * @param pClient       handle to shadow client
 * @param jsonBuffer    string buffer to store JSON document
 * @param sizeOfBuffer  size of string buffer
 * @param count         number of properties
 * @return              QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_JSON_ConstructReportAndDesireAllNull(void *pClient, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count,
                                                    ...);

/**
 * @brief Set desire field as null
 *
 * @param jsonBuffer    string buffer to store JSON document
 * @param sizeOfBuffer  size of string buffer
 * @return              QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Shadow_JSON_ConstructDesireAllNull(void *handle, char *jsonBuffer, size_t sizeOfBuffer);

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_SHADOW_H_ */
