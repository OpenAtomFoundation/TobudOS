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
#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "py/builtin.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"

#include "modmachine.h"
#if MICROPY_PY_NETWORK
#include "extmod/modnetwork.h"
#endif

#if MICROPY_VFS_TOS
#include "extmod/vfs.h"
#include "vfs_tos.h"
#endif

#include "tos_k.h"

#define MP_HEAP_SIZE        (8 * 1024)

// MicroPython GC heap
STATIC char *heap = NULL;
STATIC void *stack_top = NULL;
STATIC mp_obj_t uart_repl_obj;

int mp_main(void) 
{
    // Initialise the MicroPython runtime
    // mp_stack_ctrl_init();
    volatile int stack_dummy;
    #if MICROPY_PY_THREAD
    mp_thread_init();
    #endif
    stack_top = (void *)&stack_dummy;

    #ifdef MICROPY_HW_UART_REPL
    {
        mp_obj_t args[7] = {
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_UART_REPL),
            MP_OBJ_NEW_QSTR(MP_QSTR_rxbuf), MP_OBJ_NEW_SMALL_INT(512),
            MP_OBJ_NEW_QSTR(MP_QSTR_timeout), MP_OBJ_NEW_SMALL_INT(0),
            MP_OBJ_NEW_QSTR(MP_QSTR_timeout_char), MP_OBJ_NEW_SMALL_INT(2),
        };
        uart_repl_obj = machine_uart_type.make_new(&machine_uart_type, 1, 3, args);
        machine_uart_attach_to_repl(MP_OBJ_TO_PTR(uart_repl_obj), 1);
    }
    #endif /* MICROPY_HW_UART_REPL */

soft_reset:
    mp_stack_set_top(stack_top);
    mp_stack_set_limit(tos_task_curr_task_get()->stk_size - MP_THREAD_STACK_LIMIT_MARGIN);

    if (!heap) {
        heap = (char *)tos_mmheap_alloc(MP_HEAP_SIZE);
        if (!heap) {
            return -1;
        }
    }
    gc_init(heap, heap + MP_HEAP_SIZE);
    
    mp_init();

    #ifdef MICROPY_HW_UART_REPL
    MP_STATE_PORT(stdio_uart) = MP_OBJ_TO_PTR(uart_repl_obj);
    #endif /* MICROPY_HW_UART_REPL */

    #if MICROPY_VFS_TOS
    {
        mp_obj_t root = mp_obj_new_str("/fs", 3);
        mp_obj_t args[2] = {
            mp_type_vfs_tos.make_new(&mp_type_vfs_tos, 1, 0, &root),
            MP_OBJ_NEW_QSTR(MP_QSTR__slash_),
        };
        mp_vfs_mount(2, args, (mp_map_t *)&mp_const_empty_map);
        MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
    }
    #endif

    #if MICROPY_PY_MACHINE
    machine_pins_init();
    #endif /* MICROPY_PY_MACHINE */

    #if MICROPY_PY_NETWORK
    mod_network_init();
    #endif

    // run boot-up scripts
    // pyexec_frozen_module("_boot.py");
    pyexec_file_if_exists("boot.py");
    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
        int ret = pyexec_file_if_exists("main.py");
        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset_exit;
        }
    }

    // start a normal REPL; will exit when ctrl-D is entered on a blank line.
    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }

soft_reset_exit:

    #if MICROPY_PY_MACHINE
    machine_timer_deinit_all();
    #endif

    #if MICROPY_PY_THREAD
    mp_thread_deinit();
    #endif
    
    // Deinitialise the runtime.
    gc_sweep_all();

    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");

    mp_deinit();

    goto soft_reset;
}

// Handle uncaught exceptions (should never be reached in a correct C implemetation).
void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    for (;;) {
    }
}

// Do a garbage collection cycle
void gc_collect(void) {

    gc_collect_start();

    uintptr_t stack_base = (uint32_t)tos_task_curr_task_get()->stk_base;
    gc_collect_root((void **)stack_base, ((uintptr_t)MP_STATE_THREAD(stack_top) - (uintptr_t)stack_base) / sizeof(uintptr_t));
    
    #if MICROPY_PY_THREAD
    // trace root pointers from any threads
    mp_thread_gc_others();
    #endif

    gc_collect_end();
    // gc_dump_info();
}

#if !(MICROPY_VFS)
// There is no filesystem so stating returns noting
mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}
#endif /* MICROPY_VFS */

#if !(MICROPY_READER_VFS)
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}
#endif

#ifndef NDEBUG
#if defined(__CC_ARM)
void __aeabi_assert(const char *expr, const char *file, int line) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    for (;;) {
    }
}
#endif
NORETURN void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)func;
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    mp_raise_msg(&mp_type_AssertionError, MP_ERROR_TEXT("C-level assert"));
}
#endif

#if MICROPY_DEBUG_VERBOSE
#include <stdio.h>
#include <stdarg.h>
int DEBUG_printf(const char *format, ...)
{
    static char log_buf[512];
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */
    vsprintf(log_buf, format, args);
    printf("%s", log_buf);
    va_end(args);

    return 0;
}
#endif
