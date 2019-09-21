#ifndef _TICKLESS_ALARM_H_
#define  _TICKLESS_ALARM_H_

#if TOS_CFG_TICKLESS_EN > 0u

extern k_tickless_wkup_alarm_t tickless_wkup_alarm_systick;

extern k_tickless_wkup_alarm_t tickless_wkup_alarm_tim;

extern k_tickless_wkup_alarm_t tickless_wkup_alarm_rtc_wkupirq;

extern k_tickless_wkup_alarm_t tickless_wkup_alarm_rtc_alarmirq;

#endif

#endif

