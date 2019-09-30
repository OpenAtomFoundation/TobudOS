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
* \file OCP.c
*
* \brief   This file implements the API Layer of OCP Library.
*
* \addtogroup  grOCP
* @{
*/

#include "optiga/optiga_dtls.h"
#include "optiga/cmd/CommandLib.h"
#include "optiga/dtls/AlertProtocol.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

/// @cond hidden

extern Void ConfigHL(fPerformHandshake_d* PfPerformHandshake_d,eConfiguration_d PeConfiguration);
extern Void ConfigRL(sConfigRL_d* PpsConfigRL,eConfiguration_d PeConfiguration);
extern Void ConfigTL(sConfigTL_d* PpsConfigTL,eConfiguration_d PeConfiguration);
extern Void ConfigCL(sConfigCL_d* PpsConfigCL,eConfiguration_d PeConfiguration);
extern int32_t DtlsHS_VerifyHR(uint8_t* PprgbData, uint16_t PwLen);

///Identifier for Session ID 1
#define SESSIONID_1                 0xE100

///Session key is in used
#define INUSE                       0x4A

///Session key is not in use
#define NOTUSED                     0xA4

/**
 * \brief Structure that defines OCP Application context data
 */
typedef struct sAppOCPCtx_d
{
    ///Pointer to function that performs handshake
    fPerformHandshake_d pfPerformHandshake;

    ///Structure that contains Handshake data
    sHandshake_d sHandshake;

    ///Structure that holds Record Layer Configuration
    sConfigRL_d sConfigRL;

    ///Structure that holds logger parameters
    sLogger_d sLogger;

    ///Authentication scheme
    eAuthScheme_d eAuthScheme;

    ///Buffer to store the received application data
    uint8_t* pAppDataBuf;
}sAppOCPCtx_d;

/**
 * \brief Configures the Handshake, Record, Transport and Crypto Layers based on input parameters
 */
_STATIC_H Void OCP_Config(sAppOCPCtx_d* PpsAppOCPCntx,eConfiguration_d PeConfiguration)
{
    ConfigHL(&(PpsAppOCPCntx->pfPerformHandshake),PeConfiguration);
    ConfigRL(&(PpsAppOCPCntx->sConfigRL),PeConfiguration);
    ConfigTL(PpsAppOCPCntx->sConfigRL.sRL.psConfigTL,PeConfiguration);
    ConfigCL(PpsAppOCPCntx->sConfigRL.sRL.psConfigCL,PeConfiguration);
}

/**
 * Allocates the memory for sAppOCPCtx_d<br>
 */
_STATIC_H int32_t OCPAllocateMemory(sAppOCPCtx_d ** PppsAppOCPCntx)
{
    int32_t i4Status = (int32_t)OCP_LIB_MALLOC_FAILURE;

#define PS_APPOCPCNTX    (* PppsAppOCPCntx)

    do
    {
        //Allocate the memory for OCP context data structure
        PS_APPOCPCNTX = (sAppOCPCtx_d *)OCP_MALLOC(sizeof(sAppOCPCtx_d));
        if(NULL == PS_APPOCPCNTX)
        {
            break;
        }

        (*PS_APPOCPCNTX).pAppDataBuf = NULL;
        (*PS_APPOCPCNTX).sConfigRL.sRL.psConfigTL = NULL;
        (*PS_APPOCPCNTX).sConfigRL.sRL.psConfigCL = NULL;

        //Allocate the memory for the transport layer and crypto layer structure
        PS_APPOCPCNTX->sConfigRL.sRL.psConfigTL = (sConfigTL_d*)OCP_MALLOC(sizeof(sConfigTL_d));
        if(NULL == PS_APPOCPCNTX->sConfigRL.sRL.psConfigTL)
        {
            break;
        }

        PS_APPOCPCNTX->sConfigRL.sRL.psConfigTL->sTL.phTLHdl = NULL;

        PS_APPOCPCNTX->sConfigRL.sRL.psConfigCL = (sConfigCL_d*)OCP_MALLOC(sizeof(sConfigCL_d));
        if(NULL == PS_APPOCPCNTX->sConfigRL.sRL.psConfigCL)
        {
            break;
        }

        PS_APPOCPCNTX->sConfigRL.sRL.psConfigCL->sCL.phCryptoHdl = NULL;
        PS_APPOCPCNTX->sConfigRL.sRL.phRLHdl = NULL;
        i4Status = (int32_t)OCP_LIB_OK;
    }while(FALSE);
#undef PS_APPOCPCNTX
    return i4Status;
}

/**
* \brief Frees the allocated memory for sAppOCPCtx_d
*/
_STATIC_H Void OCPFreeMemory(sAppOCPCtx_d* PpsAppOCPCntx)
{
    if(NULL != PpsAppOCPCntx)
    {
        if(NULL != (PpsAppOCPCntx)->pAppDataBuf)
        {
            OCP_FREE((PpsAppOCPCntx)->pAppDataBuf);
        }
        if(NULL != (PpsAppOCPCntx)->sConfigRL.sRL.psConfigCL)
        {
#define S_RL (PpsAppOCPCntx)->sConfigRL

            S_RL.sRL.psConfigCL->pfClose(&(PpsAppOCPCntx)->sConfigRL.sRL.psConfigCL->sCL);
            OCP_FREE((PpsAppOCPCntx)->sConfigRL.sRL.psConfigCL);
        }

        if(NULL != (PpsAppOCPCntx)->sConfigRL.sRL.psConfigTL)
        {
#define S_TL (PpsAppOCPCntx)->sConfigRL.sRL.psConfigTL->sTL

            if(NULL != S_TL.phTLHdl)
            {
                //Free the allocated memory for Transport layer
                OCP_FREE(S_TL.phTLHdl);

                S_TL.phTLHdl = NULL;
            }

            OCP_FREE((PpsAppOCPCntx)->sConfigRL.sRL.psConfigTL);
        }

        //Free the memory held by record layer
        S_RL.pfClose(&S_RL.sRL);

        OCP_FREE(PpsAppOCPCntx);
    }
#undef S_TL
#undef S_RL
}

/**
 * \brief Structure defining Session registry content
 */
typedef struct sSessionRegistry_d
{
    ///DTLS Session ID
    uint16_t wSessionId;
    ///OCP Handle
    hdl_t hOCPHandle;
    ///Variable to indicate the session ID usage
    uint8_t bInUse;
}sSessionRegistry_d;

///Static registry for holding Session key Id information
sSessionRegistry_d sSessionRegistry[1] ={
                                            {SESSIONID_1, (hdl_t)NULL, NOTUSED}
                                        };

/**
 * This API returns the available Security Chip Session id
 * that can be used by Command Library SetAuthScheme.<br>
 *
 * \param[in,out] PwSessionId    Available session id
 *
 * \retval  #OCP_LIB_OK
 * \retval  #OCP_LIB_SESSIONID_UNAVAILABLE
 */
_STATIC_H int32_t Registry_GetSessionId(uint16_t* PwSessionId)
{
    int32_t i4Status = (int32_t)OCP_LIB_SESSIONID_UNAVAILABLE;
    uint8_t bCount = 0;

    //Search the registry for unused session key id
    for(bCount= 0;bCount<(sizeof(sSessionRegistry)/sizeof(sSessionRegistry_d));bCount++)
    {
        if(NOTUSED == sSessionRegistry[bCount].bInUse)
        {
            //return unused session key id
            *PwSessionId = sSessionRegistry[bCount].wSessionId;
            i4Status = (int32_t)OCP_LIB_OK;
            break;
        }
    }

    return i4Status;
}

/**
 * This function updates OCP handle against the given session key id in the registry if the session key id is not already used.<br>
 * The session key is marked as in-use.
 *
 * \param[in]        PdwSessionId   Session id
 * \param[in,out]    PhOCPHandle    Context to be updated for input session id
 *
 * \retval  #OCP_LIB_OK
 * \retval  #OCP_LIB_ERROR
 */
_STATIC_H int32_t Registry_Update(uint32_t PdwSessionId,const hdl_t PhOCPHandle)
{
    int32_t i4Status = (int32_t)OCP_LIB_ERROR;
    uint8_t bCount;

    //Search the table for the given session key id
    for(bCount= 0;bCount<(sizeof(sSessionRegistry)/sizeof(sSessionRegistry_d));bCount++)
    {
        if((PdwSessionId == sSessionRegistry[bCount].wSessionId) &&
            (NOTUSED == sSessionRegistry[bCount].bInUse))
        {
            //Update the OCP handle
            sSessionRegistry[bCount].hOCPHandle = PhOCPHandle;
            //Update the usage status
            sSessionRegistry[bCount].bInUse = INUSE;
            i4Status = (int32_t) OCP_LIB_OK;
            break;
        }
    }

    return i4Status;
}

/**
 * This function frees the session key id used for the given OCP handle in the registry.<br>
 * The session key is marked as Not used.
 *
 * \param[in,out]    PhOCPHandle    Context for which session id to be freed
 *
 */
//lint --e{818} suppress "PhOCPHandle is declared as const"
_STATIC_H Void Registry_Free(const hdl_t PhOCPHandle)
{
    uint8_t bCount;

    //Search the table for the given session key id
    for(bCount= 0;bCount<(sizeof(sSessionRegistry)/sizeof(sSessionRegistry_d));bCount++)
    {
        if(PhOCPHandle == sSessionRegistry[bCount].hOCPHandle)
        {
            //Free the usage status
            sSessionRegistry[bCount].bInUse = NOTUSED;
            sSessionRegistry[bCount].hOCPHandle = NULL;
            break;
        }
    }
}

/**
* This function Gets the session key id used for the given OCP handle in the registry.<br>
*
* \param[in]    PhOCPHandle    OCP handle
* \param[out]    PpwSessionId    Pointer to the session ID
*
* \retval  #OCP_LIB_OK
* \retval  #OCP_LIB_SESSIONID_UNAVAILABLE
*/
//lint --e{818} suppress "PhOCPHandle is declared as const"
_STATIC_H int32_t Registry_GetHandleSessionID(const hdl_t PhOCPHandle, uint16_t *PpwSessionId)
{
    int32_t i4Status = (int32_t)OCP_LIB_SESSIONID_UNAVAILABLE;
    uint8_t bCount = 0;

    //Search the table for the given OCP handle
    for(bCount= 0;bCount<(sizeof(sSessionRegistry)/sizeof(sSessionRegistry_d));bCount++)
    {
        if(PhOCPHandle == sSessionRegistry[bCount].hOCPHandle)
        {
            *PpwSessionId = sSessionRegistry[bCount].wSessionId;
            i4Status = (int32_t)OCP_LIB_OK;
            break;
        }
    }

    return i4Status;
}

/**
* This function validates whether handle is associated with a the session key id from the registry.<br>
*
* \param[in]    PhOCPHandle    OCP handle
*
* \retval  #OCP_LIB_OK
* \retval  #OCP_LIB_SESSIONID_UNAVAILABLE
*/
//lint --e{818} suppress "PhOCPHandle is declared as const"
_STATIC_H int32_t Registry_ValidateHandleSessionID(const hdl_t PhOCPHandle)
{
    int32_t i4Status = (int32_t)OCP_LIB_SESSIONID_UNAVAILABLE;
    uint8_t bCount;

    //Search the table for the given OCP handle
    for(bCount= 0;bCount<(sizeof(sSessionRegistry)/sizeof(sSessionRegistry_d));bCount++)
    {
        if(PhOCPHandle == sSessionRegistry[bCount].hOCPHandle)
        {
            i4Status = (int32_t)OCP_LIB_OK;
            break;
        }
    }

    return i4Status;
}

/**
 * This Function closes the session ,free the memory allocated and return close notify alert based on input parameter.<br>
 *
 * Notes: <br>
 * - Under some erroneous conditions,error codes from Command Library and transport layer can also be returned. <br>
 *
 * \param[in]    PhAppOCPCtx    OCP handle
 * \param[in]    PfFatalError    Flag indicating whether fatal error as occurred or not
 * \param[in]    PwSessionId    Session ID to be closed
 *
 */
_STATIC_H Void CloseSession(hdl_t PhAppOCPCtx, bool_t PfFatalError, uint16_t PwSessionId)
{
    sAppOCPCtx_d *psCntx = (sAppOCPCtx_d*)PhAppOCPCtx;
    #define S_CONFIGURATION_TL (psCntx->sConfigRL.sRL.psConfigTL)

    //lint --e{534} ,Return value is ignored as irrespective of this session will be closed"
    if(psCntx->sHandshake.eAuthState != eAuthInitialised)
    {
        if(!PfFatalError)
        {
            SEND_ALERT(&psCntx->sConfigRL,(int32_t) OCP_RL_ERROR);
        }
        //Close the DTLS session on Security Chip
        CmdLib_CloseSession(PwSessionId);
    }
    //Disconnect from the server via transport layer
    S_CONFIGURATION_TL->pfDisconnect(&S_CONFIGURATION_TL->sTL);

    //Clear the session reference ID registry for the handle
    Registry_Free(PhAppOCPCtx);

    //Free the memory associated with the handle
    OCPFreeMemory(psCntx);

#undef S_CONFIGURATION_TL
}

/// @endcond

/**
 * This API initialises the OCP context based on the user inputs provided in PpsAppOCPConfig.Once the OCP context
 * is initialised,PphAppOCPCtx is returned as a handle.<br>
 * PphAppOCPCtx handle is used for all interactions with the OCP APIs #OCP_Connect, #OCP_Disconnect, #OCP_Send and #OCP_Receive.
 * <br>
 * <br>
 * \image html OCPInit.png "OCP_Init()" width=20cm
 *
 *<b>Pre Conditions:</b>
 * - Communication with the security chip is up and running. <br>
 *   #optiga_comms_open() must be successfully executed.<br>
 * - The optiga comms context for command library is registered using CmdLib_SetOptigaCommsContext().
 *
 *<b>API Details:</b>
 * - Checks for an available session OID.
 * - Opens application on the security chip using CmdLib_OpenApplication().<br>
 * - Allocates the memory for internal structures, initialises it and returns as a #hdl_t*
 *
 *<b>User Input:</b><br>
 *The user must provide configuration information in #sAppOCPConfig_d
 * - eMode allows the user to configure the OCP context as a client/server as per #eMode_d. Currently server mode is not supported.<br>
 * - eConfiguration allows the user to choose supported OCP context configuration as per #eConfiguration_d. Currently eTLS_12_TCP_HWCRYPTO is not supported.<br>
 * - wOIDDevCertificate allows the user to choose the supported certificate for client authentication.
 *   - If their is no client certificate then set it to 0x0000.<br>
 * - wOIDDevPrivKey allows the user to choose the private key used for client authentication.<br>
 * - psNetworkParams allows the user to configure the port, IP Address and maximum PMTU required for transport layer connection.<br>
 * - Valid IP address  and port number must be provided. The correctness of the IP address and port number will not be verified.<br>
 * - PMTU value should range between 296 to 1500,else  #OCP_LIB_UNSUPPORTED_PMTU error is returned.<br>
 * - Logger allows user to log data. User must provide the low level log writer through #sLogger_d.<br>
 * - pfGetUnixTIme(#fGetUnixTime_d) is a call-back function pointer that allows user to provide 32-bit Unix time format.<br>
 * - If pfGetUnixTIme is set to NULL, the unix time will not be sent to security chip.<br>
 * - If pfGetUnixTIme is not set to NULL or valid function pointer, the behavior would be unexpected.<br>
 * - The call back function pfGetUnixTIme is expected to return status s as #CALL_BACK_OK for success.
 *
 *<b>Notes:</b>
 * - Currently, only 1 DTLS session is supported by security chip.<br>
 * - If user invokes OCP_Init, with out disconnecting/closing the previous session/context(if available), will lead to error #OCP_LIB_SESSIONID_UNAVAILABLE.<br>
 * - Under some failure conditions, error codes from lower layers could also be returned. <br>
 *
 * \param[in] PpsAppOCPConfig    Pointer to structure that contains the configuration from user
 * \param[in,out] PphAppOCPCtx   Pointer to the handle of OCP context
 *
 * \retval  #OCP_LIB_OK
 * \retval  #OCP_LIB_ERROR
 * \retval  #OCP_LIB_NULL_PARAM
 * \retval  #OCP_LIB_UNSUPPORTED_CONFIG
 * \retval  #OCP_LIB_SESSIONID_UNAVAILABLE
 * \retval  #OCP_LIB_UNSUPPORTED_PMTU
 */

int32_t OCP_Init(const sAppOCPConfig_d* PpsAppOCPConfig,hdl_t* PphAppOCPCtx)
{
    int32_t i4Status = (int32_t)OCP_LIB_ERROR;
    sAppOCPCtx_d *psAppOCPCntx = NULL;
    eAuthScheme_d eAuthScheme;
    sOpenApp_d sOpenApp;
    uint16_t wSessionKeyId;

    do
    {
        //NULL check for input parameters
        if((NULL == PpsAppOCPConfig) || (NULL == PphAppOCPCtx))
        {
            i4Status =  (int32_t)OCP_LIB_NULL_PARAM;
            break;
        }

        //Initialize handle to NULL
        * PphAppOCPCtx = NULL;

        //Check the registry whether a free session Id is available
        i4Status = Registry_GetSessionId(&wSessionKeyId);
        if(OCP_LIB_OK != i4Status)
        {
            break;
        }

        //Check for valid configuration
        if(eDTLS_12_UDP_HWCRYPTO != PpsAppOCPConfig->eConfiguration)
        {
            i4Status = (int32_t)OCP_LIB_UNSUPPORTED_CONFIG;
            break;
        }

        //Initialize the Auth Scheme type
        if((eClient == PpsAppOCPConfig->eMode) && (eDTLS_12_UDP_HWCRYPTO == PpsAppOCPConfig->eConfiguration))
        {
            eAuthScheme = eDTLSClient;
        }
        else
        {
            //If other mode is selected
            i4Status = (int32_t)OCP_LIB_UNSUPPORTED_MODE;
            break;
        }

        //Check if the PMTU provided is within limit or not
        if((PpsAppOCPConfig->sNetworkParams.wMaxPmtu > MAX_PMTU) || (PpsAppOCPConfig->sNetworkParams.wMaxPmtu < MIN_PMTU))
        {
            i4Status = (int32_t)OCP_LIB_UNSUPPORTED_PMTU;
            break;
        }

        //Allocate the memory for the Context data structure.It will be used for all interaction with OCP APIs
        i4Status = OCPAllocateMemory(&psAppOCPCntx);
        if(OCP_LIB_OK != i4Status)
        {
            break;
        }

        OCP_Config(psAppOCPCntx,PpsAppOCPConfig->eConfiguration);

        sOpenApp.eOpenType = eInit;

        //Open Application
        i4Status = CmdLib_OpenApplication(&sOpenApp);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        //Assign the Auth Scheme
        psAppOCPCntx->eAuthScheme = eAuthScheme;

        //Assign the maximum path transfer unit
        psAppOCPCntx->sHandshake.wMaxPmtu = PpsAppOCPConfig->sNetworkParams.wMaxPmtu;

        //Assign the Certificate type to be used for Authentication
        psAppOCPCntx->sHandshake.wOIDDevCertificate = PpsAppOCPConfig->wOIDDevCertificate;

        //Assign the private key to be used for Authentication
        psAppOCPCntx->sHandshake.wOIDDevPrivKey = PpsAppOCPConfig->wOIDDevPrivKey;

        //Assign the callback function to get unix time
        psAppOCPCntx->sHandshake.pfGetUnixTIme = PpsAppOCPConfig->pfGetUnixTIme;

        //Assign the record layer configuration pointer to the handshake layer
        psAppOCPCntx->sHandshake.psConfigRL = &psAppOCPCntx->sConfigRL;
        psAppOCPCntx->sHandshake.eMode = PpsAppOCPConfig->eMode;

        //Set the fatal error occur type to false;
        psAppOCPCntx->sHandshake.fFatalError = FALSE;

        //Assign the logger pointer for psAppOCPCntx layer
        psAppOCPCntx->sLogger = PpsAppOCPConfig->sLogger;

        //Assign the logger pointer for handshake layer
        psAppOCPCntx->sHandshake.phLogger = PpsAppOCPConfig->sLogger;

        //Assign the logger pointer for Record layer
        psAppOCPCntx->sConfigRL.sRL.sLogger.pHdl = PpsAppOCPConfig->sLogger.pHdl;
        psAppOCPCntx->sConfigRL.sRL.sLogger.phfWriter = PpsAppOCPConfig->sLogger.phfWriter;

/// @cond hidden
#define S_TL psAppOCPCntx->sConfigRL.sRL.psConfigTL->sTL
/// @endcond
        //Assign the IP address to the transport layer parameter
        S_TL.pzIpAddress = PpsAppOCPConfig->sNetworkParams.pzIpAddress;

        //Assign the port number to the transport layer parameter
        S_TL.wPort = PpsAppOCPConfig->sNetworkParams.wPort;

        //Assign the UDP Timeout to the transport layer parameter
        S_TL.wTimeout = 200;

        //Assign the logger pointer for Transport layer
        S_TL.sLogger.pHdl = PpsAppOCPConfig->sLogger.pHdl;
        S_TL.sLogger.phfWriter = PpsAppOCPConfig->sLogger.phfWriter;

        //Assign the initial connection state of transport layer
        S_TL.eIsConnected = eDisconnected;
        //Assign receive call function type
        S_TL.eCallType = eNonBlocking;

        //Assign the logger pointer for Transport layer
        psAppOCPCntx->sConfigRL.sRL.psConfigCL->sCL.sLogger.pHdl = PpsAppOCPConfig->sLogger.pHdl;
        psAppOCPCntx->sConfigRL.sRL.psConfigCL->sCL.sLogger.phfWriter = PpsAppOCPConfig->sLogger.phfWriter;

        //Initialize all the modules
        //Init for logger
        //lint --e{611} suppress "This is ignored as it is known to the user for data type conversion"
        LOG_SETWRITER((pFWriteData2)PpsAppOCPConfig->sLogger.phfWriter,(Void*)PpsAppOCPConfig->sLogger.pHdl);
        //Init Record Layer
        i4Status = psAppOCPCntx->sConfigRL.pfInit(&psAppOCPCntx->sConfigRL.sRL);
        if(OCP_RL_OK != i4Status)
        {
            break;
        }
        //Init Transport Layer
        i4Status = psAppOCPCntx->sConfigRL.sRL.psConfigTL->pfInit(&S_TL);
        if(OCP_TL_OK != i4Status)
        {
            break;
        }
        //Init Crypto Layer
        i4Status = psAppOCPCntx->sConfigRL.sRL.psConfigCL->pfInit(&psAppOCPCntx->sConfigRL.sRL.psConfigCL->sCL, (Void*)&wSessionKeyId);
        if(OCP_CL_OK != i4Status)
        {
            break;
        }

        //Update the registry with the session ID being used
        i4Status = Registry_Update(wSessionKeyId,(hdl_t) psAppOCPCntx);
        if(OCP_LIB_OK != i4Status)
        {
            break;
        }

        //Set the Authentication state to initialised
        psAppOCPCntx->sHandshake.eAuthState = eAuthInitialised;

        *PphAppOCPCtx = (hdl_t) psAppOCPCntx;

        psAppOCPCntx->sHandshake.PhAppOCPCtx = *PphAppOCPCtx;
        i4Status = (int32_t)OCP_LIB_OK;
    }while(FALSE);

    if((OCP_LIB_OK != i4Status) && ((int32_t)OCP_LIB_NULL_PARAM != i4Status) && ((int32_t)OCP_LIB_SESSIONID_UNAVAILABLE != i4Status))
    {
        OCPFreeMemory(psAppOCPCntx);
    }
/// @cond hidden
#undef S_TL
/// @endcond
    return i4Status;
}


/**
 * This API connects to the server and performs a DTLS handshake protocol as per DTLS v1.2
 * <br>
 * <br>
 * \image html OCPConnect.png "OCP_Connect()" width=20cm
 *
 *<b>Pre Conditions:</b>
 * - #OCP_Init() is successful and application context is available.<br>
 * - Server trust anchor must be available in the security chip.<br>
 *
 *<b>API Details:</b>
 * - Connects to the server via the transport layer.<br>
 * - Invokes CmdLib_SetAuthScheme() based on configuration.<br>
 * - Performs a DTLS Handshake.<br>
 *
 *<b>User Input:</b><br>
 * - User must provide a valid PhAppOCPCtx handle otherwise #OCP_LIB_SESSIONID_UNAVAILABLE is returned.<br>
 *
 *<b>Notes:</b>
 * - The default value of timeout for retransmission must be 2 seconds on the server side.<br>
 * - If a connection already exists on the given port and IP address, #OCP_LIB_CONNECTION_ALREADY_EXISTS is returned.<br>
 * - Under some failure conditions, error codes from lower layers could also be returned. <br>
 * - In case of a Failure other than #OCP_LIB_CONNECTION_ALREADY_EXISTS and #OCP_LIB_SESSIONID_UNAVAILABLE<br>
 *   - The Session gets closed automatically.<br>
 *   - The memory allocated in #OCP_Init() are freed.<br>
 *   - OCP handle will not be set to NULL.It is upto the user to check return code and take appropriate action.<br>
 * - If the return value is #CMD_DEV_EXEC_ERROR, it might indicate that the application on the security chip is either
 *   closed or a reset has occurred.<br>
 *
 *
 * \param[in] PhAppOCPCtx    Handle to OCP Context
 *
 * \retval  #OCP_LIB_OK
 * \retval  #OCP_LIB_ERROR
 * \retval  #OCP_LIB_NULL_PARAM
 * \retval  #OCP_LIB_CONNECTION_ALREADY_EXISTS
 * \retval  #OCP_LIB_SESSIONID_UNAVAILABLE
 */
int32_t OCP_Connect(const hdl_t PhAppOCPCtx)
{
    int32_t i4Status = (int32_t)OCP_LIB_ERROR;
    sAuthScheme_d sAuthScheme;
/// @cond hidden
#define PS_CNTX ((sAppOCPCtx_d*)PhAppOCPCtx)
#define S_CONFIGURATION_TL (PS_CNTX->sConfigRL.sRL.psConfigTL)
#define S_CONFIGURATION_CL (PS_CNTX->sConfigRL.sRL.psConfigCL)
#define S_CONFIGURATION_RL (PS_CNTX->sConfigRL)
/// @endcond
    do
    {
        //NULL check for handle
        if(NULL == PS_CNTX)
        {
            i4Status = (int32_t)OCP_LIB_NULL_PARAM;
            break;
        }

        i4Status = Registry_ValidateHandleSessionID(PhAppOCPCtx);
        if(OCP_LIB_OK != i4Status)
        {
            break;
        }

        //Null checks for other pointers
        if((NULL == S_CONFIGURATION_TL) || (NULL== S_CONFIGURATION_TL->pfConnect)|| (NULL == PS_CNTX->pfPerformHandshake)||
        (NULL == S_CONFIGURATION_RL.pfSend)|| (NULL == S_CONFIGURATION_RL.pfRecv) || (NULL == S_CONFIGURATION_RL.pfClose) ||
        (NULL == S_CONFIGURATION_TL->pfSend) || (NULL == S_CONFIGURATION_TL->pfRecv) || (NULL == S_CONFIGURATION_TL->pfDisconnect) ||
        (NULL == S_CONFIGURATION_CL) || (NULL == S_CONFIGURATION_CL->pfEncrypt) || (NULL == S_CONFIGURATION_CL->pfDecrypt) ||
        (NULL == S_CONFIGURATION_CL->pfClose))
        {
            i4Status = (int32_t)OCP_LIB_NULL_PARAM;
            break;
        }

        //Check whether connection is already connected
        if(eConnected == S_CONFIGURATION_TL->sTL.eIsConnected)
        {
            i4Status = (int32_t)OCP_LIB_CONNECTION_ALREADY_EXISTS;
            break;
        }
        //Connect to server
        i4Status = S_CONFIGURATION_TL->pfConnect(&S_CONFIGURATION_TL->sTL);
        if(OCP_TL_OK != i4Status)
        {
            break;
        }

        //Get the Session OID from registry
        i4Status = Registry_GetHandleSessionID(PhAppOCPCtx,&(PS_CNTX->sHandshake.wSessionOID));
        if(OCP_LIB_OK != i4Status)
        {
            break;
        }

        //Initialize Auth scheme structure
        sAuthScheme.eAuthScheme = PS_CNTX->eAuthScheme;
        sAuthScheme.wDevicePrivKey = PS_CNTX->sHandshake.wOIDDevPrivKey;
        sAuthScheme.wSessionKeyId = PS_CNTX->sHandshake.wSessionOID;

        //Set the AuthScheme
        i4Status = CmdLib_SetAuthScheme(&sAuthScheme);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        //Perform Handshake
        i4Status = PS_CNTX->pfPerformHandshake((hdl_t)&PS_CNTX->sHandshake);
        if(OCP_HL_OK != i4Status)
        {
            break;
        }
        i4Status = (int32_t) OCP_LIB_OK;

    }while(FALSE);

    do
    {
        if((OCP_LIB_OK != i4Status) &&
        ((int32_t)OCP_LIB_CONNECTION_ALREADY_EXISTS != i4Status) &&
        ((int32_t)OCP_LIB_NULL_PARAM != i4Status) &&
        ((int32_t)OCP_LIB_SESSIONID_UNAVAILABLE != i4Status))
        {
            //lint --e{794} suppress "OCP_LIB_NULL_PARAM check address this lint issue which doesn't allow null pointer in this context,"
            CloseSession(PhAppOCPCtx,PS_CNTX->sHandshake.fFatalError, PS_CNTX->sHandshake.wSessionOID);

            LOG_TRANSPORTDBVAL(i4Status,eInfo);
        }
    }while(FALSE);

/// @cond hidden
#undef PS_CNTX
#undef S_CONFIGURATION_CL
#undef S_CONFIGURATION_TL
#undef S_CONFIGURATION_RL
/// @endcond
    return i4Status;
}

/**
 * This API sends application data to the DTLS server
 * <br>
 * <br>
 * \image html OCPSend.png "OCP_Send()" width=20cm
 *
 *<b>Pre Conditions:</b>
 * - #OCP_Connect() is successful and application context is available.<br>
 *
 *<b>API Details:</b>
 * - Sends application data to DTLS server.<br>
 * - Application data is sent only if Mutual Authentication Public Key Scheme (DTLS) was successfully performed.<br>
 * - Encryption of the application data is done at the record layer.<br>
 *<br>
 *
 *<b>User Input:</b><br>
 * - User must provide a valid PhAppOCPCtx handle.<br>
 * - User must provide the data to be sent and its length
 *   - If the length of the data to be sent is greater then #MAX_APP_DATALEN(PhAppOCPCtx), then #OCP_LIB_INVALID_LEN is returned.
 *   - If the length of the data to be sent is equal to zero, then #OCP_LIB_LENZERO_ERROR is returned.<br>
 *
 *<b>Notes:</b>
 * - The maximum length of data that can be sent by the API depends upon the PMTU value set during #OCP_Init().This length can be obtained by #MAX_APP_DATALEN(PhAppOCPCtx).<br>
 * - Fragmentation of data to be sent should be done by the application. This API does not perform data fragmentation.<br>
 * - If the record sequence number has reached maximum value for epoch 1, then #OCP_RL_SEQUENCE_OVERFLOW error is returned.
 *   User must call #OCP_Disconnect() in this condition.No Alert will be sent due to the unavailability of record sequence number.<br>
 * - Under some failure conditions, error codes from lower layers could also be returned. <br>
 * - In case of a Failure,<br>
 *   - Existing session remains open and memory allocated during OCP_Init() is not freed.<br>
 *   - PhAppOCPCtx handle is not set to NULL.<br>
 *   - The API does not send any alert to the server.<br>
 * - If the return value is #CMD_DEV_EXEC_ERROR, it might indicate that the application on the security chip is either
 *   closed or a reset has occurred. In such a case, close the existing DTLS session using #OCP_Disconnect.<br>
 *
 *
 * \param[in] PhAppOCPCtx   Handle to OCP Context
 * \param[in] PprgbData       Pointer to data to be sent
 * \param[in] PwLen         Length of the data to be sent
 *
 * \retval  #OCP_LIB_OK
 * \retval  #OCP_LIB_ERROR
 * \retval  #OCP_LIB_NULL_PARAM
 * \retval  #OCP_LIB_SESSIONID_UNAVAILABLE
 * \retval  #OCP_LIB_AUTHENTICATION_NOTDONE
 * \retval  #OCP_LIB_MALLOC_FAILURE
 * \retval  #OCP_LIB_LENZERO_ERROR
 * \retval  #OCP_LIB_INVALID_LEN
 * \retval  #OCP_RL_SEQUENCE_OVERFLOW
 */
int32_t OCP_Send(const hdl_t PhAppOCPCtx,const uint8_t* PprgbData,uint16_t PwLen)
{
    int32_t i4Status = (int32_t)OCP_LIB_ERROR;
/// @cond hidden
#define PS_CNTX ((sAppOCPCtx_d*)PhAppOCPCtx)
#define S_CONFIGURATION_RL (PS_CNTX->sConfigRL)
#define S_HS (PS_CNTX->sHandshake)
/// @endcond

    do
    {
        //NULL check for handle
        if((NULL == PS_CNTX) || (NULL == PprgbData))
        {
            i4Status = (int32_t)OCP_LIB_NULL_PARAM;
            break;
        }

        //Validate the handle for the sessionID
        i4Status = Registry_ValidateHandleSessionID(PhAppOCPCtx);
        if(OCP_LIB_OK != i4Status)
        {
            break;
        }

        //Null checks for other pointers
        if((NULL == PS_CNTX->sConfigRL.sRL.psConfigTL) || (NULL == S_CONFIGURATION_RL.pfSend) ||
            (NULL == PS_CNTX->sConfigRL.sRL.psConfigTL->pfSend) || (NULL == PS_CNTX->sConfigRL.sRL.psConfigCL) ||
            (NULL == PS_CNTX->sConfigRL.sRL.psConfigCL->pfEncrypt))
        {
            i4Status = (int32_t)OCP_LIB_NULL_PARAM;
            break;
        }

        //Is Authentication session closed
        if(S_HS.eAuthState == eAuthSessionClosed)
        {
            i4Status = (int32_t)OCP_LIB_OPERATION_NOT_ALLOWED;
            break;
        }

        //Is Mutual Authentication complete
        if(S_HS.eAuthState != eAuthCompleted)
        {
            i4Status = (int32_t)OCP_LIB_AUTHENTICATION_NOTDONE;
            break;
        }

        //Zero Length
        if(0x00 == PwLen)
        {
            i4Status = (int32_t)OCP_LIB_LENZERO_ERROR;
            break;
        }

        //If length of data to be sent is greater then Max value
        if(MAX_APP_DATALEN(PhAppOCPCtx) < PwLen)
        {
            i4Status = (int32_t)OCP_LIB_INVALID_LEN;
            break;
        }

        //Memory need to be allocated
        S_CONFIGURATION_RL.sRL.bContentType = CONTENTTYPE_APP_DATA;
        S_CONFIGURATION_RL.sRL.bMemoryAllocated = FALSE;

        //Call Record layer
        i4Status = S_CONFIGURATION_RL.pfSend(&S_CONFIGURATION_RL.sRL, (uint8_t*)PprgbData, PwLen);
        if(OCP_RL_OK != i4Status)
        {
            break;
        }

        i4Status = (int32_t)OCP_LIB_OK;
    }while(FALSE);
/// @cond hidden
#undef PS_CNTX
#undef S_CONFIGURATION_RL
#undef S_HS
/// @endcond
    return i4Status;
}

/**
 * This API receives application data from the DTLS server
 * <br>
 * <br>
 * \image html OCPRecv.png "OCP_Recv()" width=20cm
 *
 *<b>Pre Conditions:</b>
 * - #OCP_Connect() is successful and application context is available.<br>
 *
 *<b>API Details:</b>
 * - Receives application data from the DTLS server.<br>
 * - Application data is received only if Mutual Authentication Public Key Scheme (DTLS) was successfully performed.<br>
 * - Received data is assumed to be encrypted and processed accordingly. Decryption of the application data is done at the record layer.<br>
 * - Total received application data length is updated in PpwLen.<br>
 *<br>
 *
 *<b>User Input:</b><br>
 * - User must provide a valid PhAppOCPCtx handle.<br>
 * - User must provide the buffer where application data should be returned.<br>
 * - User must provide the length of the buffer.<br>
 *   - If the length of the buffer is equal to zero, then #OCP_LIB_LENZERO_ERROR is returned.<br>
 * - User must provide the timeout value in milliseconds. The value should be greater than 0 and maximum up to (2^16)-1.
 *   - If the timeout is zero #OCP_LIB_INVALID_TIMEOUT is returned.
 *
 *<b>Notes:</b>
 * - The maximum length of data that can be received by the API depends upon the PMTU value set during #OCP_Init().This length is indicated by #MAX_APP_DATALEN(PhAppOCPCtx).<br>
 * - If required, the Re-Assembly of received data should be done by the application. This API does not perform data re-assembly.<br>
 * - Failure in decrypting data will return #OCP_LIB_DECRYPT_FAILURE.<br>
 * - If a fatal alert with valid description is received,
 *   - #OCP_AL_FATAL_ERROR is returned.<br>
 *   - User must invoke #OCP_Disconnect() in this condition.Invoking OCP_Send() or OCP_Recv() will return #OCP_LIB_OPERATION_NOT_ALLOWED.<br>
 * - If a valid Hello request is received, the API internally sends a warning alert with description "no-renegotiation" to the server and then waits for data until timeout occurs.<br>
 * - If the length of buffer provided by the application is not sufficient to return received data, #OCP_LIB_INSUFFICIENT_MEMORY is returned. This data will not be returned in subsequent API invocation.<br>
 * - If timeout occurs,#OCP_LIB_TIMEOUT is returned.
 * - Under some failure conditions, error codes from lower layers could also be returned.<br>
 * - In case of a Failure,<br>
 *   - Existing session remains open and memory allocated during OCP_Init() is not freed.<br>
 *   - PhAppOCPCtx handle is not set to NULL.<br>
 *   - The API does not send any alert to the server.<br>
 *   - PpwLen is set to zero.<br>
 * - If the return value is #CMD_DEV_EXEC_ERROR, it might indicate that the application on the security chip is either
 *   closed or a reset has occurred. In such a case, close the existing DTLS session using #OCP_Disconnect.<br>
 *
 *
 * \param[in] PhAppOCPCtx   Handle to OCP Context
 * \param[in,out] PprgbData     Pointer to buffer where data is to be received
 * \param[in,out] PpwLen    Pointer to the length of buffer. Updated with actual length of received data.
 * \param[in] PwTimeout     Timeout in milliseconds
 *
 * \retval  #OCP_LIB_OK
 * \retval  #OCP_LIB_ERROR
 * \retval  #OCP_LIB_NULL_PARAM
 * \retval  #OCP_LIB_SESSIONID_UNAVAILABLE
 * \retval  #OCP_LIB_AUTHENTICATION_NOTDONE
 * \retval  #OCP_LIB_MALLOC_FAILURE
 * \retval  #OCP_LIB_LENZERO_ERROR
 * \retval  #OCP_AL_FATAL_ERROR
 * \retval  #OCP_LIB_INSUFFICIENT_MEMORY
 * \retval  #OCP_LIB_INVALID_TIMEOUT
 * \retval  #OCP_LIB_TIMEOUT
 * \retval  #OCP_LIB_OPERATION_NOT_ALLOWED
 */
int32_t OCP_Receive(const hdl_t PhAppOCPCtx, uint8_t* PprgbData, uint16_t* PpwLen, uint16_t PwTimeout)
{
    int32_t i4Status = (int32_t)OCP_LIB_ERROR;
    sbBlob_d sAppData;
    int32_t i4Alert;
    uint32_t dwStarttime;

/// @cond hidden
#define PS_CNTX ((sAppOCPCtx_d*)PhAppOCPCtx)
#define S_CONFIGURATION_RL (PS_CNTX->sConfigRL)
#define S_HS (PS_CNTX->sHandshake)
/// @endcond
    do
    {
        //NULL check for handle
        if((NULL == PS_CNTX) || (NULL == PprgbData) || (NULL == PpwLen))
        {
            i4Status = (int32_t)OCP_LIB_NULL_PARAM;
            break;
        }

        //Validate the handle for the sessionID
        i4Status = Registry_ValidateHandleSessionID(PhAppOCPCtx);
        if(OCP_LIB_OK != i4Status)
        {
            break;
        }

        //Null checks for other pointers
        if((NULL == PS_CNTX->sConfigRL.sRL.psConfigTL) || (NULL == S_CONFIGURATION_RL.pfRecv) ||
            (NULL == PS_CNTX->sConfigRL.sRL.psConfigTL->pfRecv)|| (NULL == PS_CNTX->sConfigRL.sRL.psConfigCL) ||
            (NULL == PS_CNTX->sConfigRL.sRL.psConfigCL->pfDecrypt))
        {
            i4Status = (int32_t)OCP_LIB_NULL_PARAM;
            break;
        }

        //Is Authentication session closed
        if(S_HS.eAuthState == eAuthSessionClosed)
        {
            i4Status = (int32_t)OCP_LIB_OPERATION_NOT_ALLOWED;
            break;
        }

        //Is Mutual Authentication Public Key Scheme (DTLS) complete
        if(S_HS.eAuthState != eAuthCompleted)
        {
            i4Status = (int32_t)OCP_LIB_AUTHENTICATION_NOTDONE;
            break;
        }

        //Zero Length
        if(0x00 == *PpwLen)
        {
            i4Status = (int32_t)OCP_LIB_LENZERO_ERROR;
            break;
        }

        if(0x00 == PwTimeout)
        {
            i4Status = (int32_t)OCP_LIB_INVALID_TIMEOUT;
            break;
        }

        if(NULL == PS_CNTX->pAppDataBuf)
        {
            PS_CNTX->pAppDataBuf = OCP_MALLOC(TLBUFFER_SIZE);
            if(NULL == PS_CNTX->pAppDataBuf)
            {
                i4Status = (int32_t)OCP_LIB_MALLOC_FAILURE;
                break;
            }
        }

        PS_CNTX->sConfigRL.sRL.psConfigTL->sTL.wTimeout = PwTimeout;

        //Start value for the Flight timeout
        dwStarttime = pal_os_timer_get_time_in_milliseconds();

        do
        {
            PS_CNTX->sConfigRL.sRL.bContentType = CONTENTTYPE_APP_DATA;

            sAppData.prgbStream = PS_CNTX->pAppDataBuf;
            sAppData.wLen = TLBUFFER_SIZE;

            i4Status = PS_CNTX->sConfigRL.pfRecv((sRL_d*)&(PS_CNTX->sConfigRL.sRL), sAppData.prgbStream, &sAppData.wLen);

            //Application record received
            if((int32_t)OCP_RL_APPDATA_RECEIVED == i4Status)
            {
                if(sAppData.wLen > *PpwLen)
                {
                    i4Status = (int32_t)OCP_LIB_INSUFFICIENT_MEMORY;
                    break;
                }

                *PpwLen = sAppData.wLen;
                Utility_Memmove(PprgbData, sAppData.prgbStream, sAppData.wLen);
                i4Status = OCP_LIB_OK;
                break;
            }


            //Alert record received
            if((int32_t)OCP_RL_ALERT_RECEIVED == i4Status)
            {

                i4Status = Alert_ProcessMsg(&sAppData,&i4Alert);
                if(((int32_t)OCP_AL_FATAL_ERROR == i4Alert))
                {
                    S_HS.eAuthState = eAuthSessionClosed;
                    i4Status = i4Alert;
                    break;
                }
            }

            //Malloc failure
            if(((int32_t)OCP_RL_MALLOC_FAILURE == i4Status))
            {
                //Exit the state machine
                break;
            }

            //Decryption failure
            if(((int32_t)CMD_LIB_DECRYPT_FAILURE == i4Status))
            {
                i4Status = (int32_t)OCP_LIB_DECRYPT_FAILURE;
                //Exit
                break;
            }

            //Handshake record received
            if((int32_t)OCP_RL_OK == i4Status)
            {
                i4Status = DtlsHS_VerifyHR(sAppData.prgbStream, sAppData.wLen);
                if(i4Status == OCP_HL_OK)
                {
                    SEND_ALERT(&PS_CNTX->sConfigRL,(int32_t) OCP_LIB_NO_RENEGOTIATE);
                }
            }

            if((uint32_t)(pal_os_timer_get_time_in_milliseconds() - dwStarttime) > (uint32_t)PwTimeout)
            {
                i4Status = (int32_t)OCP_LIB_TIMEOUT;
                break;
            }

            //Dynamically setting the UDP timeout
            PS_CNTX->sConfigRL.sRL.psConfigTL->sTL.wTimeout = (uint16_t)(PwTimeout - (uint16_t)(pal_os_timer_get_time_in_milliseconds() - dwStarttime));
        }while(TRUE);


    }while(FALSE);


    if((OCP_LIB_OK != i4Status) && (NULL != PpwLen))
    {
        *PpwLen = 0x00;
    }

    //lint --e{794} suppress "OCP_LIB_NULL_PARAM check addresses this lint issue which doesn't allow null pointer in this context,"
    if(((int32_t)OCP_LIB_NULL_PARAM != i4Status) && ((int32_t)OCP_LIB_SESSIONID_UNAVAILABLE != i4Status) &&
    ((int32_t)OCP_LIB_OPERATION_NOT_ALLOWED != i4Status)  && ((int32_t)OCP_LIB_AUTHENTICATION_NOTDONE != i4Status) &&
    ((int32_t)OCP_LIB_LENZERO_ERROR != i4Status) && ((int32_t)OCP_LIB_INVALID_TIMEOUT != i4Status) &&
    (NULL != PS_CNTX->pAppDataBuf) && (PS_CNTX->sConfigRL.sRL.bMultipleRecord == 0))
    {
        OCP_FREE(PS_CNTX->pAppDataBuf);
        PS_CNTX->pAppDataBuf = NULL;
    }

/// @cond hidden
#undef PS_CNTX
#undef S_CONFIGURATION_RL
#undef S_HS
/// @endcond
    return i4Status;
}

/**
* This API disconnects the client from the server and closes the DTLS session
* <br>
* <br>
* \image html OCPDisconnect.png "OCP_Disconnect()" width=20cm
*
*<b>Pre Conditions:</b>
* - OCP_Init() or OCP_Connect() is successful.<br>
*
*<b>API Details:</b>
* - Applicable only if called after a successful OCP_Connect()
*   - Closes DTLS session on security chip via CmdLib_CloseSession().<br>
*   - Sends closure alert to the server via the transport layer.<br>
*   - Disconnects from the server via the transport layer.<br>
*   - Applicable if called after successful OCP_Init() or OCP_Connect()
*   - Clear memory associated with PhAppOCPCtx handle.<br>
*   - Clears the internal session reference Id registry.<br>
*   - PhAppOCPCtx handle will not be set to NULL.It is up to the user to check return code and take appropriate action.<br>
* <br>
*
*<b>User Input:</b><br>
* - User must provide a valid PhAppOCPCtx handle.<br>
*
* Notes: <br>
* - If the record sequence number has reached maximum value for epoch 1, No Alert will be send due to the unavailability of record sequence number.<br>
* - DTLS server will not be notified of the fault condition that leads to failure while sending Close_Notify alert.
*
*
* \param[in] PhAppOCPCtx    Handle to OCP Context
*
* \retval  #OCP_LIB_OK
* \retval  #OCP_LIB_NULL_PARAM
* \retval  #OCP_LIB_ERROR
*/
int32_t OCP_Disconnect(hdl_t PhAppOCPCtx)
{
    int32_t i4Status = (int32_t)OCP_LIB_ERROR;
    uint16_t wSessionId = 0;
/// @cond hidden
#define PS_CNTX  ((sAppOCPCtx_d*)PhAppOCPCtx)
#define S_CONFIGURATION_TL (PS_CNTX->sConfigRL.sRL.psConfigTL)
#define S_CONFIGURATION_CL (PS_CNTX->sConfigRL.sRL.psConfigCL)
/// @endcond
    do
    {
        //NULL check for handle
        if(NULL == PS_CNTX)
        {
            i4Status = (int32_t)OCP_LIB_NULL_PARAM;
            break;
        }

        //Validate the handle for the sessionID
        i4Status = Registry_ValidateHandleSessionID(PhAppOCPCtx);
        if(OCP_LIB_OK != i4Status)
        {
            break;
        }

        //Null checks
        if((NULL == S_CONFIGURATION_TL) || (NULL == S_CONFIGURATION_CL) || (NULL == S_CONFIGURATION_CL->pfClose) ||
        (NULL == S_CONFIGURATION_TL->pfDisconnect) || (NULL == PS_CNTX->sConfigRL.pfClose))
        {
            i4Status = (int32_t)OCP_LIB_NULL_PARAM;
            break;
        }

        //Get the session ID associated with the handle
        i4Status = Registry_GetHandleSessionID(PhAppOCPCtx, &wSessionId);
        if(OCP_LIB_OK != i4Status)
        {
            break;
        }

        //Close the session and free the memory allocated
        CloseSession(PhAppOCPCtx, PS_CNTX->sHandshake.fFatalError, wSessionId);

        PhAppOCPCtx = NULL;

    }while(FALSE);

/// @cond hidden
#undef PS_CNTX
#undef S_CONFIGURATION_TL
#undef S_CONFIGURATION_CL
/// @endcond
    return i4Status;
}

/**
* @}
*/
#endif /*MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
