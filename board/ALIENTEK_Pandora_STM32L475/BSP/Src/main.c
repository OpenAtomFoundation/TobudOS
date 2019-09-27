#include "mcu_init.h"
#include "cmsis_os.h"
#include "tiny_logo.h"

#define APPLICATION_TASK_STK_SIZE        4096
extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

__weak void application_entry(void *arg)
{
    while (1) {
        printf("This is a demo task, please use your task entry!\r\n");
        tos_task_delay(1000);
    }
}

#define STK_SIZE_TASK1      512
#define STK_SIZE_TASK2      512


k_stack_t stack_task1[STK_SIZE_TASK1];
k_stack_t stack_task2[STK_SIZE_TASK2];

k_task_t task_Task1;
k_task_t task_Task2;

void entry_task1(void *arg);
void entry_task2(void *arg);
/********************************************************/
//LCD刷屏时使用的颜色
int lcd_discolor[13]={  WHITE,  BLUE,   BRED,   GRED,   GBLUE,  RED,    MAGENTA,
                        GREEN,  CYAN,   YELLOW, BROWN,  BRRED,  GRAY };

/********************************************************/

int main(void)
{
    board_init();
    LCD_DisplayOn();
    LCD_Show_Image(10, 14, 220, 50, tiny_logo);

    BACK_COLOR = WHITE;
    POINT_COLOR = BLUE;
    LCD_ShowString(10,  64, 240, 24, 24, "Pandora STM32L475");

    POINT_COLOR = RED;
    LCD_ShowString(10, 96, 240, 16, 16, "QinYUN575" "@" __DATE__);

    printf("\r\n===========================================================\r\n");
    printf("\r\n");
    printf("\tWelcome to TencentOS tiny\r\n");
    printf("\tCompile Time:%s, %s\r\n", __TIME__, __DATE__);
    printf("\tCoding by QinYUN575\r\n");
    printf("\r\n");
    printf("===========================================================\r\n");

    osKernelInitialize();
    osThreadCreate(osThread(application_entry), NULL);
    (void)tos_task_create(&task_Task1, "task_Task1", entry_task1, (void *)NULL,
                        5, stack_task1, STK_SIZE_TASK1, 0);
    (void)tos_task_create(&task_Task2, "task_Task2", entry_task2, (void *)NULL,
                        5, stack_task2, STK_SIZE_TASK2, 0);
    
    osKernelStart();
}


/**
* Taks1_task 任务函数
*
*/
void entry_task1(void *arg)
{
    uint8_t task1_count = 0;
    POINT_COLOR = BLUE;
    LCD_ShowString(10, 120, 215, 12, 12, "Task1 Run:");
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(10, 140, 90, 220);
    
   while (1)
   {
       LCD_Fill(11, 141, 89, 219, lcd_discolor[task1_count++%13]);
       LCD_ShowNum(75, 120, task1_count, 3, 12);
       HAL_GPIO_TogglePin(GPIOE, LED_R_Pin);
       tos_task_delay(800);
   }
}


/**
* Taks2_task 任务函数
*
*/
void entry_task2(void *arg)
{
   uint8_t task2_count = 0;

   POINT_COLOR = BLUE;
   LCD_ShowString(135, 120, 215, 12, 12, "Task2 Run:");
   POINT_COLOR = BLACK;
   LCD_DrawRectangle(135, 140, 215, 220);
   while (1)
   {
       LCD_Fill(136, 141, 214, 219, lcd_discolor[task2_count++%13]);
       LCD_ShowNum(195, 120, task2_count, 3, 12);
       HAL_GPIO_TogglePin(GPIOE, LED_B_Pin);
       tos_task_delay(500);
   }
}
