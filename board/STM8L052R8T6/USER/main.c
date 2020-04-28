#include "tos_k.h"

#include "stm8l15x.h"
#include "uart.h"
#include "rtc.h"
#include "tim.h"

// this is a stm8 rtc demo UART1 print,the baud rate is 57600.
// LED GPIOB0

// Init LED IO
void LED_GPIO_Init(void)
{

    GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow);
}

// set the GPIOB0 Pin to high
void LED_On(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

// set the GPIOB0 Pin to low
void LED_Off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

// About 1 second, not exactly, just for demonstration purposes
void Delay(unsigned int time)
{
    unsigned int i;
    while (time--) {
        for (i = 300; i > 0; --i) {
            asm("nop");
        }
    }
}

void disp_rtc(void)
{
    RTC_Get_Time();
    UART1_Send_String("Current Time:");
    UART1_Send_Dec(clock.cYear, 2); /* YY-MM-DD */
    UART1_Send_String("-");
    UART1_Send_Dec(clock.cMonth, 2);
    UART1_Send_String("-");
    UART1_Send_Dec(clock.cDay, 2);

    UART1_Send_String(" "); /* HH:MM:SS */
    UART1_Send_Dec(clock.cHour, 2);
    UART1_Send_String(":");
    UART1_Send_Dec(clock.cMinute, 2);
    UART1_Send_String(":");
    UART1_Send_Dec(clock.cSecond, 2);
    UART1_Send_String(" \r\n");
}

void task1_entry(void *arg)
{
    while (1) {
        UART1_Send_String("task1\r\n");
        tos_task_delay(200);
    }
}

void task2_entry(void *arg)
{
    while (1) {
        UART1_Send_String("task2\r\n");
        tos_task_delay(300);
    }
}

k_task_t task1;
k_stack_t task1_stack[512];

k_task_t task2;
k_stack_t task2_stack[512];

#define APPLICATION_TASK_STK_SIZE       1024

k_task_t task_app;
k_stack_t task_app_stack[512];

__WEAK__ void application_entry(void * arg)
{
    tos_task_create(&task1, "task1", task1_entry, NULL,
                        4,
                        task1_stack, sizeof(task1_stack),
                        0);

    tos_task_create(&task2, "task2", task2_entry, NULL,
                        4,
                        task2_stack, sizeof(task2_stack),
                        0);
}

int main(void)
{
    LED_GPIO_Init();
    UART1_Init(9600); /* Init the UART1, baud rate 9600 */
    RTC_Setting_Init(); /* Init RTC */

    /* use as systick, interrupt handler see TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler */
    Timer2_Init(TOS_CFG_CPU_CLOCK, TOS_CFG_CPU_TICK_PER_SECOND);

    LED_On();
    UART1_Send_String("welcome to TencentOS tiny!\r\n");
    printf("welcome to TencentOS tiny!\r\n");

    tos_knl_init();

    tos_task_create(&task_app, "app", application_entry, NULL,
                        4,
                        task_app_stack, sizeof(task_app_stack),
                        0);

    tos_knl_start();

    return 0;
}

