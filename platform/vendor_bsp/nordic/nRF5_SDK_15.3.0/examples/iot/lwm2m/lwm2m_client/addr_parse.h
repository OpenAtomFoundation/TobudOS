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
#ifndef ADDR_PARSE_H__
#define ADDR_PARSE_H__

#include <stdint.h>
#include <stdbool.h>

/**@brief Parse a coap(s) uri to extract an IPv6 hostname and port if available.
 *
 * @param[out] p_addr Array of 16 bytes to hold the parsed IPv6 address.
 * @param[out] p_port Value by reference to be filled with the parsed port number. If no port
 *                    is defined in the uri the field will be filled by CoAP default ports
 *                    depending on wheter coap or coaps scheme was used.
 * @param[out] p_use_dtls Value by reference to indicate if it was coap or coaps scheme used.
 * @param[in]  p_uri  Pointer to the buffer containing the URI ascii characters.
 * @param[in]  uri_len Length of the p_uri buffer pointed to.
 *
 * @retval NRF_SUCCESS       If parsing of the uri was successfull.
 * @retval NRF_ERROR_NULL    If any of the supplied parameters where NULL.
 * @retval NRF_ERROR_INVALID_DATA If the function could not parse the content of p_uri.
 */
uint32_t addr_parse_uri(uint8_t *  p_addr,
                        uint16_t * p_port,
                        bool *     p_use_dtls,
                        char *     p_uri,
                        uint8_t    uri_len);

#endif // ADDR_PARSE_H__
