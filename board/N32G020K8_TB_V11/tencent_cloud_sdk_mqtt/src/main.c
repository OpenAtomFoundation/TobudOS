#include "n32g020xx.h"
#include "n32g020xx_common.h"
#include "n32g020xx_log.h"
#include <string.h>
#include "tos_at.h"
#include "cmsis_os.h"
#include "sal_module_wrapper.h"
#include "mqtt_wrapper.h"

#define APPLICATION_TASK_STK_SIZE           512
extern void application_entry(void *pdata);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

__weak void application_entry(void *arg)
{
    extern int bc26_sal_init(hal_uart_port_t uart_port);
    extern void mqtt_basic_thread(void);

    bc26_sal_init(HAL_UART_PORT_0);
    mqtt_basic_thread();

    while (1) {
        printf("This is a demo task,please use your task entry!\r\n");
        tos_task_delay(1000);
    }
}

int main(void)
{
    log_init();
    log("Sys StartUp.\r\n");
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(application_entry), NULL);
    osKernelStart();
}

