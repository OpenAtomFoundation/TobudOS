#include "stm32l4xx_hal.h"
#include "mcu_init.h"
#include "tos_k.h"
#include "esp8266.h"
#include "mqttclient.h"

//#define USE_ESP8266

#define USE_NB_BC35

#ifdef USE_ESP8266 
static hal_uart_port_t esp8266_port = HAL_UART_PORT_0;

void mqtt_set_esp8266_port(hal_uart_port_t port) {
    esp8266_port = port;
}
#endif

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
    esp8266_sal_init(esp8266_port);
    esp8266_join_ap("Supowang", "13975428888");
#endif

#ifdef USE_NB_BC35
    int bc35_28_95_sal_init(hal_uart_port_t uart_port);
    bc35_28_95_sal_init(HAL_UART_PORT_0);
#endif
    
    mqtt_log_init();

    client = mqtt_lease();
    
    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "111.230.189.156");
    mqtt_set_client_id(client, "RUAP1R610Vsupowang");
    mqtt_set_user_name(client, "RUAP1R610Vsupowang;12010126;80LJ2;1621294860");
    mqtt_set_password(client, "2e90a9f1e604f2f0473355228c3d59276951aa8e6ed4d27f270f05ee8a4d0983;hmacsha256");
    mqtt_set_clean_session(client, 1);

    error = mqtt_connect(client);
    
    MQTT_LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(client, "RUAP1R610V/supowang/data", QOS0, tos_topic_handler);
    
    MQTT_LOG_D("mqtt subscribe error is %#x", error);
    
    memset(&msg, 0, sizeof(msg));

    for (;;) {
        
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());
        
        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        error = mqtt_publish(client, "RUAP1R610V/supowang/data", &msg);

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
