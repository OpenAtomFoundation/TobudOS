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

#ifndef _TOS_STOPWATCH_H_
#define  _TOS_STOPWATCH_H_

__CDECLS_BEGIN

typedef struct k_stopwatch_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t   knl_obj;
#endif

    k_tick_t    until;
} k_stopwatch_t;

/**
 * @brief Create a stopwatch.
 *
 * @attention
 *
 * @param[in]   stopwatch               the stopwatch.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_stopwatch_create(k_stopwatch_t *stopwatch);

/**
 * @brief Destroy a stopwatch.
 *
 * @attention
 *
 * @param[in]   stopwatch               the stopwatch.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_stopwatch_destroy(k_stopwatch_t *stopwatch);

/**
 * @brief Count down for a certain tick.
 *
 * @attention
 *
 * @param[in]   stopwatch               the stopwatch.
 * @param[in]   tick                    tick to count down.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_stopwatch_countdown(k_stopwatch_t *stopwatch, k_tick_t tick);

/**
 * @brief Count down for a certain time(in millisecond).
 *
 * @attention
 *
 * @param[in]   stopwatch               the stopwatch.
 * @param[in]   millisec                time(in millisecond) to count down.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_stopwatch_countdown_ms(k_stopwatch_t *stopwatch, k_time_t millisec);

/**
 * @brief Delay for a certain tick.
 *
 * @attention the stopwatch delay is a "busy" delay without give up of CPU(compared to tos_task_delay)
 *
 * @param[in]   tick                    tick to delay.
 *
 * @return  None
 */
__API__ void tos_stopwatch_delay(k_tick_t tick);

/**
 * @brief Delay for a certain time(in millisecond).
 *
 * @attention the stopwatch delay is a "busy" delay without give up of CPU(compared to tos_task_delay)
 *
 * @param[in]   millisec                time(in millisecond) to delay.
 *
 * @return  None
 */
__API__ void tos_stopwatch_delay_ms(k_time_t millisec);

/**
 * @brief How much time remain of the stopwatch(in tick).
 *
 * @attention
 *
 * @param[in]   stopwatch               ticks remain.
 *
 * @return  ticks remain
 */
__API__ k_tick_t tos_stopwatch_remain(k_stopwatch_t *stopwatch);

/**
 * @brief How much time remain of the stopwatch(in millisecond).
 *
 * @attention
 *
 * @param[in]   stopwatch               milliseconds remain.
 *
 * @return  milliseconds remain
 */
__API__ k_time_t tos_stopwatch_remain_ms(k_stopwatch_t *stopwatch);

/**
 * @brief Whether the stopwatch is expired.
 *
 * @attention
 *
 * @param[in]   stopwatch               milliseconds remain.
 *
 * @return  whether the stopwatch is expired
 * @retval  #K_TRUE     the stopwatch is expired.
 * @retval  #K_FALSE    the stopwatch is no expired.
 */
__API__ int tos_stopwatch_is_expired(k_stopwatch_t *stopwatch);

__CDECLS_END

#endif /* _TOS_STOPWATCH_H_ */

