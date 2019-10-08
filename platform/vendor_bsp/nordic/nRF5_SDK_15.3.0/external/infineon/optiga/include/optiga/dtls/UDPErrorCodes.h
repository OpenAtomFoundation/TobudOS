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
* \brief This file contains a UDP error codes.
*
*
* \addtogroup  grMutualAuth
* @{
*
*/

#ifndef _UDPERRCODES_H_
#define _UDPERRCODES_H_

///Failure to allocate memory
#define E_COMMS_UDP_ALLOCATE_FAILURE        0x80000100

///Binding failure to a port
#define E_COMMS_UDP_BINDING_FAILURE         0x80000101

///No data present to send
#define E_COMMS_UDP_NO_DATA_TO_SEND         0x80000102

///No data received from the target
#define E_COMMS_UDP_NO_DATA_RECEIVED        0x80000103

///Failure to route the UDP packet
#define E_COMMS_UDP_ROUTING_FAILURE         0x80000104

///Failure to deallocate the memory
#define E_COMMS_UDP_DEALLOCATION_FAILURE    0x80000105

///Copy to the buffer failure
#define E_COMMS_UDP_COPY_BUFFER_FAILURE     0x80000106

///UDP connect failure
#define E_COMMS_UDP_CONNECT_FAILURE         0x80000107

#endif /* _UDPERRCODES_H_ */

/**
* @}
*/