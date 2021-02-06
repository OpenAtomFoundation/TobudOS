#include "gd32e50x.h"
#include "gd32e507z_eval.h"
#include "systick.h"
#include <stdio.h>
#include "cmsis_os.h"

#define TASK1_STK_SIZE		512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 2, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

void board_init(void)
{
    /* initialize the leds */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
     /* close all of leds */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
    /* configure systick */
    systick_config();
    /* configure EVAL_COM0 */
    gd_eval_com_init(EVAL_COM0);
}


void task1(void *arg)
{
    while (1) {
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);
        printf("++++++++This is task1!\r\n");
        osDelay(2000);
    }
}

void task2(void *arg)
{
    while (1) {      
        gd_eval_led_toggle(LED3);
        gd_eval_led_toggle(LED4);
        printf("---------This is task2!\r\n");
        osDelay(1000);
    }
}

int main(void)
{
    board_init();
    printf("Welcome to TencentOS Tiny!\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
    osKernelStart();
}


/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
