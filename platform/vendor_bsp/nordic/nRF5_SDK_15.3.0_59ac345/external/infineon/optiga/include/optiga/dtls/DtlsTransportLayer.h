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
* \brief   This file defines APIs, types and data structures used in the
*          DTLS Transport Layer.
*
* \addtogroup  grOCP
* @{
*/
#ifndef __DTLSTL_H__
#define __DTLSTL_H__

#include "optiga/pal/pal_socket.h"
#include "optiga/dtls/OcpTransportLayer.h"
#include "optiga/dtls/OcpCommonIncludes.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

/**
 * \brief This function initializes transport layer communication structure.
 */
int32_t DtlsTL_Init(sTL_d* PpsTL);

/**
 * \brief This function creates client port.
 */
int32_t DtlsTL_Connect(sTL_d* PpsTL);

/**
 * \brief This function transmits the data to the server from which the data
 *          was received.
 */
int32_t DtlsTL_Send(const sTL_d* PpsTL,uint8_t* PpbBuffer,uint16_t PwLen);

/**
 * \brief This function receives the data from the server.
 */
int32_t DtlsTL_Recv(const sTL_d* PpsTL,uint8_t* PpbBuffer,uint16_t* PpwLen);

/**
 * \brief This function closes the UDP communication and releases all the resources.
 */
void DtlsTL_Disconnect(sTL_d* PpsTL);

#endif /* MODULE_ENABLE_DTLS_MUTUAL_AUTH */
#endif //__DTLSTL_H__

/**
* @}
*/
