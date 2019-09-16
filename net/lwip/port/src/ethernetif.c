/*
* Copyright (c) 2001-2004 Swedish Institute of Computer Science.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* This file is part of the lwIP TCP/IP stack.
*
* Author: Adam Dunkels <adam@sics.se>
*
*/

/*
* This file is a skeleton for developing Ethernet network interface
* drivers for lwIP. Add code to the low_level functions and do a
* search-and-replace for the word "ethernetif" to replace it with
* something that better describes your network interface.
*/

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "string.h"

/* Define those to better describe your network interface. */
#define IFNAME0     's'
#define IFNAME1     't'

static struct netif *the_netif = NULL;

static ethernetif_drv_t ethernetif_drv = { NULL };

int ethernetif_drv_register(ethernetif_drv_t *drv)
{
    if (!drv) {
        return ERR_ARG;
    }

    ethernetif_drv.init     = drv->init;
    ethernetif_drv.write    = drv->write;
    ethernetif_drv.read     = drv->read;

    return ERR_OK;
}

/**
* In this function, the hardware should be initialized.
* Called from ethernetif_init().
*
* @param netif the already initialized lwip network interface structure
*        for this ethernetif
*/
static void ethernetif_drv_init(struct netif *netif)
{
    if (ethernetif_drv.init) {
        (void)ethernetif_drv.init(netif);
        the_netif = netif;
    }
}

/**
* This function should do the actual transmission of the packet. The packet is
* contained in the pbuf that is passed to the function. This pbuf
* might be chained.
*
* @param netif the lwip network interface structure for this ethernetif
* @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
* @return ERR_OK if the packet could be sent
*         an err_t value if the packet couldn't be sent
*
* @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
*       strange results. You might consider waiting for space in the DMA queue
*       to become availale since the stack doesn't retry to send a packet
*       dropped because of memory failure (except for the TCP timers).
*/
static err_t ethernetif_drv_write(struct netif *netif, struct pbuf *p)
{
    err_t rc = ERR_IF;

    if (ethernetif_drv.write) {
        rc = ethernetif_drv.write(netif, p);
    }

    return rc;
}

/**
* Should allocate a pbuf and transfer the bytes of the incoming
* packet from the interface into the pbuf.
*
* @param netif the lwip network interface structure for this ethernetif
* @return a pbuf filled with the received packet (including MAC header)
*         NULL on memory error
*/
static struct pbuf *ethernetif_drv_read(struct netif *netif)
{
    struct pbuf *p = NULL;

    if (ethernetif_drv.read) {
        p = ethernetif_drv.read(netif);
    }
    return p;
}

#if !LWIP_ARP
/**
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
    err_t errval;
    errval = ERR_OK;

    return errval;
}
#endif /* LWIP_ARP */

/**
* This function is the ethernetif_input task, it is processed when a packet
* is ready to be read from the interface. It uses the function ethernetif_drv_read()
* that should handle the actual reception of bytes from the network
* interface. Then the type of the received packet is determined and
* the appropriate input function is called.
*
* @param netif the lwip network interface structure for this ethernetif
*/
void ethernetif_input(void *arg)
{
    err_t rc;
    struct pbuf *p;

    /* move received packet into a new pbuf */
    do {
        SYS_ARCH_DECL_PROTECT(sr);

        SYS_ARCH_PROTECT(sr);
        p = ethernetif_drv_read(the_netif);
        SYS_ARCH_UNPROTECT(sr);

        if (!p) {
            return;
        }

        rc = the_netif->input(p, the_netif);
        if (rc != ERR_OK) {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    } while (p);
}

/**
* Should be called at the beginning of the program to set up the
* network interface. It calls the function ethernetif_drv_init() to do the
* actual setup of the hardware.
*
* This function should be passed as a parameter to netif_add().
*
* @param netif the lwip network interface structure for this ethernetif
* @return ERR_OK if the loopif is initialized
*         ERR_MEM if private data couldn't be allocated
*         any other err_t on error
*/
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname     = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0]      = IFNAME0;
    netif->name[1]      = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
       * You can instead declare your own function an call etharp_output()
       * from it if you have to do some checks before sending (e.g. if link
       * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
    netif->output       = etharp_output;
#else
    /* The user should write ist own code in low_level_output_arp_off function */
    netif->output       = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    netif->output_ip6   = ethip6_output;
#endif /* LWIP_IPV6 */

    netif->linkoutput   = ethernetif_drv_write;

    /* initialize the hardware */
    ethernetif_drv_init(netif);

    return ERR_OK;
}

