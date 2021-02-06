#include "stm32l4xx_hal.h"
#include "mcu_init.h"
#include "tos_k.h"
#include "mqttclient.h"

#define USE_ESP8266
//#define USE_NB_BC35
//#define USE_EC600S

#ifdef USE_ESP8266
#include "esp8266.h"
#endif

#ifdef USE_EC600S
#include "ec600s.h"
#endif

/* Tencent IoThub Device Info */
#define IOTHUB_MQTT_IP          "111.230.189.156"
#define IOTHUB_MQTT_PORT        "1883"
#define IOTHUB_MQTT_CLIENT_ID   "KHHIXM0JJ5dev001"
#define IOTHUB_MQTT_USERNAME    "KHHIXM0JJ5dev001;12010126;9K3WO;1647628324"
#define IOTHUB_MQTT_PASSWD      "53696f22a061ed2524b1de185815ad2fcae7739afd3f395cb6131521da7f0bb7;hmacsha256"
#define IOTHUB_MQTT_PUB_TOPIC   "KHHIXM0JJ5/dev001/data"
#define IOTHUB_MQTT_SUB_TOPIC   "KHHIXM0JJ5/dev001/control"

static void tos_topic_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------\n");
}


void mqttclient_task(void)
{
    int error;

    char buf[100] = { 0 };
    
    mqtt_client_t *client = NULL;
    
    mqtt_message_t msg;
    
    memset(&msg, 0, sizeof(msg));
    
#ifdef USE_ESP8266 
    esp8266_sal_init(HAL_UART_PORT_0);
    esp8266_join_ap("Mculover666", "mculover666");
#endif

#ifdef USE_NB_BC35
    int bc35_28_95_sal_init(hal_uart_port_t uart_port);
    bc35_28_95_sal_init(HAL_UART_PORT_0);
#endif
    
#ifdef USE_EC600S
    ec600s_sal_init(HAL_UART_PORT_0);
#endif
    
    
    mqtt_log_init();

    client = mqtt_lease();
    
    mqtt_set_port(client, IOTHUB_MQTT_PORT);
    mqtt_set_host(client, IOTHUB_MQTT_IP);
    mqtt_set_client_id(client, IOTHUB_MQTT_CLIENT_ID);
    mqtt_set_user_name(client, IOTHUB_MQTT_USERNAME);
    mqtt_set_password(client, IOTHUB_MQTT_PASSWD);
    mqtt_set_clean_session(client, 1);

    error = mqtt_connect(client);
    
    MQTT_LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(client, IOTHUB_MQTT_SUB_TOPIC, QOS0, tos_topic_handler);
    
    MQTT_LOG_D("mqtt subscribe error is %#x", error);
    
    memset(&msg, 0, sizeof(msg));

    for (;;) {
        
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());
        
        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        error = mqtt_publish(client, IOTHUB_MQTT_PUB_TOPIC, &msg);

        tos_task_delay(4000); 
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
