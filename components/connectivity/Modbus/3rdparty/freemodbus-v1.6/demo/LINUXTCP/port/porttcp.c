/*
 * FreeModbus Libary: Win32 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/*
 * Design Notes:
 *
 * The xMBPortTCPInit function allocates a socket and binds the socket to
 * all available interfaces ( bind with INADDR_ANY ). In addition it
 * creates an array of event objects which is used to check the state of
 * the clients. On event object is used to handle new connections or
 * closed ones. The other objects are used on a per client basis for
 * processing.
 */

 /**********************************************************
 *	Linux TCP support.
 *	Based on Walter's project. 
 *	Modified by Steven Guo <gotop167@163.com>
 ***********************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"



/* ----------------------- MBAP Header --------------------------------------*/
#define MB_TCP_UID          6
#define MB_TCP_LEN          4
#define MB_TCP_FUNC         7

/* ----------------------- Defines  -----------------------------------------*/
#define MB_TCP_DEFAULT_PORT 502 /* TCP listening port. */
#define MB_TCP_POOL_TIMEOUT 50  /* pool timeout for event waiting. */
#define MB_TCP_READ_TIMEOUT 1000        /* Maximum timeout to wait for packets. */
#define MB_TCP_READ_CYCLE   100 /* Time between checking for new data. */

#define MB_TCP_DEBUG        1   /* Set to 1 for additional debug output. */

#define MB_TCP_BUF_SIZE     ( 256 + 7 ) /* Must hold a complete Modbus TCP frame. */

#define EV_CONNECTION       0
#define EV_CLIENT           1
#define EV_NEVENTS          EV_CLIENT + 1

/* ----------------------- Static variables ---------------------------------*/
SOCKET          xListenSocket;
SOCKET          xClientSocket = INVALID_SOCKET;
static fd_set   allset;

static UCHAR    aucTCPBuf[MB_TCP_BUF_SIZE];
static USHORT   usTCPBufPos;
static USHORT   usTCPFrameBytesLeft;

/* ----------------------- External functions -------------------------------*/
CHAR           *WsaError2String( int dwError );

/* ----------------------- Static functions ---------------------------------*/
BOOL            prvMBTCPPortAddressToString( SOCKET xSocket, CHAR * szAddr, USHORT usBufSize );
CHAR           *prvMBTCPPortFrameToString( UCHAR * pucFrame, USHORT usFrameLen );
static BOOL     prvbMBPortAcceptClient( void );
static void     prvvMBPortReleaseClient( void );


/* ----------------------- Begin implementation -----------------------------*/

BOOL
xMBTCPPortInit( USHORT usTCPPort )
{
    USHORT          usPort;
    struct sockaddr_in serveraddr;

    if( usTCPPort == 0 )
    {
        usPort = MB_TCP_DEFAULT_PORT;
    }
    else
    {
        usPort = ( USHORT ) usTCPPort;
    }
    memset( &serveraddr, 0, sizeof( serveraddr ) );
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl( INADDR_ANY );
    serveraddr.sin_port = htons( usPort );
    if( ( xListenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == -1 )
    {
        fprintf( stderr, "Create socket failed.\r\n" );
        return FALSE;
    }
    else if( bind( xListenSocket, ( struct sockaddr * )&serveraddr, sizeof( serveraddr ) ) == -1 )
    {
        fprintf( stderr, "Bind socket failed.\r\n" );
        return FALSE;
    }
    else if( listen( xListenSocket, 5 ) == -1 )
    {
        fprintf( stderr, "Listen socket failed.\r\n" );
        return FALSE;
    }
    FD_ZERO( &allset );
    FD_SET( xListenSocket, &allset );
    return TRUE;
}

void
vMBTCPPortClose(  )
{
    // Close all client sockets. 
    if( xClientSocket != SOCKET_ERROR )
    {
        prvvMBPortReleaseClient(  );
    }
    // Close the listener socket.
    if( xListenSocket != SOCKET_ERROR )
    {
        close( xListenSocket );
    }
}

void
vMBTCPPortDisable( void )
{
    /* Close all client sockets. */
    if( xClientSocket != SOCKET_ERROR )
    {
        prvvMBPortReleaseClient(  );
    }
}

/*! \ingroup port_win32tcp
 *
 * \brief Pool the listening socket and currently connected Modbus TCP clients
 *   for new events.
 * \internal
 *
 * This function checks if new clients want to connect or if already connected 
 * clients are sending requests. If a new client is connected and there are 
 * still client slots left (The current implementation supports only one)
 * then the connection is accepted and an event object for the new client
 * socket is activated (See prvbMBPortAcceptClient() ).
 * Events for already existing clients in \c FD_READ and \c FD_CLOSE. In case of
 * an \c FD_CLOSE the client connection is released (See prvvMBPortReleaseClient() ).
 * In case of an \c FD_READ command the existing data is read from the client
 * and if a complete frame has been received the Modbus Stack is notified.
 *
 * \return FALSE in case of an internal I/O error. For example if the internal
 *   event objects are in an invalid state. Note that this does not include any 
 *   client errors. In all other cases returns TRUE.
 */
BOOL
xMBPortTCPPool( void )
{
    int             n;
    fd_set          fread;
    struct timeval  tval;

    tval.tv_sec = 0;
    tval.tv_usec = 5000;
    int             ret;
    USHORT          usLength;

    if( xClientSocket == INVALID_SOCKET )
    {
        /* Accept to client */
        if( ( n = select( xListenSocket + 1, &allset, NULL, NULL, NULL ) ) < 0 )
        {
            if( errno == EINTR )
            {
                ;
            }
            else
            {
                ;
            }
        }
        if( FD_ISSET( xListenSocket, &allset ) )
        {
            ( void )prvbMBPortAcceptClient(  );
        }
    }
    while( TRUE )
    {
        FD_ZERO( &fread );
        FD_SET( xClientSocket, &fread );
        if( ( ( ret = select( xClientSocket + 1, &fread, NULL, NULL, &tval ) ) == SOCKET_ERROR )
            || !ret )
        {
            continue;
        }
        if( ret > 0 )
        {
            if( FD_ISSET( xClientSocket, &fread ) )
            {
                if( ( ( ret =
                        recv( xClientSocket, &aucTCPBuf[usTCPBufPos], usTCPFrameBytesLeft,
                              0 ) ) == SOCKET_ERROR ) || ( !ret ) )
                {
                    close( xClientSocket );
                    xClientSocket = INVALID_SOCKET;
                    return TRUE;
                }
                usTCPBufPos += ret;
                usTCPFrameBytesLeft -= ret;
                if( usTCPBufPos >= MB_TCP_FUNC )
                {
                    /* Length is a byte count of Modbus PDU (function code + data) and the
                     * unit identifier. */
                    usLength = aucTCPBuf[MB_TCP_LEN] << 8U;
                    usLength |= aucTCPBuf[MB_TCP_LEN + 1];

                    /* Is the frame already complete. */
                    if( usTCPBufPos < ( MB_TCP_UID + usLength ) )
                    {
                        usTCPFrameBytesLeft = usLength + MB_TCP_UID - usTCPBufPos;
                    }
                    /* The frame is complete. */
                    else if( usTCPBufPos == ( MB_TCP_UID + usLength ) )
                    {
                        ( void )xMBPortEventPost( EV_FRAME_RECEIVED );
                        return TRUE;
                    }
                    /* This can not happend because we always calculate the number of bytes
                     * to receive. */
                    else
                    {
                        assert( usTCPBufPos <= ( MB_TCP_UID + usLength ) );
                    }
                }
            }
        }
    }
    return TRUE;
}

/*!
 * \ingroup port_win32tcp
 * \brief Receives parts of a Modbus TCP frame and if complete notifies
 *    the protocol stack.
 * \internal 
 *
 * This function reads a complete Modbus TCP frame from the protocol stack.
 * It starts by reading the header with an initial request size for
 * usTCPFrameBytesLeft = MB_TCP_FUNC. If the header is complete the 
 * number of bytes left can be calculated from it (See Length in MBAP header).
 * Further read calls are issued until the frame is complete.
 *
 * \return \c TRUE if part of a Modbus TCP frame could be processed. In case
 *   of a communication error the function returns \c FALSE.
 */

BOOL
xMBTCPPortGetRequest( UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{
    *ppucMBTCPFrame = &aucTCPBuf[0];
    *usTCPLength = usTCPBufPos;

    /* Reset the buffer. */
    usTCPBufPos = 0;
    usTCPFrameBytesLeft = MB_TCP_FUNC;
    return TRUE;
}

BOOL
xMBTCPPortSendResponse( const UCHAR * pucMBTCPFrame, USHORT usTCPLength )
{
    BOOL            bFrameSent = FALSE;
    BOOL            bAbort = FALSE;
    int             res;
    int             iBytesSent = 0;
    int             iTimeOut = MB_TCP_READ_TIMEOUT;

    do
    {
        res = send( xClientSocket, &pucMBTCPFrame[iBytesSent], usTCPLength - iBytesSent, 0 );
        switch ( res )
        {
        case -1:
            if( iTimeOut > 0 )
            {
                iTimeOut -= MB_TCP_READ_CYCLE;
                usleep( MB_TCP_READ_CYCLE );
            }
            else
            {
                bAbort = TRUE;
            }
            break;
        case 0:
            prvvMBPortReleaseClient(  );
            bAbort = TRUE;
            break;
        default:
            iBytesSent += res;
            break;
        }
    }
    while( ( iBytesSent != usTCPLength ) && !bAbort );

    bFrameSent = iBytesSent == usTCPLength ? TRUE : FALSE;

    return bFrameSent;
}

void
prvvMBPortReleaseClient(  )
{
    ( void )recv( xClientSocket, &aucTCPBuf[0], MB_TCP_BUF_SIZE, 0 );

    ( void )close( xClientSocket );
    xClientSocket = INVALID_SOCKET;
}

BOOL
prvbMBPortAcceptClient(  )
{
    SOCKET          xNewSocket;
    BOOL            bOkay;

    /* Check if we can handle a new connection. */

    if( xClientSocket != INVALID_SOCKET )
    {
        fprintf( stderr, "can't accept new client. all connections in use.\n" );
        bOkay = FALSE;
    }
    else if( ( xNewSocket = accept( xListenSocket, NULL, NULL ) ) == INVALID_SOCKET )
    {
        bOkay = FALSE;
    }
    else
    {
        xClientSocket = xNewSocket;
        usTCPBufPos = 0;
        usTCPFrameBytesLeft = MB_TCP_FUNC;
        bOkay = TRUE;
    }
    return bOkay;
}
