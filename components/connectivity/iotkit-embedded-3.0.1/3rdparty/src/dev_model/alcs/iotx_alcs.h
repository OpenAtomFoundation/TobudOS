/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _IOT_EXPORT_ALCS_BACKUP_H_
#define _IOT_EXPORT_ALCS_BACKUP_H_

#define IOTX_ALCS_ROLE_CLIENT (0x01)
#define IOTX_ALCS_ROLE_SERVER (0x02)

#define ALCS_MSG_MAX_TOKEN_LEN (8)

typedef enum {

    /*iotx_alcs_send Message Result And Receive Message*/
    IOTX_ALCS_EVENT_MSG_SEND_MESSAGE_SUCCESS,
    IOTX_ALCS_EVENT_MSG_SEND_MESSAGE_RESP_TIMEOUT,
    IOTX_ALCS_EVENT_MSG_RECV_MESSAGE,
    /*Its data type is @iotx_alcs_transfer_msg_t and see detail at the declare of this type.*/

} iotx_alcs_event_type_t;

typedef struct iotx_alcs_event_msg_st {

    /* Specify the event type */
    iotx_alcs_event_type_t  event_type;

    void *msg;
} iotx_alcs_event_msg_t, *iotx_alcs_event_msg_pt;

typedef struct iotx_alcs_transfer_msg_st {
    char *ip;
    uint16_t port;
    char *uri;
    uint8_t token_len;
    uint8_t *token;
    uint16_t payload_len;
    uint8_t *payload;
} iotx_alcs_transfer_msg_t, *iotx_alcs_transfer_msg_pt;

typedef void (*iotx_alcs_event_handle_func_fpt)(void *pcontext, void *phandle, iotx_alcs_event_msg_t *msg);

typedef struct {
    iotx_alcs_event_handle_func_fpt h_fp;
    void *pcontext;
} iotx_alcs_event_handle_t;

typedef struct iotx_alcs_param_st {
    uint8_t role;                  /*can be client, server or both*/

    uint8_t        send_maxcount;  /*list maximal count*/
    uint8_t        obs_maxcount;   /*observe maximal count*/
    uint16_t       port;           /* Local port */
    char           *group;         /* Multicast address */
    uint32_t       waittime;
    uint8_t        res_maxcount;

    iotx_alcs_event_handle_t *handle_event;
} iotx_alcs_param_t, *iotx_alcs_param_pt;

#define ITOX_ALCS_COAP_MSG_CODE_DEF(N) (((N)/100 << 5) | (N)%100)

/*CoAP Message codes*/
typedef enum {
    /* CoAP Empty Message */
    ITOX_ALCS_COAP_MSG_CODE_EMPTY_MESSAGE                  = ITOX_ALCS_COAP_MSG_CODE_DEF(0),  /* Mapping to CoAP code 0.00 */

    /* CoAP Method Codes */
    ITOX_ALCS_COAP_MSG_CODE_GET                            = ITOX_ALCS_COAP_MSG_CODE_DEF(1),  /* CoAP Get method */
    ITOX_ALCS_COAP_MSG_CODE_POST                           = ITOX_ALCS_COAP_MSG_CODE_DEF(2),  /* CoAP Post method */
    ITOX_ALCS_COAP_MSG_CODE_PUT                            = ITOX_ALCS_COAP_MSG_CODE_DEF(3),  /* CoAP Put method */
    ITOX_ALCS_COAP_MSG_CODE_DELETE                         = ITOX_ALCS_COAP_MSG_CODE_DEF(4),  /* CoAP Delete method */

    /* CoAP Success Response Codes */
    ITOX_ALCS_COAP_MSG_CODE_201_CREATED                    = ITOX_ALCS_COAP_MSG_CODE_DEF(201),  /* Mapping to CoAP code 2.01, Hex:0x41, Created */
    ITOX_ALCS_COAP_MSG_CODE_202_DELETED                    = ITOX_ALCS_COAP_MSG_CODE_DEF(202),  /* Mapping to CoAP code 2.02, Hex:0x42, Deleted*/
    ITOX_ALCS_COAP_MSG_CODE_203_VALID                      = ITOX_ALCS_COAP_MSG_CODE_DEF(203),  /* Mapping to CoAP code 2.03, Hex:0x43, Valid*/
    ITOX_ALCS_COAP_MSG_CODE_204_CHANGED                    = ITOX_ALCS_COAP_MSG_CODE_DEF(204),  /* Mapping to CoAP code 2.04, Hex:0x44, Changed*/
    ITOX_ALCS_COAP_MSG_CODE_205_CONTENT                    = ITOX_ALCS_COAP_MSG_CODE_DEF(205),  /* Mapping to CoAP code 2.05, Hex:0x45, Content*/
    ITOX_ALCS_COAP_MSG_CODE_231_CONTINUE                   = ITOX_ALCS_COAP_MSG_CODE_DEF(231),  /* Mapping to CoAP code 2.31, Hex:0x5F, Continue*/

    /* CoAP Client Error Response Codes */
    ITOX_ALCS_COAP_MSG_CODE_400_BAD_REQUEST                = ITOX_ALCS_COAP_MSG_CODE_DEF(400),  /* Mapping to CoAP code 4.00, Hex:0x80, Bad Request */
    ITOX_ALCS_COAP_MSG_CODE_401_UNAUTHORIZED               = ITOX_ALCS_COAP_MSG_CODE_DEF(401),  /* Mapping to CoAP code 4.01, Hex:0x81, Unauthorized */
    ITOX_ALCS_COAP_MSG_CODE_402_BAD_OPTION                 = ITOX_ALCS_COAP_MSG_CODE_DEF(402),  /* Mapping to CoAP code 4.02, Hex:0x82, Bad Option */
    ITOX_ALCS_COAP_MSG_CODE_403_FORBIDDEN                  = ITOX_ALCS_COAP_MSG_CODE_DEF(403),  /* Mapping to CoAP code 4.03, Hex:0x83, Forbidden */
    ITOX_ALCS_COAP_MSG_CODE_404_NOT_FOUND                  = ITOX_ALCS_COAP_MSG_CODE_DEF(404),  /* Mapping to CoAP code 4.04, Hex:0x84, Not Found */
    ITOX_ALCS_COAP_MSG_CODE_405_METHOD_NOT_ALLOWED         = ITOX_ALCS_COAP_MSG_CODE_DEF(405),  /* Mapping to CoAP code 4.05, Hex:0x85, Method Not Allowed */
    ITOX_ALCS_COAP_MSG_CODE_406_NOT_ACCEPTABLE             = ITOX_ALCS_COAP_MSG_CODE_DEF(406),  /* Mapping to CoAP code 4.06, Hex:0x86, Not Acceptable */
    ITOX_ALCS_COAP_MSG_CODE_408_REQUEST_ENTITY_INCOMPLETE  = ITOX_ALCS_COAP_MSG_CODE_DEF(408),  /* Mapping to CoAP code 4.08, Hex:0x88, Request Entity Incomplete */
    ITOX_ALCS_COAP_MSG_CODE_412_PRECONDITION_FAILED        = ITOX_ALCS_COAP_MSG_CODE_DEF(412),  /* Mapping to CoAP code 4.12, Hex:0x8C, Precondition Failed */
    ITOX_ALCS_COAP_MSG_CODE_413_REQUEST_ENTITY_TOO_LARGE   = ITOX_ALCS_COAP_MSG_CODE_DEF(413),  /* Mapping to CoAP code 4.13, Hex:0x8D, Request Entity Too Large */
    ITOX_ALCS_COAP_MSG_CODE_415_UNSUPPORTED_CONTENT_FORMAT = ITOX_ALCS_COAP_MSG_CODE_DEF(415),  /* Mapping to CoAP code 4.15, Hex:0x8F, Unsupported Content-Format */

    /* CoAP Server Error Response Codes */
    ITOX_ALCS_COAP_MSG_CODE_500_INTERNAL_SERVER_ERROR      = ITOX_ALCS_COAP_MSG_CODE_DEF(500),  /* Mapping to CoAP code 5.00, Hex:0xA0, Internal Server Error */
    ITOX_ALCS_COAP_MSG_CODE_501_NOT_IMPLEMENTED            = ITOX_ALCS_COAP_MSG_CODE_DEF(501),  /* Mapping to CoAP code 5.01, Hex:0xA1, Not Implemented */
    ITOX_ALCS_COAP_MSG_CODE_502_BAD_GATEWAY                = ITOX_ALCS_COAP_MSG_CODE_DEF(502),  /* Mapping to CoAP code 5.02, Hex:0xA2, Bad Gateway */
    ITOX_ALCS_COAP_MSG_CODE_503_SERVICE_UNAVAILABLE        = ITOX_ALCS_COAP_MSG_CODE_DEF(503),  /* Mapping to CoAP code 5.03, Hex:0xA3, Service Unavailable */
    ITOX_ALCS_COAP_MSG_CODE_504_GATEWAY_TIMEOUT            = ITOX_ALCS_COAP_MSG_CODE_DEF(504),  /* Mapping to CoAP code 5.04, Hex:0xA4, Gateway Timeout */
    ITOX_ALCS_COAP_MSG_CODE_505_PROXYING_NOT_SUPPORTED     = ITOX_ALCS_COAP_MSG_CODE_DEF(505)   /* Mapping to CoAP code 5.05, Hex:0xA5, Proxying Not Supported */

} iotx_alcs_message_code_t;

typedef enum {
    IOTX_ALCS_MESSAGE_TYPE_CON = 0,
    IOTX_ALCS_MESSAGE_TYPE_NON = 1,
    IOTX_ALCS_MESSAGE_TYPE_ACK = 2,
    IOTX_ALCS_MESSAGE_TYPE_RST = 3
} iotx_alcs_message_type_t;

typedef enum {
    IOTX_ALCS_MESSAGE_PERM_NONE    = 0x0000,
    IOTX_ALCS_MESSAGE_PERM_GET     = 0x0001,
    IOTX_ALCS_MESSAGE_PERM_POST    = 0x0002,
    IOTX_ALCS_MESSAGE_PERM_PUT     = 0x0004,
    IOTX_ALCS_MESSAGE_PERM_DEL     = 0x0008,
    IOTX_ALCS_MESSAGE_PERM_OBSERVE = 0x0100
} iotx_alcs_message_perm_t;

typedef enum {
    IOTX_ALCS_MESSAGE_CT_TEXT_PLAIN                = 0,    /* text/plain (UTF-8) */
    IOTX_ALCS_MESSAGE_CT_APP_LINK_FORMAT           = 40,   /* application/link-format */
    IOTX_ALCS_MESSAGE_CT_APP_XML                   = 41,   /* application/xml */
    IOTX_ALCS_MESSAGE_CT_APP_OCTET_STREAM          = 42,   /* application/octet-stream */
    IOTX_ALCS_MESSAGE_CT_APP_RDF_XML               = 43,   /* application/rdf+xml */
    IOTX_ALCS_MESSAGE_CT_APP_EXI                   = 47,   /* application/exi  */
    IOTX_ALCS_MESSAGE_CT_APP_JSON                  = 50,   /* application/json  */
    IOTX_ALCS_MESSAGE_CT_APP_CBOR                  = 60   /* application/cbor  */
} iotx_alcs_message_content_type_t;

typedef struct iotx_alcs_msg_st {
    uint16_t group_id; /*multicast group id, used as unicast when 0*/
    char *ip;   /*dotted decimal notation, max len 16*/
    uint16_t port;
    iotx_alcs_message_code_t msg_code;
    iotx_alcs_message_type_t msg_type;
    char *uri;
    uint32_t payload_len;
    uint8_t *payload;
} iotx_alcs_msg_t, *iotx_alcs_msg_pt;

typedef struct iotx_alcs_res_st {
    char *uri;
    int32_t need_auth;
    iotx_alcs_message_perm_t msg_perm;
    iotx_alcs_message_content_type_t msg_ct;
    uint32_t maxage;  /*0~60*/
    CoAPRecvMsgHandler callback;
} iotx_alcs_res_t, *iotx_alcs_res_pt;

/**
 * @brief Construct the ALCS handle
 *        This function initialize the data structures, initialize ALCS information.
 *
 * @param [in] params: specify the ALCS initialize parameter.
 *
 * @retval     NULL : Construct failed.
 * @retval NOT_NULL : The handle of ALCS.
 * @see None.
 */
void *iotx_alcs_construct(iotx_alcs_param_t *params);

/**
 * @brief Init Cloud Part
 *        This function initialize the cloud part.
 *
 * @param [in] params: specify the ALCS initialize parameter.
 *
 * @retval     NULL : Construct failed.
 * @retval NOT_NULL : The handle of ALCS.
 * @see None.
 */
int iotx_alcs_cloud_init(void *handle);

/**
 * @brief Deconstruct the ALCS handle
 *        This function distroy ALCS handle and release the related resource.
 *
 * @param [in] phandle: pointer of handle, specify the MQTT client.
 *
 * @retval  0 : Deconstruct success.
 * @retval -1 : Deconstruct failed.
 * @see None.
 */
int iotx_alcs_destroy(void **phandle);

/**
 * @brief Handle ALCS message from specific udp port
 *
 * @param [in] handle: specify the ALCS handle.
 *
 * @return status.
 * @see None.
 */
int iotx_alcs_yield(void *handle);

/**
 * @brief Send Message To Secific Deivce
 *
 * @param [in] handle: specify the ALCS handle.
 *
 * @return status.
 * @see None.
 */
int iotx_alcs_send(void *handle, iotx_alcs_msg_t *msg);

/**
 * @brief Send Response Message To Secific Deivce
 *
 * @param [in] handle: specify the ALCS handle.
 *
 * @return status.
 * @see None.
 */
int iotx_alcs_send_Response(void *handle, iotx_alcs_msg_t *msg, uint8_t token_len, uint8_t *token);

/**
 * @brief Register Resource
 *
 * @param [in] handle: specify the ALCS handle.
 * @param [in] handle: the resource need to be registered.
 *
 * @return status.
 * @see None.
 */
int iotx_alcs_register_resource(void *handle, iotx_alcs_res_t *resource);

/**
 * @brief ALCS Observe Notify
 *
 * @param [in] handle: specify the ALCS handle.
 * @param [in] uri: the resource need to notify.
 *
 * @return status.
 * @see None.
 */
int iotx_alcs_observe_notify(void *handle, const char *uri, uint32_t payload_len, uint8_t *payload);

/**
 * @brief Unregister Resource
 *
 * @param [in] handle: specify the ALCS handle.
 * @param [in] handle: the resource need to be registered.
 *
 * @return status.
 * @see None.
 */
int iotx_alcs_unregister_resource(void *handle, char *uri);

/**
 * @brief Add sub device
 *
 * @param [in] handle: specify the ALCS handle.
 * @param [in] pk: the productkey of device.
 * @param [in] dn: the deviceName of device.
 *
 * @return status.
 * @see None.
 */
int iotx_alcs_add_sub_device(void *handle, const char *pk, const char *dn);

/**
 * @brief remove sub device
 *
 * @param [in] handle: specify the ALCS handle.
 * @param [in] pk: the productkey of device.
 * @param [in] dn: the deviceName of device.
 *
 * @return status.
 * @see None.
 */
int iotx_alcs_remove_sub_device(void *handle, const char *pk, const char *dn);

#endif
