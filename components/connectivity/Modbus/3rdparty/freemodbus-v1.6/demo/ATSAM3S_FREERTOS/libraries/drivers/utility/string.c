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
/// !Purpose
///
/// Implementation of several methods defined in string.h, for reducing the
/// memory footprint when using them (since the whole libc.o file gets included
/// even when using a single method).
///
/// !Usage
///
/// Add string.c to the list of files to compile for the project. This will
/// automatically replace standard libc methods by the custom ones.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <string.h>

//------------------------------------------------------------------------------
//         Global Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Copies data from a source buffer into a destination buffer. The two buffers
/// must NOT overlap. Returns the destination buffer.
/// \param pDestination  Destination buffer.
/// \param pSource  Source buffer.
/// \param num  Number of bytes to copy.
//------------------------------------------------------------------------------
void * memcpy(void *pDestination, const void *pSource, size_t num)
{
    unsigned char *pByteDestination;
    unsigned char *pByteSource;
    unsigned int *pAlignedSource = (unsigned int *) pSource;
    unsigned int *pAlignedDestination = (unsigned int *) pDestination;

    // If num is more than 4 bytes, and both dest. and source are aligned,
    // then copy dwords
    if ((((unsigned int) pAlignedDestination & 0x3) == 0)
        && (((unsigned int) pAlignedSource & 0x3) == 0)
        && (num >= 4)) {

        while (num >= 4) {

            *pAlignedDestination++ = *pAlignedSource++;
            num -= 4;
        }
    }

    // Copy remaining bytes
    pByteDestination = (unsigned char *) pAlignedDestination;
    pByteSource = (unsigned char *) pAlignedSource;
    while (num--) {

        *pByteDestination++ = *pByteSource++;
    }

    return pDestination;
}

//------------------------------------------------------------------------------
/// Fills a memory region with the given value. Returns a pointer to the
/// memory region.
/// \param pBuffer  Pointer to the start of the memory region to fill
/// \param value    Value to fill the region with
/// \param num      Size to fill in bytes
//------------------------------------------------------------------------------
void * memset(void *pBuffer, int value, size_t num)
{
    unsigned char *pByteDestination;
    unsigned int  *pAlignedDestination = (unsigned int *) pBuffer;
    unsigned int  alignedValue = (value << 24) | (value << 16) | (value << 8) | value;

    // Set words if possible
    if ((((unsigned int) pAlignedDestination & 0x3) == 0) && (num >= 4)) {
        while (num >= 4) {
            *pAlignedDestination++ = alignedValue;
            num -= 4;
        }
    }
    // Set remaining bytes
    pByteDestination = (unsigned char *) pAlignedDestination;
    while (num--) {
        *pByteDestination++ = value;
    }
    return pBuffer;
}

//-----------------------------------------------------------------------------
/// Search a character in the given string.
/// Returns a pointer to the character location.
/// \param pString   Pointer to the start of the string to search.
/// \param character The character to find.
//-----------------------------------------------------------------------------
char * strchr(const char *pString, int character)
{
    char * p = (char *)pString;
    char   c = character & 0xFF;

    while(*p != c) {
        if (*p == 0) {
            return 0;
        }
        p++;
    }
    return p;
}

//-----------------------------------------------------------------------------
/// Return the length of a given string
/// \param pString Pointer to the start of the string.
//-----------------------------------------------------------------------------
size_t strlen(const char *pString)
{
    unsigned int length = 0;

    while(*pString++ != 0) {
        length++;
    }
    return length;
}


//-----------------------------------------------------------------------------
/// Search a character backword from the end of given string.
/// Returns a pointer to the character location.
/// \param pString   Pointer to the start of the string to search.
/// \param character The character to find.
//-----------------------------------------------------------------------------
char * strrchr(const char *pString, int character)
{
    char *p = 0;

    while(*pString != 0) {
        if (*pString++ == character) {
            p = (char*)pString;
        }
    }
    return p;
}

//-----------------------------------------------------------------------------
/// Copy from source string to destination string
/// Return a pointer to the destination string
/// \param pDestination Pointer to the destination string.
/// \param pSource      Pointer to the source string.
//-----------------------------------------------------------------------------
char * strcpy(char *pDestination, const char *pSource)
{
    char *pSaveDest = pDestination;

    for(; (*pDestination = *pSource) != 0; ++pSource, ++pDestination);
    return pSaveDest;
}

//-----------------------------------------------------------------------------
/// Compare the first specified bytes of 2 given strings
/// Return 0 if equals
/// Return >0 if 1st string > 2nd string
/// Return <0 if 1st string < 2nd string
/// \param pString1 Pointer to the start of the 1st string.
/// \param pString2 Pointer to the start of the 2nd string.
/// \param count    Number of bytes that should be compared.
//-----------------------------------------------------------------------------
int strncmp(const char *pString1, const char *pString2, size_t count)
{
    int r;

    while(count) {
        r = *pString1 - *pString2;
        if (r == 0) {
            if (*pString1 == 0) {
                break;
            }
            pString1++;
            pString2++;
            count--;
            continue;
        }
        return r;
    }
    return 0;
}

//-----------------------------------------------------------------------------
/// Copy the first number of bytes from source string to destination string
/// Return the pointer to the destination string.
/// \param pDestination Pointer to the start of destination string.
/// \param pSource      Pointer to the start of the source string.
/// \param count        Number of bytes that should be copied.
//-----------------------------------------------------------------------------
char * strncpy(char *pDestination, const char *pSource, size_t count)
{
    char *pSaveDest = pDestination;

    while (count) {
        *pDestination = *pSource;
        if (*pSource == 0) {
            break;
        }
        pDestination++;
        pSource++;
        count--;
    }
    return pSaveDest;
}

