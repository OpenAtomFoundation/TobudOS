/*!
    \file    gd32e50x_pmu.c
    \brief   PMU driver

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32e50x_pmu.h"

/*!
    \brief      reset PMU registers
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_deinit(void)
{
    /* reset PMU */
    rcu_periph_reset_enable(RCU_PMURST);
    rcu_periph_reset_disable(RCU_PMURST);
}

/*!
    \brief      select low voltage detector threshold
    \param[in]  lvdt_n: 
                only one parameter can be selected which is shown as below:
      \arg        PMU_LVDT_0: voltage threshold is 2.1V
      \arg        PMU_LVDT_1: voltage threshold is 2.3V
      \arg        PMU_LVDT_2: voltage threshold is 2.4V
      \arg        PMU_LVDT_3: voltage threshold is 2.6V
      \arg        PMU_LVDT_4: voltage threshold is 2.7V
      \arg        PMU_LVDT_5: voltage threshold is 2.9V
      \arg        PMU_LVDT_6: voltage threshold is 3.0V
      \arg        PMU_LVDT_7: voltage threshold is 3.1V
    \param[out] none
    \retval     none
*/
void pmu_lvd_select(uint32_t lvdt_n)
{
    /* disable LVD */
    PMU_CTL0 &= ~PMU_CTL0_LVDEN;
    /* clear LVDT bits */
    PMU_CTL0 &= ~PMU_CTL0_LVDT;
    /* set LVDT bits according to lvdt_n */
    PMU_CTL0 |= lvdt_n;
    /* enable LVD */
    PMU_CTL0 |= PMU_CTL0_LVDEN;
}

/*!
    \brief      disable PMU LVD
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_lvd_disable(void)
{
    /* disable LVD */
    PMU_CTL0 &= ~PMU_CTL0_LVDEN;
}

/*!
    \brief      enable high-driver mode
                this bit set by software only when IRC8M or HXTAL used as system clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_highdriver_mode_enable(void)
{
    PMU_CTL0 |= PMU_CTL0_HDEN;
}

/*!
    \brief      disable high-driver mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_highdriver_mode_disable(void)
{
    PMU_CTL0 &= ~PMU_CTL0_HDEN;
}

/*!
    \brief      switch high-driver mode
                this bit set by software only when IRC8M or HXTAL used as system clock
    \param[in]  highdr_switch:
                only one parameter can be selected which is shown as below:
      \arg        PMU_HIGHDR_SWITCH_NONE: disable high-driver mode switch
      \arg        PMU_HIGHDR_SWITCH_EN: enable high-driver mode switch
    \param[out] none
    \retval     none
*/
void pmu_highdriver_switch_select(uint32_t highdr_switch)
{
    /* wait for HDRF flag set */
    while(SET != pmu_flag_get(PMU_FLAG_HDRF)){
    }
    PMU_CTL0 &= ~PMU_CTL0_HDS;
    PMU_CTL0 |= highdr_switch;
}

/*!
    \brief      enable low-driver mode in deep-sleep/deep-sleep 1/deep-sleep 2 mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_lowdriver_mode_enable(void)
{
    PMU_CTL0 |= PMU_CTL0_LDEN;
}

/*!
    \brief      disable low-driver mode in deep-sleep/deep-sleep 1/deep-sleep 2 mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_lowdriver_mode_disable(void)
{
    PMU_CTL0 &= ~PMU_CTL0_LDEN;
}

/*!
    \brief      in deep-sleep/deep-sleep 1/deep-sleep 2 mode, driver mode when use low power LDO
    \param[in]  mode:
                only one parameter can be selected which is shown as below:
      \arg        PMU_NORMALDR_LOWPWR: normal driver when use low power LDO
      \arg        PMU_LOWDR_LOWPWR: low-driver mode enabled when LDEN is 11 and use low power LDO
    \param[out] none
    \retval     none
*/
void pmu_lowpower_driver_config(uint32_t mode)
{
    PMU_CTL0 &= ~PMU_CTL0_LDLP;
    PMU_CTL0 |= mode;
}

/*!
    \brief      in deep-sleep/deep-sleep 1/deep-sleep 2 mode, driver mode when use normal power LDO
    \param[in]  mode:
                only one parameter can be selected which is shown as below:
      \arg        PMU_NORMALDR_NORMALPWR: normal driver when use normal power LDO
      \arg        PMU_LOWDR_NORMALPWR: low-driver mode enabled when LDEN is 11 and use normal power LDO
    \param[out] none
    \retval     none
*/
void pmu_normalpower_driver_config(uint32_t mode)
{
    PMU_CTL0 &= ~PMU_CTL0_LDNP;
    PMU_CTL0 |= mode;
}

/*!
    \brief      PMU work at sleep mode
    \param[in]  sleepmodecmd:
                only one parameter can be selected which is shown as below:
      \arg        WFI_CMD: use WFI command
      \arg        WFE_CMD: use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_sleepmode(uint8_t sleepmodecmd)
{
    /* clear sleepdeep bit of Cortex-M33 system control register */
    SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
    
    /* select WFI or WFE command to enter sleep mode */
    if(WFI_CMD == sleepmodecmd){
        __WFI();
    }else{
        __WFE();
    }
}

/*!
    \brief      PMU work at deepsleep mode
    \param[in]  ldo:
                only one parameter can be selected which is shown as below:
      \arg        PMU_LDO_NORMAL: LDO normal work when pmu enter deepsleep mode
      \arg        PMU_LDO_LOWPOWER: LDO work at low power mode when pmu enter deepsleep mode
    \param[in]  deepsleepmodecmd:
                only one parameter can be selected which is shown as below:
      \arg        WFI_CMD: use WFI command
      \arg        WFE_CMD: use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_deepsleepmode(uint32_t ldo,uint8_t deepsleepmodecmd)
{
    static uint32_t reg_snap[ 4 ];     
    /* clear stbmod and ldolp bits */
    PMU_CTL0 &= ~((uint32_t)(PMU_CTL0_STBMOD | PMU_CTL0_LDOLP));
    /* clear deep-sleep 1/2 mode enable bits  */
    PMU_CTL1 &= ~(PMU_CTL1_DPMOD1 | PMU_CTL1_DPMOD2);
    
    /* set ldolp bit according to pmu_ldo */
    PMU_CTL0 |= ldo;
    
    /* set sleepdeep bit of Cortex-M33 system control register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    reg_snap[0] = REG32(0xE000E010U);
    reg_snap[1] = REG32(0xE000E100U);
    reg_snap[2] = REG32(0xE000E104U);
    reg_snap[3] = REG32(0xE000E108U);
    
    REG32(0xE000E010U) &= 0x00010004U;
    REG32(0xE000E180U)  = 0XFF7FF83DU;
    REG32(0xE000E184U)  = 0XBFFFF8FFU;
    REG32(0xE000E188U)  = 0xFF9FFFFFU;    
    
    /* select WFI or WFE command to enter deepsleep mode */
    if(WFI_CMD == deepsleepmodecmd){
        __WFI();
    }else{
        __SEV();
        __WFE();
        __WFE();
    }
    
    REG32(0xE000E010U) = reg_snap[0] ; 
    REG32(0xE000E100U) = reg_snap[1] ;
    REG32(0xE000E104U) = reg_snap[2] ;
    REG32(0xE000E108U) = reg_snap[3] ;  
    
    /* reset sleepdeep bit of Cortex-M33 system control register */
    SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
}

/*!
    \brief      PMU work at deepsleep mode 1
    \param[in]  ldo:
                only one parameter can be selected which is shown as below:
      \arg        PMU_LDO_NORMAL: LDO normal work when pmu enter deepsleep mode 1
      \arg        PMU_LDO_LOWPOWER: LDO work at low power mode when pmu enter deepsleep mode 1
    \param[in]  deepsleepmode1cmd: 
                only one parameter can be selected which is shown as below:
      \arg        WFI_CMD: use WFI command
      \arg        WFE_CMD: use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_deepsleepmode_1(uint32_t ldo,uint8_t deepsleepmode1cmd)
{
    static uint32_t reg_snap[ 4 ];     
    /* clear stbmod and ldolp bits */
    PMU_CTL0 &= ~((uint32_t)(PMU_CTL0_STBMOD | PMU_CTL0_LDOLP));
    /* clear deep-sleep 2 mode enable bit */
    PMU_CTL1 &= ~PMU_CTL1_DPMOD2;
    /* enable deep-sleep 1 mode */
    PMU_CTL1 |= PMU_CTL1_DPMOD1;
    
    /* set ldolp bit according to pmu_ldo */
    PMU_CTL0 |= ldo;
    
    /* set sleepdeep bit of Cortex-M33 system control register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    reg_snap[0] = REG32(0xE000E010U);
    reg_snap[1] = REG32(0xE000E100U);
    reg_snap[2] = REG32(0xE000E104U);
    reg_snap[3] = REG32(0xE000E108U);
    
    REG32(0xE000E010U) &= 0x00010004U;
    REG32(0xE000E180U)  = 0XFF7FF83DU;
    REG32(0xE000E184U)  = 0XBFFFF8FFU;
    REG32(0xE000E188U)  = 0xFF9FFFFFU;    
    
    /* select WFI or WFE command to enter deepsleep mode 1 */
    if(WFI_CMD == deepsleepmode1cmd){
        __WFI();
    }else{
        __SEV();
        __WFE();
        __WFE();
    }
    
    REG32(0xE000E010U) = reg_snap[0] ; 
    REG32(0xE000E100U) = reg_snap[1] ;
    REG32(0xE000E104U) = reg_snap[2] ;
    REG32(0xE000E108U) = reg_snap[3] ;  
    
    /* reset sleepdeep bit of Cortex-M33 system control register */
    SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
    PMU_CTL1 &= ~PMU_CTL1_DPMOD1;
}

/*!
    \brief      PMU work at deepsleep mode 2
    \param[in]  ldo:
                only one parameter can be selected which is shown as below:
      \arg        PMU_LDO_NORMAL: LDO normal work when pmu enter deepsleep mode 2
      \arg        PMU_LDO_LOWPOWER: LDO work at low power mode when pmu enter deepsleep mode 2
    \param[in]  deepsleepmode2cmd: 
                only one parameter can be selected which is shown as below:
      \arg        WFI_CMD: use WFI command
      \arg        WFE_CMD: use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_deepsleepmode_2(uint32_t ldo,uint8_t deepsleepmode2cmd)
{
    static uint32_t reg_snap[ 4 ];     
    /* clear stbmod and ldolp bits */
    PMU_CTL0 &= ~((uint32_t)(PMU_CTL0_STBMOD | PMU_CTL0_LDOLP));
    /* clear deep-sleep 1 mode enable bit */
    PMU_CTL1 &= ~PMU_CTL1_DPMOD1;
    /* enable deep-sleep 2 mode */
    PMU_CTL1 |= PMU_CTL1_DPMOD2;
    
    /* set ldolp bit according to pmu_ldo */
    PMU_CTL0 |= ldo;
    
    /* set sleepdeep bit of Cortex-M33 system control register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    reg_snap[0] = REG32(0xE000E010U);
    reg_snap[1] = REG32(0xE000E100U);
    reg_snap[2] = REG32(0xE000E104U);
    reg_snap[3] = REG32(0xE000E108U);
    
    REG32(0xE000E010U) &= 0x00010004U;
    REG32(0xE000E180U)  = 0XFF7FF83DU;
    REG32(0xE000E184U)  = 0XBFFFF8FFU;
    REG32(0xE000E188U)  = 0xFF9FFFFFU;    
    
    /* select WFI or WFE command to enter deepsleep mode 2 */
    if(WFI_CMD == deepsleepmode2cmd){
        __WFI();
    }else{
        __SEV();
        __WFE();
        __WFE();
    }
    
    REG32(0xE000E010U) = reg_snap[0] ; 
    REG32(0xE000E100U) = reg_snap[1] ;
    REG32(0xE000E104U) = reg_snap[2] ;
    REG32(0xE000E108U) = reg_snap[3] ;  
    
    /* reset sleepdeep bit of Cortex-M33 system control register */
    SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
    PMU_CTL1 &= ~PMU_CTL1_DPMOD2;
}

/*!
    \brief      pmu work at standby mode
    \param[in]  standbymodecmd:
                only one parameter can be selected which is shown as below:
      \arg        WFI_CMD: use WFI command
      \arg        WFE_CMD: use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_standbymode(uint8_t standbymodecmd)
{
    /* set sleepdeep bit of Cortex-M33 system control register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* set stbmod bit */
    PMU_CTL0 |= PMU_CTL0_STBMOD;
        
    /* reset wakeup flag */
    PMU_CTL0 |= PMU_CTL0_WURST;
    
    /* select WFI or WFE command to enter standby mode */
    if(WFI_CMD == standbymodecmd){
        __WFI();
    }else{
        __WFE();
    }
}

/*!
    \brief      enable PMU wakeup pin
    \param[in]  wakeup_pin:
                one or more parameters can be selected which are shown as below:
      \arg        PMU_WAKEUP_PIN0: WKUP Pin 0 (PA0) 
      \arg        PMU_WAKEUP_PIN1: WKUP Pin 1 (PC13)
      \arg        PMU_WAKEUP_PIN2: WKUP Pin 2 (PE6) 
      \arg        PMU_WAKEUP_PIN3: WKUP Pin 3 (PA2)   
      \arg        PMU_WAKEUP_PIN4: WKUP Pin 4 (PC5) 
      \arg        PMU_WAKEUP_PIN5: WKUP Pin 5 (PB5) 
      \arg        PMU_WAKEUP_PIN6: WKUP Pin 6 (PB15) 
      \arg        PMU_WAKEUP_PIN7: WKUP Pin 7 (PF8)   
    \param[out] none
    \retval     none
*/
void pmu_wakeup_pin_enable(uint32_t wakeup_pin)
{
    PMU_CS0 |= wakeup_pin;
}

/*!
    \brief      disable PMU wakeup pin
    \param[in]  wakeup_pin:
                one or more parameters can be selected which are shown as below:
      \arg        PMU_WAKEUP_PIN0: WKUP Pin 0 (PA0) 
      \arg        PMU_WAKEUP_PIN1: WKUP Pin 1 (PC13)
      \arg        PMU_WAKEUP_PIN2: WKUP Pin 2 (PE6) 
      \arg        PMU_WAKEUP_PIN3: WKUP Pin 3 (PA2)  
      \arg        PMU_WAKEUP_PIN4: WKUP Pin 4 (PC5) 
      \arg        PMU_WAKEUP_PIN5: WKUP Pin 5 (PB5) 
      \arg        PMU_WAKEUP_PIN6: WKUP Pin 6 (PB15) 
      \arg        PMU_WAKEUP_PIN7: WKUP Pin 7 (PF8) 
    \param[out] none
    \retval     none
*/
void pmu_wakeup_pin_disable(uint32_t wakeup_pin)
{
    PMU_CS0 &= ~(wakeup_pin);
}

/*!
    \brief      enable backup domain write
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_backup_write_enable(void)
{
    PMU_CTL0 |= PMU_CTL0_BKPWEN;
}

/*!
    \brief      disable backup domain write
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_backup_write_disable(void)
{
    PMU_CTL0 &= ~PMU_CTL0_BKPWEN;
}

/*!
    \brief      clear flag bit
    \param[in]  flag_reset:
                only one parameter can be selected which is shown as below:
      \arg        PMU_FLAG_RESET_WAKEUP: reset wakeup flag
      \arg        PMU_FLAG_RESET_STANDBY: reset standby flag
      \arg        PMU_FLAG_RESET_DEEPSLEEP_1: reset deep-sleep 1 mode status flag
      \arg        PMU_FLAG_RESET_DEEPSLEEP_2: reset deep-sleep 2 mode status flag 
    \param[out] none
    \retval     none
*/
void pmu_flag_clear(uint32_t flag_reset)
{
    switch(flag_reset){
    case PMU_FLAG_RESET_WAKEUP:
        /* reset wakeup flag */
        PMU_CTL0 |= PMU_CTL0_WURST;
        break;
    case PMU_FLAG_RESET_STANDBY:
        /* reset standby flag */
        PMU_CTL0 |= PMU_CTL0_STBRST;
        break;
    case PMU_FLAG_RESET_DEEPSLEEP_1:
        /* reset deep-sleep 1 mode status flag */
        PMU_CS1 &= ~PMU_CS1_DPF1;
        break;
    case PMU_FLAG_RESET_DEEPSLEEP_2:
        /* reset deep-sleep 2 mode status flag */
        PMU_CS1 &= ~PMU_CS1_DPF2;
        break;  
    default :
        break;
    }
}

/*!
    \brief      get flag state
    \param[in]  flag:
                only one parameter can be selected which is shown as below:
      \arg        PMU_FLAG_WAKEUP: wakeup flag
      \arg        PMU_FLAG_STANDBY: standby flag
      \arg        PMU_FLAG_LVD: lvd flag
      \arg        PMU_FLAG_HDRF: high-driver ready flag
      \arg        PMU_FLAG_HDSRF: high-driver switch ready flag
      \arg        PMU_FLAG_LDRF: low-driver mode ready flag
      \arg        PMU_FLAG_DEEPSLEEP_1: deep-sleep 1 mode status flag
      \arg        PMU_FLAG_DEEPSLEEP_2: deep-sleep 2 mode status flag 
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus pmu_flag_get(uint32_t flag)
{
    FlagStatus ret = RESET;
    
    if(RESET != (flag & BIT(31))){
        flag &= ~BIT(31);
        if(PMU_CS1 & flag){
            ret = SET;
        }
    }else{
        if(PMU_CS0 & flag){
            ret = SET;
        }
    }
    return ret;
}
