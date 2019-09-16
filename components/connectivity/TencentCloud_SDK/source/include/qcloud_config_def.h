#ifndef _QCLOUD_CONFIG_DEF_
#define  _QCLOUD_CONFIG_DEF_

#define QCLOUD_AUTH_MODE_KEY        1
#define QCLOUD_AUTH_MODE_CERT       2

typedef enum qcloud_log_level_en {
    QCLOUD_LOG_LEVEL_DISABLE    = 0,
    QCLOUD_LOG_LEVEL_DEBUG      = 1,
    QCLOUD_LOG_LEVEL_INFO       = 2,
    QCLOUD_LOG_LEVEL_WARN       = 3,
    QCLOUD_LOG_LEVEL_ERROR      = 4,
} qcloud_log_level_t;

#define QCLOUD_APPID                    "21010406"

#define QCLOUD_SDK_VERSION              "3.0.0"

#define QCLOUD_SERVER_DOMAIN_MAX        64

#define QCLOUD_TLS_HANDSHAKE_TIMEOUT    (5 * 1000)

#define QCLOUD_PSK_MAX                  48

#define QCLOUD_MQTT_DEVICE_CLIENT_ID_MAX                 80

#endif

