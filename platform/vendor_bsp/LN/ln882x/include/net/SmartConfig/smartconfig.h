
#ifndef _WIFI_SMART_CONFIG_H
#define _WIFI_SMART_CONFIG_H
#include "types.h"

/**
 * @brief WLAN SSID and passphrase definition
 */
#define WLAN_SSID_MAX_LEN         32
#define WLAN_PASSWORD_MIN_LEN     8
#define WLAN_PASSWORD_MAX_LEN     63

typedef enum {
    SMART_CONFIG_SUCCESS =  0, /* success */
    SMART_CONFIG_FAIL    = -1, /* general error */
    SMART_CONFIG_TIMEOUT = -2, /* wait timeout */
    SMART_CONFIG_INVALID = -3, /* invalid argument */
} smartconfig_status_t;

typedef struct{
    uint8_t ssid[WLAN_SSID_MAX_LEN];
    uint8_t ssid_len;
    uint8_t passphrase[WLAN_PASSWORD_MAX_LEN + 1]; /* ASCII string ending with '\0' */
    uint8_t random_num;
} smartconfig_result_t;


int wlan_smart_config_stop(void);


#endif

