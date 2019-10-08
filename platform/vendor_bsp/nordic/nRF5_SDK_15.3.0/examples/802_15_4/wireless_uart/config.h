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
#ifndef CONFIG_H__
#define CONFIG_H__

#include "sdk_config.h"
#include "802_15_4_config.h"

#ifndef CONFIG_SECURE
#define CONFIG_SECURE 0
#endif  // CONFIG_SECURE
#define CONFIG_PAN_ID               0x1234
#define CONFIG_INIT_DONE_PIN        LED_1
#define CONFIG_UPSTREAM_PIN         LED_2
#define CONFIG_DOWNSTREAM_PIN       LED_3
#define CONFIG_ERROR_PIN            LED_4
#define CONFIG_UART_MODULE          0
#define CONFIG_UART_TX_PIN          6
#define CONFIG_UART_RX_PIN          8
#define CONFIG_UART_CTS_PIN         7
#define CONFIG_UART_RTS_PIN         5
#define CONFIG_UART_TX_BUFFER_SIZE  256
#define CONFIG_UART_RX_BUFFER_SIZE  128
#define CONFIG_UART_BAUDRATE        HAL_UART_BAUDRATE_38400
#define CONFIG_UART_PARITY          HAL_UART_PARITY_NONE
#define CONFIG_DATA_SECURITY_LEVEL  5
#define CONFIG_SECURITY_KEY         {0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCA, 0xCD, 0xCE, 0xCF}

#if defined ( __ICCARM__ )
    #undef  __ALIGN
    #define __ALIGN(n)
#endif

#endif // CONFIG_H__

