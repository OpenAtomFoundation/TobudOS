/*
 * File:		m523xbcc.h
 * Purpose:		Evaluation board definitions and memory map information
 *
 * Notes:
 */

#ifndef _M523XBCC_H
#define _M523XBCC_H

/********************************************************************/

#include "mcf5xxx.h"
#include "mcf523x.h"
#include "io.h"

/********************************************************************/

/*
 * Debug prints ON (#undef) or OFF (#define)
 */
#undef DEBUG

/*
 * System Bus Clock Info
 */
#define	SYSTEM_CLOCK			75		/* system bus frequency in MHz */
#define PERIOD					13.33	/* system bus period in ns */

/*
 * Flash Device Info
 */
#define AMD_FLASH_AM29PL160CB_16BIT

/*
 * Ethernet Port Info
 */
#define FEC_PHY0            (0x01)

/*
 * Memory map definitions from linker command files
 */
extern uint8 __IPSBAR[];
extern uint8 __SDRAM[];
extern uint8 __SDRAM_SIZE[];
extern uint8 __SRAM[];
extern uint8 __SRAM_SIZE[];
extern uint8 __EXT_FLASH[];
extern uint8 __EXT_FLASH_SIZE[];

/*
 * Memory Map Info
 */
#define IPSBAR_ADDRESS			(uint32)__IPSBAR

#define SDRAM_ADDRESS			(uint32)__SDRAM
#define SDRAM_SIZE				(uint32)__SDRAM_SIZE

#define SRAM_ADDRESS			(uint32)__SRAM
#define SRAM_SIZE				(uint32)__SRAM_SIZE

#define EXT_FLASH_ADDRESS		(uint32)__EXT_FLASH
#define EXT_FLASH_SIZE			(uint32)__EXT_FLASH_SIZE

/*
 *	Interrupt Controller Definitions
 */
#define TIMER_NETWORK_LEVEL		3
#define FEC_LEVEL				4

/*
 *	Timer period info
 */
#define TIMER_NETWORK_PERIOD	1000000000/0x10000	/* 1 sec / max timeout */

/********************************************************************/

#endif
