/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ARM_SEMIHOSTING_H_
#define ARM_SEMIHOSTING_H_

// ----------------------------------------------------------------------------

// Semihosting operations.
enum OperationNumber
{
  // Regular operations
  SEMIHOSTING_EnterSVC = 0x17,
  SEMIHOSTING_ReportException = 0x18,
  SEMIHOSTING_SYS_CLOSE = 0x02,
  SEMIHOSTING_SYS_CLOCK = 0x10,
  SEMIHOSTING_SYS_ELAPSED = 0x30,
  SEMIHOSTING_SYS_ERRNO = 0x13,
  SEMIHOSTING_SYS_FLEN = 0x0C,
  SEMIHOSTING_SYS_GET_CMDLINE = 0x15,
  SEMIHOSTING_SYS_HEAPINFO = 0x16,
  SEMIHOSTING_SYS_ISERROR = 0x08,
  SEMIHOSTING_SYS_ISTTY = 0x09,
  SEMIHOSTING_SYS_OPEN = 0x01,
  SEMIHOSTING_SYS_READ = 0x06,
  SEMIHOSTING_SYS_READC = 0x07,
  SEMIHOSTING_SYS_REMOVE = 0x0E,
  SEMIHOSTING_SYS_RENAME = 0x0F,
  SEMIHOSTING_SYS_SEEK = 0x0A,
  SEMIHOSTING_SYS_SYSTEM = 0x12,
  SEMIHOSTING_SYS_TICKFREQ = 0x31,
  SEMIHOSTING_SYS_TIME = 0x11,
  SEMIHOSTING_SYS_TMPNAM = 0x0D,
  SEMIHOSTING_SYS_WRITE = 0x05,
  SEMIHOSTING_SYS_WRITEC = 0x03,
  SEMIHOSTING_SYS_WRITE0 = 0x04,

  // Codes returned by SEMIHOSTING_ReportException
  ADP_Stopped_ApplicationExit = ((2 << 16) + 38),
  ADP_Stopped_RunTimeError = ((2 << 16) + 35),

};

// ----------------------------------------------------------------------------

// SWI numbers and reason codes for RDI (Angel) monitors.
#define AngelSWI_ARM                    0x123456
#ifdef __thumb__
#define AngelSWI                        0xAB
#else
#define AngelSWI                        AngelSWI_ARM
#endif
// For thumb only architectures use the BKPT instruction instead of SWI.
#if defined(__ARM_ARCH_7M__)     \
    || defined(__ARM_ARCH_7EM__) \
    || defined(__ARM_ARCH_6M__)
#define AngelSWIInsn                    "bkpt"
#define AngelSWIAsm                     bkpt
#else
#define AngelSWIInsn                    "swi"
#define AngelSWIAsm                     swi
#endif

#if defined(OS_DEBUG_SEMIHOSTING_FAULTS)
// Testing the local semihosting handler cannot use another BKPT, since this
// configuration cannot trigger HaedFault exceptions while the debugger is
// connected, so we use an illegal op code, that will trigger an
// UsageFault exception.
#define AngelSWITestFault       "setend be"
#define AngelSWITestFaultOpCode (0xB658)
#endif

static inline int
__attribute__ ((always_inline))
call_host (int reason, void* arg)
{
  int value;
  asm volatile (

      " mov r0, %[rsn]  \n"
      " mov r1, %[arg]  \n"
#if defined(OS_DEBUG_SEMIHOSTING_FAULTS)
      " " AngelSWITestFault " \n"
#else
      " " AngelSWIInsn " %[swi] \n"
#endif
      " mov %[val], r0"

      : [val] "=r" (value) /* Outputs */
      : [rsn] "r" (reason), [arg] "r" (arg), [swi] "i" (AngelSWI) /* Inputs */
      : "r0", "r1", "r2", "r3", "ip", "lr", "memory", "cc"
      // Clobbers r0 and r1, and lr if in supervisor mode
  );

  // Accordingly to page 13-77 of ARM DUI 0040D other registers
  // can also be clobbered. Some memory positions may also be
  // changed by a system call, so they should not be kept in
  // registers. Note: we are assuming the manual is right and
  // Angel is respecting the APCS.
  return value;
}

// ----------------------------------------------------------------------------

// Function used in _exit() to return the status code as Angel exception.
static inline void
__attribute__ ((always_inline,noreturn))
report_exception (int reason)
{
  call_host (SEMIHOSTING_ReportException, (void*) reason);

  for (;;)
    ;
}

// ----------------------------------------------------------------------------

#endif // ARM_SEMIHOSTING_H_
