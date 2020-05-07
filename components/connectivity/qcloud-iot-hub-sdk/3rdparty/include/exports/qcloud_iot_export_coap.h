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

#ifndef QCLOUD_IOT_EXPORT_COAP_H_
#define QCLOUD_IOT_EXPORT_COAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief CoAP needResp message callback function define
 *
 * @param message CoAP message
 * @param userContext User context when sending message
 */
typedef void (*OnRespCallback)(void *message, void *userContext);

/**
 * CoAP EVENT TYPE
 */
typedef enum {
    COAP_EVENT_RECEIVE_ACK           = 0,   // MSG ACK received
    COAP_EVENT_RECEIVE_RESPCONTENT   = 1,   // MSG response received
    COAP_EVENT_UNAUTHORIZED          = -1,  // Device auth failed or token expired
    COAP_EVENT_FORBIDDEN             = -2,  // CoAP URI invalid or without permission
    COAP_EVENT_INTERNAL_SERVER_ERROR = -3,  // CoAP server error
    COAP_EVENT_ACK_TIMEOUT           = -4,  // MSG ACK timeout
    COAP_EVENT_SEPRESP_TIMEOUT       = -5,  // MSG response timeout
} CoAPEventType;

typedef struct {
    /* event type */
    CoAPEventType event_type;
    /* message content */
    void *message;

} CoAPEventMessage;

typedef void (*CoAPEventHandleFunc)(void *pcontext, CoAPEventMessage *message);

/* The structure of COAP event handle */
typedef struct {
    CoAPEventHandleFunc h_fp;
    void *              context;
} CoAPEventHandler;

/**
 * @brief CoAP data structure when sending msg
 */
typedef struct {
    bool           need_resp;      // Need response from peer (true), or not (false). Currently only false is supported
    char *         pay_load;       // COAP msg payload
    size_t         pay_load_len;   // COAP msg payload length
    void *         user_context;   // user context for callback
    OnRespCallback resp_callback;  // function callback when recv msg response, valid only when need_resp is true
} SendMsgParams;

#define DEFAULT_SENDMSG_PARAMS     \
    {                              \
        false, NULL, 0, NULL, NULL \
    }

/**
 * CoAP data structure when constructing client
 */
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

    uint32_t         command_timeout;  // coap timeout value when waiting for ACK/response
    unsigned char    max_retry_count;  // CoAP max sending retry count
    CoAPEventHandler event_handle;     // event callback

} CoAPInitParams;

#ifdef AUTH_MODE_CERT
#define DEFAULT_COAPINIT_PARAMS              \
    {                                        \
        NULL, NULL, {0}, {0}, 2000, 5, { 0 } \
    }
#else
#define DEFAULT_COAPINIT_PARAMS          \
    {                                    \
        NULL, NULL, NULL, 2000, 5, { 0 } \
    }
#endif

/**
 * @brief Construct CoAP Client and connect to server
 *
 * @param pParams CoAP Client construct and connect parameters
 *
 * @return COAPClient pointer, NULL when fail
 */
void *IOT_COAP_Construct(CoAPInitParams *pParams);

/**
 * @brief Disconnect with COAP server and destroy COAPClient
 *
 * @param pClient COAPClient pointer
 *
 */
void IOT_COAP_Destroy(void **pClient);

/**
 * @brief Read and handle CoAP msg in synchronized way
 *
 * @param pClient    COAPClient pointer
 * @param timeout_ms timeout value (unit: ms) for this Yielding
 * @return QCLOUD_RET_SUCCESS when success, otherwise fail
 */
int IOT_COAP_Yield(void *pClient, uint32_t timeout_ms);

/**
 * @brief Send/publish CoAP msg
 *
 * @param pClient           COAPClient pointer
 * @param topicName         CoAP URI (msg topic)
 * @param sendParams        Tx msg parameters
 * @return value > 0 for msg Id; value < 0 for failure
 */
int IOT_COAP_SendMessage(void *pClient, char *topicName, SendMsgParams *sendParams);

/**
 * @brief Get COAP Response msgId
 *
 * @param pMessage  COAP Response msg
 * @return value > 0 for msg Id; value < 0 for failure
 */
int IOT_COAP_GetMessageId(void *pMessage);

/**
 * @brief Get COAP Response payload
 *
 * @param pMessage          COAP Response msg
 * @param payload           msg payload
 * @param payloadLen        msg payload length
 * @return QCLOUD_RET_SUCCESS when success, otherwise fail
 */
int IOT_COAP_GetMessagePayload(void *pMessage, char **payload, int *payloadLen);

/**
 * @brief Get COAP Response msg error code
 *
 * @param pMessage  COAP Response msg
 * @return  COAPEventType converted from msg error code
 */
int IOT_COAP_GetMessageCode(void *pMessage);

/**
 * @brief Get the device Info of the dedicated COAP client
 *
 * @param pClient       handle to COAP client
 * @return pointer to the device Info
 */
DeviceInfo *IOT_COAP_GetDeviceInfo(void *pClient);

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_COAP_H_ */
