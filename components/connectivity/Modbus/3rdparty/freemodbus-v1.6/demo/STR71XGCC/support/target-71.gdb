#
# STR71X GDB Target Scripts for FreeModbus
# Copyright 2005 Anglia Design, Spencer Oliver
# Modifications Copyright (C) 2006 Christian Walter <wolti@sil.at>
#
# This demo code and associated components are provided as is and has no
# warranty, implied or otherwise.  You are free to use/modify any of the
# provided code at your own risk in your applications with the expressed
# limitation of liability (see below)
#
# LIMITATION OF LIABILITY:   ANGLIA OR ANGLIA DESIGNS SHALL NOT BE LIABLE
# FOR ANY LOSS OF PROFITS, LOSS OF USE, LOSS OF DATA, INTERRUPTION OF
# BUSINESS, NOR FOR INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
# OF ANY KIND WHETHER UNDER THIS AGREEMENT OR OTHERWISE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGES.
#
# $Id: target-71.gdb,v 1.1 2006/11/02 23:14:44 wolti Exp $
#

set remote memory-write-packet-size 1024
set remote memory-write-packet-size fixed

set remote memory-read-packet-size 1024
set remote memory-read-packet-size fixed
set remote hardware-watchpoint-limit 2
set remote hardware-breakpoint-limit 2

# reset and map 0 to internal ram
define ramreset
  set *(int*)0xA0000050 = 0x800A
  monitor arm7_9 sw_bkpts enable
  tb main
  echo Mapped internal SRAM at address 0x00000000\n
end

# reset and map 0 to flash
define romreset
  set *(int*)0xA0000050 = 0x8008
  monitor arm7_9 force_hw_bkpts enable
  thb main
  echo Mapped internal FLASH at address 0x00000000\n
end

define setup-and-load-rom
  reset
  romreset
end

define setup-and-load-ram
  reset
  ramreset
end

define reset
  monitor reset
  monitor sleep 50
  monitor soft_reset_halt
end

