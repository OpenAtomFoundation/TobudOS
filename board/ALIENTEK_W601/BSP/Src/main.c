#include "sys.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "dwt.h"
#include "cmsis_os.h"

#define APPLICATION_TASK_STK_SIZE           1024
extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);


void board_init(void)
{
    dwt_delay_init(80000000);
    uart_init(115200);		//串口初始化
    LED_Init();				//LED接口初始化
}

void OS_CPU_SysTickHandler(void)
{
  if (tos_knl_is_running())
  {
    tos_knl_irq_enter();
    tos_tick_handler();
    tos_knl_irq_leave();
  }
}

__weak void application_entry(void *arg)
{
    while (1) {
        printf("This is a demo task,please use your task entry!\r\n");
        tos_task_delay(1000);
    }
}


int main(void)
{
    board_init();
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize(); // TOS Tiny kernel initialize
    osThreadCreate(osThread(application_entry), NULL); // Create TOS Tiny task 
    osKernelStart(); // Start TOS Tiny
}

