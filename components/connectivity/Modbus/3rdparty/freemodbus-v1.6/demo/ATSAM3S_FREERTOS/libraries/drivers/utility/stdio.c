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
/// Implementation of several stdio.h methods, such as printf(), sprintf() and
/// so on. This reduces the memory footprint of the binary when using those
/// methods, compared to the libc implementation.
///
/// !Usage
///
/// Adds stdio.c to the list of file to compile for the project. This will
/// automatically replace libc methods by the custom ones.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdarg.h>

//------------------------------------------------------------------------------
//         Local Definitions
//------------------------------------------------------------------------------

// Maximum string size allowed (in bytes).
#define MAX_STRING_SIZE         100

//------------------------------------------------------------------------------
//         Global Variables
//------------------------------------------------------------------------------

// Required for proper compilation.
struct _reent r = {0, (FILE *) 0, (FILE *) 1, (FILE *) 0};
struct _reent *_impure_ptr = &r;

//------------------------------------------------------------------------------
//         Local Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Writes a character inside the given string. Returns 1.
// \param pStr  Storage string.
// \param c  Character to write.
//------------------------------------------------------------------------------
signed int PutChar(char *pStr, char c)
{
    *pStr = c;
    return 1;
}

//------------------------------------------------------------------------------
// Writes a string inside the given string.
// Returns the size of the written
// string.
// \param pStr  Storage string.
// \param pSource  Source string.
//------------------------------------------------------------------------------
signed int PutString(char *pStr, const char *pSource)
{
    signed int num = 0;

    while (*pSource != 0) {

        *pStr++ = *pSource++;
        num++;
    }

    return num;
}

//------------------------------------------------------------------------------
// Writes an unsigned int inside the given string, using the provided fill &
// width parameters.
// Returns the size in characters of the written integer.
// \param pStr  Storage string.
// \param fill  Fill character.
// \param width  Minimum integer width.
// \param value  Integer value.
//------------------------------------------------------------------------------
signed int PutUnsignedInt(
    char *pStr,
    char fill,
    signed int width,
    unsigned int value)
{
    signed int num = 0;

    // Take current digit into account when calculating width
    width--;

    // Recursively write upper digits
    if ((value / 10) > 0) {

        num = PutUnsignedInt(pStr, fill, width, value / 10);
        pStr += num;
    }
    // Write filler characters
    else {

        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }
    }

    // Write lower digit
    num += PutChar(pStr, (value % 10) + '0');

    return num;
}

//------------------------------------------------------------------------------
// Writes a signed int inside the given string, using the provided fill & width
// parameters.
// Returns the size of the written integer.
// \param pStr  Storage string.
// \param fill  Fill character.
// \param width  Minimum integer width.
// \param value  Signed integer value.
//------------------------------------------------------------------------------
signed int PutSignedInt(
    char *pStr,
    char fill,
    signed int width,
    signed int value)
{
    signed int num = 0;
    unsigned int absolute;

    // Compute absolute value
    if (value < 0) {

        absolute = -value;
    }
    else {

        absolute = value;
    }

    // Take current digit into account when calculating width
    width--;

    // Recursively write upper digits
    if ((absolute / 10) > 0) {

        if (value < 0) {
        
            num = PutSignedInt(pStr, fill, width, -(absolute / 10));
        }
        else {

            num = PutSignedInt(pStr, fill, width, absolute / 10);
        }
        pStr += num;
    }
    else {

        // Reserve space for sign
        if (value < 0) {

            width--;
        }

        // Write filler characters
        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }

        // Write sign
        if (value < 0) {

            num += PutChar(pStr, '-');
            pStr++;
        }
    }

    // Write lower digit
    num += PutChar(pStr, (absolute % 10) + '0');

    return num;
}

//------------------------------------------------------------------------------
// Writes an hexadecimal value into a string, using the given fill, width &
// capital parameters.
// Returns the number of char written.
// \param pStr  Storage string.
// \param fill  Fill character.
// \param width  Minimum integer width.
// \param maj  Indicates if the letters must be printed in lower- or upper-case.
// \param value  Hexadecimal value.
//------------------------------------------------------------------------------
signed int PutHexa(
    char *pStr,
    char fill,
    signed int width,
    unsigned char maj,
    unsigned int value)
{
    signed int num = 0;

    // Decrement width
    width--;

    // Recursively output upper digits
    if ((value >> 4) > 0) {

        num += PutHexa(pStr, fill, width, maj, value >> 4);
        pStr += num;
    }
    // Write filler chars
    else {

        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }
    }

    // Write current digit
    if ((value & 0xF) < 10) {

        PutChar(pStr, (value & 0xF) + '0');
    }
    else if (maj) {

        PutChar(pStr, (value & 0xF) - 10 + 'A');
    }
    else {

        PutChar(pStr, (value & 0xF) - 10 + 'a');
    }
    num++;

    return num;
}

//------------------------------------------------------------------------------
//         Global Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Stores the result of a formatted string into another string. Format
/// arguments are given in a va_list instance.
/// Return the number of characters written.
/// \param pStr    Destination string.
/// \param length  Length of Destination string.
/// \param pFormat Format string.
/// \param ap      Argument list.
//------------------------------------------------------------------------------
signed int vsnprintf(char *pStr, size_t length, const char *pFormat, va_list ap)
{
    char          fill;
    unsigned char width;
    signed int    num = 0;
    signed int    size = 0;

    // Clear the string
    if (pStr) {

        *pStr = 0;
    }

    // Phase string
    while (*pFormat != 0 && size < length) {

        // Normal character
        if (*pFormat != '%') {

            *pStr++ = *pFormat++;
            size++;
        }
        // Escaped '%'
        else if (*(pFormat+1) == '%') {

            *pStr++ = '%';
            pFormat += 2;
            size++;
        }
        // Token delimiter
        else {

            fill = ' ';
            width = 0;
            pFormat++;

            // Parse filler
            if (*pFormat == '0') {

                fill = '0';
                pFormat++;
            }

            // Parse width
            while ((*pFormat >= '0') && (*pFormat <= '9')) {
        
                width = (width*10) + *pFormat-'0';
                pFormat++;
            }

            // Check if there is enough space
            if (size + width > length) {

                width = length - size;
            }
        
            // Parse type
            switch (*pFormat) {
            case 'd': 
            case 'i': num = PutSignedInt(pStr, fill, width, va_arg(ap, signed int)); break;
            case 'u': num = PutUnsignedInt(pStr, fill, width, va_arg(ap, unsigned int)); break;
            case 'x': num = PutHexa(pStr, fill, width, 0, va_arg(ap, unsigned int)); break;
            case 'X': num = PutHexa(pStr, fill, width, 1, va_arg(ap, unsigned int)); break;
            case 's': num = PutString(pStr, va_arg(ap, char *)); break;
            case 'c': num = PutChar(pStr, va_arg(ap, unsigned int)); break;
            default:
                return EOF;
            }

            pFormat++;
            pStr += num;
            size += num;
        }
    }

    // NULL-terminated (final \0 is not counted)
    if (size < length) {

        *pStr = 0;
    }
    else {

        *(--pStr) = 0;
        size--;
    }

    return size;
}

//------------------------------------------------------------------------------
/// Stores the result of a formatted string into another string. Format
/// arguments are given in a va_list instance.
/// Return the number of characters written.
/// \param pString Destination string.
/// \param length  Length of Destination string.
/// \param pFormat Format string.
/// \param ...     Other arguments
//------------------------------------------------------------------------------
signed int snprintf(char *pString, size_t length, const char *pFormat, ...)
{
    va_list    ap;
    signed int rc;

    va_start(ap, pFormat);
    rc = vsnprintf(pString, length, pFormat, ap);
    va_end(ap);

    return rc;
}

//------------------------------------------------------------------------------
/// Stores the result of a formatted string into another string. Format
/// arguments are given in a va_list instance.
/// Return the number of characters written.
/// \param pString  Destination string.
/// \param pFormat  Format string.
/// \param ap       Argument list.
//------------------------------------------------------------------------------
signed int vsprintf(char *pString, const char *pFormat, va_list ap)
{
    return vsnprintf(pString, MAX_STRING_SIZE, pFormat, ap);
}

//------------------------------------------------------------------------------
/// Outputs a formatted string on the given stream. Format arguments are given
/// in a va_list instance.
/// \param pStream  Output stream.
/// \param pFormat  Format string
/// \param ap  Argument list.
//------------------------------------------------------------------------------
signed int vfprintf(FILE *pStream, const char *pFormat, va_list ap)
{
    char pStr[MAX_STRING_SIZE];
    char pError[] = "stdio.c: increase MAX_STRING_SIZE\n\r";

    // Write formatted string in buffer
    if (vsprintf(pStr, pFormat, ap) >= MAX_STRING_SIZE) {

        fputs(pError, stderr);
        while (1); // Increase MAX_STRING_SIZE
    }

    // Display string
    return fputs(pStr, pStream);
}

//------------------------------------------------------------------------------
/// Outputs a formatted string on the DBGU stream. Format arguments are given
/// in a va_list instance.
/// \param pFormat  Format string
/// \param ap  Argument list.
//------------------------------------------------------------------------------
signed int vprintf(const char *pFormat, va_list ap)
{
    return vfprintf(stdout, pFormat, ap);
}

//------------------------------------------------------------------------------
/// Outputs a formatted string on the given stream, using a variable number of
/// arguments.
/// \param pStream  Output stream.
/// \param pFormat  Format string.
//------------------------------------------------------------------------------
signed int fprintf(FILE *pStream, const char *pFormat, ...)
{
    va_list ap;
    signed int result;

    // Forward call to vfprintf
    va_start(ap, pFormat);
    result = vfprintf(pStream, pFormat, ap);
    va_end(ap);

    return result;
}

//------------------------------------------------------------------------------
/// Outputs a formatted string on the DBGU stream, using a variable number of
/// arguments.
/// \param pFormat  Format string.
//------------------------------------------------------------------------------
signed int printf(const char *pFormat, ...)
{
    va_list ap;
    signed int result;

    // Forward call to vprintf
    va_start(ap, pFormat);
    result = vprintf(pFormat, ap);
    va_end(ap);

    return result;
}

//------------------------------------------------------------------------------
/// Writes a formatted string inside another string.
/// \param pStr  Storage string.
/// \param pFormat  Format string.
//------------------------------------------------------------------------------
signed int sprintf(char *pStr, const char *pFormat, ...)
{
    va_list ap;
    signed int result;

    // Forward call to vsprintf
    va_start(ap, pFormat);
    result = vsprintf(pStr, pFormat, ap);
    va_end(ap);

    return result;
}

//------------------------------------------------------------------------------
/// Outputs a string on stdout.
/// \param pStr  String to output.
//------------------------------------------------------------------------------
signed int puts(const char *pStr)
{
    return fputs(pStr, stdout);
}
