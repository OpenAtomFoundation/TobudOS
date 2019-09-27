/***************************************************************************** 
* 
* File Name : tls_param.h 
* 
* Description: param manager Module 
* 
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. 
* All rights reserved. 
* 
* Author : dave
* 
* Date : 2014-6-12
*****************************************************************************/ 
#ifndef TLS_PARAM_H
#define TLS_PARAM_H
#include "wm_type_def.h"



struct tls_param_quick_connect{
    bool quick_connect_en;
    u8 chanId;
};

struct tls_sys_param {
	struct tls_param_hardware_version hardware_version;

	struct tls_param_bssid bssid;
	struct tls_param_ssid ssid;
	u8 channel_enable;
	u8 channel;
	u8 wireless_region;
	u8 encry;
	u8 wireless_protocol;
	u8 auto_create_adhoc;
	u8 auto_retrycnt;
	u8 auto_roam;
	u8 auto_powersave;
	u8 ssid_broadcast_enable;
	u16 channellist;
	struct tls_param_key key;
	struct tls_param_bgr wbgr;
	struct tls_param_wps wps;

	struct tls_param_ip ipcfg;
	
	u8 local_dnsname[32];
	u8 local_device_name[32];
	
	struct tls_param_oray_client oray_client_setting;

	u8 upnp_enable;
	
	u8 always_online;
	
	u8 user_port_mode;

	u8 res_2;
	
	struct tls_param_uart uart_cfg;
	
	u8 auto_mode;
	u8 res_3;
	u16 transparent_trigger_length;
	u16 transparent_trigger_period;
	
	struct tls_param_socket remote_socket_cfg;
	
	u32 debug_mode;
	u32 res_4[2];  /* reserv and mark the end */

	/*escape char*/
	u8  EscapeChar;
	u8  EscReserved;
	u16 EscapePeriod; /* unit: ms */

	u8 IoMode;
	u8 CmdMode;
	u8 PassWord[6];

	struct tls_webs_cfg WebsCfg;
	u8 oneshotflag;
	u8 oneshotres[2];
	struct tls_param_sha1 psk;
	struct tls_param_original_key original_key;
	struct tls_param_ssid original_ssid;
	u8 auto_reconnect;
	u8 res_5[3];

    struct tls_param_quick_connect quick_connect;

	u8 key_changed;
	u8 ssid_changed;

	struct tls_param_ssid apsta_ssid;
	struct tls_param_sha1 apsta_psk;
	u8 res_6[1];
	u8 channel4softap;
    u8 encry4softap;
    struct tls_param_key key4softap;
	struct tls_param_ip ipcfg4softap;
	struct tls_param_bgr wbgr4softap;

	char sntp_service1[32];
	char sntp_service2[32];
	char sntp_service3[32];
    struct tls_param_tem_offset params_tem;
};

struct tls_param_flash {
	u32 magic;
	u16 partition_num;
	u16 modify_count;
	u32 resv_1;
	u16 resv_2;
	u16 length;
	struct tls_sys_param parameters;
	u32 crc32;
};


#endif /* WM_PARAM_H */


