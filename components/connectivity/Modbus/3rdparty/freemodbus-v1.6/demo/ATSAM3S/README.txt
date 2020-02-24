
                      FREEMODBUS 1.5 ATSAM3S EXAMPLE
                      ==============================

REQUIREMENTS
============

This example contains a simple demo program for FreeMODBUS  for the AT91SAM3S
Cortex M3 controllers on the SAM3S-EK [0] evaluation kit. It requires a wired 
RS485  connection to a host processor and a  MODBUS master software on the PC
side to  be useful. Demo  versions of MODBUS  master  stacks can be  found in 
[1], [2] and [3]. Commercial MODBUS stacks are available from [4].

INSTALLATION
============
The SAM3S-EK evaluation board should be modified as following to enable RS485
data transmission. 

 - Place Jumpers JP11, JP10 and JP12
 - Place a solder drop accross R25

Warning: The RS485 shares data lines with USART1. It  is therefore absolutely
necessary to set PA23 to a high level.

SOURCE
======

TESTING
=======

Start the MODBUS sample application and  test if the input registers starting
at protocol address  1000 can be read. There are  four registers values avai-
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

The simple testing utility  used in the 'demo_rtu.bat' script can be found at
[3].

PROBLEMS
========

In case you can get no communication working make sure that the RS485 line is
biased correctly. The SAM3S-EK does not populate the RS485 bias resistors by 
default and if you have a RS485 master which does not bias the lines either
you will get wrong characters. For this R24 and R30 on the bottom side of the
evaluation kit of the PCB should be populated.

REFERENCES
==========

[0] Atmel SAM3S-EK: http://www.atmel.com/dyn/products/tools_card_v2.asp?tool_id=4678
[1] WinTech ModScan32: http://www.win-tech.com/html/modscan32.htm
[2] Modus Poll: http://www.modbustools.com/modbus_poll.asp
[3] FieldTalk Modpoll: http://www.focus-sw.com/fieldtalk/modpoll.html
[4] Embedded Solutions: http://www.embedded-solutions.at

Version: $Id: README.txt,v 1.1 2010/06/05 09:57:46 wolti Exp $
