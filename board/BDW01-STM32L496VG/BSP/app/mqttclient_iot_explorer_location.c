#include "stm32l4xx_hal.h"
#include "mcu_init.h"
#include "tos_k.h"
#include "mqttclient.h"
#include "cjson.h"
#include "sal_module_wrapper.h"



//#define USE_ESP8266
//#define USE_NB_BC35
//#define USE_BC26
//#define USE_EC200S
#define USE_M5313


#if defined(USE_ESP8266)
#include "esp8266.h"
#elif defined(USE_BC26)
#include "bc26.h"
#elif defined(USE_EC200S)
#include "ec200s.h"
#elif defined(USE_M5313)
#include "m5313.h"

#endif

uint8_t module_sel=0;	//0---M5313; 1---wifi;  2---BT;  3---Lora


#ifdef USE_ESP8266 
static hal_uart_port_t esp8266_port = HAL_UART_PORT_0;

void mqtt_set_esp8266_port(hal_uart_port_t port) {
    esp8266_port = port;
}
#endif

k_event_t report_result_event;
k_event_flag_t report_success = 1<<0;
k_event_flag_t report_fail    = 1<<1;

/* 接收数据最大缓存条数 */
#define MESSAGE_MAX     5

typedef struct payload_mail_st {
    void* client;
    void* payload;
} payload_mail_t;
k_mail_q_t mail_q;
uint8_t mail_pool[MESSAGE_MAX * sizeof(payload_mail_t *)];

#define REPORT_DATA_TEMPLATE "{\"method\":\"report\",\"clientToken\":\"00000001\",\"params\":{\"location\":\"%s\"}}"
#define CONTROL_REPLY_DATA   "{\"method\":\"control_reply\",\"clientToken\":\"%s\",\"code\":0,\"status\":\"success\"}"

char report_buf[1024];
char reply_buf[200];


static void tos_topic_handler(void* client, message_data_t* msg)
{
    (void) client;
    cJSON* cjson_root   = NULL;
    cJSON* cjson_status = NULL;
	cJSON* cjson_method = NULL;
	cJSON* cjson_params = NULL;
    cJSON* cjson_relay_status = NULL;
	cJSON* cjson_client_token = NULL;
	int    relay_status = 0;
    char* status = NULL;
	char*  method = NULL;
	char*  client_token = NULL;
    k_event_flag_t event_flag = report_fail;

	int error;
	mqtt_message_t reply_msg;
    payload_mail_t payload_mail;

    /* 打印日志 */
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------\n");
    
    /* 使用cjson解析上报响应数据 */
    cjson_root = cJSON_Parse((char*)msg->message->payload);
    if (cjson_root == NULL) {
        printf("report reply message parser fail\r\n");
        event_flag = report_fail;
        goto exit;
    }

	/* 提取消息类型 */
  	cjson_method = cJSON_GetObjectItem(cjson_root, "method");
    method = cJSON_GetStringValue(cjson_method);
    
	/* 判断是哪种类型的消息 */
	if (strstr(method, "report_reply")) {
		
	    /* 提取status状态 */
	    cjson_status = cJSON_GetObjectItem(cjson_root, "status");
	    status = cJSON_GetStringValue(cjson_status);
	    if (cjson_status == NULL || status == NULL) {
	        printf("report reply status parser fail\r\n");
	        event_flag = report_fail;
	        goto exit;
	    }
	    
	    /* 判断status状态 */
	    if (strstr(status,"success")) {
	        event_flag = report_success;
	    }else {
	        event_flag = report_fail;
	    }
	}else if (strstr(method, "control")) {
        
            //收到平台下发的控制报文，提取client_token，用于上报响应
       cjson_client_token = cJSON_GetObjectItem(cjson_root, "clientToken");
       client_token = cJSON_GetStringValue(cjson_client_token);
            
       printf("parse client token:%s\r\n", client_token);
            
       //提取 relay_status
       cjson_params = cJSON_GetObjectItem(cjson_root, "params");
       cjson_relay_status = cJSON_GetObjectItem(cjson_params, "led");
       relay_status = cjson_relay_status->valueint;
            
       //根据 relay_status 执行相应的动作
       if (relay_status == 0) {
           HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
       } else if (relay_status == 1) {
		   HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
       }
            
       memset(reply_buf, 0, sizeof(reply_buf));
       sprintf(reply_buf, CONTROL_REPLY_DATA, client_token);
       memset(&reply_msg, 0, sizeof(reply_msg));
       reply_msg.qos = QOS0;
       reply_msg.payload = (void *) reply_buf;
            
       printf("control reply:\r\n\t%s\r\n", reply_buf);
      
       error = mqtt_publish(payload_mail.client, "$thing/up/property/E2IGF491FP/dev001", &reply_msg);
            
       MQTT_LOG_D("control reply publish error is %#0x", error);
	}        
    
exit:
    cJSON_Delete(cjson_root);
    cjson_root = NULL;
    status = NULL;
    
    tos_event_post(&report_result_event, event_flag);
    
    return;
}

void mqttclient_task(void)
{
	#define BUFFER_SIZE        128
    int error;
    
    int lightness = 0;
    
    mqtt_client_t *client = NULL;
    
    mqtt_message_t msg;
    
    k_event_flag_t match_flag;
    
    char  host_ip[20];

	int len;
	uint8_t *buffer = K_NULL;
    
    memset(&msg, 0, sizeof(msg));

	gps_init(HAL_UART_PORT_3);
	
	buffer = tos_mmheap_alloc(BUFFER_SIZE);
    
#ifdef USE_ESP8266 
    esp8266_sal_init(esp8266_port);
    esp8266_join_ap("Supowang", "13975428888");
#endif

#ifdef USE_NB_BC35
    int bc35_28_95_sal_init(hal_uart_port_t uart_port);
    bc35_28_95_sal_init(HAL_UART_PORT_0);
#endif

#ifdef USE_BC26
    bc26_sal_init(HAL_UART_PORT_2);
#endif

#ifdef USE_EC200S
    ec200s_sal_init(HAL_UART_PORT_2);
#endif

#ifdef USE_M5313
	m5313_sal_init(HAL_UART_PORT_2);
#endif

    mqtt_log_init();

    client = mqtt_lease();
    
    tos_event_create(&report_result_event, (k_event_flag_t)0u);
    
    /* Domain Format: <your product ID>.iotcloud.tencentdevices.com */
    tos_sal_module_parse_domain("OKY0D78DP2.iotcloud.tencentdevices.com",host_ip,sizeof(host_ip));
    
    /*
        These infomation is generated by mqtt_config_gen.py tool in "TencentOS-tiny\tools" directory.
    */
    mqtt_set_port(client, "1883");
    mqtt_set_host(client, host_ip);
    mqtt_set_client_id(client, "OKY0D78DP2bd_location");
    mqtt_set_user_name(client, "OKY0D78DP2bd_location;21010406;12365;4294967295");
    mqtt_set_password(client, "55ed32e6ce98deca02ea579954d05a8ba4c442e2;hmacsha1");
    mqtt_set_clean_session(client, 1);

    error = mqtt_connect(client);
    
    MQTT_LOG_D("mqtt connect error is %#0x", error);
    
    error = mqtt_subscribe(client, "$thing/down/property/OKY0D78DP2/bd_location", QOS0, tos_topic_handler);
    
    MQTT_LOG_D("mqtt subscribe error is %#0x", error);
    
    while (1) {

		if((len =gps_read(1,buffer,BUFFER_SIZE))>0)
		{
			printf("gps:%s(%d)\n",buffer,len);
	        memset(&msg, 0, sizeof(msg));
	        
	        snprintf(report_buf, sizeof(report_buf), REPORT_DATA_TEMPLATE, buffer);
	       
	        msg.qos = QOS0;
	        msg.payload = (void *) report_buf;
	        
	        error = mqtt_publish(client, "$thing/up/property/OKY0D78DP2/bd_location", &msg);
	        
	        MQTT_LOG_D("mqtt publish error is %#0x", error);
	        
	        tos_event_pend(&report_result_event, 
	                       report_success|report_fail,
	                       &match_flag,
	                       TOS_TIME_FOREVER,
	                       TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
	        
	        if (match_flag == report_success) {
	            printf("report to Tencent IoT Explorer success\r\n");
	        
	        }else if (match_flag == report_fail){
	            printf("report to Tencent IoT Explorer fail\r\n");
	        }

		}
        
        tos_task_delay(5000); 
    }
}

void application_entry(void *arg)
{
    mqttclient_task();
    while (1) {
        printf("This is a mqtt demo!\r\n");
        tos_task_delay(1000);
    }
}
