/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "addr_parse.h"
#include "ipv6_parse.h"
#include "nrf_error.h"


static bool use_dtls_check(char * p_uri)
{
    if (p_uri[4] == 's')
    {
        return true;
    }

    return false;
}


uint32_t addr_parse_uri(uint8_t *  p_addr,
                        uint16_t * p_port,
                        bool *     p_use_dtls,
                        char *     p_uri,
                        uint8_t    uri_len)
{

    uint16_t index = 0;

    if (uri_len < 4)
    {
        return NRF_ERROR_DATA_SIZE;
    }

    // Set default port for coap.
    *p_port = 5683;

    bool use_dtls = use_dtls_check(p_uri);
    *p_use_dtls = use_dtls;

    // Check if it is secure coap.
    if (use_dtls == true)
    {
        // Skip coaps://[. 9 bytes in length.
        index = 9;

        // Set default port for coaps.
        *p_port = 5684;
    }
    else
    {
        // Skip coap://[. 8 bytes in length.
        index = 8;
    }

    if (index >= uri_len)
    {
        return NRF_ERROR_INVALID_DATA;
    }

    uint8_t addr_len = 0;

    addr_len = strcspn(p_uri, "]") - index;

    // If we could not find any end bracket ']' or we read outside the length of the provided p_uri buffer.
    if ((addr_len == 0) || (addr_len + index > uri_len))
    {
        return NRF_ERROR_INVALID_DATA;
    }

    // Parse the IPv6 and put the result into result p_addr.
    uint32_t err_code = ipv6_parse_addr(p_addr, &p_uri[index], addr_len);
    if (err_code != NRF_SUCCESS)
    {
        return NRF_ERROR_INVALID_DATA;
    }

    // Increment the size of the IPv6 hostname.
    index += addr_len;

    // Increment the index to include the end bracket token ']'.
    index++;

    // Parse port
    if (index < uri_len)
    {
        // Check if uri has port number defined. If so, retrieve it.
        char port_buf[6] = {'\0',};
        char * port_delimiter = strchr(&p_uri[index], ':');
        if (port_delimiter != NULL)
        {
            index++; // increase because of the recognized port delimiter (':').
            uint8_t port_index = 0;
            while ((index < uri_len) && (p_uri[index] != '/'))
            {
                port_buf[port_index] = p_uri[index];
                port_index++;
                index++;
            }
            *p_port = atoi(port_buf);
        }
    }

    return NRF_SUCCESS;
}
