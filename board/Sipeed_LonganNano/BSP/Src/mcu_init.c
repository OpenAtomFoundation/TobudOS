#include "mcu_init.h"
#include "lcd.h"
void board_init() {

    SystemInit();

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(LEDR_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LEDR_PIN);
    gpio_init(LEDG_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LEDG_PIN);
    gpio_init(LEDB_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LEDB_PIN);

    gpio_bit_set(LEDR_GPIO_PORT, LEDR_PIN);
    gpio_bit_set(LEDG_GPIO_PORT, LEDG_PIN);
    gpio_bit_set(LEDB_GPIO_PORT, LEDB_PIN);

    LCD_Init();         // init LCD
    LCD_Clear(BLACK);
}
