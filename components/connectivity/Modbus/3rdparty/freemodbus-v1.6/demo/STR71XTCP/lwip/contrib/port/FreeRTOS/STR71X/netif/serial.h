/*
 * FreeModbus Libary: STR71/lwIP Port serial driver.
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
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

#ifndef _SERIAL_H
#define _SERIAL_H

/* ------------------------ Platform includes ----------------------------- */

#include "uart.h"

/* ------------------------ Defines --------------------------------------- */

/* ------------------------ Type definitions  ----------------------------- */

typedef enum
{
    SIO_PAR_NONE,
    SIO_PAR_ODD,
    SIO_PAR_EVEN
} sio_parity_t;

typedef enum
{
    SIO_STOP_0_5,
    SIO_STOP_1,
    SIO_STOP_1_5,
    SIO_STOP_2
} sio_stop_t;

/* ------------------------ Prototypes ------------------------------------ */
sio_fd_t        sio_open_new( u8_t devnr, u32_t baudrate, u8_t databits,
                              sio_stop_t stopbits, sio_parity_t parity );
u32_t           sio_write_noisr( sio_fd_t fd, u8_t * buf, u32_t size );
void            sio_send_noisr( u8_t data, sio_fd_t fd );
#endif
