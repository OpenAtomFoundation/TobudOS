#include "gd32e10x.h"
#include "gd32e103v_eval.h"
#include "gd32e103v_lcd_eval.h"
#include "systick.h"
#include <stdio.h>
#include "cmsis_os.h"

#define TASK1_STK_SIZE		512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 2, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

char_format_struct char_format;
uint16_t a1=20;
uint16_t b1=10;

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
/*!
    \brief      initialize the LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_init(void)
{
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    gd_eval_led_init(LED5);
}


/*!
    \brief      board_init function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void board_init(void)
{
    uint16_t i;
    uint8_t led_string[14]="TencentOS tiny";
    /* initialize the LEDs */
    led_init();
    
    /* configure systick */
    systick_config();
    
    /* configure EVAL_COM0 */
    gd_eval_com_init(EVAL_COM0);
    
    /* configure TAMPER key */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    gd_eval_lcd_init();
    lcd_clear(WHITE);  
    /* configure char format */
    char_format.char_color = BLUE;
    char_format.bk_color = WHITE;
    char_format.direction = CHAR_DIRECTION_VERTICAL;
    char_format.font = CHAR_FONT_8_16;
    for (i = 0; i < 14; i++){
        lcd_char_display((a1+35+8*i), b1+20, *(led_string+i), char_format);
    }
    
}
void task1(void *arg)
{
    int count = 1;
    while (1) {
        printf("###This is task1, %d\r\n", count++);
        gd_eval_led_toggle(LED2);
        gd_eval_led_toggle(LED3);
        osDelay(2000);

    }
}

void task2(void *arg)
{
    int count = 1;
    while (1) {      
        printf("***This is task2, %d\r\n", count++);
        gd_eval_led_toggle(LED4);
        gd_eval_led_toggle(LED5);
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
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
    osKernelStart();
}
