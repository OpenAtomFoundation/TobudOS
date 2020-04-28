
                      FREEMODBUS 0.8 LINUX EXAMPLE
                      ============================

REQUIREMENTS
============

This example contains a simple demo program for a Linux host.   The  default
demo opens the serial port on /dev/ttyS0 and  waits  for  commands  from  an
Modbus master. Demo versions of Modbus Masters can be found in [1] and [2].

INSTALLATION
============

The program can be built by calling 'make  flash'.   This  demo  application
was tested on Redhat Enterprise Linux 4  (RHEL4)  but  should  work  on  any
recent distribution.

TESTING
=======

Install a Null-Modem cable between /dev/ttyS0 and  /dev/ttyS1.   Next  start
the Modbus Sample Application and test if the input  registers  starting  at
protocol address 1000 can be read. There are four registers value available
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

The simple testing utility used in the 'demo.sh' script can be found at [3].

[1] WinTech ModScan32: http://www.win-tech.com/html/modscan32.htm
[2] Modus Poll: http://www.modbustools.com/modbus_poll.asp
[3] FieldTalk Modpoll: http://www.focus-sw.com/fieldtalk/modpoll.html

Version: $Id: README.txt,v 1.1 2006/08/01 20:58:49 wolti Exp $
