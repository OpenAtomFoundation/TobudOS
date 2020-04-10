/**
 * @file   freertos_timer.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_FREERTOS_OS_TIMER_H_
#define _KERNEL_OS_FREERTOS_OS_TIMER_H_

#include "./FreeRTOS_Adapter/freertos_common.h"
#include "timers.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(configUSE_TIMER_ID_AS_CALLBACK_ARG) && configUSE_TIMER_ID_AS_CALLBACK_ARG == 1)
  #define OS_TIMER_USE_FREERTOS_ORIG_CALLBACK	0
#else
  #define OS_TIMER_USE_FREERTOS_ORIG_CALLBACK	1
#endif

/**
 * @brief Timer type definition
 *     - one shot timer: Timer will be in the dormant state after it expires.
 *     - periodic timer: Timer will auto-reload after it expires.
 */
typedef enum {
    OS_TIMER_ONCE       = 0, /* one shot timer */
    OS_TIMER_PERIODIC   = 1  /* periodic timer */
} OS_TimerType;

/** @brief Timer expire callback function definition */
typedef void (*OS_TimerCallback_t)(void *arg);

/** @brief Timer handle definition */
typedef TimerHandle_t OS_TimerHandle_t;

#if OS_TIMER_USE_FREERTOS_ORIG_CALLBACK
/**
 * @brief Timer expire callback data definition
 */
typedef struct OS_TimerCallbackData {
    OS_TimerCallback_t  callback;   /* Timer expire callback function */
    void                *argument;  /* Argument of timer expire callback function */
} OS_TimerCallbackData_t;
#endif

/**
 * @brief Timer object definition
 */
typedef struct OS_Timer {
    TimerHandle_t           handle;
#if OS_TIMER_USE_FREERTOS_ORIG_CALLBACK
    OS_TimerCallbackData_t  *priv;	/* private data for internally usage */
#endif
} OS_Timer_t;


OS_Status OS_TimerCreate(OS_Timer_t *timer, OS_TimerType type,
                         OS_TimerCallback_t cb, void *arg, OS_Time_t periodMS);
OS_Status OS_TimerDelete(OS_Timer_t *timer);
OS_Status OS_TimerStart(OS_Timer_t *timer);
OS_Status OS_TimerChangePeriod(OS_Timer_t *timer, OS_Time_t periodMS);
OS_Status OS_TimerStop(OS_Timer_t *timer);

/**
 * @brief Check whether the timer object is valid or not
 * @param[in] timer Pointer to the timer object
 * @return 1 on valid, 0 on invalid
 */
__STATIC_INLINE int OS_TimerIsValid(OS_Timer_t *timer)
{
	return (timer->handle != OS_INVALID_HANDLE);
}

/**
 * @brief Set the timer object to invalid state
 * @param[in] timer Pointer to the timer object
 * @return None
 */
__STATIC_INLINE void OS_TimerSetInvalid(OS_Timer_t *timer)
{
	timer->handle = OS_INVALID_HANDLE;
}

/**
 * @brief Check whether the timer is active or not
 *
 * A timer is inactive when it is in one of the following cases:
 *     - The timer has been created, but not started.
 *     - The timer is a one shot timer that has not been restarted since it
 *       expired.
 *
 * @param[in] timer Pointer to the timer object
 * @return 1 on active, 0 on inactive
 */
__STATIC_INLINE int OS_TimerIsActive(OS_Timer_t *timer)
{
	return (xTimerIsTimerActive(timer->handle) != pdFALSE);
}

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_FREERTOS_OS_TIMER_H_ */
