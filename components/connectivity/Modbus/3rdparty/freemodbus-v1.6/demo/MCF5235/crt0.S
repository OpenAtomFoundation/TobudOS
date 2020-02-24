/*
 * FreeModbus Libary: MCF5235 Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 * Parts of crt0.S Copyright (c) 1995, 1996, 1998 Cygnus Support
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: crt0.S,v 1.1 2006/05/14 21:59:16 wolti Exp $
 */

  .title "crt0.S"

  .extern main
  .extern __stack
  .extern __bss_start
  .extern __text_start
  .extern init_main
  .global start

start:
  /* disable all interrupts on startup. */
  move.w  #0x2700, sr

  /* prepare internal SRAM. */
  move.l  #__text_start, %d0
  addq.l  #1, %d0
  movec   %d0, %rambar

  /* prepare stack and frame pointer. */
  move.l  #__stack, sp
  link    a6, #-8

  /* initialize hardware. */
  jsr     init_main

  /* zero out the bss section. */
  move.l  #__bss_start, d1
  move.l  #_end, d0
  cmp.l   d0, d1
  jbeq    3f
  move.l  d1, a0
  sub.l   d1, d0
  subq.l  #1, d0
2:
  clr.b   (a0)+
  subq.l  #1, d0
  jbpl    2b
3:

  /* C library */
  move.l  #__FINI_SECTION__, -(%sp)
  jsr     atexit
  jsr     __INIT_SECTION__

  /* enable interrupts. */
  move.w  #0x2000, sr

  /* call main(int argc, char *argv[] */
  move.l  #0, -(sp)
  move.l  #0, -(sp)
  move.l  #0, -(sp)
  jsr     main
  lea     (sp, 12), %sp

  /* stop on exit from main. */
1:
  halt

