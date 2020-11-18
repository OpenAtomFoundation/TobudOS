#include "mcu_init.h"
#include "sal_module_wrapper.h"
#include "cmsis_os.h"

//#define USE_ESP8266
//#define USE_SIM800A
//#define USE_BC26
//#define USE_EC20
//#define USE_EC200S
#define USE_M5313

#ifdef USE_ESP8266
#include "esp8266.h"
#endif

#ifdef USE_SIM800A
#include "sim800a.h"
#endif

#ifdef USE_BC26
#include "bc26.h"
#endif

#ifdef USE_EC20
#include "ec20.h"
#endif

#ifdef USE_EC200S
#include "ec200s.h"
#endif

#ifdef USE_M5313
#include "m5313.h"
#endif

#define SERVER_TCP_IP		"119.139.199.196"

#define TCP_TEST_TASK0_STK_SIZE         4096
void tcp_test0(void);
osThreadDef(tcp_test0, osPriorityNormal, 1, TCP_TEST_TASK0_STK_SIZE);

#define TCP_TEST_TASK1_STK_SIZE         4096
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
#if defined(USE_ESP8266)
    esp8266_sal_init(HAL_UART_PORT_0);
    esp8266_join_ap("SheldonDai", "srnr6x9xbhmb0");
#elif defined(USE_SIM800A)
    sim800a_power_on();
    sim800a_sal_init(HAL_UART_PORT_2);
#elif defined(USE_BC26)
    bc26_sal_init(HAL_UART_PORT_2);
#elif defined(USE_EC20)
    ec20_sal_init(HAL_UART_PORT_0);
#elif defined(USE_EC200S)
    ec200s_sal_init(HAL_UART_PORT_2);
#elif defined(USE_M5313)
	m5313_sal_init(HAL_UART_PORT_2);
#endif
    
    socket_id_0 = tos_sal_module_connect(SERVER_TCP_IP, "2898", TOS_SAL_PROTO_TCP); 
    if (socket_id_0 == -1) {
        printf("TCP0 connect failed\r\n");
    } else {
        printf("TCP0 connect success! fd: %d\n", socket_id_0);
    }

	socket_id_1 = tos_sal_module_connect(SERVER_TCP_IP, "2899", TOS_SAL_PROTO_TCP);
    if (socket_id_1 == -1) {
        printf("TCP1 connect failed\r\n");
    } else {
        printf("TCP1 connect success! fd: %d\n", socket_id_1);
    }
	

    osThreadCreate(osThread(tcp_test0), NULL);
	osThreadCreate(osThread(tcp_test1), NULL);
}

