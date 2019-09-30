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
* \brief   This file implements the APIs, types and data structures used in the
*          for DTLS Windowing.
*
* \addtogroup  grMutualAuth
* @{
*/
#ifndef _H_DTLS_WINDOWING_H_
#define _H_DTLS_WINDOWING_H_

#include <stdint.h>
#include "optiga/common/Util.h"
#include "optiga/dtls/OcpCommonIncludes.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH
/**
 * \brief  Structure for DTLS Windowing.
 */
typedef struct sWindow_d
{
    ///Sequence number
    sUint64 sRecvSeqNumber;
    ///Higher Bound of window
    sUint64 sHigherBound;
    ///Lower bound of window
    sUint64 sLowerBound;
    ///Size of window, value valid through 32 to 64
    uint8_t bWindowSize;
    ///Window Frame
    sUint64 sWindowFrame;
    ///Pointer to callback to validate record
    int32_t (*fValidateRecord)(const void*);
    ///Argument to be passed to callback, if any
    void* pValidateArgs;

}sWindow_d;

/**
 * \brief  Structure for Sliding Windowing.
 */
typedef struct sSlideWindow_d
{
    ///Sequence number
    sUint64 sClientSeqNumber;
    ///Pointer to DTLS windowing structure
    sWindow_d* psWindow;

}sSlideWindow_d;

/**
 * \brief Performs record replay detection and rejects the duplicated records.
 */
int32_t DtlsCheckReplay(sWindow_d *PpsWindow);

#endif /*  MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
#endif //_H_DTLS_WINDOWING_H_

/**
* @}
*/
