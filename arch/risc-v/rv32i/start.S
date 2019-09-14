// See LICENSE for license details.

#include "riscv_encoding.h"

.section .text.entry
    .globl _start
    .type _start,@function
_start:
    csrc mstatus, MSTATUS_MIE
    csrw mie, 0

    la t0, machine_trap_entry
    csrw mtvec, t0

    la sp, _stack_top

    /* Load data section */
    la a0, _load_data
    la a1, _data
    la a2, _edata
    bgeu a1, a2, begin_clear_bss
clear_data:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, clear_data

begin_clear_bss:
    // clear bss section
    la a0, _bss
    la a1, _ebss
    bgeu a0, a1, init_finish
clear_bss:
    sw zero, (a0)
    addi a0, a0, 4
    bltu a0, a1, clear_bss
init_finish:
    call main
__die:
    j __die
