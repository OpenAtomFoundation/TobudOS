/*!
    \file  gd32f1x0_opa.c
    \brief OPA driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifdef GD32F170_190

#include "gd32f1x0_opa.h" 

/*!
    \brief      deinit the OPA register to its default reset value
    \param[in]  none
    \param[out] none
    \retval     none
*/
void opa_deinit(void)
{
    rcu_periph_reset_enable(RCU_OPAIVREFRST);
    rcu_periph_reset_disable(RCU_OPAIVREFRST);
}

/*!
    \brief      enable OPA switch
    \param[in]  opax_swy
      \arg        OPA_T3OPA0: T3 switch enable for OPA0
      \arg        OPA_S1OPA0: S1 switch enable for OPA0
      \arg        OPA_S2OPA0: S2 switch enable for OPA0
      \arg        OPA_S3OPA0: S3 switch enable for OPA0
      \arg        OPA_T3OPA1: T3 switch enable for OPA1
      \arg        OPA_S1OPA1: S1 switch enable for OPA1
      \arg        OPA_S2OPA1: S2 switch enable for OPA1
      \arg        OPA_S3OPA1: S3 switch enable for OPA1
      \arg        OPA_S4OPA1: S4 switch enable for OPA1
      \arg        OPA_T3OPA2: T3 switch enable for OPA2
      \arg        OPA_S1OPA2: S3 switch enable for OPA2
      \arg        OPA_S2OPA2: S3 switch enable for OPA2
      \arg        OPA_S3OPA2: S3 switch enable for OPA2
    \param[out] none
    \retval     none
*/
void opa_switch_enable(uint32_t opax_swy)
{
    OPA_CTL |= (uint32_t)(opax_swy);
}

/*!
    \brief      enable OPA
    \param[in]  opa_periph
      \arg        OPAx(x =0,1,2)
    \param[out] none
    \retval     none
*/
void opa_enable(uint32_t opa_periph)
{
    if(OPA0 == opa_periph){
        OPA_CTL &= ~OPA_CTL_OPA0PD;
    }else if(OPA1 == opa_periph){
        OPA_CTL &= ~OPA_CTL_OPA1PD;
    }else{
        OPA_CTL &= ~OPA_CTL_OPA2PD;
    }
}

/*!
    \brief      disable OPA
    \param[in]  opa_periph
      \arg        OPAx(x =0,1,2)
    \param[out] none
    \retval     none
*/
void opa_disable(uint32_t opa_periph)
{
    if(OPA0 == opa_periph){
        OPA_CTL |= OPA_CTL_OPA0PD;
    }else if(OPA1 == opa_periph){
        OPA_CTL |= OPA_CTL_OPA1PD;
    }else{
        OPA_CTL |= OPA_CTL_OPA2PD;
    }
}

/*!
    \brief      disable OPA switch
    \param[in]  opax_swy
      \arg        OPA_T3OPA0: T3 switch enable for OPA0
      \arg        OPA_S1OPA0: S1 switch enable for OPA0
      \arg        OPA_S2OPA0: S2 switch enable for OPA0
      \arg        OPA_S3OPA0: S3 switch enable for OPA0
      \arg        OPA_T3OPA1: T3 switch enable for OPA1
      \arg        OPA_S1OPA1: S1 switch enable for OPA1
      \arg        OPA_S2OPA1: S2 switch enable for OPA1
      \arg        OPA_S3OPA1: S3 switch enable for OPA1
      \arg        OPA_S4OPA1: S4 switch enable for OPA1
      \arg        OPA_T3OPA2: T3 switch enable for OPA2
      \arg        OPA_S1OPA2: S3 switch enable for OPA2
      \arg        OPA_S2OPA2: S3 switch enable for OPA2
      \arg        OPA_S3OPA2: S3 switch enable for OPA2
    \param[out] none
    \retval     none
*/
void opa_switch_disable(uint32_t opax_swy)
{
    OPA_CTL &= ~opax_swy;
}

/*!
    \brief      enable OPA in low power mode
    \param[in]  opa_periph
      \arg        OPAx(x =0,1,2)
    \param[out] none
    \retval     none
*/
void opa_low_power_enable(uint32_t opa_periph)
{
    if(OPA0 == opa_periph){
        OPA_CTL &= ~OPA_CTL_OPA0LPM;
    }else if(OPA1 == opa_periph){
        OPA_CTL &= ~OPA_CTL_OPA1LPM;
    }else{
        OPA_CTL &= ~OPA_CTL_OPA2LPM;
    }
}

/*!
    \brief      disable OPA in low power mode
    \param[in]  opa_periph
      \arg        OPAx(x =0,1,2)
    \param[out] none
    \retval     none
*/
void opa_low_power_disable(uint32_t opa_periph)
{
    if(OPA0 == opa_periph){
        OPA_CTL |= OPA_CTL_OPA0LPM;
    }else if(OPA1 == opa_periph){
        OPA_CTL |= OPA_CTL_OPA1LPM;
    }else{
        OPA_CTL |= OPA_CTL_OPA2LPM;
    }
}

/*!
    \brief      set OPA power range
    \param[in]  powerrange
      \arg        OPA_POWRANGE_LOW: Low power range is selected (VDDA is lower than 3.3V)
      \arg        OPA_POWRANGE_HIGH: High power range is selected (VDDA is higher than 3.3V)
    \param[out] none
    \retval     none
*/
void opa_power_range_config(uint32_t powerrange)
{
    OPA_CTL &= ~OPA_CTL_OPA_RANGE;
    OPA_CTL |= powerrange;
}

/*!
    \brief      set OPA bias trimming mode
    \param[in]  opa_trimmode
      \arg        OPA_BT_TRIM_FACTORY: factory trimming values are used for offset calibration
      \arg        OPA_BT_TRIM_USER: user trimming values are used for offset calibration
    \param[out] none
    \retval     none
*/
void opa_trim_mode_set(uint32_t opa_trimmode)
{
    OPA_BT &= ~OPA_BT_OT_USER;
    OPA_BT |= opa_trimmode;
}

/*!
    \brief      set OPA bias trimming value
    \param[in]  opa_periph
      \arg        OPAx(x =0,1,2)
    \param[in]  opa_input
      \arg        OPA_INPUT_P: PMOS input is selected to configure the trimming value
      \arg        OPA_INPUT_N: NMOS input is selected to configure the trimming value
    \param[in]  opa_trimmode
      \arg        this parameter can be any value lower or equal to 0x0000001F.
    \param[out] none
    \retval     none
*/
void opa_trim_value_config(uint32_t opa_periph,uint32_t opa_input,uint32_t opa_trimvalue)
{
    uint32_t bt = 0U, ctl = 0U;
    ctl = OPA_CTL;
    bt  = OPA_BT;

    if(OPA0 == opa_periph){
        /* clear the specified opa calibration for N diff and P diff */
        ctl &= (uint32_t)~(OPA_CTL_OPA0CAL_L | OPA_CTL_OPA0CAL_H);
        /* set the specified opa calibration for N diff or P diff */
        ctl |= opa_input;
        if(OPA_INPUT_P == opa_input){
            /* clear the specified PMOS pairs normal mode 5-bit offset trim value */
            bt &= (~OPA_BT_OA0_TRIM_LOW);
            bt |= (opa_trimvalue);
        }else{
            /* clear the specified NMOS pairs normal mode 5-bit offset trim value */
            bt &= (~OPA_BT_OA0_TRIM_HIGH);
            bt |= (opa_trimvalue << 5U);
        }

    }else if(OPA1 == opa_periph){
        ctl &= (uint32_t)~(OPA_CTL_OPA1CAL_L | OPA_CTL_OPA1CAL_H);
        ctl |= (uint32_t)(opa_input << 8U);
        if(OPA_INPUT_P == opa_input){
            /* clear the specified PMOS pairs normal mode 5-bit offset trim value */
            bt &= (~OPA_BT_OA1_TRIM_LOW);
            bt |= (opa_trimvalue << 10U);
        }else{
            /* clear the specified NMOS pairs normal mode 5-bit offset trim value */
            bt &= (~OPA_BT_OA1_TRIM_HIGH);
            bt |= (opa_trimvalue << 15U);
        }
    }else{
        ctl &= (uint32_t)~(OPA_CTL_OPA2CAL_L | OPA_CTL_OPA2CAL_H);
        ctl |= (uint32_t)(opa_input << 16U);
        if(OPA_INPUT_P == opa_input){
            /* clear the specified PMOS pairs normal mode 5-bit offset trim value */
            bt &= (~OPA_BT_OA2_TRIM_LOW);
            bt |= (opa_trimvalue << 20U);
        }else{
            /* clear the specified NMOS pairs normal mode 5-bit offset trim value */
            bt &= (~OPA_BT_OA2_TRIM_HIGH);
            bt |= (opa_trimvalue << 25U);
        }
    }

    OPA_CTL = ctl;
    OPA_BT  = bt;
}

/*!
    \brief      set OPA bias trimming value low power
    \param[in]  opa_periph
      \arg        OPAx(x =0,1,2)
    \param[in]  opa_input
      \arg        OPA_INPUT_P: PMOS input is selected to configure the trimming value
      \arg        OPA_INPUT_N: NMOS input is selected to configure the trimming value
    \param[in]  opa_trimmode
      \arg        this parameter can be any value lower or equal to 0x0000001F.
    \param[out] none
    \retval     none
*/
void opa_trim_value_lp_config(uint32_t opa_periph,uint32_t opa_input,uint32_t opa_trimvalue)
{
    uint32_t lpbt = 0U, ctl  = 0U;
    ctl  = OPA_CTL;
    lpbt = OPA_LPBT;

    if(OPA0 == opa_periph){
        ctl &= (uint32_t)~(OPA_CTL_OPA0CAL_L | OPA_CTL_OPA0CAL_H);
        ctl |= opa_input;
        if(OPA_INPUT_P == opa_input){
            /* clear the specified PMOS pairs low power mode 5-bit offset trim value */
            lpbt &= (~OPA_LPBT_OA0_TRIM_LOW);
            lpbt |= (opa_trimvalue);
        }else{
            /* clear the specified NMOS pairs low power mode 5-bit offset trim value */
            lpbt &= (~OPA_LPBT_OA0_TRIM_HIGH);
            lpbt |= (opa_trimvalue << 5U);
        }
    }else if (OPA1 == opa_periph){
        ctl &= (uint32_t)~(OPA_CTL_OPA0CAL_L | OPA_CTL_OPA0CAL_H);
        ctl |= (uint32_t)(opa_input << 8U);
        if(OPA_INPUT_P == opa_input){
            /* clear the specified PMOS pairs low power mode 5-bit offset trim value */
            lpbt &= (~OPA_LPBT_OA1_TRIM_LOW);
            lpbt |= (opa_trimvalue << 10U);
        }else{
            /* clear the specified NMOS pairs low power mode 5-bit offset trim value */
            lpbt &= (~OPA_LPBT_OA1_TRIM_HIGH);
            lpbt |= (opa_trimvalue << 15U);
        }

    }else{
        ctl &= (uint32_t)~(OPA_CTL_OPA2CAL_L | OPA_CTL_OPA2CAL_H);
        ctl |= (uint32_t)(opa_input << 16U);
        if(OPA_INPUT_P == opa_input){
            /* clear the specified PMOS pairs low power mode 5-bit offset trim value */
            lpbt &= (~OPA_LPBT_OA2_TRIM_LOW);
            lpbt |= (opa_trimvalue << 20U);
        }else{
            /* clear the specified NMOS pairs low power mode 5-bit offset trim value */
            lpbt &= (~OPA_LPBT_OA2_TRIM_HIGH);
            lpbt |= (opa_trimvalue << 25U);
        }
    }

    OPA_CTL  = ctl;
    OPA_LPBT = lpbt;
}

/*!
    \brief      get OPA calibration flag
    \param[in]  opa_periph
      \arg        OPAx(x =0,1,2)
    \param[out] none
    \retval     The state of the OPA calibration flag (SET or RESET)
*/
FlagStatus opa_cal_out_get(uint32_t opa_periph)
{
    uint32_t data = 0U;
    FlagStatus bitstatus = RESET;
    data = OPA_CTL;

    if(OPA0 == opa_periph){
        /* get opa0 calibration output bit status */
        if ((uint32_t)RESET != (data & OPA_CTL_OPA1CALOUT)){
            bitstatus = SET;
        }else{
            bitstatus = RESET;
        }
    }else if(OPA1 == opa_periph){
        /* get opa1 calibration output bit status */
        if ((uint32_t)RESET != (data & OPA_CTL_OPA1CALOUT)){
            bitstatus = SET;
        }else{
            bitstatus = RESET;
        }
    }else{
        /* get opa2 calibration output bit status */
        if((uint32_t)RESET != (data & OPA_CTL_OPA1CALOUT)){
            bitstatus = SET;
        }else{
            bitstatus = RESET;
        }
    }
    return bitstatus;
}

#endif  /* GD32F170_190 */
