/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __HTTP2_WRAPPER_H__
#define __HTTP2_WRAPPER_H__

#include "infra_types.h"
#include "infra_defs.h"
#include "wrappers_defs.h"

extern void HAL_Printf(const char *fmt, ...);
extern void HAL_SleepMs(uint32_t ms);

extern void *HAL_Malloc(uint32_t size);
extern void *HAL_Realloc(void *ptr, uint32_t size);
extern void HAL_Free(void *ptr);

extern void *HAL_MutexCreate(void);
extern void HAL_MutexDestroy(void *mutex);
extern void HAL_MutexLock(void *mutex);
extern void HAL_MutexUnlock(void *mutex);

extern void *HAL_SemaphoreCreate(void);
extern void HAL_SemaphoreDestroy(void *sem);
extern void HAL_SemaphorePost(void *sem);
extern int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);

extern int HAL_ThreadCreate(
                            void **thread_handle,
                            void *(*work_routine)(void *),
                            void *arg,
                            hal_os_thread_param_t *hal_os_thread_param,
                            int *stack_used);
extern void HAL_ThreadDetach(void *thread_handle);
extern void HAL_ThreadDelete(void *thread_handle);

#ifdef FS_ENABLED
typedef enum {
    HAL_SEEK_SET,
    HAL_SEEK_CUR,
    HAL_SEEK_END
} hal_fs_seek_type_t;

extern void *HAL_Fopen(const char *path, const char *mode);
extern uint32_t HAL_Fread(void *buff, uint32_t size, uint32_t count, void *stream);
extern uint32_t HAL_Fwrite(const void *ptr, uint32_t size, uint32_t count, void *stream);
extern int HAL_Fseek(void *stream, long offset, int framewhere);
extern int HAL_Fclose(void *stream);
extern long HAL_Ftell(void *stream);
#endif /* #ifdef FS_ENABLED */

#endif /* #ifndef __HTTP2_WRAPPER_H__ */

