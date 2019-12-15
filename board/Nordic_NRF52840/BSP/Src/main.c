#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_rtc.h"
#include "simple_uart.h"
#include "main.h"
#include "cmsis_os.h"

#define APPLICATION_TASK_STK_SIZE       1024
extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

#define TASK1_STK_SIZE		512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

void task1(void *arg)
{
    int count = 0;
    
    while (true)
    {
        nrf_gpio_pin_set(LED_0);
        nrf_gpio_pin_set(LED_1);
        nrf_delay_ms(300);
        nrf_gpio_pin_clear(LED_0);
        nrf_gpio_pin_clear(LED_1);
        nrf_delay_ms(300);
        printf("This is Task1++++++++++,count is [%d] \n", ++count);
        tos_task_delay(100);
    }
}

void task2(void *arg)
{
    int count = 0;

    while (true)
    {
        nrf_gpio_pin_set(LED_2);
        nrf_gpio_pin_set(LED_3);
        nrf_gpio_pin_set(LED_4);
        nrf_delay_ms(300);
        nrf_gpio_pin_clear(LED_2);
        nrf_gpio_pin_clear(LED_3);
        nrf_gpio_pin_clear(LED_4);
        nrf_delay_ms(300);
        printf("This is Task2----------,count is [%d] \n", ++count);
        tos_task_delay(100);
    }
}

void application_entry(void *arg)
{
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    board_init();
    printf ("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(application_entry), NULL);
    osKernelStart();
}

