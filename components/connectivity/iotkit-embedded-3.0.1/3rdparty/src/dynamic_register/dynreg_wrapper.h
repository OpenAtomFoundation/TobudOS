#ifndef _DYNREG_WRAPPER_H_
#define _DYNREG_WRAPPER_H_

#include "infra_types.h"

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt, ...);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

#endif

