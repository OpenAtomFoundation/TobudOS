#include "mcu_init.h"
#include "tos_k.h"
#include "lcd.h"

#define LCD_TASK_SIZE 1024
k_task_t lcd_handle;
uint8_t lcd_stk[LCD_TASK_SIZE];


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

void nrf24l01_init();

void main(void) {
    board_init();

    usart0_init(115200);

    tos_knl_init();

    nrf24l01_init();

    tos_task_create(&lcd_handle,     "lcd", task_lcd, NULL, 7,   lcd_stk,   LCD_TASK_SIZE, 0);

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
