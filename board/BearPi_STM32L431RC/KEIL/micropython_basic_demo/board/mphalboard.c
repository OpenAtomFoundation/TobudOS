
#include "py/mpconfig.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "modmachine.h"

#if !(MP_GEN_HDR)
#include "tos_k.h"
#include "main.h"
#endif

/********************** Tick *************************/

static uint32_t systick_get_us() {
    // get systick value
    uint32_t counter = SysTick->VAL;
    uint32_t load = SysTick->LOAD;

    // convert from decrementing to incrementing
    counter = load - counter;

    return (counter * 1000) / (load + 1);
}

mp_uint_t mp_hal_ticks_us(void) {
    uint32_t ms = tos_tick2millisec(tos_systick_get());
    uint32_t us = systick_get_us();
    return ms * 1000 + us;
}

void mp_hal_delay_us(mp_uint_t usec) {
    mp_uint_t ms = usec / 1000;
    mp_uint_t us = usec % 1000;
    if (ms > 0)
        mp_hal_delay_ms(ms);
    mp_uint_t start = mp_hal_ticks_us();
    while (mp_hal_ticks_us() - start < us) {
    }
}

/********************** GPIO *************************/

#define ST_PIN(port_num, pin_num)   ((uint8_t)(((port_num) << 4) & 0xF0) | ((pin_num) & 0x0F))

STATIC const mp_rom_map_elem_t machine_pins_locals_dict_table[] = {
    
    { MP_ROM_QSTR(MP_QSTR_A0),  MP_ROM_INT(ST_PIN(0, 0)) },
    { MP_ROM_QSTR(MP_QSTR_A1),  MP_ROM_INT(ST_PIN(0, 1)) },
    { MP_ROM_QSTR(MP_QSTR_A2),  MP_ROM_INT(ST_PIN(0, 2)) },
    { MP_ROM_QSTR(MP_QSTR_A3),  MP_ROM_INT(ST_PIN(0, 3)) },
    { MP_ROM_QSTR(MP_QSTR_A4),  MP_ROM_INT(ST_PIN(0, 4)) },
    { MP_ROM_QSTR(MP_QSTR_A5),  MP_ROM_INT(ST_PIN(0, 5)) },
    { MP_ROM_QSTR(MP_QSTR_A6),  MP_ROM_INT(ST_PIN(0, 6)) },
    { MP_ROM_QSTR(MP_QSTR_A7),  MP_ROM_INT(ST_PIN(0, 7)) },
    { MP_ROM_QSTR(MP_QSTR_A8),  MP_ROM_INT(ST_PIN(0, 8)) },
    { MP_ROM_QSTR(MP_QSTR_A9),  MP_ROM_INT(ST_PIN(0, 9)) },
    { MP_ROM_QSTR(MP_QSTR_A10), MP_ROM_INT(ST_PIN(0, 10)) },
    { MP_ROM_QSTR(MP_QSTR_A11), MP_ROM_INT(ST_PIN(0, 11)) },
    { MP_ROM_QSTR(MP_QSTR_A12), MP_ROM_INT(ST_PIN(0, 12)) },
    { MP_ROM_QSTR(MP_QSTR_A13), MP_ROM_INT(ST_PIN(0, 13)) },
    { MP_ROM_QSTR(MP_QSTR_A14), MP_ROM_INT(ST_PIN(0, 14)) },
    { MP_ROM_QSTR(MP_QSTR_A15), MP_ROM_INT(ST_PIN(0, 15)) },

    { MP_ROM_QSTR(MP_QSTR_B0),  MP_ROM_INT(ST_PIN(1, 0)) },
    { MP_ROM_QSTR(MP_QSTR_B1),  MP_ROM_INT(ST_PIN(1, 1)) },
    { MP_ROM_QSTR(MP_QSTR_B2),  MP_ROM_INT(ST_PIN(1, 2)) },
    { MP_ROM_QSTR(MP_QSTR_B3),  MP_ROM_INT(ST_PIN(1, 3)) },
    { MP_ROM_QSTR(MP_QSTR_B4),  MP_ROM_INT(ST_PIN(1, 4)) },
    { MP_ROM_QSTR(MP_QSTR_B5),  MP_ROM_INT(ST_PIN(1, 5)) },
    { MP_ROM_QSTR(MP_QSTR_B6),  MP_ROM_INT(ST_PIN(1, 6)) },
    { MP_ROM_QSTR(MP_QSTR_B7),  MP_ROM_INT(ST_PIN(1, 7)) },
    { MP_ROM_QSTR(MP_QSTR_B8),  MP_ROM_INT(ST_PIN(1, 8)) },
    { MP_ROM_QSTR(MP_QSTR_B9),  MP_ROM_INT(ST_PIN(1, 9)) },
    { MP_ROM_QSTR(MP_QSTR_B10), MP_ROM_INT(ST_PIN(1, 10)) },
    { MP_ROM_QSTR(MP_QSTR_B11), MP_ROM_INT(ST_PIN(1, 11)) },
    { MP_ROM_QSTR(MP_QSTR_B12), MP_ROM_INT(ST_PIN(1, 12)) },
    { MP_ROM_QSTR(MP_QSTR_B13), MP_ROM_INT(ST_PIN(1, 13)) },
    { MP_ROM_QSTR(MP_QSTR_B14), MP_ROM_INT(ST_PIN(1, 14)) },
    { MP_ROM_QSTR(MP_QSTR_B15), MP_ROM_INT(ST_PIN(1, 15)) },

    { MP_ROM_QSTR(MP_QSTR_C0),  MP_ROM_INT(ST_PIN(2, 0)) },
    { MP_ROM_QSTR(MP_QSTR_C1),  MP_ROM_INT(ST_PIN(2, 1)) },
    { MP_ROM_QSTR(MP_QSTR_C2),  MP_ROM_INT(ST_PIN(2, 2)) },
    { MP_ROM_QSTR(MP_QSTR_C3),  MP_ROM_INT(ST_PIN(2, 3)) },
    { MP_ROM_QSTR(MP_QSTR_C4),  MP_ROM_INT(ST_PIN(2, 4)) },
    { MP_ROM_QSTR(MP_QSTR_C5),  MP_ROM_INT(ST_PIN(2, 5)) },
    { MP_ROM_QSTR(MP_QSTR_C6),  MP_ROM_INT(ST_PIN(2, 6)) },
    { MP_ROM_QSTR(MP_QSTR_C7),  MP_ROM_INT(ST_PIN(2, 7)) },
    { MP_ROM_QSTR(MP_QSTR_C8),  MP_ROM_INT(ST_PIN(2, 8)) },
    { MP_ROM_QSTR(MP_QSTR_C9),  MP_ROM_INT(ST_PIN(2, 9)) },
    { MP_ROM_QSTR(MP_QSTR_C10), MP_ROM_INT(ST_PIN(2, 10)) },
    { MP_ROM_QSTR(MP_QSTR_C11), MP_ROM_INT(ST_PIN(2, 11)) },
    { MP_ROM_QSTR(MP_QSTR_C12), MP_ROM_INT(ST_PIN(2, 12)) },
    { MP_ROM_QSTR(MP_QSTR_C13), MP_ROM_INT(ST_PIN(2, 13)) },
    { MP_ROM_QSTR(MP_QSTR_C14), MP_ROM_INT(ST_PIN(2, 14)) },
    { MP_ROM_QSTR(MP_QSTR_C15), MP_ROM_INT(ST_PIN(2, 15)) },

    { MP_ROM_QSTR(MP_QSTR_D2),  MP_ROM_INT(ST_PIN(3, 2)) },

    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_INT(ST_PIN(2, 13)) },    // C13
    { MP_ROM_QSTR(MP_QSTR_KEY1), MP_ROM_INT(ST_PIN(1, 2)) },    // B2
    { MP_ROM_QSTR(MP_QSTR_KEY2), MP_ROM_INT(ST_PIN(1, 3)) },    // B3

};
MP_DEFINE_CONST_DICT(machine_pins_locals_dict, machine_pins_locals_dict_table);


