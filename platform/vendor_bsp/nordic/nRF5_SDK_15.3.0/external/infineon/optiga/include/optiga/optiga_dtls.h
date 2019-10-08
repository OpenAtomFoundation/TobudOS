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
*          OCP implementation.
*
* \addtogroup  grOCP
* @{
*/

#ifndef __OCP_H__
#define __OCP_H__

/// @cond hidden
#ifndef _OPTIGA_EXPORTS_DLLEXPORT_H_
#define _OPTIGA_EXPORTS_DLLEXPORT_H_

    #if defined(WIN32) || defined(_WIN32)

        #ifdef OPTIGA_LIB_EXPORTS
            #define LIBRARY_EXPORTS __declspec(dllexport)
        #else
            #define LIBRARY_EXPORTS __declspec(dllimport)
        #endif  // OPTIGA_LIB_EXPORTS

    #else
            #define LIBRARY_EXPORTS
    #endif //WIN32

#endif /*_OPTIGA_EXPORTS_DLLEXPORT_H_*/
/// @endcond
#include "optiga/common/Datatypes.h"
#include "optiga/dtls/OcpCommon.h"
#include "optiga/dtls/OcpCommonIncludes.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

/****************************************************************************
 *
 * API Error Codes
 *
 ****************************************************************************/

 /// Successful execution
#define OCP_LIB_OK                          0x75282512

/// Failure in execution
#define OCP_LIB_ERROR                       0xF3053159

/// Null Parameter(s)
#define OCP_LIB_NULL_PARAM                  BASE_ERROR_OCPLAYER

/// Unsupported Configuration
#define OCP_LIB_UNSUPPORTED_CONFIG          (BASE_ERROR_OCPLAYER + 1)

/// Connection already exists
#define OCP_LIB_CONNECTION_ALREADY_EXISTS   (BASE_ERROR_OCPLAYER + 2)

/// Session ID not available
#define OCP_LIB_SESSIONID_UNAVAILABLE       (BASE_ERROR_OCPLAYER + 3)

/// Memory Allocation failure
#define OCP_LIB_MALLOC_FAILURE              (BASE_ERROR_OCPLAYER + 4)

/// Unsupported PMTU Value
#define OCP_LIB_UNSUPPORTED_PMTU            (BASE_ERROR_OCPLAYER + 5)

/// Unsupported Mode
#define OCP_LIB_UNSUPPORTED_MODE            (BASE_ERROR_OCPLAYER + 6)

///Authentication not done
#define OCP_LIB_AUTHENTICATION_NOTDONE      (BASE_ERROR_OCPLAYER + 7)

///Data Length is greater than the maximum length
#define OCP_LIB_INVALID_LEN                 (BASE_ERROR_OCPLAYER + 8)

///Data Length is equal to zero
#define OCP_LIB_LENZERO_ERROR               (BASE_ERROR_OCPLAYER + 9)

///Memory is not sufficient
#define OCP_LIB_INSUFFICIENT_MEMORY         (BASE_ERROR_OCPLAYER + 10)

///Invalid timeout value
#define OCP_LIB_INVALID_TIMEOUT             (BASE_ERROR_OCPLAYER + 11)

///Timeout occured
#define OCP_LIB_TIMEOUT                     (BASE_ERROR_OCPLAYER + 12)

///requested operation not allowed
#define OCP_LIB_OPERATION_NOT_ALLOWED       (BASE_ERROR_OCPLAYER + 13)

///Decryption failure
#define OCP_LIB_DECRYPT_FAILURE             (BASE_ERROR_OCPLAYER + 14)

///No renegotiation supported
#define OCP_LIB_NO_RENEGOTIATE              (BASE_ERROR_OCPLAYER + 15)
/****************************************************************************
 *
 * Common data structure used across all functions.
 *
 ****************************************************************************/

/**
 * \brief Enumeration for supported OCP Configurations
 */
typedef enum eConfiguration_d
{
    ///DTLS 1.2 protocol over UDP using Hardware crypto
    eDTLS_12_UDP_HWCRYPTO =  0x85,

    ///TLS 1.2  protocol over TCP using Hardware crypto
    eTLS_12_TCP_HWCRYPTO =   0x49

}eConfiguration_d;

/**
 * \brief Structure to that hold network related information
 */
typedef struct sNetworkParams_d
{
    ///Port Number
    uint16_t wPort;

    ///IP Address
    char_t* pzIpAddress;

    ///Network Pmtu
    uint16_t wMaxPmtu;
}sNetworkParams_d;

/**
 * \brief Structure to Configure OCP Library
 */
typedef struct sConfigOCP_d
{
    ///Callback function pointer to get Unix time
    fGetUnixTime_d pfGetUnixTIme;

    ///Network connection structure
    sNetworkParams_d sNetworkParams;

    ///Mode of operation
    eMode_d eMode;

    ///Configuration of mode
    eConfiguration_d eConfiguration;

    ///Logger
    sLogger_d sLogger;

    ///Certificate OID
    uint16_t  wOIDDevCertificate;

    ///Private key OID
    uint16_t wOIDDevPrivKey;
}sAppOCPConfig_d;

/**
 * \brief  Initializes the OCP Library.
 */
LIBRARY_EXPORTS int32_t OCP_Init(const sAppOCPConfig_d* PpsAppOCPConfig,hdl_t* PphAppOCPCtx);

/**
 * \brief  Connect to server and performs a Handshake.
 */
LIBRARY_EXPORTS int32_t OCP_Connect(const hdl_t PhAppOCPCtx);

/**
 * \brief  Sends Application data.
 */
LIBRARY_EXPORTS int32_t OCP_Send(const hdl_t PhAppOCPCtx,const uint8_t* PpbData,uint16_t PwLen);

/**
 * \brief  Receives Application data.
 */
LIBRARY_EXPORTS int32_t OCP_Receive(const hdl_t PhAppOCPCtx,uint8_t* PpbData,uint16_t* PpwLen, uint16_t PwTimeout);

/**
 * \brief  Disconnects from server.
 */
LIBRARY_EXPORTS int32_t OCP_Disconnect(hdl_t PhAppOCPCtx);

#endif /* MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
#endif //__OCP_H__
/**
* @}
*/
