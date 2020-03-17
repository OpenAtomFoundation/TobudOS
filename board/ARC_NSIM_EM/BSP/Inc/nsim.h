/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
   --------------------------------------------- */
#ifndef H_NSIM
#define H_NSIM

#include "arc/arc_feature_config.h"
#include "arc/arc_em.h"
#include "arc/arc_timer.h"
#include "nsim_uart_obj.h"

/** CPU Clock Frequency definition */
#if defined(BOARD_CPU_FREQ)
/*!< Get cpu clock frequency definition from build system */
	#define CLK_CPU         (BOARD_CPU_FREQ)
#elif defined(ARC_FEATURE_CPU_CLOCK_FREQ)
/*!< Get cpu clock frequency definition from tcf file */
	#define CLK_CPU         (ARC_FEATURE_CPU_CLOCK_FREQ)
#else
/*!< Default cpu clock frequency */
	#define CLK_CPU         (1000000)
#endif

/* common macros must be defined by all boards */

#define BOARD_CONSOLE_UART_ID       NSIM_UART_0_ID
#define BOARD_CONSOLE_UART_BAUD     115200

#define BOARD_SYS_TIMER_ID          TIMER_0
#define BOARD_SYS_TIMER_INTNO       INTNO_TIMER0
#define BOARD_SYS_TIMER_HZ          (1000)

/** board timer 1ms means what HZ count */
#define BOARD_SYS_TIMER_MS_HZ       (1000)
/** board ms counter convention based on the global timer counter */
#define BOARD_SYS_TIMER_MS_CONV     (BOARD_SYS_TIMER_MS_HZ / BOARD_SYS_TIMER_HZ)

#define BOARD_OS_TIMER_ID           TIMER_0
#define BOARD_OS_TIMER_INTNO        INTNO_TIMER0

#define BOARD_CPU_CLOCK             CLK_CPU

#define button_read(x)              1
#define led_write(x, y)             EMBARC_PRINTF("led out: %x, %x\r\n", x, y)
#define BOARD_LED_MASK              0xff

#endif  /* H_NSIM */
