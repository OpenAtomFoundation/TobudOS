#include "iotx_cm_internal.h"

#ifdef COAP_COMM_ENABLED
#include "iotx_cm.h"
#include "iotx_cm_coap.h"
#include "infra_timer.h"

#ifdef COAP_DTLS_SUPPORT  /* DTLS */
    #ifdef ON_DAILY
        #define IOTX_COAP_SERVER_URI      "coaps://11.239.164.238:5684"
    #else
        #ifdef ON_PRE
            #define IOTX_COAP_SERVER_URI      "coaps://pre.coap.cn-shanghai.link.aliyuncs.com:5684"

        #else /* online */
            #define IOTX_COAP_SERVER_URI      "coaps://%s.coap.cn-shanghai.link.aliyuncs.com:5684"
        #endif
    #endif

#else
    #ifdef COAP_PSK_SUPPORT  /* PSK */
        #ifdef ON_DAILY
            #define IOTX_COAP_SERVER_URI      "coap-psk://10.101.83.159:5682"
        #else
            #ifdef ON_PRE
                #define IOTX_COAP_SERVER_URI      "coap-psk://pre.coap.cn-shanghai.link.aliyuncs.com:5682"
            #else /* online */
                #define IOTX_COAP_SERVER_URI      "coap-psk://%s.coap.cn-shanghai.link.aliyuncs.com:5682"
            #endif
        #endif
    #else                 /* UDP */
        #ifdef ON_DAILY
            #define IOTX_COAP_SERVER_URI      ""
        #else
            #ifdef ON_PRE
                #define IOTX_COAP_SERVER_URI      "coap://pre.iot-as-coap.cn-shanghai.aliyuncs.com:5683"
            #else /* online */
                #define IOTX_COAP_SERVER_URI      "coap://%s.coap.cn-shanghai.link.aliyuncs.com:5683"
            #endif
        #endif

    #endif
#endif

extern uint32_t IOT_CoAP_GetCurToken(iotx_coap_context_t *p_context);
int  IOT_CoAP_GetMessageToken(void *p_message, unsigned int *token);
static struct list_head g_coap_response_list = LIST_HEAD_INIT(g_coap_response_list);

static iotx_cm_connection_t *_coap_conncection = NULL;
static int iotx_set_devinfo(iotx_device_info_t *p_devinfo);

static int  _coap_connect(uint32_t timeout);
static int _coap_publish(iotx_cm_ext_params_t *params, const char *topic, const char *payload,
                         unsigned int payload_len);
static int _coap_sub(iotx_cm_ext_params_t *params, const char *topic,
                     iotx_cm_data_handle_cb topic_handle_func, void *pcontext);
static iotx_msg_type_t _get_coap_qos(iotx_cm_ack_types_t ack_type);
static int _coap_unsub(const char *topic);
static int _coap_close();
static void _set_common_handlers();

iotx_cm_connection_t *iotx_cm_open_coap(iotx_cm_init_param_t *params)
{
    iotx_coap_config_t      *coap_config = NULL;
    iotx_device_info_t       *deviceinfo = NULL;

    if (_coap_conncection != NULL) {
        cm_warning("mqtt connection is opened already,return it");
        return _coap_conncection;
    }

    _coap_conncection = (iotx_cm_connection_t *)cm_malloc(sizeof(iotx_cm_connection_t));
    if (_coap_conncection == NULL) {
        cm_err("_coap_conncection malloc failed!");
        goto failed;
    }

    _coap_conncection->list_lock = HAL_MutexCreate();
    if (_coap_conncection->list_lock == NULL) {
        cm_err("list_lock create failed!");
        goto failed;
    }

    coap_config = (iotx_coap_config_t *)cm_malloc(sizeof(iotx_coap_config_t));
    if (coap_config == NULL) {
        cm_err("coap_config malloc failed!");
        goto failed;
    }
    memset(coap_config, 0, sizeof(iotx_coap_config_t));
    deviceinfo = (iotx_device_info_t *)cm_malloc(sizeof(iotx_device_info_t));
    if (deviceinfo == NULL) {
        cm_err("deviceinfo malloc failed!");
        goto failed;
    }

    _coap_conncection->open_params = coap_config;

    memset(deviceinfo, 0, sizeof(iotx_device_info_t));

    iotx_set_devinfo(deviceinfo);
    coap_config->wait_time_ms = params->request_timeout_ms;
    coap_config->p_devinfo = deviceinfo;
    /* coap_config->p_url = IOTX_COAP_SERVER_URI; */

    _coap_conncection->event_handler = params->handle_event;

    _set_common_handlers();

    return _coap_conncection;

failed:
    if (_coap_conncection != NULL) {
        if (_coap_conncection->list_lock != NULL) {
            HAL_MutexDestroy(_coap_conncection->list_lock);
        }
        cm_free(_coap_conncection);
        _coap_conncection = NULL;
    }

    if (coap_config != NULL) {
        cm_free(coap_config);
    }
    if (deviceinfo != NULL) {
        cm_free(deviceinfo);
    }

    return NULL;
}

static int iotx_set_devinfo(iotx_device_info_t *p_devinfo)
{
    if (NULL == p_devinfo) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(p_devinfo, 0x00, sizeof(iotx_device_info_t));

    /**< get device info*/
    HAL_GetProductKey(p_devinfo->product_key);
    HAL_GetDeviceName(p_devinfo->device_name);
    HAL_GetDeviceSecret(p_devinfo->device_secret);
    HAL_Snprintf(p_devinfo->device_id, IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 2, "%s.%s", p_devinfo->product_key, p_devinfo->device_name);
    p_devinfo->device_id[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 1] = '\0';
    /**< end*/
    cm_info("*****The Product Key  : %s *****\r\n", p_devinfo->product_key);
    cm_info("*****The Device Name  : %s *****\r\n", p_devinfo->device_name);
    cm_info("*****The Device Secret: %s *****\r\n", p_devinfo->device_secret);
    cm_info("*****The Device ID    : %s *****\r\n", p_devinfo->device_id);
    return IOTX_SUCCESS;
}

static int  _coap_connect(uint32_t timeout)
{
    int ret;
    char url[100] = {0};
    iotx_time_t timer;
    iotx_coap_config_t *config = NULL;
    iotx_coap_context_t *p_ctx = NULL;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};

    if (_coap_conncection == NULL) {
        return NULL_VALUE_ERROR;
    }

    HAL_GetProductKey(product_key);
    config = _coap_conncection->open_params;
    if (config == NULL) {
        return NULL_VALUE_ERROR;
    }

    HAL_Snprintf(url, 100, IOTX_COAP_SERVER_URI, product_key);
    config->p_url = url;

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout);
    do {
        if (p_ctx == NULL) {
            p_ctx = IOT_CoAP_Init(config);
            if (NULL == p_ctx) {
                continue;
            }
        }
        ret = IOT_CoAP_DeviceNameAuth(p_ctx);
        if (ret == 0) {
            iotx_cm_event_msg_t event;
            event.type = IOTX_CM_EVENT_CLOUD_CONNECTED;
            event.msg = NULL;
            _coap_conncection->context = p_ctx;

            if (_coap_conncection->event_handler) {
                _coap_conncection->event_handler(_coap_conncection->fd, &event, (void *)_coap_conncection);
            }
            return 0;
        }
    } while (!utils_time_is_expired(&timer));

    {
        iotx_cm_event_msg_t event;
        event.type = IOTX_CM_EVENT_CLOUD_CONNECT_FAILED;
        event.msg = NULL;

        if (_coap_conncection->event_handler) {
            _coap_conncection->event_handler(_coap_conncection->fd, &event, (void *)_coap_conncection);
        }
    }
    cm_err("mqtt connect failed");
    return -1;
}

static void _coap_response_default(void *p_arg, void *p_message)
{
    int ret;
    int len = 0;
    unsigned char *p_payload = NULL;
    unsigned int token;
    iotx_coap_resp_code_t resp_code;
    coap_response_node_t *node = NULL;
    coap_response_node_t *next = NULL;

    if (_coap_conncection == NULL || p_message == NULL) {
        cm_err("paras err");
        return;
    }

    ret = IOT_CoAP_GetMessageCode(p_message, &resp_code);
    if (ret < 0) {
        cm_err("get msg code err");
        return;
    }

    cm_info("resp_code = %d", resp_code);

    ret = IOT_CoAP_GetMessagePayload(p_message, &p_payload, &len);
    if (ret < 0) {
        cm_err("get msg payload err");
        return;
    }

    ret = IOT_CoAP_GetMessageToken(p_message, &token);
    if (ret < 0) {
        cm_err("get msg token err");
        return;
    }

    HAL_MutexLock(_coap_conncection->list_lock);
    list_for_each_entry_safe(node, next, &g_coap_response_list, linked_list, coap_response_node_t) {
        if (node->token_num == token) {
            iotx_cm_data_handle_cb recieve_cb = node->responce_cb;
            void *context = node->context;
            unsigned int topic_len = strlen(node->topic) + 1;
            char *topic = cm_malloc(topic_len);
            if (topic == NULL) {
                cm_err("topic malloc failed");
                continue;
            }
            memset(topic, 0, topic_len);
            strncpy(topic, node->topic, topic_len);
            list_del(&node->linked_list);
            cm_free(node->topic);
            cm_free(node);
            HAL_MutexUnlock(_coap_conncection->list_lock); /* do not lock while callback */

            recieve_cb(_coap_conncection->fd, topic, (const char *)p_payload, len, context);
            /* recieve_cb(_coap_conncection->fd, &msg, context); */
            cm_free(topic);
            HAL_MutexLock(_coap_conncection->list_lock);
        }
    }
    HAL_MutexUnlock(_coap_conncection->list_lock);
}


static int _coap_publish(iotx_cm_ext_params_t *ext, const char *topic, const char *payload, unsigned int payload_len)
{
    iotx_msg_type_t qos = 0;
    iotx_message_t     message;
    uint32_t token;
    int topic_len;
    int ret;

    if (_coap_conncection == NULL) {
        return NULL_VALUE_ERROR;
    }

    if (ext != NULL) {
        qos = _get_coap_qos(ext->ack_type);
    }
    memset(&message, 0, sizeof(iotx_message_t));

    message.p_payload = (unsigned char *)payload;
    message.payload_len = payload_len;
    message.resp_callback = _coap_response_default;
    message.msg_type = qos;
    message.content_type = IOTX_CONTENT_TYPE_JSON;

    token = IOT_CoAP_GetCurToken((iotx_coap_context_t *)_coap_conncection->context);
    ret = IOT_CoAP_SendMessage((iotx_coap_context_t *)_coap_conncection->context, (char *)topic, &message);

    if (ret < 0) {
        return -1;
    }

    if (ext != NULL &&  ext->ack_cb != NULL) {
        coap_response_node_t *node;
        node = (coap_response_node_t *)cm_malloc(sizeof(coap_response_node_t));
        if (node == NULL) {
            return -1;
        }
        memset(node, 0, sizeof(coap_response_node_t));
        topic_len = strlen(topic) + 1;
        node->topic = (char *)cm_malloc(topic_len);
        if (node->topic == NULL) {
            cm_free(node);
            return -1;
        }

        memset(node->topic, 0, topic_len);
        strncpy(node->topic, topic, topic_len);

        node->user_data = _coap_conncection;
        node->responce_cb = ext->ack_cb;
        node->context = ext->cb_context;
        node->token_num = token;

        HAL_MutexLock(_coap_conncection->list_lock);
        list_add_tail(&node->linked_list, &g_coap_response_list);
        HAL_MutexUnlock(_coap_conncection->list_lock);
    }
    return 0;
}

static int _coap_yield(uint32_t timeout)
{
    if (_coap_conncection == NULL) {
        return NULL_VALUE_ERROR;
    }
    return  IOT_CoAP_Yield((iotx_coap_context_t *)_coap_conncection->context);
}

static int _coap_sub(iotx_cm_ext_params_t *ext, const char *topic,
                     iotx_cm_data_handle_cb topic_handle_func, void *pcontext)
{
    return 0;
}

static int _coap_unsub(const char *topic)
{
    return 0;
}

static int _coap_close()
{
    coap_response_node_t *node = NULL;
    coap_response_node_t *next = NULL;
    iotx_coap_config_t    *coap_config = NULL;

    if (_coap_conncection == NULL) {
        return NULL_VALUE_ERROR;
    }

    coap_config = (iotx_coap_config_t *)_coap_conncection->open_params;

    HAL_MutexLock(_coap_conncection->list_lock);
    list_for_each_entry_safe(node, next, &g_coap_response_list, linked_list, coap_response_node_t) {
        cm_free(node->topic);
        list_del(&node->linked_list);
        cm_free(node);
    }
    HAL_MutexUnlock(_coap_conncection->list_lock);

    if (_coap_conncection->list_lock != NULL) {
        HAL_MutexDestroy(_coap_conncection->list_lock);
    }
    cm_free(coap_config->p_devinfo);
    cm_free(coap_config);
    IOT_CoAP_Deinit(&_coap_conncection->context);

    cm_free(_coap_conncection);
    _coap_conncection = NULL;
    return 0;
}

static iotx_msg_type_t _get_coap_qos(iotx_cm_ack_types_t ack_type)
{
    switch (ack_type) {
        case IOTX_CM_MESSAGE_NO_ACK:
            return IOTX_MESSAGE_NON;

        case IOTX_CM_MESSAGE_NEED_ACK:
            return IOTX_MESSAGE_CON;

        default:
            return IOTX_MESSAGE_CON;
    }
}

static void _set_common_handlers()
{
    if (_coap_conncection != NULL) {
        _coap_conncection->connect_func = _coap_connect;
        _coap_conncection->sub_func = _coap_sub;
        _coap_conncection->unsub_func = _coap_unsub;
        _coap_conncection->pub_func = _coap_publish;
        _coap_conncection->yield_func = _coap_yield;
        _coap_conncection->close_func = _coap_close;
    }
}
#endif
