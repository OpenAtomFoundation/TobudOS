#include "sim800a.h"
#include "mcu_init.h"
#include "sal_module_wrapper.h"

#define MAIN_TASK_STK_SIZE              4096
void main_task(void *pdata);
osThreadDef(main_task, osPriorityNormal, 1, MAIN_TASK_STK_SIZE);

#define TCP_TEST_TASK1_STK_SIZE         4096
void tcp_test1(void);
osThreadDef(tcp_test1, osPriorityNormal, 1, TCP_TEST_TASK1_STK_SIZE);

#define TCP_TEST_TASK2_STK_SIZE         4096
void tcp_test2(void);
osThreadDef(tcp_test2, osPriorityNormal, 1, TCP_TEST_TASK2_STK_SIZE);

#define RECV_LEN  1024

char *data_send = "Welcome to TCP Test!\r\n";
char *data_send2 = "Hello Tencent!\r\n";

int socket_id = -1;
int socket_id2 = -1;

unsigned char recv_data1[RECV_LEN];
unsigned char recv_data2[RECV_LEN];

void tcp_test1(void)
{
	int ret = -1;

	while (1) {
		tos_sal_module_send(socket_id, (const unsigned char*)data_send, strlen(data_send));

		ret = tos_sal_module_recv_timeout(socket_id, recv_data1, sizeof(recv_data1), 8000);
		if (ret < 0) {
			printf("recv error\n\r");
		} else {
			recv_data1[ret] = 0;
			printf("recv:ret = %d (%s)\n\r", ret, recv_data1);
			memset(recv_data1, 0, strlen((const char *)recv_data1));
		}

		tos_sleep_ms(2000);
	}
}

void tcp_test2(void)
{
    int ret = -1;

    while (1) {
        tos_sal_module_send(socket_id2, (const unsigned char*)data_send2, strlen(data_send2));

        ret = tos_sal_module_recv_timeout(socket_id2, recv_data2, sizeof(recv_data2), 8000);
		if (ret < 0) {
			printf("recv2 error\n\r");
		} else {
			recv_data2[ret] = 0;
			printf("recv2:ret = %d (%s)\n\r", ret, recv_data2);
			memset(recv_data2,0,strlen((const char *)recv_data2));
		}
        tos_sleep_ms(2000);
    }
}

void main_task(void *arg)
{
    sim800a_power_on();
    sim800a_sal_init(HAL_UART_PORT_2);

    socket_id = tos_sal_module_connect("39.108.190.129", "8080", TOS_SAL_PROTO_TCP);
    if (socket_id == -1) {
        printf("TCP0 connect failed\r\n");
    } else {
        printf("TCP0 connect success! fd: %d\n",socket_id);
    }

    socket_id2 = tos_sal_module_connect("39.108.190.129", "8001", TOS_SAL_PROTO_TCP);
    if (socket_id2 == -1) {
        printf("TCP1 connect failed\r\n");
    } else {
        printf("TCP1 connect success! fd: %d\n",socket_id2);
    }

    osThreadCreate(osThread(tcp_test1), NULL);
    osThreadCreate(osThread(tcp_test2), NULL);
}

int main(void)
{
    board_init();
    printf("welcome to Tencent TOS Tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(main_task), NULL);
    osKernelStart();
}

