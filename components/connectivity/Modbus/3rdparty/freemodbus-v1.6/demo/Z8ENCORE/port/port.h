/*
 * FreeModbus Libary: Z8Encore Port for Z8F6422
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#ifndef _PORT_H
#define _PORT_H

#include <assert.h>

#include <eZ8.h>

#define	INLINE
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define CLOCK						18432000

#define ENTER_CRITICAL_SECTION( )   DI()
#define EXIT_CRITICAL_SECTION( )    EI()

typedef unsigned char BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef unsigned int USHORT;
typedef int     SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#endif
