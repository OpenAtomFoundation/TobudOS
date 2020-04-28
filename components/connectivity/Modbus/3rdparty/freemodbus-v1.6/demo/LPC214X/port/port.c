 /*
  * FreeModbus Libary: LPC214X Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
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

/* ----------------------- System includes --------------------------------*/
#include <LPC214x.h>

/* ----------------------- Modbus includes ----------------------------------*/

/* ----------------------- Variables ----------------------------------------*/
int             VIC_Temp;

/* ----------------------- Start implementation -----------------------------*/
void
EnterCriticalSection(  )
{
    VIC_Temp = VICIntEnable;    /* Save VICIntEnable */
    VICIntEnClr = VIC_Temp;     /* Disable Interruptions */
}

void
ExitCriticalSection(  )
{
    VICIntEnable = VIC_Temp;    /* Restore VICIntEnable */
}
