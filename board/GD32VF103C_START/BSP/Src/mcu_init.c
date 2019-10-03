#include "mcu_init.h"

void board_init() {

    SystemInit();

    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    gpio_bit_reset(GPIOA, GPIO_PIN_7);
}
