#ifndef _TOS_EVTDRV_TICK_H_
#define  _TOS_EVTDRV_TICK_H_

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

/**
 * @brief Get the current systick cout.
 *
 * @attention
 *
 * @return  current systick count.
 */
__API__ evtdrv_tick_t tos_evtdrv_systick_get(void);

/**
 * @brief tick interrupt handler.
 *
 * @attention   if you wanna use event-driven timer, you must deal with the tick.
 *
 * @return  None.
 */
__API__ void tos_evtdrv_tick_handler(void);

/**
 * @brief Convert tick count to milli-seconds.
 *
 * @attention
 *
 * @return  the milli-seconds equals to the tick count.
 */
__API__ evtdrv_time_t tos_evtdrv_tick2millisec(evtdrv_tick_t tick);

/**
 * @brief Convert milli-seconds to tick count.
 *
 * @attention
 *
 * @return  the tick count equals to the tick count.
 */
__API__ evtdrv_tick_t tos_evtdrv_millisec2tick(evtdrv_time_t ms);

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

#endif /* _TOS_EVTDRV_TICK_H_ */

