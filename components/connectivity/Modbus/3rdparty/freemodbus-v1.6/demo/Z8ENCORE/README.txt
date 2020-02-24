
                      FREEMODBUS 1.1 Z8Encore PORT
                      ============================

REQUIREMENTS
============

This demo application  provides  a  port  for  the  Z8  Encore!®  family  of
 microcontrollers from Zilog[1]. The microcontroller used was the Z8F6422. 

It requires a wired serial port to a host  processor  and  a  Modbus  Master
Software on the PC side to be  useful.   Demo  versions  of  Modbus  Masters
can be found in [2] and [3].

SOURCE
======

If one wants to recompile the software, the ZDS  II  -  Z8  Encore!   4.10.1
should be installed.  The ZDS II - Z8 Encore!  can be  found  at  [4].   The
project is for  the  Z8F6422  microcontroller,  but  has  been  tested  with
Z8F1622. 

* The ASCII mode was disabled so flash memory of Z8F1622 was enough.


TESTING
=======

Start the  Modbus  Sample  Application  and  test  if  the  input  registers
starting at protocol address 1000 can be read.   There  are  four  registers
value available and the output should look like:

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

The simple testing utility used in the 'demo_rtu.bat' script  can  be  found
at [5].


[1] Zilog - Z8Encore 64K series: http://www.zilog.com/products/family.asp?fam=225
[2] WinTech ModScan32: http://www.win-tech.com/html/modscan32.htm
[3] Modus Poll: http://www.modbustools.com/modbus_poll.asp
[4] ZDS II - Z8 Encore!: http://www.zilog.com/software/zds2.asp
[5] FieldTalk Modpoll: http://www.focus-sw.com/fieldtalk/modpoll.html
