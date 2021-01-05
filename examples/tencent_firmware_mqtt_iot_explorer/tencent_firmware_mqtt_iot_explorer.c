#include "tos_k.h"
#include "esp8266_tencent_firmware.h"
#include "tencent_firmware_module_wrapper.h"

#define PRODUCT_ID              "FWR8PGACUS"
#define DEVICE_NAME             "dev001"
#define DEVICE_KEY              "XIBjgofTv/QEQTlRTDQnGg=="

#define REPORT_DATA_TEMPLATE    "{\\\"method\\\":\\\"report\\\"\\,\\\"clientToken\\\":\\\"00000001\\\"\\,\\\"params\\\":{\\\"brightness\\\":%d\\,\\\"name\\\":\\\"bedroom\\\"}}"

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

void mqtt_demo_task(void)
{
    int ret = 0;
    int size = 0;
    int lightness = 0;
    mqtt_state_t state;
    
    char *product_id = PRODUCT_ID;
    char *device_name = DEVICE_NAME;
    char *key = DEVICE_KEY;
    
    device_info_t dev_info;
    memset(&dev_info, 0, sizeof(device_info_t));

    /**
     * Please Choose your AT Port first, default is HAL_UART_2(USART2)
    */
    ret = esp8266_tencent_firmware_sal_init(HAL_UART_PORT_2);
    
    if (ret < 0) {
        printf("esp8266 tencent firmware sal init fail, ret is %d\r\n", ret);
    }
    
    esp8266_tencent_firmware_join_ap("Mculover666", "mculover666");

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
        tos_sleep_ms(5000);
        
        /* use AT+PUB AT command */
        memset(payload, 0, sizeof(payload));
        snprintf(payload, sizeof(payload), REPORT_DATA_TEMPLATE, lightness++);
        
        if (lightness > 100) {
            lightness = 0;
        }
        
        if (tos_tf_module_mqtt_pub(report_topic_name, QOS0, payload) != 0) {
            printf("module mqtt pub fail\n");
            break;
        } else {
            printf("module mqtt pub success\n");
        }
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
