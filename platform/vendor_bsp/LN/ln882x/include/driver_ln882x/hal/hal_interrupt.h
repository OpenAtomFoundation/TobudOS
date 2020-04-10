#ifndef __HAL_INTERRUPT_LN882X_H__
#define __HAL_INTERRUPT_LN882X_H__

#include "stdbool.h"

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */


void set_interrupt_priority(void);
void switch_global_interrupt(bool enable);




#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __HAL_INTERRUPT_LN882X_H__ */
