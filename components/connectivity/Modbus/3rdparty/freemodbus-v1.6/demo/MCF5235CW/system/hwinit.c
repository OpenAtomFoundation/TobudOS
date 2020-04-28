/*
    FreeRTOS MCF5235 port - Copyright (C) 2006 Christian Walter.
    Copyright @ 1995-2005 Freescale Semiconductor, Inc. All rights reserved

    FreeRTOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FreeRTOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FreeRTOS; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes FreeRTOS, without being obliged to provide
    the source code for any proprietary components.  See the licensing section
    of http://www.FreeRTOS.org for full details of how and when the exception
    can be applied.

    ***************************************************************************
    See http://www.FreeRTOS.org for documentation, latest information, license
    and contact details.  Please ensure to read the configuration and relevant
    port sections of the online documentation.
    ***************************************************************************
*/

#include "m523xbcc.h"

void            mcf523x_init( void );
void            mcf523x_wtm_init( void );
void            mcf523x_pll_init( void );
void            mcf523x_scm_init( void );
void            mcf523x_gpio_init( void );
void            mcf523x_cs_init( void );
void            mcf523x_sdram_init( void );
void            mcf523x_flexcan_init( void );

void
mcf523x_init( void )
{
    extern char     __DATA_ROM[];
    extern char     __DATA_RAM[];
    extern char     __DATA_END[];
    extern char     __BSS_START[];
    extern char     __BSS_END[];
    extern uint32   VECTOR_TABLE[];
    extern uint32   __VECTOR_RAM[];
    register uint32 n;

    mcf523x_flexcan_init(  );
    mcf523x_wtm_init(  );
    mcf523x_pll_init(  );
    mcf523x_gpio_init(  );
    mcf523x_scm_init(  );
    mcf523x_cs_init(  );
    mcf523x_sdram_init(  );

    /* Turn Instruction Cache ON */
    mcf5xxx_wr_cacr( 0
                     | MCF5XXX_CACR_CENB
                     | MCF5XXX_CACR_CINV
                     | MCF5XXX_CACR_DISD | MCF5XXX_CACR_CEIB | MCF5XXX_CACR_CLNF_00 );


    /* Copy the vector table to RAM */
    if( __VECTOR_RAM != VECTOR_TABLE )
    {
        for( n = 0; n < 256; n++ )
            __VECTOR_RAM[n] = VECTOR_TABLE[n];
    }
    mcf5xxx_wr_vbr( ( uint32 ) __VECTOR_RAM );

}

void
mcf523x_wtm_init( void )
{
    /*
     * Disable Software Watchdog Timer
     */
    MCF_WTM_WCR = 0;
}

void
mcf523x_pll_init( void )
{
    /*
     * Multiply 25Mhz reference crystal to acheive system clock of 150Mhz
     */

    MCF_FMPLL_SYNCR = MCF_FMPLL_SYNCR_MFD( 1 ) | MCF_FMPLL_SYNCR_RFD( 0 );

    while( !( MCF_FMPLL_SYNSR & MCF_FMPLL_SYNSR_LOCK ) )
    {
    };
}

void
mcf523x_scm_init( void )
{
    /*
     * Enable on-chip modules to access internal SRAM
     */
    MCF_SCM_RAMBAR = ( 0 | MCF_SCM_RAMBAR_BA( SRAM_ADDRESS >> 16 ) | MCF_SCM_RAMBAR_BDE );
}

void
mcf523x_gpio_init( void )
{

    /*
     * When booting from external Flash, the port-size is less than
     * the port-size of SDRAM.  In this case it is necessary to enable
     * Data[15:0] on Port Address/Data.
     */
    MCF_GPIO_PAR_AD = ( 0
                        | MCF_GPIO_PAR_AD_PAR_ADDR23
                        | MCF_GPIO_PAR_AD_PAR_ADDR22
                        | MCF_GPIO_PAR_AD_PAR_ADDR21 | MCF_GPIO_PAR_AD_PAR_DATAL );

    /*
     * Initialize PAR to enable SDRAM signals
     */
    MCF_GPIO_PAR_SDRAM = 0x3F;

    /*
     * Initialize PAR to enable Ethernet signals
     */
    MCF_GPIO_PAR_FECI2C = 0xF0;
}

void
mcf523x_sdram_init( void )
{
    int             i;

    /*
     * Check to see if the SDRAM has already been initialized
     * by a run control tool
     */
    if( !( MCF_SDRAMC_DACR0 & MCF_SDRAMC_DACR0_RE ) )
    {
        /* Initialize DRAM Control Register: DCR */
        MCF_SDRAMC_DCR = ( MCF_SDRAMC_DCR_RTIM( 1 ) |
                           MCF_SDRAMC_DCR_RC( ( 15 * FSYS_2 ) >> 4 ) );

        /* Initialize DACR0 */
        MCF_SDRAMC_DACR0 = ( MCF_SDRAMC_DACR0_BA( SDRAM_ADDRESS >> 18UL ) |
                             MCF_SDRAMC_DACR0_CASL( 1 ) |
                             MCF_SDRAMC_DACR0_CBM( 3 ) |
                             MCF_SDRAMC_DACR0_PS( 0 ) );

        /*
         * Initialize DMR0
         */
        MCF_SDRAMC_DMR0 = ( 0 | MCF_SDRAMC_DMR_BAM_16M | MCF_SDRAMC_DMR0_V );

        /*      
         * Set IP (bit 3) in DACR 
         */
        MCF_SDRAMC_DACR0 |= MCF_SDRAMC_DACR0_IP;

        /* 
         * Wait 30ns to allow banks to precharge 
         */
        for( i = 0; i < 5; i++ )
        {
#ifndef __MWERKS__
            asm( " nop" );
#else
            asm( nop );
#endif
        }

        /*      
         * Write to this block to initiate precharge 
         */
        *( uint32 * ) ( SDRAM_ADDRESS ) = 0xA5A59696;

        /*      
         * Set RE (bit 15) in DACR 
         */
        MCF_SDRAMC_DACR0 |= MCF_SDRAMC_DACR0_RE;

        /* 
         * Wait for at least 8 auto refresh cycles to occur 
         */
        for( i = 0; i < 2000; i++ )
        {
#ifndef __MWERKS__
            asm( " nop" );
#else
            asm( nop );
#endif
        }

        /*      
         * Finish the configuration by issuing the IMRS. 
         */
        MCF_SDRAMC_DACR0 |= MCF_SDRAMC_DACR0_MRS;

        /*
         * Write to the SDRAM Mode Register 
         */
        *( uint32 * ) ( SDRAM_ADDRESS + 0x400 ) = 0xA5A59696;
    }

}


void
mcf523x_cs_init( void )
{
    /* 
     * ChipSelect 0 - External Flash 
     */
    MCF_CS_CSAR0 = MCF_CS_CSAR_BA( EXT_FLASH_ADDRESS );
    MCF_CS_CSCR0 = ( 0 | MCF_CS_CSCR_IWS( 6 ) | MCF_CS_CSCR_AA | MCF_CS_CSCR_PS_16 );
    MCF_CS_CSMR0 = MCF_CS_CSMR_BAM_2M | MCF_CS_CSMR_V;
}

void
mcf523x_flexcan_init( void )
{

    /* FlexCAN controller 0 disabled (CANMCR0[MDIS]=1) */
    MCF_CAN_IMASK0 = 0;
    MCF_CAN_RXGMASK0 = MCF_CAN_RXGMASK_MI( 0x1fffffff );
    MCF_CAN_RX14MASK0 = MCF_CAN_RX14MASK_MI( 0x1fffffff );
    MCF_CAN_RX15MASK0 = MCF_CAN_RX15MASK_MI( 0x1fffffff );
    MCF_CAN_CANCTRL0 = 0;
    MCF_CAN_CANMCR0 =
        MCF_CAN_CANMCR_MDIS | MCF_CAN_CANMCR_FRZ | MCF_CAN_CANMCR_HALT |
        MCF_CAN_CANMCR_SUPV | MCF_CAN_CANMCR_MAXMB( 0xf );

    /* FlexCAN controller 1 disabled (CANMCR1[MDIS]=1) */
    MCF_CAN_IMASK1 = 0;
    MCF_CAN_RXGMASK1 = MCF_CAN_RXGMASK_MI( 0x1fffffff );
    MCF_CAN_RX14MASK1 = MCF_CAN_RX14MASK_MI( 0x1fffffff );
    MCF_CAN_RX15MASK1 = MCF_CAN_RX15MASK_MI( 0x1fffffff );
    MCF_CAN_CANCTRL1 = 0;
    MCF_CAN_CANMCR1 =
        MCF_CAN_CANMCR_MDIS | MCF_CAN_CANMCR_FRZ | MCF_CAN_CANMCR_HALT |
        MCF_CAN_CANMCR_SUPV | MCF_CAN_CANMCR_MAXMB( 0xf );
}
