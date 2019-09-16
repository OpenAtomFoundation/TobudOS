#ifndef _QCLOUD_DEVICE_CONFIG_
#define  _QCLOUD_DEVICE_CONFIG_

#define QCLOUD_DEVICE_PRODUCT_ID_MAX                14

#define QCLOUD_DEVICE_DEVICE_NAME_MAX               48

#define QCLOUD_DEVICE_USERNAME_MAX                 (QCLOUD_MQTT_DEVICE_CLIENT_ID_MAX + 8 + QCLOUD_MQTT_CONNECT_ID_MAX + 10 + 4 + 5)

#define QCLOUD_DEVICE_PASSWORD_MAX                 52

#define QCLOUD_DEVICE_KEY_MAX                       32

#if (QCLOUD_CFG_DEVICE_DYN_REG_EN > 0u)
#define QCLOUD_DEVICE_PRODUCT_KEY_MAX               32
#endif

#endif

