#ifndef _OSAL_OS_H_
#define  _OSAL_OS_H_

__QCLOUD_OSAL__ void       *osal_mutex_create(void);

__QCLOUD_OSAL__ void        osal_mutex_destroy(void *mutex);

__QCLOUD_OSAL__ void        osal_mutex_lock(void *mutex);

__QCLOUD_OSAL__ int         osal_mutex_trylock(void *mutex);

__QCLOUD_OSAL__ void        osal_mutex_unlock(void *mutex);

__QCLOUD_OSAL__ void       *osal_malloc(uint32_t size);

__QCLOUD_OSAL__ void        osal_free(void *ptr);

__QCLOUD_OSAL__ void        osal_printf(const char *fmt, ...);

__QCLOUD_OSAL__ int         osal_snprintf(char *str, const int len, const char *fmt, ...);

__QCLOUD_OSAL__ int         osal_vsnprintf(char *str, const int len, const char *format, va_list ap);

__QCLOUD_OSAL__ uint32_t    osal_uptime_ms(void);

__QCLOUD_OSAL__ void        osal_sleep_ms(uint32_t ms);

#endif

