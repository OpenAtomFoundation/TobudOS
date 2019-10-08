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
*          OCP configurations.
*
* \addtogroup  grMutualAuth
* @{
*/

#ifndef __OCPCOMMON_H__
#define __OCPCOMMON_H__

#include "optiga/common/Datatypes.h"
#include "optiga/dtls/OcpRecordLayer.h"
#include "optiga/common/MemoryMgmt.h"
#include "optiga/dtls/OcpCommonIncludes.h"
#include "optiga/pal/pal_os_timer.h"

 /// Successful execution
#define OCP_HL_OK                       0x75236512

/// Failure in execution
#define OCP_HL_ERROR                    0XF3CA3154

///Null Parameter(s)
#define OCP_HL_NULL_PARAM               BASE_ERROR_HANDSHAKELAYER

///Length of input is zero
#define OCP_HL_LENZERO_ERROR            (BASE_ERROR_HANDSHAKELAYER + 1)

///Maximum PMTU value
#define MAX_PMTU                    1500

///Minimum PMTU Value
#define MIN_PMTU                    296

///Buffer size to hold the received data
#define TLBUFFER_SIZE               1500

///Overhead length for encrypted message
#define ENCRYPTED_APP_OVERHEAD      (UDP_RECORD_OVERHEAD + EXPLICIT_NOUNCE_LENGTH + MAC_LENGTH )

///Macro to get the Maximum length of the Application data which can be sent
#define MAX_APP_DATALEN(PhAppOCPCtx)        ((((sAppOCPCtx_d*)PhAppOCPCtx)->sHandshake.wMaxPmtu) - ENCRYPTED_APP_OVERHEAD)

/****************************************************************************
 *
 * Common data structure used across all functions.
 *
 ****************************************************************************/


///Function pointer to get the unix time
typedef int32_t (*fGetUnixTime_d)(uint32_t*);

/**
 * \brief Enumeration to specify the mode of operation of OCP
 */
typedef enum eMode_d
{
    ///Client
    eClient = 0x24,

    ///Server
    eServer = 0x81
}eMode_d;


/**
 * \brief Enumeration to specify the Protocol version
 */
typedef enum eProtVersion_d
{
    ///DTLS 1.2
    eDTLS_1_2,
    ///TLS 1.2
    eTLS_1_2
}eProtVersion_d;

/**
* \brief Enum for Mutual Authentication states
*/
typedef enum eAuthState_d
{
    //Authentication parameters initialized
    eAuthInitialised,
    //Authentication started
    eAuthStarted,
    //Authentication is completed
    eAuthCompleted,
    //Authentication session is closed
    eAuthSessionClosed
}eAuthState_d;

/**
 * \brief Structure containing Handshake related data.
 */
typedef struct sHandshake_d
{
    ///Mode of operation,Client/Server
    eMode_d eMode;
    ///Maximum PMTU
    uint16_t wMaxPmtu;
    ///Pointer to Record Layer
    sConfigRL_d* psConfigRL;
    ///Pointer to Logger
    sLogger_d phLogger;
    ///To indicate fatal alert
    bool_t fFatalError;
    //Indicates status of Mutual Authentication
    eAuthState_d eAuthState;
    ///Handle to OCP
    hdl_t PhAppOCPCtx;
    ///Session OID
    uint16_t wSessionOID;
    ///Certificate Type used for Authentication
    uint16_t  wOIDDevCertificate;
    ///Private key used for authentication
    uint16_t wOIDDevPrivKey;
    ///Callback function pointer to get unixtime
    fGetUnixTime_d pfGetUnixTIme;
}sHandshake_d;


/**
* \brief Enum for Flights
*/
typedef enum eFlight_d
{
    ///Flight 0
    eFlight0 = 0,
    ///Flight 1
    eFlight1 = 1,
    ///Flight 2
    eFlight2 = 2,
    ///Flight 3
    eFlight3 = 3,
    ///Flight 4
    eFlight4 = 4,
    ///Flight 5
    eFlight5 = 5,
    ///Flight 6
    eFlight6 = 6,
    ///Default
    eDefault
}eFlight_d;

///Function pointer to perform Handshake
typedef int32_t (*fPerformHandshake_d)(sHandshake_d*);

#endif //__OCPCOMMON_H__
/**
* @}
*/
