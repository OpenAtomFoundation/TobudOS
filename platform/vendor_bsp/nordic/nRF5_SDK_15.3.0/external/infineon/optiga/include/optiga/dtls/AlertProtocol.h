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
* \file AlertProtocol.h
*
* \brief   This file defines APIs, types and data structures used in the
*          (D)TLS Alert implementation.
*
* \addtogroup  grMutualAuth
* @{
*/

#ifndef __ALERT_H__
#define __ALERT_H__


#include "optiga/dtls/OcpCommon.h"
#include "optiga/dtls/OcpCommonIncludes.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH
/****************************************************************************
 *
 * API Error Codes
 *
 ****************************************************************************/

/// Successful execution
#define OCP_AL_OK                   0x74281527

/// Failure in execution
#define OCP_AL_ERROR                0xFC468021

///Null Parameter(s)
#define OCP_AL_NULL_PARAM           BASE_ERROR_ALERT

///Length of input is zero
#define OCP_AL_LENZERO_ERROR        (BASE_ERROR_ALERT + 1)

///Fatal error
#define OCP_AL_FATAL_ERROR          (BASE_ERROR_ALERT + 2)

///Warning error
#define OCP_AL_WARNING_ERROR        (BASE_ERROR_ALERT + 3)

/**
 * \brief Process the Alert Messages.
 */
int32_t Alert_ProcessMsg(const sbBlob_d* PpsAlertMsg,int32_t* Ppi4ErrorCode);

/**
 * \brief Sends Alert based on the internal error code via the Record Layer.
 */
void Alert_Send(sConfigRL_d *PpsConfigRL,int32_t Pi4ErrorCode);

///Macro to Send Alert
#ifndef DISABLE_ALERT
#define SEND_ALERT(RLHdl,error) Alert_Send(RLHdl,error);
#else
#define SEND_ALERT(RLHdl,error)
#endif

#endif  /* MODULE_ENABLE_DTLS_MUTUAL_AUTH */

#endif //__ALERT_H__
/**
* @}
*/
