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
* \file
*
* \brief This file contains a light weight logger implementation.
*
* \addtogroup  grLogger
* @{
*
*/

#include <stdio.h>
#include "optiga/common/Logger.h"
#include "optiga/pal/pal_os_timer.h"
/// @cond hidden
/*****************************************************************************
*  Defines
*****************************************************************************/
//If ENABLE_UARTLOG is defined, assign UartWriteData as log writer
#ifdef ENABLE_UARTLOG
#include "Uart.h"
pFWriteData pfWriter = (pFWriteData)UartWriteData;
#else
//The function does nothing
static int32_t WriteData(uint32_t PdwHandle, const uint8_t* PprgbBuf, uint32_t PdwDataLen)
{
    //lint --e{715} suppress "The parameters are not used as this is filler function"
    return 1;
}
pFWriteData pfWriter = (pFWriteData)WriteData;
#endif

/*****************************************************************************
*  Common Functions
*****************************************************************************/
/**
* Convert Byte to Hex String
*
*/
void ConvUint8ToHexString (uint8_t* PprgbHexByteArray, uint8_t* PprgbHexString, \
                        uint32_t dwNoOfBytes, uint8_t PbIsSpaceReq)
{
    //lint --e{818} suppress "PprgbHexString is modified in function"
    uint32_t dwLoop = 0;
    uint8_t bNibble = 0, bHexByte = 0;

    do
    {
        if((NULL == PprgbHexByteArray) || (NULL == PprgbHexString))
        {
            return;
        }

        for (dwLoop = 0; dwLoop < dwNoOfBytes; dwLoop++)
        {
            bHexByte = PprgbHexByteArray[dwLoop];

            /*Convert Byte to HexString */
            bNibble = (bHexByte & 0xF0)>>4;
            if (bNibble > 0x09)
                PprgbHexString [0] = bNibble + 0x37;
            else
                PprgbHexString [0] = bNibble + 0x30;

            bNibble = bHexByte & 0x0F;
            if (bNibble > 0x09)
                PprgbHexString [1] = bNibble + 0x37;
            else
                PprgbHexString [1] = bNibble + 0x30;

            if(PbIsSpaceReq)
            {
                PprgbHexString [2] = ' ';
                PprgbHexString += 3;
            }
            else
            {
                PprgbHexString += 2;
            }
        }
        *PprgbHexString = 0x00;
    } while(0);
}

/**
* Convert Uint32 to Hex String
*
*/
void ConvUint32ToHexString (uint32_t dwVal, uint8_t* PprgbHexString)
{
    uint8_t rgbByteArray [5];

    do
    {
        if(NULL == PprgbHexString)
        {
            return;
        }

        rgbByteArray [0] = (uint8_t)(dwVal >> 24);
        rgbByteArray [1] = (uint8_t)((dwVal & 0x00FF0000) >> 16);
        rgbByteArray [2] = (uint8_t)((dwVal & 0x0000FF00) >> 8);
        rgbByteArray [3] = (uint8_t)((dwVal & 0x000000FF));

        ConvUint8ToHexString(rgbByteArray, PprgbHexString, 4, 0);
    } while(0);
}

/**
* Convert Uint32 to Decimal String
*
*/
void ConvUint32ToDecString (uint32_t dwVal, uint8_t* PprgbDecStr, \
                            uint8_t bExpStrLen, uint8_t bFillChar)
{
    uint8_t rgbTempStr [12] = {0};
    uint8_t bCount;

    do
    {
        if(NULL ==PprgbDecStr)
        {
            return;
        }

        bCount = 0;
        for (;;)
        {
            rgbTempStr [bCount] = (dwVal % 10) + 0x30;
            dwVal = dwVal / 10;
            bCount++;
            if(0x00 == dwVal)
            {
                break;
            }
        }

        while(bExpStrLen > bCount)
        {
            *PprgbDecStr = bFillChar;
            PprgbDecStr++;
            bExpStrLen--;
        }
        bCount--;
        for(;;)
        {
            *PprgbDecStr = rgbTempStr[bCount];
            PprgbDecStr++;
            if(0x00 == bCount)
            {
                break;
            }
            bCount--;
        }

        *PprgbDecStr = 0x00;
    } while(0);
}

/*****************************************************************************
*  Static functions
*****************************************************************************/
#ifdef ENABLE_LOG
/**
* Return current system time in milliseconds as a string
*
*/
static void GetSystemDateTime(char_t *pszSystemTime)
{
    uint32_t dwTimeInMilliSecs = 0;

    if(pszSystemTime == NULL)
    {
        return;
    }

    dwTimeInMilliSecs = pal_os_timer_get_time_in_milliseconds();
    ConvUint32ToDecString (dwTimeInMilliSecs, (uint8_t *)pszSystemTime, 10, '0');
}

/**
* Perform packet analysis. This is specific to IFX I2C protocol
*
*/
static void DumpPacketAnalysis(uint8_t* prgbBuf, uint16_t wLen, bool_t fDirection)
{
    uint16_t wOffset = 0, wFrameLength, wTemp;
    uint8_t bFctr, bPctr, bTemp, bAckNumber;
    char_t pszTemp[256];
    bool_t bControlFrame = TRUE;

    //for packet analysis there must be minimum 5 bytes
    if(wLen < 5)
    {
        return;
    }

    if(prgbBuf == NULL)
    {
        return;
    }

    //0 for send
    if(fDirection == TX_DIRECTION)
    {
        wOffset = 1;
        CONSOLE_LOGSTRINGLINE("->->->->-> Packet Analysis ->->->->->");
    }
    else
    {
        CONSOLE_LOGSTRINGLINE("<-<-<-<-<- Packet Analysis <-<-<-<-<-");
    }

    do
    {
        //frame type
        bFctr = *(prgbBuf+wOffset);
        bAckNumber = bFctr & 0x03;

        if(bFctr & 0x80)
        {
            CONSOLE_LOGSTRINGLINE("Frame type:    Control frame");
        }
        else
        {
            bControlFrame = FALSE;
            CONSOLE_LOGSTRINGLINE("Frame type:    Data frame");
        }
        //seq counter 0110 0000 = 0x60
        bTemp = ((bFctr & 0x60) >> 5);
        switch(bTemp)
        {
            case 0x00:
                CONSOLE_LOGSTRING("Seq Counter:   Ack for ");
                ConvUint32ToDecString((uint32_t)bAckNumber,(uint8_t*)pszTemp, 0, '0');
                CONSOLE_LOGSTRINGLINE(pszTemp);
            break;

            case 0x01:
                CONSOLE_LOGSTRING("Seq Counter:   Nak for ");
                ConvUint32ToDecString((uint32_t)bAckNumber,(uint8_t*)pszTemp, 0, '0');
                CONSOLE_LOGSTRINGLINE(pszTemp);
            break;

            case 0x02:
                CONSOLE_LOGSTRINGLINE("Seq Counter:   Re-synch");
            break;

            default:
                CONSOLE_LOGSTRINGLINE("************************** Seq Counter: RFU ***********************");
        }

        //frame number 0000 1100 = 0x60
        bTemp = ((bFctr & 0x0C) >> 2);
        CONSOLE_LOGSTRING("Frame number:  ");
        ConvUint32ToDecString((uint32_t)bTemp,(uint8_t*)pszTemp, 0, '0');
        CONSOLE_LOGSTRINGLINE(pszTemp);

        //ack number
        CONSOLE_LOGSTRING("Ack number:    ");
        ConvUint32ToDecString((uint32_t)bAckNumber,(uint8_t*)pszTemp, 0, '0');
        CONSOLE_LOGSTRINGLINE(pszTemp);

        //Frame length:
        wOffset++;
        wFrameLength = ((uint16_t)*(prgbBuf + wOffset) << 8) | (uint16_t)*(prgbBuf + wOffset + 1);
        CONSOLE_LOGSTRING("Frame length:  ");
        ConvUint32ToDecString((uint32_t)wFrameLength,(uint8_t*)pszTemp, 0, '0');
        CONSOLE_LOGSTRINGLINE(pszTemp);

        wOffset += 2;

        //N/w and transport info not present for control frame
        if(bControlFrame)
        {
            break;
        }
        //channel info
        bPctr = *(prgbBuf+wOffset);
        bTemp = bPctr >> 4;
        CONSOLE_LOGSTRING("Channel info:  ");
        ConvUint32ToDecString((uint32_t)bTemp,(uint8_t*)pszTemp, 0, '0');
        CONSOLE_LOGSTRINGLINE(pszTemp);

        bTemp = bPctr & 0x07;
        switch(bTemp)
        {
            case 0x00:
                CONSOLE_LOGSTRINGLINE("Chaining info: Single packet");
            break;

            case 0x01:
                CONSOLE_LOGSTRINGLINE("Chaining info: First packet");
            break;

            case 0x02:
                CONSOLE_LOGSTRINGLINE("Chaining info: Intermediate packet");
            break;

            case 0x04:
                CONSOLE_LOGSTRINGLINE("Chaining info: Last packet");
            break;

            case 0x07:
                CONSOLE_LOGSTRINGLINE("********************* Chaining info: Chaining error ********************* ");
            break;

            default:
                CONSOLE_LOGSTRINGLINE("********************* Chaining info: RFU***********************");
        }

        wOffset += 1;

        //exclude till offset and checksum
        wTemp = wOffset+2;
        if(wLen > wTemp)
        {
            wTemp = wLen - wTemp;
        }
        else
        {
            // no data bytes
            break;
        }

        if(fDirection == TX_DIRECTION)
        {
            CONSOLE_LOGSTRING("Command data : ");
        }
        else
        {
            CONSOLE_LOGSTRING("Response data : ");
        }

        Util_DumpHex((prgbBuf+wOffset), wTemp);
        CONSOLE_LOGSTRINGLINE(" ");
    }while(0);

    //0 for send
    if(fDirection == TX_DIRECTION)
    {
        CONSOLE_LOGSTRINGLINE("->->->->-> Packet Analysis ->->->->->");
    }
    else
    {
        CONSOLE_LOGSTRINGLINE("<-<-<-<-<- Packet Analysis <-<-<-<-<-");
    }
}
#endif

/*****************************************************************************
*  Exposed Logging APIs
*****************************************************************************/
/**
* Logs a New Line
*/
void Util_NewLine(uint32_t PdwUartPort)
{
    uint8_t rgbcrlf [2] = {0x0D, 0x0A};
    //lint --e{534} The return value is not used*/
    pfWriter(PdwUartPort, rgbcrlf, 2);
}

/**
* Logs a string with new line
*/
void Util_LogStringLine(uint32_t PdwUartPort, const char_t *pszString)
{
    if(pszString == NULL)
    {
        return;
    }
    //lint --e{534} The return value is not used*/
    pfWriter(PdwUartPort, (uint8_t *)pszString, strlen(pszString));
    Util_NewLine(PdwUartPort);
}


/**
* Logs a string
*/
void Util_LogString(uint32_t PdwUartPort, const char_t *PpszString)
{
    if(PpszString == NULL)
    {
        return;
    }
    //lint --e{534} The return value is not used*/
    pfWriter(PdwUartPort, (uint8_t *)PpszString, strlen(PpszString));
}

#ifdef ENABLE_LOG
/**
* Logs a byte array
*/
void Util_LogArray(uint8_t* prgbBuf, uint16_t wLen, bool_t fDirection, bool_t fDumpPacketAnalysis)
{
    char_t szTemp[50];

    if(prgbBuf == NULL)
    {
        return;
    }

    memset(szTemp,0, 50);

    GetSystemDateTime(szTemp);
    //lint --e{534} The return value is not used*/
    pfWriter(CONSOLE_PORT, (uint8_t*)szTemp, strlen(szTemp));
    //lint --e{534} The return value is not used*/
    pfWriter(CONSOLE_PORT, (uint8_t*)" ", 1);

    if(fDirection == TX_DIRECTION)
    {
        //lint --e{534} The return value is not used*/
        pfWriter(CONSOLE_PORT, (uint8_t*)">>  ", 4);
    }
    else
    {
        //lint --e{534} The return value is not used*/
        pfWriter(CONSOLE_PORT, (uint8_t*)"<<  ", 4);
    }

    Util_DumpHex(prgbBuf, wLen);
    Util_NewLine(CONSOLE_PORT);

    if(fDumpPacketAnalysis)
    {
        DumpPacketAnalysis(prgbBuf, wLen, fDirection);
    }
    return;
}
#endif


/**
* Logs a four byte value
*/
void Util_LogInt(uint32_t PdwUartPort, const char_t *pszMsg, uint32_t dwValue)
{
    uint8_t rgbString [12] = {0};

    if(pszMsg == NULL)
    {
        return;
    }
    Util_LogString(PdwUartPort, pszMsg);
    Util_LogString(PdwUartPort, " 0x");
    ConvUint32ToHexString((uint32_t)dwValue, rgbString);
    Util_LogString(PdwUartPort, (char_t*)rgbString);
    Util_LogString(PdwUartPort, "(");
    ConvUint32ToDecString((uint32_t)dwValue, rgbString, 0, '0');
    Util_LogString(PdwUartPort, (char_t*)rgbString);
    Util_LogStringLine(PdwUartPort, ")");
}


/**
* Logs an array in hex format
*/
void Util_DumpHex(uint8_t* prgbBuf, uint16_t wLen)
{
    uint16_t wIndex;
    uint8_t rgbHexString[5];

    if(prgbBuf == NULL)
    {
        return;
    }
    for(wIndex = 0; wIndex < wLen; wIndex++)
    {
        ConvUint8ToHexString ((uint8_t*)(prgbBuf+wIndex), rgbHexString, 1, 1);
        //lint --e{534} The return value is not used*/
        pfWriter(CONSOLE_PORT, rgbHexString, 3);
    }
}




/*****************************************************************************
*  Level based logging Exposed APIs
*****************************************************************************/
/// @endcond
#ifdef ENABLE_LOG
//This is second log writer
pFWriteData2 pfWriter2 = NULL;
//This is the arguement to be passed to pfWriter.It refers to the handle to writer context/structure.
void* pHandle = NULL;
//Stores the level of Logging
static uint32_t dwLevelEnabler   = (uint32_t)((1<<eInfo)|(1<<eWarning)|(1<<eError));
//Store the layer type of Logging
static uint32_t dwLayerEnabler = 0;

/**
* \brief This structure contains Logging information
*/
typedef struct sLogMessage {
    ///Message to be logged
    char_t* pzStringMessage;
    ///Message Type
    eLogLayer eLogMsgLayer;
    ///Message Level
    eLogLevel eLogMsgLevel;
}sLogMessage;


/**
* Sets the state of the Logging Level.
*
* \param[in] eLevel Logging Level
* \param[in] eValue Set value
*/
void Util_SetLogLevelState(eLogLevel PeLevel,eSetState PeValue)
{
    //Validate Level
    if((eInfo <= PeLevel)&&(eError >= PeLevel))
    {
        switch(PeValue)
        {
            case eEnable:
            {
                dwLevelEnabler |= (uint32_t)(1<<PeLevel);
                break;
            }
            case eDisable:
            {
                dwLevelEnabler &= (uint32_t)(~(1<<PeLevel));
                break;
            }
            case eInvalid:
            default:
                break;
        }
    }
}


/**
* Returns the current state of Logging level.
*
* \param[in] eLevel Logging Level
*
* \retval  #eSetState
*/
eSetState Util_GetLogLevelState(eLogLevel PeLevel)
{
    //Validate Level
    if((eInfo <= PeLevel)&&(eError >= PeLevel))
        return (((dwLevelEnabler)&(1<<PeLevel))?eEnable:eDisable);
    else
        return eInvalid;
}


/**
* Sets the state of the Logging Layer.
*
* \param[in] eLayer Logging Layer
* \param[in] eValue Set value
*/
void Util_SetLogLayerState(eLogLayer PeLayer,eSetState PeValue)
{
    //Validate Layer
    if((eHS <= PeLayer)&&(eTL >= PeLayer))
    {
        switch(PeValue)
        {
            case eEnable:
            {
                dwLayerEnabler |= (uint32_t)(1<<PeLayer);
                break;
            }
            case eDisable:
            {
                dwLayerEnabler &= (uint32_t)(~(1<<PeLayer));
                break;
            }
            case eInvalid:
            default:
                break;
        }
    }
}


/**
* Returns the current state of the Logging Layer.
*
* \param[in] eLayer Logging Level
*
* \retval  #eSetState
*/
eSetState Util_GetLogLayerState(eLogLayer PeLayer)
{
    //Validate Layer
    if((eHS <= PeLayer)&&(eTL >= PeLayer))
        return (((dwLayerEnabler)&(1<<PeLayer))?eEnable:eDisable);
    else
        return eInvalid;
}


/**
* Sets the Log Writer and handle.
*
* \param[in] pWriter function pointer to pFWriteData2
* \param[in] pHdl Handle to writer context/structure
*
*/
void Util_SetLogWriter(pFWriteData2 pWriter,void* pHdl)
{
    pfWriter2 = pWriter;
    pHandle = pHdl;
}


/**
 * \brief   Logs a message.
 */
static void Util_WriteMessage(sLogMessage* psLogMessage)
{

    char_t charBuffer[103];
    char timeString[9];  // space for "HH:MM:SS\0"
    char_t* szMsgLevel[eError] = {LOGGER_LEVEL_INFO,
                                    LOGGER_LEVEL_WARNING,
                                    LOGGER_LEVEL_ERROR};

    char_t* szMsgType[eTL] = {LOGGER_TYPE_HANDSHAKE,
                                LOGGER_TYPE_RECORDLAYER,
                                LOGGER_TYPE_TRANSPORTLAYER};

        GetSystemDateTime(timeString);

#ifndef WIN32
        sprintf(charBuffer,LOG_FORMAT,timeString,szMsgLevel[psLogMessage->eLogMsgLevel -1 ],
            szMsgType[psLogMessage->eLogMsgLayer -1 ],psLogMessage->pzStringMessage,"\n");
#else
        sprintf_s(charBuffer,103,LOG_FORMAT,timeString,szMsgLevel[psLogMessage->eLogMsgLevel -1 ],
            szMsgType[psLogMessage->eLogMsgLayer -1 ],psLogMessage->pzStringMessage,"\n");
#endif
        pfWriter2(pHandle,(uint8_t*)charBuffer,strlen(charBuffer));

}

/**
* Logs a message with type and level information and content of the buffer.
* Currently the message cannot be greater than 80 bytes.This will be upgraded in future
*
* \param[in] pzMsg          Message to be logged
* \param[in] eLayer         Logging Layer
* \param[in] eLevel         Logging Level
* \param[in] PrgbBuffer     Pointer to the buffer to be logged
* \param[in] wLen           Length message to be logged
*
*/
void Util_LogMsgArray(char* pzMsg, uint8_t* PrgbBuffer, uint16_t wLen, eLogLayer eLayer, eLogLevel eLevel)
{
    sLogMessage sLogMes;
    uint16_t wCount = wLen;
    uint8_t bBytes = 25;
    uint8_t * prgbbuf = PrgbBuffer;
    eSetState eCurrentState = Util_GetLogLevelState(eLevel);
    uint8_t rgbHexString[100];
    do
    {
        if((NULL==pHandle)||(NULL ==pfWriter2) || (eEnable != eCurrentState) || (PrgbBuffer == NULL))
        {
            break;
        }
        sLogMes.eLogMsgLevel = eLevel;
        sLogMes.eLogMsgLayer = eLayer;
        sLogMes.pzStringMessage = pzMsg;
        Util_WriteMessage(&sLogMes);

        while(wCount > 0)
        {
            if(wCount < 25)
            {
               bBytes =  (uint8_t)wCount;
            }
            ConvUint8ToHexString ((uint8_t*)(prgbbuf), rgbHexString, bBytes, 1);
            pfWriter2(pHandle, rgbHexString, (bBytes*3));
            prgbbuf+= bBytes;
            wCount-=bBytes;
        }
        pfWriter2(pHandle, (uint8_t *)"\n", 1);
    }while (0);
}



/**
* Logs a message with type and level information.
* Currently the message cannot be greater than 80 bytes.This will be upgraded in future
*
* \param[in] pzMsg Message to be logged
* \param[in] eLayer Logging Layer
* \param[in] eLevel Logging Level
*
*/
void Util_LogMessage(char* pzMsg, eLogLayer eLayer, eLogLevel eLevel)
{
    sLogMessage sLogMes;
    eSetState eCurrentState = Util_GetLogLevelState(eLevel);

    do
    {
        if((NULL==pHandle)||(NULL ==pfWriter2) || (eEnable != eCurrentState))
        {
            break;
        }
        sLogMes.eLogMsgLevel = eLevel;
        sLogMes.eLogMsgLayer = eLayer;
        sLogMes.pzStringMessage = pzMsg;
        Util_WriteMessage(&sLogMes);

    }while (0);
}

/**
* Logs a 4 byte debug value with type and level information.
*
* Currently the message cannot be greater than 80 bytes.This will be upgraded in future
* \param[in] dwDBValue 4 byte value to be logged
* \param[in] eLayer Logging Layer
* \param[in] eLevel Logging Level
*
*/
void Util_LogDebugVal(uint32_t dwDBValue, eLogLayer eLayer, eLogLevel eLevel)
{
    uint8_t rgbString [12];

    if((NULL!=pHandle)&&(NULL !=pfWriter2))
    {
        ConvUint32ToHexString(dwDBValue, rgbString);
        Util_LogMessage((char_t*)rgbString,eLayer,eLevel);
    }
}

#endif //#ENABLE_LOG

/**
* @}
*/
