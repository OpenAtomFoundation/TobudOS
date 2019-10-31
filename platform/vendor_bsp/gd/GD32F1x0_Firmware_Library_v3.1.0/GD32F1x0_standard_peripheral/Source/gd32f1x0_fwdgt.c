/*!
    \file  gd32f1x0_fwdgt.c
    \brief FWDGT driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0_fwdgt.h"

/*!
    \brief      disable write access to FWDGT_PSC,FWDGT_RLD and FWDGT_WND
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fwdgt_write_disable(void)
{
    FWDGT_CTL = FWDGT_WRITEACCESS_DISABLE;
}

/*!
    \brief      reload the counter of FWDGT
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fwdgt_counter_reload(void)
{
    FWDGT_CTL = FWDGT_KEY_RELOAD;
}

/*!
    \brief      start the free watchdog timer counter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fwdgt_enable(void)
{
    FWDGT_CTL = FWDGT_KEY_ENABLE;
}

/*!
    \brief      configure the free watchdog timer counter window value
    \param[in]  window_value: specify window value(0x0000 - 0x0FFF)
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus fwdgt_window_value_config(uint16_t window_value)
{
    uint32_t time_index = FWDGT_WND_TIMEOUT;
    uint32_t flag_status = RESET;

    /* enable write access to FWDGT_WND */
    FWDGT_CTL = FWDGT_WRITEACCESS_ENABLE;

    /* wait until the WUD flag to be reset */
    do{
        flag_status = FWDGT_STAT & FWDGT_STAT_WUD;
    }while((--time_index > 0U) && ((uint32_t)RESET != flag_status));

    if ((uint32_t)RESET != flag_status){
        return ERROR; 
    }
    
    FWDGT_WND = WND_WND(window_value);

    return SUCCESS;
}

/*!
    \brief      configure counter reload value, and prescaler divider value
    \param[in]  reload_value: specify reload value(0x0000 - 0x0FFF)
    \param[in]  prescaler_div: FWDGT prescaler value
      \arg        FWDGT_PSC_DIV4: FWDGT prescaler set to 4
      \arg        FWDGT_PSC_DIV8: FWDGT prescaler set to 8
      \arg        FWDGT_PSC_DIV16: FWDGT prescaler set to 16
      \arg        FWDGT_PSC_DIV32: FWDGT prescaler set to 32
      \arg        FWDGT_PSC_DIV64: FWDGT prescaler set to 64
      \arg        FWDGT_PSC_DIV128: FWDGT prescaler set to 128
      \arg        FWDGT_PSC_DIV256: FWDGT prescaler set to 256
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus fwdgt_config(uint16_t reload_value, uint8_t prescaler_div)
{
    uint32_t timeout = FWDGT_PSC_TIMEOUT;
    uint32_t flag_status = RESET;
  
    /* enable write access to FWDGT_PSC,and FWDGT_RLD */
    FWDGT_CTL = FWDGT_WRITEACCESS_ENABLE;
  
    /* wait until the PUD flag to be reset */
    do{
        flag_status = FWDGT_STAT & FWDGT_STAT_PUD;
    }while((--timeout > 0U) && ((uint32_t)RESET != flag_status));
    
    if ((uint32_t)RESET != flag_status){
        return ERROR;
    }
    
    /* configure FWDGT */
    FWDGT_PSC = (uint32_t)prescaler_div;       

    timeout = FWDGT_RLD_TIMEOUT;    
    /* wait until the RUD flag to be reset */
    do{
        flag_status = FWDGT_STAT & FWDGT_STAT_RUD;
    }while((--timeout > 0U) && ((uint32_t)RESET != flag_status));
   
    if ((uint32_t)RESET != flag_status){
        return ERROR;
    }
    
    FWDGT_RLD = RLD_RLD(reload_value);
    
    /* reload the counter */
    FWDGT_CTL = FWDGT_KEY_RELOAD;
    
    return SUCCESS;
}

/*!
    \brief      get flag state of FWDGT
    \param[in]  flag: flag to get 
      \arg        FWDGT_FLAG_PUD: a write operation to FWDGT_PSC register is on going
      \arg        FWDGT_FLAG_RUD: a write operation to FWDGT_RLD register is on going
      \arg        FWDGT_FLAG_WUD: a write operation to FWDGT_WND register is on going
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus fwdgt_flag_get(uint16_t flag)
{
    if(FWDGT_STAT & flag){
        return SET;
    }
    
    return RESET;
}
