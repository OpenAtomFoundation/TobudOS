#ifndef _QCLOUD_MQTT_COMMON_H_
#define  _QCLOUD_MQTT_COMMON_H_

typedef enum mqtt_version_en {
    MQTT_VERSION_3_1_1 = 4,
} mqtt_version_t;

typedef enum mqtt_clean_session_state_en {
    MQTT_CLEAN_SESSION_STATE_DISABLED = 0,
    MQTT_CLEAN_SESSION_STATE_ENABLED = 1,
} mqtt_clean_session_state_t;

typedef enum mqtt_quality_of_service_en {
    MQTT_QOS0 = 0,  // 至多分发一次
    MQTT_QOS1 = 1,  // 至少分发一次, 消息的接收者需回复PUBACK报文
    MQTT_QOS2 = 2,  // 仅分发一次, 目前腾讯物联云不支持该等级
} mqtt_qos_t;

typedef enum mqtt_packet_suback_return_code_en {
    MQTT_PACKET_SUBACK_RC_QOS0      = 0x00,
    MQTT_PACKET_SUBACK_RC_QOS1      = 0x01,
    MQTT_PACKET_SUBACK_RC_QOS2      = 0x02,
    MQTT_PACKET_SUBACK_RC_FAILURE   = 0x80,
} mqtt_packet_suback_rc_t;

typedef enum mqtt_packet_type_en {
    MQTT_PACKET_TYPE_RESERVED   = 0,    // Reserved
    MQTT_PACKET_TYPE_CONNECT    = 1,    // Client request to connect to Server
    MQTT_PACKET_TYPE_CONNACK    = 2,    // Connect Acknowledgment
    MQTT_PACKET_TYPE_PUBLISH    = 3,    // Publish message
    MQTT_PACKET_TYPE_PUBACK     = 4,    // Publish Acknowledgment
    MQTT_PACKET_TYPE_PUBREC     = 5,    // Publish Received
    MQTT_PACKET_TYPE_PUBREL     = 6,    // Publish Release
    MQTT_PACKET_TYPE_PUBCOMP    = 7,    // Publish Complete
    MQTT_PACKET_TYPE_SUBSCRIBE  = 8,    // Client Subscribe request
    MQTT_PACKET_TYPE_SUBACK     = 9,    // Subscribe Acknowledgment
    MQTT_PACKET_TYPE_UNSUBSCRIBE    = 10,   // Client Unsubscribe request
    MQTT_PACKET_TYPE_UNSUBACK   = 11,   // Unsubscribe Acknowledgment
    MQTT_PACKET_TYPE_PINGREQ    = 12,   // PING Request
    MQTT_PACKET_TYPE_PINGRESP   = 13,   // PING Response
    MQTT_PACKET_TYPE_DISCONNECT = 14,   // Client is Disconnecting
} mqtt_packet_t;

/**
 * Bitfields for the MQTT header byte.
 */
typedef union mqtt_header_un {
    uint8_t byte;                   /**< the whole byte */
#if defined(REVERSED)
    struct {
        unsigned int type : 4;      /**< message type nibble */
        unsigned int dup : 1;       /**< DUP flag bit */
        unsigned int qos : 2;       /**< QoS value, 0, 1 or 2 */
        unsigned int retain : 1;    /**< retained flag bit */
    } bits;
#else
    struct {
        unsigned int retain : 1;    /**< retained flag bit */
        unsigned int qos : 2;       /**< QoS value, 0, 1 or 2 */
        unsigned int dup : 1;       /**< DUP flag bit */
        unsigned int type : 4;      /**< message type nibble */
    } bits;
#endif
} mqtt_header_t;

typedef union mqtt_connect_flags_un {
    uint8_t all;                        /**< all connect flags */
#if defined(REVERSED)
    struct {
        unsigned int username : 1;      /**< 3.1 user name */
        unsigned int password : 1;      /**< 3.1 password */
        unsigned int willRetain : 1;    /**< will retain setting */
        unsigned int willQoS : 2;       /**< will QoS value */
        unsigned int will : 1;          /**< will flag */
        unsigned int cleansession : 1;	    /**< clean session flag */
        unsigned int : 1;               /**< unused */
    } bits;
#else
    struct {
        unsigned int : 1;               /**< unused */
        unsigned int cleansession : 1;  /**< cleansession flag */
        unsigned int will : 1;          /**< will flag */
        unsigned int willQoS : 2;       /**< will QoS value */
        unsigned int willRetain : 1;    /**< will retain setting */
        unsigned int password : 1;      /**< 3.1 password */
        unsigned int username : 1;      /**< 3.1 user name */
    } bits;
#endif
} mqtt_connect_flag_t;  	/**< connect flags byte */

typedef union mqtt_connack_flags_un {
    uint8_t all;                            /**< all connack flags */
#if defined(REVERSED)
    struct {
        unsigned int reserved : 7;          /**< unused */
        unsigned int sessionpresent : 1;    /**< session present flag */
    } bits;
#else
    struct {
        unsigned int sessionpresent : 1;    /**< session present flag */
        unsigned int reserved: 7;           /**< unused */
    } bits;
#endif
} mqtt_connack_flags_t; /**< connack flags byte */

/**
 * connect return code
 */
typedef enum mqtt_connack_return_code_en {
    MQTT_CONNACK_CONNECTION_ACCEPTED = 0,                       // connection accepted
    MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR = 1,   // connection refused: unaccpeted protocol verison
    MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR = 2,             // connection refused: identifier rejected
    MQTT_CONNACK_SERVER_UNAVAILABLE_ERROR = 3,              // connection refused: server unavailable
    MQTT_CONNACK_BAD_USERDATA_ERROR = 4,                    // connection refused: bad user name or password
    MQTT_CONNACK_NOT_AUTHORIZED_ERROR = 5                   // connection refused: not authorized
} mqtt_connack_rc_t;

typedef struct mqtt_connect_option_st {
    /* The eyecatcher for this structure.  must be MQTC. */
    char                        struct_id[4];

    /* The version number of this structure.  Must be 0 */
    int                         struct_version;

    /* Version of MQTT to be used.  3 = 3.1 4 = 3.1.1
     */
    mqtt_version_t              mqtt_version;

    char                        client_id[QCLOUD_MQTT_DEVICE_CLIENT_ID_MAX + 1];

    uint16_t                    keep_alive_interval;
    mqtt_clean_session_state_t  clean_session;

    char                        username[QCLOUD_DEVICE_USERNAME_MAX + 1];
    size_t                      username_len;
    char                        password[QCLOUD_DEVICE_PASSWORD_MAX + 1];
    size_t                      password_len;
} mqtt_connect_opt_t;

/**
 * @brief 发布或接收已订阅消息的结构体定义
 */
typedef struct mqtt_publish_option_st {
    mqtt_qos_t  qos;            // MQTT 服务质量等级
    uint8_t     retained;       // RETAIN 标识位
    uint8_t     dup;            // DUP 标识位
    uint16_t    id;             // MQTT 消息标识符

    const char *topic;          // MQTT topic
    size_t      topic_len;      // topic 长度

    void       *payload;        // MQTT 消息负载
    size_t      payload_len;    // MQTT 消息负载长度
} mqtt_publish_opt_t;

typedef mqtt_publish_opt_t  mqtt_incoming_msg_t;

/**
 * @brief MQTT PUBLISH 消息回调处理函数指针定义
 */
typedef void (*mqtt_message_handler_fn_t)(void *client, mqtt_incoming_msg_t *message, void *private_data);

/**
 * @brief 订阅主题的结构体定义
 */
typedef struct mqtt_subscribe_option_st {
    mqtt_qos_t                  qos;                    // 服务质量等级, 目前支持QOS0和QOS1
    mqtt_message_handler_fn_t   message_handler;     // 接收已订阅消息的回调函数
    void                       *private_data;              // 用户数据, 通过callback返回
} mqtt_subscribe_opt_t;

typedef enum mqtt_event_type_en {
    /* 未定义事件 */
    MQTT_EVENT_UNDEF = 0,

    /* MQTT 断开连接 */
    MQTT_EVENT_DISCONNECT = 1,

    /* MQTT 重连 */
    MQTT_EVENT_RECONNECT = 2,

    /* 订阅成功 */
    MQTT_EVENT_SUBCRIBE_SUCCESS = 3,

    /* 订阅超时 */
    MQTT_EVENT_SUBCRIBE_TIMEOUT = 4,

    /* 订阅失败 */
    MQTT_EVENT_SUBCRIBE_NACK = 5,

    /* 取消订阅成功 */
    MQTT_EVENT_UNSUBCRIBE_SUCCESS = 6,

    /* 取消订阅超时 */
    MQTT_EVENT_UNSUBCRIBE_TIMEOUT = 7,

    /* 取消订阅失败 */
    MQTT_EVENT_UNSUBCRIBE_NACK = 8,

    /* 发布成功 */
    MQTT_EVENT_PUBLISH_SUCCESS = 9,

    /* 发布超时 */
    MQTT_EVENT_PUBLISH_TIMEOUT = 10,

    /* 发布失败 */
    MQTT_EVENT_PUBLISH_NACK = 11,

    /* SDK订阅的topic收到后台push消息 */
    MQTT_EVENT_PUBLISH_RECVEIVED = 12,
} mqtt_event_type_t;

typedef struct mqtt_event_st {
    /* 事件类型 */
    mqtt_event_type_t   type;
    void               *message;
} mqtt_event_t;

typedef void (*mqtt_event_handler_fn_t)(void *client, void *context, mqtt_event_t *event);

/* The structure of MQTT event handle */
typedef struct mqtt_event_handler_st {
    mqtt_event_handler_fn_t handler;
    void                   *context;
} mqtt_event_handler_t;

__QCLOUD_INTERNAL__ int mqtt_common_packet_encode(uint8_t *buf, int length);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_ack_packet(uint8_t *packet_type,
                                                                                                uint8_t *dup,
                                                                                                uint16_t *packet_id,
                                                                                                uint8_t *buf,
                                                                                                size_t buf_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_suback_packet(uint16_t *packet_id,
                                                                                                    uint32_t max_count,
                                                                                                    uint32_t *count,
                                                                                                    int granted_qoss[],
                                                                                                    uint8_t *buf,
                                                                                                    size_t buf_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_unsuback_packet(uint16_t *packet_id,
                                                                                                        uint8_t *buf,
                                                                                                        size_t buf_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_connect_packet(uint8_t *buf,
                                                                                                    size_t buf_len,
                                                                                                    mqtt_connect_opt_t *connect_opt,
                                                                                                    uint32_t *serialized_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_connack_packet(uint8_t *session_present,
                                                                                                    uint8_t *connack_rc,
                                                                                                    uint8_t *buf,
                                                                                                    size_t buf_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_zero_payload_packet(uint8_t *buf,
                                                                                                            size_t buf_len,
                                                                                                            mqtt_packet_t packet_type,
                                                                                                            uint32_t *serialized_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_publish_packet(uint8_t *dup, int *qos,
                                                                                                    uint8_t *retained, uint16_t *packet_id,
                                                                                                    char **topic, uint16_t *topic_len,
                                                                                                    uint8_t **payload, size_t *payload_len,
                                                                                                    uint8_t *buf, size_t buf_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_puback_packet(uint8_t *buf,
                                                                                                size_t buf_len,
                                                                                                uint16_t packet_id,
                                                                                                uint32_t *serialized_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_pubrel_packet(uint8_t *buf,
                                                                                                size_t buf_len,
                                                                                                uint8_t dup,
                                                                                                uint16_t packet_id,
                                                                                                uint32_t *serialized_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_pubrec_packet(uint8_t *buf,
                                                                                                size_t buf_len,
                                                                                                uint16_t packet_id,
                                                                                                uint32_t *serialized_len);


__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_publish_packet(uint8_t *buf,
                                                                                                size_t buf_len,
                                                                                                uint8_t dup,
                                                                                                int qos,
                                                                                                uint8_t retained,
                                                                                                uint16_t packet_id,
                                                                                                char *topic,
                                                                                                uint8_t *payload,
                                                                                                size_t payload_len,
                                                                                                uint32_t *serialized_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_subscribe_packet(uint8_t *buf,
                                                                                                    size_t buf_len,
                                                                                                    uint8_t dup,
                                                                                                    uint16_t packet_id,
                                                                                                    uint32_t count,
                                                                                                    char *topic_filters[],
                                                                                                    int requested_qoss[],
                                                                                                    uint32_t *serialized_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_unsubscribe_packet(uint8_t *buf, size_t buf_len,
                                                                                                        uint8_t dup, uint16_t packet_id,
                                                                                                        uint32_t count, char *topic_filters[],
                                                                                                        uint32_t *serialized_len);

#endif

