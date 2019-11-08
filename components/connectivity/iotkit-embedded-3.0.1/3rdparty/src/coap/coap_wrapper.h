#ifndef _COAP_WRAPPER_H_
#define _COAP_WRAPPER_H_

#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "wrappers_defs.h"

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_SleepMs(uint32_t ms);
uint64_t HAL_UptimeMs(void);
void HAL_Srandom(uint32_t seed);
uint32_t HAL_Random(uint32_t region);
void HAL_Printf(const char *fmt, ...);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN]);

int HAL_SetProductKey(char *product_key);
int HAL_SetProductSecret(char *product_secret);
int HAL_SetDeviceName(char *device_name);
int HAL_SetDeviceSecret(char *device_secret);

int HAL_DTLSHooks_set(dtls_hooks_t *hooks);
DTLSContext *HAL_DTLSSession_create(coap_dtls_options_t  *p_options);
unsigned int HAL_DTLSSession_write(DTLSContext *context,
        const unsigned char *p_data,
        unsigned int *p_datalen);
unsigned int HAL_DTLSSession_read(DTLSContext *context,
        unsigned char *p_data,
        unsigned int *p_datalen,
        unsigned int timeout_ms);
unsigned int HAL_DTLSSession_free(DTLSContext *context);
intptr_t HAL_UDP_create(char *host, unsigned short port);
intptr_t HAL_UDP_create_without_connect(const char *host, unsigned short port);
int HAL_UDP_write(intptr_t p_socket,
                  const unsigned char *p_data,
                  unsigned int datalen);
int HAL_UDP_readTimeout(intptr_t p_socket,
                        unsigned char *p_data,
                        unsigned int datalen,
                        unsigned int timeout);
int HAL_UDP_close_without_connect(intptr_t sockfd);
int HAL_UDP_recvfrom(intptr_t sockfd,
                     NetworkAddr *p_remote,
                     unsigned char *p_data,
                     unsigned int datalen,
                     unsigned int timeout_ms);
int HAL_UDP_sendto(intptr_t sockfd,
                   const NetworkAddr *p_remote,
                   const unsigned char *p_data,
                   unsigned int datalen,
                   unsigned int timeout_ms);
int HAL_UDP_joinmulticast(intptr_t sockfd,
                          char *p_group);
uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname);
p_HAL_Aes128_t HAL_Aes128_Init(
            const uint8_t *key,
            const uint8_t *iv,
            AES_DIR_t dir);
int HAL_Aes128_Destroy(p_HAL_Aes128_t aes);
int HAL_Aes128_Cbc_Encrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst);
int HAL_Aes128_Cbc_Decrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst);
void *HAL_MutexCreate(void);
void HAL_MutexDestroy(void *mutex);
void HAL_MutexLock(void *mutex);
void HAL_MutexUnlock(void *mutex);
void *HAL_SemaphoreCreate(void);
void HAL_SemaphoreDestroy(void *sem);
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);
void HAL_SemaphorePost(void *sem);
int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            hal_os_thread_param_t *hal_os_thread_param,
            int *stack_used);
void HAL_ThreadDelete(void *thread_handle);
#endif
