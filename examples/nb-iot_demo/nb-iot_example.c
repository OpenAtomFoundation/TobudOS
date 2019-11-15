#include "bc35_28_95_lwm2m.h"
#include "mcu_init.h"
#include "sal_module_wrapper.h"
#include "cmsis_os.h"


#define NB_IoT_TEST_TASK_STK_SIZE         1024
void nb_iot_demo(void);
osThreadDef(nb_iot_demo, osPriorityNormal, 1, NB_IoT_TEST_TASK_STK_SIZE);

int socket_id=-1;
#define RECV_LEN            1024
uint8_t recv_data[RECV_LEN];

void nb_iot_demo(void)
{
    int recv_len = -1;
    int bc35_28_95_sal_lwm2m_init(hal_uart_port_t uart_port);
    bc35_28_95_sal_lwm2m_init(HAL_UART_PORT_0);
    socket_id = tos_sal_module_connect("49.4.85.232","5683",TOS_SAL_PROTO_UDP);
    if(socket_id !=-1){
        printf("connect to cloud success! fd: %d\n", socket_id);	
    }
	else{
        printf("connect to cloud failed!\n");
	}
	while (1) {
		tos_sal_module_send(socket_id, (const void*)"22", strlen("22"));

		recv_len = tos_sal_module_recv(socket_id, recv_data, sizeof(recv_data));
		if (recv_len < 0) {
			printf("task receive error\n");
		} else if (recv_len == 0) {
		    printf("task receive none\n");
		} else {
			recv_data[recv_len] = 0;
			printf("task: receive len: %d\nmsg from remote: %s\n", recv_len, recv_data);
		}

		tos_sleep_ms(2000);
	}
}

void application_entry(void *arg)
{
    osThreadCreate(osThread(nb_iot_demo), NULL);
}
