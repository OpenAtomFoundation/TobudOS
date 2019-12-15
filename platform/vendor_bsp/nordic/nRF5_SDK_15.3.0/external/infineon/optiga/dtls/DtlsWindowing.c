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
* \brief   This file implements the APIs, types used in the
*          for DTLS windowing.
*
* \addtogroup  grMutualAuth
* @{
*/

#include <stdint.h>
#include "optiga/dtls/DtlsWindowing.h"
#include "optiga/dtls/DtlsRecordLayer.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

/// @cond hidden

///Maximum window size supported
#define MAX_WINDOW_SIZE 64

/// @endcond

/**
 * Implementation for Record Replay Detection.<br>
 * Return status as #OCP_RL_WINDOW_IGNORE if record is already received or record sequence number is less then lower bound of window.<br>
 * Under some erroneous conditions, error codes from Record Layer can also be returned.<br>
 *
 * \param[in]   PpsWindow   Pointer to the structure that contains details required for windowing like
 *                          record sequence number, lower and higher boundaries.
 *
 * \retval      OCP_RL_WINDOW_UPDATED       Valid record is received and window is updated.
 * \retval      OCP_RL_WINDOW_MOVED         Valid record is received and window is updated and moved.
 * \retval      OCP_RL_WINDOW_IGNORE        Failure in execution and record already received.
 *
 */
int32_t DtlsCheckReplay(sWindow_d *PpsWindow)
{
    int32_t i4Status = (int32_t) OCP_RL_WINDOW_IGNORE;
    int32_t i4Retval;
    sUint64 sIntermidateVal;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if((NULL == PpsWindow) || (NULL == PpsWindow->fValidateRecord))
        {
            break;
        }
#endif
        if((MAX_WINDOW_SIZE < PpsWindow->bWindowSize) || (WORD_SIZE > PpsWindow->bWindowSize))
        {
            break;
        }

        //Compare the received sequence number with the Lower window boundary
        i4Retval = CompareUint64(&PpsWindow->sRecvSeqNumber, &PpsWindow->sLowerBound);

        //If sequence number is lesser than the low bound of window
        if(LESSER_THAN == i4Retval)
        {
            break;
        }

        //If sequence number is greater than low bound window
        //Compare the received sequence number with the Higher window boundary
        i4Retval = CompareUint64(&PpsWindow->sRecvSeqNumber, &PpsWindow->sHigherBound);

        //If Sequence number is greater than high bound of the window
        //Slide the window
        if(GREATER_THAN == i4Retval)
        {
            //Record validation
            i4Retval = PpsWindow->fValidateRecord(PpsWindow->pValidateArgs);
            //If record validation fails
            if(OCP_RL_OK != i4Retval)
            {
                if(((int32_t)CMD_LIB_DECRYPT_FAILURE == i4Retval) || ((int32_t)OCP_RL_MALLOC_FAILURE == i4Retval))
                {
                    i4Status = i4Retval;
                }
                break;
            }
            else
            {
                //Calculate the count to slide the window
                //lint --e{534} suppress "The return value check is suppressed as this function always return Success.Only error condition where
                //RecvSeqNumber <  sHigherBound is not possible as it will enter this path only when RecvSeqNumber >  sHigherBound"
                i4Retval = SubtractUint64(&PpsWindow->sRecvSeqNumber, &PpsWindow->sHigherBound, &sIntermidateVal);

                //Slide the window
                i4Retval = ShiftLeftUint64(&PpsWindow->sWindowFrame, sIntermidateVal, PpsWindow->bWindowSize, (uint8_t)MAX_WINDOW_SIZE);
                if(UTIL_SUCCESS != i4Retval)
                {
                    break;
                }
                //Set the sequence number received as the Higher Bound
                PpsWindow->sHigherBound = PpsWindow->sRecvSeqNumber;

                sIntermidateVal.dwHigherByte = DEFAULT_LOWBOUND_DOUBLEWORD ;
                sIntermidateVal.dwLowerByte = (uint32_t)PpsWindow->bWindowSize - 1;

                //Difference of Higher bound and window size is set as lower bound
                i4Retval = SubtractUint64(&PpsWindow->sHigherBound, &sIntermidateVal, &PpsWindow->sLowerBound);
                if(UTIL_SUCCESS != i4Retval)
                {
                    break;
                }
                //Set the bit position of sequence number to 1 which is the MSB of the window frame
                i4Retval = Utility_SetBitUint64(&PpsWindow->sWindowFrame, PpsWindow->bWindowSize, PpsWindow->bWindowSize);
                if(UTIL_SUCCESS != i4Retval)
                {
                    break;
                }

                i4Status = (int32_t) OCP_RL_WINDOW_MOVED;
                break;
            }
        }
        //Compare the received sequence number with the Higher and Lower window boundary
        //lint --e{534} suppress "The return value check is suppressed as this function always return Success.Only error condition where
        //RecvSeqNumber >  sHigherBound is not possible as it will enter this path only when RecvSeqNumber <  sHigherBound"
        //Calculate bit position of sequence number from high bound of the window
        i4Retval = SubtractUint64(&PpsWindow->sHigherBound, &PpsWindow->sRecvSeqNumber, &sIntermidateVal);

        //If window size is equal to 32
        if(WORD_SIZE == PpsWindow->bWindowSize)
        {
            if((MOST_SIGNIFICANT_BIT_HIGH == ((PpsWindow->sWindowFrame.dwHigherByte << (uint32_t)((WORD_SIZE - sIntermidateVal.dwLowerByte) - 1))
            & MOST_SIGNIFICANT_BIT_HIGH)))
            {
                break;
            }
        }
        else
        {
            //Received sequence number is in the lower byte of the window frame
            if((DEFAULT_LOWBOUND_DOUBLEWORD == sIntermidateVal.dwHigherByte) && (sIntermidateVal.dwLowerByte < WORD_SIZE))
            {
                if((MOST_SIGNIFICANT_BIT_HIGH == ((PpsWindow->sWindowFrame.dwLowerByte << (uint32_t)((WORD_SIZE - sIntermidateVal.dwLowerByte) - 1 )) & MOST_SIGNIFICANT_BIT_HIGH)))
                {
                    break;
                }
            }
            //Received sequence number is in the higher byte of the window frame
            else if((DEFAULT_LOWBOUND_DOUBLEWORD == sIntermidateVal.dwHigherByte) && (sIntermidateVal.dwLowerByte >= WORD_SIZE))
            {
                if((MOST_SIGNIFICANT_BIT_HIGH == ((PpsWindow->sWindowFrame.dwHigherByte << (uint32_t)((MAX_WINDOW_SIZE - sIntermidateVal.dwLowerByte) - 1)) & MOST_SIGNIFICANT_BIT_HIGH)))
                {
                    break;
                }
            }
        }
        //Record validation
        i4Retval = PpsWindow->fValidateRecord(PpsWindow->pValidateArgs);
        //If record validation fails
        if(OCP_RL_OK != i4Retval)
        {
            if(((int32_t)CMD_LIB_DECRYPT_FAILURE == i4Retval) || ((int32_t)OCP_RL_MALLOC_FAILURE == i4Retval))
            {
                i4Status = i4Retval;
            }
            break;
        }
        else
        {
            i4Retval = SubtractUint64(&PpsWindow->sRecvSeqNumber, &PpsWindow->sLowerBound,&sIntermidateVal);
            if(UTIL_SUCCESS != i4Retval)
            {
                break;
            }

            //Set the bit position of sequence number to 1
            i4Retval = Utility_SetBitUint64(&PpsWindow->sWindowFrame, PpsWindow->bWindowSize, (uint8_t)sIntermidateVal.dwLowerByte);
            if(UTIL_SUCCESS != i4Retval)
            {
                break;
            }

            if(PpsWindow->bWindowSize > WORD_SIZE)
            {
                PpsWindow->sWindowFrame.dwHigherByte &= MASK_DOUBLE_WORD >> (MAX_WINDOW_SIZE - PpsWindow->bWindowSize);
            }
            i4Status = (int32_t)OCP_RL_WINDOW_UPDATED;
        }
    }while(0);

    return i4Status;
}

#endif /*MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
