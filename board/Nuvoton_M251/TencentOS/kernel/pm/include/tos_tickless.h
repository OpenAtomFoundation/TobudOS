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

