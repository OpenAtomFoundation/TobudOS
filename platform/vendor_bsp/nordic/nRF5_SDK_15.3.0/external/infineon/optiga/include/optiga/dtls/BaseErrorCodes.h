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
* \brief   This file defines the error code layering.
*
* \addtogroup  grMutualAuth
* @{
*/

#ifndef _BASEERRORCODE_H_
#define _BASEERRORCODE_H_
/*
The following defines the Error code layering for a 4 byte error code.
Each byte is represented as XX, where X being a single nibble.
    |8X|XX|XX|XX|
      |  |  |  |---->Error Codes
      |  |  |------->Layer
      |  |---------->Unused
      |------------->80
*/

 /// Base Error Code
#define ERROR_CODE_BASE                     0x80000000

///Base Error code for Comms
#define BASE_ERROR_COMMS                    (ERROR_CODE_BASE | 0x00000000)

///Base Error code for Command Lib
#define BASE_ERROR_CMDLIB                   (ERROR_CODE_BASE | 0x00000100)

///Base Error code for Integration Lib
#define BASE_ERROR_INTLIB                   (ERROR_CODE_BASE | 0x00000200)

///Base Error code for Crypto Lib
#define BASE_ERROR_CRYPTOLIB                (ERROR_CODE_BASE | 0x00000300)

///Base Error code for Alert
#define BASE_ERROR_ALERT                    (ERROR_CODE_BASE | 0x00000500)

///Base Error code for Handshake Layer
#define BASE_ERROR_HANDSHAKELAYER           (ERROR_CODE_BASE | 0x00000600)

///Base Error code for Transport Layer
#define BASE_ERROR_TRANSPORTLAYER           (ERROR_CODE_BASE | 0x00000700)

///Base Error code for OCP
#define BASE_ERROR_OCPLAYER                 (ERROR_CODE_BASE | 0x00000800)

///Base Error code for Record Layer
#define BASE_ERROR_RECORDLAYER              (ERROR_CODE_BASE | 0x00000900)

///Base Error code for Windowing
#define BASE_ERROR_WINDOW                   (ERROR_CODE_BASE | 0x00000A00)

///Base Error code for Message Layer
#define BASE_ERROR_MESSAGELAYER             (ERROR_CODE_BASE | 0x00000B00)

///Base Error code for Flight Handler
#define BASE_ERROR_FLIGHT                   (ERROR_CODE_BASE | 0x00000C00)

///Base Error code for Crypto Layer
#define BASE_ERROR_CRYPTO_LAYER             (ERROR_CODE_BASE | 0x00000D00)

///Base Error code UDP
#define BASE_ERROR_UDP                      (ERROR_CODE_BASE | 0x00000E00)

#endif //_BASEERRORCODE_H_
/**
* @}
*/
