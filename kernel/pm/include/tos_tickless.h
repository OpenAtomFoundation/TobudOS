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

#ifndef _TOS_TICKLESS_H_
#define _TOS_TICKLESS_H_

#if TOS_CFG_TICKLESS_EN > 0u

typedef struct k_tickless_wakeup_alarm_st {
    int         (*init)(void);
    int         (*setup)(k_time_t millisecond);
    int         (*dismiss)(void);
    k_time_t    (*max_delay)(void); /* in millisecond */
} k_tickless_wkup_alarm_t;

__API__ void tos_tickless_wkup_alarm_install(k_cpu_lpwr_mode_t mode, k_tickless_wkup_alarm_t *wkup_alarm);

__API__ k_err_t tos_tickless_wkup_alarm_init(k_cpu_lpwr_mode_t mode);

__HOOK__ int tos_bsp_tickless_setup(void);

__KERNEL__ int tickless_wkup_alarm_is_installed(k_cpu_lpwr_mode_t mode);

__KERNEL__ void tickless_init(void);

__KERNEL__ void tickless_proc(void);

#endif /* TOS_CFG_TICKLESS_EN */

#endif /* _TOS_TICKLESS_H_ */

