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
#include "embARC.h"
#include "embARC_debug.h"

#define MAX_SYS_COUNTER_VALUE       (0xffffffff)

#ifndef BOARD_SYS_TIMER_HZ
#define BOARD_SYS_TIMER_HZ          (1000)
#endif

#ifndef EMBARC_BANNER_TYPE
#define EMBARC_BANNER_TYPE  1
#endif

static const char *embarc_banner =
#if EMBARC_BANNER_TYPE == 1
	"-----------------------------------------------------------	\r\n\
 ____                                _ ____			\r\n\
|  _ \\ _____      _____ _ __ ___  __| | __ ) _   _		\r\n\
| |_) / _ \\ \\ /\\ / / _ \\ '__/ _ \\/ _` |  _ \\| | | |	\r\n\
|  __/ (_) \\ V  V /  __/ | |  __/ (_| | |_) | |_| |		\r\n\
|_|   \\___/ \\_/\\_/ \\___|_|  \\___|\\__,_|____/ \\__, |	\r\n\
                                             |___/		\r\n\
                     _       _    ____   ____			\r\n\
       ___ _ __ ___ | |__   / \\  |  _ \\ / ___|		\r\n\
      / _ \\ '_ ` _ \\| '_ \\ / _ \\ | |_) | |			\r\n\
     |  __/ | | | | | |_) / ___ \\|  _ <| |___			\r\n\
      \\___|_| |_| |_|_.__/_/   \\_\\_| \\_\\\\____|		\r\n\
------------------------------------------------------------	\r\n\
";
#else
	"-----------------------------------------------------------------------------------------------\r\n\
      _/_/_/                                                                _/ _/_/_/		\r\n\
     _/    _/   _/_/   _/      _/      _/   _/_/   _/  _/_/   _/_/     _/_/_/ _/    _/ _/    _/	\r\n\
    _/_/_/   _/    _/ _/      _/      _/ _/_/_/_/ _/_/     _/_/_/_/ _/    _/ _/_/_/   _/    _/	\r\n\
   _/       _/    _/   _/  _/  _/  _/   _/       _/       _/       _/    _/ _/    _/ _/    _/	\r\n\
  _/         _/_/       _/      _/       _/_/_/ _/         _/_/_/   _/_/_/ _/_/_/     _/_/_/	\r\n\
                                                                                         _/	\r\n\
                                                                                    _/_/	\r\n\
                                         _/            _/_/      _/_/_/     _/_/_/		\r\n\
              _/_/     _/_/_/  _/_/     _/_/_/      _/    _/   _/    _/   _/			\r\n\
           _/_/_/_/   _/    _/    _/   _/    _/    _/_/_/_/   _/_/_/     _/			\r\n\
          _/         _/    _/    _/   _/    _/    _/    _/   _/    _/   _/			\r\n\
           _/_/_/   _/    _/    _/   _/_/_/      _/    _/   _/    _/     _/_/_/			\r\n\
------------------------------------------------------------------------------------------------\r\n\
";
#endif

/** board timer interrupt reset count */
static uint32_t cyc_hz_count = (BOARD_CPU_CLOCK / BOARD_SYS_TIMER_HZ);

/** board timer counter in timer interrupt */
static volatile uint64_t gl_sys_hz_cnt = 0;
/** board 1ms counter */
static volatile uint32_t gl_ms_cnt = 0;

#define HZ_COUNT_CONV(precision, base)  ((precision) / (base))

/**
 * @brief Board bare-metal timer interrupt.
 *  Interrupt frequency is based on the defined @ref BOARD_SYS_TIMER_HZ
 */
static void board_timer_isr(void *ptr)
{
	timer_int_clear(BOARD_SYS_TIMER_ID);

	board_timer_update(BOARD_SYS_TIMER_HZ);
}

/**
 * @brief Initialise bare-metal board timer and interrupt
 * @details
 * This function is called in @ref board_init, and
 * it initializes the 1-MS timer interrupt for bare-metal mode
 */
static void board_timer_init(void)
{
	if (timer_present(BOARD_SYS_TIMER_ID)) {
		int_disable(BOARD_SYS_TIMER_INTNO);                                             /* disable first then enable */
		int_handler_install(BOARD_SYS_TIMER_INTNO, board_timer_isr);
		timer_start(BOARD_SYS_TIMER_ID, TIMER_CTRL_IE | TIMER_CTRL_NH, cyc_hz_count);   /* start 1ms timer interrupt */

		int_enable(BOARD_SYS_TIMER_INTNO);
	}
}

static void platform_print_banner(void)
{
	EMBARC_PRINTF("%s\r\n", embarc_banner);
	EMBARC_PRINTF("embARC Build Time: %s, %s\r\n", __DATE__, __TIME__);
#if defined(__GNU__)
	EMBARC_PRINTF("Compiler Version: ARC GNU, %s\r\n", __VERSION__);
#else
	EMBARC_PRINTF("Compiler Version: Metaware, %s\r\n\r\n", __VERSION__);
#endif
}

EMBARC_WEAK void platform_main(void)
{
#ifdef LIB_CONSOLE
	xprintf_setup();
#endif
	platform_print_banner();
	arc_goto_main(0, NULL);
}

EMBARC_WEAK void board_main(void)
{
#if defined(__MW__)
/* Metaware toolchain C++ init */
	arc_mwdt_init();
#elif defined(__GNU__)
/* ARC GNU toolchain C++ init */

	arc_gnu_do_global_ctors_aux();
	arc_gnu_do_init_array_aux();
#endif
	/* init core level interrupt & exception management */
	exc_int_init();
	/* init cache */
	arc_cache_init();
	/* necessary board level init */
	board_init();
	/* Initialise bare-metal board timer and interrupt */
	board_timer_init();
	/* platform (e.g RTOS, baremetal)level init */
	platform_main();
#if defined(__MW__)
	arc_mwdt_fini();
#elif defined(__GNU__)
	arc_gnu_do_global_dtors_aux();
#endif
}

/**
 * @brief Update timer counter and other MS period operation
 * in cycling interrupt and must be called periodically.
 * @param precision interrupt-period precision in Hz
 */
void board_timer_update(uint32_t precision)
{
	static uint32_t sys_hz_update = 0;
	static uint32_t sys_ms_update = 0;
	uint32_t hz_conv = 0;

	/** count sys hz */
	hz_conv = HZ_COUNT_CONV(precision, BOARD_SYS_TIMER_HZ);
	sys_hz_update++;
	if (sys_hz_update >= hz_conv) {
		sys_hz_update = 0;
		gl_sys_hz_cnt++;
	}

	/** count ms */
	hz_conv = HZ_COUNT_CONV(precision, BOARD_SYS_TIMER_MS_HZ);
	sys_ms_update++;
	if (sys_ms_update >= hz_conv) {
		sys_ms_update = 0;
		gl_ms_cnt++;
	}
}

/**
 * @brief Get current timer's counter value in ticks
 * @retval Ticks count in 64 bit format
 */
uint64_t board_get_hwticks(void)
{
	uint32_t sub_ticks;
	uint64_t total_ticks;

	timer_current(TIMER_0, &sub_ticks);

	total_ticks = (uint64_t)GET_CUR_MS() * (BOARD_CPU_CLOCK / BOARD_SYS_TIMER_HZ);
	total_ticks += (uint64_t)sub_ticks;

	return total_ticks;
}

/**
 * @brief Get current passed us since timer init
 * @retval us Count in 64 bit format
 */
uint64_t board_get_cur_us(void)
{
	uint32_t sub_us;
	uint64_t total_us;

	timer_current(TIMER_0, &sub_us);

	sub_us = ((uint64_t)sub_us * 1000000) / BOARD_CPU_CLOCK;
	total_us = ((uint64_t)GET_CUR_MS()) * 1000 + (uint64_t)sub_us;

	return total_us;
}

/**
 * @brief Get current passed ms since timer init
 * @retval ms Count in 32 bit format
 */
uint32_t board_get_cur_ms(void)
{
	return gl_ms_cnt;
}

/**
 * @brief Get board timer counter in timer interrupt
 * @retval Count in 64 bit format
 */
uint64_t board_get_cur_syshz(void)
{
	return gl_sys_hz_cnt;
}

/**
 * @brief Function for delaying execution for number of milliseconds
 * @details
 * 	This function needs a 1-MS timer interrupt to work.
 * 	For bare-metal, it is implemented in this file.
 * @param ms Delay in milliseconds
 */
void board_delay_ms(uint32_t ms)
{
	uint64_t start_us, us_delayed;

	us_delayed = ((uint64_t)ms * 1000);
	start_us = board_get_cur_us();
	while ((board_get_cur_us() - start_us) < us_delayed) {
		;
	}
}
