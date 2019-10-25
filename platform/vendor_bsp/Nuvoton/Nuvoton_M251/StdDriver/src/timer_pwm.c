/**************************************************************************//**
 * @file     timer.c
 * @version  V3.00
 * @brief    Timer PWM Controller(Timer PWM) driver source file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup TIMER_PWM_Driver TIMER PWM Driver
  @{
*/

/** @addtogroup TIMER_PWM_EXPORTED_FUNCTIONS TIMER PWM Exported Functions
  @{
*/

/**
  * @brief      Get PWM Clock Frequency
  *
  * @param[in]  timer   The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     PWM clock frequency
  *
  * @details    This API is used to get the timer clock frequency.
  * @note       This API cannot return correct clock rate if timer source is from external clock input.
  */
uint32_t TPWM_GetModuleClock(TIMER_T *timer)
{
    uint32_t u32Src;
    const uint32_t au32Clk[] = {__HXT, __LXT, 0UL, 0UL, 0UL, __LIRC, 0UL, __HIRC};

    if (timer == TIMER0)
        u32Src = (CLK->CLKSEL1 & CLK_CLKSEL1_TMR0SEL_Msk) >> CLK_CLKSEL1_TMR0SEL_Pos;
    else if (timer == TIMER1)
        u32Src = (CLK->CLKSEL1 & CLK_CLKSEL1_TMR1SEL_Msk) >> CLK_CLKSEL1_TMR1SEL_Pos;
    else if (timer == TIMER2)
        u32Src = (CLK->CLKSEL1 & CLK_CLKSEL1_TMR2SEL_Msk) >> CLK_CLKSEL1_TMR2SEL_Pos;
    else  // Timer 3
        u32Src = (CLK->CLKSEL1 & CLK_CLKSEL1_TMR3SEL_Msk) >> CLK_CLKSEL1_TMR3SEL_Pos;

    if (u32Src == 2UL)
    {
        return (SystemCoreClock);
    }

    return (au32Clk[u32Src]);
}

/**
  * @brief      Configure PWM Output Frequency and Duty Cycle
  *
  * @param[in]  timer           The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  * @param[in]  u32Frequency    Target generator frequency.
  * @param[in]  u32DutyCycle    Target generator duty cycle percentage. Valid range are between 0~100. 10 means 10%, 20 means 20%...
  *
  * @return     Nearest frequency clock in nano second
  *
  * @details    This API is used to configure PWM output frequency and duty cycle in up count type and auto-reload operation mode.
  * @note       This API is only available if Timer PWM counter clock source is from TMRx_CLK.
  */
uint32_t TPWM_ConfigOutputFreqAndDuty(TIMER_T *timer, uint32_t u32Frequency, uint32_t u32DutyCycle)
{
    uint32_t u32PWMClockFreq, u32TargetFreq;
    uint32_t u32Prescaler = 0x100UL, u32Period, u32CMP;

    u32PWMClockFreq = TPWM_GetModuleClock(timer);

    /* Calculate u8PERIOD and u8PSC */
    for (u32Prescaler = 1; u32Prescaler <= 0x100UL; u32Prescaler++)
    {
        u32Period = (u32PWMClockFreq / u32Prescaler) / u32Frequency;

        /* If target u32Period is larger than 0x10000, need to use a larger prescaler */
        if (u32Period > 0x10000UL)
            continue;

        break;
    }

    /* Store return value here 'cos we're gonna change u32Prescaler & u32Period to the real value to fill into register */
    u32TargetFreq = (u32PWMClockFreq / u32Prescaler) / u32Period;

    /* Set PWM to auto-reload mode */
    timer->PWMCTL = (timer->PWMCTL & ~TIMER_PWMCTL_CNTMODE_Msk) | (TPWM_AUTO_RELOAD_MODE << TIMER_PWMCTL_CNTMODE_Pos);

    /* Convert to real register value */
    TPWM_SET_PRESCALER(timer, (u32Prescaler - 1UL));

    TPWM_SET_PERIOD(timer, (u32Period - 1UL));

    if (u32DutyCycle)
    {
        u32CMP = (u32DutyCycle * u32Period) / 100UL;
    }
    else
    {
        u32CMP = 0UL;
    }

    TPWM_SET_CMPDAT(timer, u32CMP);
    return (u32TargetFreq);
}

/**
  * @brief      Enable PWM Counter
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This function is used to enable PWM generator and start counter counting.
  */
void TPWM_EnableCounter(TIMER_T *timer)
{
    timer->PWMCTL |= TIMER_PWMCTL_CNTEN_Msk;
}

/**
  * @brief      Disable PWM Generator
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details This function is used to disable PWM counter immediately by clear CNTEN (TIMERx_PWMCTL[0]) bit.
  */
void TPWM_DisableCounter(TIMER_T *timer)
{
    timer->PWMCTL &= ~TIMER_PWMCTL_CNTEN_Msk;
}

/**
  * @brief      Enable Trigger ADC
  *
  * @param[in]  timer           The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  * @param[in]  u32Condition    The condition to trigger ADC. It could be one of following conditions:
  *                                 - \ref TPWM_TRIGGER_AT_PERIOD_POINT
  *                                 - \ref TPWM_TRIGGER_AT_COMPARE_POINT
  *                                 - \ref TPWM_TRIGGER_AT_PERIOD_OR_COMPARE_POINT
  * @return     None
  *
  * @details    This function is used to enable specified counter compare event to trigger ADC.
  */
void TPWM_EnableTriggerADC(TIMER_T *timer, uint32_t u32Condition)
{
    timer->PWMTRGCTL &= ~TIMER_PWMTRGCTL_TRGSEL_Msk;
    timer->PWMTRGCTL |= TIMER_PWMTRGCTL_PWMTRGEADC_Msk | (u32Condition << TIMER_PWMTRGCTL_TRGSEL_Pos);
}

/**
  * @brief      Disable Trigger ADC
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This function is used to disable counter compare event to trigger ADC.
  */
void TPWM_DisableTriggerADC(TIMER_T *timer)
{
    timer->PWMTRGCTL &= ~TIMER_PWMTRGCTL_PWMTRGEADC_Msk;
}

/**
  * @brief      Enable Trigger DAC
  *
  * @param[in]  timer           The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  * @param[in]  u32Condition    The condition to trigger DAC. It could be one of following conditions:
  *                                 - \ref TPWM_TRIGGER_AT_PERIOD_POINT
  *                                 - \ref TPWM_TRIGGER_AT_COMPARE_POINT
  *                                 - \ref TPWM_TRIGGER_AT_PERIOD_OR_COMPARE_POINT
  * @return     None
  *
  * @details    This function is used to enable specified counter compare event to trigger ADC.
  */
void TPWM_EnableTriggerDAC(TIMER_T *timer, uint32_t u32Condition)
{
    timer->PWMTRGCTL &= ~TIMER_PWMTRGCTL_TRGSEL_Msk;
    timer->PWMTRGCTL |= TIMER_PWMTRGCTL_PWMTRGDAC_Msk | (u32Condition << TIMER_PWMTRGCTL_TRGSEL_Pos);
}

/**
  * @brief      Disable Trigger DAC
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This function is used to disable counter compare event to trigger ADC.
  */
void TPWM_DisableTriggerDAC(TIMER_T *timer)
{
    timer->PWMTRGCTL &= ~TIMER_PWMTRGCTL_PWMTRGDAC_Msk;
}

/**
  * @brief      Enable Trigger PDMA
  *
  * @param[in]  timer           The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  * @param[in]  u32Condition    The condition to trigger PDMA. It could be one of following conditions:
  *                                 - \ref TPWM_TRIGGER_AT_PERIOD_POINT
  *                                 - \ref TPWM_TRIGGER_AT_COMPARE_POINT
  *                                 - \ref TPWM_TRIGGER_AT_PERIOD_OR_COMPARE_POINT
  * @return     None
  *
  * @details    This function is used to enable specified counter compare event to trigger ADC.
  */
void TPWM_EnableTriggerPDMA(TIMER_T *timer, uint32_t u32Condition)
{
    timer->PWMTRGCTL &= ~TIMER_PWMTRGCTL_TRGSEL_Msk;
    timer->PWMTRGCTL |= TIMER_PWMTRGCTL_PWMTRGPDMA_Msk | (u32Condition << TIMER_PWMTRGCTL_TRGSEL_Pos);
}

/**
  * @brief      Disable Trigger PDMA
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This function is used to disable counter compare event to trigger ADC.
  */
void TPWM_DisableTriggerPDMA(TIMER_T *timer)
{
    timer->PWMTRGCTL &= ~TIMER_PWMTRGCTL_PWMTRGPDMA_Msk;
}

/*@}*/ /* end of group TIMER_PWM_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group TIMER_PWM_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
