#include "lwip/api.h"
#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <errno.h>

#include "mqttclient.h"

static void tos_topic_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------\n");
}

extern void TCPIP_Init(void);

void mqtt_iothub(void)
{
    int error;
    char buf[80] = { 0 };
    
    mqtt_client_t *client = NULL;
    
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    
    sprintf(buf, "welcome to mqttclient, this is a publish test...");
    
    mqtt_log_init();

    client = mqtt_lease();
    
    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "iotcloud-mqtt.gz.tencentdevices.com");
    mqtt_set_client_id(client, "RUAP1R610Vsupowang");
    mqtt_set_user_name(client, "RUAP1R610Vsupowang;12010126;FTDC9;1623108655");
    mqtt_set_password(client, "29e68d809ac4ac0e41bf30cc77cd377d30e7af03f3c76b0c50fa27a2c59e174b;hmacsha256");
    mqtt_set_clean_session(client, 1);
    

    error = mqtt_connect(client);
    
    MQTT_LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(client, "RUAP1R610V/supowang/data", QOS0, tos_topic_handler);

    memset(&msg, 0, sizeof(msg));

    for (;;) {

        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        mqtt_publish(client, "RUAP1R610V/supowang/data", &msg);
        
        tos_task_delay(4000); 
    }

}
