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
*          OCP Transport Layer.
*
* \addtogroup  grMutualAuth
* @{
*/

#ifndef __OCPTRANSPORTLAYER_H__
#define __OCPTRANSPORTLAYER_H__

#include "optiga/common/Datatypes.h"
#include "optiga/common/Logger.h"
#include "optiga/dtls/OcpCommonIncludes.h"

///General error
#define OCP_TL_ERROR                0xF1267AB3

///Successful execution
#define OCP_TL_OK                   0x72309781

///Null Parameter(S)
#define OCP_TL_NULL_PARAM           BASE_ERROR_TRANSPORTLAYER

///Null Parameter(S)
#define OCP_TL_BINDING_FAILURE      (BASE_ERROR_TRANSPORTLAYER + 1)

///NO Data received
#define OCP_TL_NO_DATA              (BASE_ERROR_TRANSPORTLAYER + 2)

///Malloc failure
#define OCP_TL_MALLOC_FAILURE       (BASE_ERROR_TRANSPORTLAYER + 3)

/****************************************************************************
 *
 * Common data structure used across all functions.
 *
 ****************************************************************************/

/**
* \brief Enumeration to represent transport layer communication state
*/
typedef enum eConnectionState_d
{
    ///Connected to network
    eConnected = 0x01,

    ///Disconnected from network
    eDisconnected = 0x02
}eConnectionState_d;


/**
 * \brief This structure contains Ethernet communication data
 */
typedef enum eReceiveCall_d
{
    ///Blocking Ethernet call
    eBlocking = 0x10,

    ///Non Blocking Ethernet call
    eNonBlocking = 0x20
}eReceiveCall_d;

/**
 * \brief Structure holding Transport Layer Information.
 */
typedef struct sTL_d
{
    ///Handle to  transport layer
    hdl_t phTLHdl;

    ///Port Number
    uint16_t wPort;

    ///IP Address
    char_t* pzIpAddress;

    ///Transport Layer Timeout
    uint16_t wTimeout;

    ///Connection state
    eConnectionState_d eIsConnected;

    ///Call type Blocking or NonBlocking
    eReceiveCall_d eCallType;

    //Structure that holds logger parameters
    sLogger_d sLogger;
}sTL_d;

///Function pointer for Transport Layer Init.
typedef int32_t (*fTLInit)(sTL_d* psTL);

///Function pointer for Transport Layer Connect.
typedef int32_t (*fTLConnect)(sTL_d* psTL);

///Function pointer for Transport Layer Disconnect
typedef void (*fTLDisconnect)(sTL_d* psTL);

///Function pointer for Transport Layer Send
typedef int32_t (*fTLSend)(const sTL_d* psTL,uint8_t* pbBuffer,uint16_t wLen);

///Function pointer for Transport Layer Receive
typedef int32_t (*fTLRecv)(const sTL_d* psTL,uint8_t* pbBuffer,uint16_t* pwLen);

/**
 * \brief Structure to configure Transport Layer.
 */
typedef struct sConfigTL_d
{
    ///Function pointer to Init via TL
    fTLInit pfInit;

    ///Function pointer to Send via TL
    fTLSend pfSend;

    ///Function pointer to Receive via TL
    fTLRecv pfRecv;

    ///Function pointer to Connect to TL
    fTLConnect pfConnect;

    ///Function pointer to Disconnect from TL
    fTLDisconnect pfDisconnect;

    ///Transport Layer
    sTL_d sTL;
}sConfigTL_d;

#endif //__OCPTRANSPORTLAYER_H__
/**
* @}
*/
