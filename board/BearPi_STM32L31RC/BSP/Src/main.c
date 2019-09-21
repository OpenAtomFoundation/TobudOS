#include "mcu_init.h"
#include "cmsis_os.h"

#define APPLICATION_TASK_STK_SIZE       4096

extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

__weak void application_entry(void *arg)
{
	char bh1750_dat_buf[12] = "LUX:00000.00";

	
    while (1) {
        //printf("This is a demo task,please use your task entry!\r\n");
	
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			sprintf(bh1750_dat_buf,"LUX:%.2f", Convert_BH1750());
		LCD_Fill(0,55,240,55+24,WHITE);
		LCD_ShowCharStr(0,55,bh1750_dat_buf,WHITE,BLUE,24);
			
		
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

