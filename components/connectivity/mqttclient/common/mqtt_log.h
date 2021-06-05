/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-27 03:25:58
 * @LastEditTime: 2020-10-17 14:15:55
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTT_LOG_H_
#define _MQTT_LOG_H_

#include "mqtt_defconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MQTT_LOG_BASE_LEVEL      (0)
#define MQTT_LOG_ERR_LEVEL       (MQTT_LOG_BASE_LEVEL + 1)
#define MQTT_LOG_WARN_LEVEL      (MQTT_LOG_ERR_LEVEL + 1)
#define MQTT_LOG_INFO_LEVEL      (MQTT_LOG_WARN_LEVEL + 1)
#define MQTT_LOG_DEBUG_LEVEL     (MQTT_LOG_INFO_LEVEL + 1)

#ifdef MQTT_LOG_IS_SALOF
    #include "salof.h"

    #define MQTT_LOG_D(fmt, ...)    SALOF_LOG_DEBUG(fmt, ##__VA_ARGS__)
    #define MQTT_LOG_I(fmt, ...)    SALOF_LOG_INFO(fmt, ##__VA_ARGS__)
    #define MQTT_LOG_W(fmt, ...)    SALOF_LOG_WARN(fmt, ##__VA_ARGS__)
    #define MQTT_LOG_E(fmt, ...)    SALOF_LOG_ERR(fmt, ##__VA_ARGS__)
    #define mqtt_log_init   salof_init
#else
    #include <stdio.h>
    
#if MQTT_LOG_LEVEL < MQTT_LOG_DEBUG_LEVEL
    #define MQTT_LOG_D(fmt, ...)
#else
    #define MQTT_LOG_D(fmt, ...)        { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

#if MQTT_LOG_LEVEL < MQTT_LOG_INFO_LEVEL
    #define MQTT_LOG_I(fmt, ...)
#else
    #define MQTT_LOG_I(fmt, ...)        { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

#if MQTT_LOG_LEVEL < MQTT_LOG_WARN_LEVEL
    #define MQTT_LOG_W(fmt, ...)
#else
    #define MQTT_LOG_W(fmt, ...)        { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

#if MQTT_LOG_LEVEL < MQTT_LOG_ERR_LEVEL
    #define MQTT_LOG_E(fmt, ...)
#else
    #define MQTT_LOG_E(fmt, ...)        { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

#if MQTT_LOG_LEVEL < MQTT_LOG_BASE_LEVEL
    #define MQTT_LOG(fmt, ...)
#else
    #define MQTT_LOG(fmt, ...)          { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

    #define mqtt_log_init()
#endif

#ifdef __cplusplus
}
#endif

#endif /* _LOG_H_ */
