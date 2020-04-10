#include "string.h"
#include "smartconfig/sc_decode.h"
#include "smartconfig/smartconfig.h"
#include "wifi/wifi.h"

#include "osal/osal.h"
#include "utils/debug/log.h"
#include "utils/debug/art_assert.h"


#define MAX_TIME 4294967295

#define SC_TIMEOUT_MS (1000*50) 
uint32_t timeout_ms = SC_TIMEOUT_MS;
 
static sc_private_t * m_sc_priv;

static uint32_t d_time(uint32_t time1, uint32_t time2)
{
	uint32_t _d_time = 0;
	if (time1 <= time2){
		_d_time = time2 - time1;
    }
	else{
		_d_time = MAX_TIME - time1 + time2;
    }
    
	return _d_time;
}

static void macframe_process_callback(uint8_t *data, wifi_promiscuous_pkt_type_t type)
{
    uint16_t len;
	/* make sure frame is qos data frame. */
	decode_status_enum_t sta = packet_deoced(m_sc_priv, data, len);

	if (sta == SC_COMPLETED) {
        LOG(LOG_LVL_DEBUG, "[%s, %d]Smartconfig complete!\r\n", __func__, __LINE__);
		m_sc_priv->sc_ctrl = SC_END;

        wifi_set_promiscuous(false);
//        wifi_set_promiscuous_rx_cb(NULL);

	} else if (sta == SC_LOCKED_CHAN) {
		m_sc_priv->sc_ctrl = SC_LOCKED_CHAN;
	}
}


static void smartconfig_close(void)
{
    LOG(LOG_LVL_DEBUG, "[%s, %d]Smartconfig close!\r\n", __func__, __LINE__);
	if (m_sc_priv) {
        
        wifi_set_promiscuous(false);
//        wifi_set_promiscuous_rx_cb(NULL);
        
		m_sc_priv->sc_ctrl = SC_END;
        
		vTaskDelay(150/ portTICK_RATE_MS);
        
		free(m_sc_priv);
		m_sc_priv = NULL;
	}
}

static void smartconfig_task_enty(void * Parameters)
{
	smartconfig_status_t status = SMART_CONFIG_FAIL;
	int16_t channel = 0;
	int16_t channel_burrer[] = {1, 6, 12, 2, 3, 4, 5, 7, 8, 9, 10, 11, 13};

    LOG(LOG_LVL_DEBUG, "[%s, %d]Smartconfig start!\r\n", __func__, __LINE__);

	if (!m_sc_priv) {
		m_sc_priv = malloc(sizeof(sc_private_t));
		if (!m_sc_priv) {
            LOG(LOG_LVL_DEBUG, "[%s, %d]Malloc faild!\r\n", __func__, __LINE__);

		}
		memset(m_sc_priv, 0, sizeof(sc_private_t));
	}

	m_sc_priv->sc_ctrl = SC_SEARCH_CHAN;
        
//	wifi_set_promiscuous_rx_cb(macframe_process_callback);
    wifi_set_promiscuous(true);
    
	uint32_t start_time = OS_GetTicks();

	while (1) 
    {
		if (m_sc_priv->sc_ctrl == SC_SEARCH_CHAN) 
        {
			//smartconfig_wifi_set_channel(priv_nif, channel_burrer[channel]);

			if (++channel >= sizeof(channel_burrer))
				channel = 0;
		} 
        else if (m_sc_priv->sc_ctrl == SC_LOCKED_CHAN) 
        {
			if (sc_read_locked_channel(m_sc_priv)){
				//smartconfig_wifi_set_channel(priv_nif, sc_read_locked_channel(m_sc_priv));
            }
            
            LOG(LOG_LVL_DEBUG, "[%s, %d]Channel locked!\r\n", __func__, __LINE__);

			while (m_sc_priv->sc_ctrl) 
            {
				vTaskDelay(10/ portTICK_RATE_MS);
			}
			break;
		}
        else if (m_sc_priv->sc_ctrl == SC_END) 
        {
			break;
		}
        
		uint32_t d_t = d_time(start_time, OS_GetTicks());
		if (d_t >= timeout_ms) 
        {
			smartconfig_close();
			status = SMART_CONFIG_TIMEOUT;
			goto out;
		}

		vTaskDelay(100/ portTICK_RATE_MS);
	}

out:
	if (m_sc_priv) 
    {
		free(m_sc_priv);
		m_sc_priv = NULL;
	}
    LOG(LOG_LVL_DEBUG, "[%s, %d]Smartconfig end!\r\n", __func__, __LINE__);
}


#define SC_TASK_STACK_SIZE       128*4
static OS_Thread_t m_smartconfig_thread;

void sc_task_start(void)
{
	if (OS_ThreadCreate(&m_smartconfig_thread, (const char*) "SmartCfg", smartconfig_task_enty, NULL, OS_PRIORITY_NORMAL, SC_TASK_STACK_SIZE) != OS_OK) {
        ART_ASSERT(1);
	}
}

void sc_task_stop(void)
{
	smartconfig_close();
    OS_ThreadDelete(&m_smartconfig_thread);
}
