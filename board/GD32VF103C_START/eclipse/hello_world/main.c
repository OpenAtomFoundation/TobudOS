#include "gd32vf103.h"
#include "tos.h"
void delay_1ms(uint32_t count)
{
    uint64_t start_mtime, delta_mtime;

    /* don't start measuruing until we see an mtime tick */
    uint64_t tmp = get_timer_value();

    do{
        start_mtime = get_timer_value();
    }while(start_mtime == tmp);

    do{
        delta_mtime = get_timer_value() - start_mtime;
    }while(delta_mtime <(SystemCoreClock/4000.0 *count));
}

#define TASK_SIZE 1024
k_task_t k_task_task1;
k_task_t k_task_task2;
uint8_t k_task1_stk[TASK_SIZE];
uint8_t k_task2_stk[TASK_SIZE];

int share = 123;
void task1(void *pdata)
{
	int t1 = 0;
	while(1)
	{
		t1++;
		share++;
		//k_tick_t delay = tos_millisec2tick(10);
        gpio_bit_write(GPIOA, GPIO_PIN_7, (bit_status)(1-gpio_input_bit_get(GPIOA, GPIO_PIN_7)));
		tos_task_delay(1000);
		//tos_task_yield();
		//osDelay(10);
		//asm("wfi;");
	}
}

void task2(void *pdata)
{
	int t2 = 0;
	while(1)
	{
		t2--;
		share--;
		//tos_task_delay(10);
		tos_task_yield();
		asm("wfi;");
	}
}


void main(void) {
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure LED GPIO port */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    gpio_bit_reset(GPIOA, GPIO_PIN_7);
	tos_knl_init();

    eclic_irq_enable(CLIC_INT_TMR, 0, 0);

	tos_task_create(&k_task_task1, "task1", task1, NULL, 3, k_task1_stk, TASK_SIZE, 0);
	//tos_task_create(&k_task_task2, "task2", task2, NULL, 3, k_task2_stk, TASK_SIZE, 0);

    //eclic_irq_enable(CLIC_INT_TMR, 0, 0);
	tos_knl_start();


	int c = 0;
	while(1)
	{
		c++;
		asm("wfi;");
	}

    while(1){
        /* insert 500 ms delay */
        delay_1ms(1000);

        /* toggle the LED */
        gpio_bit_write(GPIOA, GPIO_PIN_7, (bit_status)(1-gpio_input_bit_get(GPIOA, GPIO_PIN_7)));

        /* insert 500 ms delay */
        delay_1ms(1000);
    }
}
