#include "gd32f1x0.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"

void delay(int x)
{
    for (int i = 0;i<2000;i++)
        for (int j = 0 ; j<x;j++);
}

#define TASK1_STK_SIZE		512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 2, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

void board_init(void)
{
    systick_config();
    rcu_periph_clock_enable(RCU_GPIOF);
    /* configure LED1 GPIO port */ 
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}
void task1(void *arg)
{
    //int count = 1;
    while (1) {
        gpio_bit_write(GPIOF,GPIO_PIN_6,SET);
        gpio_bit_write(GPIOF,GPIO_PIN_7,RESET);
        //printf("###This is task1, %d\r\n", count++);
        osDelay(2000);

    }
}

void task2(void *arg)
{
    //int count = 1;
    while (1) {      
        //printf("***This is task2, %d\r\n", count++);
        gpio_bit_write(GPIOF,GPIO_PIN_7,SET); 
        gpio_bit_write(GPIOF,GPIO_PIN_6,RESET);
        osDelay(1000);
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    board_init();
    //printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
    osKernelStart();
}





