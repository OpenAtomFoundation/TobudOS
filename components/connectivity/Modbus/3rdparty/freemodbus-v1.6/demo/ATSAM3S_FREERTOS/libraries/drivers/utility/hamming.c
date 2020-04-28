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

#include "hamming.h"
#include <utility/trace.h>
#include <utility/assert.h>

//------------------------------------------------------------------------------
//         Internal function
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Counts and return the number of bits set to '1' in the given byte.
/// \param byte  Byte to count.
//------------------------------------------------------------------------------
static unsigned char CountBitsInByte(unsigned char byte)
{
    unsigned char count = 0;
    while (byte > 0) {

        if (byte & 1) {

            count++;
        }
        byte >>= 1;
    }

    return count;
}

//------------------------------------------------------------------------------
/// Counts and return the number of bits set to '1' in the given hamming code.
/// \param code  Hamming code.
//------------------------------------------------------------------------------
static unsigned char CountBitsInCode256(unsigned char *code)
{
    return CountBitsInByte(code[0])
           + CountBitsInByte(code[1])
           + CountBitsInByte(code[2]);
}

//------------------------------------------------------------------------------
/// Calculates the 22-bit hamming code for a 256-bytes block of data.
/// \param data  Data buffer to calculate code for.
/// \param code  Pointer to a buffer where the code should be stored.
//------------------------------------------------------------------------------
static void Compute256(const unsigned char *data, unsigned char *code)
{
    unsigned int i;
    unsigned char columnSum = 0;
    unsigned char evenLineCode = 0;
    unsigned char oddLineCode = 0;
    unsigned char evenColumnCode = 0;
    unsigned char oddColumnCode = 0;

    // Xor all bytes together to get the column sum;
    // At the same time, calculate the even and odd line codes
    for (i=0; i < 256; i++) {

        columnSum ^= data[i];

        // If the xor sum of the byte is 0, then this byte has no incidence on
        // the computed code; so check if the sum is 1.
        if ((CountBitsInByte(data[i]) & 1) == 1) {

            // Parity groups are formed by forcing a particular index bit to 0
            // (even) or 1 (odd).
            // Example on one byte:
            // 
            // bits (dec)  7   6   5   4   3   2   1   0    
            //      (bin) 111 110 101 100 011 010 001 000    
            //                            '---'---'---'----------.
            //                                                   |
            // groups P4' ooooooooooooooo eeeeeeeeeeeeeee P4     |
            //        P2' ooooooo eeeeeee ooooooo eeeeeee P2     |
            //        P1' ooo eee ooo eee ooo eee ooo eee P1     |
            //                                                   |
            // We can see that:                                  |
            //  - P4  -> bit 2 of index is 0 --------------------'
            //  - P4' -> bit 2 of index is 1.
            //  - P2  -> bit 1 of index if 0.
            //  - etc...
            // We deduce that a bit position has an impact on all even Px if
            // the log2(x)nth bit of its index is 0
            //     ex: log2(4) = 2, bit2 of the index must be 0 (-> 0 1 2 3)
            // and on all odd Px' if the log2(x)nth bit of its index is 1
            //     ex: log2(2) = 1, bit1 of the index must be 1 (-> 0 1 4 5)
            // 
            // As such, we calculate all the possible Px and Px' values at the
            // same time in two variables, evenLineCode and oddLineCode, such as
            //     evenLineCode bits: P128  P64  P32  P16  P8  P4  P2  P1
            //     oddLineCode  bits: P128' P64' P32' P16' P8' P4' P2' P1'
            // 
            evenLineCode ^= (255 - i);
            oddLineCode ^= i;
        }
    }

    // At this point, we have the line parities, and the column sum. First, We
    // must caculate the parity group values on the column sum.
    for (i=0; i < 8; i++) {

        if (columnSum & 1) {

            evenColumnCode ^= (7 - i);
            oddColumnCode ^= i;
        }
        columnSum >>= 1;
    }

    // Now, we must interleave the parity values, to obtain the following layout:
    // Code[0] = Line1
    // Code[1] = Line2
    // Code[2] = Column
    // Line = Px' Px P(x-1)- P(x-1) ...
    // Column = P4' P4 P2' P2 P1' P1 PadBit PadBit 
    code[0] = 0;
    code[1] = 0;
    code[2] = 0;

    for (i=0; i < 4; i++) {

        code[0] <<= 2;
        code[1] <<= 2;
        code[2] <<= 2;

        // Line 1
        if ((oddLineCode & 0x80) != 0) {

            code[0] |= 2;
        }
        if ((evenLineCode & 0x80) != 0) {

            code[0] |= 1;
        }

        // Line 2
        if ((oddLineCode & 0x08) != 0) {

            code[1] |= 2;
        }
        if ((evenLineCode & 0x08) != 0) {

            code[1] |= 1;
        }

        // Column
        if ((oddColumnCode & 0x04) != 0) {

            code[2] |= 2;
        }
        if ((evenColumnCode & 0x04) != 0) {

            code[2] |= 1;
        }

        oddLineCode <<= 1;
        evenLineCode <<= 1;
        oddColumnCode <<= 1;
        evenColumnCode <<= 1;
    }

    // Invert codes (linux compatibility)
    code[0] = ~code[0];
    code[1] = ~code[1];
    code[2] = ~code[2];

    TRACE_DEBUG("Computed code = %02X %02X %02X\n\r",
              code[0], code[1], code[2]);
}

//------------------------------------------------------------------------------
/// Verifies and corrects a 256-bytes block of data using the given 22-bits
/// hamming code.
/// Returns 0 if there is no error, otherwise returns a HAMMING_ERROR code.
/// \param data  Data buffer to check.
/// \param originalCode  Hamming code to use for verifying the data.
//------------------------------------------------------------------------------
static unsigned char Verify256(
    unsigned char *data,
    const unsigned char *originalCode)
{
    // Calculate new code
    unsigned char computedCode[3];
    unsigned char correctionCode[3];
    Compute256(data, computedCode);

    // Xor both codes together
    correctionCode[0] = computedCode[0] ^ originalCode[0];
    correctionCode[1] = computedCode[1] ^ originalCode[1];
    correctionCode[2] = computedCode[2] ^ originalCode[2];

    TRACE_DEBUG("Correction code = %02X %02X %02X\n\r",
              correctionCode[0], correctionCode[1], correctionCode[2]);

    // If all bytes are 0, there is no error
    if ((correctionCode[0] == 0)
        && (correctionCode[1] == 0)
        && (correctionCode[2] == 0)) {

        return 0;
    }
    // If there is a single bit error, there are 11 bits set to 1
    if (CountBitsInCode256(correctionCode) == 11) {

        // Get byte and bit indexes
        unsigned char byte = correctionCode[0] & 0x80;
        byte |= (correctionCode[0] << 1) & 0x40;
        byte |= (correctionCode[0] << 2) & 0x20;
        byte |= (correctionCode[0] << 3) & 0x10;

        byte |= (correctionCode[1] >> 4) & 0x08;
        byte |= (correctionCode[1] >> 3) & 0x04;
        byte |= (correctionCode[1] >> 2) & 0x02;
        byte |= (correctionCode[1] >> 1) & 0x01;

        unsigned char bit = (correctionCode[2] >> 5) & 0x04;
        bit |= (correctionCode[2] >> 4) & 0x02;
        bit |= (correctionCode[2] >> 3) & 0x01;

        // Correct bit
        TRACE_DEBUG("Correcting byte #%d at bit %d\n\r", byte, bit);
        data[byte] ^= (1 << bit);

        return Hamming_ERROR_SINGLEBIT;
    }
    // Check if ECC has been corrupted
    if (CountBitsInCode256(correctionCode) == 1) {

        return Hamming_ERROR_ECC;
    }
    // Otherwise, this is a multi-bit error
    else {

        return Hamming_ERROR_MULTIPLEBITS;
    }
}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Computes 3-bytes hamming codes for a data block whose size is multiple of
/// 256 bytes. Each 256 bytes block gets its own code.
/// \param data  Data to compute code for.
/// \param size  Data size in bytes.
/// \param code  Codes buffer.
//------------------------------------------------------------------------------
void Hamming_Compute256x(
    const unsigned char *data,
    unsigned int size,
    unsigned char *code)
{
    TRACE_DEBUG("Hamming_Compute256x()\n\r");

    while (size > 0) {

        Compute256(data, code);
        data += 256;
        code += 3;
        size -= 256;
    }
}

//------------------------------------------------------------------------------
/// Verifies 3-bytes hamming codes for a data block whose size is multiple of
/// 256 bytes. Each 256-bytes block is verified with its own code.
/// Returns 0 if the data is correct, Hamming_ERROR_SINGLEBIT if one or more
/// block(s) have had a single bit corrected, or either Hamming_ERROR_ECC
/// or Hamming_ERROR_MULTIPLEBITS.
/// \param data  Data buffer to verify.
/// \param size  Size of the data in bytes.
/// \param code  Original codes.
//------------------------------------------------------------------------------
unsigned char Hamming_Verify256x(
    unsigned char *data,
    unsigned int size,
    const unsigned char *code)
{
    unsigned char error;
    unsigned char result = 0;

    TRACE_DEBUG("Hamming_Verify256x()\n\r");

    while (size > 0) {

        error = Verify256(data, code);
        if (error == Hamming_ERROR_SINGLEBIT) {

            result = Hamming_ERROR_SINGLEBIT;
        }
        else if (error) {

            return error;
        }

        data += 256;
        code += 3;
        size -= 256;
    }

    return result;
}

