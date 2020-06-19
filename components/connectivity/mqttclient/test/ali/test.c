/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 21:53:07
 * @LastEditTime: 2020-06-08 20:40:47
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "mqttclient.h"

static void topic1_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

void *mqtt_publish_thread(void *arg)
{
    mqtt_client_t *client = (mqtt_client_t *)arg;
    
    char buf[100] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "welcome to mqttclient, this is a publish test...");

    sleep(2);

    mqtt_list_subscribe_topic(client);

    msg.payload = (void *) buf;
    msg.qos = 0;
    while(1) {
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());
        mqtt_publish(client, "/a1w7XupONEX/test1/user/topic1", &msg);
        sleep(4);
    }
}

int main(void)
{
    int res;
    pthread_t thread1;
    mqtt_client_t *client = NULL;
    
    printf("\nwelcome to mqttclient test...\n");

    mqtt_log_init();

    client = mqtt_lease();
    
    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "a1w7XupONEX.iot-as-mqtt.cn-shanghai.aliyuncs.com");
    mqtt_set_client_id(client, "123456|securemode=3,signmethod=hmacsha1|");
    mqtt_set_user_name(client, "test1&a1w7XupONEX");
    mqtt_set_password(client, "A9EFF34CCA05EABAE560373CBED3E43AC88956CF");
    mqtt_set_clean_session(client, 1);

    mqtt_connect(client);
    
    mqtt_subscribe(client, "/a1w7XupONEX/test1/user/topic1", QOS0, topic1_handler);
    
    res = pthread_create(&thread1, NULL, mqtt_publish_thread, client);
    if(res != 0) {
        MQTT_LOG_E("create mqtt publish thread fail");
        exit(res);
    }

    while (1) {
        sleep(100);
    }
}
