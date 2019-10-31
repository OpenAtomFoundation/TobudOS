/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOTX_COAP_API_H__
#define __IOTX_COAP_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "infra_defs.h"

/*iotx return code definition*/
typedef enum {
    IOTX_ERR_RECV_MSG_TIMEOUT = -9,   /*Receive message timeout */
    IOTX_ERR_SEND_MSG_FAILED =  -8,   /* Send message failed*/
    IOTX_ERR_MSG_TOO_LOOG    =  -7,   /* The payload too loog */
    IOTX_ERR_URI_TOO_LOOG    =  -6,   /* URI length too long */
    IOTX_ERR_NOT_AUTHED      =  -5,   /* Client isn't authed */
    IOTX_ERR_AUTH_FAILED     =  -4,   /* Client authed failed */
    IOTX_ERR_BUFF_TOO_SHORT  =  -3,   /* Buffer too short */
    IOTX_ERR_NO_MEM          =  -2,   /* Malloc failed */
    IOTX_ERR_INVALID_PARAM   =  -1,   /* Invalid parameter */
    IOTX_SUCCESS             =   0,   /* Success */
} iotx_ret_code_t;

/* The message payload encode format */
typedef enum {
    IOTX_CONTENT_TYPE_JSON,
    IOTX_CONTENT_TYPE_CBOR,
} iotx_content_type_t;

/* The message type */
typedef enum {
    IOTX_MESSAGE_CON  = 0,       /* confirmable message */
    IOTX_MESSAGE_NON  = 1,       /* non-confirmable message */
    IOTX_MESSAGE_ACK  = 2,       /* acknowledgement message */
    IOTX_MESSAGE_RST  = 3,       /* reset message */
} iotx_msg_type_t;

/* IoTx events to notify application */
typedef enum {
    IOTX_COAP_EVENT_SEND_FAILED = 0,
    IOTX_COAP_EVENT_RECV_FAILED = 1,
    IOTX_COAP_EVENT_AUTH_FAILED = 2,
} iotx_coap_event_t;

typedef enum {
    IOTX_COAP_RESP_CODE_CONTENT        = 0x45,       /* Mapping to 2.05, Content*/
    IOTX_COAP_RESP_CODE_BAD_REQUEST    = 0x80,       /* Mapping to 4.00, Bad Request*/
    IOTX_COAP_RESP_CODE_UNAUTHORIZED   = 0x81,       /* Mapping to 4.01, Token is invalid or expire*/
    IOTX_COAP_RESP_CODE_NOT_FOUND      = 0x84,       /* Mapping to 4.04, Path or uri is not found*/
    IOTX_COAP_RESP_CODE_URL_TOO_LONG   = 0x8E,       /* Mapping to 4.14, The request url is too long*/
    IOTX_COAP_RESP_CODE_INTERNAL_SERVER_ERROR = 0xA0,/* Mapping to 5.00, Internal server error*/

} iotx_coap_resp_code_t;

/* Callback function to notify the application events.*/
typedef void (*iotx_event_handle_t)(void *context, iotx_coap_event_t event, void *p_data);

/*IoTx device*/
typedef struct {
    char     product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char     device_name[IOTX_DEVICE_NAME_LEN + 1];
    char     device_id[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 2];
    char     device_secret[IOTX_DEVICE_SECRET_LEN + 1];
} iotx_deviceinfo_t;

typedef struct {
    char        product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char        device_name[IOTX_DEVICE_NAME_LEN + 1];
    char        device_id[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 2];
    char        device_secret[IOTX_DEVICE_SECRET_LEN + 1];
    char        module_vendor_id[IOTX_PARTNER_ID_LEN + 1];
} iotx_device_info_t;

/* IoTx initializa parameters */
typedef struct {
    char                 *p_url;        /*Can be NULL*/
    int                   wait_time_ms; /*unit is micro second*/
    iotx_device_info_t   *p_devinfo;    /*Device info*/
    iotx_event_handle_t   event_handle; /*TODO, not supported now*/
} iotx_coap_config_t;

/* Callback function to handle the response message.*/
typedef void (*iotx_response_callback_t)(void *p_arg, void *p_message);

/* IoTx message definition */
typedef struct {
    unsigned char           *p_payload;
    unsigned short           payload_len;
    iotx_content_type_t      content_type;
    iotx_msg_type_t          msg_type;
    void                    *user_data;
    iotx_response_callback_t resp_callback;
} iotx_message_t;


/*iotx coap context definition*/
typedef void iotx_coap_context_t;


/** @defgroup group_api api
 *  @{
 */

/** @defgroup group_api_coap coap
 *  @{
 */

/**
 * @brief   Initialize the CoAP client.
 *        This function initialize the data structures and network,
 *        and create the DTLS session.
 *
 * @param [in] p_config: Specify the CoAP client parameter.
 *
 * @retval NULL : Initialize failed.
 * @retval NOT_NULL : The contex of CoAP client.
 * @see None.
 */
iotx_coap_context_t *IOT_CoAP_Init(iotx_coap_config_t *p_config);

/**
 * @brief   De-initialize the CoAP client.
 *        This function release CoAP DTLS session.
 *        and release the related resource.
 *
 * @param [in] p_context: Pointer of contex, specify the CoAP client.
 *
 * @return None.
 * @see None.
 */
void IOT_CoAP_Deinit(iotx_coap_context_t **p_context);


/**
 * @brief   Handle device name authentication with remote server.
 *
 * @param [in] p_context: Pointer of contex, specify the CoAP client.
 *
 * @retval IOTX_SUCCESS             : Authenticate success.
 * @retval IOTX_ERR_SEND_MSG_FAILED : Send authentication message failed.
 * @retval IOTX_ERR_AUTH_FAILED     : Authenticate failed or timeout.
 * @see iotx_ret_code_t.
 */
int  IOT_CoAP_DeviceNameAuth(iotx_coap_context_t *p_context);


/**
 * @brief   Handle CoAP response packet from remote server,
 *        and process timeout request etc..
 *
 * @param [in] p_context : Pointer of contex, specify the CoAP client.
 *
 * @return status.
 * @see iotx_ret_code_t.
 */
int  IOT_CoAP_Yield(iotx_coap_context_t *p_context);


/**
 * @brief   Send a message with specific path to server.
 *        Client must authentication with server before send message.
 *
 * @param [in] p_context : Pointer of contex, specify the CoAP client.
 * @param [in] p_path: Specify the path name.
 * @param [in] p_message: Message to be sent.
 *
 * @retval IOTX_SUCCESS             : Send the message success.
 * @retval IOTX_ERR_MSG_TOO_LOOG    : The message length is too long.
 * @retval IOTX_ERR_NOT_AUTHED      : The client hasn't authenticated with server
 * @see iotx_ret_code_t.
 */
int  IOT_CoAP_SendMessage(iotx_coap_context_t *p_context,   char *p_path, iotx_message_t *p_message);

/**
* @brief Retrieves the length and payload pointer of specified message.
*
* @param  [in] p_message: Pointer to the message to get the payload. Should not be NULL.
* @param  [out] pp_payload: Pointer to the payload.
* @param  [out] p_len: Size of the payload.
*
* @retval IOTX_SUCCESS              : Get the payload success.
* @retval IOTX_ERR_INVALID_PARAM    : Can't get the payload due to invalid parameter.
* @see iotx_ret_code_t.
**/
int  IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload, int *p_len);

/**
* @brief Get the response code from a CoAP message.
*
* @param [in] p_message: Pointer to the message to add the address information to.
*                            Should not be NULL.
* @param [out] p_resp_code: The response code.
*
* @retval  IOTX_SUCCESS             : When get the response code to message success.
* @retval  IOTX_ERR_INVALID_PARAM   : Pointer to the message is NULL.
* @see iotx_ret_code_t.
**/
int  IOT_CoAP_GetMessageCode(void *p_message, iotx_coap_resp_code_t *p_resp_code);

/** @} */ /* end of api_coap */
/** @} */ /* end of api */

#if defined(__cplusplus)
}
#endif
#endif
