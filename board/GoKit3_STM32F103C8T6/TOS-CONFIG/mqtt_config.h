/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 00:42:16
 * @LastEditTime: 2020-02-25 09:25:35
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTT_CONFIG_H_
#define _MQTT_CONFIG_H_

//#define             LOG_IS_SALOF

#define             LOG_LEVEL                   DEBUG_LEVEL   //WARN_LEVEL DEBUG_LEVEL

#ifdef LOG_IS_SALOF
    #define         USE_LOG                     (1U)
    #define         USE_SALOF                   (1U)
    #define         SALOF_OS                    USE_TENCENTOS
    #define         USE_IDLE_HOOK               (0U)
    #define         LOG_COLOR                   (0U)
    #define         LOG_TS                      (0U)
    #define         LOG_TAR                     (0U)
    #define         SALOF_BUFF_SIZE             (512U)
    #define         SALOF_FIFO_SIZE             (1024U)
    #define         SALOF_TASK_STACK_SIZE       (1024U)
    #define         SALOF_TASK_TICK             (50U)
#endif


#define     MQTT_MAX_PACKET_ID                  (0xFFFF - 1)
#define     MQTT_TOPIC_LEN_MAX                  64
#define     MQTT_ACK_HANDLER_NUM_MAX            64
#define     MQTT_DEFAULT_BUF_SIZE               1024
#define     MQTT_DEFAULT_CMD_TIMEOUT            4000
#define     MQTT_MAX_CMD_TIMEOUT                20000
#define     MQTT_MIN_CMD_TIMEOUT                1000
#define     MQTT_KEEP_ALIVE_INTERVAL            100         // unit: second
#define     MQTT_VERSION                        4           // 4 is mqtt 3.1.1
#define     MQTT_RECONNECT_DEFAULT_DURATION     1000
#define     MQTT_THREAD_STACK_SIZE              2048
#define     MQTT_THREAD_PRIO                    5
#define     MQTT_THREAD_TICK                    50

#define     MQTT_NETSOCKET_USE_AT

//#define     MQTT_NETWORK_TYPE_TLS

#endif /* _MQTT_CONFIG_H_ */
