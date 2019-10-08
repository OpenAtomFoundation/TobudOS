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
* \brief   This file implements the functionality to form and process DTLS messages for OCP Library.
*
* \addtogroup  grMutualAuth
* @{
*/

#include "optiga/dtls/MessageLayer.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

#define CALL_BACK_OK            0x00000001


/**
* Call back function to allocate memory to the received message from Security Chip.<br>
*
* \param[in,out]        PpCBParam           Pointer to structure containing information required to allocate memory
* \param[in]            psInOutMsg          Pointer to sBlob containing the received Handshake message from Security Chip
*
* \retval       #CMD_LIB_OK     Successful Execution
* \retval       #CMD_LIB_ERROR  Failure Execution
*/
int32_t CallBack_GetMessage(Void* PpCBParam, const sbBlob_d* psInOutMsg)
{
    int32_t i4Status = (int32_t) CMD_LIB_ERROR;
    uint32_t dwFragLen;
    uint32_t dwTotalLen;
    uint32_t dwOffset;

/// @cond hidden
#define PS_CBGETMSG ((sCBGetMsg_d*)PpCBParam)
#define OFFSET_MSG_FRAG_LENGTH      9
#define OFFSET_MSG_TOTAL_LENGTH     1
#define OFFSET_MSG_FRAGMENT_OFFSET  6
/// @endcond

    do
    {
        dwFragLen = Utility_GetUint24(psInOutMsg->prgbStream + OFFSET_MSG_FRAG_LENGTH);
        dwTotalLen = Utility_GetUint24(psInOutMsg->prgbStream + OFFSET_MSG_TOTAL_LENGTH);
        dwOffset =  Utility_GetUint24(psInOutMsg->prgbStream + OFFSET_MSG_FRAGMENT_OFFSET);

        //If first time, allocate memory
        if(FALSE == PS_CBGETMSG->bRepeatCall)
        {
            //Allocate memory
            PS_CBGETMSG->dwMsgLen = (uint16_t)dwTotalLen + OVERHEAD_LEN;
            PS_CBGETMSG->pbActualMsg = (uint8_t*)OCP_MALLOC(dwTotalLen + OVERHEAD_LEN);
            if(PS_CBGETMSG->pbActualMsg == NULL)
            {
                i4Status = (int32_t)OCP_ML_MALLOC_FAILURE;
                break;
            }
            //Set to true indicating memory is already allocated
            PS_CBGETMSG->bRepeatCall = TRUE;

            OCP_MEMCPY(PS_CBGETMSG->pbActualMsg + (OVERHEAD_LEN - MSG_HEADER_LEN),psInOutMsg->prgbStream,(uint16_t)(dwFragLen + MSG_HEADER_LEN));

            //Set fragment offset to zero and fragment len to total len
            Utility_SetUint24((PS_CBGETMSG->pbActualMsg + (OVERHEAD_LEN - 3)), dwTotalLen);
        }
        else
        {
            //copy data from offset
            OCP_MEMCPY(PS_CBGETMSG->pbActualMsg + dwOffset + OVERHEAD_LEN, (psInOutMsg->prgbStream + MSG_HEADER_LEN), (uint16_t)dwFragLen);
        }
        i4Status = CMD_LIB_OK;
    }while(FALSE);

/// @cond hidden
#undef OFFSET_MSG_FRAG_LENGTH
#undef OFFSET_MSG_TOTAL_LENGTH
#undef OFFSET_MSG_FRAGMENT_OFFSET
#undef PS_CBGETMSG
/// @endcond

    return i4Status;

}

/**
 * Provide Handshake message using Command Library.<br>
 * Under some erroneous conditions, error codes from Command Library can also be returned.<br>
 *
 * \param[in]       eMsgType                Message type of the handshake message to be received from Security Chip
 * \param[in]       PpsMessageLayer         Pointer to structure containing information required for Message Layer
 * \param[out]       PpsMessage             Pointer to sBlob containing the Handshake message
 *
 * \retval      #OCP_ML_OK                  Successful Execution
 * \retval      #OCP_ML_ERROR               Failure Execution
 * \retval      #OCP_ML_INVALID_UNIXTIME    Invalid unix time provided by user
 */
int32_t MsgLayer_FormMessage(eMsgType_d eMsgType,const sMessageLayer_d* PpsMessageLayer, sbBlob_d* PpsMessage)
{
    int32_t i4Status = (int32_t) OCP_ML_ERROR;
    sProcMsgData_d sGMsgVector;
    sCallBack_d sCallBack;
    sCBGetMsg_d sCBGetMsg;
    uMsgParams_d uMsgParams;

    do
    {
        //Null Check
        if((NULL == PpsMessageLayer) || (NULL == PpsMessage))
        {
            i4Status = (int32_t)OCP_ML_NULL_PARAM;
            break;
        }

        //Assign the call back function parameters
        sCBGetMsg.bRepeatCall = FALSE;
        sCBGetMsg.pbActualMsg = NULL;
        sCBGetMsg.dwMsgLen = 0;

        //Assign call back function to allocate memory
        sCallBack.pfAcceptMessage = CallBack_GetMessage;
        sCallBack.fvParams = (Void*) &sCBGetMsg;

        //Form the Get message command APDU parameters
        sGMsgVector.eParam = eMsgType;
        sGMsgVector.wSessionKeyOID = PpsMessageLayer->wSessionID;
        sGMsgVector.psCallBack = &sCallBack;
        sGMsgVector.psBlobInBuffer = NULL;
        //sGMsgVector.puMsgParams = &uMsgParams;
        sGMsgVector.puMsgParams = NULL;

        //Based on the message type set the message specific data
        switch((uint8_t)eMsgType)
        {
            case eClientHello:
                if(NULL != PpsMessageLayer->pfGetUnixTIme)
                {
                    sGMsgVector.puMsgParams = &uMsgParams;

                    //To use the unix time provided
                    i4Status = PpsMessageLayer->pfGetUnixTIme(&sGMsgVector.puMsgParams->sMsgParamCH_d.dwUnixTime);
                    if(CALL_BACK_OK != i4Status)
                    {
                        i4Status = (int32_t) OCP_ML_INVALID_UNIXTIME;
                        break;
                    }
                }
                break;
            case eClientCertificate:
                    if(0x0000 != PpsMessageLayer->wOIDDevCertificate)
                    {
                        sGMsgVector.puMsgParams = &uMsgParams;
                        sGMsgVector.puMsgParams->sMsgParamCert_d.wCertOID = PpsMessageLayer->wOIDDevCertificate;
                    }

                break;
            default:
                //lint -e788 suppress "Default values for return are already set.No more operation required here"
                //For other messages there is no message specific data
                break;
        }

        if(((int32_t) OCP_ML_INVALID_UNIXTIME == i4Status) || ((int32_t) OCP_ML_INVALID_CERTTYPE == i4Status))
        {
            break;
        }
        //Get the Message using Get Message command from the Security Chip
        i4Status =  CmdLib_GetMessage(&sGMsgVector);
        if(CMD_LIB_OK != i4Status)
        {
            LOG_TRANSPORTDBVAL(i4Status,eInfo);
            break;
        }

        //Allocated memory is assigned the sBlob
        PpsMessage->prgbStream = sCBGetMsg.pbActualMsg;
        PpsMessage->wLen = sCBGetMsg.dwMsgLen;

        i4Status = (int32_t) OCP_ML_OK ;
    }while(FALSE);

return i4Status;
}

/**
 * Process Handshake message using Command Library.<br>
 * Under some erroneous conditions, error codes from Command Library can also be returned. <br>
 *
 * \param[in]       eMsgType                Message type of the handshake message to be send to Security Chip
 * \param[in]       PpsMessageLayer         Pointer to structure containing information required for Message Layer
 * \param[in]       PpsMessage              Pointer to sBlob containing the Handshake message
 *
 * \retval      #OCP_ML_OK      Successful Execution
 * \retval      #OCP_ML_ERROR   Failure Execution
 */
int32_t MsgLayer_ProcessMessage(eMsgType_d eMsgType,const sMessageLayer_d* PpsMessageLayer, sbBlob_d* PpsMessage)
{
    int32_t i4Status = (int32_t) OCP_ML_ERROR;
    sProcMsgData_d sPMsgVector ;

    do
    {
        //Null Check
        if((NULL == PpsMessageLayer) || (NULL == PpsMessage) || (NULL == PpsMessage->prgbStream))
        {
            i4Status = (int32_t)OCP_ML_NULL_PARAM;
            break;
        }

        //Length check for input parameters
        if(0 == PpsMessage->wLen)
        {
            i4Status = (int32_t)OCP_ML_ZERO_LEN;
            break;
        }

        //Assign the required parameter(s) for the Put Message command
        sPMsgVector.eParam = eMsgType;
        sPMsgVector.psBlobInBuffer = PpsMessage;
        sPMsgVector.wSessionKeyOID = PpsMessageLayer->wSessionID;
        sPMsgVector.puMsgParams = NULL;
        sPMsgVector.psCallBack = NULL;

        //Invoke the Put Message command API from the command library to send the message to Security Chip to Process
        i4Status = CmdLib_PutMessage(&sPMsgVector);
        if(CMD_LIB_OK != i4Status)
        {
            LOG_TRANSPORTDBVAL(i4Status,eInfo);
            break;
        }

        i4Status = (int32_t) OCP_ML_OK;
    }while(FALSE);

#undef PhOCPHandle
    return i4Status;
}

/**
* @}
*/
#endif /*MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
