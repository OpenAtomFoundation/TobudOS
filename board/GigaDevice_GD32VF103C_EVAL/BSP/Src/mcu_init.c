#include "mcu_init.h"

void board_init() {

    SystemInit();

    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOE);

    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_2);
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);

    gpio_bit_reset(GPIOC, GPIO_PIN_0 | GPIO_PIN_2);
    gpio_bit_reset(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
}
