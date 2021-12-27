/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "app_system.h"
#include "app_lorawan.h"
#include "stm32_lpm.h"
#include "adc_if.h"
#include "lorawan_version.h"

#include "pm2d5_parser.h"
#include "tos_k.h"
#include "esp8266_tencent_firmware.h"
#include "tencent_firmware_module_wrapper.h"
#include "math.h"

//#define USE_WIFI

#define APP_TX_DUTYCYCLE                            30000      //ms
#define LORAWAN_DOWNLINK_PORT						2
#define LORAWAN_UPLINK_PORT                         10 

#define WIFI_SSID               "TencentOS"
#define WIFI_PASSWD             "tencentostiny"
#define PRODUCT_ID              "ZLC54JVTAH"
#define DEVICE_NAME             "node_20DEBC0A0000"
#define DEVICE_KEY              "QXiswx4I1T+bZyHOrHvaqg=="


#define REPORT_DATA_TEMPLATE    "{\"method\":\"report\",\"clientToken\":\"%s\"," \
    "\"params\":{\"a\":%d,"	\
    "\"b\":%d," \
    "\"c\":%d," \
    "\"d\":%d," \
    "\"e\":%d," \
    "\"f\":%d," \
    "\"g\":%d," \
    "\"h\":%d,"	\
    "\"i\":%d," \
    "\"j\":%d," \
    "\"k\":%d," \
    "\"l\":%d," \
    "\"v\":%d," \
    "\"ec\":%d" \
    "}}"

void default_message_handler(mqtt_message_t* msg)
{
    printf("callback:\r\n");
    printf("---------------------------------------------------------\r\n");
    printf("\ttopic:%s\r\n", msg->topic);
    printf("\tpayload:%s\r\n", msg->payload);
    printf("---------------------------------------------------------\r\n");
}

char payload[1024] = {0};
static char report_topic_name[TOPIC_NAME_MAX_SIZE] = {0};
static char report_reply_topic_name[TOPIC_NAME_MAX_SIZE] = {0};

k_mail_q_t mail_q;
pm2d5_data_u pm2d5_value;
uint8_t pm2d5_value_pool[3 * sizeof(pm2d5_data_u)];

void generate_client_token(char* buffer, size_t size)
{
    long client_token_value;
    
    memset(buffer, 0, size);
    client_token_value = ((long)tos_systick_get()) % (long)(pow(10, size));
    snprintf(buffer, size, "%ld", client_token_value);
}

void mqtt_demo_task(void)
{
    int ret = 0;
    int size = 0;
    int  i = 0;    
    char *product_id = PRODUCT_ID;
    char *device_name = DEVICE_NAME;
    char *key = DEVICE_KEY;
    
    device_info_t dev_info;
    memset(&dev_info, 0, sizeof(device_info_t));
    char str[16];   
    size_t mail_size;
    uint8_t report_error_count = 0;
    char client_token[10];

    /**
     * Please Choose your AT Port first, default is HAL_UART_2(USART2)
    */
    ret = esp8266_tencent_firmware_sal_init(HAL_UART_PORT_0);
    
    if (ret < 0) {
        printf("esp8266 tencent firmware sal init fail, ret is %d\r\n", ret);
        return;
    }
    
    while (1) {
        if ( esp8266_tencent_firmware_join_ap(WIFI_SSID, WIFI_PASSWD) == 0) {
            printf("module WIFI connect success\n");
            break;
        }
        printf("module WIFI connect fail\n");
        tos_sleep_ms(2000);
    }
    
    strncpy(dev_info.product_id, product_id, PRODUCT_ID_MAX_SIZE);
    strncpy(dev_info.device_name, device_name, DEVICE_NAME_MAX_SIZE);
    strncpy(dev_info.device_serc, key, DEVICE_SERC_MAX_SIZE);
    tos_tf_module_info_set(&dev_info, TLS_MODE_PSK);
    mqtt_param_t init_params = DEFAULT_MQTT_PARAMS;
    while (1) {
        if (tos_tf_module_mqtt_conn(init_params) == 0) {
            printf("module mqtt connect success\n");
            break;
        }
        printf("module mqtt connect fail\n");
        tos_sleep_ms(5000);
    }
    
    /* 开始订阅topic */
    size = snprintf(report_reply_topic_name, TOPIC_NAME_MAX_SIZE, "$thing/down/property/%s/%s", product_id, device_name);

    if (size < 0 || size > sizeof(report_reply_topic_name) - 1) {
        printf("sub topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(report_reply_topic_name));
    }
    if (tos_tf_module_mqtt_sub(report_reply_topic_name, QOS0, default_message_handler) != 0) {
        printf("module mqtt sub fail\n");
    } else {
        printf("module mqtt sub success\n");
    }
    
    memset(report_topic_name, 0, sizeof(report_topic_name));
    size = snprintf(report_topic_name, TOPIC_NAME_MAX_SIZE, "$thing/up/property/%s/%s", product_id, device_name);

    if (size < 0 || size > sizeof(report_topic_name) - 1) {
        printf("pub topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(report_topic_name));
    }
    
    /* 创建邮箱 */
    tos_mail_q_create(&mail_q, pm2d5_value_pool, 3, sizeof(pm2d5_data_u));
    
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    
    if (pm2d5_parser_init() == -1) {
        printf("pm2d5 parser init fail\r\n");
        return;
    }
  
    while (1) {
        /* 通过接收邮件来读取数据 */
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        tos_mail_q_pend(&mail_q, (uint8_t*)&pm2d5_value, &mail_size, TOS_TIME_FOREVER);
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        
        //收到之后打印信息
        printf("\r\n\r\n\r\n");
        for (i = 0; i < 13; i++) {
            printf("data[%d]:%d ug/m3\r\n", i+1, pm2d5_value.data[i]);
        }
        
        
        /* 上报值 */
        generate_client_token(client_token, sizeof(client_token));
        memset(payload, 0, 1024);
        snprintf(payload, 1024, REPORT_DATA_TEMPLATE, client_token,
                pm2d5_value.pm2d5_data.data1, pm2d5_value.pm2d5_data.data2,
                pm2d5_value.pm2d5_data.data3, pm2d5_value.pm2d5_data.data4,
                pm2d5_value.pm2d5_data.data5, pm2d5_value.pm2d5_data.data6,                
                pm2d5_value.pm2d5_data.data7, pm2d5_value.pm2d5_data.data8,
                pm2d5_value.pm2d5_data.data9, pm2d5_value.pm2d5_data.data10,
                pm2d5_value.pm2d5_data.data11, pm2d5_value.pm2d5_data.data12,
                2, pm2d5_value.pm2d5_data.err_code);
        
        if (tos_tf_module_mqtt_publ(report_topic_name, QOS0, payload) != 0) {
            report_error_count++;
            printf("module mqtt publ fail, count: %d\n", report_error_count);
            sprintf(str, "# report fail");
        } else {
            report_error_count = 0;
            printf("module mqtt publ success\n");
            sprintf(str, "# report ok");
        }

        if (report_error_count >= 6) {
            HAL_NVIC_SystemReset();
        }
             
        tos_sleep_ms(5000);
        
    }
}

/*!
 *  user main_callback function declare , you need to implement them
 * 	and register.
 */
static uint8_t LORA_GetBatteryLevel(void);
static uint16_t LORA_GetTemperatureLevel(void);
static void GetUniqueId(uint8_t *id);
static uint32_t GetRandomSeed(void);

static void LoraMacProcessNotify(void);


void mac_event_callback(APP_EVENT_t evt,const void* param);

static LoRaMainCallback_t LoRaMainCallbacks = { mac_event_callback,
												LoraMacProcessNotify,
												LORA_GetBatteryLevel,
                                                LORA_GetTemperatureLevel,
                                                GetUniqueId,
                                                GetRandomSeed,
                                              };

 

#define LORAWAN_APP_DATA_BUFF_SIZE                           64
static uint8_t app_data_buff[LORAWAN_APP_DATA_BUFF_SIZE];
lw_app_data_t app_data = { app_data_buff,  0, 0 };


/*send task function  declaration*/
static TimerEvent_t send_timer;
static void send_process(void *args);
static void cycle_send_event(void *context);
static void Lora_start_send(void);

k_task_t	lora_mac_process_task;
//4096 is miniest stack size to work normally
k_stack_t	lora_mac_process_task_stack[4096];

k_sem_t	lora_mac_process_sem;

//MAC层任务处理
void LoraMacProcess_task_entry(void *args)
{
	while(1)
	{
		tos_sem_pend(&lora_mac_process_sem, TOS_TIME_FOREVER);
		LoRaMacProcess(NULL);
	}

}
/*Exported functions ---------------------------------------------------------*/
void MX_LoRaWAN_Init(void)
{
	printf("Lora node powered by TencentOS-tiny\r\n");  

	/* Configure the Lora Stack*/
	lw_init(&LoRaMainCallbacks);

	APP_LOG("APP_VERSION= %02X.%02X.%02X.%02X\r\n", (uint8_t)(__APP_VERSION >> 24), (uint8_t)(__APP_VERSION >> 16), (uint8_t)(__APP_VERSION >> 8), (uint8_t)__APP_VERSION);
	APP_LOG("MAC_VERSION= %02X.%02X.%02X.%02X\r\n", (uint8_t)(__LORA_MAC_VERSION >> 24), (uint8_t)(__LORA_MAC_VERSION >> 16), (uint8_t)(__LORA_MAC_VERSION >> 8), (uint8_t)__LORA_MAC_VERSION);

	/*Class*/
	lw_request_class(CLASS_A);
	
	/*channel 80-87*/
	LWChannel_mask_t ch={0x0000,0,0,0,0,0x00FF};	
	lw_current_chmask_set(&ch);
	
	/*tx dr*/
	lw_config_tx_datarate_set(DR_3);
	
	/*OTAA*/
 	lw_config_otaa_set(LORA_ENABLE);

	/*enable adr*/
	lw_adr_set(LORA_ENABLE);
	
	/*set retry*/
	lw_confirm_retry_set(3);

	printf("lorawan init ok.\r\n");
}


uint16_t report_period = 10;

void uart_output(const char *str)
{
    /* if using c lib printf through uart, a simpler one is: */
    printf(str);
}

void recv_callback(uint8_t *data, uint8_t len)
{
    int i = 0;

    printf("len: %d\n", len);

    for (i = 0; i < len; ++i) {
        printf("data[%d]: %d\n", i, data[i]);
    }

    if (len == 1) {
        report_period = data[0];
    } else if (len >= 2) {
        report_period = data[0] | (data[1] << 8);
    }
    printf("report_period: %d\n", report_period);
}

void application_entry(void *arg)
{
#ifdef USE_WIFI
    mqtt_demo_task();
    while (1) {
        printf("This is a mqtt demo!\r\n");
        tos_task_delay(1000);
    }    
#else
    int i = 0;
    int ret = 0;
    int send_failed_count = 0;
    size_t mail_size;
    tos_mail_q_create(&mail_q, pm2d5_value_pool, 3, sizeof(pm2d5_data_u));
    
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    
    if (pm2d5_parser_init() == -1) {
        printf("pm2d5 parser init fail\r\n");
        return;
    }    

    //create task to process loramac
    tos_sem_create_max(&lora_mac_process_sem, 0, 1);
    tos_task_create(&lora_mac_process_task, 
                  "lora_mac_process_task", 
                    LoraMacProcess_task_entry, 
                    NULL, 
                    2, 
                    lora_mac_process_task_stack, 
                    sizeof(lora_mac_process_task_stack), 
                    10);

    //report pm2.5 data
    while (1) {
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        tos_mail_q_pend(&mail_q, (uint8_t*)&pm2d5_value, &mail_size, TOS_TIME_FOREVER);
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        //收到之后打印信息
        printf("\r\n\r\n\r\n");
        for (i = 0; i < 13; i++) {
            printf("data[%d]:%d ug/m3\r\n", i+1, pm2d5_value.data[i]);
        }
        
        /*fill the data*/
        i = 0;
        app_data.port = LORAWAN_UPLINK_PORT;

        for (i = 0; i < mail_size; i++) {
            app_data.buff[i] = pm2d5_value.data[i];
        }
        printf("\n\n");
        app_data.size = i;
        
        /*Check if it is in OTAA mode*/
        if(LORA_ENABLE == lw_config_otaa_get())
        {
            /*check if it is already joined*/
            if(lw_join_status() != LORA_SET)
            {
                lw_join();
                continue;
            }
        }

        ret = lw_send(&app_data, LORAWAN_CONFIRMED_MSG);
        if (ret != LORAMAC_STATUS_OK )
        {
            printf("LoRa Send data faild!, retcode = %d, count is %d\r\n",ret, send_failed_count);
            send_failed_count++;
            if (send_failed_count > 10)
            {
                NVIC_SystemReset();// when lora send faied more than 10 times , cpu reset to reconnect.
            }
        }
        else
        {
            send_failed_count = 0;
        }
        
        //tos_task_delay(10000);
    }
					
    //Lora_start_send();
#endif
}

void Lora_start_send(void)
{
	/* send everytime timer elapses */
	TimerInit(&send_timer, cycle_send_event);
	TimerSetValue(&send_timer,  APP_TX_DUTYCYCLE);
	cycle_send_event(NULL);
}

static void cycle_send_event(void *args)
{
	send_process(NULL);
	/*Wait for next tx slot*/
	TimerStart(&send_timer);
	//tos_timer_start(&send_timer);
}

static void send_process(void *args)
{
	int16_t temperature = 0;
	uint8_t batteryLevel=0;

	/*Check if it is in OTAA mode*/
	if(LORA_ENABLE == lw_config_otaa_get())
	{
		/*check if it is already joined*/
		if(lw_join_status() != LORA_SET)
		{
			lw_join();
			return;
		}
	}
		
	APP_LOG("SEND REQUEST=======================================>\n\r");
	/*fill the data*/
	uint32_t i = 0;

	app_data.port = LORAWAN_UPLINK_PORT;

	batteryLevel = LORA_GetBatteryLevel();        /* 1 (very low) to 254 (fully charged) */
	temperature  = SYS_GetTemperatureLevel();                    /* report Temperature */

	app_data.buff[i++] = batteryLevel * 100 / 254;
	app_data.buff[i++] = temperature>>8;
	app_data.buff[i++] = temperature&0xff;
  
	app_data.size = i;
	
	/*you can set the channel by using these function*/
//	LWChannel_mask_t ch_r={0};
//	LWChannel_mask_t ch_w={0x00FF,0,0,0,0,0};

//	lw_current_chmask_set(&ch_w);
//	APP_LOG("ch_write: %04X %04X %04X %04X %04X %04X\r\n",ch_w.mask[0],ch_w.mask[1],ch_w.mask[2],ch_w.mask[3],ch_w.mask[4],ch_w.mask[5]);
//	lw_current_chmask_get(&ch_r);
//	APP_LOG("ch_read: %04X %04X %04X %04X %04X %04X\r\n",ch_r.mask[0],ch_r.mask[1],ch_r.mask[2],ch_r.mask[3],ch_r.mask[4],ch_r.mask[5]);
	

	lw_send(&app_data, LORAWAN_CONFIRMED_MSG);

	
}


void mac_event_callback(APP_EVENT_t evt,const void* param)
{
	switch(evt)
	{
		case APP_RX_EVENT:{
				lw_app_data_t* rx_data = (lw_app_data_t*)param;
				
				switch(rx_data->port)
				{
					case 2:
						APP_LOG("%s\r\n",rx_data->buff);
						APP_LOG("size = %d\r\n",rx_data->size);
						APP_LOG("<===============================>");break;
					case 3:
						 lw_request_class(CLASS_A);break;
					case 4:
						 lw_request_class(CLASS_B);break;	
					case 5:
						 lw_request_class(CLASS_C);break;						
				}
				
				if(rx_data->fpending)
				{
					app_data.size = 0;
					app_data.port = LORAWAN_UPLINK_PORT;
					lw_send(&app_data, LORAWAN_UNCONFIRMED_MSG);
				}
				
			}break;
	
		case APP_JOINED_EVENT:{
			  APP_LOG("end node joined \n\r");
			  //lw_request_class(CLASS_A);
		}break;
	
		case APP_CONFIRM_ClASS_EVENT:{
			DeviceClass_t* cur_class = (DeviceClass_t*)param;
			APP_LOG("==================Switch to Class%c done\n\r","ABC"[*cur_class]);
			/*Optionnal*/
			/*informs the server that switch has occurred ASAP*/
			app_data.size = 0;
			app_data.port = LORAWAN_UPLINK_PORT;
			lw_send(&app_data, LORAWAN_UNCONFIRMED_MSG);
		}break;
		
		case APP_TX_DONE_EVENT:{
			lw_app_data_t* tx_done = (lw_app_data_t*)param;
			
			if(tx_done->ack){
				APP_LOG("Network Server \"ack\" an uplink data confirmed message transmission\n\r");
			}
			else{
				APP_LOG("tx =======> down\n\r");
			}
		}break;
	
		default :break;
	}
}
static void LoraMacProcessNotify(void)
{
	tos_sem_post(&lora_mac_process_sem);
}

/**
  * @brief This function return the battery level
  * @param none
  * @retval the battery level  1 (very low) to 254 (fully charged)
  */
static uint8_t LORA_GetBatteryLevel(void)
{
	/* USER CODE BEGIN 1 */
	uint16_t batteryLevelmV;
	uint8_t batteryLevel = 0;

	batteryLevelmV = (uint16_t) SYS_GetBatteryLevel();

	/* Convert batterey level from mV to linea scale: 1 (very low) to 254 (fully charged) */
	if(batteryLevelmV > VDD_BAT)
	{
		batteryLevel = 254;
	}
	else if(batteryLevelmV < VDD_MIN)
	{
		batteryLevel = 0;
	}
	else
	{
		batteryLevel = (((uint32_t)(batteryLevelmV - VDD_MIN) * 254) / (VDD_BAT - VDD_MIN));
	}
	APP_LOG("VDDA= %d\n\r", batteryLevel);

	return batteryLevel;  /* 1 (very low) to 254 (fully charged) */
	/* USER CODE END 1 */
}

static uint16_t LORA_GetTemperatureLevel(void)
{
	return (uint16_t) SYS_GetTemperatureLevel();
}


/**
  * @brief This function return a random seed
  * @note based on the device unique ID
  * @param None
  * @retval see
  */
static uint32_t GetRandomSeed(void)
{
	return ((HAL_GetUIDw0()) ^ (HAL_GetUIDw1()) ^ (HAL_GetUIDw2()));
}

/**
  * @brief This function return a unique ID
  * @param unique ID
  * @retval none
  */
static void GetUniqueId(uint8_t *id)
{
	uint32_t ID_1_3_val = HAL_GetUIDw0() + HAL_GetUIDw2();
	uint32_t ID_2_val = HAL_GetUIDw1();

	id[7] = (ID_1_3_val) >> 24;
	id[6] = (ID_1_3_val) >> 16;
	id[5] = (ID_1_3_val) >> 8;
	id[4] = (ID_1_3_val);
	id[3] = (ID_2_val) >> 24;
	id[2] = (ID_2_val) >> 16;
	id[1] = (ID_2_val) >> 8;
	id[0] = (ID_2_val);
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
