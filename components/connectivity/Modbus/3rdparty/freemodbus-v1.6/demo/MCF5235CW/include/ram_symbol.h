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

/* Defining DEBUG turns on debug print information */
#define DEBUG	1

/* CodeWarrior looks for an underscore prepended to C function names */
#define _UNDERSCORE_

/* Define a constant to inform files we are using CodeWarrior */
#ifndef _MWERKS_
#define _MWERKS_
#endif

/* Modify the interrupt type to work with CodeWarrior */
#define __interrupt__	__declspec(interrupt)

#define CODEWARRIOR_MCF5235
#define FCPU                    ( 150000000UL )
#define FSYS_2                  ( 75000000UL )
