#include "farm_ai_demo.h"
#include "tos_farm_ai_config.h"
#include "cJSON.h"
#include "E53_IA1.h"
#include "mcu_init.h"

#define  USE_ESP8266

int sock_id = 0;

double math_keep_ndot(double num,char dot){
    char temp[100];
    sprintf((char *)temp,"%.*lf",dot,num);	
    sscanf(temp,"%lf",&num);
    return num;

}

static int str_to_hex(const char *bufin, int len, char *bufout)
{
    int i = 0;
    unsigned char tmp2 = 0x0;
    unsigned char tmp  = 0x0;
	
    if (NULL == bufin || len <= 0 || NULL == bufout)
    {
        return -1;
    }
	
    for(i = 0; i < len; i = i+2)
    {
        tmp2 =  bufin[i];
        tmp2 =  tmp2 <= '9'?tmp2-0x30:tmp2-0x37;
        tmp =  bufin[i+1];
        tmp =  tmp <= '9'?tmp-0x30:tmp-0x37;
        bufout[i/2] =(tmp2<<4)|(tmp&0x0F);

    }
	
    return 0; 
}

int mq_dev_report(void)
{
    char *data;
    cJSON *root = NULL;
    mqtt_pub_opt_t pub_opt;
    pub_opt.dup = 0;
    pub_opt.qos = 0;
    pub_opt.retained = 0;
    pub_opt.id = 0;
    pub_opt.topic = MQTT_PUBLISH_TOPIC;
    
    //{"motor":0,"light":1,"temperature":22.22,"humidity":55.55,"light_intensity":100.00}
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "motor",E53_IA1_Data.MotorMode);
    cJSON_AddNumberToObject(root, "light",E53_IA1_Data.LightMode);
    cJSON_AddNumberToObject(root, "temperature",math_keep_ndot((double)E53_IA1_Data.Temperature,2));
    cJSON_AddNumberToObject(root, "humidity",math_keep_ndot((double)E53_IA1_Data.Humidity,2));
    cJSON_AddNumberToObject(root, "light_intensity",math_keep_ndot((double)E53_IA1_Data.Lux,2));
     /* formatted print */
    data=cJSON_PrintUnformatted(root);

    pub_opt.topic = MQTT_PUBLISH_TOPIC;
    pub_opt.payload =(unsigned char *)data;
    pub_opt.payload_len = strlen(data);
    printf("\r\nreport--->topic:%s\r\ndata(%d):%s", pub_opt.topic,pub_opt.payload_len,pub_opt.payload);
    if (tos_mqtt_publish(sock_id, &pub_opt) != 0) {
        printf("publish failed!!!\n");
    }
    tos_mmheap_free(data);
    cJSON_Delete(root);
    
    return 0;	
}

int parse_dev_cmd(uint8_t *data)
{
    //{"motor":0,"light":1}
    //JSON字符串到cJSON格式
    cJSON* cjson = cJSON_Parse((char *)data); 
    cJSON *cjsonret=NULL;
    int motor;
    int light;
    char *str;
	
    printf("parse_dev_cmd...\n");
    if(cjson == NULL){
        printf("json pack into cjson error...");
        return -1;
    }
    else{
        cJSON_Print(cjson);
    }
	
    cjsonret = NULL;
    cjsonret = cJSON_GetObjectItem(cjson,"motor");
    if(cjsonret!=NULL)
    {
        //打印输出
        motor=cjsonret->valueint;
        printf("motor=%d\n",motor);
        if(motor==1)
        {   
            //power on relay
            motor_control(1);
        }	
        else if(motor==0)
        {
            //power off relay
            motor_control(0);
        }	
    }
	
    cjsonret = NULL;
    cjsonret = cJSON_GetObjectItem(cjson,"light");
    if(cjsonret!=NULL)
    {
        //打印输出
        light=cjsonret->valueint;
        //打印输出
        printf("light=%d\n",light);
        if(light==1)
        {
            //power on relay
            light_control(1);
        }	
        else if(light==0)
        {
            //power off relay
            light_control(0);
        }			
    }
    cjsonret = cJSON_GetObjectItem(cjson,"speaker");
    if(cjsonret!=NULL)
    {
        //打印输出
        printf("parse speaker:%s\n",cjsonret->valuestring);
        str=tos_mmheap_alloc(strlen(cjsonret->valuestring));
        if(str!=NULL)
        {
            memset(str,0x00,strlen(cjsonret->valuestring));
            str_to_hex(cjsonret->valuestring,strlen(cjsonret->valuestring),str);
            extern void speaker(char *str);	
    	    speaker(str);
            tos_mmheap_free(str);
        }
    } 
    //delete cjson
    cJSON_Delete(cjson);
    
    return 0;	
}

int mq_dev_receiv(void)
{
    static int count = 1;
    uint8_t read_data[100];
    int read_len;
    mqtt_sub_opt_t sub_opt;

    sub_opt.count = 1;
    sub_opt.dup = 0;
    sub_opt.id = 1;
    sub_opt.req_qos = 0;
    sub_opt.topic = MQTT_SUBSCRIBE_TOPIC;

    printf("MQTT count: %d\r\n",count);
    printf("read buf1,\r\n");
    read_len = tos_mqtt_receive(sub_opt.topic, strlen(sub_opt.topic), read_data, sizeof(read_data));
    if (read_len >= 0) {
        printf("---------->topic: %s, payload: %s, payload_len: %d\n", sub_opt.topic, read_data, read_len);
        parse_dev_cmd(read_data);
    }
    count++;
		
    return 0;
}

int mq_dev_subscribe(void)
{
    mqtt_sub_opt_t sub_opt;
    sub_opt.count = 1;
    sub_opt.dup = 0;
    sub_opt.id = 1;
    sub_opt.req_qos = 0;
    sub_opt.topic = MQTT_SUBSCRIBE_TOPIC;

    printf("subscribe topic1:%s",sub_opt.topic);
    if (tos_mqtt_subscribe(sock_id, &sub_opt) != 0) {
        printf("subscribe failed!!!\n");
    }
	
    return 0;
}
void ai_demo(void)
{
    mqtt_con_opt_t con_opt;
#ifdef USE_ESP8266
    extern int esp8266_sal_init(hal_uart_port_t uart_port);
    extern int esp8266_join_ap(const char *ssid, const char *pwd);
    esp8266_sal_init(HAL_UART_PORT_0);
    esp8266_join_ap("test", "12345678");
#endif

    con_opt.keep_alive_interval = 2000;
    con_opt.cleansession = 1;
    con_opt.username = MQTT_USR_NAME;
    con_opt.password = MQTT_PASSWORD;
    con_opt.client_id = MQTT_CLIENT_ID;

    sock_id = tos_mqtt_connect(MQTT_SERVER_IP, MQTT_SERVER_PORT, &con_opt);
    printf("socke id id %d \r\n",sock_id);
    mq_dev_subscribe();
    while (1)
    {
        mq_dev_report();
        mq_dev_receiv();
        osDelay(1000);
    }
}

void sensor_read(void)
{
    while(1)
    {
        E53_IA1_Read_Data();
        osDelay(100);
    }
}

#define welcomspeak "欢迎TOS"
void speaker(char *str)
{
    uint8_t data[100];
    int16_t len=0;

    len=2+strlen(str);
    printf("speaker(%s)\n",str);
    data[0]=0xFD;
    data[1]=len>>16;
    data[2]=len&0x00FF;
    data[3]=0x01;
    data[4]=0x01;
    memcpy(&data[5],str,len-2);
    HAL_UART_Transmit(&huart3,data,len+3,1000);
}

#define SENSOR_READ_TASK_STK_SIZE		2048
extern void sensor_read(void);
osThreadDef(sensor_read, osPriorityNormal, 1, SENSOR_READ_TASK_STK_SIZE);

void application_entry(void *arg)
{
    printf("mqtt demo run!\r\n");
    Init_E53_IA1();
    speaker(welcomspeak);
    osThreadCreate(osThread(sensor_read), NULL);
    ai_demo();
    while (1) {
        printf("This is a mqtt demo!\r\n");
        tos_task_delay(1000);
    }
}
