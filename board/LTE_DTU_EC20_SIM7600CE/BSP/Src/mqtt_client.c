#include "ec20.h"
#include "mcu_init.h"
#include "tos_k.h"
#include "mqttclient.h"

//#define USE_SIM7600CE

#define USE_EC20

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
    
#ifdef USE_SIM7600CE 
    sim7600ce_sal_init(HAL_UART_PORT_2);
#endif

#ifdef USE_EC20
    ec20_sal_init(HAL_UART_PORT_2);
#endif
    mqtt_log_init();

    client = mqtt_lease();
    
    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "111.230.189.156");
    mqtt_set_client_id(client, "RUAP1R610Vsupowang");
    mqtt_set_user_name(client, "RUAP1R610Vsupowang;12010126;OQHZL;1626061655");
    mqtt_set_password(client, "ed41ae0cdf9100ce2aa5d5874566f8f4f77b1eab80b19ede4d1419eafe92eb54;hmacsha256");
    mqtt_set_clean_session(client, 1);
    mqtt_set_read_buf_size(client, 256);
    mqtt_set_write_buf_size(client, 256);
    
    error = mqtt_connect(client);
    
    MQTT_LOG_D("mqtt connect error is %#x", error);
    
    //mqtt_set_interceptor_handler(&client,tos_topic_handler);
    
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
