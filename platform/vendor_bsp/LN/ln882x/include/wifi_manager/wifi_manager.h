#ifndef __WIFI_MANAGER_H__
#define __WIFI_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "wifi/wifi.h"
	
//--------------------------------------------------------------------------------------------------
typedef struct {
	uint8_t  mac[6];
} softap_mode_probereq_recv_msg_t;

typedef struct {
	uint8_t  mac[6];
} softap_mode_sta_connected_msg_t;

typedef struct {
	uint8_t  mac[6];
} softap_mode_sta_disconnected_msg_t;

typedef struct {
	wifi_auth_mode_enum_t old_authmode;
	wifi_auth_mode_enum_t new_authmode;
} sta_mode_authmode_change_msg_t;

typedef struct {
	uint8_t ssid[32];
	uint8_t ssid_len;
	uint8_t bssid[6];
	uint8_t channel;
} sta_mode_connected_msg_t;

typedef struct {
	uint8_t ssid[32];
	uint8_t ssid_len;
	uint8_t bssid[6];
	uint8_t channel;
} sta_mode_disconnected_msg_t;

typedef struct {
	uint32_t ip;
	uint32_t mask;
	uint32_t gw;
} sta_mode_got_ip_msg_t;

typedef struct {
	ap_info_t ap_info;
} wifi_recv_beacon_msg_t;




typedef enum 
{
	WIFI_MSG_ID_WIFI_INVALID = 0,       //STA无效	                            
	WIFI_MSG_ID_STA_READY,              //wifi ready
	WIFI_MSG_ID_STA_SCAN_DONE,          //STA扫描完毕
	WIFI_MSG_ID_STA_CONNECTED,          //STA连接上AP
	WIFI_MSG_ID_STA_DISCONNECTED,       //STA断开AP
	WIFI_MSG_ID_STA_AUTHMODE_CHANGE,    //身份验证模式改变
    WIFI_MSG_ID_STA_DHCP_GETING_IP,     //DHCP 正在获取IP
    WIFI_MSG_ID_STA_DHCP_GOT_IP,        //DHCP 获取到IP
	WIFI_MSG_ID_STA_DHCP_TIMEOUT,       //DHCP 获取IP超时    
    WIFI_MSG_ID_STA_USE_STATIC_IP,      //STA use static IP   
	WIFI_MSG_ID_AP_READY,               //LN882x soft-AP ready
	WIFI_MSG_ID_AP_STACONNECTED,        //a station connected to WIFI soft-AP
	WIFI_MSG_ID_AP_STADISCONNECTED,     //a station disconnected to LN882x soft-AP
	WIFI_MSG_ID_AP_PROBEREQRECVED,      //Receive probe request packet in soft-AP interface
    WIFI_MSG_ID_RECV_BEACON,            //Receive beacon
    WIFI_MSG_ID_MAX
} wifi_msg_id_enum_t;

typedef struct {
    wifi_msg_id_enum_t msg_id;
    uint16_t           msg_len;
    uint8_t            msg[60];
} wifi_msg_t;

typedef void(* wifi_msg_callback_fun)(wifi_msg_t * msg);

typedef struct{
    wifi_msg_callback_fun  fun_callback;
} wifi_msg_handle_table_t;

//wifi manager 维护这个状态
typedef enum {
	STA_STATUS_IDLE,                //station idle  没有连上AP，处于被动扫描状态
	STA_STATUS_CONNECTING,          //station is connecting to AP  //connect后，暂未获取到结果
	STA_STATUS_WRONG_PASSWORD,      //the password is wrong   //
	STA_STATUS_NO_TARGET_AP_FOUND,  //station can not find the target AP  //没有目标AP
	STA_STATUS_CONNECT_FAIL,        //station fail to connect to AP       //暂时依据timeout判断
	STA_STATUS_GOT_IP,              //station got IP address from AP
	STA_STATUS_USE_STATIC_IP        //station use static IP
} station_status_enum_t;
//station_status_enum_t g_station_status;

struct wifi_manager_ctrl;
typedef struct wifi_manager_ctrl wifi_manager_ctrl_t;

//wifi get mode api (需要wifi_cfg 提供)

bool reg_wifi_msg_callbcak(wifi_msg_id_enum_t msg_id, wifi_msg_callback_fun callback_fun);
void notify_wifi_manager_task(wifi_msg_t * pWifiMsg);
bool wifi_manager_init(void);
bool wifi_manager_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_MANAGER_H__ */


