#include "tos_k.h"
#include "esp8266_tencent_firmware.h"
#include "tencent_firmware_module_wrapper.h"
#include "e53_sc1.h"
#include "oled.h"
#include "cJSON.h"

extern void stm32g0xx_key_init(void);

// 物联网开发平台设备信息
#define PRODUCT_ID              "3P9346G7NL"
#define DEVICE_NAME             "led_01"
#define DEVICE_KEY              "uNdwCyxa1Jd6Zch4r3/khw=="

// WIFI信息
#define YOUR_WIFI_SSID          "iot_explorer"
#define YOUR_WIFI_PWD           "iot_explorer"

// 数据模板
#define REPORT_LX_DATA_TEMPLATE               "{\\\"method\\\":\\\"report\\\"\\,\\\"clientToken\\\":\\\"00000001\\\"\\,\\\"params\\\":{\\\"lx\\\":%.0f}}"
#define REPORT_POWER_SWITCH_DATA_TEMPLATE     "{\\\"method\\\":\\\"report\\\"\\,\\\"clientToken\\\":\\\"00000001\\\"\\,\\\"params\\\":{\\\"power_switch\\\":%d}}"
#define REPORT_BRIGHTNESS_LEVEL_DATA_TEMPLATE "{\\\"method\\\":\\\"report\\\"\\,\\\"clientToken\\\":\\\"00000001\\\"\\,\\\"params\\\":{\\\"brightness_level\\\":%d}}"
#define CONTROL_REPLY_DATA_TEMPLATE       	  "{\\\"method\\\":\\\"control_reply\\\"\\,\\\"clientToken\\\":\\\"%s\\\"\\,\\\"code\\\":0\\,\\\"status\\\":\\\"ok\\\"}"

char payload[256] = {0};
static char report_topic_name[TOPIC_NAME_MAX_SIZE] = {0};
static char report_reply_topic_name[TOPIC_NAME_MAX_SIZE] = {0};

static bool is_power_switch_changed     = true;
static bool is_brightness_level_changed = true;
static bool is_client_token_received    = false;

int  power_switch_cache 	= 0;
int  brightness_level_cache = 2;
char client_token_cache[128] = {0};

/***************************************************************
* 函数名称: key1_handler_callback
* 说    明: 按键处理回调
***************************************************************/
void key1_handler_callback(void)
{
	if(power_switch_cache){
		power_switch_cache = 0;
		LED_CLOSE;
	}else{
		power_switch_cache = 1;
		LED_OPEN;
	}

	is_power_switch_changed = true;
}

/***************************************************************
* 函数名称: iot_explorer_handle_power_switch
* 说    明: 根据power switch控制开关
***************************************************************/
static void iot_explorer_handle_power_switch(int power_switch)
{
	power_switch_cache = power_switch;

	if (0 == power_switch){
		if(LED_IS_OPEN){
			printf("iot-explorer close the light\r\n");
			LED_CLOSE;
			is_power_switch_changed = true;
		}else{
			printf("the light already closed\r\n");
		}
	}else{
		if(LED_IS_OPEN){
			printf("the already opened\r\n");
		}else{
			printf("iot-explorer open the light\r\n");
			LED_OPEN;
			is_power_switch_changed = true;
		}
	}
}

/***************************************************************
* 函数名称: default_message_handler
* 说    明: IoT Explorer下行数据处理
***************************************************************/
static void default_message_handler(mqtt_message_t* msg)
{
	cJSON *root;
	cJSON *params;
	cJSON *token;
	cJSON *method;
	cJSON *power_switch;
	cJSON *brightness_level;

    printf("callback:\r\n");
    printf("---------------------------------------------------------\r\n");
    printf("\ttopic:%s\r\n", msg->topic);
    printf("\tpayload:%s\r\n", msg->payload);
    printf("---------------------------------------------------------\r\n");

	// 1. 解析从云端收到的控制信息，示例控制信息为
	//"{"method":"control","clientToken":"clientToken-Xx-N_enad","params":{"power_switch":0}}"
	root = cJSON_Parse(msg->payload +1);
	if (!root){
		printf("Invalid json root\r\n");
		return;
	}

	// 2. 解析出method
	method = cJSON_GetObjectItem(root, "method");
	if (!method){
		printf("Invalid json method\r\n");
		cJSON_Delete(root);
		return;
	}

	// 3. 仅处理云端下发的 control 数据，report_reply暂不处理
	if (0 != strncmp(method->valuestring, "control", sizeof("control") - 1)){
		cJSON_Delete(root);
		return;
	}

	// 4. 解析出params
	params = cJSON_GetObjectItem(root, "params");
	if (!params){
		printf("Invalid json params\r\n");
		cJSON_Delete(root);
		return;
	}

    // 1. 解析出 power_switch
    power_switch = cJSON_GetObjectItem(params, "power_switch");
    // 2. 根据 power_switch 控制实际硬件开关
    if (power_switch){
        iot_explorer_handle_power_switch(power_switch->valueint);
    }

    brightness_level = cJSON_GetObjectItem(params, "brightness_level");
    if (brightness_level){
        e53_sc1_set_ligth_level(brightness_level->valueint);
        brightness_level_cache = brightness_level->valueint;
        is_brightness_level_changed = true;
    }

	// 4. 设置clientToken回复
	token = cJSON_GetObjectItem(root, "clientToken");
	if (token) {
		strcpy(client_token_cache, token->valuestring);
		is_client_token_received = true;
	}

	cJSON_Delete(root);
}

/***************************************************************
* 函数名称: mqtt_demo_task
* 说    明: 主任务，连接平台以及发送上行数据
***************************************************************/
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
	// 1. 初始化ESP8266
    ret = esp8266_tencent_firmware_sal_init(HAL_UART_PORT_2);
    if (ret < 0) {
        printf("esp8266 tencent firmware sal init fail, ret is %d\r\n", ret);
    }

    // 2. 连接wifi，如果需要自行配置热点或者wifi，请打开注释
    esp8266_tencent_firmware_join_ap(YOUR_WIFI_SSID, YOUR_WIFI_PWD);

	// 3. 设置设备信息：产品ID，设备名，设备密钥
    strncpy(dev_info.product_id, product_id, PRODUCT_ID_MAX_SIZE);
    strncpy(dev_info.device_name, device_name, DEVICE_NAME_MAX_SIZE);
    strncpy(dev_info.device_serc, key, DEVICE_SERC_MAX_SIZE);
    tos_tf_module_info_set(&dev_info, TLS_MODE_PSK);

	// 4. 连接IoT Explorer
    mqtt_param_t init_params = DEFAULT_MQTT_PARAMS;
    if (tos_tf_module_mqtt_conn(init_params) != 0) {
        printf("module mqtt conn fail\n");
    } else {
        printf("module mqtt conn success\n");
    }

    if (tos_tf_module_mqtt_state_get(&state) != -1) {
        printf("MQTT: %s\n", state == MQTT_STATE_CONNECTED ? "CONNECTED" : "DISCONNECTED");
    }

	// 5. 订阅数据模板 topic
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

    while (1) {
		// 判断当前IoT Explorer连接状态
		mqtt_state_t state;
		tos_tf_module_mqtt_state_get(&state);
		if (state == MQTT_STATE_DISCONNECTED) {
			printf("mqtt is disconnected\r\n");
            tos_sleep_ms(5000);
			continue;
		}

		// 读取光照强度，数据存放在 e53_value
        e53_value = e53_scl_read_data();
        printf("e53 value %.0f lx\r\n", e53_value);

        memset(e53_str, 0, sizeof(e53_str));
        sprintf(e53_str, "%.0f lx(lm/m2)", e53_value);
        OLED_ShowString(0, 2, (uint8_t*)e53_str, 16);
	
		// 如果开关状态改变，需要上报给平台以同步
        if (is_power_switch_changed){
            is_power_switch_changed = false;
            memset(payload, 0, sizeof(payload));
            snprintf(payload, sizeof(payload), REPORT_POWER_SWITCH_DATA_TEMPLATE, power_switch_cache);

            if (tos_tf_module_mqtt_pub(report_topic_name, QOS0, payload) != 0) {
                printf("module mqtt pub fail\n");
            } else {
                printf("module mqtt pub success\n");
            }
        }
		
		// 扩展实验2：请在此处添加亮度等级状态上报物联网开发平台的代码
        if (is_brightness_level_changed){
            is_brightness_level_changed = false;
			//----请在空白处填写代码

			//----
        }
		
		// 接受到下行control消息，回复最近一条
		if (is_client_token_received) {
			is_client_token_received = false;
			memset(payload, 0, sizeof(payload));
			snprintf(payload, sizeof(payload), CONTROL_REPLY_DATA_TEMPLATE, client_token_cache);
			if (tos_tf_module_mqtt_pub(report_topic_name, QOS0, payload) != 0) {
				printf("module mqtt pub fail\n");
			} else {
				printf("module mqtt pub success\n");
			}
		}

		// 1. 构造上报的JSON
		memset(payload, 0, sizeof(payload));
        snprintf(payload, sizeof(payload), REPORT_LX_DATA_TEMPLATE, e53_value);
        // 2. 向数据模板 topic 发布光照强度信息
        if (tos_tf_module_mqtt_pub(report_topic_name, QOS0, payload) != 0) {
            printf("module mqtt pub fail\n");
            break;
        } else {
            printf("module mqtt pub success\n");
        }

		// 2. 向数据模板topic发布光照强度信息
        memset(payload, 0, sizeof(payload));
        snprintf(payload, sizeof(payload), REPORT_BRIGHTNESS_LEVEL_DATA_TEMPLATE,
        brightness_level_cache);
        if (tos_tf_module_mqtt_pub(report_topic_name, QOS0, payload) != 0) {
            printf("module mqtt pub fail\n");
        } else {
            printf("module mqtt pub success\n");
        }
		
        tos_sleep_ms(1000);
    }
}


/***************************************************************
* 函数名称: application_entry
* 说    明: 应用入口
***************************************************************/
void application_entry(void *arg)
{
    char *str = "TencentOS-tiny";

    // 初始化tencent-os tiny定时器模块
    timer_init();
	
    // 初始化按键检测模块
    stm32g0xx_key_init();
	
	// 初始化灯以及光照强度检测模块
    e53_sc1_init();

    // OLED 显示屏初始化
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0, 0, (uint8_t*)str, 16);

    mqtt_demo_task();

    while (1) {
        printf("This is a demo\r\n");
        tos_task_delay(10000);
    }
}
