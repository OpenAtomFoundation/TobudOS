
                FreeModbus STR71X/FreeRTOS/GCC Port 

REQUIREMENTS
============

This port is the same as the STR71X port  but  contains  complete  startup
scripts, makefiles  and  debugger  scripts  to  work  without  the  Rowley
Crossworks tools.
On the software side a GCC [1] installation (tested with 4.1.1) is required
together  with  a  working  C-Library  [2].   If  debugging   is   desired
a Wiggler interface must be present.  OpenOCD  [3]  binaries  for  Windows
and Linux are provided in the tools directory  together  with  a  suitable
configuration.

 [1] ... GNU ARM Homepage http://www.gnuarm.org/
 [2] ... The Newlib Homepage http://sources.redhat.com/newlib 
 [3] ... OpenOCD homepage http://openocd.berlios.de/web/
 
USAGE
=====

A makefile is supplied with the demo  application  and  a  binary  can  be
produced by calling 'make all'.  A  special  target  'debug'  is  provided
which executes the insight debugger.  Command line debugging  with  GDB  is
also possible and is described here.  After you have compiled  the  binary
start the OpenOCD daemon in a shell.

$ ./tools/openocd -f tools/str71x_wig.cfg
Info:    openocd.c:82 main(): Open On-Chip Debugger (2006-08-01 12:00 CEST)

Then start the GDB debugger and continue till you have  reached  the  main
entry point.

$ arm-elf-gdb -se=demo.elf --command=support/target-71.gdb
GNU gdb 6.5.0.20060626-cvs
...
(gdb) target remote localhost:3333
Remote debugging using localhost:3333
0x00000004 in ?? ()
(gdb) setup-and-load-ram
requesting target halt and executing a soft reset
software breakpoints enabled
Breakpoint 1 at 0x400011e4: file demo.c, line 53.
Mapped internal SRAM at address 0x00000000
(gdb) load demo.elf
Loading section .text, size 0x945c lma 0x20000000
Loading section .rodata, size 0x40c lma 0x2000945c
Loading section .data, size 0x90 lma 0x20009868
Start address 0x20000000, load size 39160
Transfer rate: 99170 bits/sec, 495 bytes/write.
(gdb) stepi
_start () at system/startup.S:97
97          ldr   pc, =NextInst
Current language:  auto; currently asm
(gdb) c
Continuing.
main () at demo.c:53
53          RCCU_Div2Config( ENABLE );
Current language:  auto; currently c
(gdb)

Now you are ready to start or debug the application. If you want to  test
your system simply enter 'continue' at the prompt.  Now start  the  Modbus
test application [1] from the 'demo.sh' or 'demo.bat' shell  script.   You
should see the following output.

$ ./demo.sh
modpoll - FieldTalk(tm) Modbus(R) Polling Utility
...
Protocol opened successfully.
Polling slave (Ctrl-C to stop) ...
[1000]: 5
[1001]: 0
[1002]: 0
[1003]: 0

If this is working then congratulations.  The Modbus Stack and the  Target
Hardware is ready.

 [1] ... You need the FieldTalk Modpoll Modbus® Polling Tool which you
         can get at http://www.focus-sw.com/fieldtalk/modpoll.html.

FLASH DOWNLOAD
==============

Flash download is possible with OpenOCD.  First edit the Makefile and  set
the variable RAM_RUN to ROM_RUN and execute 'make clean' and  'make  all'.
Open a telnet connection to your OpenOCD daemon and  enter  the  following
commands at the telnet prompt:

$ telnet localhost 4444
Trying 127.0.0.1...
Connected to localhost.localdomain (127.0.0.1).
Escape character is '^]'.
Open On-Chip Debugger
> flash probe 0
flash 'str7x' found at 0x40000000
> flash erase 0 0 9
> flash erase 1 0 3
> flash write 0 demo.bin 0x0
wrote file ../demo.bin to flash bank 0 at offset 0x00000000 in 82s 783us

Now restart the target system and the it with the  demo  application.   It
is also possible to attach a debugger but you are limited to two  hardware
breakpoints.  Use the same procedure as above but leave out the load  step
and replace 'setup-and-load-ram' by 'setup-and-load-rom'.

COMMON PROBLEMS
===============
The following list should serve as  a  checklist  where  each  point  must
be satisfied for the port to work.

 - If you have changed the execution mode make sure you have called
   'make clean'
 
$Id: README.txt,v 1.2 2006/11/02 23:29:47 wolti Exp $

STR71X FreeModbus Port Copyright (c) 2006 Christian Walter.

