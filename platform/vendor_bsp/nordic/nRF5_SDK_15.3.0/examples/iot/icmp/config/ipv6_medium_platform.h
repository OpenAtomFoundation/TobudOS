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
/** @cond To make doxygen skip this file */

/** @file
 *  This header needs to include the desired IPv6 medium implementation(s).
 *  Based on the chosen implementation, it also definitions that are specific to
 *  the IPv6 medium implementation(s) and the application use case.
 * @{
 */

#ifndef IPV6_MEDIUM_PLATFORM_H__
#define IPV6_MEDIUM_PLATFORM_H__

#include "ipv6_medium_ble.h"
#ifdef COMMISSIONING_ENABLED
#include "commissioning.h"
#endif // COMMISSIONING_ENABLED

typedef union
{
    ipv6_medium_ble_cb_params_t ble;
#ifdef COMMISSIONING_ENABLED
    commissioning_evt_t         commissioning;
#endif // COMMISSIONING_ENABLED
} ipv6_medium_cb_params_union_t;

typedef union
{
    ipv6_medium_ble_error_params_t ble;
#ifdef COMMISSIONING_ENABLED
    commissioning_evt_t            commissioning;
#endif // COMMISSIONING_ENABLED
} ipv6_medium_err_params_union_t;

#endif // IPV6_MEDIUM_PLATFORM_H__

/** @} */
/** @endcond */
