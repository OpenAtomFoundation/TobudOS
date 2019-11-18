#include "infra_types.h"
#include "infra_defs.h"
#include "wrappers_defs.h"

#include "mqtt_api.h"

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt, ...);
uint64_t HAL_UptimeMs(void);
void HAL_SleepMs(uint32_t ms);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

void *HAL_MutexCreate(void);
void HAL_MutexDestroy(void *mutex);
void HAL_MutexLock(void *mutex);
void HAL_MutexUnlock(void *mutex);

int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1]);
int HAL_GetProductSecret(char product_secret[IOTX_PRODUCT_SECRET_LEN + 1]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN + 1]);
int HAL_GetFirmwareVersion(char *version);

#ifdef DYNAMIC_REGISTER
int HAL_SetDeviceSecret(char *device_secret);
int HAL_GetProductSecret(char product_secret[IOTX_PRODUCT_SECRET_LEN + 1]);
int HAL_Kv_Set(const char *key, const void *val, int len, int sync);
int HAL_Kv_Get(const char *key, void *val, int *buffer_len);
#endif

#ifdef SUPPORT_TLS
    uintptr_t HAL_SSL_Establish(const char *host, uint16_t port, const char *ca_crt, uint32_t ca_crt_len);
    int32_t HAL_SSL_Destroy(uintptr_t handle);
    int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms);
    int HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms);
#else
    uintptr_t HAL_TCP_Establish(const char *host, uint16_t port);
    int HAL_TCP_Destroy(uintptr_t fd);
    int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms);
    int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms);
#endif

/* mqtt protocol wrapper */
void *wrapper_mqtt_init(iotx_mqtt_param_t *mqtt_params);
int wrapper_mqtt_connect(void *client);
int wrapper_mqtt_yield(void *client, int timeout_ms);
int wrapper_mqtt_check_state(void *client);
int wrapper_mqtt_subscribe(void *client,
                           const char *topicFilter,
                           iotx_mqtt_qos_t qos,
                           iotx_mqtt_event_handle_func_fpt topic_handle_func,
                           void *pcontext);
int wrapper_mqtt_subscribe_sync(void *client,
                                const char *topic_filter,
                                iotx_mqtt_qos_t qos,
                                iotx_mqtt_event_handle_func_fpt topic_handle_func,
                                void *pcontext,
                                int timeout_ms);
int wrapper_mqtt_unsubscribe(void *client, const char *topicFilter);
int wrapper_mqtt_publish(void *client, const char *topicName, iotx_mqtt_topic_info_pt topic_msg);
int wrapper_mqtt_release(void **pclient);
int wrapper_mqtt_nwk_event_handler(void *client, iotx_mqtt_nwk_event_t event, iotx_mqtt_nwk_param_t *param);


