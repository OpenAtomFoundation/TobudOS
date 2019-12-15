#include "gd32f30x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"

void delay(int x)
{
    for (int i = 0;i<2000;i++)
        for (int j = 0 ; j<x;j++);
}

#define TASK1_STK_SIZE		256
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 2, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		256
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);


void board_init(void)
{
    systick_config();
    rcu_periph_clock_enable(RCU_GPIOB);
    /* configure LED1 GPIO port */ 
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    GPIO_BC(GPIOB) = GPIO_PIN_14;
    GPIO_BC(GPIOB) = GPIO_PIN_15;
}

void task1(void *arg)
{
    //int count = 1;
    while (1) {
        gpio_bit_write(GPIOB,GPIO_PIN_14,SET);
        gpio_bit_write(GPIOB,GPIO_PIN_15,RESET);
        //printf("###This is task1, %d\r\n", count++);
        osDelay(2000);

    }
}

void task2(void *arg)
{
    //int count = 1;
    while (1) {      
        //printf("***This is task2, %d\r\n", count++);
        gpio_bit_write(GPIOB,GPIO_PIN_15,SET); 
        gpio_bit_write(GPIOB,GPIO_PIN_14,RESET);
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
