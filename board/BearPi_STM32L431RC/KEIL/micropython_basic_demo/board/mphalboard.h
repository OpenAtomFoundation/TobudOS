#ifndef _MPHALBOARD_H_
#define _MPHALBOARD_H_

#include "py/obj.h"

extern mp_uint_t mp_hal_ticks_us(void);
extern void mp_hal_delay_us(mp_uint_t usec);

extern const mp_obj_dict_t machine_pins_locals_dict;

#endif /* _MPHALBOARD_H_ */
