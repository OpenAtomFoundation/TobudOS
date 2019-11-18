/*!
    \file  gd32f1x0_syscfg.c
    \brief SYSCFG driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0_syscfg.h"

/*!
    \brief      reset the SYSCFG registers
    \param[in]  none
    \param[out] none
    \retval     none
*/
void syscfg_deinit(void)
{
    rcu_periph_reset_enable(RCU_CFGCMPRST);
    rcu_periph_reset_disable(RCU_CFGCMPRST);
}

/*!
    \brief      enable the DMA channels remapping
    \param[in]  syscfg_dma_remap: specify the DMA channels to remap
      \arg        SYSCFG_DMA_REMAP_TIMER16: remap TIMER16 channel0 and UP DMA requests to channel1(defaut channel0)
      \arg        SYSCFG_DMA_REMAP_TIMER15: remap TIMER15 channel2 and UP DMA requests to channel3(defaut channel2)
      \arg        SYSCFG_DMA_REMAP_USART0RX: remap USART0 Rx DMA request to channel4(default channel2)
      \arg        SYSCFG_DMA_REMAP_USART0TX: remap USART0 Tx DMA request to channel3(default channel1)
      \arg        SYSCFG_DMA_REMAP_ADC: remap ADC DMA requests from channel0 to channel1
    \param[out] none
    \retval     none
*/
void syscfg_dma_remap_enable(uint32_t syscfg_dma_remap)
{
    SYSCFG_CFG0 |= syscfg_dma_remap;
}

/*!
    \brief      disable the DMA channels remapping
    \param[in]  syscfg_dma_remap: specify the DMA channels to remap
      \arg        SYSCFG_DMA_REMAP_TIMER16: remap TIMER16 channel0 and UP DMA requests to channel1(defaut channel0)
      \arg        SYSCFG_DMA_REMAP_TIMER15: remap TIMER15 channel2 and UP DMA requests to channel3(defaut channel2)
      \arg        SYSCFG_DMA_REMAP_USART0RX: remap USART0 Rx DMA request to channel4(default channel2)
      \arg        SYSCFG_DMA_REMAP_USART0TX: remap USART0 Tx DMA request to channel3(default channel1)
      \arg        SYSCFG_DMA_REMAP_ADC: remap ADC DMA requests from channel0 to channel1
    \param[out] none
    \retval     none
*/
void syscfg_dma_remap_disable(uint32_t syscfg_dma_remap)
{
    SYSCFG_CFG0 &= ~syscfg_dma_remap;
}

/*!
    \brief      enable PB9 high current capability
    \param[in]  none
    \param[out] none
    \retval     none
*/
void syscfg_high_current_enable(void)
{
    SYSCFG_CFG0 |= SYSCFG_HIGH_CURRENT_ENABLE;
}

/*!
    \brief      disable PB9 high current capability
    \param[in]  none
    \param[out] none
    \retval     none
*/
void syscfg_high_current_disable(void)
{
    SYSCFG_CFG0 &= SYSCFG_HIGH_CURRENT_DISABLE;
}

#ifdef GD32F170_190
/*!
    \brief      configure the VLCD intermediate voltage rail
    \param[in]  vlcd_bias: specify VLCD bias
      \arg        VLCD_BIAS1_2_RAIL1: VLCD bias is 1/2, using rail1
      \arg        VLCD_BIAS1_2_RAIL2: VLCD bias is 1/2, using rail2
      \arg        VLCD_BIAS1_2_RAIL3: VLCD bias is 1/2, using rail3
      \arg        VLCD_BIAS1_3_RAIL1_2: VLCD bias is 1/3, using rail1 and rail2
      \arg        VLCD_BIAS1_3_RAIL1_3: VLCD bias is 1/3, using rail1 and rail3
      \arg        VLCD_BIAS1_4_RAILALL: VLCD bias is 1/4, using all rails
    \param[out] none
    \retval     none
*/
void syscfg_vlcd_rail_config(uint8_t vlcd_bias)
{
    uint32_t cfg1 = SYSCFG_CFG1;

    /* Clear system configuration register 1 */
    SYSCFG_CFG1 = 0U;

    switch(vlcd_bias){
    /* according to VLCD bias, configure rails combination */
    case VLCD_BIAS1_2_RAIL1:
        SYSCFG_CFG1 |= SYSCFG_VLCD_RAIL1;
        break;
    case VLCD_BIAS1_2_RAIL2:
        SYSCFG_CFG1 |= SYSCFG_VLCD_RAIL2;
        break;
    case VLCD_BIAS1_2_RAIL3:
        SYSCFG_CFG1 |= SYSCFG_VLCD_RAIL3;
        break;
    case VLCD_BIAS1_3_RAIL1_2:
        SYSCFG_CFG1 |= SYSCFG_VLCD_RAIL2 | SYSCFG_VLCD_RAIL1;
        break;
    case VLCD_BIAS1_3_RAIL1_3:
        SYSCFG_CFG1 |= SYSCFG_VLCD_RAIL3 | SYSCFG_VLCD_RAIL1;
        break;
    case VLCD_BIAS1_4_RAILALL:
        SYSCFG_CFG1 |= SYSCFG_VLCD_RAIL3 | SYSCFG_VLCD_RAIL2 | SYSCFG_VLCD_RAIL1;
        break;
    default:
        SYSCFG_CFG1 = cfg1;
        break;
    }
}
#endif /* GD32F170_190 */

/*!
    \brief      configure the GPIO pin as EXTI Line
    \param[in]  exti_port: specify the GPIO port used in EXTI
      \arg        EXTI_SOURCE_GPIOx(x = A,B,C,D,F): EXTI GPIO port
    \param[in]  exti_pin: specify the EXTI line
      \arg        EXTI_SOURCE_PINx(x = 0..15): EXTI GPIO pin
    \param[out] none
    \retval     none
*/
void syscfg_exti_line_config(uint8_t exti_port, uint8_t exti_pin)
{
    uint32_t clear_exti_mask = ~((uint32_t)EXTI_SS_MASK << (EXTI_SS_MSTEP(exti_pin)));
    uint32_t config_exti_mask = ((uint32_t)exti_port) << (EXTI_SS_MSTEP(exti_pin));

    switch(exti_pin / EXTI_SS_JSTEP){
    case EXTISS0:
        /* clear EXTI source line(0..3) */
        SYSCFG_EXTISS0 &= clear_exti_mask;
        /* configure EXTI soure line(0..3) */
        SYSCFG_EXTISS0 |= config_exti_mask;
        break;
    case EXTISS1:
        /* clear EXTI soure line(4..7) */
        SYSCFG_EXTISS1 &= clear_exti_mask;
        /* configure EXTI soure line(4..7) */
        SYSCFG_EXTISS1 |= config_exti_mask;
        break;
    case EXTISS2:
        /* clear EXTI soure line(8..11) */
        SYSCFG_EXTISS2 &= clear_exti_mask;
        /* configure EXTI soure line(8..11) */
        SYSCFG_EXTISS2 |= config_exti_mask;
        break;
    case EXTISS3:
        /* clear EXTI soure line(12..15) */
        SYSCFG_EXTISS3 &= clear_exti_mask;
        /* configure EXTI soure line(12..15) */
        SYSCFG_EXTISS3 |= config_exti_mask;
        break;
    default:
        break;
    }
}

/*!
    \brief      connect TIMER0/14/15/16 break input to the selected parameter
    \param[in]  syscfg_lock: Specify the parameter to be connected
      \arg        SYSCFG_LOCK_LOCKUP: Cortex-M3 lockup output connected to the break input
      \arg        SYSCFG_LOCK_SRAM_PARITY_ERROR: SRAM_PARITY check error connected to the break input
      \arg        SYSCFG_LOCK_LVD: LVD interrupt connected to the break input
    \param[out] none
    \retval     none
*/
void syscfg_lock_config(uint32_t syscfg_lock)
{
    SYSCFG_CFG2 |= syscfg_lock;
}

/*!
    \brief      check if the specified flag in SYSCFG_CFG2 is set or not.
    \param[in]  syscfg_flag: specify the flag in SYSCFG_CFG2 to check.
      \arg        SYSCFG_SRAM_PCEF: SRAM parity check error flag.
    \param[out] none
    \retval     the syscfg_flag state returned (SET or RESET).
  */
FlagStatus syscfg_flag_get(uint32_t syscfg_flag)
{
    if((SYSCFG_CFG2 & syscfg_flag) != (uint32_t)RESET){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear the flag in SYSCFG_CFG2 by writing 1.
    \param[in]  syscfg_flag: Specify the flag in SYSCFG_CFG2 to clear.
      \arg        SYSCFG_SRAM_PCEF: SRAM parity check error flag.
    \param[out] none
    \retval     none
*/
void syscfg_flag_clear(uint32_t syscfg_flag)
{
    SYSCFG_CFG2 |= (uint32_t) syscfg_flag;
}
