#include "mcu_init.h"
#include "sal_module_wrapper.h"
#include "cmsis_os.h"
#include "ml302.h"



#define ML302_RESET_Pin GPIO_PIN_14
#define ML302_RESET_GPIO_Port GPIOB
#define ML302_ENABLE_Pin GPIO_PIN_15
#define ML302_ENABLE_GPIO_Port GPIOA

float BH1750_Convert(void);
void MX_ML302_GPIO_Init(void);
static void ml302_power_on(void);
static void ml302_power_off(void);
static void ml302_hardware_reset(void);


int socket_id_0 = -1;
#define RECV_LEN  1024
uint8_t recv_data_0[RECV_LEN];

#define TCP_TEST_TASK0_STK_SIZE         4096
void tcp_test0(void);
osThreadDef(tcp_test0, osPriorityNormal, 1, TCP_TEST_TASK0_STK_SIZE);

#define ONENET_SERVER_IP  		"183.230.40.33"
#define ONENET_SERVER_PORT 		"80"
#define ONENET_DEVICE_ID  		"604618155"
#define ONENET_API_KEY	  		"NGSRfMY43gLebqieuaX4UfPgoac="

#define http_uploader_head	\
    "POST /devices/%s/datapoints?type=3 HTTP/1.1\r\n"	\
    "api-key:%s\r\n"					\
    "Host:api.heclouds.com\r\n"									\
    "Content-Length:%d\r\n\r\n"

#define http_uploader_data \
    "{\"lux\":%d}"

char buf[512] = {0};
char buf_data[120] = {0};
int OneNet_Packet_Send(const char *device_id, const char *api_key, int lux)
{
    int len = 0 ;
    int ret = -1 ;
    sprintf(buf_data, http_uploader_data, lux);
    len = strlen(buf_data);
    sprintf(buf, http_uploader_head, device_id, api_key, len);
    strcat(buf, buf_data);
    memset(recv_data_0, 0, RECV_LEN);
    /*1.连接服务器*/
    socket_id_0 = tos_sal_module_connect(ONENET_SERVER_IP, ONENET_SERVER_PORT, TOS_SAL_PROTO_TCP);

    if (socket_id_0 < 0)
    {
        printf("连接服务器失败===>id:%d\n", socket_id_0);
        return  -1;
    }

    /*2、发送JSON格式的数据流到服务器*/
    printf("即将发送的数据包:\n");
    printf("%s", buf);
    ret = tos_sal_module_send(socket_id_0, buf, strlen(buf));

    if(ret < 0)
    {
        printf("接收服务器数据错误!\n");
        return -2;
    }

    /*3、等待接收服务器发来的数据*/
    ret = tos_sal_module_recv_timeout(socket_id_0, recv_data_0, 203, 4000);

    if (ret < 0)
    {
        printf("接收服务器数据错误!\n");
        return -3 ;
    }

    printf("已接收完整的服务器数据:\n%s\n", recv_data_0);
    recv_data_0[ret] = 0;
    /*4、关闭TCP网络套接字*/
    tos_sal_module_close(socket_id_0);
    return 0 ;
}

void tcp_test0(void)
{
		float e53_sc1_lux = 0.0 ;
    while (1)
    {
				e53_sc1_lux = BH1750_Convert(); 
        OneNet_Packet_Send(ONENET_DEVICE_ID, ONENET_API_KEY, (int)e53_sc1_lux);
        tos_sleep_ms(1000);
    }
}

static void ml302_hardware_reset(void)
{
    HAL_GPIO_WritePin(ML302_RESET_GPIO_Port, ML302_RESET_Pin, GPIO_PIN_RESET);
}

static void ml302_power_off(void)
{
    HAL_GPIO_WritePin(ML302_ENABLE_GPIO_Port, ML302_ENABLE_Pin, GPIO_PIN_RESET);
}

static void ml302_power_on(void)
{
    HAL_GPIO_WritePin(ML302_ENABLE_GPIO_Port, ML302_ENABLE_Pin, GPIO_PIN_SET);
}

void MX_ML302_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(ML302_RESET_GPIO_Port, ML302_RESET_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(ML302_ENABLE_GPIO_Port, ML302_ENABLE_Pin, GPIO_PIN_RESET);
    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = ML302_RESET_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(ML302_RESET_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = ML302_ENABLE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(ML302_ENABLE_GPIO_Port, &GPIO_InitStruct);
}


extern float result_lx;
extern uint8_t BUF[2];
extern int result;

float BH1750_Convert(void)
{
	BH1750_Start();
	HAL_Delay(180);
	HAL_I2C_Master_Receive(&hi2c1, BH1750_Addr+1,BUF,2,0xff); 
	result=BUF[0];
	result=(result<<8)+BUF[1];  //合成数据，即光照数据	
	result_lx=(float)(result/1.2);
  return result_lx;
}

void application_entry(void *arg)
{
		BH1750_Init();
		MX_ML302_GPIO_Init();
    ml302_power_off();
    osDelay(500);
    ml302_power_on();
    ml302_hardware_reset();
    ml302_sal_init(HAL_UART_PORT_0);
    osThreadCreate(osThread(tcp_test0), NULL);
    while(1)
    {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        osDelay(500);
    }
}

