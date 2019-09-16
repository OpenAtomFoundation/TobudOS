#ifndef _QCLOUD_EVENT_H_
#define  _QCLOUD_EVENT_H_

#define QCLOUD_EVENT_TOKEN_MAX      (32)

#define EVENT_FIELD_POST    "event_post"
#define EVENT_FIELD_POSTS   "events_post"
#define EVENT_FIELD_REPLY   "event_reply"

#define EVENT_REPLY_FIELD_CODE          "code"
#define EVENT_REPLY_FIELD_STATUS        "status"

typedef void (*event_reply_handler_fn_t)(void *client, mqtt_incoming_msg_t *msg);

typedef struct qcloud_event_reply_st {
    qcloud_list_t               list;

    char                        client_token[QCLOUD_EVENT_TOKEN_MAX];  // æ ‡è¯†è¯¥è¯·æ±‚çš„clientTokenå­—æ®µ
    osal_timer_t                timer;  // è¯·æ±‚è¶…æ—¶å®šæ—¶å™¨
    event_reply_handler_fn_t    handler;    // äº‹ä»¶ä¸ŠæŠ¥å›å¤å›è°ƒ
} qcloud_event_reply_t;

typedef enum event_sync_state_en {
    QCLOUD_EVENT_SYNC_STATE_NONE,
    QCLOUD_EVENT_SYNC_STATE_PENDACK,
    QCLOUD_EVENT_SYNC_STATE_SUCCESS,
    QCLOUD_EVENT_SYNC_STATE_TIMEOUT,
    QCLOUD_EVENT_SYNC_STATE_NACK,
} event_sync_state_t;

typedef struct qcloud_event_st {
    char                   *event_name; // event name
    char                   *type;   // event type
    uint32_t                timestamp;  // timestamp
    uint8_t                 event_payload_count;    // count of event_payload
    shadow_dev_property_t  *event_payload;  // payload of the event
} qcloud_event_t;

typedef struct qcloud_event_client_st {
    void                   *reply_list_lock;
    qcloud_list_t           reply_list;

    char                    down_topic[QCLOUD_MQTT_TOPIC_SIZE_MAX];
    char                    up_topic[QCLOUD_MQTT_TOPIC_SIZE_MAX];

    event_sync_state_t      sync_state;

    qcloud_shadow_client_t *shadow_client;
} qcloud_event_client_t;

__QCLOUD_API__ qcloud_err_t qcloud_event_client_create(qcloud_event_client_t *client, qcloud_shadow_client_t *shadow_client, qcloud_device_t *device);

__QCLOUD_API__ qcloud_err_t qcloud_event_client_destroy(qcloud_event_client_t *client);

/**
 * @brief ÊÂ¼şÉÏ±¨£¬´«ÈëÊÂ¼şÊı×é£¬SDKÍê³ÉÊÂ¼şµÄjson¸ñÊ½·â×°
 * @param pClient shadow ÊµÀıÖ¸Õë
 * @param pJsonDoc    ÓÃÓÚ¹¹½¨json¸ñÊ½ÉÏ±¨ĞÅÏ¢µÄbuffer
 * @param sizeOfBuffer    ÓÃÓÚ¹¹½¨json¸ñÊ½ÉÏ±¨ĞÅÏ¢µÄbuffer´óĞ¡
 * @param event_count     ´ıÉÏ±¨µÄÊÂ¼ş¸öÊı
 * @param pEventArry	  ´ıÉÏ±¨µÄÊÂ¼şÊı×éÖ¸
 * @param replyCb	  ÊÂ¼ş»Ø¸´ÏûÏ¢µÄ»Øµ÷
 * @return @see IoT_Error_Code
 */
__QCLOUD_API__ qcloud_err_t qcloud_event_client_post(qcloud_event_client_t *client,
                                                                        char *json_doc,
                                                                        size_t json_doc_size,
                                                                        int event_count,
                                                                        qcloud_event_t *events[],
                                                                        event_reply_handler_fn_t handler);

/**
 * @brief ÊÂ¼şÉÏ±¨£¬ÓÃ»§´«ÈëÒÑ¹¹½¨ºÃµÄÊÂ¼şµÄjson¸ñÊ½£¬SDKÔö¼ÓÊÂ¼şÍ·²¿¼´ÉÏ±¨
 * @param pClient shadow ÊµÀıÖ¸Õë
 * @param pJsonDoc    ÓÃÓÚ¹¹½¨json¸ñÊ½ÉÏ±¨ĞÅÏ¢µÄbuffer
 * @param sizeOfBuffer    ÓÃÓÚ¹¹½¨json¸ñÊ½ÉÏ±¨ĞÅÏ¢µÄbuffer´óĞ¡
 * @param pEventMsg     ´ıÉÏ±¨µÄÊÂ¼şjsonĞÅÏ¢
 *  jsonÊÂ¼ş¸ñÊ½£º
 *  µ¥¸öÊÂ¼ş£º
 *	 {"method": "event_post",
 *		"clientToken": "123",
 *		"version": "1.0",
 *		"eventId": "PowerAlarm",
 *		"type": "fatal",
 *		"timestamp": 1212121221,
 *		"params": {
 *			"Voltage": 2.8,
 *			"Percent": 20
 *		}
 *	}
 *
 *  ¶à¸öÊÂ¼ş£º
 *	 {
 *		 "eventId": "PowerAlarm",
 *		 "type": "fatal",
 *		 "timestamp": 1212121221,
 *		 "params": {
 *			 "Voltage": 2.8,
 *			 "Percent": 20
 *		 }
 *	 },
 *	 {
 *		 "name": "PowerAlarm",
 *		 "type": "fatal",
 *		 "timestamp": 1212121223,
 *		 "params": {
 *			 "Voltage": 2.1,
 *			 "Percent": 10
 *		 }
 *	 },
 *   ....
 *
 * @param replyCb	  ÊÂ¼ş»Ø¸´ÏûÏ¢µÄ»Øµ÷
 * @return @see IoT_Error_Code
 */
__QCLOUD_API__ qcloud_err_t qcloud_event_client_post_raw(qcloud_event_client_t *client,
                                                                            char *json_doc,
                                                                            size_t json_doc_size,
                                                                            char *event_msg,
                                                                            event_reply_handler_fn_t handler);

__QCLOUD_INTERNAL__ qcloud_err_t event_json_node_add(char *json_doc, size_t json_doc_size, const char *key, void *data, json_data_type_t type);

__QCLOUD_INTERNAL__ int event_json_return_code_parse(char *json_doc, int32_t *return_code);

__QCLOUD_INTERNAL__ int event_json_status_parse(char *json_doc, char **return_status);

#endif

