/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 00:42:16
 * @LastEditTime : 2020-01-16 00:39:23
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTT_CONFIG_H_
#define _MQTT_CONFIG_H_

#define     MQTT_NO                             0
#define     MQTT_YES                            1

#define     MQTT_MAX_PACKET_ID                  (0xFFFF - 1)
#define     MQTT_TOPIC_LEN_MAX                  64
#define     MQTT_ACK_HANDLER_NUM_MAX            64
#define     MQTT_DEFAULT_BUF_SIZE               1024
#define     MQTT_DEFAULT_CMD_TIMEOUT            4000
#define     MQTT_MAX_CMD_TIMEOUT                20000
#define     MQTT_MIN_CMD_TIMEOUT                1000
#define     MQTT_KEEP_ALIVE_INTERVAL            20      // unit: second
#define     MQTT_VERSION                        4       // 4 is mqtt 3.1.1
#define     MQTT_RECONNECT_DEFAULT_DURATION     1000
#define     MQTT_THREAD_STACK_SIZE              2048
#define     MQTT_THREAD_PRIO                    5
#define     MQTT_THREAD_TICK                    50


#define     MQTT_NETWORK_TYPE_TLS               MQTT_YES


#if MQTT_NETWORK_TYPE_TLS

    #define MQTT_TLS_HANDSHAKE_TIMEOUT  (5 * 1000)

    #include "mbedtls/ssl.h"
    #include "mbedtls/entropy.h"
    #include "mbedtls/net_sockets.h"
    #include "mbedtls/ctr_drbg.h"
    #include "mbedtls/error.h"
    #include "mbedtls/debug.h"
#endif /* MQTT_NETWORK_TYPE_TLS */

#endif /* _MQTT_CONFIG_H_ */
