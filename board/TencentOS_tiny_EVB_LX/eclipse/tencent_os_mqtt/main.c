#include "mcu_init.h"
#include "tos_k.h"

#define TASK_SIZE 1024
k_task_t k_task_bled;
k_task_t k_task_wifi;
uint8_t k_bled_stk[TASK_SIZE];
uint8_t k_wifi_stk[TASK_SIZE];

void task_bled(void *pdata)
{
    int cnt = 0;
    while (1) {
        printf("blink led task cnt: %d\n", cnt++);
        gpio_bit_write(LED_GPIO_PORT, LED_PIN,cnt % 2 ? SET : RESET);
    	tos_task_delay(1000);
    }
}

extern void mqtt_demo();

void task_wifi(void *pdata)
{
    while(1) {
    	mqtt_demo();
    	tos_task_delay(500);
    }
}


void main(void) {
    board_init();

    tos_knl_init();

    tos_task_create(&k_task_bled, "bled", task_bled, NULL, 5, k_bled_stk, TASK_SIZE, 0);
    tos_task_create(&k_task_wifi, "wifi", task_wifi, NULL, 3, k_wifi_stk, TASK_SIZE, 0);

    tos_knl_start();

}
