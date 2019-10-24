#include "mcu_init.h"

uint16_t a1=20;
uint16_t b1=10;


char_format_struct char_format;


void board_init() {

    uint16_t i;
    uint8_t led_string[14]="TencentOS tiny";
    SystemInit();

    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOE);

    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_2);
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);

    gpio_bit_reset(GPIOC, GPIO_PIN_0 | GPIO_PIN_2);
    gpio_bit_reset(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);

    exmc_lcd_init();
    touch_panel_gpio_configure();

    lcd_init();

    /* clear the LCD screen */
    lcd_clear(LCD_COLOR_WHITE);

    /* draw the picture of Gigadevice logo */
    /* if you don't want to draw the picture, you should modify the macro on
       the line 422th of picture.c file and comment the next line */
    lcd_picture_draw(0,100,240-1,100+55-1,(uint16_t *)(gImage_logo + BMP_HEADSIZE));

    lcd_picture_draw(60,210,60+120-1,210+95-1,(uint16_t *)(picture + BMP_HEADSIZE));

    /* draw a rectangle */
    //lcd_rectangle_draw(10,10,230,310,LCD_COLOR_BLUE);

    /* configure char format */
    char_format.char_color = LCD_COLOR_BLUE;
    char_format.bk_color = LCD_COLOR_WHITE;
    char_format.direction = CHAR_DIRECTION_VERTICAL;
    char_format.font = CHAR_FONT_8_16;

    /* draw character on LCD screen */
    for (i = 0; i < 14; i++){
        lcd_char_display((a1+35+8*i), b1+20, *(led_string+i), char_format);
    }
}
