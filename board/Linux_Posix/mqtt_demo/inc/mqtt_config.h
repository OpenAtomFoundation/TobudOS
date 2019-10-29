#ifndef  TOS_MQTT_CONFIG_H
#define  TOS_MQTT_CONFIG_H

#define MQTT_SERVER_IP          "111.230.189.156"
#define MQTT_SERVER_PORT        "1883"
#define MQTT_PRODUCT_ID         "$product"
#define MQTT_DEV_NAME           "$dev"
#define MQTT_CLIENT_ID          "$product$dev"
#define MQTT_USR_NAME           "$product$dev;21010406;12365;4294967295"
#define MQTT_PASSWORD           "$sign;hmacsha1"
#define MQTT_SUBSCRIBE_TOPIC    "$product/$dev/$sub"
#define MQTT_PUBLISH_TOPIC      "$product/$dev/$pub"

#error please replace yourself server configuration
/**
 * 1. run python tool
 * ```
 *  cd tiny/tools/
 *  python3 mqtt_config_gen.py
 * ```
 * then input your server information
 * 
 * 2. tool will generate `mqtt_config.h` file, copy to replace this file
 */

#endif

