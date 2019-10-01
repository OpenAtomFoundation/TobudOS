#include "gd32vf103.h"
#include "usart.h"
void usart0_init(int baud)
{
    uint32_t com = USART0;

    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */
    gpio_init(USART0_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, USART0_GPIO_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_init(USART0_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USART0_GPIO_RX_PIN);

    /* USART configure */
    usart_deinit(com);
    usart_baudrate_set(com, baud);
    usart_word_length_set(com, USART_WL_8BIT);
    usart_stop_bit_set(com, USART_STB_1BIT);
    usart_parity_config(com, USART_PM_NONE);
    usart_hardware_flow_rts_config(com, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(com, USART_CTS_DISABLE);
    usart_receive_config(com, USART_RECEIVE_ENABLE);
    usart_transmit_config(com, USART_TRANSMIT_ENABLE);
    usart_enable(com);
}
