#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "bsp_led.h"   
#include "cmsis_os.h"


#define TASK1_STK_SIZE          512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE          512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);


void task1(void *arg)
{
	int count = 0;
	while(1)
	{
		PRINTF("********This is Task 1, count is %d \r\n",count++);
		/* LED亮 */
		CORE_BOARD_LED_ON;

		/* 整体操作红色 */
		RGB_LED_COLOR_RED;  
		osDelay(1000);
	}
}


void task2(void *arg)
{
	int count = 0;
	while(1)
	{
		/* 整体操作绿色 */
		RGB_LED_COLOR_GREEN;
		PRINTF("++++++++This is Task 2, count is %d \r\n",count++);
		osDelay(2000);
	}
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    /* 初始化内存保护单元 */
    BOARD_ConfigMPU();
    /* 初始化开发板引脚 */
    BOARD_InitPins();
    /* 初始化开发板时钟 */
    BOARD_BootClockRUN();
    /* 初始化调试串口 */
    BOARD_InitDebugConsole();
    /* 打印系统时钟 */
    PRINTF("\r\n");
    PRINTF("*****欢迎使用i.MX RT1052 开发板*****\r\n");
    PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
    PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
    PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
    PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));  
  
    PRINTF("GPIO输出-使用固件库点亮LED\r\n");
  
    /* 初始化LED引脚 */
    LED_GPIO_Config();  
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize(); // TencentOS Tiny kernel initialize
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
    osKernelStart(); // Start TencentOS Tiny
}
/****************************END OF FILE**********************/
