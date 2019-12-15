#include "infra_types.h"
#include "infra_defs.h"
#include "wrappers_defs.h"
/*************************************** common hals ***************************************/
int HAL_Timer_Stop(void *timer);
int HAL_Timer_Delete(void *timer);
void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data);
int HAL_Timer_Start(void *timer, int ms);
void HAL_SleepMs(uint32_t ms);
void *HAL_Malloc(uint32_t size);
void HAL_MutexLock(void *mutex);
void HAL_MutexUnlock(void *mutex);
uint64_t HAL_UptimeMs(void);
void HAL_Free(void *ptr);
void *HAL_MutexCreate(void);
void HAL_MutexDestroy(void *mutex);

/*************************************** wifi provision frameworks hals ***************************************/
/* frameworks/awss.c*/
int HAL_Awss_Get_Timeout_Interval_Ms(void);
int HAL_Sys_Net_Is_Ready();
int HAL_Wifi_Get_Ap_Info(char ssid[HAL_MAX_SSID_LEN],char passwd[HAL_MAX_PASSWD_LEN],uint8_t bssid[ETH_ALEN]);
/* awss_crypt.c */
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN]);
int HAL_GetProductSecret(char *product_secret);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN]);
void HAL_Awss_Close_Monitor(void);
void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb);
void HAL_Awss_Switch_Channel(char primary_channel, char secondary_channel, uint8_t bssid[ETH_ALEN]);
int HAL_Awss_Get_Channelscan_Interval_Ms(void);
/* zconfig_vendor_common.c */
p_HAL_Aes128_t HAL_Aes128_Init(
            _IN_ const uint8_t *key,
            _IN_ const uint8_t *iv,
            _IN_ AES_DIR_t dir);
int HAL_Aes128_Destroy(_IN_ p_HAL_Aes128_t aes);
int HAL_Aes128_Cbc_Decrypt(
            _IN_ p_HAL_Aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OU_ void *dst);
/* os_misc.c */
char *HAL_Wifi_Get_Mac(_OU_ char mac_str[HAL_MAC_LEN]);
/* awss_main.c */
int HAL_Awss_Connect_Ap(
            _IN_ uint32_t connection_timeout_ms,
            _IN_ char ssid[HAL_MAX_SSID_LEN],
            _IN_ char passwd[HAL_MAX_PASSWD_LEN],
            _IN_OPT_ enum AWSS_AUTH_TYPE auth,
            _IN_OPT_ enum AWSS_ENC_TYPE encry,
            _IN_OPT_ uint8_t bssid[ETH_ALEN],
            _IN_OPT_ uint8_t channel);
int HAL_Awss_Close_Ap();
/*************************************** dev-ap special hals ***************************************/
int HAL_Awss_Open_Ap(const char *ssid, const char *passwd, int beacon_interval, int hide);
