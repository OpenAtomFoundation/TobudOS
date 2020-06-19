#include "cmsis_os.h"
#include "osal/osal.h"
#include "utils/debug/log.h"
#include "utils/debug/art_assert.h"
#include "utils/art_string.h"
#include "wifi/wifi.h"
#include "ping/ping.h"
#include "netif/ethernetif.h"
#include "wifi_manager/wifi_manager.h"
#include "lwip/tcpip.h"
#include "drv_adc_measure.h"
#include "utils/system_parameter.h"
#include "hal/hal_adc.h"

static OS_Thread_t g_temp_cal_thread;
#define TEMP_APP_TASK_STACK_SIZE   4*256 //Byte

volatile uint8_t dhcp_get_ip = 0;

void wifi_init_sta(void)
{
    uint8_t macaddr[6] = {0}, macaddr_default[6] = {0};
    wifi_config_t wifi_config = {
        .sta = {
            .ssid     = "Supowang",
            .password = "13975426138",
            0,
        },
    };
    
    wifi_config_t temp_config = {0};
    
    wifi_init_type_t init_param = {
        .wifi_mode = WIFI_MODE_STATION,
        .sta_ps_mode = WIFI_NO_POWERSAVE,
        #if 1
        .dhcp_mode = WLAN_DHCP_CLIENT,
        #else
        .dhcp_mode = WLAN_STATIC_IP,
        .local_ip_addr = "192.168.1.110",
        .net_mask = "255.255.255.0",
        .gateway_ip_addr = "192.168.1.1",
        #endif
        //In station mode, define the length of the AP list scanned.
        .scanned_ap_list_size = SCANNED_AP_LIST_SIZE,
    };

    
    //Set wifi mode
    wifi_set_mode(init_param.wifi_mode);

    //Check mac address
    system_parameter_get_wifi_macaddr_default(STATION_IF, macaddr_default);
    wifi_get_macaddr(STATION_IF, macaddr);
    if(is_valid_mac((const char *)macaddr) && memcmp(macaddr, macaddr_default, 6) != 0){
        //If there is a valid MAC in flash, use it
        wifi_set_macaddr_current(STATION_IF, macaddr);
    }else{
        //generate random macaddr
        generate_mac_randomly(macaddr);
        wifi_set_macaddr(STATION_IF, macaddr);
    }

    //Check config
    wifi_get_config(STATION_IF, &temp_config);
    if(strlen((const char *)temp_config.sta.ssid) > 0){
        //If there is a valid config in flash, use it;
        wifi_set_config_current(STATION_IF, &temp_config);
    }else{
        //else, use the prev wifi_config and save it to flash.
        wifi_set_config(STATION_IF, &wifi_config);
    }

    //Startup WiFi.
    if(!wifi_start(&init_param)){//WIFI_MAX_POWERSAVE
        LOG(LOG_LVL_ERROR, "[%s, %d]wifi_start() fail.\r\n", __func__, __LINE__);
    }
}

void wifi_init_ap(void)
{
    uint8_t macaddr[6] = {0}, macaddr_default[6] = {0};
    wifi_config_t wifi_config = {
        .ap = {
            .ssid            = "Supowang",
            .ssid_len        = strlen("Supowang"),
            .password        = "Supowang",
            .channel         = 1,
            .authmode        = WIFI_AUTH_OPEN,
            .ssid_hidden     = 0,
            .max_connection  = 4,
            .beacon_interval = 100,
            .reserved        = 0,
        },
    };
    
    wifi_config_t temp_config = {0};
    
    wifi_init_type_t init_param = {
        .wifi_mode = WIFI_MODE_AP,
        .sta_ps_mode = WIFI_NO_POWERSAVE,
        .dhcp_mode = WLAN_DHCP_SERVER,
    };
    
    //Set wifi mode
    wifi_set_mode(init_param.wifi_mode);

    //Check mac address
    system_parameter_get_wifi_macaddr_default(SOFT_AP_IF, macaddr_default);
    wifi_get_macaddr(SOFT_AP_IF, macaddr);
    if(is_valid_mac((const char *)macaddr) && memcmp(macaddr, macaddr_default, 6) != 0){
        //If there is a valid MAC in flash, use it
        wifi_set_macaddr_current(SOFT_AP_IF, macaddr);
    }else{
        //generate random macaddr
        generate_mac_randomly(macaddr);
        wifi_set_macaddr(SOFT_AP_IF, macaddr);
    }

    //Check config
    wifi_get_config(SOFT_AP_IF, &temp_config);
    if(strlen((const char *)temp_config.sta.ssid) > 0){
        //If there is a valid config in flash, use it;
        wifi_set_config_current(SOFT_AP_IF, &temp_config);
    }else{
        //else, use the prev wifi_config and save it to flash.
        wifi_set_config(SOFT_AP_IF, &wifi_config);
    }

    //Startup WiFi.
    if(!wifi_start(&init_param)){
        LOG(LOG_LVL_ERROR, "[%s, %d]wifi_start() fail.\r\n", __func__, __LINE__);
    }
}


void temp_cal_app_task_entry(void *params)
{
    drv_adc_init();
    OS_MsDelay(1);
    wifi_temp_cal_init(drv_adc_read(ADC_CHAN_0));

    while(1)
    {
        OS_MsDelay(1);
        wifi_do_temp_cal_period(drv_adc_read(ADC_CHAN_0));
        OS_MsDelay(1000);
    }
}

void wifi_event_sta_got_ip_cb(wifi_msg_t * msg)
{
    dhcp_get_ip =  1;
};

extern void mqtt_iothub(void);
void application_entry(void *arg)
{
    //wifi chip temperature calibration.
    if(OS_OK != OS_ThreadCreate(&g_temp_cal_thread, "TempAPP", temp_cal_app_task_entry, NULL, OS_PRIORITY_BELOW_NORMAL, TEMP_APP_TASK_STACK_SIZE)) {
        ART_ASSERT(1);
    }
    
    reg_wifi_msg_callbcak(WIFI_MSG_ID_STA_DHCP_GOT_IP,wifi_event_sta_got_ip_cb);

    wifi_mode_enum_t wifi_mode = WIFI_MODE_STATION;

    tcpip_ip_info_t ip_info = {0};
    wifi_interface_enum_t if_index;

    if(wifi_mode == WIFI_MODE_STATION){
        if_index = STATION_IF;
        wifi_init_sta();
    }else if(wifi_mode == WIFI_MODE_AP){
        if_index = SOFT_AP_IF;
        wifi_init_ap();
    }else if(wifi_mode == WIFI_MODE_MONITOR){
        if_index = MONITOR_IF;
    }else{
        LOG(LOG_LVL_ERROR, "wifi interface error!\r\n");
    }

    if((wifi_mode == WIFI_MODE_STATION)||(wifi_mode == WIFI_MODE_AP)) {
        //Wait for network link up
        while(LINK_UP != ethernetif_get_link_state()){
            OS_MsDelay(1000);
        }

        ethernetif_get_ip_info(if_index, &ip_info);
    }
    
    while(!dhcp_get_ip){
        OS_MsDelay(1000);
    };
    
    mqtt_iothub();
}



