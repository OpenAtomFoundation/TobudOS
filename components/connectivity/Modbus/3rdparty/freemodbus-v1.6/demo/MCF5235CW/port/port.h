 /*
  * FreeModbus Libary: MCF5235 Port
  * Copyright (C) 2006 Christian Walter <wolti@sil.at>
  * Parts of crt0.S Copyright (c) 1995, 1996, 1998 Cygnus Support
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * File: $Id$
  */

#ifndef _PORT_H
#define _PORT_H

/* ----------------------- Defines ----------------------------------------- */
#define	INLINE
#define PR_BEGIN_EXTERN_C         extern "C" {
#define	PR_END_EXTERN_C           }

#undef assert
#define assert( x )

#define ENTER_CRITICAL_SECTION( ) prvvPortEnterCritical()
#define EXIT_CRITICAL_SECTION( )  prvvPortExitCritical()

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

/* ----------------------- Type definitions -------------------------------- */
typedef char    BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

/* ----------------------- Defines ----------------------------------------- */
void            prvvPortEnterCritical(  );
void            prvvPortExitCritical(  );


#endif
