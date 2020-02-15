Overview
========
The Hello World demo application provides a sanity check for the TencentOS-tiny porting on i.MXRT1050. The Hello
World demo creates two tasks. Two tasks print the information with task number and the count of being run by OS.
The task 1 also turns to shine the LED.

Toolchain supported
===================
- IAR embedded Workbench  8.40.2
- Keil MDK  5.29 (ARM Compiler 5 only)

Hardware requirements
=====================
- Mini/micro USB cable
- EVKB-IMXRT1050 board
- Personal Computer

Board settings
==============
No special settings are required.

Prepare the Demo
================
1.  Connect a USB cable between the host PC and the OpenSDA USB port on the target board. 
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

Running the demo
================
The log below shows the output of the hello world demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Hello world from i.MX RT1010.
Welcome to TencentOS tiny
++++++++This is Task 1, count is 0
********This is Task 2, count is 0
++++++++This is Task 1, count is 1
++++++++This is Task 1, count is 2

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Note:
To download binary into hyper flash and boot from hyperflash directly, following steps are needed:
1. Select the target flexspi_nor_debug or flexspi_nor_release.
2. Compile the project, and get the binaray file "hello_world.bin"
3. Set the SW7: 1 off 2 off 3 off 4 on, then power on the board and connect USB cable to J28
4. Drop the binaray into disk "EVK-MIMXRT"
5. Wait for the disk disappear and appear again which will take around ~10s, then power off the board
6. Set the SW7: 1 off 2 on 3 on 4 off, then power on the board
7. After power on the baord, program has already started to run, reset SW4 is recommended. 
 
Note:
To debug in hyper flash in MDK, following steps are needed:
1. Select the target flexspi_nor_debug or flexspi_nor_release.
2. Compile the project.
3. Press F8 or click the download button, to program the application into hyper flash.
4. Set the SW7: 1 off 2 on 3 on 4 off, then power on the board
5. Push SW4 to reset.
6. Start to debug.


Customization options
=====================

