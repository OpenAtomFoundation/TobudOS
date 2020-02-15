#include "stm8l15x.h"
#include "tim.h"

void Timer2_Init(uint32_t clock, uint32_t tick_per_second)
{
    uint32_t tick_time;

    tick_time = clock / tick_per_second;

    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);

    TIM2_TimeBaseInit(TIM2_Prescaler_1, TIM2_CounterMode_Up, (uint16_t)tick_time);

    /* Clear TIM2 update flag */
    TIM2_ClearFlag(TIM2_FLAG_Update);
    TIM2_ARRPreloadConfig(ENABLE);

    /* Enable update interrupt */
    TIM2_ITConfig(TIM2_IT_Update, ENABLE);
    TIM2_Cmd(ENABLE);

    enableInterrupts();
}

