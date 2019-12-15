#include "sim7600ce.h"
#include "mcu_init.h"
#include "sal_module_wrapper.h"
#include "cmsis_os.h"

#define TCP_TEST_TASK0_STK_SIZE         512
void tcp_test0(void);
osThreadDef(tcp_test0, osPriorityNormal, 1, TCP_TEST_TASK0_STK_SIZE);

#define TCP_TEST_TASK1_STK_SIZE         512
void tcp_test1(void);
osThreadDef(tcp_test1, osPriorityNormal, 1, TCP_TEST_TASK1_STK_SIZE);

int socket_id_0 = -1;
int socket_id_1 = -1;

#define RECV_LEN            1024
uint8_t recv_data_0[RECV_LEN];
uint8_t recv_data_1[RECV_LEN];

void tcp_test0(void)
{
	int recv_len = -1;

	while (1) {
		tos_sal_module_send(socket_id_0, (const void*)"This is TCP Test!\r\n", strlen("This is TCP Test!\r\n"));

		recv_len = tos_sal_module_recv_timeout(socket_id_0, recv_data_0, sizeof(recv_data_0), 8000);
		if (recv_len < 0) {
			printf("task0 receive error\n");
		} else if (recv_len == 0) {
		    printf("task0 receive none\n");
		} else {
			recv_data_0[recv_len] = 0;
			printf("task0: receive len: %d\nmsg from remote: %s\n", recv_len, recv_data_0);
		}

		tos_sleep_ms(2000);
	}
}

void tcp_test1(void)
{
    int recv_len = -1;

    while (1) {
        tos_sal_module_send(socket_id_1, (const void *)"Hello Tencent!\r\n", strlen("Hello Tencent!\r\n"));

        recv_len = tos_sal_module_recv_timeout(socket_id_1, recv_data_1, sizeof(recv_data_1), 8000);
		if (recv_len < 0) {
			printf("task1 receive error\n\r");
		} else if (recv_len == 0) {
		    printf("task1 receive none\n");
		} else {
			recv_data_1[recv_len] = 0;
			printf("task1: receive len: %d\nmsg from remote: %s\n", recv_len, recv_data_1);
		}

        tos_sleep_ms(2000);
    }
}


void application_entry(void *arg)
{
    sim7600ce_sal_init(HAL_UART_PORT_2);

    socket_id_0 = tos_sal_module_connect("39.108.190.129", "8080", TOS_SAL_PROTO_TCP);
    if (socket_id_0 == -1) {
        printf("TCP0 connect failed\r\n");
    } else {
        printf("TCP0 connect success! fd: %d\n", socket_id_0);
    }

//    socket_id_1 = tos_sal_module_connect("39.108.190.129", "8001", TOS_SAL_PROTO_TCP);
//    if (socket_id_1 == -1) {
//        printf("TCP1 connect failed\r\n");
//    } else {
//        printf("TCP1 connect success! fd: %d\n", socket_id_1);
//    }

    osThreadCreate(osThread(tcp_test0), NULL);
    //osThreadCreate(osThread(tcp_test1), NULL);
}

