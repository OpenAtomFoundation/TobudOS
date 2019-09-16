#ifndef _QCLOUD_MQTT_CONFIG_
#define  _QCLOUD_MQTT_CONFIG_

#define QCLOUD_MQTT_COMMAND_TIMEOUT                 (15 * 1000)

#define QCLOUD_MQTT_TX_BUFFER_SIZE                  (2048)

#define QCLOUD_MQTT_RX_BUFFER_SIZE                  (2048)

#define QCLOUD_MQTT_REMAIN_WAIT_MS_MAX              (2000)

#define QCLOUD_MQTT_CONNECT_ID_MAX                  (6)

#define QCLOUD_MQTT_ACCESS_EXPIRE_TIMEOUT_MAX       (0)

/* 重连最大等待时间 */
#define QCLOUD_MQTT_RECONNECT_TRY_THRESHOLD         (60 * 1000)

/* 重连最小等待时间 */
#define QCLOUD_MQTT_RECONNECT_TRY_INITIAL           (1000)

#define QCLOUD_MQTT_REPUB_PACKET_MAX                20

/* 云端保留主题的最大长度 */
#define QCLOUD_MQTT_TOPIC_SIZE_MAX                      (QCLOUD_DEVICE_DEVICE_NAME_MAX + QCLOUD_DEVICE_PRODUCT_ID_MAX + 64 + 6)

/* MQTT消息发送buffer大小, 支持最大256*1024 */
#define QCLOUD_MQTT_CLIENT_TX_BUF_LEN                   (2048)

/* MQTT消息接收buffer大小, 支持最大256*1024 */
#define QCLOUD_MQTT_CLIENT_RX_BUF_LEN                   (2048)

#endif

