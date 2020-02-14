#include "esp8266.h"
#include "mcu_init.h"
#include "sal_module_wrapper.h"
#include "cmsis_os.h"

#define UDP_TEST_TASK0_STK_SIZE         4096
void udp_test0(void);
osThreadDef(udp_test0, osPriorityNormal, 1, UDP_TEST_TASK0_STK_SIZE);

#define UDP_TEST_TASK1_STK_SIZE         4096
void udp_test1(void);
osThreadDef(udp_test1, osPriorityNormal, 1, UDP_TEST_TASK1_STK_SIZE);

int socket_id_0 = -1;
int socket_id_1 = -1;

#define RECV_LEN            1024
uint8_t recv_data_0[RECV_LEN];
uint8_t recv_data_1[RECV_LEN];

void udp_test0(void)
{
	int recv_len = -1;

	while (1) {
		tos_sal_module_sendto(socket_id_0, "47.107.78.107", "1234",
                                (const void*)"This is UDP Test!\r\n", strlen("This is UDP Test!\r\n"));

		recv_len = tos_sal_module_recvfrom(socket_id_0, recv_data_0, sizeof(recv_data_0));
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

void udp_test1(void)
{
    int recv_len = -1;

    while (1) {
        tos_sal_module_sendto(socket_id_1, "47.107.78.107", "4321",
                                (const void *)"Hello Tencent!\r\n", strlen("Hello Tencent!\r\n"));

        recv_len = tos_sal_module_recvfrom(socket_id_1, recv_data_1, sizeof(recv_data_1));
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

#define USE_ESP8266

void application_entry(void *arg)
{
#ifdef USE_ESP8266
    esp8266_sal_init(HAL_UART_PORT_0);
    esp8266_join_ap("SheldonDai", "srnr6x9xbhmb0");
#endif

#ifdef USE_SIM800A
    sim800a_power_on();
    sim800a_sal_init(HAL_UART_PORT_2);
#endif

    socket_id_0 = tos_sal_module_connect("47.107.78.107", "1234", TOS_SAL_PROTO_UDP);
    if (socket_id_0 == -1) {
        printf("UDP0 connect failed\r\n");
    } else {
        printf("UDP0 connect success! fd: %d\n", socket_id_0);
    }

    socket_id_1 = tos_sal_module_connect("47.107.78.107", "4321", TOS_SAL_PROTO_UDP);
    if (socket_id_1 == -1) {
        printf("UDP1 connect failed\r\n");
    } else {
        printf("UDP1 connect success! fd: %d\n", socket_id_1);
    }

    osThreadCreate(osThread(udp_test0), NULL);
    osThreadCreate(osThread(udp_test1), NULL);
}

