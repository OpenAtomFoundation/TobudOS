
                FreeModbus Coldfire MC523x PORT with lwIP

REQUIREMENTS
============

The FreeModbus/lwIP port requires are port of the lwIP TCP/IP stack for
the target platform. This port includes a suitable Ethernet driver for 
lwIP as well as a FreeRTOS port.
The FreeModbus part itself does NOT depend on FreeRTOS or the Coldfire 
ethernet controller and can therefore be used for creating other ports.

The development environment used is  based  on  the  GNU  C  Compiler  for
a m68k-elf target as well as the insight debugger with  some  patches  for
the BDM interface[1].  GDB startup and linker scripts  are  supplied  with
the demo for the M5235BCC evaluation kit from Freescale.

 [1] ... BDM tools: http://sourceforge.net/projects/bdm/

USAGE
=====

A makefile is supplied with the demo  application  and  a  binary  can  be
produced by calling 'make all'.  A  special  target  'debug'  is  provided
which executes the insight  debugger.   At  the  insight  debugger  prompt
one should select the  appropriate  target  interface  (either  BDM/Direct
or BDM/TCP)  and  should  download  the  application  to  the  development
board.  It is important that the GDB  script  setup-and-load  is  executed
prior to downloading to  initialize  the  SDRAM.   After  downloading  one
should call the GDB function 'execute' and the PC  is  set  to  the  start
of the executable.  Execution can  be  started  by  typing  'continue'  at
the Insight console interface.
In addition it is possible to download the software to the flash. This can
be done by calling 'make flash-erase' and 'make flash-programm'.

By default the port using the following settings:

 IP Address      : 10.0.10.2
 Netmask         : 255.255.255.0
 Gateway         : 10.0.10.1
 Modbus TCP Port : 502

COMMON PROBLEMS
===============
 
Most of the problems have  their  origin  in  the  startup  scripts.   The
following list should serve as  a  checklist  where  each  point  must  be
satisfied for the port to work.

 - The FreeRTOS port only works correctly in the supervisor mode.   There-
   fore the Coldfire CPU must run in the supervisor mode.

 - portVECTOR_TABLE does not point to the currently active  vector  table.
   Please also note that the vector table must be in  RAM  such  that  the
   FreeRTOS port can install a traphandler for the portYIELD() call.

$Id: README.txt,v 1.1 2006/08/30 23:18:06 wolti Exp $

MCF5235 + lwIP port - Copyright (c) 2006 Christian Walter.

