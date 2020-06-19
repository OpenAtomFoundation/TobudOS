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

static void tos_topic_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------\n");
}


mqtt_client_t client;

extern void TCPIP_Init(void);

void application_entry(void *arg)
{
    int error;
    char buf[80] = { 0 };
    mqtt_message_t msg;
    mqtt_client_t *client = NULL;
    memset(&msg, 0, sizeof(msg));
    
    printf("\nwelcome to mqttclient test...\n");
    
    TCPIP_Init();
    
    mqtt_log_init();

    client = mqtt_lease();

    mqtt_set_port(client, "1883");

    mqtt_set_host(client, "iotcloud-mqtt.gz.tencentdevices.com");
    mqtt_set_client_id(client, "7ABAKP6KNCtest1");
    mqtt_set_user_name(client, "7ABAKP6KNCtest1;12010126;KGRDH;1618120908");
    mqtt_set_password(client, "35c065988c4beeb32d8a2f1f283a7a1535e79295f4b6368f2ec61ee7b1b8c271;hmacsha256");
    mqtt_set_clean_session(client, 1);
    
    error = mqtt_connect(client);
    
    MQTT_LOG_D("mqtt connect error is %#x", error);
    
    mqtt_subscribe(client, "7ABAKP6KNC/test1/data", QOS0, tos_topic_handler);

    memset(&msg, 0, sizeof(msg));

    sprintf(buf, "welcome to mqttclient, this is a publish test...");

    for (;;) {

        msg.qos = QOS0;
        msg.payload = (void *) buf;
        
        mqtt_publish(client, "7ABAKP6KNC/test1/data", &msg);
        
        tos_task_delay(4000); 
    }

}

