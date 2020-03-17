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
/**
 *
 * @file
 * @ingroup	BOARD_COMMON
 * @brief	Common board definitions used with all boards
 * @details
 * - This header file will contain board related settings for different boards.
 * - Each board configurations are put in its own header file, like emsk/emsk.h
 * - If you want to change the configuration, you need to go to related header file, e.g.
 *   if you want to change EMSK board settings, you need to go to emsk/emsk.h
 */

/**
 * @addtogroup BOARD_COMMON
 * @{
 */
#ifndef H_BOARD
#define H_BOARD
/**
 * @todo	add comments and documents to describe the macros
 * @note 	the following macros must use the same name, because
 *	they are used by middleware and other applications
 */
/** here is a sample of EMSK board resource definitions */
#ifdef BOARD_EMSK
#include "emsk/emsk.h"
#endif /* BOARD_EMSK */

/** you can add your board configuration as BOARD_EMSK defined up */

/** nsim related definition */
#ifdef BOARD_NSIM
#include "nsim.h"
#endif /* BOARD_NSIM */

#ifdef BOARD_AXS
#include "axs/axs.h"
#endif /* BOARD_AXS */

#ifdef BOARD_HSDK
#include "hsdk/hsdk.h"
#endif /* BOARD_HSDK */

#ifdef BOARD_IOTDK
#include "iotdk/iotdk.h"
#endif /* BOARD_IOTDK */

#ifdef BOARD_EMSDP
#include "emsdp/emsdp.h"
#endif  /* BOARD_EMDK */

#ifdef __cplusplus
extern "C" {
#endif

extern void board_init(void);
extern void board_timer_update(uint32_t precision);
extern void board_delay_ms(uint32_t ms);
extern uint64_t board_get_hwticks(void);
extern uint64_t board_get_cur_us(void);
extern uint64_t board_get_cur_syshz(void);
extern uint32_t board_get_cur_ms(void);
extern void platform_main(void);
extern void board_main(void);

#ifdef __cplusplus
}
#endif

#define GET_CUR_SYSHZ()         bord_get_cur_syshz()
#define GET_CUR_MS()            board_get_cur_ms()
#define GET_CUR_US()            board_get_cur_us()
#define GET_CUR_HWTICKS()       board_get_hwticks()

#endif  /* H_BOARD */

/** @} end of group BOARD_COMMON */
