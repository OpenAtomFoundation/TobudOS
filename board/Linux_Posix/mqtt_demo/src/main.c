#include "cmsis_os.h"
#include "socket_wrapper.h"
#include "sal_module_wrapper.h"
// #include "mqtt_wrapper.h"
#include "mqtt_config.h"
#include "mqttclient.h"

#include <stdio.h>

int sock_id = 0;

//mqtt_publisher
#define MQTT_PUBLISHER_STK_SIZE 1024
void mqtt_publisher(void *pdata);
osThreadDef(mqtt_publisher, osPriorityNormal, 1, MQTT_PUBLISHER_STK_SIZE);

static void tos_topic_handler(void* client, message_data_t* msg)
{
   (void)client;
   MQTT_LOG_I("-----------------------------------------------------------------------------------");
   MQTT_LOG_I("%s:%d %s()...\ntopic: %s, qos: %d. \nmessage:\n\t%s\n", __FILE__, __LINE__, __FUNCTION__,
              msg->topic_name, msg->message->qos, (char *)msg->message->payload);
   MQTT_LOG_I("-----------------------------------------------------------------------------------\n");
}

void mqtt_publisher(void *pdata)
{
   int error;
   char buf[100] = {0};
   mqtt_client_t *client = NULL;
   mqtt_message_t msg;

   memset(&msg, 0, sizeof(msg));

   mqtt_log_init();
   client = mqtt_lease();

   mqtt_set_port(client, MQTT_SERVER_PORT);
   mqtt_set_host(client, MQTT_SERVER_IP);
   mqtt_set_client_id(client, MQTT_CLIENT_ID);
   mqtt_set_user_name(client, MQTT_USR_NAME);
   mqtt_set_password(client, MQTT_PASSWORD);
   mqtt_set_clean_session(client, 1);

   error = mqtt_connect(client);

   MQTT_LOG_D("mqtt connect error is %#x", error);

   mqtt_subscribe(client, MQTT_SUBSCRIBE_TOPIC, QOS0, tos_topic_handler);

   MQTT_LOG_D("mqtt subscribe error is %#x", error);

   memset(&msg, 0, sizeof(msg));

   for (;;)
   {
      sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());

      msg.qos = QOS0;
      msg.payload = (void *)buf;

      error = mqtt_publish(client, MQTT_PUBLISH_TOPIC, &msg);

      osDelay(4000);
   }
}

// void mqtt_reciever(void *pdata)
// {
//    uint8_t read_data[100];
//    int8_t topic[30];
//    uint32_t read_len;

//    for (;;)
//    {
//       read_len = tos_mqtt_receive(topic, sizeof(topic), read_data, sizeof(read_data));
//       if (read_len >= 0)
//       {
//          printf("receive topic-->%s| data-->%s| \n", topic, read_data);
//       }
//       osDelay(100);
//    }
// }

int main(void)
{
   osKernelInitialize();                           //TOS Tiny kernel initialize
   osThreadCreate(osThread(mqtt_publisher), NULL); // start connect and publish
   osKernelStart();                                //Start TOS Tiny

   while (1)
   {
   }
}
