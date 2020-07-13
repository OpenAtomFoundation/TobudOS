
#ifndef AT_CMD_WIFI_H
#define AT_CMD_WIFI_H

#include "proj_config.h"
#include "types.h"
#include "atcmd/at_list.h"
#define IP4_ADDR_LENGTH		4

char at_pvtcmd_set(char *str);

//station+softap
char at_set_wifi_mode(char *str);
char at_get_wifi_mode(char *str);
char at_help_wifi_mode(char *str);
char at_set_wifi_mode_current(char *str);
char at_get_wifi_mode_current(char *str);
char at_help_wifi_mode_current(char *str);
char at_set_wifi_mode_def(char *str);
char at_get_wifi_mode_def(char *str);
char at_help_wifi_mode_def(char *str);


//station
char at_station_setmac(char *str);
char at_station_getmac(char *str);
char at_station_setmac_current(char *str);
char at_station_getmac_current(char *str);
char at_station_setmac_def(char *str);
char at_station_getmac_def(char *str);
char at_station_connect(char *str);
char at_station_connect_current(char *str);
char at_station_get_connected_info(char *str);
char at_station_get_connected_info_current(char *str);
char at_station_get_connected_info_def(char *str);
char at_station_disconnect(void);
char at_station_set_scan_list_display_option(char *str);
char at_station_scan(char *str);
char at_station_scan_no_filter(char *str);
#if WIFI_TRACK
char at_notify_aplist(void);
char at_station_aplx(char *str);
char at_station_aplist(char *str);
#endif

//softap
char at_softap_setmac(char *str);
char at_softap_getmac(char *str);
char at_softap_setmac_current(char *str);
char at_softap_getmac_current(char *str);
char at_softap_setmac_def(char *str);
char at_softap_getmac_def(char *str);
char at_softap_set_config(char *str);
char at_softap_get_config(char *str);
char at_softap_set_config_current(char *str);
char at_softap_get_config_current(char *str);
char at_softap_set_config_def(char *str);
char at_softap_get_config_def(char *str);
char at_softap_get_station_list(char *str);

//get station ip
char at_get_sta_ip(char *str);
char at_get_sta_ip_cur(char *str);
char at_get_sta_ip_def(char *str);//!!!(esp8266)[AT+CIPSTA_DEF?]Why not take parameters from flash?
//set statation ip
char at_set_sta_ip_cur(char *str);
char at_set_sta_ip_def(char *str);
//get spatAP ip
char at_get_ap_ip(char *str);
char at_get_ap_ip_cur(char *str);
char at_get_ap_ip_def(char *str);//!!!(esp8266)[AT+CIPAP_DEF?]Why not take parameters from flash?
//set softAP ip
char at_set_ap_ip_cur(char *str);
char at_set_ap_ip_def(char *str);

//set dhcp
char at_set_dhcp_cur(char * str);
char at_set_dhcp_def(char * str);
//get dhcp
char at_get_dhcp_cur(char * str);
char at_get_dhcp_def(char * str);
//set dhcps
char at_set_dhcps_cur(char * str);
char at_set_dhcps_def(char * str);
//get dhcps
char at_get_dhcps_cur(char * str);
char at_get_dhcps_def(char * str);

//ping
char at_ping(char * str);

//get CPU usage
char at_get_cpu_usage(char * str);

//chip SN
char at_get_chip_sn(char * str);
char at_set_chip_sn(char * str);

//bt id
char at_get_bt_id(char * str);
char at_set_bt_id(char * str);

//bt mac
char at_get_bt_mac(char * str);
char at_set_bt_mac(char * str);

//CNVDS
char at_nvds(char * str);

//CEFUSE
char at_get_efuse(char * str);
char at_set_efuse(char * str);

//XTAL_COMP
char at_get_xtal_cap_comp(char * str);
char at_set_xtal_cap_comp(char * str);

//TXPOWER_COMP
char at_get_tx_power_comp(char * str);
char at_set_tx_power_comp(char * str);


//-------------All the code below is waiting to be deleted!----------------------

char at_set_station_auto_connect(char *str);
char at_get_station_auto_connect(char *str);

char at_set_sta_host_name(char *str);
char at_get_sta_host_name(char *str);
char at_set_ip_connect(char *str);
char at_set_tcp_server(char *str);
char at_set_udp_server(char *str);
char at_set_ip_send_ex_length(char *str);
char at_set_cip_mux(char *str);
char at_get_cip_mux(char *str);
char at_set_netconn_disconnect(char *str);
char at_set_ip_close(char *str);
char at_get_cip_status(char *str);
#if WIFI_SWITCH
char at_wifi_switch(char *str);
#endif

char at_iperf(char *str);


#endif


