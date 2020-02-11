#ifndef _H_TOS_FARM_AI_CONFIG_H  
#define  _H_TOS_FARM_AI_CONFIG_H

#define MQTT_PRODUCT_ID         "U1BZWHF7F9"
#define MQTT_DEV_NAME           "dev_01"
#define MQTT_CLIENT_ID          "U1BZWHF7F9dev_01"
#define MQTT_USR_NAME           "U1BZWHF7F9dev_01;12010126;O3I57;1611533066"
#define MQTT_PASSWORD           "6ebcf20c9f7f725b5bcf18a85cc7a49479f489eea65946f92ba18a51a135c89e;hmacsha256"
#define MQTT_SUBSCRIBE_TOPIC    "U1BZWHF7F9/dev_01/data"
#define MQTT_PUBLISH_TOPIC      "$shadow/operation/U1BZWHF7F9/dev_01" // $shadow/operation/{MQTT_PRODUCT_ID}/{MQTT_DEV_NAME}

#endif
