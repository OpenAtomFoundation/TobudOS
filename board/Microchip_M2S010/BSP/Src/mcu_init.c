#include "mcu_init.h"

mss_uart_instance_t * const gp_my_uart = &g_mss_uart0;

void board_init(void)
{
    /* disable watchdog */
    SYSREG->WDOG_CR = 0;
    
    /* update system core clock */
    SystemCoreClockUpdate();

    /* mss gpio config */
    MSS_GPIO_init();
    MSS_GPIO_config(MSS_GPIO_0, MSS_GPIO_OUTPUT_MODE);
    MSS_GPIO_config(MSS_GPIO_1, MSS_GPIO_OUTPUT_MODE);
    
    /* set gpio output */
    MSS_GPIO_set_output(MSS_GPIO_0, 1);
    MSS_GPIO_set_output(MSS_GPIO_1, 0);
    
    /* mss uart initial: 115200, 8, no, 1 */
    MSS_UART_init(gp_my_uart,
              MSS_UART_115200_BAUD,
              MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);
    
    printf("SystemCoreClock Frequency: %.2f MHz\r\n", SystemCoreClock/1000000.0);
}

void SysTick_Handler(void)
{
    if (tos_knl_is_running())
    {
        tos_knl_irq_enter();
        tos_tick_handler();
        tos_knl_irq_leave();
    }
}   
