#include "lwip/api.h"
#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <errno.h>

#include "mqttclient.h"

static void tos_topic_handler(void* client, message_data_t* msg)
{
    (void) client;
    LOG_I("-----------------------------------------------------------------------------------");
    LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
    LOG_I("-----------------------------------------------------------------------------------\n");
}


mqtt_client_t client;
client_init_params_t init_params;

extern void TCPIP_Init(void);

void mqtt_iothub(void)
{
    int error;
    char buf[80] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    
    sprintf(buf, "welcome to mqttclient, this is a publish test...");
    
    init_params.read_buf_size = 1024;
    init_params.write_buf_size = 1024;
    init_params.connect_params.network_params.addr = "iotcloud-mqtt.gz.tencentdevices.com";
    init_params.connect_params.network_params.port = "1883";
    init_params.connect_params.user_name = "RUAP1R610Vsupowang;12010126;FTDC9;1623108655"; 
    init_params.connect_params.password = "29e68d809ac4ac0e41bf30cc77cd377d30e7af03f3c76b0c50fa27a2c59e174b;hmacsha256"; 
    init_params.connect_params.client_id = "RUAP1R610Vsupowang";
    init_params.connect_params.clean_session = 1;

    log_init();

    mqtt_init(&client, &init_params);

    error = mqtt_connect(&client);
    
    LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(&client, "RUAP1R610V/supowang/data", QOS0, tos_topic_handler);

    memset(&msg, 0, sizeof(msg));

    for (;;) {

        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        mqtt_publish(&client, "RUAP1R610V/supowang/data", &msg);
        
        tos_task_delay(4000); 
    }

}
