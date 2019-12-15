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

#ifndef _TOS_KTYPES_H_
#define  _TOS_KTYPES_H_

#include <stdint.h>
#include <stddef.h>

typedef uint8_t         k_prio_t;
typedef uint8_t         k_stack_t;
typedef uint8_t         k_task_state_t;

typedef uint8_t         k_nesting_t;

typedef uint16_t        k_opt_t;
typedef uint16_t        k_sem_cnt_t;
typedef uint32_t        k_event_flag_t;
typedef uint16_t        k_countdownlatch_cnt_t;

typedef uint32_t        k_time_t;
typedef uint32_t        k_timeslice_t;

typedef uint32_t        k_cycle_t;
typedef uint64_t        k_tick_t;

#define K_TRUE          (1u)
#define K_FALSE         (0u)

#ifndef K_NULL
#define  K_NULL         0
#endif

#endif /* _TOS_KTYPES_H_ */

