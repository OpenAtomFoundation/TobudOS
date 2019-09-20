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

// ----------------------------------------------------------------------------

#include "cortexm/ExceptionHandlers.h"
#include "cmsis_device.h"
#include "arm/semihosting.h"
#include "diag/Trace.h"
#include <string.h>

// ----------------------------------------------------------------------------

extern void
__attribute__((noreturn,weak))
_start (void);

// ----------------------------------------------------------------------------
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
// ----------------------------------------------------------------------------

#if defined(DEBUG)

// The DEBUG version is not naked, but has a proper stack frame,
// to allow setting breakpoints at Reset_Handler.
void __attribute__ ((section(".after_vectors"),noreturn))
Reset_Handler (void)
{
  _start ();
}

#else

// The Release version is optimised to a quick branch to _start.
void __attribute__ ((section(".after_vectors"),naked))
Reset_Handler(void)
  {
    asm volatile
    (
        " ldr     r0,=_start \n"
        " bx      r0"
        :
        :
        :
    );
  }

#endif

void __attribute__ ((section(".after_vectors"),weak))
NMI_Handler (void)
{
#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

// ----------------------------------------------------------------------------

#if defined(TRACE)

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

// The values of BFAR and MMFAR stay unchanged if the BFARVALID or
// MMARVALID is set. However, if a new fault occurs during the
// execution of this fault handler, the value of the BFAR and MMFAR
// could potentially be erased. In order to ensure the fault addresses
// accessed are valid, the following procedure should be used:
// 1. Read BFAR/MMFAR.
// 2. Read CFSR to get BFARVALID or MMARVALID. If the value is 0, the
//    value of BFAR or MMFAR accessed can be invalid and can be discarded.
// 3. Optionally clear BFARVALID or MMARVALID.
// (See Joseph Yiu's book).

void
dumpExceptionStack (ExceptionStackFrame* frame,
                uint32_t cfsr, uint32_t mmfar, uint32_t bfar,
                                        uint32_t lr)
{
  trace_printf ("Stack frame:\n");
  trace_printf (" R0 =  %08X\n", frame->r0);
  trace_printf (" R1 =  %08X\n", frame->r1);
  trace_printf (" R2 =  %08X\n", frame->r2);
  trace_printf (" R3 =  %08X\n", frame->r3);
  trace_printf (" R12 = %08X\n", frame->r12);
  trace_printf (" LR =  %08X\n", frame->lr);
  trace_printf (" PC =  %08X\n", frame->pc);
  trace_printf (" PSR = %08X\n", frame->psr);
  trace_printf ("FSR/FAR:\n");
  trace_printf (" CFSR =  %08X\n", cfsr);
  trace_printf (" HFSR =  %08X\n", SCB->HFSR);
  trace_printf (" DFSR =  %08X\n", SCB->DFSR);
  trace_printf (" AFSR =  %08X\n", SCB->AFSR);

  if (cfsr & (1UL << 7))
    {
      trace_printf (" MMFAR = %08X\n", mmfar);
    }
  if (cfsr & (1UL << 15))
    {
      trace_printf (" BFAR =  %08X\n", bfar);
    }
  trace_printf ("Misc\n");
  trace_printf (" LR/EXC_RETURN= %08X\n", lr);
}

#endif // defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

#if defined(__ARM_ARCH_6M__)

void
dumpExceptionStack (ExceptionStackFrame* frame, uint32_t lr)
{
  trace_printf ("Stack frame:\n");
  trace_printf (" R0 =  %08X\n", frame->r0);
  trace_printf (" R1 =  %08X\n", frame->r1);
  trace_printf (" R2 =  %08X\n", frame->r2);
  trace_printf (" R3 =  %08X\n", frame->r3);
  trace_printf (" R12 = %08X\n", frame->r12);
  trace_printf (" LR =  %08X\n", frame->lr);
  trace_printf (" PC =  %08X\n", frame->pc);
  trace_printf (" PSR = %08X\n", frame->psr);
  trace_printf ("Misc\n");
  trace_printf (" LR/EXC_RETURN= %08X\n", lr);
}

#endif // defined(__ARM_ARCH_6M__)

#endif // defined(TRACE)

// ----------------------------------------------------------------------------

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

#if defined(OS_USE_SEMIHOSTING) || defined(OS_USE_TRACE_SEMIHOSTING_STDOUT) || defined(OS_USE_TRACE_SEMIHOSTING_DEBUG)

int
isSemihosting (ExceptionStackFrame* frame, uint16_t opCode);

/**
 * This function provides the minimum functionality to make a semihosting program execute even without the debugger present.
 * @param frame pointer to an exception stack frame.
 * @param opCode the 16-bin word of the BKPT instruction.
 * @return 1 if the instruction was a valid semihosting call; 0 otherwise.
 */
int
isSemihosting (ExceptionStackFrame* frame, uint16_t opCode)
{
  uint16_t* pw = (uint16_t*) frame->pc;
  if (*pw == opCode)
    {
      uint32_t r0 = frame->r0;
#if defined(OS_DEBUG_SEMIHOSTING_FAULTS) || defined(OS_USE_SEMIHOSTING) || defined(OS_USE_TRACE_SEMIHOSTING_STDOUT)
      uint32_t r1 = frame->r1;
#endif
#if defined(OS_USE_SEMIHOSTING) || defined(OS_USE_TRACE_SEMIHOSTING_STDOUT)
      uint32_t* blk = (uint32_t*) r1;
#endif

#if defined(OS_DEBUG_SEMIHOSTING_FAULTS)
      // trace_printf ("sh r0=%d\n", r0);
#endif

      switch (r0)
        {

#if defined(OS_USE_SEMIHOSTING)

        case SEMIHOSTING_SYS_CLOCK:
        case SEMIHOSTING_SYS_ELAPSED:
        case SEMIHOSTING_SYS_FLEN:
        case SEMIHOSTING_SYS_GET_CMDLINE:
        case SEMIHOSTING_SYS_REMOVE:
        case SEMIHOSTING_SYS_RENAME:
        case SEMIHOSTING_SYS_SEEK:
        case SEMIHOSTING_SYS_SYSTEM:
        case SEMIHOSTING_SYS_TICKFREQ:
        case SEMIHOSTING_SYS_TMPNAM:
        case SEMIHOSTING_SYS_ISTTY:
          frame->r0 = (uint32_t)-1; // the call is not successful or not supported
          break;

        case SEMIHOSTING_SYS_CLOSE:
          frame->r0 = 0; // call is successful
          break;

        case SEMIHOSTING_SYS_ERRNO:
          frame->r0 = 0; // the value of the C library errno variable.
          break;

        case SEMIHOSTING_SYS_HEAPINFO:
          blk[0] = 0; // heap_base
          blk[1] = 0; // heap_limit
          blk[2] = 0; // stack_base
          blk[3] = 0; // stack_limit
          break;

        case SEMIHOSTING_SYS_ISERROR:
          frame->r0 = 0; // 0 if the status word is not an error indication
          break;

        case SEMIHOSTING_SYS_READ:
          // If R0 contains the same value as word 3, the call has
          // failed and EOF is assumed.
          frame->r0 = blk[2];
          break;

        case SEMIHOSTING_SYS_READC:
          frame->r0 = '\0'; // the byte read from the console.
          break;

        case SEMIHOSTING_SYS_TIME:
          frame->r0 = 0; // the number of seconds since 00:00 January 1, 1970.
          break;

        case SEMIHOSTING_ReportException:

          NVIC_SystemReset ();
          // Should not reach here
          return 0;

#endif // defined(OS_USE_SEMIHOSTING)

#if defined(OS_USE_SEMIHOSTING) || defined(OS_USE_TRACE_SEMIHOSTING_STDOUT)

#define HANDLER_STDIN   (1)
#define HANDLER_STDOUT  (2)
#define HANDLER_STDERR  (3)

        case SEMIHOSTING_SYS_OPEN:
          // Process only standard io/out/err and return 1/2/3
          if (strcmp ((char*) blk[0], ":tt") == 0)
            {
              if ((blk[1] == 0))
                {
                  frame->r0 = HANDLER_STDIN;
                  break;
                }
              else if (blk[1] == 4)
                {
                  frame->r0 = HANDLER_STDOUT;
                  break;
                }
              else if (blk[1] == 8)
                {
                  frame->r0 = HANDLER_STDERR;
                  break;
                }
            }
          frame->r0 = (uint32_t)-1; // the call is not successful or not supported
          break;

        case SEMIHOSTING_SYS_WRITE:
          // Silently ignore writes to stdout/stderr, fail on all other handler.
          if ((blk[0] == HANDLER_STDOUT) || (blk[0] == HANDLER_STDERR))
            {
#if defined(OS_DEBUG_SEMIHOSTING_FAULTS)
              frame->r0 = (uint32_t) blk[2]
                  - trace_write ((char*) blk[1], blk[2]);
#else
              frame->r0 = 0; // all sent, no more.
#endif // defined(OS_DEBUG_SEMIHOSTING_FAULTS)
            }
          else
            {
              // If other handler, return the total number of bytes
              // as the number of bytes that are not written.
              frame->r0 = blk[2];
            }
          break;

#endif // defined(OS_USE_SEMIHOSTING) || defined(OS_USE_TRACE_SEMIHOSTING_STDOUT)

#if defined(OS_USE_SEMIHOSTING) || defined(OS_USE_TRACE_SEMIHOSTING_STDOUT) || defined(OS_USE_TRACE_SEMIHOSTING_DEBUG)

        case SEMIHOSTING_SYS_WRITEC:
#if defined(OS_DEBUG_SEMIHOSTING_FAULTS)
          {
            char ch = *((char*) r1);
            trace_write (&ch, 1);
          }
#endif
          // Register R0 is corrupted.
          break;

        case SEMIHOSTING_SYS_WRITE0:
#if defined(OS_DEBUG_SEMIHOSTING_FAULTS)
          {
            char* p = ((char*) r1);
            trace_write (p, strlen (p));
          }
#endif
          // Register R0 is corrupted.
          break;

#endif

        default:
          return 0;
        }

      // Alter the PC to make the exception returns to
      // the instruction after the faulty BKPT.
      frame->pc += 2;
      return 1;
    }
  return 0;
}

#endif

// Hard Fault handler wrapper in assembly.
// It extracts the location of stack frame and passes it to handler
// in C as a pointer. We also pass the LR value as second
// parameter.
// (Based on Joseph Yiu's, The Definitive Guide to ARM Cortex-M3 and
// Cortex-M4 Processors, Third Edition, Chap. 12.8, page 402).

void __attribute__ ((section(".after_vectors"),weak,naked))
HardFault_Handler (void)
{
  asm volatile(
      " tst lr,#4       \n"
      " ite eq          \n"
      " mrseq r0,msp    \n"
      " mrsne r0,psp    \n"
      " mov r1,lr       \n"
      " ldr r2,=HardFault_Handler_C \n"
      " bx r2"

      : /* Outputs */
      : /* Inputs */
      : /* Clobbers */
  );
}

void __attribute__ ((section(".after_vectors"),weak,used))
HardFault_Handler_C (ExceptionStackFrame* frame __attribute__((unused)),
                     uint32_t lr __attribute__((unused)))
{
#if defined(TRACE)
  uint32_t mmfar = SCB->MMFAR; // MemManage Fault Address
  uint32_t bfar = SCB->BFAR; // Bus Fault Address
  uint32_t cfsr = SCB->CFSR; // Configurable Fault Status Registers
#endif

#if defined(OS_USE_SEMIHOSTING) || defined(OS_USE_TRACE_SEMIHOSTING_STDOUT) || defined(OS_USE_TRACE_SEMIHOSTING_DEBUG)

  // If the BKPT instruction is executed with C_DEBUGEN == 0 and MON_EN == 0,
  // it will cause the processor to enter a HardFault exception, with DEBUGEVT
  // in the Hard Fault Status register (HFSR) set to 1, and BKPT in the
  // Debug Fault Status register (DFSR) also set to 1.

  if (((SCB->DFSR & SCB_DFSR_BKPT_Msk) != 0)
      && ((SCB->HFSR & SCB_HFSR_DEBUGEVT_Msk) != 0))
    {
      if (isSemihosting (frame, 0xBE00 + (AngelSWI & 0xFF)))
        {
          // Clear the exception cause in exception status.
          SCB->HFSR = SCB_HFSR_DEBUGEVT_Msk;

          // Continue after the BKPT
          return;
        }
    }

#endif

#if defined(TRACE)
  trace_printf ("[HardFault]\n");
  dumpExceptionStack (frame, cfsr, mmfar, bfar, lr);
#endif // defined(TRACE)

#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

#endif // defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)


#if defined(__ARM_ARCH_6M__)

// Hard Fault handler wrapper in assembly.
// It extracts the location of stack frame and passes it to handler
// in C as a pointer. We also pass the LR value as second
// parameter.
// (Based on Joseph Yiu's, The Definitive Guide to ARM Cortex-M0
// First Edition, Chap. 12.8, page 402).

void __attribute__ ((section(".after_vectors"),weak,naked))
HardFault_Handler (void)
{
  asm volatile(
      " movs r0,#4      \n"
      " mov r1,lr       \n"
      " tst r0,r1       \n"
      " beq 1f          \n"
      " mrs r0,psp      \n"
      " b   2f          \n"
      "1:               \n"
      " mrs r0,msp      \n"
      "2:"
      " mov r1,lr       \n"
      " ldr r2,=HardFault_Handler_C \n"
      " bx r2"

      : /* Outputs */
      : /* Inputs */
      : /* Clobbers */
  );
}

void __attribute__ ((section(".after_vectors"),weak,used))
HardFault_Handler_C (ExceptionStackFrame* frame __attribute__((unused)),
                     uint32_t lr __attribute__((unused)))
{
  // There is no semihosting support for Cortex-M0, since on ARMv6-M
  // faults are fatal and it is not possible to return from the handler.

#if defined(TRACE)
  trace_printf ("[HardFault]\n");
  dumpExceptionStack (frame, lr);
#endif // defined(TRACE)

#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

#endif // defined(__ARM_ARCH_6M__)


#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

void __attribute__ ((section(".after_vectors"),weak))
MemManage_Handler (void)
{
#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

void __attribute__ ((section(".after_vectors"),weak,naked))
BusFault_Handler (void)
{
  asm volatile(
      " tst lr,#4       \n"
      " ite eq          \n"
      " mrseq r0,msp    \n"
      " mrsne r0,psp    \n"
      " mov r1,lr       \n"
      " ldr r2,=BusFault_Handler_C \n"
      " bx r2"

      : /* Outputs */
      : /* Inputs */
      : /* Clobbers */
  );
}

void __attribute__ ((section(".after_vectors"),weak,used))
BusFault_Handler_C (ExceptionStackFrame* frame __attribute__((unused)),
                    uint32_t lr __attribute__((unused)))
{
#if defined(TRACE)
  uint32_t mmfar = SCB->MMFAR; // MemManage Fault Address
  uint32_t bfar = SCB->BFAR; // Bus Fault Address
  uint32_t cfsr = SCB->CFSR; // Configurable Fault Status Registers

  trace_printf ("[BusFault]\n");
  dumpExceptionStack (frame, cfsr, mmfar, bfar, lr);
#endif // defined(TRACE)

#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

void __attribute__ ((section(".after_vectors"),weak,naked))
UsageFault_Handler (void)
{
  asm volatile(
      " tst lr,#4       \n"
      " ite eq          \n"
      " mrseq r0,msp    \n"
      " mrsne r0,psp    \n"
      " mov r1,lr       \n"
      " ldr r2,=UsageFault_Handler_C \n"
      " bx r2"

      : /* Outputs */
      : /* Inputs */
      : /* Clobbers */
  );
}

void __attribute__ ((section(".after_vectors"),weak,used))
UsageFault_Handler_C (ExceptionStackFrame* frame __attribute__((unused)),
                      uint32_t lr __attribute__((unused)))
{
#if defined(TRACE)
  uint32_t mmfar = SCB->MMFAR; // MemManage Fault Address
  uint32_t bfar = SCB->BFAR; // Bus Fault Address
  uint32_t cfsr = SCB->CFSR; // Configurable Fault Status Registers
#endif

#if defined(OS_DEBUG_SEMIHOSTING_FAULTS)

  if ((cfsr & (1UL << 16)) != 0) // UNDEFINSTR
    {
      // For testing purposes, instead of BKPT use 'setend be'.
      if (isSemihosting (frame, AngelSWITestFaultOpCode))
        {
          return;
        }
    }

#endif

#if defined(TRACE)
  trace_printf ("[UsageFault]\n");
  dumpExceptionStack (frame, cfsr, mmfar, bfar, lr);
#endif // defined(TRACE)

#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

#endif

void __attribute__ ((section(".after_vectors"),weak))
SVC_Handler (void)
{
#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

void __attribute__ ((section(".after_vectors"),weak))
DebugMon_Handler (void)
{
#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

#endif

void __attribute__ ((section(".after_vectors"),weak))
PendSV_Handler (void)
{
#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

void __attribute__ ((section(".after_vectors"),weak))
SysTick_Handler (void)
{
  // DO NOT loop, just return.
  // Useful in case someone (like STM HAL) inadvertently enables SysTick.
  ;
}

// ----------------------------------------------------------------------------
