/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 21:53:07
 * @LastEditTime : 2020-01-18 13:54:38
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "stdio.h"
#include "string.h"
#include "ethernetif.h"
#include "lwip/api.h"
#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <errno.h>

#include "mqttclient.h"

extern const char *test_ca_get(void);

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

void application_entry(void *arg)
{
    int error;
    char buf[80] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    
    sprintf(buf, "welcome to mqttclient, this is a publish test...");
    
    TCPIP_Init();
    
    init_params.read_buf_size = 1024;
    init_params.write_buf_size = 1024;
    init_params.connect_params.network_params.network_ssl_params.ca_crt = test_ca_get();
    init_params.connect_params.network_params.addr = "www.jiejie01.top";
    init_params.connect_params.network_params.port = "8883";
    init_params.connect_params.user_name = random_string(10); 
    init_params.connect_params.password = random_string(10); 
    init_params.connect_params.client_id = random_string(10);
    init_params.connect_params.clean_session = 1;

    log_init();

    mqtt_init(&client, &init_params);

    error = mqtt_connect(&client);
    
    LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(&client, "tos-topic", QOS0, tos_topic_handler);

    memset(&msg, 0, sizeof(msg));

    for (;;) {

        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        mqtt_publish(&client, "tos-topic", &msg);
        
        tos_task_delay(4000); 
    }

}

