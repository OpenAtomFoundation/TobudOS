#include "proj_config.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "ln88xx.h"
#include "utils/wrap_stdio.h"

static wrap_stdio_t s_wrap_stdio = {0};

static wrap_stdio_t *wrap_stdio_get_handle(void)
{
	return &s_wrap_stdio;
}

static void printf_mutex_lock(void)
{
#ifdef __CONFIG_OS_KERNEL
    wrap_stdio_t *wrap_stdio = wrap_stdio_get_handle();

    if (__get_PRIMASK() || __get_FAULTMASK() || __get_IPSR() || \
        !OS_ThreadIsSchedulerRunning()){
        return;
    }

	if (!OS_MutexIsValid(&wrap_stdio->stdout_mutex)) {
		OS_MutexCreate(&wrap_stdio->stdout_mutex);
	}
	OS_MutexLock(&wrap_stdio->stdout_mutex, OS_WAIT_FOREVER);
#endif
}

static void printf_mutex_unlock(void)
{
#ifdef __CONFIG_OS_KERNEL
    wrap_stdio_t *wrap_stdio = wrap_stdio_get_handle();

    if (__get_PRIMASK() || __get_FAULTMASK() || __get_IPSR() || \
        !OS_ThreadIsSchedulerRunning()){
        return;
    }

	if (OS_MutexIsValid(&wrap_stdio->stdout_mutex)) {
		OS_MutexUnlock(&wrap_stdio->stdout_mutex);
	}
#endif
}

void __wrap_sprintf(stdio_write_fn write, const char *format, ...)
{
    va_list args;
    wrap_stdio_t *wrap_stdio = wrap_stdio_get_handle();

    printf_mutex_lock();
    
    va_start(args, format);
    vsnprintf(wrap_stdio->stdout_buf, WRAP_STDOUT_BUF_SIZE, format, args);
    va_end(args);
    if(write){
        write(wrap_stdio->stdout_buf, strlen(wrap_stdio->stdout_buf));
    }
    
    printf_mutex_unlock();
}


