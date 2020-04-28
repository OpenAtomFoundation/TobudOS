#ifndef __LL_SLEEP_H__
#define __LL_SLEEP_H__

#include "types.h"

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

typedef enum
{
    ACTIVE = 0,
    LIGHT_SLEEP,
    DEEP_SLEEP,
    RETENTION_SLEEP,
    FROZEN_SLEEP
}sleep_mode_enum_t;

void ll_pre_sleep_processing(sleep_mode_enum_t sleep_mode, uint8_t sram_powerdown);
void ll_post_sleep_processing(sleep_mode_enum_t sleep_mode);

#ifdef __cplusplus
    }
#endif // __cplusplus

#endif // __LL_SLEEP_H__


