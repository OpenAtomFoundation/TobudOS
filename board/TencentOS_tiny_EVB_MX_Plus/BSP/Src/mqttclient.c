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

mqtt_client_t client;
client_init_params_t init_params;

static void tos_topic_handler(void* client, message_data_t* msg)
{
    (void) client;
    LOG_I("-----------------------------------------------------------------------------------");
    LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
    LOG_I("-----------------------------------------------------------------------------------\n");
}


void mqttclient_task(void)
{
    int error;

    char buf[100] = { 0 };
    
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
    
    init_params.read_buf_size = 256;
    init_params.write_buf_size = 256;
    
#ifdef MQTT_NETWORK_TYPE_TLS
    init_params.connect_params.network_params.network_ssl_params.ca_crt = test_ca_get();
    init_params.connect_params.network_params.port = "8883";
#else
    init_params.connect_params.network_params.port = "1883";
#endif
    init_params.connect_params.network_params.addr = "111.230.189.156"; //"47.95.164.112";//"jiejie01.top"; //"129.204.201.235"; //"192.168.1.101";

    init_params.connect_params.user_name = "RUAP1R610Vsupowang;12010126;80LJ2;1621294860"; // random_string(10); //"jiejietop-acer1";
    init_params.connect_params.password = "2e90a9f1e604f2f0473355228c3d59276951aa8e6ed4d27f270f05ee8a4d0983;hmacsha256"; //random_string(10); // "123456";
    init_params.connect_params.client_id = "RUAP1R610Vsupowang";; //random_string(10); // "clientid-acer1";
    init_params.connect_params.clean_session = 1;

    log_init();

    mqtt_init(&client, &init_params);

    error = mqtt_connect(&client);
    
    LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(&client, "RUAP1R610V/supowang/data", QOS0, tos_topic_handler);
    
    LOG_D("mqtt subscribe error is %#x", error);
    
    memset(&msg, 0, sizeof(msg));

    for (;;) {
        
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());
        
        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        error = mqtt_publish(&client, "RUAP1R610V/supowang/data", &msg);

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
