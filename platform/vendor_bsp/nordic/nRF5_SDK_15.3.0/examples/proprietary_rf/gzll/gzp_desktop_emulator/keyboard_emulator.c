/**
 * Copyright (c) 2012 - 2019, Nordic Semiconductor ASA
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
/**
 * @file
 * @brief Implementation of keyboard_emulate.h .
 */

#include "keyboard_emulator.h"

/*
 * Function to get non empty keyboard packet.
 */
void keyboard_get_non_empty_packet(uint8_t * out_keyboard_packet)
{
    uint_least8_t i;

    out_keyboard_packet[NRFR_KEYBOARD_MOD] = 0;
    out_keyboard_packet[NRFR_KEYBOARD_KEYS] = 0x04; // Keyboard 'a'

    for (i = NRFR_KEYBOARD_KEYS + 1; i < NRFR_KEYBOARD_PACKET_LENGTH; i++)
    {
        out_keyboard_packet[i] = 0;
    }
}


/*
 * Function to get empty keyboard packet.
 */
void keyboard_get_empty_packet(uint8_t * out_keyboard_packet)
{
    uint_least8_t i;

    out_keyboard_packet[NRFR_KEYBOARD_MOD] = 0;

    for (i = NRFR_KEYBOARD_KEYS; i < NRFR_KEYBOARD_PACKET_LENGTH; i++)
    {
        out_keyboard_packet[i] = 0;
    }
}


