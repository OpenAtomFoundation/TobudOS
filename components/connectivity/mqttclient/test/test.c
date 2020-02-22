/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 21:53:07
 * @LastEditTime : 2020-01-18 13:54:38
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "mqttclient.h"

extern const char *test_ca_get();

mqtt_client_t client;
client_init_params_t init_params;

// static void reconnect_handler(void* client, void* reconnect_date)
// {
//     LOG_E("%s:%d %s()...mqtt is reconnecting, reconnect_date is %s", __FILE__, __LINE__, __FUNCTION__, (char*)reconnect_date);
// }

static void topic_test1_handler(void* client, message_data_t* msg)
{
    (void) client;
    LOG_I("-----------------------------------------------------------------------------------");
    LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    LOG_I("-----------------------------------------------------------------------------------");
}

// void *mqtt_unsubscribe_thread(void *arg)
// {
//     sleep(2);
//     mqtt_unsubscribe(&client, "test");

//     // sleep(10);
//     mqtt_disconnect(&client);

//     sleep(2);

//     mqtt_connect(&client);

//     pthread_exit(NULL);
// }

void *mqtt_publish_thread(void *arg)
{
    char buf[80] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "welcome to mqttclient, this is a publish test...");
    
    msg.qos = 2;
    msg.payload = (void *) buf;
    // msg.payloadlen = strlen(buf);
    while(1) {
        mqtt_publish(&client, "testtopic1-acer3", &msg);
        mqtt_publish(&client, "testtopic2-acer3", &msg);
        mqtt_publish(&client, "testtopic3-acer3", &msg);
        mqtt_publish(&client, "testtopic4-acer3", &msg);
        // LOG_I("random_number is %d",random_number());
        sleep(2);
    }
}

int main(void)
{
    int res;
    // pthread_t thread1;
    pthread_t thread2;

    init_params.read_buf_size = 1024;
    init_params.write_buf_size = 1024;
    // init_params.reconnect_date = "this is a test";
    // init_params.reconnect_handler = reconnect_handler;
    init_params.connect_params.network_params.network_ssl_params.ca_crt = test_ca_get();
    init_params.connect_params.network_params.addr = "www.jiejie01.top"; //"47.95.164.112";//"jiejie01.top"; //"129.204.201.235"; //"192.168.1.101";
    init_params.connect_params.network_params.port = "8883";
    init_params.connect_params.user_name = random_string(10); // random_string(10); //"jiejietop-acer1";
    init_params.connect_params.password = random_string(10);; //random_string(10); // "123456";
    init_params.connect_params.client_id = random_string(10);; //random_string(10); // "clientid-acer1";
    init_params.connect_params.clean_session = 1;

    log_init();

    mqtt_init(&client, &init_params);

    mqtt_connect(&client);

    LOG_D("mqtt connect success...");
    
    mqtt_subscribe(&client, "testtopic1-acer3", QOS2, topic_test1_handler);
    mqtt_subscribe(&client, "testtopic2-acer3", QOS2, NULL);
    mqtt_subscribe(&client, "testtopic3-acer3", QOS2, NULL);
    mqtt_subscribe(&client, "testtopic4-acer3", QOS2, NULL);
    mqtt_subscribe(&client, "testtopic5-acer3", QOS1, NULL);
    mqtt_subscribe(&client, "testtopic6-acer3", QOS2, NULL);
    mqtt_subscribe(&client, "testtopic7-acer3", QOS0, NULL);

    //  LOG_E("create mqtt publish thread fail");
    // res = pthread_create(&thread1, NULL, mqtt_unsubscribe_thread, NULL);
    // if(res != 0) {
    //     LOG_I("create thread2 fail");
    //     exit(res);
    // }
    
    res = pthread_create(&thread2, NULL, mqtt_publish_thread, NULL);
    if(res != 0) {
        LOG_E("create mqtt publish thread fail");
        exit(res);
    }

    while (1) {
        sleep(100);
    }
}