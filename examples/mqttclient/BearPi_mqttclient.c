#include "tos_k.h"
#include "esp8266.h"
#include "mqttclient.h"

#define USE_ESP8266

static hal_uart_port_t esp8266_port = HAL_UART_PORT_0;

static void tos_topic_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------\n");
}


void mqtt_demo(void)
{
    int error;

    char buf[100] = { 0 };
    mqtt_message_t msg;
    mqtt_client_t *client = NULL;
    
    memset(&msg, 0, sizeof(msg));

    printf("\nwelcome to mqttclient test...\n");
    
#ifdef USE_ESP8266 
    esp8266_sal_init(esp8266_port);
    esp8266_join_ap("wifii", "woshijiejie");
#endif

#ifdef USE_NB_BC35
    int bc35_28_95_sal_init(hal_uart_port_t uart_port);
    bc35_28_95_sal_init(HAL_UART_PORT_0);
#endif
    
    esp8266_join_ap("wifii", "woshijiejie");
    
    mqtt_log_init();

    client = mqtt_lease();

    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "www.jiejie01.top");
    mqtt_set_client_id(client, random_string(10));
    mqtt_set_user_name(client, random_string(10));
    mqtt_set_password(client, random_string(10));
    mqtt_set_clean_session(client, 1);
    mqtt_set_read_buf_size(client, 256);
    mqtt_set_write_buf_size(client, 256);

    error = mqtt_connect(client);
    
    MQTT_LOG_D("mqtt connect error is %#x", error);
    

    error = mqtt_connect(client);
    
    MQTT_LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(client, "tos-topic", QOS0, tos_topic_handler);
    
    MQTT_LOG_D("mqtt subscribe error is %#x", error);
    
    memset(&msg, 0, sizeof(msg));

    for (;;) {
        
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());
        
        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        error = mqtt_publish(client, "tos-topic", &msg);

        tos_task_delay(4000); 
    }
}


void application_entry(void *arg)
{
    mqtt_demo();
    while (1) {
        printf("This is a mqtt demo!\r\n");
        tos_task_delay(1000);
    }
}
