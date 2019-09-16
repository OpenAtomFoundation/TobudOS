#ifndef _QCLOUD_MQTT_
#define  _QCLOUD_MQTT_

static const char *qcloud_mqtt_server = "iotcloud.tencentdevices.com";

#if (QCLOUD_CFG_TLS_EN > 0u)
static const int qcloud_mqtt_port = 8883;
#else
static const int qcloud_mqtt_port = 1883;
#endif

typedef enum qcloud_mqtt_connection_status_en {
    QCLOUD_MQTT_CONNECTION_STATE_DISCONNECTED  = 0,
    QCLOUD_MQTT_CONNECTION_STATE_CONNECTED     = 1,
} qcloud_mqtt_con_status_t;

/**
 * @brief 订阅主题对应的消息处理结构体定义
 */
typedef struct qcloud_mqtt_message_handler_st {
    qcloud_list_t               list;
    char                       *topic_filter_mutable;   // 订阅主题名, 可包含通配符
    mqtt_message_handler_fn_t   handler;                // 订阅主题消息回调函数指针
    mqtt_qos_t                  qos;
    void                       *private_data;           // 用户数据, 通过回调函数返回
} qcloud_mqtt_msg_handler_t;

typedef enum qcloud_mqtt_ack_type_en {
    QCLOUD_MQTT_ACK_TYPE_PUBACK,
    QCLOUD_MQTT_ACK_TYPE_SUBACK,
    QCLOUD_MQTT_ACK_TYPE_UNSUBACK,
} qcloud_mqtt_ack_type_t;

typedef struct qcloud_mqtt_ack_info_st {
    qcloud_list_t               list;
    osal_timer_t                ack_timer;
    qcloud_mqtt_ack_type_t      type;
    qcloud_mqtt_msg_handler_t  *handler; // when type == suback, handler is not null
    uint16_t                    packet_id;
    uint16_t                    packet_len;
    uint8_t                    *packet_body;
} qcloud_mqtt_ack_info_t;

typedef enum qcloud_auto_connect_state_en {
    QCLOUD_AUTO_CONN_STATE_DISABLED = 0,
    QCLOUD_AUTO_CONN_STATE_ENABLED = 1,
} qcloud_auto_connect_state_t;

/**
 * @brief IoT Client结构体定义
 */
typedef struct qcloud_mqtt_client_st {
    qcloud_auto_connect_state_t auto_connect_state;

    uint16_t                    keep_alive_interval;

    qcloud_mqtt_con_status_t    connection_state;               // 网络是否连接
    uint8_t                     is_manually_disconnected;       // 是否手动断开连接
    uint8_t                     ping_outstanding;               // 心跳包是否未完成, 即未收到服务器响应

    uint16_t                    packet_id;                      // MQTT报文标识符
    uint32_t                    command_timeout;                // MQTT消息超时时间, 单位:ms

    uint32_t                    reconnect_try_duration;         // MQTT重连周期, 单位:ms
    uint32_t                    network_disconnect_counter;         // 网络断开连接次数

    uint8_t                     tx_buffer[QCLOUD_MQTT_CLIENT_TX_BUF_LEN];  // MQTT消息发送buffer
    uint8_t                     rx_buffer[QCLOUD_MQTT_CLIENT_RX_BUF_LEN];  // MQTT消息接收buffer

    void                       *global_lock;                            // client原子锁
    void                       *tx_lock;                                // 输出流的锁

    void                       *ack_pend_list_lock;
    qcloud_list_t               ack_pend_list;

    mqtt_event_handler_t        event_handler;                          // 事件句柄

    qcloud_network_t            network;                                // MQTT底层使用的网络参数

    osal_timer_t                ping_timer;                             // MQTT心跳包发送定时器
    osal_timer_t                reconnect_timer;                        // MQTT重连定时器, 判断是否已到重连时间

    void                       *msg_handler_list_lock;
    qcloud_list_t               msg_handler_list;
} qcloud_mqtt_client_t;

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_connect_opt_create(mqtt_connect_opt_t *connect_opt,
                                                                                    qcloud_device_t *device,
                                                                                    mqtt_version_t mqtt_version,
                                                                                    uint16_t keep_alive_interval,
                                                                                    mqtt_clean_session_state_t clean_session);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_create(qcloud_mqtt_client_t *client,
                                                                        qcloud_device_t *device,
                                                                        mqtt_event_handler_fn_t handler,
                                                                        void *handler_context,
                                                                        qcloud_auto_connect_state_t auto_connect_state);

__QCLOUD_API__ void qcloud_mqtt_client_destroy(qcloud_mqtt_client_t *client);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_connect(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_disconnect(qcloud_mqtt_client_t *client);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_publish(qcloud_mqtt_client_t *client, char *topic, mqtt_publish_opt_t *publish_opt);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_subscribe(qcloud_mqtt_client_t *client, const char *topic_filter, mqtt_subscribe_opt_t *subscribe_opt);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_unsubscribe(qcloud_mqtt_client_t *client, const char *topic_filter);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_yield(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt, uint32_t timeout_ms);

__QCLOUD_API__ int qcloud_mqtt_client_is_connected(qcloud_mqtt_client_t *client);


__QCLOUD_API__ qcloud_err_t qcloud_mqtt_connect(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_disconnect(qcloud_mqtt_client_t *client);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_publish(qcloud_mqtt_client_t *client, char *topic, mqtt_publish_opt_t *publish_opt);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_subscribe(qcloud_mqtt_client_t *client, const char *topic_filter, mqtt_subscribe_opt_t *subscribe_opt);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_unsubscribe(qcloud_mqtt_client_t *client, const char *topic_filter);

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_yield(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt, uint32_t timeout_ms);


__QCLOUD_INTERNAL__ void mqtt_client_connection_state_set(qcloud_mqtt_client_t *client, qcloud_mqtt_con_status_t state);

__QCLOUD_INTERNAL__ char *mqtt_glue_string_const2mutable(const char *orig_str, size_t orig_str_len);


__QCLOUD_INTERNAL__ void mqtt_glue_string_mutable_free(char *str_mutable);

__QCLOUD_INTERNAL__ uint16_t mqtt_glue_packet_id_generate(qcloud_mqtt_client_t *client);

__QCLOUD_INTERNAL__ void mqtt_glue_connect_id_generate(char *conn_id);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_packet_send(qcloud_mqtt_client_t *client, size_t length, osal_timer_t *timer);

__QCLOUD_INTERNAL__ int mqtt_glue_is_topic_equals(const char *topic_filter, char *topic);

__QCLOUD_INTERNAL__ int mqtt_glue_is_topic_matches(const char *topic_filter, char *topic, uint16_t topic_len);

__QCLOUD_INTERNAL__ void mqtt_glue_packet_id_cache_reset(void);

__QCLOUD_INTERNAL__ void mqtt_glue_msg_handler_destory(qcloud_mqtt_msg_handler_t *msg_handler);

__QCLOUD_INTERNAL__ qcloud_mqtt_msg_handler_t * mqtt_glue_msg_handler_create(char *topic_filter_mutable, mqtt_subscribe_opt_t *subscribe_opt);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_msg_handler_list_destroy(qcloud_mqtt_client_t *client);

__QCLOUD_INTERNAL__ void mqtt_glue_msg_handler_uninstall(qcloud_mqtt_client_t *client, const char *topic_filter, int *is_subscribed);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_ack_list_scan(qcloud_mqtt_client_t *client);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_ack_list_destroy(qcloud_mqtt_client_t *client);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_ack_list_record(qcloud_mqtt_client_t *client,
                                                                                qcloud_mqtt_ack_type_t type,
                                                                                qcloud_mqtt_msg_handler_t *handler,
                                                                                uint16_t packet_id,
                                                                                uint16_t packet_len);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_callback_involve(qcloud_mqtt_client_t *client, mqtt_event_type_t event_type, void *msg);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_spin(qcloud_mqtt_client_t *client, osal_timer_t *timer, uint8_t *packet_type);

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_spin4ack(qcloud_mqtt_client_t *client, osal_timer_t *timer, uint8_t packet_type);

#endif

