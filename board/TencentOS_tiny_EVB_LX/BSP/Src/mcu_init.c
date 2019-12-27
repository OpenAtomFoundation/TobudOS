#include "mcu_init.h"

static void led_config(void)
{
	rcu_periph_clock_enable(LED_GPIO_CLK);


	/* configure LED GPIO port */
	gpio_init(LED_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_PIN);
	/* reset LED GPIO pin */
	gpio_bit_set(LED_GPIO_PORT, LED_PIN);
}

void board_init()
{
	char *str = "Tencent";
    SystemInit();
    led_config();
    usart0_init(115200);
    usart1_init(115200);
    usart2_init(115200);
    OLED_Init();
    OLED_Clear();
    OLED_ShowChinese(36,0,0);
    OLED_ShowChinese(54,0,1);
    OLED_ShowChinese(72,0,2);
    OLED_ShowString(36,2,(uint8_t*)str,8);
}
