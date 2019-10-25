#ifndef _TOS_KTYPES_H_
#define  _TOS_KTYPES_H_

#include <stdint.h>

typedef uint8_t         k_prio_t;
typedef uint8_t         k_stack_t;
typedef uint8_t         k_task_state_t;

typedef uint8_t         k_nesting_t;

typedef uint16_t        k_opt_t;
typedef uint16_t        k_sem_cnt_t;
typedef uint32_t        k_event_flag_t;

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

