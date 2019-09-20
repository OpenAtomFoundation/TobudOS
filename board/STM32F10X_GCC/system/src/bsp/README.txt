
The following files extend or replace some of the the newlib functionality:

_startup.c: a customised startup sequence, written in C

_exit.c: a customised exit() implementation

_syscalls.c: local versions of the libnosys/librdimon code

_sbrk.c: a custom _sbrk() to match the actual linker scripts

assert.c: implementation for the asserion macros

_cxx.cpp: local versions of some C++ support, to avoid references to 
	large functions.

