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

#define TEST_USEING_TLS  

extern const char *test_ca_get(void);

static void tos_topic_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------\n");
}


extern void TCPIP_Init(void);

void application_entry(void *arg)
{
    int error;
    char buf[80] = { 0 };
    mqtt_client_t *client = NULL;
    
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    
    sprintf(buf, "welcome to mqttclient, this is a publish test...");
    
    TCPIP_Init();
    
    mqtt_log_init();

    client = mqtt_lease();

#ifdef TEST_USEING_TLS
    mqtt_set_port(client, "8883");
    mqtt_set_ca(client, (char*)test_ca_get());
#else
    mqtt_set_port(client, "1883");
#endif

    mqtt_set_host(client, "www.jiejie01.top");
    mqtt_set_client_id(client, random_string(10));
    mqtt_set_user_name(client, random_string(10));
    mqtt_set_password(client, random_string(10));
    mqtt_set_clean_session(client, 1);

    error = mqtt_connect(client);
    
    MQTT_LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(client, "tos-topic", QOS0, tos_topic_handler);
    
    MQTT_LOG_D("mqtt subscribe error is %#x", error);
    
    memset(&msg, 0, sizeof(msg));

    for (;;) {
        
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());
        
        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        mqtt_publish(client, "tos-topic", &msg);
        
        tos_task_delay(4000); 
    }

}

