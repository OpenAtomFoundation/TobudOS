/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file contains utility functions
*
* \defgroup gr Opiga utility functions
* \ingroup infineon_api
* @{
*/
#ifndef _UTIL_H_
#define _UTIL_H_

#include "optiga/common/Datatypes.h"

///Greater than
#define  GREATER_THAN 0x00000001

///Lesser than
#define LESSER_THAN 0x00000002

///Equal
#define EQUAL 0x00000000

/// Default Low bound value value
#define DEFAULT_LOWBOUND_DOUBLEWORD 0x00000000

/// Mask double word value
#define MASK_DOUBLE_WORD 0xFFFFFFFF

///Word bit size
#define WORD_SIZE 32

///Successful Execution
#define UTIL_SUCCESS 0x00000000

///Execution Failure
#define UTIL_ERROR 0xFFFFFFFF

///Least significant bit set to high
#define LEAST_SIGNIFICANT_BIT_HIGH 0x00000001

///Least significant bit set to high
#define MOST_SIGNIFICANT_BIT_HIGH 0x80000000

/**
 * \brief structure to store the record sequence number
 */
typedef struct sUint64
{
    ///Higher 32 bit of the record sequence number
    uint32_t dwHigherByte;

    ///Lower 32 bit of the record sequence number
    uint32_t dwLowerByte;
}sUint64;

/**
 * \brief The function compares two uint64 data type.<br>
 */
int32_t CompareUint64(const sUint64 *PpsSrc1, const sUint64 *PpsSrc2);

/**
 * \brief Calculates the difference between the two uint64 data.<br>
 */
int32_t SubtractUint64(const sUint64 *PpsMinuend, const sUint64 *PpsSubtrahend,sUint64 *PpsDifference);

/**
 * \brief Adds two uint64 data.<br>
 */
int32_t AddUint64(const sUint64 *PpsSrc1, const sUint64 *PpsSrc2,sUint64 *PpsDest);

/**
 * \brief Increments uint64 data.<br>
 */
int32_t IncrementUint64(sUint64 *PpsSrc1);

/**
 * \brief Converts byte array to uint64.<br>
 */
int32_t GetUint64(sUint64 *PpsOut,const uint8_t* pbVal,uint16_t wLen);

/**
 * \brief To Slide the window by shift count.<br>
 */
int32_t ShiftLeftUint64(sUint64 *PpsWindow, sUint64 PsShiftCount, uint8_t PWindowSize, uint8_t PbMaxWindowSize);

/**
 * \brief To set a bit in the sUnit64 data.<br>
 */
int32_t Utility_SetBitUint64(sUint64* PprgbData, uint8_t bWindowSize, uint8_t bBitPosition);

/**
 * \brief Prepares uint16 [Big endian] type value from the buffer.<br>
 */
uint16_t Utility_GetUint16 (const uint8_t* PprgbData);

/**
 * \brief Prepares uint32 [Big endian] type value from the buffer .<br>
 */
uint32_t Utility_GetUint32 (const uint8_t* PprgbData);

/**
 * \brief Copies 2 bytes of uint16 type value to the buffer<br>
 */
void Utility_SetUint16 (puint8_t PprgbData,uint16_t PwValue);

/**
 * \brief Prepares uint24 [Big endian] type value from the buffer .<br>
 */
uint32_t Utility_GetUint24 (const uint8_t* PprgbData);

/**
 * \brief Copies LSB 3 bytes of uint32 [Big endian] type value to the buffer and store.<br>
 */
void Utility_SetUint24 (uint8_t* PprgbData,uint32_t Pdwvalue);

/**
 * \brief Copies LSB 4 bytes of uint32 [Big endian] type value to the buffer and store.<br>
 */
void Utility_SetUint32 (uint8_t* PprgbData,uint32_t Pdwvalue);

/**
 * \brief Copies the data from source buffer to destination buffer.<br>
 */
void Utility_Memmove(puint8_t PprgbDestBuf, const puint8_t PprgbSrcBuf, uint16_t PwLength);

#endif //_UTIL_H_

/**
* @}
*/


