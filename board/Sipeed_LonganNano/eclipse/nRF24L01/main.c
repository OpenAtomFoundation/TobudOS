#include "mcu_init.h"
#include "tos_k.h"
#include "lcd.h"
#include "nrf24.h"

#if 0
#define TASK_SIZE 1024
k_task_t task2_handle
uint8_t task2_stk[TASK_SIZE*1];


#define LCD_TASK_SIZE 1024
k_task_t lcd_handle;
uint8_t lcd_stk[LCD_TASK_SIZE];
#endif


#define LED_TASK_SIZE 1024
k_task_t led_handle;
uint8_t led_stk[LED_TASK_SIZE];

k_sem_t sem_led;

typedef struct {
    int port;
    int pin;
} Led_t;

Led_t leds[] = {
        { LEDR_GPIO_PORT, LEDR_PIN },
        { LEDG_GPIO_PORT, LEDG_PIN },
        { LEDB_GPIO_PORT, LEDB_PIN }
};


void task_led(void *arg)
{
    int task_cnt1 = 0;
    while (1) {
        printf("hello world from %s cnt: %d\n", __func__, task_cnt1++);

        tos_sem_pend(&sem_led, ~0);

        gpio_bit_reset(LEDR_GPIO_PORT, LEDR_PIN);
        tos_task_delay(50);
        gpio_bit_set(LEDR_GPIO_PORT, LEDR_PIN);
    }
}

void task2(void *arg)
{
    int task_cnt2 = 0;

    while (1) {
        //printf("hello world from %s cnt: %08x\n", __func__, task_cnt2--);
        tos_task_delay(200);
    }
}

void task_lcd(void *arg)
{
    uint16_t color_table[] = { WHITE, BLUE, RED, GREEN, CYAN, YELLOW, GRAY};

    for(int i=0; ; i++) {
        LCD_SetDisplayMode((i % 2) ? LCD_DISPMODE_HORIZONTAL_MIRROR : LCD_DISPMODE_HORIZONTAL);
        LCD_Clear(BLACK);

        uint16_t color = color_table[i % (sizeof(color_table)/sizeof(uint16_t))];


        LCD_ShowChinese(50,       16, 0, color);
        LCD_ShowChinese(50+16+28, 16, 1, color);

        for(int j=0; j<6; j++) {
            LCD_ShowChinese(22+j*20, 48, j+2, color);
        }

        tos_task_delay(1000);
        LCD_Clear(BLACK);

        LCD_ShowString(24, 32, "TencentOS tiny", color);

        tos_task_delay(1000);
    }
}

void main(void) {
    board_init();

    //usart0_init(115200);

    tos_knl_init();


    nrf24l01_init();


    tos_sem_create(&sem_led, 1);
    tos_task_create(&led_handle,      "led", task_led,    NULL, 6, led_stk, LED_TASK_SIZE, 0);
    //tos_task_create(&task2_handle, "task2", task2,      NULL, 3, task2_stk, TASK_SIZE*1, 0);
    //tos_task_create(&lcd_handle,   "lcd",   task_lcd,   NULL, 2, lcd_stk,     TASK_SIZE, 0);

    tos_knl_start();

die:
    while (1) {
        asm("wfi;");
    }
}


int _put_char(int ch)
{
    usart_data_transmit(USART0, (uint8_t) ch );
    while (usart_flag_get(USART0, USART_FLAG_TBE)== RESET){
    }

    return ch;
}
