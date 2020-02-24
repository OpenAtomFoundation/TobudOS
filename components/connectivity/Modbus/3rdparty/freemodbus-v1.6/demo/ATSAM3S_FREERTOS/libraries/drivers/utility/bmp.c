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

#include "bmp.h"
#include <board.h>
#include <utility/trace.h>

#include <string.h>

//-----------------------------------------------------------------------------
//         Define
//-----------------------------------------------------------------------------
/// BMP offset for header
#define  IMAGE_OFFSET       0x100


//------------------------------------------------------------------------------
//         Internal constants
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//         Internal types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Describe the BMP palette
//------------------------------------------------------------------------------
struct BMPPaletteEntry {

    /// Blue value
    unsigned char b;
    /// Green value
    unsigned char g;
    /// Red value
    unsigned char r;
    /// Filler character value
    unsigned char filler;
};

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Test if BMP is valid
/// \param file  Buffer holding the file to examinate.
/// \return 1 if the header of a BMP file is valid; otherwise returns 0.
//------------------------------------------------------------------------------
unsigned char BMP_IsValid(void *file)
{
    return ((struct BMPHeader *) file)->type == BMP_TYPE;
}

//------------------------------------------------------------------------------
/// Returns the size of a BMP image given at least its header (the file does
/// not have to be complete).
/// \param file  Pointer to the buffer which holds the BMP file.
/// \return size of BMP image
//------------------------------------------------------------------------------
unsigned int BMP_GetFileSize(void *file)
{
    return ((struct BMPHeader *) file)->fileSize;
}

//-----------------------------------------------------------------------------
/// Write a BMP header
/// \param pAddressHeader Begin address of the BMP
/// \param bmpHSize BMP heigth size
/// \param bmpVSize BMP width size
/// \param bmpRgb Type of BMP (YUV or RGB)
/// \param nbByte_Pixels Number of byte per pixels
//-----------------------------------------------------------------------------
void WriteBMPheader(unsigned int* pAddressHeader, 
                    unsigned int  bmpHSize,
                    unsigned int  bmpVSize,
                    unsigned char bmpRgb,
                    unsigned char nbByte_Pixels)
{
    unsigned int i;
    unsigned int* fill;
    struct BMPHeader *Header;

    fill = pAddressHeader;
    for (i=0; i<IMAGE_OFFSET; i+=4) {
        *fill++ = 0;
    }

    Header = (struct BMPHeader*) pAddressHeader;

    Header->type = BMP_TYPE;
    Header->fileSize = (bmpHSize * bmpVSize * nbByte_Pixels) + IMAGE_OFFSET;
    Header->reserved1 = 0;
    Header->reserved2 = 0;
    Header->offset = IMAGE_OFFSET;
    Header->headerSize = BITMAPINFOHEADER;
    Header->width  = bmpHSize;
    Header->height = bmpVSize;
    Header->planes = 1;
    Header->bits = nbByte_Pixels * 8;
    Header->compression = 0;
    Header->imageSize = bmpHSize * bmpVSize * nbByte_Pixels;
    Header->xresolution = 0;
    Header->yresolution = 0;
    Header->ncolours = 0;
    Header->importantcolours = 0;
}


//------------------------------------------------------------------------------
/// debug function, dislay BMP header
/// \param pAddressHeader Address of the BMP
//------------------------------------------------------------------------------
void BMP_displayHeader(unsigned int* pAddressHeader)
{
    struct BMPHeader *header;

    header = (struct BMPHeader*) pAddressHeader;

    TRACE_INFO("BMP\n\r");
    TRACE_INFO("type       0x%X \n\r", header->type);
    TRACE_INFO("fileSize   %d \n\r", header->fileSize);
    TRACE_INFO("reserved1  %d \n\r", header->reserved1);
    TRACE_INFO("reserved2  %d \n\r", header->reserved2);
    TRACE_INFO("offset     %d \n\r", header->offset);
    TRACE_INFO("headerSize %d \n\r", header->headerSize);
    TRACE_INFO("width      %d \n\r", header->width);
    TRACE_INFO("height     %d \n\r", header->height);
    TRACE_INFO("planes     %d \n\r", header->planes);
    TRACE_INFO("bits       %d \n\r", header->bits);
    TRACE_INFO("compression %d \n\r", header->compression);
    TRACE_INFO("imageSize   %d \n\r", header->imageSize);
    TRACE_INFO("xresolution %d \n\r", header->xresolution);
    TRACE_INFO("yresolution %d \n\r", header->yresolution);
    TRACE_INFO("ncolours    %d \n\r", header->ncolours);
    TRACE_INFO("importantcolours %d\n\r", header->importantcolours);
}


//------------------------------------------------------------------------------
/// Loads a BMP image located at the given address, decodes it and stores the
/// resulting image inside the provided buffer. Image must have the specified
/// width & height.
/// If no buffer is provided, this function simply checks if it is able to
/// decode the image.
/// \param file  Buffer which holds the BMP file.
/// \param buffer  Buffer in which to store the decoded image.
/// \param width  Buffer width in pixels.
/// \param height  Buffer height in pixels.
/// \param bpp  Number of bits per pixels that the buffer stores.
/// \return 0 if the image has been loaded; otherwise returns an error code.
//------------------------------------------------------------------------------
unsigned char BMP_Decode(
    void *file,
    unsigned char *buffer,
    unsigned int width,
    unsigned int height,
    unsigned char bpp)
{
    struct BMPHeader *header;
    unsigned int i, j;
    unsigned char r, g, b;
    unsigned char *image;

    // Read header information
    header = (struct BMPHeader *) file;

    // Verify that the file is valid
    if (!BMP_IsValid(file)) {

        TRACE_ERROR("BMP_Decode: File type is not 'BM' (0x%04X).\n\r",header->type);
        return 1;
    }

    // Check that parameters match
    if ((header->compression != 0)
        || (header->width != width)
        || (header->height != height)) {

        TRACE_ERROR("BMP_Decode: File format not supported\n\r");
        TRACE_ERROR(" -> .compression = %u\n\r", header->compression);
        TRACE_ERROR(" -> .width = %u\n\r", header->width);
        TRACE_ERROR(" -> .height = %u\n\r", header->height);
        TRACE_ERROR(" -> .bits = %d\n\r", header->bits);
        return 2;
    }

    // Get image data
    image = (unsigned char *) ((unsigned int) file + header->offset);

    // Check that the bpp resolution is supported
    // Only a 24-bit output & 24- or 8-bit input are supported
    if (bpp != 24) {

        TRACE_ERROR("BMP_Decode: Output resolution not supported\n\r");
        return 3;
    }
    else if (header->bits == 24) {

        // Decoding is ok
        if (!buffer) return 0;

        // Get image data (swapping red & blue)
        for (i=0; i < height; i++) {
            for (j=0; j < width; j++) {

                r = image[((height - i - 1) * width + j) * 3 + 2];
                g = image[((height - i - 1) * width + j) * 3 + 1];
                b = image[((height - i - 1) * width + j) * 3];
                
#if defined(BOARD_LCD_RGB565)
                // Interlacing
                r = ((r << 1) & 0xF0) | ((g & 0x80) >> 4) | ((r & 0x80) >> 5);
                g = (g << 1) & 0xF8;
                b = b & 0xF8;
                
                buffer[(i * width + j) * 3] = b;
                buffer[(i * width + j) * 3 + 1] = g;
                buffer[(i * width + j) * 3 + 2] = r;

#else
                buffer[(i * width + j) * 3] = r;
                buffer[(i * width + j) * 3 + 1] = g;
                buffer[(i * width + j) * 3 + 2] = b;
#endif //#if defined(BOARD_LCD_RGB565)
            }
        }
    }
    else if (header->bits == 8) {

        // Decoding is ok
        if (!buffer) return 0;    

        // Retrieve palette
        struct BMPPaletteEntry palette[256];
        memcpy(palette,
               (unsigned char *) ((unsigned int) file + sizeof(struct BMPHeader)),
               header->offset - sizeof(struct BMPHeader));

        // Decode image (reversing row order)
        for (i=0; i < height; i++) {
            for (j=0; j < width; j++) {

                r = palette[image[(height - i - 1) * width + j]].r;
                g = palette[image[(height - i - 1) * width + j]].g;
                b = palette[image[(height - i - 1) * width + j]].b;

                buffer[(i * width + j) * 3] = r;
                buffer[(i * width + j) * 3 + 1] = g;
                buffer[(i * width + j) * 3 + 2] = b;
            }
        }
    }
    else {

        TRACE_ERROR("BMP_Decode: Input resolution not supported\n\r");
        TRACE_INFO("header->bits 0x%X \n\r", header->bits);
        return 4;
    }

    return 0;
}

//------------------------------------------------------------------------------
/// Convert RGB 565 to RGB 555 (RGB 555 is adapted to LCD)
/// \param fileSource  Buffer which holds the RGB file
/// \param fileDestination  Buffer in which to store the decoded image
/// \param width  Buffer width in pixels.
/// \param height  Buffer height in pixels.
/// \param bpp  Number of bits per pixels that the buffer stores.
//------------------------------------------------------------------------------
void RGB565toBGR555(
    unsigned char *fileSource,
    unsigned char *fileDestination,
    unsigned int width,
    unsigned int height,
    unsigned char bpp)
{
    unsigned int i;
    unsigned int j;
    unsigned int row;

    for (i=0; i < height*(bpp/8); i++) {
        row = (i*width*(bpp/8));
        for (j=0; j <= width*(bpp/8); j+=2) {
            fileDestination[row+j] = ((fileSource[row+j+1]>>3)&0x1F)
                                    | (fileSource[row+j]&0xE0);
            fileDestination[row+j+1] = (fileSource[row+j+1]&0x03)
                                    | ((fileSource[row+j]&0x1F)<<2);
        }
    }
}
