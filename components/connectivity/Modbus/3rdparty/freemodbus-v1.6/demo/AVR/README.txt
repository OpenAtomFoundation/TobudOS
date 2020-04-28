
                      FREEMODBUS 0.4 AVR EXAMPLE
                      ==========================

REQUIREMENTS
============

This example contains a simple demo program for FreeMODBUS for an ATMega168 
processor. It requires a wired serial port to a host processor and a Modbus
Master Software on the PC side to be useful. Demo versions of Modbus Masters
can be found in [1] and [2].

In addition it requires an ATMega168 target microcontroller. The micro-
controller must be configured for 20Mhz if the binary image is going to
be used. Otherwise the constant F_CPU in the Makefile must be changed
and the source code recompiled.

INSTALLATION
============

If avrdude [3] is used for flashing the Makefile contains a target 'flash'
which can be executed by calling 'make flash'. This programs the micro-
controller with the current binary named 'demo.hex'.

SOURCE
======

If one wants to recompile the software the WinAVR tools should be installed
if the host platform is Microsoft Windows. For Unix the necessary tools
must be recompiled from source. The WinAVR tools can be found at [4].

TESTING
=======

Start the Modbus Sample Application and test if the input registers starting
at protocol address 1000 can be read. There are four registers value available
and the output should look like:

Polling slave (Ctrl-C to stop) ...
[1000]: 6474
[1001]: 0
[1002]: 0
[1003]: 0
Polling slave (Ctrl-C to stop) ...
[1000]: -8831
[1001]: 0
[1002]: 0
[1003]: 0
Polling slave (Ctrl-C to stop) ...

The simple testing utility used in the 'demo_rtu.bat' script can be found
at [5].

PROBLEMS
========

If the provided binary is not working as expected make sure that the fuse
bits for the microcontroller are programmed correctly. In addition it would
be a good idea to download a simple serial test programm to test serial
communication with the host. The fuse bits required for an ATMega168 with
an 20Mhz crystal must be set to the following values:

 Fuse Low Byte: 1XXXXXX 
                |
               CKDIV8

The other values depend on the crystal used and the startup time. The
authors configuration used a value of 0xFF. Again if avrdude is used the
value can be set by:

  avrdude -p m168 -c stk200 -U lfuse:w:0xFF:m

where the 0xFF at the end of line is the actual value.

[1] WinTech ModScan32: http://www.win-tech.com/html/modscan32.htm
[2] Modus Poll: http://www.modbustools.com/modbus_poll.asp
[3] AVRDUDE AVR Downloader/UploaDEr:  http://www.nongnu.org/avrdude/
[4] WinAVR: http://winavr.sourceforge.net/
[5] FieldTalk Modpoll: http://www.focus-sw.com/fieldtalk/modpoll.html

Version: $Id: README.txt,v 1.1 2006/05/08 21:07:01 wolti Exp $
