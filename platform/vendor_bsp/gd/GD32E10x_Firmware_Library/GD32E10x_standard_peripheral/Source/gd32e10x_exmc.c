/*!
    \file  gd32e10x_exmc.c
    \brief EXMC driver
    
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

#include "gd32e10x_exmc.h"

/* EXMC bank0 register reset value */
#define BANK0_SNCTL_RESET                 ((uint32_t)0x000030DBU)
#define BANK0_SNTCFG_RESET                ((uint32_t)0x0FFFFFFFU)
#define BANK0_SNWTCFG_RESET               ((uint32_t)0x0FFFFFFFU)

/* EXMC register bit offset */
#define SNCTL_NRMUX_OFFSET                ((uint32_t)1U)
#define SNCTL_SBRSTEN_OFFSET              ((uint32_t)8U)
#define SNCTL_WRAPEN_OFFSET               ((uint32_t)10U)
#define SNCTL_WREN_OFFSET                 ((uint32_t)12U)
#define SNCTL_NRWTEN_OFFSET               ((uint32_t)13U)
#define SNCTL_EXMODEN_OFFSET              ((uint32_t)14U)
#define SNCTL_ASYNCWAIT_OFFSET            ((uint32_t)15U)

#define SNTCFG_AHLD_OFFSET                ((uint32_t)4U)
#define SNTCFG_DSET_OFFSET                ((uint32_t)8U)
#define SNTCFG_BUSLAT_OFFSET              ((uint32_t)16U)

#define SNWTCFG_WAHLD_OFFSET              ((uint32_t)4U)
#define SNWTCFG_WDSET_OFFSET              ((uint32_t)8U)
#define SNWTCFG_WBUSLAT_OFFSET            ((uint32_t)16U)

/*!
    \brief      deinitialize EXMC NOR/SRAM bank
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_norsram_deinit(void)
{
    /* reset the registers */
    EXMC_SNCTL = BANK0_SNCTL_RESET;

    EXMC_SNTCFG = BANK0_SNTCFG_RESET;
    EXMC_SNWTCFG = BANK0_SNWTCFG_RESET;
}

/*!
    \brief      initialize the struct exmc_norsram_parameter_struct
    \param[in]  none
    \param[out] exmc_norsram_init_struct: the initialized struct exmc_norsram_parameter_struct pointer
    \retval     none
*/
void exmc_norsram_struct_para_init(exmc_norsram_parameter_struct* exmc_norsram_init_struct)
{
    /* configure the structure with default value */
    exmc_norsram_init_struct->address_data_mux = ENABLE;
    exmc_norsram_init_struct->memory_type = EXMC_MEMORY_TYPE_NOR;
    exmc_norsram_init_struct->databus_width = EXMC_NOR_DATABUS_WIDTH_16B;
    exmc_norsram_init_struct->burst_mode = DISABLE;
    exmc_norsram_init_struct->nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
    exmc_norsram_init_struct->wrap_burst_mode = DISABLE;
    exmc_norsram_init_struct->nwait_config = EXMC_NWAIT_CONFIG_BEFORE;
    exmc_norsram_init_struct->memory_write = ENABLE;
    exmc_norsram_init_struct->nwait_signal = ENABLE;
    exmc_norsram_init_struct->extended_mode = DISABLE;
    exmc_norsram_init_struct->asyn_wait = DISABLE;
    exmc_norsram_init_struct->write_mode = EXMC_ASYN_WRITE;

    /* read/write timing configure */
    exmc_norsram_init_struct->read_write_timing->asyn_address_setuptime = 0xFU;
    exmc_norsram_init_struct->read_write_timing->asyn_address_holdtime = 0xFU;
    exmc_norsram_init_struct->read_write_timing->asyn_data_setuptime = 0xFFU;
    exmc_norsram_init_struct->read_write_timing->bus_latency = 0xFU;
    exmc_norsram_init_struct->read_write_timing->syn_clk_division = EXMC_SYN_CLOCK_RATIO_16_CLK;
    exmc_norsram_init_struct->read_write_timing->syn_data_latency = EXMC_DATALAT_17_CLK;
    exmc_norsram_init_struct->read_write_timing->asyn_access_mode = EXMC_ACCESS_MODE_A;

    /* write timing configure, when extended mode is used */
    exmc_norsram_init_struct->write_timing->asyn_address_setuptime = 0xFU;
    exmc_norsram_init_struct->write_timing->asyn_address_holdtime = 0xFU;
    exmc_norsram_init_struct->write_timing->asyn_data_setuptime = 0xFFU;
    exmc_norsram_init_struct->write_timing->bus_latency = 0xFU;
    exmc_norsram_init_struct->write_timing->asyn_access_mode = EXMC_ACCESS_MODE_A;
}

/*!
    \brief      initialize EXMC NOR/SRAM bank
    \param[in]  exmc_norsram_parameter_struct: configure the EXMC NOR/SRAM parameter
                  write_mode: EXMC_ASYN_WRITE,EXMC_SYN_WRITE
                  extended_mode: ENABLE or DISABLE 
                  asyn_wait: ENABLE or DISABLE
                  nwait_signal: ENABLE or DISABLE
                  memory_write: ENABLE or DISABLE
                  nwait_config: EXMC_NWAIT_CONFIG_BEFORE,EXMC_NWAIT_CONFIG_DURING
                  wrap_burst_mode: ENABLE or DISABLE
                  nwait_polarity: EXMC_NWAIT_POLARITY_LOW,EXMC_NWAIT_POLARITY_HIGH
                  burst_mode: ENABLE or DISABLE
                  databus_width: EXMC_NOR_DATABUS_WIDTH_8B,EXMC_NOR_DATABUS_WIDTH_16B
                  memory_type: EXMC_MEMORY_TYPE_SRAM,EXMC_MEMORY_TYPE_PSRAM,EXMC_MEMORY_TYPE_NOR
                  address_data_mux: ENABLE or DISABLE
                  read_write_timing: struct exmc_norsram_timing_parameter_struct set the time
                  write_timing: struct exmc_norsram_timing_parameter_struct set the time
    \param[out] none
    \retval     none
*/
void exmc_norsram_init(exmc_norsram_parameter_struct* exmc_norsram_init_struct)
{
    uint32_t snctl = 0x00000000U, sntcfg = 0x00000000U, snwtcfg = 0x00000000U;

    /* get the register value */
    snctl = EXMC_SNCTL;

    /* clear relative bits */
    snctl &= ((uint32_t)~(EXMC_SNCTL_NRMUX | EXMC_SNCTL_NRTP | EXMC_SNCTL_NRW | EXMC_SNCTL_SBRSTEN | 
                          EXMC_SNCTL_NREN | EXMC_SNCTL_NRWTPOL | EXMC_SNCTL_WRAPEN | EXMC_SNCTL_NRWTCFG | 
                          EXMC_SNCTL_WREN | EXMC_SNCTL_NRWTEN | EXMC_SNCTL_EXMODEN | EXMC_SNCTL_ASYNCWAIT | 
                          EXMC_SNCTL_SYNCWR ));

    snctl |= (uint32_t)(exmc_norsram_init_struct->address_data_mux << SNCTL_NRMUX_OFFSET) |
                        exmc_norsram_init_struct->memory_type |
                        exmc_norsram_init_struct->databus_width |
                       (exmc_norsram_init_struct->burst_mode << SNCTL_SBRSTEN_OFFSET) |
                        exmc_norsram_init_struct->nwait_polarity |
                       (exmc_norsram_init_struct->wrap_burst_mode << SNCTL_WRAPEN_OFFSET) |
                        exmc_norsram_init_struct->nwait_config |
                       (exmc_norsram_init_struct->memory_write << SNCTL_WREN_OFFSET) |
                       (exmc_norsram_init_struct->nwait_signal << SNCTL_NRWTEN_OFFSET) |
                       (exmc_norsram_init_struct->extended_mode << SNCTL_EXMODEN_OFFSET) |
                       (exmc_norsram_init_struct->asyn_wait << SNCTL_ASYNCWAIT_OFFSET) |
                        exmc_norsram_init_struct->write_mode;

    sntcfg = (uint32_t)((exmc_norsram_init_struct->read_write_timing->asyn_address_setuptime - 1U ) & EXMC_SNTCFG_ASET )|
                       (((exmc_norsram_init_struct->read_write_timing->asyn_address_holdtime - 1U ) << SNTCFG_AHLD_OFFSET ) & EXMC_SNTCFG_AHLD ) |
                       (((exmc_norsram_init_struct->read_write_timing->asyn_data_setuptime - 1U ) << SNTCFG_DSET_OFFSET ) & EXMC_SNTCFG_DSET ) |
                       (((exmc_norsram_init_struct->read_write_timing->bus_latency - 1U ) << SNTCFG_BUSLAT_OFFSET ) & EXMC_SNTCFG_BUSLAT )|
                       exmc_norsram_init_struct->read_write_timing->syn_clk_division |
                       exmc_norsram_init_struct->read_write_timing->syn_data_latency |
                       exmc_norsram_init_struct->read_write_timing->asyn_access_mode;

    /* nor flash access enable */
    if(EXMC_MEMORY_TYPE_NOR == exmc_norsram_init_struct->memory_type){
        snctl |= (uint32_t)EXMC_SNCTL_NREN;
    }

    /* extended mode configure */
    if(ENABLE == exmc_norsram_init_struct->extended_mode){
        snwtcfg = (uint32_t)((exmc_norsram_init_struct->write_timing->asyn_address_setuptime - 1U) & EXMC_SNWTCFG_WASET ) |
                           (((exmc_norsram_init_struct->write_timing->asyn_address_holdtime -1U ) << SNWTCFG_WAHLD_OFFSET ) & EXMC_SNWTCFG_WAHLD )|
                           (((exmc_norsram_init_struct->write_timing->asyn_data_setuptime -1U ) << SNWTCFG_WDSET_OFFSET ) & EXMC_SNWTCFG_WDSET )|
                           (((exmc_norsram_init_struct->write_timing->bus_latency - 1U ) << SNWTCFG_WBUSLAT_OFFSET ) & EXMC_SNWTCFG_WBUSLAT ) |
                            exmc_norsram_init_struct->write_timing->asyn_access_mode;
    }else{
        snwtcfg = BANK0_SNWTCFG_RESET;
    }

    /* configure the registers */
    EXMC_SNCTL = snctl;
    EXMC_SNTCFG = sntcfg;
    EXMC_SNWTCFG = snwtcfg;
}

/*!
    \brief      enable EXMC NOR/PSRAM bank 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_norsram_enable(void)
{
    EXMC_SNCTL |= (uint32_t)EXMC_SNCTL_NRBKEN;
}

/*!
    \brief      disable EXMC NOR/PSRAM bank 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exmc_norsram_disable(void)
{
    EXMC_SNCTL &= ~(uint32_t)EXMC_SNCTL_NRBKEN;
}

/*!
    \brief      configure CRAM page size 
    \param[in]  page_size: CRAM page size
                only one parameter can be selected which is shown as below:
      \arg        EXMC_CRAM_AUTO_SPLIT: the clock is generated only during synchronous access
      \arg        EXMC_CRAM_PAGE_SIZE_128_BYTES: page size is 128 bytes
      \arg        EXMC_CRAM_PAGE_SIZE_256_BYTES: page size is 256 bytes
      \arg        EXMC_CRAM_PAGE_SIZE_512_BYTES: page size is 512 bytes
      \arg        EXMC_CRAM_PAGE_SIZE_1024_BYTES: page size is 1024 bytes
    \param[out] none
    \retval     none
*/
void exmc_norsram_page_size_config(uint32_t page_size)
{
    /* reset the bits */
    EXMC_SNCTL &= ~EXMC_SNCTL_CPS;
    /* set the CPS bits */
    EXMC_SNCTL |= page_size;
}
