/*
 * Copyright (c) 2006 Christian Walter
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Note: Most of the network buffer code is based on the MCF5235 examples from
 *   Freescale.
 *
 * File: $Id$
 */

#ifndef _NBUF_H
#define _NBUF_H

/* ------------------------ Defines --------------------------------------- */

#ifdef __GNUC__
#define ATTR_FECMEM             \
    __attribute__((section(".nbuf"),aligned(16)))
#endif

#define NBUF_RX                 ( 1 )
#define NBUF_TX                 ( 0 )

/* We set the receiver buffers to the maximum size the FEC supports ( See
 * MCF5235 reference manual 19.2.5.1.2 - Driver/DMA Operation with Receive
 * BDs). This gives us the benefit that any frame fits into one buffer. A
 * maximum size of 2047 is guaranteed by the FEC and 2048 is therefore a
 * safe value.
 * Note: The value MUST be dividable by 16!
 */
#define RX_BUFFER_SIZE          ( 2048 )

/* Size of the transmit buffers. If you set this value to small all frames
 * greater than this size will be dropped. The value 1520 was choosen because
 * it is bigger than the FEC MTU (1518) and is dividable by 16.
 * Note: The value MUST be dividable by 16! */
#define TX_BUFFER_SIZE          ( 1520 )

/* Number of Receive and Transmit Buffers and Buffer Descriptors */
#define NUM_RXBDS               ( 8 )
#define NUM_TXBDS               ( 4 )

/* ------------------------ Defines ( Buffer Descriptor Flags )------------ */

#define TX_BD_R                 ( 0x8000 )
#define TX_BD_INUSE             ( 0x4000 )
#define TX_BD_TO1               ( 0x4000 )
#define TX_BD_W                 ( 0x2000 )
#define TX_BD_TO2               ( 0x1000 )
#define TX_BD_L                 ( 0x0800 )
#define TX_BD_TC                ( 0x0400 )
#define TX_BD_DEF               ( 0x0200 )
#define TX_BD_HB                ( 0x0100 )
#define TX_BD_LC                ( 0x0080 )
#define TX_BD_RL                ( 0x0040 )
#define TX_BD_UN                ( 0x0002 )
#define TX_BD_CSL               ( 0x0001 )

#define RX_BD_E                 ( 0x8000 )
#define RX_BD_INUSE             ( 0x4000 )
#define RX_BD_R01               ( 0x4000 )
#define RX_BD_W                 ( 0x2000 )
#define RX_BD_R02               ( 0x1000 )
#define RX_BD_L                 ( 0x0800 )
#define RX_BD_M                 ( 0x0100 )
#define RX_BD_BC                ( 0x0080 )
#define RX_BD_MC                ( 0x0040 )
#define RX_BD_LG                ( 0x0020 )
#define RX_BD_NO                ( 0x0010 )
#define RX_BD_SH                ( 0x0008 )
#define RX_BD_CR                ( 0x0004 )
#define RX_BD_OV                ( 0x0002 )
#define RX_BD_TR                ( 0x0001 )

/* ------------------------ Type definitions ------------------------------ */
typedef struct
{
    uint16          status;     /* control and status */
    uint16          length;     /* transfer length */
    uint8          *data;       /* buffer address */
} nbuf_t;

/* ------------------------ Prototypes ------------------------------------ */

void            nbuf_init( void );
uint32          nbuf_get_start( uint8 );
nbuf_t         *nbuf_rx_allocate( void );
nbuf_t         *nbuf_tx_allocate( void );
void            nbuf_rx_release( nbuf_t * );
void            nbuf_tx_release( nbuf_t * );
int             nbuf_rx_next_ready( void );

#endif
