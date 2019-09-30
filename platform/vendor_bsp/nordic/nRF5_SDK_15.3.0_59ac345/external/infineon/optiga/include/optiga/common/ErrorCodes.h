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
* \brief This file defines the error codes.
*
* \addtogroup grError
* @{
*/

#ifndef _ERRORCODES_H_
#define _ERRORCODES_H_

/******************************************************************************
* required includes
* Setup common include order for the used types and definitions
******************************************************************************/

/******************************************************************************
* ErrorCodes.h
******************************************************************************/

/******************************************************************************
* defines
******************************************************************************/

/// Communication success
#define E_COMMS_SUCCESS                     0x00000000

/// Error code if protocol exit timeout has occurred
#define E_COMMS_TIMEOUT_OCCURRED            0x80000003

/// Error code if soft reset is not supported by slave
#define E_COMMS_SOFT_RESET_NOT_SUPPORTED    0x80000005

/// Error code if network layer channel does not mismatch with that of the slave
#define E_COMMS_CHANNEL_NUM_MISMATCH        0x80000006

/// Error code if packet size could not be set
#define E_COMMS_PACKET_SIZE_NOT_SET	        0x80000007

/// Error code for invalid key
#define E_COMMS_INVALID_KEY                 0x80000008

/// Error code if the input parameter is NULL
#define E_COMMS_PARAMETER_NULL              0x8000000A

/// Error code for uninitialized protocol
#define E_COMMS_PROTOCOL_NOT_INITIALIZED    0x8000000B

/// Error code for wrong reset value
#define E_INVALID_RESET_VALUE               0x8000000C

///Error code for invalid value for a given key
#define E_INVALID_VALUE_FOR_KEY             0x8000000D

/// Error code for if the slave doesn't have any data ready
#define E_COMMS_SLAVE_NODATA                0x8000000E

/// Error code if no channel information is present
#define E_COMMS_NO_CHANNEL_INFORMATION      0x8000000F

/// Error code if APDU is not present
#define E_COMMS_NO_APDU_PRESENT             0x80000010

/// Error code for cold reset not supported
#define  E_COMMS_COLD_RESET_NOT_SUPPORTED   0x80000011

/// Error code for partial frame send
#define  E_COMMS_PARTIAL_FRAME_SENT         0x80000012

/// Error code for low level I2C write failed
#define  E_COMMS_I2C_WRITE_FAILED           0x80000013

/// Error code for low level I2C read failed
#define  E_COMMS_I2C_READ_FAILED            0x80000014

/// Error code for buffer size or memory insufficient
#define  E_COMMS_INSUFFICIENT_BUF_SIZE      0x80000015

/// Error code when the value of key is not set
#define  E_COMMS_KEY_VALUE_NOT_SET          0x80000016

/// Error code for setting a property within a session not allowed
#define  E_COMMS_SET_PROPERTY_DENIED        0x80000017

/// Error code for opening the already opened protocol stack
#define  E_COMMS_ALREADY_OPEN               0x80000018

/// Error code for frame buffer overflow(at data link layer)
#define  E_COMMS_FRAME_BUFFER_OVERFLOW      0x80000019

/// Error code for unsupported functionality
#define  E_COMMS_FUNCTION_NOT_SUPPORTED     0x8000001A

/// Error code for invalid channel number
#define  E_COMMS_INVALID_CHANNEL_NO         0x8000001B

/// Error code for insufficient memory
#define E_COMMS_INSUFFICIENT_MEMORY         0x8000001C

/// Error code if Nack is received
#define E_COMMS_I2C_NACK_RECEIVED           0x8000001D

/// Error code for I2C Tx/Rx Error
#define E_COMMS_I2C_TXRX_ERROR             	0x8000001E

/// Error code for I2C Tx/Rx Error
#define E_COMMS_I2C_TIMEOUT                 0x8000001F

/// Error code for I2C Tx/Rx Error
#define E_COMMS_INVALID_REQUEST             0x80000020

/// Error code for communication failure
#define E_COMMS_FAILURE                     0xFFFFFFFF

/// OPTIGA Host Library API status - Success
#define OPTIGA_LIB_SUCCESS                  0x0000

/// OPTIGA Host Library API status - Busy
#define OPTIGA_LIB_STATUS_BUSY              0x0001

/// OPTIGA Host Library API status - Failure
#define OPTIGA_LIB_ERROR                    0xFFFF

#endif /* _ERRORCODES_H_ */

/**
* @}
*/

