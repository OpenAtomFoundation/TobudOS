/*!
    \file  gd32e10x_fmc.c
    \brief FMC driver
        
    \version 2017-12-26, V1.0.0, firmware for GD32E10x
*/

/*
    Copyright (c) 2017, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#include "gd32e10x_fmc.h"

/* FMC register bit offset */
#define OB_WP1_WP1_OFFSET                 ((uint32_t)8U)
#define OB_WP2_WP2_OFFSET                 ((uint32_t)16U)
#define OB_WP3_WP3_OFFSET                 ((uint32_t)24U)
#define FMC_OBSTAT_USER_OFFSET            ((uint32_t)2U)
#define FMC_OBSTAT_DATA_OFFSET            ((uint32_t)10U)

/* return the FMC state */
static fmc_state_enum fmc_state_get(void);
/* check FMC ready or not */
static fmc_state_enum fmc_ready_wait(uint32_t timeout);

/*!
    \brief      set the wait state counter value
    \param[in]  wscnt:wait state counter value
                only one parameter can be selected which is shown as below:
      \arg        FMC_WAIT_STATE_0: FMC 0 wait
      \arg        FMC_WAIT_STATE_1: FMC 1 wait
      \arg        FMC_WAIT_STATE_2: FMC 2 wait
      \arg        FMC_WAIT_STATE_3: FMC 3 wait
    \param[out] none
    \retval     none
*/
void fmc_wscnt_set(uint32_t wscnt)
{
    uint32_t ws;
    
    ws = FMC_WS;
    /* set the wait state counter value */
    ws &= ~FMC_WS_WSCNT;
    FMC_WS = (ws | wscnt);
}

/*!
    \brief      enable pre-fetch
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_prefetch_enable(void)
{
    FMC_WS |= FMC_WS_PFEN;
}

/*!
    \brief      disable pre-fetch
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_prefetch_disable(void)
{
    FMC_WS &= ~FMC_WS_PFEN;
}

/*!
    \brief      enable IBUS cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_ibus_enable(void)
{
    FMC_WS |= FMC_WS_ICEN;
}

/*!
    \brief      disable IBUS cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_ibus_disable(void)
{
    FMC_WS &= ~FMC_WS_ICEN;
}

/*!
    \brief      enable DBUS cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_dbus_enable(void)
{
    FMC_WS |= FMC_WS_DCEN;
}

/*!
    \brief      disable DBUS cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_dbus_disable(void)
{
    FMC_WS &= ~FMC_WS_DCEN;
}

/*!
    \brief      reset IBUS cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_ibus_reset(void)
{
    FMC_WS |= FMC_WS_ICRST;
}

/*!
    \brief      reset DBUS cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_dbus_reset(void)
{
    FMC_WS |= FMC_WS_DCRST;
}

/*!
    \brief      set program width to flash memory
    \param[in]  pgw
                only one parameter can be selected which is shown as below:
      \arg        FMC_PROG_W_32B: 32-bit program width to flash memory
      \arg        FMC_PROG_W_64B: 64-bit program width to flash memory
    \param[out] none
    \retval     none
*/
void fmc_program_width_set(uint32_t pgw)
{
    uint32_t ws = 0U;
    
    /* configure program width to flash memory */
    ws = FMC_WS;
    ws &= ~FMC_WS_PGW;
    ws |= pgw;
    FMC_WS = ws;
}

/*!
    \brief      unlock the main FMC operation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_unlock(void)
{
    if(RESET != (FMC_CTL & FMC_CTL_LK)){
        /* write the FMC unlock key */
        FMC_KEY = UNLOCK_KEY0;
        FMC_KEY = UNLOCK_KEY1;
    }
}

/*!
    \brief      lock the main FMC operation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_lock(void)
{
    /* set the LK bit */
    FMC_CTL |= FMC_CTL_LK; 
}

/*!
    \brief      erase page
    \param[in]  page_address: the page address to be erased
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_page_erase(uint32_t page_address)
{
    fmc_state_enum fmc_state;
    
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
    /* if the last operation is completed, start page erase */
    if(FMC_READY == fmc_state){
        FMC_CTL |= FMC_CTL_PER;
        FMC_ADDR = page_address;
        FMC_CTL |= FMC_CTL_START;
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
        /* reset the PER bit */
        FMC_CTL &= ~FMC_CTL_PER;
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      erase whole chip
    \param[in]  none
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_mass_erase(void)
{
    fmc_state_enum fmc_state;
    
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
    if(FMC_READY == fmc_state){
        /* start whole chip erase */
        FMC_CTL |= FMC_CTL_MER;
        FMC_CTL |= FMC_CTL_START;
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
        /* reset the MER bit */
        FMC_CTL &= ~FMC_CTL_MER;
    }
    /* return the FMC state  */
    return fmc_state;
}

/*!
    \brief      program a double word at the corresponding address
    \param[in]  address: address to program
    \param[in]  data: double word to program
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_doubleword_program(uint32_t address, uint64_t data)
{
    fmc_state_enum fmc_state = FMC_READY;
        
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT); 
  
    if(FMC_READY == fmc_state){
        /* set the PG bit to start program */
        FMC_CTL |= FMC_CTL_PG;
        *(__IO uint64_t*)(address) = data;
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
        /* reset the PG bit */
        FMC_CTL &= ~FMC_CTL_PG;
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      program a word at the corresponding address
    \param[in]  address: address to program
    \param[in]  data: word to program
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_word_program(uint32_t address, uint32_t data)
{
    fmc_state_enum fmc_state = FMC_READY;

    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT); 
  
    if(FMC_READY == fmc_state){
        /* set the PG bit to start program */
        FMC_CTL |= FMC_CTL_PG;
        REG32(address) = data;
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
        /* reset the PG bit */
        FMC_CTL &= ~FMC_CTL_PG;
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      program a half word at the corresponding address
    \param[in]  address: address to program
    \param[in]  data: halfword to program
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_halfword_program(uint32_t address, uint16_t data)
{
    fmc_state_enum fmc_state = FMC_READY;

    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT); 
  
    if(FMC_READY == fmc_state){
        /* set the PG bit to start program */
        FMC_CTL |= FMC_CTL_PG;
        REG16(address) = data;
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
        /* reset the PG bit */
        FMC_CTL &= ~FMC_CTL_PG;
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      unlock the option bytes operation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ob_unlock(void)
{
    if(RESET == (FMC_CTL & FMC_CTL_OBWEN)){
        /* write the FMC key */
        FMC_OBKEY = UNLOCK_KEY0;
        FMC_OBKEY = UNLOCK_KEY1;
    }
}

/*!
    \brief      lock the option bytes operation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ob_lock(void)
{
    /* reset the OBWEN bit */
    FMC_CTL &= ~FMC_CTL_OBWEN;
}

/*!
    \brief      erase the FMC option bytes
                unlock the FMC_CTL and option bytes before calling this function
    \param[in]  none
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum ob_erase(void)
{
    uint16_t temp_spc = FMC_NSPC;
    uint32_t temp;

    fmc_state_enum fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    /* check the option bytes security protection value */
    if(RESET != ob_security_protection_flag_get()){
        temp_spc = FMC_USPC;
    }

    if(FMC_READY == fmc_state){

        /* start erase the option bytes */
        FMC_CTL |= FMC_CTL_OBER;
        FMC_CTL |= FMC_CTL_START;

        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
    
        if(FMC_READY == fmc_state){
            /* reset the OBER bit */
            FMC_CTL &= ~FMC_CTL_OBER;
            /* set the OBPG bit */
            FMC_CTL |= FMC_CTL_OBPG;
            temp = ((uint32_t)temp_spc)|0xffff0000;
            /* no security protection */
            OB_SPC = temp; 
            /* wait for the FMC ready */
            fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT); 
            if(FMC_TOERR != fmc_state){
                /* reset the OBPG bit */
                FMC_CTL &= ~FMC_CTL_OBPG;
            }
        }else{
            if(FMC_TOERR != fmc_state){
                /* reset the OBPG bit */
                FMC_CTL &= ~FMC_CTL_OBPG;
            }
        }
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      enable write protection
    \param[in]  ob_wp: specify sector to be write protected
                one or more parameters can be selected which are shown as below:
      \arg        OB_WP_ALL: write protect all sector
      \arg        OB_WPx(x=0..31): write protect specify sector
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum ob_write_protection_enable(uint32_t ob_wp)
{
    uint16_t temp_wp0, temp_wp1, temp_wp2, temp_wp3;
    uint32_t temp1,temp2;

    fmc_state_enum fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    ob_wp = (uint32_t)(~ob_wp);
    temp_wp0 = (uint16_t)(ob_wp & OB_WP0_WP0);
    temp_wp1 = (uint16_t)((ob_wp & OB_WP1_WP1) >> OB_WP1_WP1_OFFSET);
    temp_wp2 = (uint16_t)((ob_wp & OB_WP2_WP2) >> OB_WP2_WP2_OFFSET);
    temp_wp3 = (uint16_t)((ob_wp & OB_WP3_WP3) >> OB_WP3_WP3_OFFSET);
    
    temp1 = (((uint32_t)temp_wp1)<<16) | ((uint32_t)temp_wp0);
    temp2 = (((uint32_t)temp_wp3)<<16) | ((uint32_t)temp_wp2);

    if(FMC_READY == fmc_state){
    
        /* set the OBPG bit*/
        FMC_CTL |= FMC_CTL_OBPG;

        if(0xFFFFU != temp1){
            OB_WP0 = temp1;
      
            /* wait for the FMC ready */
            fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
        }
        if(0xFFFFU != temp2){
            OB_WP2 = temp2;
      
            /* wait for the FMC ready */
            fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
        }
        if(FMC_TOERR != fmc_state){
            /* reset the OBPG bit */
            FMC_CTL &= ~FMC_CTL_OBPG;
        }
    } 
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      configure security protection
    \param[in]  ob_spc: specify security protection
                only one parameter can be selected which is shown as below:
      \arg        FMC_NSPC: no security protection
      \arg        FMC_USPC: under security protection
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum ob_security_protection_config(uint8_t ob_spc)
{
    uint16_t temp_spc;
    uint32_t temp;
    fmc_state_enum fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state){
        FMC_CTL |= FMC_CTL_OBER;
        FMC_CTL |= FMC_CTL_START;
    
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
    
        if(FMC_READY == fmc_state){
            /* reset the OBER bit */
            FMC_CTL &= ~FMC_CTL_OBER;
      
            /* start the option bytes program */
            FMC_CTL |= FMC_CTL_OBPG;
            temp_spc = (uint16_t)ob_spc;
            temp = ((uint32_t)temp_spc)|0xffff0000;
            /* config security protection */
            OB_SPC = temp; 

            /* wait for the FMC ready */
            fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT); 
    
            if(FMC_TOERR != fmc_state){
                /* reset the OBPG bit */
                FMC_CTL &= ~FMC_CTL_OBPG;
            }
        }else{
            if(FMC_TOERR != fmc_state){
                /* reset the OBER bit */
                FMC_CTL &= ~FMC_CTL_OBER;
            }
        }
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      program the FMC user option bytes 
    \param[in]  ob_fwdgt: option bytes free watchdog value
                only one parameter can be selected which is shown as below:
      \arg        OB_FWDGT_SOFTWARE: software free watchdog
      \arg        OB_FWDGT_HARDWARE: hardware free watchdog
    \param[in]  ob_deepsleep: option bytes deepsleep reset value
                only one parameter can be selected which is shown as below:
      \arg        OB_DEEPSLEEP_NO_RST: no reset when entering deepsleep mode
      \arg        OB_DEEPSLEEP_RST: generate a reset instead of entering deepsleep mode 
    \param[in]  ob_stdby:option bytes standby reset value
                only one parameter can be selected which is shown as below:
      \arg        OB_STDBY_NO_RST: no reset when entering standby mode
      \arg        OB_STDBY_RST: generate a reset instead of entering standby mode
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum ob_user_write(uint8_t ob_fwdgt, uint8_t ob_deepsleep, uint8_t ob_stdby)
{
    fmc_state_enum fmc_state = FMC_READY;
    uint8_t temp;

    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
  
    if(FMC_READY == fmc_state){
        /* set the OBPG bit*/
        FMC_CTL |= FMC_CTL_OBPG; 

        temp = ((uint8_t)((uint8_t)((uint8_t)(ob_fwdgt) | ob_deepsleep) | ob_stdby) | (OB_USER_MASK));
        OB_USER = temp;
    
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

        if(FMC_TOERR != fmc_state){
            /* reset the OBPG bit */
            FMC_CTL &= ~FMC_CTL_OBPG;
        }
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      program option bytes data
    \param[in]  address: the option bytes address to be programmed
    \param[in]  data: the byte to be programmed
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_PGERR: program error
      \arg        FMC_PGAERR: program alignment error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum ob_data_program(uint32_t address, uint8_t data)
{
    fmc_state_enum fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state){
        /* set the OBPG bit */
        FMC_CTL |= FMC_CTL_OBPG; 
        REG16(address) = data;
    
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
    
        if(FMC_TOERR != fmc_state){
            /* reset the OBPG bit */
            FMC_CTL &= ~FMC_CTL_OBPG;
        }
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      get the FMC user option bytes
    \param[in]  none
    \param[out] none
    \retval     the FMC user option bytes values
*/
uint8_t ob_user_get(void)
{
    /* return the FMC user option bytes value */
    return (uint8_t)(FMC_OBSTAT >> FMC_OBSTAT_USER_OFFSET);
}

/*!
    \brief      get OB_DATA in register FMC_OBSTAT
    \param[in]  none
    \param[out] none
    \retval     ob_data
*/
uint16_t ob_data_get(void)
{
    return (uint16_t)(FMC_OBSTAT >> FMC_OBSTAT_DATA_OFFSET);
}

/*!
    \brief      get the FMC option bytes write protection
    \param[in]  none
    \param[out] none
    \retval     the FMC write protection option bytes value
*/
uint32_t ob_write_protection_get(void)
{
    /* return the FMC write protection option bytes value */
    return FMC_WP;
}

/*!
    \brief      get the FMC option bytes security protection
    \param[in]  none
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus ob_security_protection_flag_get(void)
{
    FlagStatus spc_state = RESET;

    if(RESET != (FMC_OBSTAT & FMC_OBSTAT_SPC)){
        spc_state = SET;
    }else{
        spc_state = RESET;
    }
    return spc_state;
}

/*!
    \brief      enable FMC interrupt
    \param[in]  interrupt: the FMC interrupt source
                only one parameter can be selected which is shown as below:
      \arg        FMC_INT_BANK_END: enable FMC end of program interrupt
      \arg        FMC_INT_BANK_ERR: enable FMC error interrupt
    \param[out] none
    \retval     none
*/
void fmc_interrupt_enable(uint32_t interrupt)
{
    FMC_REG_VAL(interrupt) |= BIT(FMC_BIT_POS(interrupt));
}

/*!
    \brief      disable FMC interrupt
    \param[in]  interrupt: the FMC interrupt source
                only one parameter can be selected which is shown as below:
      \arg        FMC_INT_END: enable FMC end of program interrupt
      \arg        FMC_INT_ERR: enable FMC error interrupt
    \param[out] none
    \retval     none
*/
void fmc_interrupt_disable(uint32_t interrupt)
{
    FMC_REG_VAL(interrupt) &= ~BIT(FMC_BIT_POS(interrupt));
}

/*!
    \brief      check flag is set or not
    \param[in]  flag: check FMC flag
                only one parameter can be selected which is shown as below:
      \arg        FMC_FLAG_BUSY: FMC busy flag bit
      \arg        FMC_FLAG_PGERR: FMC operation error flag bit
      \arg        FMC_FLAG_PGAERR: FMC program alignment error flag bit
      \arg        FMC_FLAG_WPERR: FMC erase/program protection error flag bit
      \arg        FMC_FLAG_END: FMC end of operation flag bit
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus fmc_flag_get(uint32_t flag)
{
    if(RESET != (FMC_REG_VAL(flag) & BIT(FMC_BIT_POS(flag)))){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear the FMC flag
    \param[in]  flag: clear FMC flag
                only one parameter can be selected which is shown as below:
      \arg        FMC_FLAG_PGERR: FMC operation error flag bit
      \arg        FMC_FLAG_PGAERR: FMC program alignment error flag bit
      \arg        FMC_FLAG_WPERR: FMC erase/program protection error flag bit
      \arg        FMC_FLAG_END: FMC end of operation flag bit
    \param[out] none
    \retval     none
*/
void fmc_flag_clear(uint32_t flag)
{
    FMC_REG_VAL(flag) |= BIT(FMC_BIT_POS(flag));
}

/*!
    \brief      get FMC interrupt flag state
    \param[in]  flag: FMC interrupt flags, refer to fmc_interrupt_flag_enum
                only one parameter can be selected which is shown as below:
      \arg        FMC_INT_FLAG_PGERR: FMC operation error interrupt flag bit
      \arg        FMC_FLAG_PGAERR: FMC program alignment error flag bit
      \arg        FMC_INT_FLAG_WPERR: FMC erase/program protection error interrupt flag bit
      \arg        FMC_INT_FLAG_END: FMC end of operation interrupt flag bit
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus fmc_interrupt_flag_get(fmc_interrupt_flag_enum flag)
{
    FlagStatus ret1 = RESET;
    FlagStatus ret2 = RESET;
    
    if(FMC_STAT_REG_OFFSET == FMC_REG_OFFSET_GET(flag)){
        /* get the staus of interrupt flag */
        ret1 = (FlagStatus)(FMC_REG_VALS(flag) & BIT(FMC_BIT_POS0(flag)));
        /* get the staus of interrupt enale bit */
        ret2 = (FlagStatus)(FMC_CTL & BIT(FMC_BIT_POS1(flag)));
    }

    if(ret1 && ret2){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear FMC interrupt flag state
    \param[in]  flag: FMC interrupt flags, refer to can_interrupt_flag_enum
                only one parameter can be selected which is shown as below:
      \arg        FMC_INT_FLAG_PGERR: FMC operation error interrupt flag bit
      \arg        FMC_FLAG_PGAERR: FMC program alignment error flag bit
      \arg        FMC_INT_FLAG_WPERR: FMC erase/program protection error interrupt flag bit
      \arg        FMC_INT_FLAG_END: FMC end of operation interrupt flag bit
    \param[out] none
    \retval     none
*/
void fmc_interrupt_flag_clear(fmc_interrupt_flag_enum flag)
{
    FMC_REG_VALS(flag) |= BIT(FMC_BIT_POS0(flag));
}

/*!
    \brief      get the FMC state
    \param[in]  none
    \param[out] none
    \retval     state of FMC, refer to fmc_state_enum
*/
static fmc_state_enum fmc_state_get(void)
{
    fmc_state_enum fmc_state = FMC_READY;
  
    if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_BUSY)){
        fmc_state = FMC_BUSY;
    }else{
        if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_WPERR)){
            fmc_state = FMC_WPERR;
        }else{
            if((uint32_t)0x00U != (FMC_STAT & (FMC_STAT_PGERR))){
                fmc_state = FMC_PGERR; 
            }else{
                if((uint32_t)0x00U != (FMC_STAT & (FMC_STAT_PGAERR))){
                    fmc_state = FMC_PGAERR; 
                }
            }
        }
  }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      check whether FMC is ready or not
    \param[in]  timeout: count of loop
    \param[out] none
    \retval     state of FMC, refer to fmc_state_enum
*/
static fmc_state_enum fmc_ready_wait(uint32_t timeout)
{
    fmc_state_enum fmc_state = FMC_BUSY;
  
    /* wait for FMC ready */
    do{
        /* get FMC state */
        fmc_state = fmc_state_get();
        timeout--;
    }while((FMC_BUSY == fmc_state) && (0x00U != timeout));
    
    if(FMC_BUSY == fmc_state){
        fmc_state = FMC_TOERR;
    }
    /* return the FMC state */
    return fmc_state;
}
