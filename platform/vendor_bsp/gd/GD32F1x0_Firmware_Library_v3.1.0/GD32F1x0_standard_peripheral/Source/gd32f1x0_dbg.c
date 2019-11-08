/*!
    \file  gd32f1x0_dbg.c
    \brief DBG driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0_dbg.h"

#define DBG_RESET_VAL       0x00000000U

/*!
    \brief      deinitialize the DBG
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dbg_deinit(void)
{
    DBG_CTL0 = DBG_RESET_VAL;
    DBG_CTL1 = DBG_RESET_VAL;
}

/*!
    \brief      read DBG_ID code register
    \param[in]  none
    \param[out] none
    \retval     DBG_ID code
*/
uint32_t dbg_id_get(void)
{
    return DBG_ID;
}

/*!
    \brief      enable low power behavior when the mcu is in debug mode
    \param[in]  dbg_low_power:
                this parameter can be any combination of the following values:
      \arg        DBG_LOW_POWER_SLEEP: keep debugger connection during sleep mode
      \arg        DBG_LOW_POWER_DEEPSLEEP: keep debugger connection during deepsleep mode
      \arg        DBG_LOW_POWER_STANDBY: keep debugger connection during standby mode
    \param[out] none
    \retval     none
*/
void dbg_low_power_enable(uint32_t dbg_low_power)
{
    DBG_CTL0 |= dbg_low_power;
}

/*!
    \brief      disable low power behavior when the mcu is in debug mode
    \param[in]  dbg_low_power:
                this parameter can be any combination of the following values:
      \arg        DBG_LOW_POWER_SLEEP: donot keep debugger connection during sleep mode
      \arg        DBG_LOW_POWER_DEEPSLEEP: donot keep debugger connection during deepsleep mode
      \arg        DBG_LOW_POWER_STANDBY: donot keep debugger connection during standby mode
    \param[out] none
    \retval     none
*/
void dbg_low_power_disable(uint32_t dbg_low_power)
{
    DBG_CTL0 &= ~dbg_low_power;
}

/*!
    \brief      enable peripheral behavior when the mcu is in debug mode
    \param[in]  dbg_periph: refer to dbg_periph_enum
                only one parameter can be selected which is shown as below:
      \arg        DBG_FWDGT_HOLD : debug FWDGT kept when core is halted
      \arg        DBG_WWDGT_HOLD : debug WWDGT kept when core is halted
      \arg        DBG_CANx_HOLD (x=0,1): hold CANx counter when core is halted(170_190 series only)
      \arg        DBG_I2Cx_HOLD (x=0,1,2): hold I2Cx smbus when core is halted
      \arg        DBG_TIMERx_HOLD (x=0,1,2,5,13,14,15,16): hold TIMERx counter when core is halted
      \arg        DBG_RTC_HOLD : hold RTC calendar and wakeup counter when core is halted
    \param[out] none
    \retval     none
*/
void dbg_periph_enable(dbg_periph_enum dbg_periph)
{
    DBG_REG_VAL(dbg_periph) |= BIT(DBG_BIT_POS(dbg_periph));
}

/*!
    \brief      disable peripheral behavior when the mcu is in debug mode
    \param[in]  dbg_periph: refer to dbg_periph_enum
                only one parameter can be selected which is shown as below:
      \arg        DBG_FWDGT_HOLD : debug FWDGT kept when core is halted
      \arg        DBG_WWDGT_HOLD : debug WWDGT kept when core is halted
      \arg        DBG_CANx_HOLD (x=0,1): hold CANx counter when core is halted(170_190 series only)
      \arg        DBG_I2Cx_HOLD (x=0,1,2): hold I2Cx smbus when core is halted
      \arg        DBG_TIMERx_HOLD (x=0,1,2,5,13,14,15,16): hold TIMERx counter when core is halted
      \arg        DBG_RTC_HOLD : hold RTC calendar and wakeup counter when core is halted
    \param[out] none
    \retval     none
*/
void dbg_periph_disable(dbg_periph_enum dbg_periph)
{
    DBG_REG_VAL(dbg_periph) &= ~BIT(DBG_BIT_POS(dbg_periph));
}
