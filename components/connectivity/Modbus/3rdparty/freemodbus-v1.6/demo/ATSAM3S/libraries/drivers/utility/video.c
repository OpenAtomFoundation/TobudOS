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
//         Headers
//------------------------------------------------------------------------------
#include "video.h"

//-----------------------------------------------------------------------------
/// Conversion YCrCb to RGB:<BR>
/// R = 1.164(Yi – 16) + 1.596(Cr – 128)<BR>
/// G = 1.164(Yi – 16) – 0.813(Cr – 128) – 0.391(Cb – 128)<BR>
/// B = 1.164(Yi – 16) + 2.018(Cb – 128)<BR>
///
/// Pixel i+1:<BR>
/// YCrCb to RGB:<BR>
/// R = 1.164(Yi+1 – 16) + 1.596(Cr – 128)<BR>
/// G = 1.164(Yi+1 – 16) – 0.813(Cr – 128) – 0.391(Cb – 128)<BR>
/// B = 1.164(Yi+1 – 16) + 2.018(Cb – 128)
/// \param ycc Source buffer (YCC)
/// \param rgb Destination buffer (RGB)
/// \param len Length of buffer
//-----------------------------------------------------------------------------
void VIDEO_Ycc2Rgb(unsigned char *ycc, unsigned short *rgb, unsigned int len)
{
    int r_calc_i;
    int g_calc_i;
    int b_calc_i;
    int r_calc_i_incr;
    int g_calc_i_incr;
    int b_calc_i_incr;
    int cr_i;
    int y_i;
    int cb_i;
    int y_i_incr;
    int val;
    int val1;
    int val2;
    int val3;
    int val4;
    int val5;
    int i;

    for(i=0; i<len; i++)
    {
        cb_i = (int)ycc[4*i];
        y_i  = (int )ycc[4*i+1];
        cr_i = (int )ycc[4*i+2];
        y_i_incr =(int )ycc[4*i+3];

        val  = 1164*(y_i-16);
        val1 = 1164*(y_i_incr-16);
        val2 = 1596*(cr_i- 128);
        val3 =  813*(cb_i-128);
        val4 =  392*(cr_i-128);
        val5 = 2017*(cb_i-128);

        r_calc_i = (val + val2)/1000;
        g_calc_i = (val - val3 - val4)/1000;
        b_calc_i = (val + val5)/1000;
        r_calc_i_incr = (val1 + val2)/1000;
        g_calc_i_incr = (val1 - val3 - val4)/1000;
        b_calc_i_incr = (val1 + val5)/1000;

        if (r_calc_i < 0)
            r_calc_i = 0;
        else if (r_calc_i > 255)
            r_calc_i = 255;

        if (g_calc_i < 0)
            g_calc_i = 0;
        else if (g_calc_i > 255)
            g_calc_i = 255;

        if (b_calc_i < 0)
            b_calc_i = 0;
        else if (b_calc_i > 255)
            b_calc_i = 255;

        if (r_calc_i_incr < 0)
            r_calc_i_incr = 0;
        else if (r_calc_i_incr > 255)
            r_calc_i_incr = 255;

        if (g_calc_i_incr < 0)
            g_calc_i_incr = 0;
        else if (g_calc_i_incr > 255)
            g_calc_i_incr = 255;

        if (b_calc_i_incr < 0)
            b_calc_i_incr = 0;
        else if (b_calc_i_incr > 255)
            b_calc_i_incr = 255;

        *rgb++ = (((unsigned short )r_calc_i & 0xF8) >> 3)
               | ((((unsigned short)g_calc_i & 0xF8) >> 3) << 5)
               | ((((unsigned short)b_calc_i & 0xF8) >> 3) << 10);

        *rgb++ = (((unsigned short )r_calc_i_incr & 0xF8) >> 3)
               | ((((unsigned short)g_calc_i_incr & 0xF8) >> 3) << 5)
               | ((((unsigned short)b_calc_i_incr & 0xF8) >> 3) << 10);
    }
}

