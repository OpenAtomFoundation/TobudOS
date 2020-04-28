/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 *      Linker script for running in internal SRAM on the AT91SAM3U4
 *----------------------------------------------------------------------------*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(entry)

/* Memory Spaces Definitions */
MEMORY
{
    romcodesram (W!RX) : ORIGIN = 0x20000000, LENGTH = 0x01000
    sram0 (W!RX) : ORIGIN = 0x20001000, LENGTH = 0x00007000 /* Sram0, 32K */
}

/* Entry point */
/*ENTRY (ResetException)*/

SECTIONS
{
    /* startup code in the .isr_vector */
    .text :
    {
        . = ALIGN(4);
        _stext = .;
        KEEP(*(.isr_vector .isr_vector.*))
        *(.mailbox)
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.glue_7)
        *(.glue_7t)
        *(.gcc_except_table)
        *(.rodata .rodata*)
        *(.gnu.linkonce.r.*)
        . = ALIGN(4);
        _etext = .;
    } > sram0

    /* data */
    .data :
    {
        . = ALIGN(4);
        _sidata = .;
        _sdata = .;

        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    } > sram0

    .bss (NOLOAD) : {
        _szero = .;
        *(.bss)
        . = ALIGN(4);
        _ezero = .;
    } >sram0

    /* Stack in SRAM0 */
    _sstack = 0x20008000;
}
end = .;

