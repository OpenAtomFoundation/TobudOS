/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_H__
#define __AWSS_H__

#include <stdint.h>
#include "infra_defs.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#ifndef _IN_
    #define _IN_
#endif

int awss_start(void);

/**
 * @brief   stop wifi setup service
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      if awss_stop is called before exit of awss_start, awss and notify will stop.
 */
int awss_stop(void);

/**
 * @brief   make sure user touches device belong to themselves
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note: AWSS doesn't parse awss packet until user touches deivce use this api.
 */
int awss_config_press(void);

/**
 * @brief   get the awss config press status in realtime.
 *
 * @retval  1 : user has touched device
 * @retval  0 : user don't touch device
 */
uint8_t awss_get_config_press(void);
void awss_set_config_press(uint8_t press);

/**
 * @brief   check reset flag in perisistent storage.
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      check reset flag in perisistent storage, if device failed to report reset message last time, retry it.
 */
int awss_check_reset(void);

/**
 * @brief   report reset to cloud.
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      device will save reset flag if device dosen't connect cloud, device will fails to send reset to cloud.
 *      when connection between device and cloud is ready, device will retry to report reset to cloud.
 */
int awss_report_reset(void);

/**
 * @brief   stop to report reset to cloud.
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      just stop report reset to cloud without any touch reset flag in flash.
 */
int awss_stop_report_reset(void);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
