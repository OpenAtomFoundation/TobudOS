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

#include "py/mpconfig.h"

#if MICROPY_PY_MACHINE

#include "py/obj.h"
#include "py/runtime.h"
#include "modmachine.h"
#if !(MP_GEN_HDR)
#include "tos_k.h"
#endif

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    k_timer_t timer;
    mp_obj_t callback;
    mp_int_t period;
    uint8_t mode;
    uint8_t init : 1;
    uint8_t running : 1;
    struct _machine_timer_obj_t *next;
} machine_timer_obj_t;

STATIC void machine_timer_disable(machine_timer_obj_t *self);

void machine_timer_deinit_all(void) {
    machine_timer_obj_t *t = MP_STATE_PORT(machine_timer_obj_head);
    for (; t != NULL; t = t->next) {
        machine_timer_disable(t);
    }
}

STATIC void machine_timer_handler(void *arg) {
    machine_timer_obj_t *self = (machine_timer_obj_t *)arg;
    mp_sched_schedule(self->callback, MP_OBJ_FROM_PTR(self));
    if (self->mode == TOS_OPT_TIMER_ONESHOT) 
        self->running = 0;
}

STATIC void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->init) {
        qstr mode = self->mode == TOS_OPT_TIMER_ONESHOT ? MP_QSTR_ONE_SHOT : MP_QSTR_PERIODIC;
        mp_printf(print, "Timer(%p, mode=%q, period=%u, running=%u)", self, mode, self->period, self->running);
    } else {
        mp_printf(print, "Timer(%p)", self);
    }
}

STATIC mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    // mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    machine_timer_obj_t *self = mp_obj_malloc(machine_timer_obj_t, &machine_timer_type);
    self->init = 0;
    self->mode = TOS_OPT_TIMER_ONESHOT;
    self->running = 0;
    self->period = 1000;
    self->callback = mp_const_none;
    memset(&self->timer, 0x00, sizeof(k_timer_t));
    self->next = MP_STATE_PORT(machine_timer_obj_head);
    MP_STATE_PORT(machine_timer_obj_head) = self;

    if (n_args > 0 || n_kw > 0) {
        // start the timer
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_timer_enable(machine_timer_obj_t *self) {
    k_tick_t period_ticks = tos_millisec2tick(self->period);
    k_err_t err = tos_timer_create(&self->timer, period_ticks, period_ticks, 
                                    machine_timer_handler, self, self->mode);
    if (err != K_ERR_NONE) {
        mp_raise_msg(&mp_type_OSError, "can't create timer");
    }
}

STATIC void machine_timer_disable(machine_timer_obj_t *self) {
    if (self->init) {
        tos_timer_destroy(&self->timer);
        self->callback = mp_const_none;
        self->init = 0;
    }
}

STATIC mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_callback, ARG_tick_hz, ARG_period, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TOS_OPT_TIMER_PERIODIC} },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_tick_hz, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_period, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        #if MICROPY_PY_BUILTINS_FLOAT
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        #else
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        #endif
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint64_t period = self->period;
    #if MICROPY_PY_BUILTINS_FLOAT
    if (args[ARG_freq].u_obj != mp_const_none) {
        // frequency specified in Hz
        period = (uint64_t)(MICROPY_FLOAT_CONST(1000.0) / mp_obj_get_float(args[ARG_freq].u_obj));
    }
    #else
    if (args[ARG_freq].u_int != 0xffffffff) {
        // frequency specified in Hz
        period = 1000 / (args[ARG_freq].u_int);
    }
    #endif
    else {
        // period specified
        period = ((uint64_t)args[ARG_period].u_int * 1000 / args[ARG_tick_hz].u_int);
    }

    if (period < 1) {
        period = 1;
    } else if (period >= 0x40000000) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("period too large"));
    }
    self->period = period;

    if (args[ARG_mode].u_int != TOS_OPT_TIMER_ONESHOT && args[ARG_mode].u_int != TOS_OPT_TIMER_PERIODIC) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("invalid timer mode"));
    }
    self->mode = args[ARG_mode].u_int;
    self->callback = args[ARG_callback].u_obj;

    // create timer
    machine_timer_enable(self);

    self->init = 1;

    if (self->callback == mp_const_none) {
        // do nothing
    } else if (mp_obj_is_callable(self->callback)) {
        // start timer
        self->running = 1;
        tos_timer_start(&self->timer);
    }
    
    return mp_const_none;
}

// timer.init()
STATIC mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_disable(args[0]);
    return machine_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

// timer.deinit()
STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_timer_disable(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

STATIC mp_obj_t machine_timer_callback(mp_obj_t self_in, mp_obj_t callback) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (callback == mp_const_none) {
        tos_timer_stop(&self->timer);
        self->running = 0;
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
        if (!self->running) {
            // restart timer
            tos_timer_start(&self->timer);
            self->running = 1;
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_timer_callback_obj, machine_timer_callback);

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&machine_timer_callback_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TOS_OPT_TIMER_ONESHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TOS_OPT_TIMER_PERIODIC) },
};
STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

const mp_obj_type_t machine_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = machine_timer_print,
    .make_new = machine_timer_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_timer_locals_dict,
};

#endif /* MICROPY_PY_MACHINE */
