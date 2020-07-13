#ifndef _WRAP_STDIO_H_
#define _WRAP_STDIO_H_

#ifdef __CONFIG_OS_KERNEL
#include "osal/osal.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define WRAP_STDOUT_BUF_SIZE	(256)
typedef int (*stdio_write_fn)(char *buf, int len);

typedef struct {
#ifdef __CONFIG_OS_KERNEL
	OS_Mutex_t stdout_mutex;
#endif
	char stdout_buf[WRAP_STDOUT_BUF_SIZE];
} wrap_stdio_t;

void __wrap_sprintf(stdio_write_fn write, const char *format, ...);


#ifdef __cplusplus
}
#endif


#endif /* _WRAP_STDIO_H_ */
