OUTPUT_ARCH( "riscv" )
ENTRY( _start )

MEMORY
{
    ROM      (rxai!w)    : ORIGIN = 0x80000000, LENGTH = 512K
    RAM      (wxa!ri)    : ORIGIN = 0x84000000, LENGTH = 128K
}

SECTIONS
{
  .text : {
  	PROVIDE( _text = . );
  	*(.text.entry)
  	*(.text)
  	*(.rodata)
  	PROVIDE( _etext = . );
  } >ROM AT>ROM
  
  . = ALIGN(4);
  _load_data = LOADADDR(.data);
  
  . = ALIGN(4);
  .data : {
  	PROVIDE( _data = . );
  	*(.data) 
  . = ALIGN(4);
  } >RAM AT>ROM  
  . = ALIGN(4);
  PROVIDE( _edata = . );
  
  . = ALIGN(0x1000);
  PROVIDE( _bss = . );
  .bss : {
  	*(.bss)
  } >RAM AT>RAM  
  . = ALIGN(4);
  PROVIDE( _ebss = . );
  
  
  . = ALIGN(8);
  PROVIDE( end = . );
  
  _stack_size = 128;
  .stack ORIGIN(RAM) + LENGTH(RAM) - _stack_size :
  {
    . = _stack_size;  
    PROVIDE( _stack_top = . ); 
  } >RAM AT>RAM
  _end = .;
}
