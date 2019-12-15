/**
 * @file    wm_wl_timers.h
 *
 * @brief   task APIs
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef __TLS_WL_TIMERS_H__
#define __TLS_WL_TIMERS_H__

#include "wm_type_def.h"
#include "wm_wl_mbox.h"

/** callback function of time out */
typedef void (* tls_timeout_handler)(void *arg);

/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup Timer_APIs Timer APIs
 * @brief Software timer APIs
 */

/**
 * @addtogroup Timer_APIs
 * @{
 */

/**
 * @brief          Create a one-shot timer (aka timeout)
 *
 * @param[in]      timeo_assigned   timer NO. by assigned
 * @param[in]      msecs            time in milliseconds after that the timer should expire
 * @param[in]      handler          callback function that would be called by the timeout
 * @param[in]      *arg             callback argument that would be passed to handler
 *
 * @return         None
 *
 * @note           While waiting for a message using sys_timeouts_mbox_fetch()
 */
void tls_timeout_p(u8 timeo_assigned, u32 msecs, tls_timeout_handler handler, void *arg);

/**
 * @brief          Go through timeout list (for this task only) and remove the first
 *                 matching entry, even though the timeout has not been triggered yet
 *
 * @param[in]      timeo_assigned   timer NO. by assigned
 * @param[in]      handler          callback function that would be called by the timeout
 * @param[in]      *arg             callback argument that would be passed to handler
 *
 * @return         None
 *
 * @note           None
 */
void tls_untimeout_p(u8 timeo_assigned, tls_timeout_handler handler, void *arg);

/**
 * @brief          Wait (forever) for a message to arrive in an mbox.
 *                 While waiting, timeouts are processed
 *
 * @param[in]      timeo_assigned   timer NO. by assigned
 * @param[in]      mbox             the mbox to fetch the message from
 * @param[out]     **msg            the place to store the message
 *
 * @return         None
 *
 * @note           None
 */
void tls_timeouts_mbox_fetch_p(u8 timeo_assigned, tls_mbox_t mbox, void **msg);

/**
 * @brief          Initialize the timer
 *
 * @param          None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
s8 tls_wl_timer_init(void);

/**
 * @}
 */

/**
 * @}
 */

#endif

