/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-08 19:41:14
 * @LastEditTime: 2020-01-08 19:44:11
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
/**
 * \file timing_alt.h
 *
 * \brief Portable interface to the CPU cycle counter
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
 
#ifndef _TIMING_ALT_H_
#define  _TIMING_ALT_H_

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "stdint.h"

/**
 * @brief   timer structure
 */
struct mbedtls_timing_hr_time {
    uint64_t timer_ms;
};

/**
 * @brief   Context for mbedtls_timing_set/get_delay()
 */
typedef struct {
    struct mbedtls_timing_hr_time   timer;
    uint32_t                        int_ms;
    uint32_t                        fin_ms;
} mbedtls_timing_delay_context;

/**
 * @brief   Return the elapsed time in milliseconds
 *
 * @param   val     points to a timer structure
 * @param   reset   if set to 1, the timer is restarted
 */
unsigned long mbedtls_timing_get_timer(struct mbedtls_timing_hr_time *val, int reset);

/**
 * @brief   Set a pair of delays to watch
 *          Must point to a valid mbedtls_timing_delay_context struct.
 *
 * @param   data    Pointer to timing data
 * @param   int_ms  First (intermediate) delay in milliseconds.
 * @param   fin_ms  Second (final) delay in milliseconds.
 *                  Pass 0 to cancel the current delay.
 */
void mbedtls_timing_set_delay(void *data, uint32_t int_ms, uint32_t fin_ms);

/**
 * @brief   Get the status of delays
 *          (Memory helper: number of delays passed.)
 *
 * @param   data    Pointer to timing data
 *                  Must point to a valid mbedtls_timing_delay_context struct.
 *
 * @return  -1 if cancelled (fin_ms = 0)
 *          0 if none of the delays are passed,
 *          1 if only the intermediate delay is passed,
 *          2 if the final delay is passed.
 */
int mbedtls_timing_get_delay(void *data);

#endif

