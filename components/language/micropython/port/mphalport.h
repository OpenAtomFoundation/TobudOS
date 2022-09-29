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

#define mp_hal_stdio_poll(poll_flags) (0) // NOT IMPLEMENT

#include "mphalboard.h"
#include "py/obj.h"
#include "shared/runtime/interrupt_char.h"

#if !(MP_GEN_HDR)
#include "tos_k.h"
#include "mp_tos_hal_pin.h"
#endif

#define MP_HAL_IS_PIN_MODE(mode) \
   (((mode) == HAL_PIN_MODE_OUTPUT) || \
    ((mode) == HAL_PIN_MODE_OUTPUT_OD) || \
    ((mode) == HAL_PIN_MODE_INPUT) || \
    ((mode) == HAL_PIN_MODE_INPUT_PULLUP) || \
    ((mode) == HAL_PIN_MODE_INPUT_PULLDOWN))

struct _machine_pin_obj_t;
#define MP_HAL_PIN_FMT "%q"
#define mp_hal_pin_obj_t struct _machine_pin_obj_t *
void mp_hal_pin_write(mp_hal_pin_obj_t pin, int v);
int mp_hal_pin_read(mp_hal_pin_obj_t pin);
void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode);

#define mp_hal_pin_high(pin)        mp_hal_pin_write((pin), 1)
#define mp_hal_pin_low(pin)         mp_hal_pin_write((pin), 0)
#define mp_hal_pin_od_high(pin)     mp_hal_pin_high(pin)
#define mp_hal_pin_od_low(pin)      mp_hal_pin_low(pin)
#define mp_hal_pin_input(pin)       mp_hal_pin_config((pin), HAL_PIN_MODE_INPUT)
#define mp_hal_pin_output(pin)      mp_hal_pin_config((pin), HAL_PIN_MODE_OUTPUT)
#define mp_hal_pin_open_drain(pin)  mp_hal_pin_config((pin), HAL_PIN_MODE_OUTPUT_OD)

#define mp_hal_delay_us_fast(us)    mp_hal_delay_us(us)
