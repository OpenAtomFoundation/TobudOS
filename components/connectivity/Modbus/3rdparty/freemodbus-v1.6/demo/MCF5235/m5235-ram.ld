STARTUP(crt0.o)
INPUT(vector.o)
OUTPUT_ARCH(m68k)
SEARCH_DIR(.)
GROUP(-lc -lgcc)

__DYNAMIC  =  0;

MEMORY
{
  ram (rwx) : ORIGIN = 0x00100000, LENGTH = 0x00010000
  per (rwx) : ORIGIN = 0x40000000, LENGTH = 0x40000000
}

PROVIDE (__stack = 0x10FFFC);

SECTIONS
{
  .text :
  {
    __text_start = . ;
    *(.vector)
    . = ALIGN (0x100);
    *(.text)
    . = ALIGN (16);

    *(.eh_frame)
    . = ALIGN (16);

    *(.gnu.linkonce.t.*) 

    . = ALIGN(0x4);
     __CTOR_LIST__ = .;
    ___CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    *(.ctors)
    LONG(0)
    __CTOR_END__ = .;
    __DTOR_LIST__ = .;
    ___DTOR_LIST__ = .;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
    *(.dtors)
     LONG(0)
    __DTOR_END__ = .;
    *(.rodata)
    *(.gcc_except_table) 

    . = ALIGN(0x2);
    __INIT_SECTION__ = . ;
    LONG (0x4e560000)	/* linkw %fp,#0 */
    *(.init)
    SHORT (0x4e5e)	/* unlk %fp */
    SHORT (0x4e75)	/* rts */

    __FINI_SECTION__ = . ;
    LONG (0x4e560000)	/* linkw %fp,#0 */
    *(.fini)
    SHORT (0x4e5e)	/* unlk %fp */
    SHORT (0x4e75)	/* rts */

    *(.lit)
    . = ALIGN(16);
    _etext = .;
    etext = .;
  } > ram

  .data :
  {
    copy_start = .;
    *(.shdata)
    *(.data)
    *(.gnu.linkonce.d.*)
    . = ALIGN (16);
    _edata = .;
    copy_end = .;
  } > ram

  .bss :
  {
    . = ALIGN(0x4);
    __bss_start = . ;
    *(.shbss)
    *(.bss)
    *(COMMON)
    _end =  ALIGN (0x8);
    __end = _end;
  } > ram

  .peripheral (NOLOAD):
  {
    __per_start = . ;
    *(.peripheral)
  } > per

  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }

  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }
}
