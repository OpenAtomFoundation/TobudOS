/*

    提供 board_init 函数, 最终调用的是

        tos/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Inc/stm32g0xx_hal.h

    中提供的 HAL_Init() 函数

*/
#include "mcu_init.h"

/*

    位于 tos/platform/vendor_bsp/st/CMSIS/RTOS2/Template/cmsis_os.h

    提供 osKernelXXX 函数

*/
#include "cmsis_os.h"

#define APPLICATION_TASK_STK_SIZE       1024
extern void application_entry_rust();
osThreadDef(application_entry_rust, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

int main(void)
{
    board_init();
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(application_entry_rust), NULL);
    osKernelStart();
}
