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
* \brief This file contains a light weight logger.
*
*
* \addtogroup  grLogger
* @{
*
*/
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "optiga/common/Util.h"
#ifdef WIN32
#include <stdio.h>
#endif

/// @cond hidden
/*****************************************************************************
*  Defines
*****************************************************************************/
/// Flag for transmit direction
#define TX_DIRECTION       0
/// Flag for reception direction
#define RX_DIRECTION       1
/// Flag to write to console UART
#define CONSOLE_PORT        0
/// Flag to write to cuint UART
#define CUNIT_PORT          1
/// Flag to write to code coverage UART
#define CODE_COVERAGE_PORT  2

//Pointer to writer
typedef int32_t (*pFWriteData)(uint32_t PdwPort, uint8_t* PprgbBuf, uint32_t PdwDataLen);
///This is pointer to log writer
extern pFWriteData pfWriter;

///This function converts Uint8 value to Hex String
void ConvUint8ToHexString (uint8_t* PprgbHexByteArray, uint8_t* PprgbHexString, \
                        uint32_t dwNoOfBytes, uint8_t PbIsSpaceReq);

///This function converts Uint32 to Hex String
void ConvUint32ToHexString (uint32_t dwVal, uint8_t* PprgbHexString);


///This function converts Uint32 value to Decimal String
void ConvUint32ToDecString (uint32_t dwVal, uint8_t* PprgbDecStr, \
                            uint8_t bExpStrLen, uint8_t bFillChar);

/*****************************************************************************
*  Exposed Logging APIs
*****************************************************************************/
/// Logs a new line
void Util_NewLine(uint32_t PdwUartPort);

/// Logs a string with newline at the end
void Util_LogStringLine(uint32_t PdwUartPort, const char_t *pszString);

/// Logs a string
void Util_LogString(uint32_t PdwUartPort, const char_t *pszString);

//Logs Integer
void Util_LogInt(uint32_t PdwUartPort, const char_t *pszMsg, uint32_t dwValue);

/// Logs a byte array
void Util_LogArray(uint8_t* prgbBuf, uint16_t wLen, bool_t fDirection, bool_t fDumpPacketAnalysis);

/// Print an array in hex format
void Util_DumpHex(uint8_t* prgbBuf, uint16_t wLen);

//Print on console port with new line
#define CONSOLE_LOGSTRINGLINE(pszString) Util_LogStringLine(CONSOLE_PORT,pszString);

//Print on console port
#define CONSOLE_LOGSTRING(pszString) Util_LogString(CONSOLE_PORT,pszString);

//Print on console port
#define CONSOLE_LOGINTEGER(pszString,value) Util_LogInt(CONSOLE_PORT,pszString,value);

//Print on Cunit port
#define CUNIT_LOGSTRING(pszString) Util_LogString(CUNIT_PORT,pszString);

//Print on coverage port
#define COVERAGE_LOGSTRING(pszString,dwLen) pfWriter(CODE_COVERAGE_PORT,pszString,dwLen);


/*****************************************************************************
*  Level based logging Exposed APIs
*****************************************************************************/
/// @endcond
/**
 * \brief Structure to Logger.
 */
typedef struct sLogger_d
{
    ///Handle to instance of writer
    uint32_t* pHdl;
    ///Log Writer
    hdl_t phfWriter;//This to be replaced by actual type
}sLogger_d;

#ifdef ENABLE_LOG
//This is second write
typedef int32_t (*pFWriteData2)(void* Hdl, uint8_t* PprgbBuf, uint32_t PdwDataLen);

//This is the arguement to be passed to pfWriter.It refers to the handle to writer context/structure.
extern void* pHandle;

//This is second log writer
extern pFWriteData2 pfWriter2;

///Delimiter to indicate "Seperator Open"
#define DELIMITER_OPEN                  "["
///Delimiter to indicate "Seperator Close"
#define DELIMITER_CLOSE                 "]"
///Format to print time as [hh:mm:ss][level][type] :message newline
#define LOG_FORMAT "[%s]%-4s%-2s:%s%s"

///Log message Level
///This indicates message is an Information
#define LOGGER_LEVEL_INFO               DELIMITER_OPEN"Info"DELIMITER_CLOSE
///This indicates message is a Warning
#define LOGGER_LEVEL_WARNING            DELIMITER_OPEN"Warn"DELIMITER_CLOSE
///This indicates message is an Error
#define LOGGER_LEVEL_ERROR              DELIMITER_OPEN"Erro"DELIMITER_CLOSE

///Log message Type
///This indicates message is logged from Handshake Layer
#define LOGGER_TYPE_HANDSHAKE           DELIMITER_OPEN"HS"DELIMITER_CLOSE
///This indicates message is logged from Record Layer
#define LOGGER_TYPE_RECORDLAYER         DELIMITER_OPEN"RL"DELIMITER_CLOSE
///This indicates message is logged from Transport Layer
#define LOGGER_TYPE_TRANSPORTLAYER      DELIMITER_OPEN"TL"DELIMITER_CLOSE



/**
* \brief This structure represents Log Level Types
*/
typedef enum eLogLevel {
    ///Information
    eInfo = 1,
    ///Warning
    eWarning = 2,
    ///Error
    eError = 3,
}eLogLevel;

/**
* \brief This structure represents Log Layer Types
*/
typedef enum eLogLayer {
    ///Handshake
    eHS = 1,
    ///Record
    eRL = 2,
    ///Transport
    eTL = 3,
}eLogLayer;


/**
* \brief This structure contains state of logging
*/
typedef enum eSetState {
    ///Enable Logging
    eEnable = 0x01,
    ///Disable Logging
    eDisable = 0x02,
    //Invalid
    eInvalid = 0xFF
}eSetState;

/**
 * \brief   Returns the current state of Logging level.
 */
eSetState Util_GetLogLevelState(eLogLevel level);
/**
 * \brief   Sets the state of the Logging Level.
 */
void Util_SetLogLevelState(eLogLevel type,eSetState value);
/**
 * \brief   Returns the current state of the Logging Layer.
 */
eSetState Util_GetLogLayerState(eLogLayer layer);
/**
 * \brief   Sets the state of the Logging Layer.
 */
void Util_SetLogLayerState(eLogLayer layer,eSetState value);
/**
 * \brief   Sets the Log Writer and handle.
 */
void Util_SetLogWriter(pFWriteData2 writer,void* pHdl);
/**
 * \brief   Logs a message with type and level information.
 */
void Util_LogMessage(char* message, eLogLayer layer, eLogLevel level);
/**
 * \brief   Logs a 4 byte debug value with type and level information.
 */
void Util_LogDebugVal(uint32_t dwDBValue, eLogLayer layer, eLogLevel level);
/**
 * \brief   Logs a message with type,level information and also the content of the buffer.
 */
void Util_LogMsgArray(char* pzMsg, uint8_t* PrgbBuffer, uint16_t wLen, eLogLayer eLayer, eLogLevel eLevel);
#endif

///Define ENABLE_LOG to enable logging
#ifdef ENABLE_LOG
///Disable Layer during runtime
#define DISABLE_LOGGING_LAYER(layer) Util_SetLogLayerState(layer,eDisable)
///Enable Layer during runtime
#define ENABLE_LOGGING_LAYER(layer) Util_SetLogLayerState(layer,eEnable)
///Get Layer State
#define GET_LOGGING_LAYER(layer) Util_GetLogLayerState(layer)
///Disable Level during runtime
#define DISABLE_LOGGING_LEVEL(level) Util_SetLogLevelState(level,eDisable)
///Enable Level during runtime
#define ENABLE_LOGGING_LEVEL(level) Util_SetLogLevelState(level,eEnable)
///Get Level State
#define GET_LOGGING_LEVEL(level) Util_GetLogLevelState(level)
#else
/// @cond hidden
#define DISABLE_LOGGING_LAYER(layer)
#define ENABLE_LOGGING_LAYER(layer)
#define GET_LOGGING_LAYER(layer)
#define DISABLE_LOGGING_LEVEL(level)
#define ENABLE_LOGGING_LEVEL(level)
#define GET_LOGGING_LEVEL(level)
/// @endcond
#endif

/// @cond hidden
#ifdef _OCPDEBUG_WIN32
    #define DEBUG_PRINT(msg) printf("%s",msg);
#else
    #define DEBUG_PRINT(msg)
#endif
/// @endcond


///Define ENABLE_LOG and ENABLE_HANDSHAKETYPE to enable Handshake layer logging.
///ENABLE_LOG enable logs at top level and logging will not wok if not defined.
#if !defined ENABLE_HANDSHAKETYPE || !defined ENABLE_LOG
/// @cond hidden
#define LOG_HANDSHAKEMSG(msg,level) DEBUG_PRINT("HANDSHAKETYPE Undefined\n")
#define LOG_HANDSHAKEDBVAL(val,level) DEBUG_PRINT("HANDSHAKETYPE Undefined\n")
/// @endcond
#else
///Log Handshake Message
#define LOG_HANDSHAKEMSG(msg,level) \
    {   \
    if(eEnable==Util_GetLogLayerState(eHS)) \
        Util_LogMessage(msg,eHS,level); \
    else{DEBUG_PRINT("Handshake Layer Disabled\n");} \
    }

///Logs Handshake layer 4 byte debug value
#define LOG_HANDSHAKEDBVAL(val,level) \
    {   \
    if(eEnable==Util_GetLogLayerState(eHS)) \
        Util_LogDebugVal(val,eHS,level); \
    else{DEBUG_PRINT("Handshake Layer Disabled\n");} \
    }
#endif

///Define ENABLE_LOG and ENABLE_RECORDLAYERTYPE to enable Record layer logging.
///ENABLE_LOG enable logs at top level and logging will not wok if not defined.
#if !defined ENABLE_RECORDLAYERTYPE || !defined ENABLE_LOG
/// @cond hidden
#define LOG_RECORDLAYERMSG(msg,level) DEBUG_PRINT("RECORDLAYERTYPE Undefined\n")
#define LOG_RECORDLAYERDBVAL(val,level) DEBUG_PRINT("RECORDLAYERTYPE Undefined\n")
/// @endcond
#else
///Log Record Header Message
#define LOG_RECORDLAYERMSG(msg,level) \
    {   \
    if(eEnable==Util_GetLogLayerState(eRL)) \
        Util_LogMessage(msg,eRL,level); \
    else{DEBUG_PRINT("Record Layer Disabled\n");} \
    }

///Logs record layer 4 byte debug value
#define LOG_RECORDLAYERDBVAL(val,level) \
    {   \
    if(eEnable==Util_GetLogLayerState(eRL)) \
        Util_LogDebugVal(val,eRL,level); \
    else{DEBUG_PRINT("Record Layer Disabled\n");} \
    }
#endif

///Define ENABLE_LOG and ENABLE_TRANSPORTTYPE to enable Transport Layer logging.
///ENABLE_LOG enable logs at top level and logging will not wok if not defined.
#if !defined ENABLE_TRANSPORTTYPE || !defined ENABLE_LOG
/// @cond hidden
#define LOG_TRANSPORTMSG(msg,level) DEBUG_PRINT("TRANSPORTTYPE Undefined\n")
#define LOG_TRANSPORTDBVAL(val,level) DEBUG_PRINT("TRANSPORTTYPE Undefined\n")
#define LOG_TRANSPORTDBARY(Msg, buffer, wLen, level) DEBUG_PRINT("TRANSPORTTYPE Undefined\n")
/// @endcond
#else
///Log Transport layer Message
#define LOG_TRANSPORTMSG(msg,level) \
    {   \
    if(eEnable==Util_GetLogLayerState(eTL)) \
        Util_LogMessage(msg,eTL,level); \
    else{DEBUG_PRINT("Transport Layer Disabled\n");} \
    }

///Logs Transport layer 4 byte debug value
#define LOG_TRANSPORTDBVAL(val,level) \
    {   \
    if(eEnable==Util_GetLogLayerState(eTL)) \
        Util_LogDebugVal(val,eTL,level); \
    else{DEBUG_PRINT("Transport Layer Disabled\n");} \
    }

///Logs Transport layer Array value
#define LOG_TRANSPORTDBARY(Msg, buffer, wLen, level) \
    {   \
    if(eEnable==Util_GetLogLayerState(eTL)) \
        Util_LogMsgArray(Msg, buffer, wLen, eTL, level); \
    else{DEBUG_PRINT("Transport Layer Disabled\n");} \
    }
#endif

#ifdef ENABLE_LOG
///Enable the logger handle
#define LOG_SETWRITER(writer,hdl)    Util_SetLogWriter((pFWriteData2)writer,hdl);
#else
/// @cond hidden
#define LOG_SETWRITER(writer,hdl)    DEBUG_PRINT("Logger is Disabled\n")
/// @endcond
#endif

#endif //__LOGGER_H__
    /**
* @}
*/
