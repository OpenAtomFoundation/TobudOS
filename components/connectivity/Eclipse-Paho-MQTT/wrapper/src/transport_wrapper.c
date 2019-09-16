/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/

#include "sal_module_wrapper.h"
#include "transport_wrapper.h"

/**
  This simple low-level implementation assumes a single connection for a single thread. Thus, a static
  variable is used for that connection.
  On other scenarios, the user must solve this by taking into account that the current implementation of
  MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
  to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
static int msock = -1;

int transport_sendPacketBuffer(int sock, unsigned char *buf, int buflen)
{
    return tos_sal_module_send(sock, buf, buflen);
}

int transport_getdata(unsigned char *buf, int count)
{
    return tos_sal_module_recv(msock, buf, count);
}

int transport_getdatanb(void *sck, unsigned char *buf, int count)
{
    /* sck: pointer to whatever the system may use to identify the transport */
	int sock = *((int *)sck);

	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc = tos_sal_module_recv(sock, buf, count);
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
		return 0;
	}
	return rc;
}

/**
  return >=0 for a socket descriptor, <0 for an error code
  @todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
  removing indirections
*/
int transport_open(char *addr, const char *port)
{
	msock = tos_sal_module_connect(addr, port, TOS_SAL_PROTO_TCP);
    return msock;
}

int transport_close(int sock)
{
    return tos_sal_module_close(sock);
}

