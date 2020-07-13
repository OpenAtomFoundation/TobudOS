/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _TOS_TIME_H_
#define  _TOS_TIME_H_

__CDECLS_BEGIN

// if you wanna pend for something forever, use TOS_TIME_FOREVER
#define TOS_TIME_FOREVER            (k_tick_t)(-1)
// if you don't wanna wait when you pend nothing, use TOS_TIME_NOWAIT
#define TOS_TIME_NOWAIT             (k_tick_t)0u

// those two are not for you, for kernel only.
#define K_TIME_MILLISEC_PER_SEC     1000u
#define K_TIME_MAX                      (k_tick_t)(TOS_TIME_FOREVER - 1)

/**
 * @brief Get system tick.
 * Get the number of ticks since boot.
 *
 * @attention None
 *
 * @param   None
 *
 * @return  tick count since boot
 */
__API__ k_tick_t    tos_systick_get(void);

/**
 * @brief Set system tick.
 * Set the number of ticks.
 *
 * @attention None
 *
 * @param   tick        systick count to set
 *
 * @return  None
 */
__API__ void        tos_systick_set(k_tick_t tick);

/**
 * @brief Convert ticks to milliseconds.
 * Convert tick to millisecond.
 *
 * @attention None
 *
 * @param[in]   tick        tick to convert.
 *
 * @return  milliseconds equals to the ticks.
 */
__API__ k_time_t    tos_tick2millisec(k_tick_t tick);

/**
 * @brief Convert milliseconds to ticks.
 * Convert milliseconds to ticks.
 *
 * @attention None
 *
 * @param[in]   millisec    millisecond to convert.
 *
 * @return  ticks equals to the millisecond.
 */
__API__ k_tick_t    tos_millisec2tick(k_time_t millisec);

/**
 * @brief Sleep current task.
 * Sleep for a specified amount of milliseconds.
 *
 * @attention None
 *
 * @param[in]   millisec    amount of milliseconds to delay.
 *
 * @return  errcode
 * @retval  #K_ERR_DELAY_ZERO   millisec is zero.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t     tos_sleep_ms(k_time_t millisec);

/**
 * @brief Sleep current task.
 * Sleep for a specified amount of time.
 *
 * @attention None
 *
 * @param[in]   hour        amount of hours.
 * @param[in]   minute      amount of minutes.
 * @param[in]   second      amount of seconds.
 * @param[in]   millisec    amount of milliseconds.
 *
 * @return  errcode
 * @retval  #K_ERR_DELAY_ZERO     time is zero.
 * @retval  #K_ERR_NONE           return successfully.
 */
__API__ k_err_t     tos_sleep_hmsm(k_time_t hour, k_time_t minute, k_time_t second, k_time_t millisec);

__CDECLS_END

#endif /* _TOS_TIME_H_ */

