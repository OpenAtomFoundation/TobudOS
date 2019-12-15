#ifndef _DM_WRAPPER_H_
#define _DM_WRAPPER_H_

#include "infra_compat.h"
#include "wrappers_defs.h"

int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1]);
int HAL_GetProductSecret(char product_secret[IOTX_PRODUCT_SECRET_LEN + 1]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN + 1]);

int HAL_SetDeviceName(char *device_name);
int HAL_SetDeviceSecret(char *device_secret);
int HAL_SetProductKey(char *product_key);
int HAL_SetProductSecret(char *product_secret);

void *HAL_MutexCreate(void);
void HAL_MutexDestroy(void *mutex);
void HAL_MutexLock(void *mutex);
void HAL_MutexUnlock(void *mutex);

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
uint64_t HAL_UptimeMs(void);
void HAL_SleepMs(uint32_t ms);
void HAL_Srandom(uint32_t seed);
uint32_t HAL_Random(uint32_t region);
void HAL_Printf(const char *fmt, ...);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

#ifdef INFRA_LOG
#include <stdarg.h>
int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap);
#endif

int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            hal_os_thread_param_t *hal_os_thread_param,
            int *stack_used);
void HAL_ThreadDelete(void *thread_handle);

void *HAL_SemaphoreCreate(void);
void HAL_SemaphoreDestroy(void *sem);
void HAL_SemaphorePost(void *sem);
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);
int HAL_Kv_Set(const char *key, const void *val, int len, int sync);
int HAL_Kv_Get(const char *key, void *val, int *buffer_len);
int HAL_Kv_Del(const char *key);
#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    void HAL_Firmware_Persistence_Start(void);
    int HAL_Firmware_Persistence_Write(char *buffer, uint32_t length);
    int HAL_Firmware_Persistence_Stop(void);
#endif

#ifdef DEPRECATED_LINKKIT
int HAL_SetProductKey(char *product_key);
int HAL_SetProductSecret(char *product_secret);
int HAL_SetDeviceName(char *device_name);
int HAL_SetDeviceSecret(char *device_secret);
#endif

#ifdef ALCS_ENABLED
p_HAL_Aes128_t HAL_Aes128_Init(
            const uint8_t *key,
            const uint8_t *iv,
            AES_DIR_t dir);
int HAL_Aes128_Cbc_Encrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst);
int HAL_Aes128_Destroy(p_HAL_Aes128_t aes);
int HAL_Aes128_Cbc_Decrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst);
#endif

#endif
