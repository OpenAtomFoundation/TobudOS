
                     STR71X PORT with lwIP/PPP

REQUIREMENTS
============

The FreeModbus/lwIP port requires are port of the lwIP	TCP/IP	stack  for
the target platform.  This port includes a PPP driver  for  lwIP  together
with suitable drivers for the serial interfaces of the STR71x device.

The development environment used is Rowley Crossworks 1.5 for ARM  but	as
 they use the GNU compilers any other environment should  work	too.   The
 hardware used is STR-P711 [1] development board from Olimex.

 [1] ... STR-P711: http://www.olimex.com/dev/str-p711.html

USAGE
=====

Simply compile the software with the Rowley compilers and download  it	to
the target.  The provided configuration "Flash Debug" and "Flash  Release"
should work out of the box.  The first UART of the STR711 device  is  used
as stdout where debug messages are sent to.  You should connect  this  one
to a simple terminal program.  The second UART is used for the	PPP  conn-
ection.  You must setup a PPP server on this line.  Example for Linux  can
be found in the utils directory together with a startup  script  to  start
the server.  Remember to replace ttyS0 with the line  you  have  connected
the second UART to.

By default the port using the following settings:

 IP Address (STR71X) : 10.0.10.2
 IP Address (HOST)   : 10.0.10.1
 Modbus TCP Port     : 502

If you have configured everything correctly you should see  the  following
output from your local PPP daemon.

----------------------- start /var/log/messages output ----------------------
Sep  4 21:12:40 pcno-4 pppd[22597]: pppd 2.4.2 started by root, uid 0
Sep  4 21:12:40 pcno-4 pppd[22597]: Using interface ppp0
Sep  4 21:12:40 pcno-4 pppd[22597]: Connect: ppp0 <--> /dev/ttyS0
Sep  4 21:12:44 pcno-4 pppd[22597]: found interface eth0 for proxy arp
Sep  4 21:12:44 pcno-4 pppd[22597]: local  IP address 10.0.10.1
Sep  4 21:12:44 pcno-4 pppd[22597]: remote IP address 10.0.10.2
----------------------- end /var/log/messages output ------------------------

The output from the debug console received by the target should match:

----------------------- start console output --------------------------------
ERROR: PPP: new PPP connection established
ERROR: PPP:   our IP address = 10.0.10.2
ERROR: PPP:   his IP address = 10.0.10.1
ERROR: PPP:   netmask = 255.255.255.0
DEBUG: MBTCP-ACCEPT: Accepted new client 10.0.10.1
DEBUG: MBTCP-RECV: | TID = 0000 | PID = 0000 | LEN = 0006 | UID = 01||
  FUNC = 04 | DATA = 03E70002 |
DEBUG: MBTCP-SENT: | TID = 0000 | PID = 0000 | LEN = 0007 | UID = 01||
  FUNC = 04 | DATA = 04AC610001 |
DEBUG: MBTCP-RECV: | TID = 0000 | PID = 0000 | LEN = 0006 | UID = 01|| 
  FUNC = 04 | DATA = 03E70002 |
DEBUG: MBTCP-SENT: | TID = 0000 | PID = 0000 | LEN = 0007 | UID = 01||
  FUNC = 04 | DATA = 04B0420001 |
----------------------- end console output ----------------------------------

$Id: README.txt,v 1.1 2006/09/04 19:33:58 wolti Exp $

STR71X + lwIP/PPP port - Copyright (c) 2006 Christian Walter.

