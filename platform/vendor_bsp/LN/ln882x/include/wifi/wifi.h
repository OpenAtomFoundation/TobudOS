#ifndef __WIFI_H__
#define __WIFI_H__
#include "types.h"
#include "utils/list.h"

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

#define SSID_MAX_LEN            33
#define PASSWORD_MAX_LEN        64
#define MAC_ADDRESS_LEN         6
#define BSSID_LEN               MAC_ADDRESS_LEN

/*
* This macro defines the maximum size of the scanned AP list, which can be modified by the customer, Maximum value is 50.
*/
#define SCANNED_AP_LIST_SIZE          (32)

typedef enum {
    STATION_IF = 0,         /**<  station interface */
    SOFT_AP_IF,             /**<  soft-AP interface */
    MONITOR_IF,             /**<  Monitor interface */
    WIFI_IF_NUM,
} wifi_interface_enum_t;

typedef enum {
    AP_LIST_BY_SIGNAL = 0,        /**<  Sort match AP in scan list by RSSI */
    AP_LIST_BY_SECURITY,          /**<  Sort match AP in scan list by security mode */
} ap_list_sort_method_t;


typedef enum
{
    WIFI_SCAN_TYPE_ACTIVE,   //active scan
	WIFI_SCAN_TYPE_PASSIVE,  //passive scan
}wifi_scan_type_t;

typedef enum {
	CIPHER_NONE,      /**<  cipher type is none */
	CIPHER_WEP40,     /**<  cipher type is WEP40 */
	CIPHER_WEP104,    /**<  cipher type is WEP104 */
	CIPHER_TKIP,      /**<  cipher type is TKIP */
	CIPHER_CCMP,      /**<  cipher type is CCMP */
	CIPHER_TKIP_CCMP, /**<  cipher type is TKIP and CCMP */
	CIPHER_UNKNOWN    /**<  cipher type is unknown */
} cipher_type_enum_t;

typedef enum {
    WIFI_AUTH_OPEN = 0,         /**<  authenticate mode : open */
    WIFI_AUTH_WEP,              /**<  authenticate mode : WEP */
    WIFI_AUTH_WPA_PSK,          /**<  authenticate mode : WPA_PSK */
    WIFI_AUTH_WPA2_PSK,         /**<  authenticate mode : WPA2_PSK */
    WIFI_AUTH_WPA_WPA2_PSK,     /**<  authenticate mode : WPA_WPA2_PSK */
    WIFI_AUTH_WPA2_ENTERPRISE,  /**<  authenticate mode : WPA2_ENTERPRISE */
    WIFI_AUTH_MAX
} wifi_auth_mode_enum_t;

typedef enum {
	WIFI_COUNTRY_POLICY_AUTO,    //Country policy is auto, use the country info of AP to which the station is connected
	WIFI_COUNTRY_POLICY_MANUAL   //Country policy is manual, always use the configured country info
} wifi_country_polocy_enum_t;

typedef enum
{
    WIFI_MODE_STATION,     /**<  station mode */
    WIFI_MODE_AP,     /**<  SoftAP mode */
    WIFI_MODE_MONITOR,
    WIFI_MODE_AP_STATION,   /**<  Station + SoftAP mode */
    WIFI_MODE_MAX
} wifi_mode_enum_t;
typedef enum
{
    WIFI_NO_POWERSAVE = 0,
    WIFI_MIN_POWERSAVE,
    WIFI_MAX_POWERSAVE,
} sta_powersave_mode_enum_t;

typedef enum{
    WIFI_STA_STATUS_READY = 0,
    WIFI_STA_STATUS_SCANING,
    WIFI_STA_STATUS_CONNECTING,
    WIFI_STA_STATUS_CONNECTED,
    WIFI_STA_STATUS_DISCONNECTING,
    WIFI_STA_STATUS_DISCONNECTED,
}wifi_station_status_enum_t;

typedef enum{
    WIFI_AP_STATUS_READY = 0,
    WIFI_AP_STATUS_STACONNECTED,
    WIFI_AP_STATUS_STADISCONNECTED,
}wifi_softap_status_enum_t;

typedef enum {
    WLAN_DHCP_DISABLE = 0,
    WLAN_DHCP_CLIENT,
    WLAN_DHCP_SERVER,
    WLAN_STATIC_IP = WLAN_DHCP_DISABLE,
}dhcp_mode_enum_t;

typedef struct {
    wifi_mode_enum_t            wifi_mode;
    sta_powersave_mode_enum_t   sta_ps_mode;
    dhcp_mode_enum_t            dhcp_mode;              /* DHCP mode, @ref DHCP_Disable, @ref DHCP_Client and @ref DHCP_Server. */
    char                        local_ip_addr[16];      /* Static IP configuration, Local IP address. */
    char                        net_mask[16];           /* Static IP configuration, Netmask. */
    char                        gateway_ip_addr[16];    /* Static IP configuration, Router IP address. */
    char                        dns_server_ip_addr[16]; /* Static IP configuration, DNS server IP address. */
    int                         scanned_ap_list_size;   /* In station mode, this parameter defines the list size of APS list*/
}wifi_init_type_t;

/** @brief Aggregate of active & passive scan time per channel */
typedef struct {
    uint16_t active;  /**< active scan time per channel, units: millisecond. */
    uint16_t passive;  /**< passive scan time per channel, units: millisecond, values above 1500ms may
                            cause station to disconnect from AP and are not recommended. */
} wifi_scan_time_t;

/** @brief STA configuration settings for the LN882x */
typedef struct {
    uint8_t                    ssid[SSID_MAX_LEN];     /**<  SSID of target AP*/
    uint8_t                    password[PASSWORD_MAX_LEN]; /**<  password of target AP*/
    uint8_t                    bssid[BSSID_LEN];     /**<  MAC address of target AP*/
    uint8_t                    channel;      /**<  channel of target AP. Set to 1~13 to scan starting from the specified channel before connecting to AP. If the channel of AP is unknown, set it to 0.*/
    bool                       bssid_set;    /**<  whether set MAC address of target AP or not. Generally, station_config.bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.*/
} wifi_sta_config_t;

/** @brief Soft-AP configuration settings for the LN882x */
typedef struct {
    uint8_t               ssid[SSID_MAX_LEN];        /**<  SSID of LN882x soft-AP */
    uint8_t               password[PASSWORD_MAX_LEN];    /**<  Password of LN882x soft-AP */
    uint8_t               ssid_len;        /**<  Length of SSID. If softap_config.ssid_len==0, check the SSID until there is a termination character; otherwise, set the SSID length according to softap_config.ssid_len. */
    uint8_t               channel;         /**<  Channel of LN882x soft-AP */
    wifi_auth_mode_enum_t authmode;        /**<  Auth mode of LN882x soft-AP. Do not support AUTH_WEP in soft-AP mode */
    uint8_t               ssid_hidden;     /**<  Broadcast SSID or not, default 0, broadcast the SSID */
    uint8_t               max_connection;  /**<  Max number of stations allowed to connect in, default 4, max 4 */
    uint16_t              beacon_interval; /**<  Beacon interval, 100 ~ 60000 ms, default 100 ms */
    uint16_t              reserved;
} wifi_ap_config_t;

typedef union {
    wifi_ap_config_t  ap;  /**< configuration of AP */
    wifi_sta_config_t sta; /**< configuration of STA */
} wifi_config_t;

/** @brief Parameters for an SSID scan. */
typedef struct wifi_scan_config{
    uint8_t ssid[SSID_MAX_LEN];               /**< SSID of AP */
    uint8_t bssid[BSSID_LEN];              /**< MAC address of AP */
    uint8_t channel;             /**< channel, scan the specific channel */
    bool show_hidden;            /**< enable to scan AP whose SSID is hidden */
    wifi_scan_type_t scan_type;  /**< scan type, active or passive */
    wifi_scan_time_t scan_time;  /**< scan time per channel */
} wifi_scan_config_t;

typedef struct {
    bool     valid;
	uint8_t  id;       //MAX = 最大连接个数
    uint8_t  bssid[6];
    uint32_t ip;
} station_info_t;

typedef struct {
    list_t                list;
    uint8_t               bssid[BSSID_LEN];
    uint8_t               ssid[SSID_MAX_LEN];
    uint8_t               ssid_len;
    uint8_t               channel;
    wifi_auth_mode_enum_t authmode;
    uint8_t               imode;
    int8_t                rssi;
    uint8_t               is_hidden;
    int16_t               freq_offset;
    int16_t               freqcal_val;
    uint8_t               wps;
    int                   rest_lifetime;
} ap_info_t;

void wifi_rf_preprocess(void);
void wifi_rf_image_cal(void);
void wifi_temp_cal_init(uint16_t adc_ch0_val);
void wifi_do_temp_cal_period(uint16_t adc_ch0_val);

//STA
bool wifi_station_scan(wifi_scan_config_t *config);
int wifi_station_get_scan_list_size(void);
int wifi_station_get_scan_list(list_t *out_list, int out_list_size, bool sort);
void wifi_station_connect(wifi_sta_config_t *sta_config);
bool wifi_station_disconnect(void);
int8_t  wifi_station_get_rssi(void);
bool wifi_station_set_dtim_period(int period);

//AP
bool wifi_softap_disconnect(void);

//COMMON
bool wifi_init(void);
bool wifi_deinit(void);
bool wifi_start(wifi_init_type_t *init_param);
void wifi_stop(void);
bool wifi_set_mode(wifi_mode_enum_t mode);//设置wifi mode, 同时写到Flash参数区
bool wifi_set_mode_current(wifi_mode_enum_t mode);//设置wifi mode, 不写到Flash参数区
wifi_mode_enum_t wifi_get_mode(void);//从Flash参数区读取
wifi_mode_enum_t wifi_get_mode_current(void);//获取当前wifi mode, 从RAM中读取
wifi_mode_enum_t wifi_get_mode_default(void);//从出厂默认参数中读取
bool wifi_get_config(wifi_interface_enum_t if_index, wifi_config_t *config);
bool wifi_get_config_default (wifi_interface_enum_t if_index, wifi_config_t *config);
bool wifi_set_config (wifi_interface_enum_t if_index, wifi_config_t *config);
bool wifi_set_config_current (wifi_interface_enum_t if_index, wifi_config_t *config);
bool wifi_get_config_current(wifi_interface_enum_t if_index, wifi_config_t *config);

bool wifi_get_macaddr (wifi_interface_enum_t if_index, uint8_t *macaddr);//read from flash
bool wifi_set_macaddr (wifi_interface_enum_t if_index, uint8_t *macaddr);
bool wifi_set_macaddr_current (wifi_interface_enum_t if_index, uint8_t *macaddr);
bool wifi_get_macaddr_current (wifi_interface_enum_t if_index, uint8_t *macaddr);//read from RAM

uint8_t wifi_get_channel(void);
bool wifi_set_channel (uint8_t channel);
bool wifi_get_link_status (wifi_interface_enum_t if_index, int *status);
bool wifi_private_command(char *pvtcmd);

uint8_t * get_wifi_lib_version(void);

/** @brief Received packet radio metadata header, this is the common header at the beginning of all promiscuous mode RX callback buffers */
typedef struct {
    uint8_t rssi;
} wifi_pkt_rx_ctrl_t;

/*
 * @brief Payload passed to 'buf' parameter of promiscuous mode RX callback.
 */
typedef struct {
    wifi_pkt_rx_ctrl_t rx_ctrl; /**< metadata header */
    uint8_t payload[1];       /**< Data or management payload. Length of payload is described by rx_ctrl.sig_len. Type of content determined by packet type argument of callback. */
} wifi_promiscuous_pkt_t;

/**
  * @brief Promiscuous frame type
  *
  * Passed to promiscuous mode RX callback to indicate the type of parameter in the buffer.
  *
  */
typedef enum {
    WIFI_PKT_MGMT = 0, /**< Management frame, indicates 'buf' argument is wifi_promiscuous_pkt_t */
    WIFI_PKT_CTRL,     /**< Control frame, indicates 'buf' argument is wifi_promiscuous_pkt_t */
    WIFI_PKT_DATA,     /**< Data frame, indiciates 'buf' argument is wifi_promiscuous_pkt_t */
    WIFI_PKT_MISC,     /**< Other type, such as MIMO etc. 'buf' argument is wifi_promiscuous_pkt_t but the payload is zero length. */
} wifi_promiscuous_pkt_type_t;

#define WIFI_PROMIS_FILTER_MASK_ALL         (0x7FFFFF)  /**< filter all packets */
#define WIFI_PROMIS_FILTER_MASK_MGMT        (1)           /**< filter the packets with type of WIFI_PKT_MGMT */
#define WIFI_PROMIS_FILTER_MASK_CTRL        (1<<1)        /**< filter the packets with type of WIFI_PKT_CTRL */
#define WIFI_PROMIS_FILTER_MASK_DATA        (1<<2)        /**< filter the packets with type of WIFI_PKT_DATA */
#define WIFI_PROMIS_FILTER_MASK_MISC        (1<<3)        /**< filter the packets with type of WIFI_PKT_MISC */
#define WIFI_PROMIS_FILTER_MASK_DATA_MPDU   (1<<4)        /**< filter the MPDU which is a kind of WIFI_PKT_DATA */
#define WIFI_PROMIS_FILTER_MASK_DATA_AMPDU  (1<<5)        /**< filter the AMPDU which is a kind of WIFI_PKT_DATA */

#define WIFI_PROMIS_CTRL_FILTER_MASK_ALL         (0xFF800000)  /**< filter all control packets */
#define WIFI_PROMIS_CTRL_FILTER_MASK_WRAPPER     (1<<23)       /**< filter the control packets with subtype of Control Wrapper */
#define WIFI_PROMIS_CTRL_FILTER_MASK_BAR         (1<<24)       /**< filter the control packets with subtype of Block Ack Request */
#define WIFI_PROMIS_CTRL_FILTER_MASK_BA          (1<<25)       /**< filter the control packets with subtype of Block Ack */
#define WIFI_PROMIS_CTRL_FILTER_MASK_PSPOLL      (1<<26)       /**< filter the control packets with subtype of PS-Poll */
#define WIFI_PROMIS_CTRL_FILTER_MASK_RTS         (1<<27)       /**< filter the control packets with subtype of RTS */
#define WIFI_PROMIS_CTRL_FILTER_MASK_CTS         (1<<28)       /**< filter the control packets with subtype of CTS */
#define WIFI_PROMIS_CTRL_FILTER_MASK_ACK         (1<<29)       /**< filter the control packets with subtype of ACK */
#define WIFI_PROMIS_CTRL_FILTER_MASK_CFEND       (1<<30)       /**< filter the control packets with subtype of CF-END */
#define WIFI_PROMIS_CTRL_FILTER_MASK_CFENDACK    (uint32_t)((uint32_t)1<<31)       /**< filter the control packets with subtype of CF-END+CF-ACK */


/** @brief Mask for filtering different packet types in promiscuous mode. */
typedef struct {
    uint32_t filter_mask; /**< OR of one or more filter values WIFI_PROMIS_FILTER_* */
} wifi_promiscuous_filter_t;

/**
  * @brief The RX callback function in the promiscuous mode.
  *        Each time a packet is received, the callback function will be called.
  *
  * @param buf  Data received. Type of data in buffer (wifi_promiscuous_pkt_t or wifi_pkt_rx_ctrl_t) indicated by 'type' parameter.
  * @param type  promiscuous packet type.
  *
  */
typedef void (* wifi_promiscuous_cb_t)(void *buf, uint16_t len, wifi_promiscuous_pkt_type_t type);

/**
  * @brief Register the RX callback function in the promiscuous mode.
  *
  * Each time a packet is received, the registered callback function will be called.
  *
  * @param cb  callback
  */
void wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

/**
  * @brief     Enable the promiscuous mode.
  *
  * @param     en  false - disable, true - enable
  *
  * @return
  *    - true: succeed
  *    - false: WiFi is not initialized by wifi_init
  */
bool wifi_set_promiscuous(bool en);

/**
  * @brief     Get the promiscuous mode.
  *
  * @param[out] en  store the current status of promiscuous mode
  *
  * @return
  *    - true: succeed
  *    - false: WiFi is not initialized by wifi_init or invalid argument
  *
  */
bool wifi_get_promiscuous(bool *en);

/**
  * @brief Enable the promiscuous mode packet type filter.
  *
  * @note The default filter is to filter all packets except WIFI_PKT_MISC
  *
  * @param filter the packet type filtered in promiscuous mode.
  *
  * @return
  *    - true: succeed
  *    - false: WiFi is not initialized by wifi_init
  */
bool wifi_set_promiscuous_filter(const wifi_promiscuous_filter_t *filter);

/**
  * @brief     Get the promiscuous filter.
  *
  * @param[out] filter  store the current status of promiscuous filter
  *
  * @return
  *    - true: succeed
  *    - false: WiFi is not initialized by wifi_init or invalid argument
  */
bool wifi_get_promiscuous_filter(wifi_promiscuous_filter_t *filter);



//wifi advanced API
void wifi_send_80211_mgmt_raw_frame(uint8_t *data, int len);

void wifi_if_send_ethernet_pkt(uint8_t *data, int len);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __WIFI_H__ */
