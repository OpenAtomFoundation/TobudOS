/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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
#include "sdk_common.h"
/** @file
 *
 * @defgroup retarget Retarget layer for printf to log
 * @{
 * @ingroup iot_socket_tcp_client
 * @} */

#include <stdio.h>
#include <stdint.h>
#include "nrf_log.h"


#if defined(__CC_ARM)

// This part is taken from MDK-ARM template file and is required here to prevent
// linker from selecting libraries functions that use semihosting and failing
// because of multiple definitions of fgetc() and fputc().
// Refer to: http://www.keil.com/support/man/docs/gsac/gsac_retargetcortex.htm
// -- BEGIN --
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
// --- END ---

int fputc(int ch, FILE * p_file)
{
    UNUSED_PARAMETER(p_file);

    NRF_LOG_RAW_INFO("%c", (uint8_t)ch);

    return ch;
}

#elif defined(__GNUC__) && defined(__SES_ARM)

#if defined(__SES_VERSION) && (__SES_VERSION >= 34000)
int __putchar(int ch, __printf_tag_ptr tag_ptr)
{
    UNUSED_PARAMETER(tag_ptr);

    NRF_LOG_RAW_INFO("%c", (uint8_t)ch);
    return ch;
}
#else
int __putchar(int ch, FILE * p_file)
{
    UNUSED_PARAMETER(p_file);

    NRF_LOG_RAW_INFO("%c", (uint8_t)ch);

    return ch;
}
#endif 

#elif defined(__GNUC__) && !defined(__SES_ARM)

int _write(int file, const char * p_char, int len)
{
    int i;

    UNUSED_PARAMETER(file);

    for (i = 0; i < len; i++)
    {
        NRF_LOG_RAW_INFO("%c", *p_char++);
    }

    return len;
}

#endif