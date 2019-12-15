/*!
    \file  gd32f1x0_ivref.c
    \brief IVREF driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifdef GD32F170_190

#include "gd32f1x0_ivref.h"

/*!
    \brief      deinit IVREF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ivref_deinit(void)
{
    rcu_periph_reset_enable(RCU_OPAIVREFRST);
    rcu_periph_reset_disable(RCU_OPAIVREFRST);
}

/*!
    \brief      enable VREF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void vref_enable(void)
{
    IVREF_CTL |= IVREF_CTL_VREN;
}

/*!
    \brief      disable VREF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void vref_disable(void)
{
    IVREF_CTL &= ~IVREF_CTL_VREN;
}

/*!
    \brief      set VREF mode
    \param[in]  vrefmode
      \arg        VREF_CONNECT_EXTERNAL_CAP: vref connect external capacitor
      \arg        VREF_DISCONNECT_EXTERNAL_CAP: vref disconnect external capacitor
    \param[out] none
    \retval     none
*/
void vref_mode_set(uint32_t vrefmode)
{
    uint32_t ctl = 0U;
    ctl = IVREF_CTL;
    /* clear voltage reference enable bits */
    ctl &= ~IVREF_CTL_DECAP;
    ctl |= vrefmode;
    IVREF_CTL = ctl;
}

/*!
    \brief      set VREF voltage precision trim.
    \param[in]  precisiontrim
      \arg        VREF_VOLT_PRECISION_TRIM_X(x=0..31): (-6.4+ 0.4*x)%
    \param[out] none
    \retval     none
*/
void vref_precision_trim_value_set(uint32_t precisiontrim)
{
    uint32_t ctl = 0U;
    ctl = IVREF_CTL;
    ctl &= ~IVREF_CTL_VPT;
    ctl |= precisiontrim;
    IVREF_CTL = ctl;
}

/*!
    \brief      enable IREF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void iref_enable(void)
{
    IVREF_CTL |= IVREF_CTL_CREN;
}

/*!
    \brief      disable IREF
    \param[in]  none
    \param[out] none
    \retval     none
*/
void iref_disable(void)
{
    IVREF_CTL &= ~IVREF_CTL_CREN;
}

/*!
    \brief      set IREF mode
    \param[in]  irefmode
      \arg        IREF_MODE_LOW_POWER: 1uA step
      \arg        IREF_MODE_HIGH_CURRENT: 8uA step
    \param[out] none
    \retval     none
*/
void iref_mode_set(uint32_t irefmode)
{
    uint32_t ctl = 0U;
    ctl = IVREF_CTL;
    ctl &= ~IVREF_CTL_SSEL;
    ctl |= irefmode;
    IVREF_CTL = ctl;
}

/*!
    \brief      set IREF precision_trim_value
    \param[in]  precisiontrim
      \arg        IREF_CUR_PRECISION_TRIM_X(x=0..31): (-15+ x)%
    \param[out] none
    \retval     none
*/
void iref_precision_trim_value_set(uint32_t precisiontrim)
{
    uint32_t ctl = 0U;
    ctl = IVREF_CTL;
    ctl &= ~IVREF_CTL_CPT;
    ctl |= precisiontrim;
    IVREF_CTL = ctl;
}

/*!
    \brief      set IREF sink mode
    \param[in]  irefsinkmode
      \arg        IREF_SOURCE_CURRENT : source current.
      \arg        IREF_SINK_CURRENT: sink current
    \param[out] none
    \retval     none
*/
void iref_sink_set(uint32_t irefsinkmode)
{
    uint32_t ctl = 0U;
    ctl = IVREF_CTL;
    /* clear sink current mode bits */
    ctl &= ~IVREF_CTL_SCMOD;
    /* set sink current mode bits */
    ctl |= irefsinkmode;
    IVREF_CTL = ctl;
}

/*!
    \brief      set IREF step data 
    \param[in]  irefstepdata
      \arg        IREF_CUR_STEP_DATA_X:(x=0..63): step*x
    \param[out] none
    \retval     none
*/
void iref_step_data_config(uint32_t irefstepdata)
{
    uint32_t ctl = 0U;
    /* get ctl value */
    ctl = IVREF_CTL;
    /* clear current step data bits */
    ctl &= ~IVREF_CTL_CSDT;
    /* set current step data bits */
    ctl |= irefstepdata;
    IVREF_CTL = ctl;
}

#endif  /* GD32F170_190 */
