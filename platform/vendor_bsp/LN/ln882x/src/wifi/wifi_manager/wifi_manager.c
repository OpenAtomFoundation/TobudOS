#include "proj_config.h"
#include "wifi_manager/wifi_manager.h"
#include "osal/osal.h"
#include "utils/debug/art_assert.h"
#include "utils/debug/log.h"
#include "netif/ethernetif.h"
#include "dhcpd/dhcpd.h"

#define WIFI_MANAGER_TASK_STACK_SIZE     5*256 //Byte
#define WIFI_MSG_QUEUE_DEPTH        10


struct wifi_manager_ctrl{
    OS_Thread_t                 wifi_manager_thread;
    OS_Queue_t                  wifi_manager_msgq;
    wifi_msg_handle_table_t     msg_handler_table[WIFI_MSG_ID_MAX];
};
static wifi_manager_ctrl_t gwifi_manager_ctrl= {0,};

static wifi_manager_ctrl_t *wifi_manager_get_handle(void)
{
    return &gwifi_manager_ctrl;
}

bool reg_wifi_msg_callbcak(wifi_msg_id_enum_t msg_id, wifi_msg_callback_fun callback_fun)
{
    wifi_manager_ctrl_t *manager_ctrl = wifi_manager_get_handle();
    wifi_msg_handle_table_t *msg_handler_table = NULL;
    
    ART_ASSERT(manager_ctrl);
    msg_handler_table = manager_ctrl->msg_handler_table;
    if((uint8_t)msg_id > WIFI_MSG_ID_MAX){
        return false;
    }else{
        msg_handler_table[(uint8_t)msg_id].fun_callback =  callback_fun;
        return true;
    }
}

void notify_wifi_manager_task(wifi_msg_t *msg)
{
    wifi_manager_ctrl_t *manager_ctrl = wifi_manager_get_handle();

    ART_ASSERT(msg && manager_ctrl);
    OS_QueueSend(&(manager_ctrl->wifi_manager_msgq), msg, OS_WAIT_FOREVER);
}

static void wifi_manager_task_entry(void *params)
{
    wifi_manager_ctrl_t *manager_ctrl = (wifi_manager_ctrl_t *)params;
    wifi_msg_t wifi_msg;
    wifi_msg_callback_fun fun_callback = NULL; 

    while(1)
    {
        if(OS_OK == OS_QueueReceive(&manager_ctrl->wifi_manager_msgq, &wifi_msg, OS_WAIT_FOREVER))
        {
            switch(wifi_msg.msg_id)
            {
                case WIFI_MSG_ID_WIFI_INVALID:
                    //turn off WIFI LED
                    break;
                
                case WIFI_MSG_ID_STA_READY:
                    //turn on WIFI LED
                    LOG(LOG_LVL_TRACE, "[%s, %d]WIFI_MSG_ID_WIFI_READY\r\n", __func__, __LINE__);
                    break;
                
                case WIFI_MSG_ID_STA_SCAN_DONE:
                    LOG(LOG_LVL_TRACE, "[%s, %d]WIFI_MSG_ID_STA_SCAN_DONE\r\n", __func__, __LINE__);
                    //update ap list
                    break;
                
                case WIFI_MSG_ID_STA_CONNECTED:
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_STA_CONNECTED\r\n", __func__, __LINE__);
                    //1.netif set up & link up.
                    ethernetif_set_state(STATION_IF, NETIF_UP);
#if BLE_SUPPORT == ENABLE
                    ble_wifi_connected_ind();
#endif
                    break;
                
                case WIFI_MSG_ID_STA_DISCONNECTED:
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_STA_DISCONNECTED\r\n", __func__, __LINE__);
                    //1.netif link down & set down.
                    ethernetif_set_state(STATION_IF, NETIF_DOWN);
                    break;
                
                case WIFI_MSG_ID_STA_AUTHMODE_CHANGE:
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_STA_AUTHMODE_CHANGE\r\n", __func__, __LINE__);
                    break;
                
                case WIFI_MSG_ID_STA_DHCP_GETING_IP:
                    //1.Log out:dhcp geting ip,update sta status.
                    //LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_STA_DHCP_GETING_IP\r\n", __func__, __LINE__);
                    break;
                
                case WIFI_MSG_ID_STA_DHCP_GOT_IP:
                    //1.Log out:dhcp got ip,update sta status.
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_STA_DHCP_GOT_IP\r\n", __func__, __LINE__);
                    break;
                
                case WIFI_MSG_ID_STA_DHCP_TIMEOUT:
                    //1.Log out:dhcp timeout,update sta status
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_STA_DHCP_TIMEOUT\r\n", __func__, __LINE__);
                    //!!!Don't break!!!
                case WIFI_MSG_ID_STA_USE_STATIC_IP:
                    //1.Log out:sta use static ip,update sta status
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_STA_USE_STATIC_IP\r\n", __func__, __LINE__);
                    break;

                case WIFI_MSG_ID_AP_READY:
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_AP_READY\r\n", __func__, __LINE__);
                    ethernetif_set_state(SOFT_AP_IF, NETIF_UP);
                    break;

                case WIFI_MSG_ID_AP_STACONNECTED:
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_AP_STACONNECTED\r\n", __func__, __LINE__);
                    //1.update sta_infor list
                    //2.flash WIFI LED
                    break;

                case WIFI_MSG_ID_AP_STADISCONNECTED:
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_AP_STADISCONNECTED\r\n", __func__, __LINE__);
                    LOG(LOG_LVL_INFO, "[%s, %d]wifi_msg.msg="MACSTR"\r\n", __func__, __LINE__, MAC2STR(wifi_msg.msg));
                    ethernetif_remove_station((uint8_t *)wifi_msg.msg);
                    //1.update sta_infor list
                    //2.flash WIFI LED
                    break;

                //暂为firmware处理，未抛上来
                case WIFI_MSG_ID_AP_PROBEREQRECVED:
                    LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_AP_PROBEREQRECVED\r\n", __func__, __LINE__);
                    break;
                
                //recv beacon
                case WIFI_MSG_ID_RECV_BEACON:
                    //LOG(LOG_LVL_INFO, "[%s, %d]WIFI_MSG_ID_RECV_BEACON\r\n", __func__, __LINE__);
                    break;

                default:
                    LOG(LOG_LVL_ERROR, "[%s, %d] msg_id invalid.\r\n", __func__, __LINE__);
                    continue;
                    //break;
            }

            fun_callback = manager_ctrl->msg_handler_table[(uint8_t)wifi_msg.msg_id].fun_callback;
            if(fun_callback != NULL && (uint8_t)wifi_msg.msg_id <= WIFI_MSG_ID_MAX){
                fun_callback(&wifi_msg);
            }
        }
        //OS_MsDelay(5);
    }
}

bool wifi_manager_init(void)
{
    OS_Status ret = OS_OK;
    wifi_manager_ctrl_t *manager_ctrl = wifi_manager_get_handle();
    
    ART_ASSERT(manager_ctrl);
    ret = OS_QueueCreate(&manager_ctrl->wifi_manager_msgq, WIFI_MSG_QUEUE_DEPTH, sizeof(wifi_msg_t));
    if (ret != OS_OK){
		LOG(LOG_LVL_ERROR, "[%s, %d]OS_QueueCreate manager_ctrl->wifi_manager_msgq fail.\r\n", __func__, __LINE__);
        return false;
    }
    ret = OS_ThreadCreate(&manager_ctrl->wifi_manager_thread, "wifi_manager_thread", wifi_manager_task_entry, manager_ctrl, OS_PRIORITY_BELOW_NORMAL, WIFI_MANAGER_TASK_STACK_SIZE);
    if (ret != OS_OK){
		LOG(LOG_LVL_ERROR, "[%s, %d]OS_ThreadCreate manager_ctrl->wifi_manager_thread fail.\r\n", __func__, __LINE__);
        return false;
    }
    return true;
}

bool wifi_manager_deinit(void)
{
    OS_Status ret = OS_OK;
    wifi_manager_ctrl_t *manager_ctrl = wifi_manager_get_handle();

    ART_ASSERT(manager_ctrl);
    ret = OS_ThreadDelete(&manager_ctrl->wifi_manager_thread);
    if (ret != OS_OK){
		LOG(LOG_LVL_ERROR, "[%s, %d]OS_ThreadDelete manager_ctrl->wifi_manager_thread fail.\r\n", __func__, __LINE__);
        return false;
    }

    ret = OS_QueueDelete(&manager_ctrl->wifi_manager_msgq);
    if (ret != OS_OK){
        LOG(LOG_LVL_ERROR, "[%s, %d]OS_QueueDelete manager_ctrl->wifi_manager_msgq fail.\r\n", __func__, __LINE__);
        return false;
    }
    return true;
}


