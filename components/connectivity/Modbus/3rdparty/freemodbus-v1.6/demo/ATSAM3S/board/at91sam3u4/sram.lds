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
ENTRY(ResetException)

/* Vector Definition */
_vect_start = 0x20000000;

/* Memory Spaces Definitions */
MEMORY
{
    sram0 (W!RX) : ORIGIN = 0x20000000, LENGTH = 0x00008000 /* Sram0, 32K */
    sram1 (W!RX) : ORIGIN = 0x20080000, LENGTH = 0x00004000 /* Sram1, 16K */
}

SECTIONS
{
    .fixed :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors))
        *(.text*)
        *(.ramfunc)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        . = ALIGN(4);
        _efixed = .;            /* End of text section */
    } > sram0

    .relocate : AT (_efixed)
    {
        . = ALIGN(4);
        _srelocate = .;
        *(.data)
        . = ALIGN(4);
        _erelocate = .;
    } >sram1

    .bss (NOLOAD) : {
        . = ALIGN(4);
        _szero = .;
        *(.bss)
        . = ALIGN(4);
        _ezero = .;
    } >sram1

    /* Stack in the end of SRAM1 */
    _estack = 0x20083FFC;
}
end = .;
