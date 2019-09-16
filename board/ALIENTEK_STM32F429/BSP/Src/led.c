#include "led.h"

void led0_toggle(void)
{
    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
}
void led1_toggle(void)
{
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}
