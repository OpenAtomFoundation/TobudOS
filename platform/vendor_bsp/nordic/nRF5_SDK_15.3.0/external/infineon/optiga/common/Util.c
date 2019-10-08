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
*
* \file
*
* \brief This file contains utility functions
*
*
*
*/
#ifndef WIN32

#endif
#ifdef WIN32
#include <Windows.h>
#include <stdio.h>
#endif
#include "optiga/common/Util.h"


/**
 *
 * Compare PprgbBuf1 uint64 data with the PprgbBuf2 uint64 data.<br>
 *
 * \param[in]  PpsSrc1  Pointer to the uint64 structure
 * \param[in]  PpsSrc2  Pointer to the uint64 structure
 *
 * \retval    GREATER_THAN if PpsSrc1 > PpsSrc2
 * \retval    LESSER_THAN if PpsSrc1 < PpsSrc2
 * \retval    EQUAL if PpsSrc1 == PpsSrc2
 * \retval    UTIL_ERROR for NULL parameter
 *
 */
int32_t CompareUint64(const sUint64 *PpsSrc1, const sUint64 *PpsSrc2)
 {
     int32_t i4Retval = (int32_t) UTIL_ERROR;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if((NULL == PpsSrc1) || (NULL == PpsSrc2))
        {
            break;
        }
#endif
        if(PpsSrc1->dwHigherByte == PpsSrc2->dwHigherByte)
            {
                if(PpsSrc1->dwLowerByte > PpsSrc2->dwLowerByte)
                {
                    i4Retval = GREATER_THAN;
                }
                else if(PpsSrc1->dwLowerByte < PpsSrc2->dwLowerByte)
                {
                    i4Retval = LESSER_THAN;
                }
                //PpsSrc1->dwLowerByte == PpsSrc2->dwLowerByte
                else
                {
                    i4Retval = EQUAL;
                }
            }
        else
        {
            if(PpsSrc1->dwHigherByte > PpsSrc2->dwHigherByte)
            {
                i4Retval = GREATER_THAN;
            }
            // PpsSrc1->dwHigherByte < PpsSrc2->dwHigherByte
            else
            {
                i4Retval = LESSER_THAN;
            }
        }
    }while(0);

    return i4Retval;
 }

/**
* Subtraction of PpsSubtrahend uint64 data type from PpsMinuend uint64 data
* PpsMinuend should be greater than PpsSubtrahend else will return error
*
* \param[in]  PpsMinuend    Minued
* \param[in]  PpsSubtrahend Subtrahend
* \param[in]  PpsDifference Difference
*
* \retval    UTIL_SUCCESS if PpsMinuend > PpsSubtrahend
* \retval    UTIL_ERROR if PpsMinuend < PpsSubtrahend and for NULL parameter
*/
int32_t SubtractUint64(const sUint64 *PpsMinuend, const sUint64 *PpsSubtrahend,sUint64 *PpsDifference)
{
    int32_t i4Retval = (int32_t) UTIL_ERROR;
    sUint64 sIntermediateVal;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if((NULL == PpsMinuend) || (NULL == PpsSubtrahend) || (NULL == PpsDifference))
        {
            break;
        }
#endif
        i4Retval = CompareUint64(PpsMinuend, PpsSubtrahend);

        //Check if Minuend is greater than Subtrahend to avoid overflow
        if((GREATER_THAN != i4Retval) && (EQUAL != i4Retval))
        {
            i4Retval = (int32_t) UTIL_ERROR;
            break;
        }

        sIntermediateVal.dwLowerByte = PpsMinuend->dwLowerByte - PpsSubtrahend->dwLowerByte;
        sIntermediateVal.dwHigherByte = PpsMinuend->dwHigherByte - PpsSubtrahend->dwHigherByte;

        if(sIntermediateVal.dwLowerByte > PpsMinuend->dwLowerByte)
        {
            --sIntermediateVal.dwHigherByte;
        }

        PpsDifference->dwLowerByte = sIntermediateVal.dwLowerByte;
        PpsDifference->dwHigherByte = sIntermediateVal.dwHigherByte;

        i4Retval = (int32_t) UTIL_SUCCESS;

    }while(0);
    return i4Retval;
}

/**
* Shift left the PpsWindow uint64 data for shiftcount times based on the window size
*
*/
int32_t ShiftLeftUint64(sUint64 *PpsWindow, sUint64 PsShiftCount, uint8_t PbWindowSize, uint8_t PbMaxWindowSize)
{
    int32_t i4Retval = (int32_t) UTIL_ERROR;
    uint32_t dwShiftCount = 0;
    uint32_t dwMaskShiftCount = MASK_DOUBLE_WORD;
    sUint64 sWindowSize = {0} ;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if(NULL == PpsWindow)
        {
            break;
        }
#endif
        sWindowSize.dwLowerByte = (uint32_t) PbWindowSize;

        i4Retval = CompareUint64(&PsShiftCount, &sWindowSize);

        //If Shift Count size is greater than or equal to window size
        if((GREATER_THAN == i4Retval) || (EQUAL == i4Retval))
        {
            ///Set the window with all bit zero
            PpsWindow->dwLowerByte =  DEFAULT_LOWBOUND_DOUBLEWORD;
            PpsWindow->dwHigherByte =  DEFAULT_LOWBOUND_DOUBLEWORD;
            i4Retval = (int32_t) UTIL_SUCCESS;
            break;
        }

        //If Shift count is less than the window size

        //Set the window shift count
        dwShiftCount = PsShiftCount.dwLowerByte;
        //Mask the lower byte data
        dwMaskShiftCount &= PpsWindow->dwLowerByte ;

        //If window size is equal to 32
        if(WORD_SIZE == PbWindowSize)
        {
            ///Shift only higher byte if window size is 32
            PpsWindow->dwLowerByte = DEFAULT_LOWBOUND_DOUBLEWORD ;

            PpsWindow->dwHigherByte <<= dwShiftCount ;
            i4Retval = (int32_t) UTIL_SUCCESS;
            break;
        }
        //Shift count is greater than 32
        if(WORD_SIZE <= dwShiftCount)
        {
            //Set the lower byte to zero which is equal to shift by 32
            PpsWindow->dwLowerByte = DEFAULT_LOWBOUND_DOUBLEWORD ;
            //Set the Higher byte to zero which is equal to shift by 32
            PpsWindow->dwHigherByte  =  DEFAULT_LOWBOUND_DOUBLEWORD ;
            //copy the data of lower byte
            PpsWindow->dwHigherByte |= dwMaskShiftCount;
            //Remaining shift count to be done
            dwShiftCount -= WORD_SIZE;
            //Shift the left higher byte data with the remaining count
            PpsWindow->dwHigherByte <<= dwShiftCount;
            //Reset the outside of window bits
            PpsWindow->dwHigherByte &= MASK_DOUBLE_WORD >> (PbMaxWindowSize - PbWindowSize);

            i4Retval = (int32_t) UTIL_SUCCESS;
            break;
        }
        //Shift count is lesser than 32

        //Shift the lower byte for shift count times
        PpsWindow->dwLowerByte <<= dwShiftCount ;
        //Shift the higher byte for shift count times
        PpsWindow->dwHigherByte <<= dwShiftCount ;
        //To mask the data to be copied to higher byte
        dwMaskShiftCount >>= WORD_SIZE - dwShiftCount;
        //Copy data shifted from lower byte to higher byte
        PpsWindow->dwHigherByte |= dwMaskShiftCount;
        //Reset the outside of window bits
        PpsWindow->dwHigherByte &= MASK_DOUBLE_WORD >> (PbMaxWindowSize - PbWindowSize);

        i4Retval = (int32_t) UTIL_SUCCESS;

    }while(0);
    return i4Retval;
}


/**
* Addition of two uint64 data type
*
*/
int32_t AddUint64(const sUint64 *PpsSrc1, const sUint64 *PpsSrc2,sUint64 *PpsDest)
{
    int32_t i4Retval = (int32_t) UTIL_ERROR;
    sUint64 sIntermediateval;

    sIntermediateval.dwLowerByte = PpsSrc1->dwLowerByte + PpsSrc2->dwLowerByte;
    sIntermediateval.dwHigherByte = PpsSrc1->dwHigherByte + PpsSrc2->dwHigherByte;

    if(sIntermediateval.dwLowerByte < PpsSrc1->dwLowerByte)
    {
        ++sIntermediateval.dwHigherByte;
    }

    PpsDest->dwLowerByte = sIntermediateval.dwLowerByte;
    PpsDest->dwHigherByte = sIntermediateval.dwHigherByte;
    i4Retval = (int32_t) UTIL_SUCCESS;
    return i4Retval;
}

int32_t IncrementUint64(sUint64 *PpsSrc1)
{
    int32_t i4Retval = (int32_t) UTIL_ERROR;
    sUint64 sOne;
    sOne.dwHigherByte = 0;
    sOne.dwLowerByte = 1;
    i4Retval = AddUint64(PpsSrc1,&sOne,PpsSrc1);
    return i4Retval;
}

/**
 *
 * Prepares uint16 [Big endian] type value from the buffer.<br>
 *
 * \param[in]  PprgbData    Pointer to the buffer
 *
 * \retval    return 16 bit value
 *
 */
uint16_t Utility_GetUint16 (const uint8_t* PprgbData)
{
    uint16_t wVal;

    wVal = (uint16_t)(*PprgbData << 8);
    wVal |= (uint16_t)(*(PprgbData+1));

    return wVal;
}

/**
 *
 * Copies LSB 3 bytes of uint32 [Big endian] type value to the buffer and store .<br>
 *
 * \param[in,out]  PprgbData    Pointer to the buffer
 * \param[in]      Pdwvalue     32 bit value
 *
 */
void Utility_SetUint24 (uint8_t* PprgbData,uint32_t Pdwvalue)
{
    #define prgbBuffer PprgbData

    *(prgbBuffer) = (uint8_t)(Pdwvalue>>16);
    *(prgbBuffer+1) = (uint8_t)(Pdwvalue>>8);
    *(prgbBuffer+2) = (uint8_t)(Pdwvalue);

    #undef prgbBuffer
}

/**
 *
 * Prepares uint24 [Big endian] type value from the buffer and store .<br>
 *
 * \param[in]  PprgbData    Pointer to the buffer
 *
 * \retval    return 32 bit value
 *
 */
uint32_t Utility_GetUint24 (const uint8_t* PprgbData)
{
    uint32_t dwVal;
    dwVal = ((uint32_t)(*PprgbData))<< 16;
    dwVal |= ((uint32_t)(*(PprgbData+1)))<<8;
    dwVal |= (uint32_t)(*(PprgbData+2));

    return dwVal;
}

/**
 *
 * Prepares uint24 [Big endian] type value from the buffer and store .<br>
 *
 * \param[in]  PprgbData    Pointer to the buffer
 *
 * \retval    return 32 bit value
 *
 */
uint32_t Utility_GetUint32 (const uint8_t* PprgbData)
{
    uint32_t dwVal;
    dwVal = ((uint32_t)(*PprgbData))<< 24 | ((uint32_t)(*(PprgbData + 1))<< 16 | ((uint32_t)(*(PprgbData + 2)))<< 8 | (uint32_t)(*(PprgbData + 3)));

    return dwVal;
}

/**
 *
 * Copies 2 bytes of uint16 type value to the buffer<br>
 *
 * \param[in,out]  PprgbData    Pointer to the buffer
 * \param[in]      PwValue      16 bit value
 *
 */
void Utility_SetUint16 (puint8_t PprgbData,uint16_t PwValue)
{
    *PprgbData = (uint8_t)(PwValue>>8);
    *(PprgbData+1) = (uint8_t)(PwValue);
}

/**
 *
 * sets the bit position to high in the window frame.<br>
 *
 * \param[in,out]  PprgbData    Pointer to the uint64 data
 * \param[in]      bWindowSize  Window size
 * \param[in]      bBitPosition bit position from low bound of the window(Bit is set at lowbound+bBitPosition )
 *
 * \retval    UTIL_SUCCESS for successful execution
 * \retval    UTIL_ERROR if execution is failure
 */
int32_t Utility_SetBitUint64(sUint64* PprgbData, uint8_t bWindowSize, uint8_t bBitPosition)
{
    int32_t i4Retval = (int32_t) UTIL_ERROR;
    uint8_t bShiftCount = 0;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if(NULL == PprgbData)
        {
            break;
        }
#endif
        if(bBitPosition > bWindowSize)
        {
            break;
        }

        //Window size is equal to bit position
        if(bBitPosition == bWindowSize)
        {
            if(WORD_SIZE == bWindowSize)
            {
                PprgbData->dwHigherByte |= LEAST_SIGNIFICANT_BIT_HIGH;
                i4Retval = (int32_t) UTIL_SUCCESS;
                break;
            }

            PprgbData->dwLowerByte |= LEAST_SIGNIFICANT_BIT_HIGH;
            i4Retval = (int32_t) UTIL_SUCCESS;
            break;
        }

        //Bit Position from the Higher bound
        bShiftCount = bWindowSize - bBitPosition;
        //Window size is equal to 32
        if(WORD_SIZE == bWindowSize)
        {
            PprgbData->dwHigherByte |= LEAST_SIGNIFICANT_BIT_HIGH << (bShiftCount - 1);
            i4Retval = (int32_t) UTIL_SUCCESS;
            break;
        }
        //If Bit position from High Bound is greater than 32 then bit set is to be in Higher byte
        if(WORD_SIZE < bShiftCount)
        {
            bShiftCount -= WORD_SIZE;

            PprgbData->dwHigherByte |= LEAST_SIGNIFICANT_BIT_HIGH << (bShiftCount - 1);

            i4Retval = (int32_t) UTIL_SUCCESS;
            break;
        }

        //If Bit position from High Bound is lesser or equal 32 then bit set is to be in lower byte
        PprgbData->dwLowerByte |= LEAST_SIGNIFICANT_BIT_HIGH << (bShiftCount - 1);

        i4Retval = (int32_t) UTIL_SUCCESS;

    }while(0);
    return i4Retval;
}


/**
 *
 * Copies 4 bytes of uint32 [Big endian] type value to the buffer and store .<br>
 *
 * \param[in,out]  PprgbData    Pointer to the buffer
 * \param[in]      Pdwvalue     32 bit value
 *
 */
void Utility_SetUint32 (uint8_t* PprgbData,uint32_t Pdwvalue)
{
    #define prgbBuffer PprgbData

    *(prgbBuffer) = (uint8_t)(Pdwvalue>>24);
    *(prgbBuffer + 1) = (uint8_t)(Pdwvalue>>16);
    *(prgbBuffer+2) = (uint8_t)(Pdwvalue>>8);
    *(prgbBuffer+3) = (uint8_t)(Pdwvalue);

    #undef prgbBuffer
}

/**
 *
 * Copies the data from source buffer to destination buffer.<br>
 *
 * \param[in,out]  PprgbDestBuf Pointer to the destination buffer
 * \param[in,out]  PprgbSrcBuf  Pointer to the source buffer
 * \param[in]      PwLength     Number of bytes to be copied/moved
 *
 */
void Utility_Memmove(puint8_t PprgbDestBuf, const puint8_t PprgbSrcBuf, uint16_t PwLength)
{
    uint16_t wIndex=0;
    puint8_t pTempSrcBuf = PprgbSrcBuf;

    do
    {
        //if source and destination are the same buffer. and the buffers overlap
        if((PprgbDestBuf > pTempSrcBuf) && (PprgbDestBuf <= (pTempSrcBuf + PwLength - 1)))
        {
            while(0 < PwLength)
            {
                PwLength -= 1;
                *(PprgbDestBuf + PwLength) = *(pTempSrcBuf + PwLength);
            }
        }
        else
        {
            while(wIndex < PwLength)
            {
                *(PprgbDestBuf + wIndex) = *(pTempSrcBuf + wIndex);
                wIndex++;
            }
        }
    }while(0);
}
