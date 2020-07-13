#include "mcu_init.h"
#include "tos_k.h"
#include "lcd.h"

#define TASK_SIZE 2048

k_task_t task1_handle;
k_task_t task2_handle;
k_task_t lcd_handle;

uint8_t task1_stk[TASK_SIZE];
uint8_t task2_stk[TASK_SIZE];
uint8_t lcd_stk[TASK_SIZE/2];

int share = 0xCBA7F9;
k_sem_t sem;

typedef struct {
    int port;
    int pin;
} Led_t;

Led_t leds[] = {
        { LEDR_GPIO_PORT, LEDR_PIN },
        { LEDG_GPIO_PORT, LEDG_PIN },
        { LEDB_GPIO_PORT, LEDB_PIN }
};


void task1(void *arg)
{
    int task_cnt1 = 0;
    while (1) {
        printf("hello world from %s cnt: %d\n", __func__, task_cnt1++);
        tos_sem_pend(&sem, ~0U);

        for(int i=0; i<sizeof(leds)/sizeof(Led_t); i++) {
            int on = !(task_cnt1 & (1 << i));
            gpio_bit_write(leds[i].port, leds[i].pin, on);
        }
    }
}

void task2(void *arg)
{
    int task_cnt2 = 0;
    while (1) {
        share++;
        printf("hello world from %s cnt: %08x\n", __func__, task_cnt2--);
        tos_task_delay(100);
        tos_sem_post(&sem);
    }
}

uint16_t get_next_color() {
    static uint16_t color_table[] = { WHITE, RED, BLUE, GREEN, RED, CYAN, YELLOW, BLACK, GRAY };
    static uint8_t i = 0;
    uint16_t color = BLACK;
    i %= sizeof(color_table)/sizeof(uint16_t);
    color = color_table[i];
    i++;
    return color;
}

uint8_t xbase() {
	lcd_display_mode_t m = LCD_GetDisplayMode();
	if(m == LCD_DISPMODE_HORIZONTAL || m == LCD_DISPMODE_HORIZONTAL_MIRROR) {
		return 52;
	} else {
		return 0;
	}
}

uint8_t ybase() {
	lcd_display_mode_t m = LCD_GetDisplayMode();
	if(m == LCD_DISPMODE_HORIZONTAL || m == LCD_DISPMODE_HORIZONTAL_MIRROR) {
		return 0;
	} else {
		return 52;
	}
}

void task_lcd(void *arg)
{

    for(int i=0; ; i++) {
    	lcd_display_mode_t modes[] = { LCD_DISPMODE_HORIZONTAL, LCD_DISPMODE_VERTICAL , LCD_DISPMODE_HORIZONTAL_MIRROR, LCD_DISPMODE_VERTICAL_MIRROR };


        LCD_SetDisplayMode(modes[i % (sizeof(modes)/sizeof(lcd_display_mode_t))]);


        uint16_t color = get_next_color();


        LCD_Clear(color);

        color = get_next_color();
        LCD_ShowChinese(xbase()+37,       ybase()+43, 0, color);
        LCD_ShowChinese(xbase()+37+16+28, ybase()+43, 1, color);
        for(int j=0; j<6; j++) {
            LCD_ShowChinese(xbase()+10+j*20, ybase()+60+16, j+2, color);
        }

        tos_task_delay(1000);

        color = get_next_color();
        LCD_Clear(color);

        color = get_next_color();
        LCD_ShowString(xbase()+11, ybase()+60, "TencentOS tiny", color);

        tos_task_delay(1000);
    }
}

void main(void) {
    board_init();

    usart0_init(115200);

    tos_knl_init();


    tos_task_create(&task1_handle, "task1", task1,      NULL, 5, task1_stk, TASK_SIZE, 0);
    tos_task_create(&task2_handle, "task2", task2,      NULL, 5, task2_stk, TASK_SIZE, 0);
    tos_task_create(&lcd_handle,   "lcd",   task_lcd,   NULL, 4, lcd_stk,   TASK_SIZE, 0);

    k_err_t err = tos_sem_create(&sem, 1);
    if (err != K_ERR_NONE) {
        goto die;
    }

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
