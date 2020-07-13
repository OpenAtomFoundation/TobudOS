#include "stm32f1xx_hal.h"
#include "bsp_init.h"
#include "tos_k.h"
#include "esp8266.h"
#include "mqttclient.h"

k_task_t task;
k_stack_t task_stack[2048];

static void tos_topic_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------\n");
}


void mqttclient_task(void *Parameter)
{
    int error;

    char buf[100] = { 0 };
    mqtt_message_t msg;
    mqtt_client_t *client = NULL;
    
    memset(&msg, 0, sizeof(msg));

    printf("\nwelcome to mqttclient test...\n");
    
    esp8266_sal_init(HAL_UART_PORT_2);
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
