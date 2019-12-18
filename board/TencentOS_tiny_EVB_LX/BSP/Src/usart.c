#include "gd32vf103.h"
#include "usart.h"
void usart0_init(int baud)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART0 clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USART0_Tx */
    gpio_init(USART0_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, USART0_GPIO_TX_PIN);

    /* connect port to USART0_Rx */
    gpio_init(USART0_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USART0_GPIO_RX_PIN);

    /* USART0 configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, baud);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
}

void usart1_init(int baud)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART1 clock */
    rcu_periph_clock_enable(RCU_USART1);

    /* connect port to USART0_Tx */
    gpio_init(USART1_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, USART1_GPIO_TX_PIN);

    /* connect port to USART0_Rx */
    gpio_init(USART1_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USART1_GPIO_RX_PIN);

    /* USART1 configure */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, baud);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_enable(USART1);
}



void usart2_init(int baud)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);

    /* enable USART2 clock */
    rcu_periph_clock_enable(RCU_USART2);

    /* connect port to USART0_Tx */
    gpio_init(USART2_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, USART2_GPIO_TX_PIN);

    /* connect port to USART0_Rx */
    gpio_init(USART2_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USART2_GPIO_RX_PIN);

    gpio_pin_remap_config(GPIO_USART2_FULL_REMAP,ENABLE);

    /* USART1 configure */
    usart_deinit(USART2);
    usart_baudrate_set(USART2, baud);
    usart_word_length_set(USART2, USART_WL_8BIT);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    usart_parity_config(USART2, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
    usart_enable(USART2);
}


int _put_char(int ch)
{
    usart_data_transmit(USART0, (uint8_t) ch );
    while ( usart_flag_get(USART0, USART_FLAG_TBE)== RESET){
    }

    return ch;
}
