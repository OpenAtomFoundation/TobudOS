/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOT_EXPORT_MQTT_H_
#define _IOT_EXPORT_MQTT_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "infra_types.h"
#include "infra_defs.h"

#define MUTLI_SUBSCIRBE_MAX                                     (5)

/* From mqtt_client.h */
typedef enum {
    IOTX_MQTT_QOS0 = 0,
    IOTX_MQTT_QOS1,
    IOTX_MQTT_QOS2,
    IOTX_MQTT_QOS3_SUB_LOCAL
} iotx_mqtt_qos_t;

typedef enum {

    /* Undefined event */
    IOTX_MQTT_EVENT_UNDEF = 0,

    /* MQTT disconnect event */
    IOTX_MQTT_EVENT_DISCONNECT = 1,

    /* MQTT reconnect event */
    IOTX_MQTT_EVENT_RECONNECT = 2,

    /* A ACK to the specific subscribe which specify by packet-id be received */
    IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS = 3,

    /* No ACK to the specific subscribe which specify by packet-id be received in timeout period */
    IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT = 4,

    /* A failed ACK to the specific subscribe which specify by packet-id be received*/
    IOTX_MQTT_EVENT_SUBCRIBE_NACK = 5,

    /* A ACK to the specific unsubscribe which specify by packet-id be received */
    IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS = 6,

    /* No ACK to the specific unsubscribe which specify by packet-id be received in timeout period */
    IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT = 7,

    /* A failed ACK to the specific unsubscribe which specify by packet-id be received*/
    IOTX_MQTT_EVENT_UNSUBCRIBE_NACK = 8,

    /* A ACK to the specific publish which specify by packet-id be received */
    IOTX_MQTT_EVENT_PUBLISH_SUCCESS = 9,

    /* No ACK to the specific publish which specify by packet-id be received in timeout period */
    IOTX_MQTT_EVENT_PUBLISH_TIMEOUT = 10,

    /* A failed ACK to the specific publish which specify by packet-id be received*/
    IOTX_MQTT_EVENT_PUBLISH_NACK = 11,

    /* MQTT packet published from MQTT remote broker be received */
    IOTX_MQTT_EVENT_PUBLISH_RECEIVED = 12,

    /* MQTT packet buffer overflow which the remaining space less than to receive byte */
    IOTX_MQTT_EVENT_BUFFER_OVERFLOW = 13,
} iotx_mqtt_event_type_t;

/* topic information */
typedef struct {
    uint16_t        packet_id;
    uint8_t         qos;
    uint8_t         dup;
    uint8_t         retain;
    uint16_t        topic_len;
    uint32_t        payload_len;
    const char     *ptopic;
    const char     *payload;
} iotx_mqtt_topic_info_t, *iotx_mqtt_topic_info_pt;


typedef struct {

    /* Specify the event type */
    iotx_mqtt_event_type_t  event_type;

    /*
     * Specify the detail event information. @msg means different to different event types:
     *
     * 1) IOTX_MQTT_EVENT_UNKNOWN,
     *    IOTX_MQTT_EVENT_DISCONNECT,
     *    IOTX_MQTT_EVENT_RECONNECT :
     *      Its data type is string and the value is detail information.
     *
     * 2) IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS,
     *    IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT,
     *    IOTX_MQTT_EVENT_SUBCRIBE_NACK,
     *    IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS,
     *    IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT,
     *    IOTX_MQTT_EVENT_UNSUBCRIBE_NACK
     *    IOTX_MQTT_EVENT_PUBLISH_SUCCESS,
     *    IOTX_MQTT_EVENT_PUBLISH_TIMEOUT,
     *    IOTX_MQTT_EVENT_PUBLISH_NACK :
     *      Its data type is @uint32_t and the value is MQTT packet identifier.
     *
     * 3) IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
     *      Its data type is @iotx_mqtt_topic_info_pt and see detail at the declare of this type.
     *
     * */
    void *msg;
} iotx_mqtt_event_msg_t, *iotx_mqtt_event_msg_pt;


/**
 * @brief It define a datatype of function pointer.
 *        This type of function will be called when a related event occur.
 *
 * @param pcontext : The program context.
 * @param pclient : The MQTT client.
 * @param msg : The event message.
 *
 * @return none
 */
typedef void (*iotx_mqtt_event_handle_func_fpt)(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);


/* The structure of MQTT event handle */
typedef struct {
    iotx_mqtt_event_handle_func_fpt     h_fp;
    void                               *pcontext;
} iotx_mqtt_event_handle_t, *iotx_mqtt_event_handle_pt;


/* The structure of MQTT initial parameter */
typedef struct {

    uint16_t                    port;                   /* Specify MQTT broker port */
    const char                 *host;                   /* Specify MQTT broker host */
    const char                 *client_id;              /* Specify MQTT connection client id*/
    const char                 *username;               /* Specify MQTT user name */
    const char                 *password;               /* Specify MQTT password */
    const char                 *customize_info;         /* Specify User custom information */
    /* Specify MQTT transport channel and key.
     * If the value is NULL, it means that use TCP channel,
     * If the value is NOT NULL, it means that use SSL/TLS channel and
     *   @pub_key point to the CA certification */

    const char                 *pub_key;

    uint8_t                     clean_session;            /* Specify MQTT clean session or not*/
    uint32_t                    request_timeout_ms;       /* Specify timeout of a MQTT request in millisecond */
    uint32_t                    keepalive_interval_ms;    /* Specify MQTT keep-alive interval in millisecond */
    uint32_t                    write_buf_size;           /* Specify size of write-buffer in byte */
    uint32_t                    read_buf_size;            /* Specify size of read-buffer in byte */

    iotx_mqtt_event_handle_t    handle_event;             /* Specify MQTT event handle */

} iotx_mqtt_param_t, *iotx_mqtt_param_pt;

typedef enum {
    IOTX_MQTT_SOC_CONNECTED,
    IOTX_MQTT_SOC_CLOSE,
    IOTX_MQTT_SOC_READ,
    IOTX_MQTT_SOC_WRITE,
    IOTX_MQTT_SOC_MAX
} iotx_mqtt_nwk_event_t;

typedef struct {
    uintptr_t fd;
} iotx_mqtt_nwk_param_t;

/** @defgroup group_api api
 *  @{
 */

/** @defgroup group_api_mqtt mqtt
 *  @{
 */

/**
 * @brief Construct the MQTT client
 *        This function initialize the data structures, establish MQTT connection.
 *
 * @param [in] pInitParams: specify the MQTT client parameter.
 *
 * @retval     NULL : Construct failed.
 * @retval NOT_NULL : The handle of MQTT client.
 * @see None.
 */
void *IOT_MQTT_Construct(iotx_mqtt_param_t *pInitParams);


/**
 * @brief Deconstruct the MQTT client
 *        This function disconnect MQTT connection and release the related resource.
 *
 * @param [in] phandle: pointer of handle, specify the MQTT client.
 *
 * @retval  0 : Deconstruct success.
 * @retval -1 : Deconstruct failed.
 * @see None.
 */
int IOT_MQTT_Destroy(void **phandle);


/**
 * @brief Handle MQTT packet from remote server and process timeout request
 *        which include the MQTT subscribe, unsubscribe, publish(QOS >= 1), reconnect, etc..
 *
 * @param [in] handle: specify the MQTT client.
 * @param [in] timeout_ms: specify the timeout in millisecond in this loop.
 *
 * @return status.
 * @see None.
 */
int IOT_MQTT_Yield(void *handle, int timeout_ms);

/**
 * @brief check whether MQTT connection is established or not.
 *
 * @param [in] handle: specify the MQTT client.
 *
 * @retval true  : MQTT in normal state.
 * @retval false : MQTT in abnormal state.
 * @see None.
 */
int IOT_MQTT_CheckStateNormal(void *handle);


/**
 * @brief Subscribe MQTT topic.
 *
 * @param [in] handle: specify the MQTT client.
 * @param [in] topic_filter: specify the topic filter.
 * @param [in] qos: specify the MQTT Requested QoS.
 * @param [in] topic_handle_func: specify the topic handle callback-function.
 * @param [in] pcontext: specify context. When call 'topic_handle_func', it will be passed back.
 *
 * @retval -1  : Subscribe failed.
 * @retval >=0 : Subscribe successful.
          The value is a unique ID of this request.
          The ID will be passed back when callback 'iotx_mqtt_param_t:handle_event'.
 * @see None.
 */
int IOT_MQTT_Subscribe(void *handle,
                       const char *topic_filter,
                       iotx_mqtt_qos_t qos,
                       iotx_mqtt_event_handle_func_fpt topic_handle_func,
                       void *pcontext);

/**
 * @brief Subscribe MQTT topic and wait suback.
 *
 * @param [in] handle: specify the MQTT client.
 * @param [in] topic_filter: specify the topic filter.
 * @param [in] qos: specify the MQTT Requested QoS.
 * @param [in] topic_handle_func: specify the topic handle callback-function.
 * @param [in] pcontext: specify context. When call 'topic_handle_func', it will be passed back.
 * @param [in] timeout_ms: time in ms to wait.
 *
 * @retval -1  : Subscribe failed.
 * @retval >=0 : Subscribe successful.
          The value is a unique ID of this request.
          The ID will be passed back when callback 'iotx_mqtt_param_t:handle_event'.
 * @see None.
 */
int IOT_MQTT_Subscribe_Sync(void *handle,
                            const char *topic_filter,
                            iotx_mqtt_qos_t qos,
                            iotx_mqtt_event_handle_func_fpt topic_handle_func,
                            void *pcontext,
                            int timeout_ms);


/**
 * @brief Unsubscribe MQTT topic.
 *
 * @param [in] handle: specify the MQTT client.
 * @param [in] topic_filter: specify the topic filter.
 *
 * @retval -1  : Unsubscribe failed.
 * @retval >=0 : Unsubscribe successful.
          The value is a unique ID of this request.
          The ID will be passed back when callback 'iotx_mqtt_param_t:handle_event'.
 * @see None.
 */
int IOT_MQTT_Unsubscribe(void *handle, const char *topic_filter);


/**
 * @brief Publish message to specific topic.
 *
 * @param [in] handle: specify the MQTT client.
 * @param [in] topic_name: specify the topic name.
 * @param [in] topic_msg: specify the topic message.
 *
 * @retval -1 :  Publish failed.
 * @retval  0 :  Publish successful, where QoS is 0.
 * @retval >0 :  Publish successful, where QoS is >= 0.
        The value is a unique ID of this request.
        The ID will be passed back when callback 'iotx_mqtt_param_t:handle_event'.
 * @see None.
 */
int IOT_MQTT_Publish(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg);
/**
 * @brief Publish message to specific topic.
 *
 * @param [in] handle: specify the MQTT client.
 * @param [in] topic_name: specify the topic name.
 * @param [in] qos: specify the MQTT Requested QoS.
 * @param [in] data: specify the topic message payload.
 * @param [in] len: specify the topic message payload len.
 *
 * @retval -1 :  Publish failed.
 * @retval  0 :  Publish successful, where QoS is 0.
 * @retval >0 :  Publish successful, where QoS is >= 0.
        The value is a unique ID of this request.
        The ID will be passed back when callback 'iotx_mqtt_param_t:handle_event'.
 * @see None.
 */
int IOT_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len);
/* From mqtt_client.h */
/** @} */ /* end of api_mqtt */

/** @} */ /* end of api */

/**
 * @brief Only used in async network stack and FEATURE_ASYNC_PROTOCOL_STACK must be selected
 *
 * @param [in] handle: specify the MQTT client.
 * @param [in] event: specify the network event.
 * @param [in] param: specify the network params.
 *
 * @retval -1 :  Handle failed.
 * @retval  0 :  Handle successful.
 *
 */
int IOT_MQTT_Nwk_Event_Handler(void *handle, iotx_mqtt_nwk_event_t event, iotx_mqtt_nwk_param_t *param);

/* MQTT Configurations
 *
 * These switches will affect mqtt_api.c and IOT_MQTT_XXX() functions' behaviour
 *
 */

/* Default message length in bytes when PLATFORM_HAS_DYNMEM is not set */
#define CONFIG_MQTT_MESSAGE_MAXLEN              (1024)

/* Default maximum length of topic name in byte when PLATFORM_HAS_DYNMEM is not set */
#ifdef PLATFORM_HAS_DYNMEM
#define CONFIG_MQTT_TOPIC_MAXLEN                (128)
#else
#define CONFIG_MQTT_TOPIC_MAXLEN                (50)
#endif

/* Default keepalive interval of MQTT request in second */
#define CONFIG_MQTT_KEEPALIVE_INTERVAL          (60)


/* Default offline subscribe list max length when PLATFORM_HAS_DYNMEM is not set */
#ifndef CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM
#define CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM        (5)
#endif

/* Default timeout interval of MQTT request in millisecond */
#define CONFIG_MQTT_REQUEST_TIMEOUT             (2000)

/* Minimum timeout interval of MQTT request in millisecond */
#define CONFIG_MQTT_REQ_TIMEOUT_MIN             (500)

/* Maximum timeout interval of MQTT request in millisecond */
#define CONFIG_MQTT_REQ_TIMEOUT_MAX             (5000)

/* Minimum keepalive interval of MQTT request in second */
#define CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN      (30)

/* Maximum keepalive interval of MQTT request in second */
#define CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX      (1200)

#if defined(__cplusplus)
}
#endif
#endif


