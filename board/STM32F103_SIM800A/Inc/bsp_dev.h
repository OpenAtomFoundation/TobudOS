#ifndef BSP_DEV_H
#define BSP_DEV_H

/*!
 * UART peripheral ID
 */
typedef enum
{
    UART_1,
    UART_2,
    UART_3,
    UART_COUNT,
} uart_id_t;

#define DEV_UART_NUM UART_COUNT

#define SIM800C_UART_ID UART_2

#endif
