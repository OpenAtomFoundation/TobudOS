#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_rtc.h"
#include "simple_uart.h"
#include "main.h"

/* for printf */

#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f)
{
    simple_uart_put ((uint8_t) ch);
    return ch;
}

void _ttywrch(int ch)
{
	ch = ch;
}

void SysTick_Handler(void)
{
    tos_knl_irq_enter();
    tos_tick_handler ();
    tos_knl_irq_leave();
}

/** @brief Function starting the internal LFCLK XTAL oscillator.
 */
static void lfclk_config(void)
{
    NRF_CLOCK->LFCLKSRC             = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED  = 0;
    NRF_CLOCK->TASKS_LFCLKSTART     = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
        ;
    }

    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}

void board_init(void)
{
    lfclk_config();

    nrf_gpio_range_cfg_output(LED_START, LED_STOP);
    simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);
}

