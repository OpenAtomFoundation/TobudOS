/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "dev_bind_internal.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int awss_stop_timer(void *timer)
{
    if (timer == NULL)
        return 0;
    HAL_Timer_Stop(timer);
    HAL_Timer_Delete(timer);
    return 0;
}
#if 0
int awss_start_timer(void **timer, const char *name, void *func, void *user_data, int ms)
{
    if (timer == NULL)
        return -1;
    *timer = HAL_Timer_Create(name, (void (*)(void *))func, user_data);
    if (*timer == NULL)
        return -1;
    HAL_Timer_Stop(*timer);
    HAL_Timer_Start(*timer, ms);
    return 0;
}
#endif
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
