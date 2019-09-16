#include "n32g020xx.h"
#include "n32g020xx_common.h"
#include "n32g020xx_log.h"
#include <string.h>
#include "tos_at.h"
#include "cmsis_os.h"
#include "sal_module_wrapper.h"
#include "mqtt_wrapper.h"
#include "mqtt_config.h"

#define APPLICATION_TASK_STK_SIZE           1024
extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

void mqtt_demo(void)
{
    int count = 1;
    int sock_id = 0;
    uint8_t read_data[40];
    int read_len;
    char topic[30];
    char buffer[32];

    mqtt_con_opt_t con_opt;
    con_opt.keep_alive_interval = 2000;
    con_opt.cleansession = 1;
    con_opt.username = MQTT_USR_NAME;
    con_opt.password = MQTT_PASSWORD;
    con_opt.client_id = MQTT_CLIENT_ID;

    mqtt_pub_opt_t pub_opt;
    pub_opt.dup = 0;
    pub_opt.qos = 0;
    pub_opt.retained = 0;
    pub_opt.id = 0;
    pub_opt.topic = MQTT_PUBLISH_TOPIC;

    mqtt_sub_opt_t sub_opt;
    sub_opt.count = 1;
    sub_opt.dup = 0;
    sub_opt.id = 1;
    sub_opt.req_qos = 0;
    sub_opt.topic = MQTT_SUBSCRIBE_TOPIC;

    extern int bc26_sal_init(hal_uart_port_t uart_port);
    bc26_sal_init(HAL_UART_PORT_0);  

    sock_id = tos_mqtt_connect(MQTT_SERVER_IP, MQTT_SERVER_PORT, &con_opt);

    printf("socket id: %d\r\n",sock_id);

    if (tos_mqtt_subscribe(sock_id, &sub_opt) != 0) {
        printf("subscribe failed!!!\n");
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        snprintf(buffer, sizeof(buffer), "@%3.1f@%3.1f@%.1f", 25.8,80.6, 90.1);

        printf("publish topic\r\n");
        pub_opt.topic = MQTT_PUBLISH_TOPIC;
        pub_opt.payload = (unsigned char *)buffer;
        pub_opt.payload_len = sizeof(buffer);
        if (tos_mqtt_publish(sock_id, &pub_opt) != 0) {
            printf("publish failed!!!\n");
        }

        printf("MQTT count: %d\r\n",count);
        printf("read buf\r\n");
        read_len = tos_mqtt_receive(topic, sizeof(topic), read_data, sizeof(read_data));
        if (read_len >= 0) {
            printf("---------->topic: %s, payload: %s, payload_len: %d\n", topic, read_data, read_len);
        }

        count++;
        osDelay(1000);
    }
}

void application_entry(void *arg)
{
    mqtt_demo();

    while (1) {
        printf("This is a demo task,please use your task entry!\r\n");
        tos_task_delay(1000);
    }
}

int main(void)
{
    log_init();
    log("Sys StartUp.\r\n");
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(application_entry), NULL);
    osKernelStart();
}

