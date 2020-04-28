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
/// Utility for BMP
/// 
//------------------------------------------------------------------------------

#ifndef BMP_H
#define BMP_H

/// BMP magic number ('BM').
#define BMP_TYPE       0x4D42

/// headerSize must be set to 40
#define BITMAPINFOHEADER   40

//------------------------------------------------------------------------------
//         Exported types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

// BMP (Windows) Header Format
struct BMPHeader {
    /// signature, must be 4D42 hex
    unsigned short type;
    /// size of BMP file in bytes (unreliable)
    unsigned int fileSize;
    /// reserved, must be zero
    unsigned short reserved1;
    /// reserved, must be zero
    unsigned short reserved2;
    /// offset to start of image data in bytes
    unsigned int offset;
    /// size of BITMAPINFOHEADER structure, must be 40
    unsigned int headerSize;
    /// image width in pixels
    unsigned int width;
    /// image height in pixels
    unsigned int height;
    /// number of planes in the image, must be 1
    unsigned short planes;
    /// number of bits per pixel (1, 4, 8, 16, 24, 32)
    unsigned short bits;
    /// compression type (0=none, 1=RLE-8, 2=RLE-4)
    unsigned int compression;
    /// size of image data in bytes (including padding)
    unsigned int imageSize;
    /// horizontal resolution in pixels per meter (unreliable)
    unsigned int xresolution;
    /// vertical resolution in pixels per meter (unreliable)
    unsigned int yresolution;
    /// number of colors in image, or zero
    unsigned int ncolours;
    /// number of important colors, or zero
    unsigned int importantcolours;

} __attribute__ ((packed)); // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned char BMP_IsValid(void *file);

extern unsigned int BMP_GetFileSize(void *file);

extern unsigned char BMP_Decode(
    void *file,
    unsigned char *buffer,
    unsigned int width,
    unsigned int height,
    unsigned char bpp);

extern void WriteBMPheader(unsigned int* pAddressHeader, 
                    unsigned int  bmpHSize,
                    unsigned int  bmpVSize,
                    unsigned char bmpRgb,
                    unsigned char nbByte_Pixels);

extern void BMP_displayHeader(unsigned int* pAddressHeader);

extern void RGB565toBGR555(
    unsigned char *fileSource,
    unsigned char *fileDestination,
    unsigned int width,
    unsigned int height,
    unsigned char bpp);


#endif //#ifndef BMP_H

