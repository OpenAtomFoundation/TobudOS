#include "tos_k.h"
#include "esp8266_tencent_firmware.h"
#include "tencent_firmware_module_wrapper.h"
#include "e53_sc1.h"
#include "oled.h"
#include "cJSON.h"

extern void stm32g0xx_key_init(void);

#define PRODUCT_ID              "LRKSCAZLF1"
#define DEVICE_NAME             "techo_test01"
#define DEVICE_KEY              "UOgIWiO8kcVkgif6FO3tJQ=="

#define YOUR_WIFI_SSID			"iot-explorer"
#define YOUR_WIFI_PWD			"iot-explorer"

// 数据模板支持数据增量上报
#define REPORT_LX_DATA_TEMPLATE    "{\\\"method\\\":\\\"report\\\"\\,\\\"clientToken\\\":\\\"00000001\\\"\\,\\\"params\\\":{\\\"lx\\\":%.0f}}"
#define REPORT_POWER_SWITCH_DATA_TEMPLATE    "{\\\"method\\\":\\\"report\\\"\\,\\\"clientToken\\\":\\\"00000001\\\"\\,\\\"params\\\":{\\\"power_switch\\\":%d}}"

#define MAIL_SIZE	256
#define MAIL_MAX    10
static uint8_t mail_pool[MAIL_MAX * MAIL_SIZE];
static k_mail_q_t mail_q;

static bool key_pressed_flag = false;
void key1_handler_callback(void)
{
	key1_handle_power_switch();
	// 在主循环中进行数据上报
	key_pressed_flag = true;
}

static void default_message_handler(mqtt_message_t* msg)
{
	int payload_len = 0;

    printf("callback:\r\n");
    printf("---------------------------------------------------------\r\n");
    printf("\ttopic:%s\r\n", msg->topic);
    printf("\tpayload:%s\r\n", msg->payload);
    printf("---------------------------------------------------------\r\n");

	payload_len = strlen(msg->payload);
	if (payload_len > MAIL_SIZE){
		printf("too long payload\r\n");
		return;
	}
	
	tos_mail_q_post(&mail_q, msg->payload, MAIL_SIZE);
}

static char report_reply_topic_name[TOPIC_NAME_MAX_SIZE] = {0};
static char report_topic_name[TOPIC_NAME_MAX_SIZE] = {0};
static char payload[256] = {0};
static void mqtt_demo_task(void)
{
    int ret = 0;
    int size = 0;
    mqtt_state_t state;
    
    char *product_id = PRODUCT_ID;
    char *device_name = DEVICE_NAME;
    char *key = DEVICE_KEY;
    
    device_info_t dev_info;
    memset(&dev_info, 0, sizeof(device_info_t));

	float e53_value = 0;
    char e53_str[16] = {0};
    
    OLED_ShowString(0, 2, (uint8_t*)"connecting...", 16);

    /**
     * Please Choose your AT Port first, default is HAL_UART_2(USART2)
    */
    ret = esp8266_tencent_firmware_sal_init(HAL_UART_PORT_2);
    if (ret < 0) {
        printf("esp8266 tencent firmware sal init fail, ret is %d\r\n", ret);
    }
    
    esp8266_tencent_firmware_join_ap(YOUR_WIFI_SSID, YOUR_WIFI_PWD);

    strncpy(dev_info.product_id, product_id, PRODUCT_ID_MAX_SIZE);
    strncpy(dev_info.device_name, device_name, DEVICE_NAME_MAX_SIZE);
    strncpy(dev_info.device_serc, key, DEVICE_SERC_MAX_SIZE);
    tos_tf_module_info_set(&dev_info, TLS_MODE_PSK);

    mqtt_param_t init_params = DEFAULT_MQTT_PARAMS;
    if (tos_tf_module_mqtt_conn(init_params) != 0) {
        printf("module mqtt conn fail\n");
    } else {
        printf("module mqtt conn success\n");
    }

    if (tos_tf_module_mqtt_state_get(&state) != -1) {
        printf("MQTT: %s\n", state == MQTT_STATE_CONNECTED ? "CONNECTED" : "DISCONNECTED");
    }
    
	// 订阅数据模板
    size = snprintf(report_reply_topic_name, TOPIC_NAME_MAX_SIZE, "$thing/down/property/%s/%s", product_id, device_name);
    if (size < 0 || size > sizeof(report_reply_topic_name) - 1) {
        printf("sub topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(report_reply_topic_name));
    }
    if (tos_tf_module_mqtt_sub(report_reply_topic_name, QOS0, default_message_handler) != 0) {
        printf("module mqtt sub fail\n");
    } else {
        printf("module mqtt sub success\n");
    }
    
    memset(report_topic_name, sizeof(report_topic_name), 0);
    size = snprintf(report_topic_name, TOPIC_NAME_MAX_SIZE, "$thing/up/property/%s/%s", product_id, device_name);
    if (size < 0 || size > sizeof(report_topic_name) - 1) {
        printf("pub topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(report_topic_name));
    }
    
    if (e53_sc1_init() == -1) {
        printf("e53 init fail\r\n");
        return;
    }
  
    while (1) {
		e53_value = e53_scl_read_data();
		printf("e53 value %.0f lx\r\n", e53_value);
		
        memset(e53_str, 0, sizeof(e53_str));
        sprintf(e53_str, "%.0f lx(lm/m2)", e53_value);
        OLED_ShowString(0, 2, (uint8_t*)e53_str, 16);
		
		if (key_pressed_flag){
			key_pressed_flag = false;
			memset(payload, 0, sizeof(payload));
			snprintf(payload, sizeof(payload), REPORT_POWER_SWITCH_DATA_TEMPLATE, LED_IS_OPEN);
			
			if (tos_tf_module_mqtt_pub(report_topic_name, QOS0, payload) != 0) {
				printf("module mqtt pub fail\n");
			} else {
				printf("module mqtt pub success\n");
			}
		}
		
		// 请在此处添加光照强度数据上报物联网开发平台的代码
		
		tos_sleep_ms(5000);
    }
}

static void iot_explorer_data_handle(void *arg)
{
	char buf[MAIL_SIZE];
	size_t mail_size = 0;
	cJSON *root;
	cJSON *params;
	//cJSON *token;
	cJSON *method;
	cJSON *p;
	
	while(1){
		memset(buf, 0, MAIL_SIZE);
		if (K_ERR_NONE == tos_mail_q_pend(&mail_q, buf, &mail_size, TOS_TIME_FOREVER)){
			//printf("mail content: %s\r\n", buf);
			//字符串首尾的引号丢弃后才是标准的json数据格式
			buf[strlen(buf) - 1] = '\0';
			root = cJSON_Parse(buf + 1);
			if (NULL == root){
				printf("Invalid json root\r\n");
				continue;
			}
			method = cJSON_GetObjectItem(root, "method");
			if (NULL == method){
				printf("Invalid json method\r\n");
				cJSON_Delete(root);
				continue;
			}
			// 仅处理云端下发的 control 数据
			if (0 != strncmp(method->valuestring, "control", sizeof("control") - 1)){
				//printf("Invalid method\r\n");
				cJSON_Delete(root);
				continue;
			}
			// 若需要回复报文，需要携带数据报文的 clitenToken 字段
			//token = cJSON_GetObjectItem(root, "clientToken");
			params = cJSON_GetObjectItem(root, "params");
			if (NULL == params){
				printf("Invalid json params\r\n");
				cJSON_Delete(root);
				continue;
			}
			
			// 请在此处添加物联网开发平台对智能灯的控制代码
			
			
			cJSON_Delete(root);
		}
	}
}

#define TASK_HANDLE_MAIL_PRIORITY	3
#define TASK_HANDLE_MAIL_STACK_SIZE	4096
__STATIC__ k_stack_t stack_task_handle_mail[TASK_HANDLE_MAIL_STACK_SIZE];
k_task_t task_handle_mail;	

void application_entry(void *arg)
{
    char *str = "TencentOS-tiny";
    
	// 初始化tencent-os tiny定时器模块
	timer_init();
	// 初始化按键检测模块
	stm32g0xx_key_init();
	
	// OLED 显示屏初始化
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0, 0, (uint8_t*)str, 16);
    
	// 创建tencent-os tiny邮箱，使用邮箱机制进行任务间通信
	tos_mail_q_create(&mail_q, mail_pool, MAIL_MAX, MAIL_SIZE);
	// 创建tencent-os tiny任务，任务遍历邮箱，处理iot-explorer下发的数据
    (void)tos_task_create(&task_handle_mail, "iot-explorer", iot_explorer_data_handle,
                          (void *)NULL, TASK_HANDLE_MAIL_PRIORITY,
                          stack_task_handle_mail, TASK_HANDLE_MAIL_STACK_SIZE, 0);
						  
    mqtt_demo_task();
    while (1) {
		printf("This is a demo\r\n");
		tos_task_delay(10000);
    }
}
