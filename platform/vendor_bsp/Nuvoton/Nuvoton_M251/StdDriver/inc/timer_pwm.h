/**************************************************************************//**
 * @file     timer_pwm.h
 * @version  V1.00
 * @brief    M251 series Timer PWM Controller(Timer PWM) driver header file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __TIMER_PWM_H__
#define __TIMER_PWM_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup TIMER_PWM_Driver TIMER PWM Driver
  @{
*/

/** @addtogroup TIMER_PWM_EXPORTED_CONSTANTS TIMER PWM Exported Constants
  @{
*/
/*---------------------------------------------------------------------------------------------------------*/
/*  TPWM Output Channel Constant Definitions                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define TPWM_CH0                                (BIT0)       /*!< Indicate PWMx_CH0 \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  TPWM Output Channel Selection Definitions                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define TPWM_TOUT_PIN_FROM_TX                   (BIT0)       /*!< Indicate PWMx output to Tx pins \hideinitializer */
#define TPWM_TOUT_PIN_FROM_TX_EXT               (BIT8)       /*!< Indicate PWMx output to Tx_ext pins \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  TPWM Counter Mode Constant Definitions                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define TPWM_AUTO_RELOAD_MODE                   (0UL)       /*!< Auto-reload mode \hideinitializer */
#define TPWM_ONE_SHOT_MODE                      (1UL)       /*!< One-shot mode \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  TPWM Output Level Constant Definitions                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define TPWM_OUTPUT_TOGGLE                      (0UL)      /*!< Timer PWM output toggle \hideinitializer */
#define TPWM_OUTPUT_NOTHING                     (1UL)      /*!< Timer PWM output nothing \hideinitializer */
#define TPWM_OUTPUT_LOW                         (2UL)      /*!< Timer PWM output low \hideinitializer */
#define TPWM_OUTPUT_HIGH                        (3UL)      /*!< Timer PWM output high \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  TPWM Trigger ADC/DAC/PDMA Source Select Constant Definitions                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define TPWM_TRIGGER_AT_PERIOD_POINT            (0UL)       /*!< Timer PWM trigger EADC while counter period point event occurred \hideinitializer */
#define TPWM_TRIGGER_AT_COMPARE_POINT           (1UL)       /*!< Timer PWM trigger EADC while counter compare point event occurred \hideinitializer */
#define TPWM_TRIGGER_AT_PERIOD_OR_COMPARE_POINT (2UL)       /*!< Timer PWM trigger EADC while counter period or compare point event occurred \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  TPWM Counter Clock Source Select Constant Definitions                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define TPWM_CLKSRC_HXT                         (0UL)    /*!< Timer PWM Clock source selects to HXT  \hideinitializer \hideinitializer */
#define TPWM_CLKSRC_LXT                         (1UL)    /*!< Timer PWM Clock source selects to LXT  \hideinitializer \hideinitializer */
#define TPWM_CLKSRC_PCLK                        (2UL)    /*!< Timer PWM Clock source selects to PCLK \hideinitializer \hideinitializer */
#define TPWM_CLKSRC_TX                          (3UL)    /*!< Timer PWM Clock source selects to TX   \hideinitializer \hideinitializer */
#define TPWM_CLKSRC_LIRC                        (5UL)    /*!< Timer PWM Clock source selects to LIRC \hideinitializer \hideinitializer */
#define TPWM_CLKSRC_HIRC                        (7UL)    /*!< Timer PWM Clock source selects to HIRC \hideinitializer \hideinitializer */

/*@}*/ /* end of group TIMER_PWM_EXPORTED_CONSTANTS */


/** @addtogroup TIMER_PWM_EXPORTED_FUNCTIONS TIMER PWM Exported Functions
  @{
*/

/**
  * @brief      Enable PWM Counter Mode
  *
  * @param[in]  timer   The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to enable specified Timer channel as PWM counter mode, then timer counter mode is invalid.
  * @note       All registers about time counter function will be cleared to 0 and timer clock source will be changed to PCLKx automatically after executing this macro.
  * \hideinitializer
  */
#define TPWM_ENABLE_PWM_MODE(timer)         ((timer)->CTL |= TIMER_CTL_FUNCSEL_Msk)

/**
  * @brief      Disable PWM Counter Mode
  *
  * @param[in]  timer   The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to disable specified Timer channel as PWM counter mode, then timer counter mode is available.
  * @note       All registers about PWM counter function will be cleared to 0 after executing this macro.
  * \hideinitializer
  */
#define TPWM_DISABLE_PWM_MODE(timer)        ((timer)->CTL &= ~TIMER_CTL_FUNCSEL_Msk)

/**
  * @brief      Start PWM Counter
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to enable PWM generator and start counter counting.
  * \hideinitializer
  */
#define TPWM_START_COUNTER(timer)               ((timer)->PWMCTL |= TIMER_PWMCTL_CNTEN_Msk)

/**
  * @brief      Stop PWM Counter
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to stop PWM counter after current period is completed.
  * \hideinitializer
  */
#define TPWM_STOP_COUNTER(timer)                ((timer)->PWMPERIOD = 0x0UL)

/**
  * @brief      Set Counter Clock Prescaler
  *
  * @param[in]  timer           The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @param[in]  u32Prescaler    Clock prescaler of specified channel. Valid values are between 0x0~0xFFF.
  *
  * @return     None
  *
  * @details    This macro is used to set the prescaler of specified TIMER PWM.
  * @note       If prescaler is 0, then there is no scaling in counter clock source.
  * \hideinitializer
  */
#define TPWM_SET_PRESCALER(timer, u32Prescaler) ((timer)->PWMCLKPSC = (u32Prescaler))

/**
  * @brief      Get Counter Clock Prescaler
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     Target prescaler setting, CLKPSC (TIMERx_PWMCLKPSC[11:0])
  *
  * @details    Get the prescaler setting, the target counter clock divider is (CLKPSC + 1).
  * \hideinitializer
  */
#define TPWM_GET_PRESCALER(timer)       ((timer)->PWMCLKPSC)

/**
  * @brief      Set Counter Period
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @param[in]  u32Period   Period of specified channel. Valid values are between 0x0~0xFFFF.
  *
  * @return     None
  *
  * @details    This macro is used to set the period of specified TIMER PWM.
  * \hideinitializer
  */
#define TPWM_SET_PERIOD(timer, u32Period)  ((timer)->PWMPERIOD = (u32Period))

/**
  * @brief      Get Counter Period
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     Target period setting, PERIOD (TIMERx_PWMPERIOD[15:0])
  *
  * @details    This macro is used to get the period of specified TIMER PWM.
  * \hideinitializer
  */
#define TPWM_GET_PERIOD(timer)          ((timer)->PWMPERIOD)

/**
  * @brief      Set Comparator Value
  *
  * @param[in]  timer   The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @param[in]  u32Cmp  Comparator of specified channel. Valid values are between 0x0~0xFFFF.
  *
  * @return     None
  *
  * @details    This macro is used to set the comparator value of specified TIMER PWM.
  * \hideinitializer
  */
#define TPWM_SET_CMPDAT(timer, u32Cmp)     ((timer)->PWMCMPDAT = (u32Cmp))

/**
  * @brief      Get Comparator Value
  *
  * @param[in]  timer   The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     Target comparator setting, CMPDAT (TIMERx_PWMCMPDAT[15:0])
  *
  * @details    This macro is used to get the comparator value of specified TIMER PWM.
  * \hideinitializer
  */
#define TPWM_GET_CMPDAT(timer)          ((timer)->PWMCMPDAT)

/**
  * @brief      Clear Counter
  *
  * @param[in]  timer   The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to clear counter of specified TIMER PWM.
  * \hideinitializer
  */
#define TPWM_CLEAR_COUNTER(timer)       ((timer)->PWMCNTCLR = TIMER_PWMCNTCLR_CNTCLR_Msk)

/**
  * @brief      Enable Output Function
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @param[in]  u32Channel  Enable specified channel output function. Valid values are:
  *                             - \ref TPWM_CH0
  *
  * @return     None
  *
  * @details    This macro is used to enable output function of specified output pins.
  * @note       If the corresponding bit in u32ChMask parameter is 0, then output function will be disabled in this channel.
  * \hideinitializer
  */
#define TPWM_ENABLE_OUTPUT(timer, u32Channel)  ((timer)->PWMPOCTL = (u32Channel))

/**
  * @brief      Select Toggle-output Pin
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  * @param[in]  u32ToutSel  Toggle-output pin selection, valid values are:
  *                         - \ref TPWM_TOUT_PIN_FROM_TX
  *                         - \ref TPWM_TOUT_PIN_FROM_TX_EXT
  *
  * @return     None
  *
  * @details    This macro is used to select timer toggle-output pin is output on Tx or Tx_EXT pin.
  */
#define TPWM_SELECT_TOUT_PIN(timer, u32ToutSel)    ((timer)->PWMPOCTL = ((timer)->PWMPOCTL & ~TIMER_PWMPOCTL_POSEL_Msk) | (u32ToutSel))

/**
  * @brief      Set Output Inverse
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @param[in]  u32Channel  Set specified channel output is inversed or not. Valid values are:
  *                             - \ref TPWM_CH0
    *
  * @return     None
  *
  * @details    This macro is used to enable output inverse of specified output pins.
  * @note       If u32ChMask parameter is 0, then output inverse function will be disabled.
  * \hideinitializer
  */
#define TPWM_SET_OUTPUT_INVERSE(timer, u32Channel)  ((timer)->PWMPOLCTL = (u32Channel))

/**
  * @brief      Enable Period Event Interrupt
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to enable the period event interrupt function.
  * \hideinitializer
  */
#define TPWM_ENABLE_PERIOD_INT(timer)       ((timer)->PWMINTEN0 |= TIMER_PWMINTEN0_PIEN_Msk)

/**
  * @brief      Disable Period Event Interrupt
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to disable the period event interrupt function.
  * \hideinitializer
  */
#define TPWM_DISABLE_PERIOD_INT(timer)      ((timer)->PWMINTEN0 &= ~TIMER_PWMINTEN0_PIEN_Msk)

/**
  * @brief      Get Period Event Interrupt Flag
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @retval     0   Period event interrupt did not occur
  * @retval     1   Period event interrupt occurred
  *
  * @details    This macro indicates period event occurred or not.
  * \hideinitializer
  */
#define TPWM_GET_PERIOD_INT_FLAG(timer)     (((timer)->PWMINTSTS0 & TIMER_PWMINTSTS0_PIF_Msk)? 1 : 0)

/**
  * @brief      Clear Period Event Interrupt Flag
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro clears period event interrupt flag.
  * \hideinitializer
  */
#define TPWM_CLEAR_PERIOD_INT_FLAG(timer)   ((timer)->PWMINTSTS0 = TIMER_PWMINTSTS0_PIF_Msk)

/**
  * @brief      Enable Compare Up Event Interrupt
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to enable the compare up event interrupt function.
  * \hideinitializer
  */
#define TPWM_ENABLE_CMP_UP_INT(timer)       ((timer)->PWMINTEN0 |= TIMER_PWMINTEN0_CMPUIEN_Msk)

/**
  * @brief      Disable Compare Up Event Interrupt
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to disable the compare up event interrupt function.
  * \hideinitializer
  */
#define TPWM_DISABLE_CMP_UP_INT(timer)      ((timer)->PWMINTEN0 &= ~TIMER_PWMINTEN0_CMPUIEN_Msk)

/**
  * @brief      Get Compare Up Event Interrupt Flag
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @retval     0   Compare up event interrupt did not occur
  * @retval     1   Compare up event interrupt occurred
  *
  * @details    This macro indicates compare up event occurred or not.
  * \hideinitializer
  */
#define TPWM_GET_CMP_UP_INT_FLAG(timer)     (((timer)->PWMINTSTS0 & TIMER_PWMINTSTS0_CMPUIF_Msk)? 1 : 0)

/**
  * @brief      Clear Compare Up Event Interrupt Flag
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro clears compare up event interrupt flag.
  * \hideinitializer
  */
#define TPWM_CLEAR_CMP_UP_INT_FLAG(timer)   ((timer)->PWMINTSTS0 = TIMER_PWMINTSTS0_CMPUIF_Msk)

/**
  * @brief      Get Counter Reach Maximum Count Status
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @retval     0   Timer PWM counter never counts to maximum value
  * @retval     1   Timer PWM counter counts to maximum value, 0xFFFF
  *
  * @details    This macro indicates Timer PWM counter has count to 0xFFFF or not.
  * \hideinitializer
  */
#define TPWM_GET_REACH_MAX_CNT_STATUS(timer)    (((timer)->PWMSTATUS & TIMER_PWMSTATUS_CNTMAXF_Msk)? 1 : 0)

/**
  * @brief      Clear Counter Reach Maximum Count Status
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro clears reach maximum count status.
  * \hideinitializer
  */
#define TPWM_CLEAR_REACH_MAX_CNT_STATUS(timer)  ((timer)->PWMSTATUS = TIMER_PWMSTATUS_CNTMAXF_Msk)

/**
  * @brief      Get Trigger ADC Status
  *
  * @param[in]  timer   The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @retval     0       Trigger ADC start conversion is not occur
  * @retval     1       Specified counter compare event has trigger ADC start conversion
  *
  * @details    This macro is used to indicate PWM counter compare event has triggered ADC start conversion.
  * \hideinitializer
  */
#define TPWM_GET_TRG_ADC_STATUS(timer)              (((timer)->PWMSTATUS & TIMER_PWMSTATUS_EADCTRGF_Msk)? 1 : 0)

/**
  * @brief      Clear Trigger ADC Status
  *
  * @param[in]  timer   The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @return     None
  *
  * @details    This macro is used to clear PWM counter compare event trigger ADC status.
  * \hideinitializer
  */
#define TPWM_CLEAR_TRG_ADC_STATUS(timer)            ((timer)->PWMSTATUS = TIMER_PWMSTATUS_EADCTRGF_Msk)

uint32_t TPWM_GetModuleClock(TIMER_T *timer);
uint32_t TPWM_ConfigOutputFreqAndDuty(TIMER_T *timer, uint32_t u32Frequency, uint32_t u32DutyCycle);
void TPWM_EnableCounter(TIMER_T *timer);
void TPWM_DisableCounter(TIMER_T *timer);
void TPWM_EnableTriggerADC(TIMER_T *timer, uint32_t u32Condition);
void TPWM_DisableTriggerADC(TIMER_T *timer);
void TPWM_EnableTriggerDAC(TIMER_T *timer, uint32_t u32Condition);
void TPWM_DisableTriggerDAC(TIMER_T *timer);
void TPWM_EnableTriggerPDMA(TIMER_T *timer, uint32_t u32Condition);
void TPWM_DisableTriggerPDMA(TIMER_T *timer);
void TPWM_SetLoadMode(TIMER_T *timer, uint32_t u32LoadMode);

/*@}*/ /* end of group TIMER_PWM_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group TIMER_PWM_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_PWM_H__ */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
