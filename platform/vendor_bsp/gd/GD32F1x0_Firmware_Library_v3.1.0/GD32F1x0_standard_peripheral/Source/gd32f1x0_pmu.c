/*!
    \file  gd32f1x0_pmu.c
    \brief PMU driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0_pmu.h"

/*!
    \brief      reset PMU register
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
      \arg        PMU_LVDT_0: voltage threshold is 2.2V (GD32F130_150) or 2.4V (GD32F170_190)
      \arg        PMU_LVDT_1: voltage threshold is 2.3V (GD32F130_150) or 2.7V (GD32F170_190)
      \arg        PMU_LVDT_2: voltage threshold is 2.4V (GD32F130_150) or 3.0V (GD32F170_190)
      \arg        PMU_LVDT_3: voltage threshold is 2.5V (GD32F130_150) or 3.3V (GD32F170_190)
      \arg        PMU_LVDT_4: voltage threshold is 2.6V (GD32F130_150) or 3.6V (GD32F170_190)
      \arg        PMU_LVDT_5: voltage threshold is 2.7V (GD32F130_150) or 3.9V (GD32F170_190)
      \arg        PMU_LVDT_6: voltage threshold is 2.8V (GD32F130_150) or 4.2V (GD32F170_190)
      \arg        PMU_LVDT_7: voltage threshold is 2.9V (GD32F130_150) or 4.5V (GD32F170_190)
    \param[out] none
    \retval     none
*/
void pmu_lvd_select(uint32_t lvdt_n)
{
    /* disable LVD */
    PMU_CTL &= ~PMU_CTL_LVDEN;
    /* clear LVDT bits */
    PMU_CTL &= ~PMU_CTL_LVDT;
    /* set LVDT bits according to lvdt_n */
    PMU_CTL |= lvdt_n;
    /* enable LVD */
    PMU_CTL |= PMU_CTL_LVDEN;

}

/*!
    \brief      PMU lvd disable
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_lvd_disable(void)
{
    /* disable LVD */
    PMU_CTL &= ~PMU_CTL_LVDEN;
}

/*!
    \brief      PMU work at sleep mode
    \param[in]  sleepmodecmd:
      \arg        WFI_CMD:  use WFI command
      \arg        WFE_CMD:  use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_sleepmode(uint8_t sleepmodecmd)
{
    /* clear sleepdeep bit of Cortex-M3 system control register */
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
    \param[in]  ldo
      \arg        PMU_LDO_NORMAL: LDO normal work when pmu enter deepsleep mode
      \arg        PMU_LDO_LOWPOWER: LDO work at low power mode when pmu enter deepsleep mode
    \param[in]  deepsleepmodecmd: 
      \arg        WFI_CMD: use WFI command
      \arg        WFE_CMD: use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_deepsleepmode(uint32_t ldo,uint8_t deepsleepmodecmd)
{
    static uint32_t reg_snap[ 4 ];
    /* clear stbmod and ldolp bits */
    PMU_CTL &= ~((uint32_t)(PMU_CTL_STBMOD | PMU_CTL_LDOLP));
    
    /* set ldolp bit according to pmu_ldo */
    PMU_CTL |= ldo;
    
    /* set sleepdeep bit of Cortex-M3 system control register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    reg_snap[ 0 ] = REG32( 0xE000E010 );
    reg_snap[ 1 ] = REG32( 0xE000E100 );
    reg_snap[ 2 ] = REG32( 0xE000E104 );
    reg_snap[ 3 ] = REG32( 0xE000E108 );
    
    REG32( 0xE000E010 ) &= 0x00010004;
    REG32( 0xE000E180 )  = 0XB7FFEF19;
    REG32( 0xE000E184 )  = 0XFFFFFBFF;
    REG32( 0xE000E188 )  = 0xFFFFFFFF;
    
     /* select WFI or WFE command to enter deepsleep mode */
    if(WFI_CMD == deepsleepmodecmd){
        __WFI();
    }else{
        __SEV();
        __WFE();
        __WFE();
    }
    
    REG32( 0xE000E010 ) = reg_snap[ 0 ] ; 
    REG32( 0xE000E100 ) = reg_snap[ 1 ] ;
    REG32( 0xE000E104 ) = reg_snap[ 2 ] ;
    REG32( 0xE000E108 ) = reg_snap[ 3 ] ;   
    
    /* reset sleepdeep bit of Cortex-M3 system control register */
    SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
}

/*!
    \brief      pmu work at standby mode
    \param[in]  standbymodecmd:
      \arg        WFI_CMD: use WFI command
      \arg        WFE_CMD: use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_standbymode(uint8_t standbymodecmd)
{
    /* set sleepdeep bit of Cortex-M3 system control register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* set stbmod bit */
    PMU_CTL |= PMU_CTL_STBMOD;
        
    /* reset wakeup flag */
    PMU_CTL |= PMU_CTL_WURST;
    
    /* select WFI or WFE command to enter standby mode */
    if(WFI_CMD == standbymodecmd){
        __WFI();
    }else{
        __WFE();
    }
}

/*!
    \brief      clear flag bit
    \param[in]  flag_reset:
      \arg        PMU_FLAG_RESET_WAKEUP: reset wakeup flag
      \arg        PMU_FLAG_RESET_STANDBY: reset standby flag
    \param[out] none
    \retval     none
*/
void pmu_flag_clear(uint32_t flag_reset)
{
    switch(flag_reset){
    case PMU_FLAG_RESET_WAKEUP:
        /* reset wakeup flag */
        PMU_CTL |= PMU_CTL_WURST;
        break;
    case PMU_FLAG_RESET_STANDBY:
        /* reset standby flag */
        PMU_CTL |= PMU_CTL_STBRST;
        break;
    default :
        break;
    }
}

/*!
    \brief      get flag state
    \param[in]  flag:         
      \arg        PMU_FLAG_WAKEUP: wakeup flag 
      \arg        PMU_FLAG_STANDBY: standby flag 
      \arg        PMU_FLAG_LVD: lvd flag 
    \param[out] none
    \retval     FlagStatus SET or RESET
*/
FlagStatus pmu_flag_get(uint32_t flag )
{
    if(PMU_CS & flag){
        return  SET;
    }else{
        return  RESET;
    }
}

/*!
    \brief      backup domain write enable
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_backup_write_enable(void)
{
    PMU_CTL |= PMU_CTL_BKPWEN;
}

/*!
    \brief      backup domain write disable
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_backup_write_disable(void)
{
    PMU_CTL &= ~PMU_CTL_BKPWEN;
}

/*!
    \brief      wakeup pin enable
    \param[in]  wakeup_pin:
      \arg        PMU_WAKEUP_PIN0: wakeup pin 0
      \arg        PMU_WAKEUP_PIN1: wakeup pin 1
    \param[out] none
    \retval     none
*/
void pmu_wakeup_pin_enable(uint32_t wakeup_pin )
{
    PMU_CS |= wakeup_pin;
}

/*!
    \brief      wakeup pin disable
    \param[in]  wakeup_pin:
      \arg        PMU_WAKEUP_PIN0: wakeup pin 0
      \arg        PMU_WAKEUP_PIN1: wakeup pin 1
    \param[out] none
    \retval     none
*/
void pmu_wakeup_pin_disable(uint32_t wakeup_pin )
{
    PMU_CS &= ~wakeup_pin;

}
