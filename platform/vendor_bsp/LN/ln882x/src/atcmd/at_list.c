#include "proj_config.h"

#include "atcmd/at_list.h"
#include "atcmd/at_cmd_basic.h"
#include "atcmd/at_cmd_wifi.h"
#include "atcmd/at_parser.h"

void at_cmd_init(void)
{
    at_register_short_command("AT",NULL,NULL,NULL,(at_callback)at_at_excute);
    at_register_short_command("ATE0",NULL,NULL,NULL,(at_callback)at_ate0_excute);
    at_register_short_command("ATE1",NULL,NULL,NULL,(at_callback)at_ate1_excute);

    at_register_command("RST",NULL,NULL,NULL,(at_callback)at_rst_excute);
    at_register_command("GMR",NULL,NULL,NULL,(at_callback)at_gmr_excute);
    at_register_command("GSLP",NULL,(at_callback)at_gslp_excute,NULL,NULL);
    at_register_command("RESTORE",NULL,NULL,NULL,(at_callback)at_restore_excute);
    at_register_command("UART_CUR",(at_callback)at_uart_cur_get,NULL,NULL,NULL);
    at_register_command("SLEEP",(at_callback)at_sleep_get,(at_callback)at_sleep_set,NULL,NULL);
    at_register_command("WAKEUPGPIO",NULL,(at_callback)at_wakeupgpio_set,NULL,NULL);

    at_register_command("SYSGPIODIR",NULL,(at_callback)at_sysgpiodir_set,NULL,NULL);
    at_register_command("SYSGPIOWRITE",NULL,(at_callback)at_sysgpiowrite_set,NULL,NULL);

    at_register_command("SYSGPIOREAD",NULL,(at_callback)at_sysgpioread_set,NULL,NULL);

    at_register_command("PVTCMD",NULL,(at_callback)at_pvtcmd_set,NULL,NULL);
    //sta mac
    at_register_command("CIPSTAMAC", (at_callback)at_station_getmac, (at_callback)at_station_setmac, NULL, NULL);
    at_register_command("CIPSTAMAC_CUR", (at_callback)at_station_getmac_current, (at_callback)at_station_setmac_current, NULL, NULL);
    at_register_command("CIPSTAMAC_DEF", (at_callback)at_station_getmac_def, (at_callback)at_station_setmac_def,NULL,NULL);
    //softAP mac
    at_register_command("CIPAPMAC", (at_callback)at_softap_getmac, (at_callback)at_softap_setmac, NULL, NULL);
    at_register_command("CIPAPMAC_CUR", (at_callback)at_softap_getmac_current, (at_callback)at_softap_setmac_current, NULL, NULL);
    at_register_command("CIPAPMAC_DEF", (at_callback)at_softap_getmac_def, (at_callback)at_softap_setmac_def,NULL,NULL);
    //wifi mode
    at_register_command("CWMODE", (at_callback)at_get_wifi_mode, (at_callback)at_set_wifi_mode, (at_callback)at_help_wifi_mode, NULL);
    at_register_command("CWMODE_CUR", (at_callback)at_get_wifi_mode_current, (at_callback)at_set_wifi_mode_current, (at_callback)at_help_wifi_mode_current, NULL);
    at_register_command("CWMODE_DEF", (at_callback)at_get_wifi_mode_def, (at_callback)at_set_wifi_mode_def, (at_callback)at_help_wifi_mode_def, NULL);

    at_register_command("SYSCAL",NULL,(at_callback)at_cal_excute,NULL,NULL);
    //config station(connect to ap)
    at_register_command("CWJAP", (at_callback)at_station_get_connected_info, (at_callback)at_station_connect, NULL, NULL);
    at_register_command("CWJAP_CUR", (at_callback)at_station_get_connected_info_current, (at_callback)at_station_connect_current, NULL, NULL);
    at_register_command("CWJAP_DEF", (at_callback)at_station_get_connected_info_def, (at_callback)at_station_connect, NULL, NULL);

    at_register_command("CWLAPOPT",NULL,(at_callback)at_station_set_scan_list_display_option,NULL,NULL);
#if WIFI_TRACK
    at_register_command("CWLAPLN",NULL,(at_callback)at_station_scan,NULL,(at_callback)at_station_scan_no_filter);
	at_register_command("CWLAP",NULL,NULL,NULL,(at_callback)at_station_aplx);

    at_register_command("CWLAPLST",NULL,NULL,NULL,(at_callback)at_station_aplist);
#else
    at_register_command("CWLAP",NULL,(at_callback)at_station_scan,NULL,(at_callback)at_station_scan_no_filter);

#endif
    at_register_command("CWQAP",NULL,NULL,NULL,(at_callback)at_station_disconnect);
    //config softAP
    at_register_command("CWSAP", (at_callback)at_softap_get_config, (at_callback)at_softap_set_config, NULL, NULL);
    at_register_command("CWSAP_CUR", (at_callback)at_softap_get_config_current, (at_callback)at_softap_set_config_current, NULL, NULL);
    at_register_command("CWSAP_DEF", (at_callback)at_softap_get_config_def, (at_callback)at_softap_set_config_def, NULL, NULL);

    at_register_command("CWLIF", NULL, NULL, NULL, (at_callback)at_softap_get_station_list);

    at_register_command("CWAUTOCONN",(at_callback)at_get_station_auto_connect,(at_callback)at_set_station_auto_connect,NULL,NULL);
    at_register_command("CWHOSTNAME",(at_callback)at_get_sta_host_name,(at_callback)at_set_sta_host_name,NULL,NULL);

    at_register_command("CIPSTART",NULL,(at_callback)at_set_ip_connect,NULL,NULL);
    at_register_command("CIPSENDEX",NULL,(at_callback)at_set_ip_send_ex_length,NULL,NULL);
    at_register_command("CIP_TCP_SVR",NULL,(at_callback)at_set_tcp_server,NULL,NULL);
    at_register_command("CIP_UDP_SVR",NULL,(at_callback)at_set_udp_server,NULL,NULL);
    at_register_command("CIPMUX",(at_callback)at_get_cip_mux,(at_callback)at_set_cip_mux,NULL,NULL);
    at_register_command("CIPTCPDISCON",NULL,NULL,NULL,(at_callback)at_set_netconn_disconnect);
    at_register_command("CIPCLOSE",NULL,(at_callback)at_set_ip_close,NULL,NULL);

    //dhcp
    at_register_command("CWDHCP",    (at_callback)at_get_dhcp_def,  (at_callback)at_set_dhcp_def,NULL,NULL);
    at_register_command("CWDHCP_CUR",(at_callback)at_get_dhcp_cur,  (at_callback)at_set_dhcp_cur,NULL,NULL);
    at_register_command("CWDHCP_DEF",(at_callback)at_get_dhcp_def,  (at_callback)at_set_dhcp_def,NULL,NULL);
    //dhcps
    at_register_command("CWDHCPS_CUR",(at_callback)at_get_dhcps_cur,(at_callback)at_set_dhcps_cur,NULL,NULL);
    at_register_command("CWDHCPS_DEF",(at_callback)at_get_dhcps_def,(at_callback)at_set_dhcps_def,NULL,NULL);
    //sta ip
    at_register_command("CIPSTA",    (at_callback)at_get_sta_ip,    (at_callback)at_set_sta_ip_cur,NULL,NULL);
    at_register_command("CIPSTA_CUR",(at_callback)at_get_sta_ip_cur,(at_callback)at_set_sta_ip_cur,NULL,NULL);
    at_register_command("CIPSTA_DEF",(at_callback)at_get_sta_ip_def,(at_callback)at_set_sta_ip_def,NULL,NULL);
    //ap ip
    at_register_command("CIPAP",     (at_callback)at_get_ap_ip,     (at_callback)at_set_ap_ip_cur,NULL,NULL);
    at_register_command("CIPAP_CUR", (at_callback)at_get_ap_ip_cur, (at_callback)at_set_ap_ip_cur,NULL,NULL);
    at_register_command("CIPAP_DEF", (at_callback)at_get_ap_ip_def, (at_callback)at_set_ap_ip_def,NULL,NULL);
    //get ip&mac
    at_register_command("CIFSR",NULL,NULL,NULL,NULL);

    //ping
    at_register_command("PING",NULL,(at_callback)at_ping,NULL,NULL);
    at_register_command("IPERF",NULL,(at_callback)at_iperf,NULL,NULL);

    //CPU usage
    at_register_command("CPUUSG",(at_callback)at_get_cpu_usage,NULL,NULL,(at_callback)at_get_cpu_usage);
    
    //bt ID
    at_register_command("CBTID", (at_callback)at_get_bt_id, (at_callback)at_set_bt_id,NULL,NULL);
    //bt MAC
    at_register_command("CBTMAC", (at_callback)at_get_bt_mac, (at_callback)at_set_bt_mac,NULL,NULL);

    //chip SN
    at_register_command("CSN", (at_callback)at_get_chip_sn, (at_callback)at_set_chip_sn,NULL,NULL);

    //CNVDS
    at_register_command("CNVDS",NULL, (at_callback)at_nvds,  NULL,NULL);

    //CEFUSE
    at_register_command("CEFUSE", (at_callback)at_get_efuse, (at_callback)at_set_efuse, NULL,NULL);

    //XTAL_COMP
    at_register_command("XTAL_COMP", (at_callback)at_get_xtal_cap_comp, (at_callback)at_set_xtal_cap_comp, NULL,NULL);
    
    //TXPOWER_COMP
    at_register_command("TXPOWER_COMP", (at_callback)at_get_tx_power_comp, (at_callback)at_set_tx_power_comp, NULL,NULL);

#if WIFI_SWITCH
    at_register_command("WF_SWITCH",NULL,(at_callback)at_wifi_switch,NULL,NULL);
#endif

}



