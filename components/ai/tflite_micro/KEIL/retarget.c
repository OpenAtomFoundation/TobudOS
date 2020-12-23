/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005 Keil Software. All rights reserved.                     */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

typedef int FILEHANDLE;
__asm(".global __use_no_semihosting_swi");


#ifdef NUCLEO_STM32L496ZG

#include "mcu_init.h"

#else
int fputc(int ch, FILE *f) {
  /* Place your implementation of fputc here */
  /* e.g. readwrite a character to the LPUSART1 and Loop until the end of transmission */
}

int fgetc(FILE *f)
{
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the LPUSART1 and Loop until the end of transmission */
}
#endif

void _ttywrch(int ch)
{
	ch = ch;
}

int ferror(FILE *f)
{
	(void)f;
	return EOF;
}

void _sys_exit(int return_code)
{
	(void)return_code;
	while (1) {
	};
}

FILEHANDLE _sys_open(const char *name, int openmode)
{
	return 1;
}

int _sys_close(FILEHANDLE fh)
{
	return 0;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
	//your_device_write(buf, len);
	return 0;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
	return -1;
}

int _sys_istty(FILEHANDLE fh)
{
	return 0;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
	return -1;
}

long _sys_flen(FILEHANDLE fh)
{
	return -1;
}