#include "mcu_init.h"
#include "tos_k.h"


void nrf24l01_init();

void main(void) {
    board_init();

    usart0_init(115200);

    tos_knl_init();

    nrf24l01_init();

    tos_knl_start();

die:
    while (1) {
        asm("wfi;");
    }
}


int _put_char(int ch)
{
    usart_data_transmit(USART0, (uint8_t) ch );
    while (usart_flag_get(USART0, USART_FLAG_TBE)== RESET){
    }

    return ch;
}
