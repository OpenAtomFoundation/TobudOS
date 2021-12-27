#include "bh1750_i2c_drv.h"
#include "tos_k.h"
#include "esp8266_tencent_firmware.h"
#include "tencent_firmware_module_wrapper.h"
#include "math.h"

#define USE_WIFI

#define WIFI_SSID               "Mculover666"
#define WIFI_PASSWD             "mculover666"
#define PRODUCT_ID              "4MBSKQWDO9"
#define DEVICE_NAME             "TOS_EVB_WL"
#define DEVICE_KEY              "C9s+2U3B7uXrzngyXggsyA=="

#define REPORT_DATA_TEMPLATE    "{\"method\":\"report\",\"clientToken\":\"%s\",\"params\":{\"lux\":%d}}"

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
    uint8_t report_error_count = 0;
    char client_token[10];
    uint16_t lux;

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
    
    while (1) {
        // 读取传感器数据
        if (bh1750_read_data_once(&lux) != 0) {
            printf("bh1750 read fail\r\n");
            tos_sleep_ms(5000);
            continue;
        }
        
        // 收到之后打印信息
        printf("lux[%d]\r\n", lux);
        
        // 上报值
        generate_client_token(client_token, sizeof(client_token));
        memset(payload, 0, 1024);
        snprintf(payload, 1024, REPORT_DATA_TEMPLATE, client_token, lux);
        
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

void application_entry(void *arg)
{
    mqtt_demo_task();
    while (1) {
        printf("This is a mqtt demo!\r\n");
        tos_task_delay(1000);
    }    

}
