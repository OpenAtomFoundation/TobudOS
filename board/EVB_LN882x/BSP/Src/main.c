#include "mcu_init.h"
#include "cmsis_os.h"
#include "osal/osal.h"

#define APPLICATION_TASK_STK_SIZE           (8*1024)
extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityLow, 1, APPLICATION_TASK_STK_SIZE);//osPriorityNormal

__weak void application_entry(void *arg)
{
    while (1) {
        printf("This is a demo task,please use your task entry!\r\n");
        tos_task_delay(1000);
    }
}

#include "wifi_manager/wifi_manager.h"
#include "utils/system_parameter.h"
#include "atcmd/at_task.h"
#include "ln_kv_port.h"
#include "ln_kv_api.h"
#include "flash_partition_table.h"

int main(void)
{
    board_init();

    osKernelInitialize();
    os_heap_mem_add_pool();  // after kernel(tos_knl_init) init!!!
    
    at_init();
	system_parameter_init();
    if (KV_ERR_NONE != ln_kv_port_init(KV_SPACE_OFFSET, (KV_SPACE_OFFSET + KV_SPACE_SIZE))) {
        LOG(LOG_LVL_ERROR, "KV init filed!\r\n");
    }

    wifi_init();
    lwip_tcpip_init();
    wifi_manager_init();

    osThreadCreate(osThread(application_entry), NULL); // Create TOS Tiny task
    osKernelStart(); // Start TOS Tiny
}

