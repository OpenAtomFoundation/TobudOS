#include "infra_types.h"
#include "infra_defs.h"
#include "wrappers_defs.h"
#ifdef WIFI_PROVISION_ENABLED
#include "iot_import_awss.h"
#endif
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN]);
int HAL_GetProductSecret(char *product_secret);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN]);
void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data);
int HAL_Timer_Stop(void *timer);
int HAL_Timer_Start(void *timer, int ms);
int HAL_Timer_Delete(void *timer);
char *HAL_Wifi_Get_Mac(char mac_str[HAL_MAC_LEN]);
void HAL_Srandom(uint32_t seed);
uint32_t HAL_Random(uint32_t region);
void HAL_Reboot();
void *HAL_MutexCreate(void);
void HAL_SleepMs(uint32_t ms);
void HAL_MutexDestroy(void *mutex);
void HAL_MutexLock(void *mutex);
void HAL_MutexUnlock(void *mutex);
void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
int HAL_Sys_Net_Is_Ready();
uint64_t HAL_UptimeMs(void);
uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname);
#ifdef WIFI_PROVISION_ENABLED
int HAL_Wifi_Get_Ap_Info(char ssid[HAL_MAX_SSID_LEN],char passwd[HAL_MAX_PASSWD_LEN],uint8_t bssid[ETH_ALEN]);
int HAL_Awss_Connect_Ap(
            _IN_ uint32_t connection_timeout_ms,
            _IN_ char ssid[HAL_MAX_SSID_LEN],
            _IN_ char passwd[HAL_MAX_PASSWD_LEN],
            _IN_OPT_ enum AWSS_AUTH_TYPE auth,
            _IN_OPT_ enum AWSS_ENC_TYPE encry,
            _IN_OPT_ uint8_t bssid[ETH_ALEN],
            _IN_OPT_ uint8_t channel);
#endif
/* dev reset */
int HAL_Kv_Set(const char *key, const void *val, int len, int sync);
int HAL_Kv_Get(const char *key, void *val, int *buffer_len);
