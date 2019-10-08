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
* \brief    This file implements the prototype declarations of pal socket functionalities
*
* \addtogroup  grPAL
* @{
*/


#ifndef _PAL_SOCKET_H_
#define _PAL_SOCKET_H_

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

#ifndef WIN32
    #include "optiga/common/Datatypes.h"
    #include "udp.h"
    #include "inet.h"
#else
    #include <winsock2.h>
    #include "optiga/common/Datatypes.h"
#endif

#include "optiga/common/ErrorCodes.h"
#include "optiga/dtls/UDPErrorCodes.h"

/// @cond hidden
/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/
#ifndef WIN32
    #define IPAddressParse(pzIpAddress, psIPAddress)      (inet_aton(pzIpAddress, psIPAddress))
#else
    #define IPAddressParse(pzIpAddress, psIPAddress)      (1)
#endif
/// @endcond
/**********************************************************************************************************************
 * ENUMS
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * DATA STRUCTURES
 *********************************************************************************************************************/

#ifndef WIN32
/**
 * \brief Pointer type definition of pal socket receive event callback
 */
typedef void (*pal_socket_event_listener)(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                                          ip_addr_t *addr, u16_t port);
#endif



/**
 * \brief This structure contains socket communication data
 */
typedef enum eRecvMode_d
{
    eBlock = 0x10,

    eNonBlock = 0x20
}eRecvMode_d;

/**
 * \brief This structure contains socket communication data
 */
#ifndef WIN32

typedef struct pal_socket
{

    ///UDP structure Tx
    struct udp_pcb *pcbTx;

    ///UDP structure Rx
    struct udp_pcb *pcbRx;

    //Received IP address
    ip_addr_t sIPAddress;

    ///Function pointer to hold receive callback
    pal_socket_event_listener pfListen;

    ///Port for UDP communication
    uint16_t wPort;

    ///Transport Layer Timeout
    uint16_t wTimeout;

    ///Enumeration to indicate Blocking or Non blocking
    uint8_t bMode;

} pal_socket_t;

#else

typedef struct pal_socket
{
    ///Received IP address
    char* sIPAddress;

    ///Port for UDP communication
    uint16_t wPort;

    ///Pointer to the socket for Receiving
    SOCKET SocketHdl;

    ///IPv4 Socket address for Receiving
    SOCKADDR_IN sSocketAddrIn;

    ///Transport Layer Timeout
    uint16_t wTimeout;

    ///Enumeration to indicate Blocking or Non blocking
    uint8_t bMode;

} pal_socket_t;
#endif

/**********************************************************************************************************************
 * API Prototypes
 *********************************************************************************************************************/

/**
 * \brief Assign IP address
 */
#ifndef WIN32
int32_t pal_socket_assign_ip_address(const char* p_ip_address,void *p_input_ip_address);
#else
int32_t pal_socket_assign_ip_address(const char_t* p_ip_address,char** p_input_ip_address);
#endif

/**
 * \brief Initializes the socket communication structure
 */
int32_t pal_socket_init(pal_socket_t* p_socket);

/**
 * \brief Creates server port and bind
 */
int32_t pal_socket_open(pal_socket_t* p_socket,
                        uint16_t port);
/**
 * \brief Creates a client port and connect
 */
int32_t pal_socket_connect(pal_socket_t* p_socket,
                           uint16_t port);
/**
 * \brief Receive data from the client
 */
int32_t pal_socket_listen(pal_socket_t* p_socket, uint8_t *p_data,
                          uint32_t *p_length);
/**
 * \brief Sends the data to the the client
 */
int32_t pal_socket_send(const pal_socket_t* p_socket, uint8_t *p_data,
                        uint32_t length);
/**
 * \brief Closes the socket communication and release the udp port
 */
void pal_socket_close(pal_socket_t* p_socket);

#endif

#endif //_PAL_SOCKET_H_

/**
* @}
*/

