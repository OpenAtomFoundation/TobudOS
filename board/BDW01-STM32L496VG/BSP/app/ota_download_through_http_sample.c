#include "m5313.h"
#include "mcu_init.h"
#include "sal_module_wrapper.h"
#include "cmsis_os.h"

#include "ota_download.h"

extern ota_flash_drv_t stm32l4_norflash_onchip_drv_ota;
extern ota_flash_prop_t stm32l4_norflash_onchip_prop_ota;

k_sem_t sem;

#define TASK1_STK_SIZE          1024
void user_task(void *arg);
osThreadDef(user_task, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE          1024
void ota_download_task(void *arg);
osThreadDef(ota_download_task, osPriorityNormal, 1, TASK2_STK_SIZE);

void user_task(void *arg)
{
    int iter = 0;

    while (K_TRUE) {
        osDelay(2000);

        printf("do sth(v0.1)...\n");

        if (++iter == 2) {
            printf("trigger ota download\n");
			tos_sem_post(&sem);
        }
    }
}

void ota_download_task(void *arg)
{
    tos_sem_pend(&sem, TOS_TIME_FOREVER);

	m5313_sal_init(HAL_UART_PORT_2);

    uint32_t partition_addr = 0x080ff800;

    if (ota_env_init(OTA_UPDATE_IN_POSITION, partition_addr, &stm32l4_norflash_onchip_drv_ota, &stm32l4_norflash_onchip_prop_ota) < 0) {
        printf("env init failed!\n");
        return;
    }

    if (!ota_download_http("http://119.139.199.196:2899/patch.bin")) {
        printf("download successfully!\n");
    } else {
        printf("download failed!\n");
    }
}

void application_entry(void *arg)
{
    tos_sem_create(&sem, 0);

    osThreadCreate(osThread(user_task), NULL); // Create task1
    osThreadCreate(osThread(ota_download_task), NULL); // Create task2
}

