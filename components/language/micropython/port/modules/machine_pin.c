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

#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/virtpin.h"
#include "modmachine.h"
#if !(MP_GEN_HDR)
#include "mp_tos_hal_pin.h"
#endif

typedef struct  _machine_pin_obj_t {
    mp_obj_base_t base;
    hal_pin_port_t port;
    hal_pin_t pin;
    qstr name;
    mp_obj_t handler;
} machine_pin_obj_t;

void mp_hal_pin_write(machine_pin_obj_t *pin, int v) {
    if (tos_hal_pin_write(&pin->pin, v) != 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Pin(%q) failed to be written"), pin->name);
    }
}

int mp_hal_pin_read(machine_pin_obj_t * pin) {
    int v = tos_hal_pin_read(&pin->pin);
    if (v < 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Pin(%q) failed to be read"), pin->name);
    }
    return v;
}

void mp_hal_pin_config(machine_pin_obj_t * pin, uint32_t mode) {
    if (tos_hal_pin_init(&pin->pin, pin->port, (hal_pin_mode_t)mode) != 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Pin(%q) failed to be configured"), pin->name);
    }
}

qstr mp_hal_pin_name(machine_pin_obj_t *pin) {
    return pin->name;
}

extern const mp_obj_dict_t machine_pins_locals_dict;

machine_pin_obj_t *mp_hal_get_pin_obj(mp_obj_t name) {
    const mp_map_t *named_map = &machine_pins_locals_dict.map;
    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)named_map, name, MP_MAP_LOOKUP);
    machine_pin_obj_t *pin;
    if (named_elem != NULL && named_elem->value != MP_OBJ_NULL) {
        pin = m_new_obj(machine_pin_obj_t);
        pin->port = mp_obj_get_int(named_elem->value);
        pin->name = mp_obj_str_get_qstr(name);
        pin->base.type = &machine_pin_type;
        pin->handler = mp_const_none;
        return pin;
    }
    return NULL;
}


void machine_pins_init(void) {
    
}

STATIC machine_pin_obj_t *machine_pin_find(mp_obj_t user_obj) {
    machine_pin_obj_t *pin_obj = mp_hal_get_pin_obj(user_obj);
    if (pin_obj) {
        return pin_obj;
    }
    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%s) doesn't exist"), mp_obj_str_get_str(user_obj));
}

STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    mp_printf(print, "Pin(%q)", self->name);
}

STATIC mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Run an argument through the mapper and return the result.
    machine_pin_obj_t *pin = machine_pin_find(args[0]);
    if (pin == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(pin);
}

STATIC mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
    } else {
        // set pin
        mp_hal_pin_write(self, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

// pin.init(mode=None, pull=-1, *, value, drive, hold)
STATIC mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
    };
    
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get mode
    uint mode = args[ARG_mode].u_int;
    if (!MP_HAL_IS_PIN_MODE(mode)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin mode: %d"), mode);
    }

    // if given, set the pin value before initialising to prevent glitches
    if (args[ARG_value].u_obj != MP_OBJ_NULL) {
        mp_hal_pin_write(self, mp_obj_is_true(args[ARG_value].u_obj));
    }

    // configure the GPIO as requested
    mp_hal_pin_config(self, mode);

    return mp_const_none;
}

// pin.init(mode, pull)
STATIC mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

// pin.value([value])
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// pin.off()
STATIC mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_low(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

// pin.on()
STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_high(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

void machine_pin_isr_handler(void *arg) {
    machine_pin_obj_t *self = (machine_pin_obj_t *)arg;
    mp_sched_schedule(self->handler, MP_OBJ_FROM_PTR(self));
}

// pin.irq(handler=None,  trigger=IRQ_FALLING|IRQ_RISING)
STATIC mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = HAL_PIN_IRQ_RISING|HAL_PIN_IRQ_FALLING } },
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (n_args > 1 || kw_args->used != 0) {
        // set handler
        mp_obj_t handler = args[ARG_handler].u_obj;
        if (handler == mp_const_none || !mp_obj_is_callable(handler)) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("invalid irq handler"));
        }
        self->handler = handler;

        // configure irq
        if ((tos_hal_pin_irq_callback(&self->pin, machine_pin_isr_handler, self) != 0)
        ||  (tos_hal_pin_irq(&self->pin, (hal_pin_irq_t)args[ARG_trigger].u_int) != 0)) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Pin(%q) failed to configure irq"), self->name);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_pin_irq_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN),        MP_ROM_INT(HAL_PIN_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT),       MP_ROM_INT(HAL_PIN_MODE_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(HAL_PIN_MODE_OUTPUT_OD) },
    { MP_ROM_QSTR(MP_QSTR_IN_PULLUP),   MP_ROM_INT(HAL_PIN_MODE_INPUT_PULLUP) },
    { MP_ROM_QSTR(MP_QSTR_IN_PULLDOWN), MP_ROM_INT(HAL_PIN_MODE_INPUT_PULLDOWN) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(HAL_PIN_IRQ_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(HAL_PIN_IRQ_FALLING) },
};

STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    switch (request) {
        case MP_PIN_READ: {
            return mp_hal_pin_read(self);
        }
        case MP_PIN_WRITE: {
            mp_hal_pin_write(self, arg);
            return 0;
        }
    }
    return (mp_uint_t)-1;
}

STATIC const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

const mp_obj_type_t machine_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = machine_pin_print,
    .make_new = mp_pin_make_new,
    .call = machine_pin_call,
    .protocol = &pin_pin_p,
    .locals_dict = (mp_obj_t)&machine_pin_locals_dict,
};

#endif /* MICROPY_PY_MACHINE */
