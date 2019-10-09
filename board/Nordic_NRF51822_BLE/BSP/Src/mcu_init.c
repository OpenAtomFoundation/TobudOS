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

_ttywrch(int ch)
{
	ch = ch;
}

void board_init(void)
{
    nrf_gpio_range_cfg_output(LED_START,LED_STOP);
    simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);
    nrf_rtc_init();
}

