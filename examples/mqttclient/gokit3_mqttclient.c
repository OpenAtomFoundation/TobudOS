#include "stm32f1xx_hal.h"
#include "bsp_init.h"
#include "tos_k.h"
#include "esp8266.h"
#include "mqttclient.h"

k_task_t task;
k_stack_t task_stack[2048];

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


void mqttclient_task(void *Parameter)
{
    int error;

    char buf[100] = { 0 };
    
    mqtt_message_t msg;
    
    memset(&msg, 0, sizeof(msg));
    
    esp8266_sal_init(HAL_UART_PORT_2);
    esp8266_join_ap("wifii", "woshijiejie");
    
    init_params.read_buf_size = 256;
    init_params.write_buf_size = 256;
    
#ifdef MQTT_NETWORK_TYPE_TLS
    init_params.connect_params.network_params.network_ssl_params.ca_crt = test_ca_get();
    init_params.connect_params.network_params.port = "8883";
#else
    init_params.connect_params.network_params.port = "1883";
#endif
    init_params.connect_params.network_params.addr = "www.jiejie01.top"; //"47.95.164.112";//"jiejie01.top"; //"129.204.201.235"; //"192.168.1.101";

    init_params.connect_params.user_name = random_string(10); // random_string(10); //"jiejietop-acer1";
    init_params.connect_params.password = random_string(10);; //random_string(10); // "123456";
    init_params.connect_params.client_id = random_string(10);; //random_string(10); // "clientid-acer1";
    init_params.connect_params.clean_session = 1;

    log_init();

    mqtt_init(&client, &init_params);

    error = mqtt_connect(&client);
    
    LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(&client, "tos-topic", QOS0, tos_topic_handler);
    
    LOG_D("mqtt subscribe error is %#x", error);
    
    memset(&msg, 0, sizeof(msg));

    for (;;) {
        
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());
        
        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        error = mqtt_publish(&client, "tos-topic", &msg);

        tos_task_delay(4000); 
    }
}


int main(void)
{

    k_err_t err;
    
    bsp_init();
    
    printf("Welcome to TencentOS tiny\r\n");

    tos_knl_init(); // TOS Tiny kernel initialize
    
    printf("create mqttclient task\r\n");
    err = tos_task_create(&task, 
                          "mqttclient-task", 
                          mqttclient_task,
                          NULL, 
                          3, 
                          task_stack,
                          1024,
                          20);
    if(err != K_ERR_NONE)
        printf("TencentOS Create mqttclient task fail! code : %d \r\n",err);
    
    tos_knl_start(); // Start TOS Tiny

}
