
                      FREEMODBUS 1.2 LPC214X PORT
                      ===========================

REQUIREMENTS
============

This demo application provides a port for the LPC214X series  of  processors
from NXP Semiconductors.  The port was done using the MCB2140  from  Keil[1]
which features a LPC2148 MCU.
It requires a wired serial port to a host  processor  and  a  Modbus  Master
Software on the PC side to be  useful.   Demo  versions  of  Modbus  Masters
can be found in [2] and [3].


TESTING
=======

Start the Modbus Sample Application and test if the input registers starting
at protocol  address 1000 can be  read. There are four registers value avai-
lable and the output should look like:

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
at [4].


[1] Keil MCB2140 kit: http://www.keil.com/mcb2140/
[2] WinTech ModScan32: http://www.win-tech.com/html/modscan32.htm
[3] Modus Poll: http://www.modbustools.com/modbus_poll.asp
[4] FieldTalk Modpoll: http://www.focus-sw.com/fieldtalk/modpoll.html




