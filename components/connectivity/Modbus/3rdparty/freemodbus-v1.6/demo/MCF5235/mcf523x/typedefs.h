/**************************************************************************
 * FILE NAME: $RCSfile: typedefs.h,v $       COPYRIGHT (c) Freescale 2004 *
 * DESCRIPTION:                                     All Rights Reserved   *
 * This file defines all of the data types for the Motorola header file.  *
 *========================================================================*
 * ORIGINAL AUTHOR: Jeff Loeliger (r12110)                                *
 * $Log: typedefs.h,v $
 * Revision 1.1  2006/05/14 21:59:16  wolti
 *  - initial version of MCF5235 port
 *
 * Revision 1.1.1.1  2005/05/26 17:07:05  C-machim
 * CF 6.0 Stationeries
 *
 * Revision 1.2  2004/11/17 12:43:12  r12110
 * -Removed #ifdef DEBUG from around initial comment block.
 *
 * Revision 1.1  2004/11/17 12:38:48  r12110
 * -Initial version checked into CVS.
 * -Updated copyright from Motorola to Freescale.
 *
 *........................................................................*
 * 0.1   J. Loeliger  17/Feb/03    Initial version of file.               *
 * 0.2   J. Loeliger  06/Mar/03    Added DCC support.                     *
 * 0.3   J. Loeliger  07/May/03    Change to fully use ISO data types.    *
 * 0.4   J. Loeliger  17/Jun/03    Change name to motint.h and merge      *
 *                                  MPC5500 and MAC7100 files.            *
 * 0.5   J. Loeliger  04/Nov/03    Changed name to typedefs.h.            *
 * 0.6   J. Loeliger  09/May/04    Changed to support GHS and GCC.        *
 **************************************************************************/

#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

#ifdef __MWERKS__               //Metrowerk CodeWarrior
#include <stdint.h>

    // Standard typedefs used by header files, based on ISO C standard
typedef volatile int8_t vint8_t;
typedef volatile uint8_t vuint8_t;

typedef volatile int16_t vint16_t;
typedef volatile uint16_t vuint16_t;

typedef volatile int32_t vint32_t;
typedef volatile uint32_t vuint32_t;

#else
#ifdef __GHS__                  //GreenHills
#include <stdint.h>

    // Standard typedefs used by header files, based on ISO C standard
typedef volatile int8_t vint8_t;
typedef volatile uint8_t vuint8_t;

typedef volatile int16_t vint16_t;
typedef volatile uint16_t vuint16_t;

typedef volatile int32_t vint32_t;
typedef volatile uint32_t vuint32_t;

#else

    // This is needed for compilers that don't have a stdint.h file

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef volatile signed char vint8_t;
typedef volatile unsigned char vuint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef volatile signed short vint16_t;
typedef volatile unsigned short vuint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef volatile signed int vint32_t;
typedef volatile unsigned int vuint32_t;

#endif
#endif
#endif

/*********************************************************************
 *
 * Copyright:
 *	Freescale Semiconductor, INC. All Rights Reserved.
 *  You are hereby granted a copyright license to use, modify, and
 *  distribute the SOFTWARE so long as this entire notice is
 *  retained without alteration in any modified and/or redistributed
 *  versions, and that such modified versions are clearly identified
 *  as such. No licenses are granted by implication, estoppel or
 *  otherwise under any patents or trademarks of Freescale
 *  Semiconductor, Inc. This software is provided on an "AS IS"
 *  basis and without warranty.
 *
 *  To the maximum extent permitted by applicable law, Freescale
 *  Semiconductor DISCLAIMS ALL WARRANTIES WHETHER EXPRESS OR IMPLIED,
 *  INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A
 *  PARTICULAR PURPOSE AND ANY WARRANTY AGAINST INFRINGEMENT WITH
 *  REGARD TO THE SOFTWARE (INCLUDING ANY MODIFIED VERSIONS THEREOF)
 *  AND ANY ACCOMPANYING WRITTEN MATERIALS.
 *
 *  To the maximum extent permitted by applicable law, IN NO EVENT
 *  SHALL Freescale Semiconductor BE LIABLE FOR ANY DAMAGES WHATSOEVER
 *  (INCLUDING WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
 *  BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR OTHER
 *  PECUNIARY LOSS) ARISING OF THE USE OR INABILITY TO USE THE SOFTWARE.
 *
 *  Freescale Semiconductor assumes no responsibility for the
 *  maintenance and support of this software
 *
 ********************************************************************/
