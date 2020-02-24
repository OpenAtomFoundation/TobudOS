/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
/// \unit
///
/// !!!Purpose
/// 
/// Video structure, used for LCD and ISI.
/// Conversion Video from YCC to RGB
///
//------------------------------------------------------------------------------

#ifndef _VIDEO_H
#define _VIDEO_H

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------
/// Type of video is YUV
#define YUV 0
/// Type of video is RGB
#define RGB 1

//-----------------------------------------------------------------------------
/// Video structure
//-----------------------------------------------------------------------------
typedef struct _AT91S_VIDEO
{
    /// LCD Vertical Size
    unsigned int  lcd_vsize;
    /// LCD Horizontal Size
    unsigned int  lcd_hsize;
    /// LCD Number of Bit Per Pixel
    unsigned int  lcd_nbpp;
    /// LCD Frame Buffer Address
    unsigned int  lcd_fb_addr;
    /// Base address for the frame buffer descriptors list
    unsigned int  Isi_fbd_base;
    /// Start of Line Delay
    unsigned int  Hblank;
    /// Start of frame Delay 
    unsigned int  Vblank;
    /// Vertical size of the Image sensor [0..2047]
    unsigned int  codec_vsize;
    /// Horizontal size of the Image sensor [0..2047]
    unsigned int  codec_hsize;
    /// Base address for codec DMA
    unsigned int  codec_fb_addr;
    /// Buffer index
    unsigned int  IsiPrevBuffIndex;
    /// Type of video 
    unsigned char rgb_or_yuv;        
}AT91S_VIDEO, *AT91PS_VIDEO;


//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
extern void VIDEO_Ycc2Rgb(unsigned char *ycc, unsigned short *rgb, unsigned int len);

#endif

