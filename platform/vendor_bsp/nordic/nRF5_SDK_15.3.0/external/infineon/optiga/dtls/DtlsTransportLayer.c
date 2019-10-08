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
 * \file DtlsTransportLayer.c
 *
 * \brief This file provides APIs for the transport layer functionalities.
 *
 * \addtogroup grOCP
 * @{
 *
 */

#include "optiga/dtls/DtlsTransportLayer.h"
#include "optiga/common/MemoryMgmt.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

/// @cond hidden

/// @endcond
/**
 * This API initialises transport layer communication structure.
 *
 * \param[in,out]  PpsTL Pointer to the transport layer communication structure
 *
 * \return  #OCP_TL_OK on successful execution
 * \return  #OCP_TL_ERROR on failure
 * \return  #OCP_TL_NULL_PARAM on parameter received is NULL
 * \return  #E_COMMS_UDP_ALLOCATE_FAILURE on failure to allocate memory
 */
int32_t DtlsTL_Init(sTL_d* PpsTL)
{
    int32_t i4Status = (int32_t)OCP_TL_ERROR;
    do
    {
        //NULL check
        if((NULL == PpsTL) || (NULL == PpsTL->pzIpAddress))
        {
            i4Status = (int32_t)OCP_TL_NULL_PARAM;
            break;
        }

        //Allocate the memory for the ethernet communication structure
        PpsTL->phTLHdl = (pal_socket_t*)OCP_MALLOC(sizeof(pal_socket_t));
        if(NULL == PpsTL->phTLHdl)
        {
            i4Status = (int32_t)OCP_TL_MALLOC_FAILURE;
            break;
        }
/// @cond hidden
#define PS_COMMS_HANDLE ((pal_socket_t*)PpsTL->phTLHdl)
/// @endcond

        PS_COMMS_HANDLE->wPort = PpsTL->wPort;

        //Converting IP address from string format to hex format
        i4Status = pal_socket_assign_ip_address(PpsTL->pzIpAddress,&(PS_COMMS_HANDLE->sIPAddress));
        if(i4Status != E_COMMS_SUCCESS)
        {
            break;
        }

        //Assigning the timeout value
        PS_COMMS_HANDLE->wTimeout = PpsTL->wTimeout ;

        //Non Blockage receive mode
        PS_COMMS_HANDLE->bMode = (uint8_t)PpsTL->eCallType;

        //Add logging
        LOG_TRANSPORTMSG("Initializing UDP Connection",eInfo);

        //Initialize the communication handle with the parameters
        i4Status = pal_socket_init(PS_COMMS_HANDLE);
        if(E_COMMS_SUCCESS != i4Status)
        {
            break;
        }
        i4Status = (int32_t)OCP_TL_OK;
    }while(FALSE);
    if(OCP_TL_OK != i4Status)
    {
        if((NULL != PpsTL)&& (NULL != PpsTL->phTLHdl))
        {
            OCP_FREE(PpsTL->phTLHdl);
            PpsTL->phTLHdl = NULL;
        }
    }
/// @cond hidden
#undef PS_COMMS_HANDLE
/// @endcond
    return i4Status;
}

/**
 * This API creates client port
 *
 * \param[in,out]  PpsTL     Pointer to the transport layer communication structure
 *
 * \return  #OCP_TL_OK on successful execution
 * \return  #OCP_TL_NULL_PARAM on parameter received is NULL
 * \return  #E_COMMS_UDP_BINDING_FAILURE on port binding failure
 * \return  #OCP_TL_ERROR on failure
 */
int32_t DtlsTL_Connect(sTL_d* PpsTL)
{
    int32_t i4Status = (int32_t)OCP_TL_ERROR;
    do
    {
        //NULL check
        if((NULL == PpsTL) || (NULL == PpsTL->phTLHdl))
        {
            i4Status = (int32_t)OCP_TL_NULL_PARAM;
            break;
        }
/// @cond hidden
#define PS_COMMS_HANDLE ((pal_socket_t*)PpsTL->phTLHdl)
/// @endcond
        //Logging
        LOG_TRANSPORTMSG("Connecting to UDP",eInfo);

        //Open the client port with the port number initialised
        i4Status = pal_socket_connect(PS_COMMS_HANDLE, PS_COMMS_HANDLE->wPort);
        if(E_COMMS_SUCCESS != i4Status)
        {
            LOG_TRANSPORTMSG("Error connecting to UDP",eError);
            break;
        }

        PpsTL->eIsConnected = eConnected;
        i4Status = (int32_t)OCP_TL_OK;
    }while(FALSE);
/// @cond hidden
#undef PS_COMMS_HANDLE
/// @endcond
    return i4Status;
}

/**
 * This API transmits the data to the server.
 *
 * \param[in,out]  PpsTL               Pointer to the transport layer communication structure
 * \param[in]      PpbBuffer           Pointer to buffer containing data to be transmitted
 * \param[in]      PdwLen              Length of the data to be transmitted
 *
 * \return  #OCP_TL_OK on successful execution
 * \return  #OCP_TL_NULL_PARAM on parameter received is NULL
 * \return  #E_COMMS_UDP_NO_DATA_TO_SEND on no date present to send
 * \return  #E_COMMS_INSUFFICIENT_MEMORY on out of memory failure
 * \return  #E_COMMS_UDP_ROUTING_FAILURE on failure to route the UDP packet
 * \return  #E_COMMS_UDP_DEALLOCATION_FAILURE on failure to deallocate
 * \return  #OCP_TL_ERROR on failure
 */
int32_t DtlsTL_Send(const sTL_d* PpsTL,uint8_t* PpbBuffer,uint16_t PdwLen)
{
    int32_t i4Status = (int32_t)OCP_TL_ERROR;

    do
    {
        //NULL check
        if((NULL == PpsTL) || (NULL == PpsTL->phTLHdl) ||(NULL == PpbBuffer))
        {
            i4Status = (int32_t)OCP_TL_NULL_PARAM;
            break;
        }

        LOG_TRANSPORTDBARY("Sending Data over UDP", PpbBuffer, PdwLen, eInfo);

        //Send the data over IP address and Port initialized
/// @cond hidden
#define PS_COMMS_HANDLE ((pal_socket_t*)PpsTL->phTLHdl)
/// @endcond
        i4Status = pal_socket_send(PS_COMMS_HANDLE, PpbBuffer, PdwLen);
        if (E_COMMS_SUCCESS != i4Status)
        {
            LOG_TRANSPORTMSG("Error while sending data",eError);
            break;
        }
        i4Status = (int32_t)OCP_TL_OK;
    }while(FALSE);
/// @cond hidden
#undef PS_COMMS_HANDLE
/// @endcond
    return i4Status;
}

/**
 * This API receives the data from the server
 *
 * \param[in]       PpsTL               Pointer to the transport layer communication structure
 * \param[in,out]   PpbBuffer           Pointer to buffer where data is to be received
 * \param[in,out]   PpdwLen             Length of the buffer/Length of the received data
 *
 * \return  #OCP_TL_OK on successful execution
 * \return  #OCP_TL_NULL_PARAM on parameter received is NULL
 * \return  #OCP_TL_NO_DATA on no data received from the target
 * \return  #E_COMMS_INSUFFICIENT_BUF_SIZE on insufficient buffer size
 * \return  #OCP_TL_ERROR on failure
 */
int32_t DtlsTL_Recv(const sTL_d* PpsTL,uint8_t* PpbBuffer,uint16_t* PpdwLen)
{
    int32_t i4Status = (int32_t)OCP_TL_ERROR;
    uint32_t dwRecvLen;

    do
    {
        //NULL check
        if((NULL == PpsTL) || (NULL == PpsTL->phTLHdl) || (NULL == PpbBuffer))
        {
            i4Status = (int32_t)OCP_TL_NULL_PARAM;
            break;
        }
        //logging
        LOG_TRANSPORTMSG("Receiving over UDP",eInfo);
/// @cond hidden
#define PS_COMMS_HANDLE ((pal_socket_t*)PpsTL->phTLHdl)
/// @endcond

        PS_COMMS_HANDLE->wTimeout = PpsTL->wTimeout;

        dwRecvLen = *PpdwLen;

        //Listen the server port and receive the data
        i4Status = pal_socket_listen(PS_COMMS_HANDLE, PpbBuffer, &dwRecvLen);
        if ((int32_t)E_COMMS_UDP_NO_DATA_RECEIVED == i4Status)
        {
            i4Status = (int32_t)OCP_TL_NO_DATA;
            LOG_TRANSPORTMSG("No data received over UDP",eError);
            break;
        }

        if (E_COMMS_SUCCESS != i4Status)
        {
            LOG_TRANSPORTMSG("Error while receiving data over UDP",eError);
            break;
        }

        LOG_TRANSPORTMSG("Received Data",eInfo);
        LOG_TRANSPORTDBARY("Received Data over UDP", PpbBuffer, dwRecvLen, eInfo);

        *PpdwLen = (uint16_t)dwRecvLen;

        i4Status = (int32_t)OCP_TL_OK;
    }while(FALSE);
/// @cond hidden
#undef PS_COMMS_HANDLE
/// @endcond
    return i4Status;
}

/**
 * This API closes the UDP communication and releases all the resources
 *
 * \param[in,out]  PpsTL     Pointer to the transport layer communication structure
 *
 * \return  None
 */
Void DtlsTL_Disconnect(sTL_d* PpsTL)
{

    //NULL check
    if(NULL != PpsTL)
    {
        if(NULL != PpsTL->phTLHdl)
        {
            //logging
            LOG_TRANSPORTMSG("Closing UDP Connection",eInfo);
/// @cond hidden
#define PS_COMMS_HANDLE ((pal_socket_t*)PpsTL->phTLHdl)
/// @endcond

            //Close the UDP connection
            pal_socket_close(PS_COMMS_HANDLE);

            //Free the allocated memory for ethernet structure
            OCP_FREE(PS_COMMS_HANDLE);

            PpsTL->phTLHdl = NULL;

            PpsTL->eIsConnected = eDisconnected;
/// @cond hidden
#undef PS_COMMS_HANDLE
/// @endcond
        }
    }
}

/**
* @}
*/
#endif /*MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
