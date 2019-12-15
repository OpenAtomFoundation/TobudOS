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

#ifndef IOT_COAP_CLIENT_H_
#define IOT_COAP_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
// #include <sys/types.h>

#include "qcloud_iot_export.h"

#include "coap_client_net.h"

#include "network_interface.h"
#include "utils_list.h"
#include "utils_timer.h"

#define ssize_t     size_t
#define ENOSPC      1

/* COAP protocol version  */
#define COAP_MSG_VER                           						(0x01)

/* COAP init token */
#define COAP_MSG_INIT_TOKEN     									(0x01020304)

/* Minimal command timeout of CoAP ACK/RESP */
#define MIN_COMMAND_TIMEOUT         								(500)

/* Maximal command timeout of CoAP ACK/RESP */
#define MAX_COMMAND_TIMEOUT         								(5000)

/* Max size of conn Id */
#define COAP_MAX_CONN_ID_LEN                                        (6)

/* Max size of Message id */
#define COAP_MSG_MAX_MSG_ID                    						((1 << 16) - 1)

/* Max size of Topic length */
#define URI_PATH_MAX_LEN           									(128)

/* CoAP auth success */
#define COAP_TRUE    												(1)

/* CoAP auth fail */
#define COAP_FALSE   												(0)

/* unique CoAP auth URI */
#define COAP_AUTH_URI												("txauth9w0BAQsFA")

/* Max size of token */
#define COAP_MSG_MAX_TOKEN_LEN                 						(8)

/* COAP Max code class */
#define COAP_MSG_MAX_CODE_CLASS                						(7)

/* COAP Max code detail */
#define COAP_MSG_MAX_CODE_DETAIL               						(31)

/* Get field of Option num */
#define COAP_MSG_OPTION_NUM(option)                					((option)->option_num)

/* Get field of Next Option */
#define COAP_MSG_OP_NEXT(option)               						((option)->next)

/* If COAP msg is empty */
#define COAP_MSG_IS_EMPTY(message)                 					(((message)->code_class == 0) && ((message)->code_detail == 0))

/* If COAP msg is empty ACK */
#define COAP_MSG_IS_EMPTY_ACK(message)                 					(((message)->code_class == 2) && ((message)->code_detail == 3))

/* If COAP msg is RESP */
#define COAP_MSG_IS_EMPTY_RSP(message)                 					(((message)->code_class == 2) && ((message)->code_detail == 5))

/**
 *  @brief COAP msg type
 */
typedef enum {
    COAP_MSG_REQ = 0,
    COAP_MSG_SUCCESS = 2,
    COAP_MSG_CLIENT_ERR = 4,
    COAP_MSG_SERVER_ERR = 5,
    COAP_MSG_SDKINTERNAL_ERR = 6,
} CoAPMessageClass;

/**
 *  @brief COAP msg type
 */
typedef enum {
    COAP_MSG_GET = 1,
    COAP_MSG_POST = 2,
    COAP_MSG_PUT = 3,
    COAP_MSG_DELETE =4
} CoAPRequestMethod;

typedef enum {
	/* CoAP Success Response code detail */
	COAP_MSG_CODE_201_CREATED                    = 01,  /* Mapping to CoAP codeClass.codeDetail 2.01 */
	COAP_MSG_CODE_202_DELETED                    = 02,  /* Mapping to CoAP codeClass.codeDetail 2.02 */
	COAP_MSG_CODE_203_VALID                      = 03,  /* Mapping to CoAP codeClass.codeDetail 2.03 */
	COAP_MSG_CODE_204_CHANGED                    = 04,  /* Mapping to CoAP codeClass.codeDetail 2.04 */
	COAP_MSG_CODE_205_CONTENT                    = 05,  /* Mapping to CoAP codeClass.codeDetail 2.05 */
	COAP_MSG_CODE_231_CONTINUE                   = 31,  /* Mapping to CoAP codeClass.codeDetail 2.31 */

	/* CoAP Client Error Response code detail */
	COAP_MSG_CODE_400_BAD_REQUEST                = 00,  /* Mapping to CoAP codeClass.codeDetail 4.00 */
	COAP_MSG_CODE_401_UNAUTHORIZED               = 01,  /* Mapping to CoAP codeClass.codeDetail 4.01 */
	COAP_MSG_CODE_402_BAD_OPTION                 = 02,  /* Mapping to CoAP codeClass.codeDetail 4.02 */
	COAP_MSG_CODE_403_FORBIDDEN                  = 03,  /* Mapping to CoAP codeClass.codeDetail 4.03 */
	COAP_MSG_CODE_404_NOT_FOUND                  = 04,  /* Mapping to CoAP codeClass.codeDetail 4.04 */
	COAP_MSG_CODE_405_METHOD_NOT_ALLOWED         = 05,  /* Mapping to CoAP codeClass.codeDetail 4.05 */
	COAP_MSG_CODE_406_NOT_ACCEPTABLE             = 06,  /* Mapping to CoAP codeClass.codeDetail 4.06 */
	COAP_MSG_CODE_408_REQUEST_ENTITY_INCOMPLETE  = 8,   /* Mapping to CoAP codeClass.codeDetail 4.08 */
	COAP_MSG_CODE_412_PRECONDITION_FAILED        = 12,  /* Mapping to CoAP codeClass.codeDetail 4.12 */
	COAP_MSG_CODE_413_REQUEST_ENTITY_TOO_LARGE   = 13,  /* Mapping to CoAP codeClass.codeDetail 4.13 */
	COAP_MSG_CODE_415_UNSUPPORTED_CONTENT_FORMAT = 15,  /* Mapping to CoAP codeClass.codeDetail 4.15 */

	/* CoAP Server Error Response code detail */
	COAP_MSG_CODE_500_INTERNAL_SERVER_ERROR      = 00,  /* Mapping to CoAP codeClass.codeDetail 5.00 */
	COAP_MSG_CODE_501_NOT_IMPLEMENTED            = 01,  /* Mapping to CoAP codeClass.codeDetail 5.01 */
	COAP_MSG_CODE_502_BAD_GATEWAY                = 02,  /* Mapping to CoAP codeClass.codeDetail 5.02 */
	COAP_MSG_CODE_503_SERVICE_UNAVAILABLE        = 03,  /* Mapping to CoAP codeClass.codeDetail 5.03 */
	COAP_MSG_CODE_504_GATEWAY_TIMEOUT            = 04,  /* Mapping to CoAP codeClass.codeDetail 5.04 */
	COAP_MSG_CODE_505_PROXYING_NOT_SUPPORTED     = 05,  /* Mapping to CoAP codeClass.codeDetail 5.05 */

	COAP_MSG_CODE_600_TIMEOUT					 = 00,  /* Mapping to self define CoAP codeClass.codeDetail 6.00 */
} CoAPRespCodeDetail;

/**
 *  @brief Option number enumeration
 */
typedef enum {
    COAP_MSG_IF_MATCH = 1,                                                      // If-Match option number
    COAP_MSG_URI_HOST = 3,                                                      // URI-Host option number
    COAP_MSG_ETAG = 4,                                                          // Entity-Tag option number
    COAP_MSG_IF_NONE_MATCH = 5,                                                 // If-None-Match option number
    COAP_MSG_URI_PORT = 7,                                                      // URI-Port option number
    COAP_MSG_LOCATION_PATH = 8,                                                 // Location-Path option number
    COAP_MSG_URI_PATH = 11,                                                     // URI-Path option number
    COAP_MSG_CONTENT_FORMAT = 12,                                               // Content-Format option number
    COAP_MSG_MAX_AGE = 14,                                                      // Max-Age option number
    COAP_MSG_URI_QUERY = 15,                                                    // URI-Query option number
    COAP_MSG_ACCEPT = 17,                                                       // Accept option number
    COAP_MSG_LOCATION_QUERY = 20,                                               // Location-Query option number
    COAP_MSG_BLOCK2 = 23,                                                       // Block2 option number
    COAP_MSG_BLOCK1 = 27,                                                       // Block1 option number
    COAP_MSG_SIZE2 = 28,                                                        // Size2 option number
    COAP_MSG_PROXY_URI = 35,                                                    // Proxy-URI option number
    COAP_MSG_PROXY_SCHEME = 39,                                                 // Proxy-Scheme option number
    COAP_MSG_SIZE1 = 60,                                                        // Size1 option number

    COAP_MSG_AUTH_TOKEN = 61,													// auth token option number
    COAP_MSG_NEED_RESP = 62,													// CoAP need content response
} CoAPMsgOptionNum;

/* CoAP QCloud IoT Client structure */
typedef struct Client {
	char									is_authed;											// CoAP Client auth or not
    char					 	            conn_id[COAP_MAX_CONN_ID_LEN];						// conn id for a CoAP connection

	unsigned int         					message_token;										// msg token

	char                					*auth_token;										// auth token
	int                  					auth_token_len;

    uint16_t                 				next_msg_id;                                		// COAP msg id

    size_t                   				send_buf_size;                                		// size of write buffer
    size_t                   				read_buf_size;                                 		// size of read buffer

    unsigned char							send_buf[COAP_SENDMSG_MAX_BUFLEN];
    unsigned char							recv_buf[COAP_RECVMSG_MAX_BUFLEN];

    void                     				*lock_send_buf;                          			// mutex/lock for write buffer
    void                     				*lock_list_wait_ack;                                // mutex/lock for wait ack list

    Network                  				network_stack;										// MQTT network stack

    uint32_t                 				command_timeout_ms;                                	// CoAP command timeout, unit:ms

    List*									message_list;										// msg list

    unsigned char               			max_retry_count;         							// Max retry count

    CoAPEventHandler            			event_handle;            							// event callback

} CoAPClient;

/**
 *  @brief CoAP Option
 */
typedef struct coap_msg_op
{
    unsigned short							option_num;                                         // Option number
    unsigned 								val_len;                                            // Option length
    char 									*val;                                               // Pointer to a buffer containing the option value
    struct coap_msg_op 						*next;                                              // Pointer to the next option structure in the list
} CoAPMsgOption;

/**
 *  @brief CoAP Option list
 */
typedef struct
{
	CoAPMsgOption 							*first;                                             // Pointer to the first option structure in the list
	CoAPMsgOption 							*last;                                              // Pointer to the last option structure in the list
} CoAPMsgOptionList;

/**
 * @brief CoAP node state
 */
typedef enum {
    COAP_NODE_STATE_NORMANL = 0,
    COAP_NODE_STATE_INVALID,
} CoAPNodeState;

typedef struct {
    CoAPNodeState           				node_state;
	void                    				*user_context;
	unsigned short           				msg_id;
	char                     				acked;
	unsigned char            				token_len;
	unsigned char            				token[COAP_MSG_MAX_TOKEN_LEN];
	unsigned char            				retrans_count;
	Timer           						start_time;
	unsigned char           				*message;
	unsigned int             				msglen;
	OnRespCallback       	 				handler;
} CoAPMsgSendInfo;

/**
 *  @brief COAP msg type
 */
typedef enum {
    COAP_MSG_CON = 0x0,                                                         /**< msg need to wait for ACK */
    COAP_MSG_NON = 0x1,                                                         /**< msg no need to wait for ACK */
    COAP_MSG_ACK = 0x2,                                                         /**< msg ACK */
    COAP_MSG_RST = 0x3                                                          /**< msg Reset */
} CoAPMsgType;

/**
 *  @brief CoAP message structure
 */
typedef struct
{
    unsigned 								version;                                            // CoAP protocol version
    CoAPMsgType 							type;                                               // msg type

    unsigned 								code_class;                                         // Code class
    unsigned 								code_detail;                                        // Code detail

    unsigned short 							msg_id;                                             // msg id

    char 									*pay_load;                                          // msg payload
    size_t 									pay_load_len;                                       // length of payload

    char 									token[COAP_MSG_MAX_TOKEN_LEN];                      // msg token
    unsigned 								token_len;                                          // length of token

    CoAPMsgOptionList 						op_list;                                            // Option list

    OnRespCallback							handler;											// CoAP Response msg callback
    void									*user_context;										// user context
} CoAPMessage;

#define DEFAULT_COAP_MESSAGE {COAP_MSG_VER, COAP_MSG_CON, COAP_MSG_REQ, COAP_MSG_POST, 0, NULL, 0, {0}, 0, {0}, NULL, NULL}

/**
 * @brief Init CoAPClient
 *
 * @param pClient reference to CoAP client
 * @param pParams CoAP init parameters
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_iot_coap_init(CoAPClient *pClient, CoAPInitParams *pParams);

/**
 * @brief Generate next CoAPMessage msg Id
 *
 * @param pClient reference to CoAP client
 * @return msg Id
 */
uint16_t get_next_coap_msg_id(CoAPClient *pClient);

/**
 * @brief Generate next CoAPMessage msg token
 *
 * @param pClient reference to CoAP client
 * @param tokenData msg token
 * @return token length
 */
unsigned int get_coap_message_token(CoAPClient *client, char *tokenData);

/**
 * @brief set message type
 *
 * @param message CoAP msg
 * @param
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int coap_message_type_set(CoAPMessage *message, unsigned type);

/**
 * @brief set message code
 *
 * @param message CoAP msg
 * @param code_class CoAPMessageClass
 * @param code_detail
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int coap_message_code_set(CoAPMessage *message, unsigned code_class, unsigned code_detail);

/**
 * @brief set message Id
 *
 * @param message CoAP msg
 * @param msg_id
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int coap_message_id_set(CoAPMessage *message, unsigned msg_id);

/**
 * @brief set msg token
 *
 * @param message CoAP msg
 * @param buf token string
 * @param len token length
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int coap_message_token_set(CoAPMessage *message, char *buf, size_t len);

/**
 * @brief set msg payload
 *
 * @param message CoAP msg
 * @param buf msg payload buffer
 * @param len length of payload
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int coap_message_payload_set(CoAPMessage *message, char *buf, size_t len);

/**
 * @brief add msg option
 *
 * @param message CoAP msg
 * @param num option number
 * @param len option length
 * @param val option string
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int coap_message_option_add(CoAPMessage *message, unsigned num, unsigned len, const char *val);

/**
 * @brief set msg callback
 *
 * @param message CoAP msg
 * @param callback
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int coap_message_callback_set(CoAPMessage *message, OnRespCallback callback);

/**
 * @brief set user context
 *
 * @param message CoAP msg
 * @param userContext
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int coap_message_context_set(CoAPMessage *message, void *userContext);

/**
 * @brief create CoAPMsgOption from option number/len/val
 *
 * @param num CoAP option number
 * @param len CoAP option string len
 * @param val CoAP option string value
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
CoAPMsgOption* qcloud_iot_coap_option_init(unsigned num, unsigned len, const char *val);

/**
 *  @brief destroy CoAPMessage
 *
 *  @param[in,out] message
 */
void coap_message_destroy(CoAPMessage *message);

/**
 * @brief Read and handle CoAP msg
 *
 * @param pClient CoAPClient
 * @param timeout_ms timeout value in millisecond
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int coap_message_cycle(CoAPClient *client, uint32_t timeout_ms);

/**
 * @brief Send CoAP msg
 *
 * @param client CoAPClient
 * @param message msg to send
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
ssize_t coap_message_send(CoAPClient *client, CoAPMessage *message);

/**
 * @brief Read CoAP msg
 *
 * @param client CoAPClient
 * @param timeout_ms timeout value in millisecond
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
ssize_t coap_message_recv(CoAPClient *client, uint32_t timeout_ms);

/**
 * @brief do CoAPClient auth
 *
 * @param client CoAPClient
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int  coap_client_auth(CoAPClient *client);

/**
 *  @brief Parse a message
 *
 *  @param[in,out] message Pointer to a message structure
 *  @param[in] buf Pointer to a buffer containing the message
 *  @param[in] len Length of the buffer
 *
 *  @returns Operation status
 *  @retval 0 Success
 *  @retval <0 Error
 */
ssize_t deserialize_coap_message(CoAPMessage *message, char *buf, size_t len);

/**
 *  @brief Format a message
 *
 *  @param[in] message Pointer to a message structure
 *  @param[out] buf Pointer to a buffer to contain the formatted message
 *  @param[in] len Length of the buffer
 *
 *  @returns Length of the formatted message or error code
 *  @retval >0 Length of the formatted message
 *  @retval <0 Error
 */
ssize_t serialize_coap_message(CoAPMessage *message, char *buf, size_t len);


void coap_msg_dump(CoAPMessage* msg);

#ifdef __cplusplus
}
#endif


#endif /* IOT_COAP_CLIENT_H_ */
