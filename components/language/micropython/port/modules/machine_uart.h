/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 KY-zhang-X
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_MACHINE_UART_H
#define MICROPY_INCLUDED_MACHINE_UART_H

#include "py/obj.h"

#ifndef MP_GEN_HDR
#include "tos_k.h"
#include "tos_hal.h"
#include "tos_at.h"
#endif

extern const mp_obj_type_t machine_uart_type;

typedef struct _machine_uart_obj_t machine_uart_obj_t;

/* board */
void mp_hal_uart_rx_start(uint32_t uart_id);
void mp_hal_uart_irq_handler(uint32_t uart_id);

/* repl */
int machine_uart_rx_chr(machine_uart_obj_t *self);
int machine_uart_tx_strn(machine_uart_obj_t *self, const char *str, mp_uint_t len);
void machine_uart_rx_start(machine_uart_obj_t *self);
void machine_uart_attach_to_repl(machine_uart_obj_t *self, uint8_t attached);

/* esp8266 */
void machine_uart_set_at_agent(machine_uart_obj_t *self, at_agent_t *at_agent);
hal_uart_port_t machine_uart_get_port(machine_uart_obj_t *self);

#endif /* MICROPY_INCLUDED_MACHINE_UART_H */
