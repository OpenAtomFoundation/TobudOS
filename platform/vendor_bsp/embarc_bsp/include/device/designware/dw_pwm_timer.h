/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
#ifndef H_DW_PWM_TIMER
#define H_DW_PWM_TIMER

#include "device/ip_hal/dev_pwm_timer.h"
#include "arc/arc_exception.h"

#define DW_PWM_TIMER_CH_MAX_COUNT       8

#define DW_PWM_TIMER_INVALID_INTNO      (DEV_INTNO_INVALID)

/**
 * @name	Structure definitions for DesignWare PWM_TIMER Register
 * @brief	Contains definitions of DesignWare PWM_TIMER register structure
 * @{
 */
typedef volatile struct {
	uint32_t LOAD_COUNT;            /*!< Value to be loaded into Timer */
	uint32_t CURRENT_VALUE;         /*!< Current Value of Timer */
	uint32_t CONTROL_REG;           /*!< Control Register for Timer */
	uint32_t EOI;                   /*!< Clears the interrupt from Timer */
	uint32_t INT_STATUS;            /*!< Contains the interrupt status for Timer */
} CH_CTRL;

typedef volatile struct {
	CH_CTRL CTRL[DW_PWM_TIMER_CH_MAX_COUNT];                /*!< (0x00 ~ 0x9C) */
	uint32_t PWM_TIMERS_INT_STATUS;                         /*!< (0xA0) Contains the interrupt status of all timers in the component.*/
	uint32_t PWM_TIMERS_EOI;                                /*!< (0xA4) Read return all zeroes (0) and clears all active interrupts.*/
	uint32_t PWM_TIMERS_RAWINT_STATUS;                      /*!< (0xA8) Contains the unmasked interrupt status of all timers in the component.*/
	uint32_t PWM_TIMERS_COMP_VER;                           /*!< (0xAC) Current revision number of the DW_apb_timers component.*/
	uint32_t LOAD_COUNT2[DW_PWM_TIMER_CH_MAX_COUNT];        /*!< (0xB0 ~ 0xCC) Value to be loaded into Timer when toggle output changes from 0 to 1*/
} DW_PWM_TIMER_REG, *DW_PWM_TIMER_REG_PTR;
/** @} */

/** interrupt handler for each pwm_timer channel */
typedef struct {
	uint32_t int_ch_max_cnt;                        /*!< max channel count for each pwm_timer group */
	DEV_PWM_TIMER_HANDLER *int_ch_handler_ptr;      /*!< interrupt handler pointer */
} DW_PWM_TIMER_CH_ISR, *DW_PWM_TIMER_CH_ISR_PTR;

/**
 * @brief	Structure definitions for DesignWare PWM_TIMER control
 * @details	Structure type for DesignWare PWM_TIMER implementation
 */
typedef struct {
	uint32_t id;                            /*!< pwm_timer group id */
	DW_PWM_TIMER_REG_PTR regs;              /*!< pwm_timer port register */
	uint32_t intno;                         /*!< pwm_timer interrupt vector number */
	uint32_t ch_num;                        /*!< pwm_timer group channel count */
	uint32_t clock;                         /*!< pwm_timer clock */
	INT_HANDLER_T int_handler;              /*!< pwm_timer interrupt handler pointer */
	DW_PWM_TIMER_CH_ISR_PTR ch_isr;         /*!< pwm_timer group channel callback pointer */
	DEV_PWM_TIMER_MODE_PTR mode;            /*!< pwm_timer group channel work mode array pointer*/
} DW_PWM_TIMER_CTRL, *DW_PWM_TIMER_CTRL_PTR;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name	DesignWare PWM_TIMER Function Declaration
 * @brief	Contains declarations of designware pwm_timer functions.
 * @details	These are only used in pwm_timer object implementation source file
 * @{
 */
extern int32_t dw_pwm_timer_open(DEV_PWM_TIMER *pwm_timer_obj);
extern int32_t dw_pwm_timer_close(DEV_PWM_TIMER *pwm_timer_obj);
extern int32_t dw_pwm_timer_read(DEV_PWM_TIMER *pwm_timer_obj, uint32_t ch, uint32_t *mode, uint32_t *freq, uint32_t *dc);
extern int32_t dw_pwm_timer_write(DEV_PWM_TIMER *pwm_timer_obj, uint32_t ch, uint32_t mode, uint32_t freq, uint32_t dc);
extern int32_t dw_pwm_timer_control(DEV_PWM_TIMER *pwm_timer_obj, uint32_t ch, uint32_t cmd, void *param);
extern int32_t dw_pwm_timer_isr_handler(DEV_PWM_TIMER *pwm_timer_obj, void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* H_DW_PWM_TIMER */
/** @} */
