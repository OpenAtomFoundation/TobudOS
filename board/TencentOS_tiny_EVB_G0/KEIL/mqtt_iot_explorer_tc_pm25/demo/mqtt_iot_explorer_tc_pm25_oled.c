#include "tos_k.h"
#include "esp8266_tencent_firmware.h"
#include "tencent_firmware_module_wrapper.h"
#include "pm2d5_parser.h"
#include "oled.h"
#include "math.h"

#define WIFI_SSID               "CMCC-Quq4"
#define WIFI_PASSWD             "c66trb8d"
#define PRODUCT_ID              "7WLPW5Z48Q"
#define DEVICE_NAME             "dev001"
#define DEVICE_KEY              "5SEZMc2+Ts1FMCwoU6hV6Q=="

#define REPORT_DATA_TEMPLATE1    "{\"method\":\"report\",\"clientToken\":\"%s\"," \
    "\"params\":{\"PM1_CF1\":%d,"	\
    "\"PM2d5_CF1\":%d," \
    "\"PM10_CF1\":%d," \
    "\"PM1\":%d," \
    "\"PM2d5\":%d," \
    "\"PM10\":%d" \
    "}}"

#define REPORT_DATA_TEMPLATE2    "{\"method\":\"report\",\"clientToken\":\"%s\"," \
    "\"params\":{"  \
    "\"particles_0d3\":%d," \
    "\"particles_0d5\":%d,"	\
    "\"particles_1\":%d," \
    "\"particles_2d5\":%d," \
    "\"particles_5\":%d," \
    "\"particles_10\":%d," \
    "\"version\":%d," \
    "\"Error\":%d" \
    "}}"

void default_message_handler(mqtt_message_t* msg)
{
    printf("callback:\r\n");
    printf("---------------------------------------------------------\r\n");
    printf("\ttopic:%s\r\n", msg->topic);
    printf("\tpayload:%s\r\n", msg->payload);
    printf("---------------------------------------------------------\r\n");
}

char payload[256] = {0};
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
    ret = esp8266_tencent_firmware_sal_init(HAL_UART_PORT_2);
    
    if (ret < 0) {
        printf("esp8266 tencent firmware sal init fail, ret is %d\r\n", ret);
        return;
    }
    
    OLED_Clear();
    sprintf(str, "Connecting WIFI...");
    OLED_ShowString(0, 0, (uint8_t*)str, 16);
    while (1) {
        if ( esp8266_tencent_firmware_join_ap(WIFI_SSID, WIFI_PASSWD) == 0) {
            printf("module WIFI connect success\n");
            break;
        }
        printf("module WIFI connect fail\n");
        tos_sleep_ms(2000);
    }
    
    OLED_Clear();
    sprintf(str, "Connecting IoT Explorer...");
    OLED_ShowString(0, 0, (uint8_t*)str, 16);
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
    
    HAL_NVIC_DisableIRQ(USART3_4_IRQn);
    
    if (pm2d5_parser_init() == -1) {
        printf("pm2d5 parser init fail\r\n");
        return;
    }
  
    while (1) {
        /* 通过接收邮件来读取数据 */
        HAL_NVIC_EnableIRQ(USART3_4_IRQn);
        tos_mail_q_pend(&mail_q, (uint8_t*)&pm2d5_value, &mail_size, TOS_TIME_FOREVER);
        HAL_NVIC_DisableIRQ(USART3_4_IRQn);
        
        //收到之后打印信息
        printf("\r\n\r\n\r\n");
        for (i = 0; i < 13; i++) {
            printf("data[%d]:%d ug/m3\r\n", i+1, pm2d5_value.data[i]);
        }
        
        /* 显示PM2.5的值 */
        OLED_Clear();
        sprintf(str, "PM1.0:%4d ug/m3", pm2d5_value.pm2d5_data.data4);
        OLED_ShowString(0,0,(uint8_t*)str,16);
        
        /* 上报值 */
        generate_client_token(client_token, sizeof(client_token));
        memset(payload, 0, sizeof(payload));
        snprintf(payload, sizeof(payload), REPORT_DATA_TEMPLATE1, client_token,
                pm2d5_value.pm2d5_data.data1, pm2d5_value.pm2d5_data.data2,
                pm2d5_value.pm2d5_data.data3, pm2d5_value.pm2d5_data.data4,
                pm2d5_value.pm2d5_data.data5, pm2d5_value.pm2d5_data.data6);
        
        if (tos_tf_module_mqtt_publ(report_topic_name, QOS0, payload) != 0) {
            report_error_count++;
            printf("module mqtt publ fail, count: %d\n", report_error_count);
            sprintf(str, "1# report fail");
            OLED_ShowString(0,2,(uint8_t*)str,16);
        } else {
            report_error_count = 0;
            printf("module mqtt publ success\n");
            sprintf(str, "1# report ok");
            OLED_ShowString(0,2,(uint8_t*)str,16);
        }
        
        tos_sleep_ms(5000);
        
        OLED_Clear();
        sprintf(str, "PM2.5:%4d ug/m3", pm2d5_value.pm2d5_data.data5);
        OLED_ShowString(0,0,(uint8_t*)str,16);
        
        /* 上报值 */
        generate_client_token(client_token, sizeof(client_token));
        memset(payload, 0, sizeof(payload));
        snprintf(payload, sizeof(payload), REPORT_DATA_TEMPLATE2, client_token, 
                pm2d5_value.pm2d5_data.data7, pm2d5_value.pm2d5_data.data8,
                pm2d5_value.pm2d5_data.data9, pm2d5_value.pm2d5_data.data10,
                pm2d5_value.pm2d5_data.data11, pm2d5_value.pm2d5_data.data12,
                pm2d5_value.pm2d5_data.version, pm2d5_value.pm2d5_data.err_code);
        
        if (tos_tf_module_mqtt_publ(report_topic_name, QOS0, payload) != 0) {
            report_error_count++;
            printf("module mqtt publ fail, count: %d\n", report_error_count);
            sprintf(str, "2# report fail");
            OLED_ShowString(0,2,(uint8_t*)str,16);
        } else {
            report_error_count = 0;
            printf("module mqtt publ success\n");
            sprintf(str, "2# report ok");
            OLED_ShowString(0,2,(uint8_t*)str,16);
        }
        
        if (report_error_count >= 6) {
            HAL_NVIC_SystemReset();
        }
        
        tos_sleep_ms(5000);
    }
}

void application_entry(void *arg)
{
    char *str = "TencentOS-tiny";
    
    /* 初始化OLED */
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0, 0, (uint8_t*)str, 16);
    
    mqtt_demo_task();
    while (1) {
        printf("This is a mqtt demo!\r\n");
        tos_task_delay(1000);
    }
}
