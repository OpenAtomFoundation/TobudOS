#include "mcu_init.h"
#include "tos_at.h"

extern at_agent_t esp8266_agent;
extern at_agent_t ec600s_agent;

void board_init(void)
{
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_InitBootClocks();
    
    BOARD_InitDebugConsole();
}

void SysTick_Handler(void)
{
    if (tos_knl_is_running()) {
        tos_knl_irq_enter();
        tos_tick_handler();
        tos_knl_irq_leave();
    }
}

/* LPUART2_IRQn interrupt handler */
void LPUART2_IRQHandler(void)
{
    uint8_t data;
    
    tos_knl_irq_enter();

    /* If new data arrived. */
    if ((kLPUART_RxDataRegFullFlag)&LPUART_GetStatusFlags(LPUART2))
    {
        data = LPUART_ReadByte(LPUART2);
        tos_at_uart_input_byte(&esp8266_agent, data);
    }
    
    tos_knl_irq_leave();
}

/* LPUART4_IRQn interrupt handler */
void LPUART4_IRQHandler(void)
{
    uint8_t data;
    
    tos_knl_irq_enter();

    /* If new data arrived. */
    if ((kLPUART_RxDataRegFullFlag)&LPUART_GetStatusFlags(LPUART4))
    {
        data = LPUART_ReadByte(LPUART4);
        tos_at_uart_input_byte(&ec600s_agent, data);
    }
    
    tos_knl_irq_leave();
}

