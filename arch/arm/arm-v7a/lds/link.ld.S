#include "chip/chip.h"

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(_start)

SECTIONS
{
    . = KERNEL_BASE;

    _start = .;

    .text : AT(KERNEL_BASE) {
        __text_start = .;

        KEEP(*(.text.vector))
        *(.text)
        *(.text.*)

        KEEP(*(.init))
        KEEP(*(.fini))
    }

    .ARM.extab : ALIGN(4) {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }

    /* .ARM.exidx is sorted, so has to go in its own output section. */
    __exidx_start = .;
    .ARM.exidx : ALIGN(4) {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    }
    __exidx_end = .;

    .dummy_post_text : {
        __text_end = .;
    }

    .rodata : ALIGN(4) {
        __rodata_start = .;
        *(.rodata)
        *(.rodata.*)
    }

    .dummy_post_rodata : {
        __rodata_end = .;
    }

    .data : ALIGN(4) {
        __data_start = .;
        *(.data)
        *(.data.*)
    }

    .ctors : ALIGN(4) {
        __ctor_list = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.ctors .init_array*))
        __ctor_end = .;
    }

    .dtors : ALIGN(4) {
        __dtor_list = .;
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.dtors .fini_array*))
        __dtor_end = .;
    }

    .dummy_post_rodata : {
        __data_end = .;
    }

    .bss : ALIGN(4) {
        KEEP(*(.bss.prebss.*))
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss .bss.*)
        *(COMMON)

        . = ALIGN(4);
        __bss_end__ = .;
    }

    _end = .;

    /* Strip unnecessary stuff */
    /DISCARD/ : { *(.comment .note) }
}

