#include "cmsis_os.h"
#include "socket_wrapper.h"
#include "sal_module_wrapper.h"
#include "mqtt_wrapper.h"
#include "mqtt_config.h"

#include <stdio.h>

int sock_id = 0;

//mqtt_publisher
#define MQTT_PUBLISHER_STK_SIZE 1024
void mqtt_publisher(void *pdata);
osThreadDef(mqtt_publisher, osPriorityNormal, 1, MQTT_PUBLISHER_STK_SIZE);

//mqtt_reciever
#define MQTT_RECIEVER_STK_SIZE 1024
void mqtt_reciever(void *pdata);
osThreadDef(mqtt_reciever, osPriorityNormal, 1, MQTT_RECIEVER_STK_SIZE);

void mqtt_publisher(void *pdata)
{
   mqtt_con_opt_t con_param;
   con_param.keep_alive_interval = 2000;
   con_param.cleansession = 1;
   con_param.username = MQTT_USR_NAME;
   con_param.password = MQTT_PASSWORD;
   con_param.client_id = MQTT_CLIENT_ID;

   mqtt_pub_opt_t pub_param;
   pub_param.dup = 0;
   pub_param.qos = 0;
   pub_param.retained = 0;
   pub_param.id = 0;
   pub_param.payload = "hello tencent cloud";
   pub_param.payload_len = 20;
   pub_param.topic = MQTT_PUBLISH_TOPIC;

   mqtt_sub_opt_t sub_param;
   sub_param.count = 1;
   sub_param.dup = 0;
   sub_param.id = 0;
   sub_param.req_qos = 0;
   sub_param.topic = MQTT_SUBSCRIBE_TOPIC;

   printf("start connect\n");
   tos_sal_module_register(get_socket_module());
   tos_sal_module_init();

   sock_id = tos_mqtt_connect(MQTT_SERVER_IP, MQTT_SERVER_PORT, &con_param);
   if (sock_id == -1)
   {
      printf("connect failed!!!\n");
      return -1; //to exit thread
   }
   printf("connect success\n");

   if (tos_mqtt_subscribe(sock_id, &sub_param) != 0)
   {
      printf("subscribe failed!!!\n");
   }else{
      printf("subscribe success\n");
   }

   osThreadCreate(osThread(mqtt_reciever), NULL); // start receive

   for (;;)
   {
      printf("\n");
      printf("publish topic-->%s| data-->%s| \n", pub_param.topic, pub_param.payload);
      if (tos_mqtt_publish(sock_id, &pub_param) != 0) {
         printf("publish failed!!!\n");
      }
      osDelay(2000);
   }
}

void mqtt_reciever(void *pdata)
{
   uint8_t read_data[100];
   int8_t topic[30];
   uint32_t read_len;

   for (;;)
   {
      read_len = tos_mqtt_receive(topic, sizeof(topic), read_data, sizeof(read_data));
      if (read_len >= 0)
      {
         printf("receive topic-->%s| data-->%s| \n", topic, read_data);
      }
      osDelay(100);
   }
}

int main(void)
{
   osKernelInitialize();                           //TOS Tiny kernel initialize
   osThreadCreate(osThread(mqtt_publisher), NULL); // start connect and publish
   osKernelStart();                                //Start TOS Tiny

   while (1)
   {
   }
}
