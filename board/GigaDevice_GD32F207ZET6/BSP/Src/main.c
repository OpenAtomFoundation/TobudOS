#include "gd32f20x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f20x_eval.h"
#include "cmsis_os.h"

#define APPLICATION_TASK_STK_SIZE       256
extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

#define TASK1_STK_SIZE		128
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		128
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);


/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));

    return ch;
}

void board_init(void)
{
    /* configure systick */
    systick_config();
    /* initilize the LEDs, USART and key */
    gd_eval_led_init(LED2); 
    gd_eval_led_init(LED3); 
    gd_eval_led_init(LED4);
    gd_eval_com_init(EVAL_COM0);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    printf("\r\nCK_AHB is %d", rcu_clock_freq_get(CK_AHB));
    printf("\r\nCK_APB1 is %d", rcu_clock_freq_get(CK_APB1));
    printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));
//    while(1){
//        if(RESET == gd_eval_key_state_get(KEY_WAKEUP)){
//            gd_eval_led_on(LED3);
//            delay_1ms(500);
//            gd_eval_led_off(LED3);
//            gd_eval_led_toggle(LED4);
//        }
////        gd_eval_led_toggle(LED2);
////        gd_eval_led_toggle(LED3);
////        gd_eval_led_toggle(LED4);
////        delay_1ms(500);
//    }
}

void task1(void *arg)
{
    int count = 1;
    while (1) {
        gd_eval_led_toggle(LED2);
        printf("###This is task1, %d\r\n", count++);
        osDelay(2000);
    }
}

void task2(void *arg)
{
    int count = 1;
    while (1) {      
        printf("***This is task2, %d\r\n", count++);
        gd_eval_led_toggle(LED3);
        gd_eval_led_toggle(LED4);
        osDelay(1000);
    }
}

void application_entry(void *arg)
{
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
}


int main(void)
{
    board_init();
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(application_entry), NULL);
    osKernelStart();
}
