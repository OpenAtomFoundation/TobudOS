/*!
    \file  gd32f1x0_exti.c
    \brief EXTI driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0_exti.h"

#define EXTI_INTEN_RESET_VAL    0x0F900000U
#define EXTI_EVEN_RESET_VAL     0x00000000U
#define EXTI_RTEN_RESET_VAL     0x00000000U
#define EXTI_FTEN_RESET_VAL     0x00000000U
#define EXTI_SWIEV_RESET_VAL    0x00000000U

/*!
    \brief      deinitialize the EXTI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exti_deinit(void)
{
    /* reset the value of the EXTI registers */
    EXTI_INTEN = EXTI_INTEN_RESET_VAL;
    EXTI_EVEN  = EXTI_EVEN_RESET_VAL;
    EXTI_RTEN  = EXTI_RTEN_RESET_VAL;
    EXTI_FTEN  = EXTI_FTEN_RESET_VAL;
    EXTI_SWIEV = EXTI_SWIEV_RESET_VAL;
}

/*!
    \brief      initialize the EXTI
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[in]  mode: interrupt or event mode, refer to exti_mode_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_INTERRUPT: interrupt mode
      \arg        EXTI_EVENT: event mode
    \param[in]  trig_type: interrupt trigger type, refer to exti_trig_type_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_TRIG_RISING: rising edge trigger
      \arg        EXTI_TRIG_FALLING: falling trigger
      \arg        EXTI_TRIG_BOTH: rising and falling trigger
    \param[out] none
    \retval     none
*/
void exti_init(exti_line_enum linex, \
                exti_mode_enum mode, \
                exti_trig_type_enum trig_type)
{
    /* reset the EXTI line x */
    EXTI_INTEN &= ~(uint32_t)linex;
    EXTI_EVEN &= ~(uint32_t)linex;
    EXTI_RTEN &= ~(uint32_t)linex;
    EXTI_FTEN &= ~(uint32_t)linex;
    
    /* set the EXTI mode and enable the interrupts or events from EXTI line x */
    switch(mode){
    case EXTI_INTERRUPT:
        EXTI_INTEN |= (uint32_t)linex;
        break;
    case EXTI_EVENT:
        EXTI_EVEN |= (uint32_t)linex;
        break;
    default:
        break;
    }
    
    /* set the EXTI trigger type */
    switch(trig_type){
    case EXTI_TRIG_RISING:
        EXTI_RTEN |= (uint32_t)linex;
        EXTI_FTEN &= ~(uint32_t)linex;
        break;
    case EXTI_TRIG_FALLING:
        EXTI_RTEN &= ~(uint32_t)linex;
        EXTI_FTEN |= (uint32_t)linex;
        break;
    case EXTI_TRIG_BOTH:
        EXTI_RTEN |= (uint32_t)linex;
        EXTI_FTEN |= (uint32_t)linex;
        break;
    default:
        break;
    }
}

/*!
    \brief      enable the interrupts from EXTI line x
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     none
*/
void exti_interrupt_enable(exti_line_enum linex)
{
    EXTI_INTEN |= (uint32_t)linex;
}

/*!
    \brief      enable the events from EXTI line x
    \param[in]  linex: EXTI line number, refer to exti_line_enum 
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     none
*/
void exti_event_enable(exti_line_enum linex)
{
    EXTI_EVEN |= (uint32_t)linex;
}

/*!
    \brief      disable the interrupt from EXTI line x
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     none
*/
void exti_interrupt_disable(exti_line_enum linex)
{
    EXTI_INTEN &= ~(uint32_t)linex;
}

/*!
    \brief      disable the events from EXTI line x
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     none
*/
void exti_event_disable(exti_line_enum linex)
{
    EXTI_EVEN &= ~(uint32_t)linex;
}

/*!
    \brief      get EXTI lines flag
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     FlagStatus: status of flag (RESET or SET)
*/
FlagStatus exti_flag_get(exti_line_enum linex)
{
    if(RESET != (EXTI_PD & (uint32_t)linex)){
        return SET;
    }else{
        return RESET;
    } 
}

/*!
    \brief      clear EXTI lines pending flag
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     none
*/
void exti_flag_clear(exti_line_enum linex)
{
    EXTI_PD = (uint32_t)linex;
}

/*!
    \brief      get EXTI lines flag when the interrupt flag is set
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     FlagStatus: status of flag (RESET or SET)
*/
FlagStatus exti_interrupt_flag_get(exti_line_enum linex)
{
    uint32_t flag_left, flag_right;
    flag_left = EXTI_PD & (uint32_t)linex;
    flag_right = EXTI_INTEN & (uint32_t)linex;
    if((RESET != flag_left) && (RESET != flag_right)){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear EXTI lines pending flag
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     none
*/
void exti_interrupt_flag_clear(exti_line_enum linex)
{
    EXTI_PD = (uint32_t)linex;
}

/*!
    \brief      enable EXTI software interrupt event
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     none
*/
void exti_software_interrupt_enable(exti_line_enum linex)
{
    EXTI_SWIEV |= (uint32_t)linex;
}

/*!
    \brief      disable EXTI software interrupt event
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
      \arg        EXTI_x (x=0..17,19,21,22,25,27): EXTI line x (for GD32F130xx and GD32F150xx devices)
    \param[out] none
    \retval     none
*/
void exti_software_interrupt_disable(exti_line_enum linex)
{
    EXTI_SWIEV &= ~(uint32_t)linex;
}
