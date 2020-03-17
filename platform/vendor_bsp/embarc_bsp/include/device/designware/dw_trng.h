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
#ifndef H_DW_TRNG
#define H_DW_TRNG

#include "device/ip_hal/dev_trng.h"
#include "arc/arc_exception.h"

#ifndef TRNG_VERSION_NIST

#define DW_TRNG_INVALID_INTNO       (DEV_INTNO_INVALID)

// DesignWare TRNG register structure
typedef volatile struct {
	uint32_t CTRL;          // (0x0)
	uint32_t STAT;          // (0x4)
	uint32_t MODE;          // (0x8)
	uint32_t SMODE;         // (0xC)
	uint32_t IE;            // (0x10)
	uint32_t ISTAT;         // (0x14)
	uint32_t COREKIT_REL;   // (0x18)
	uint32_t FEATURES;      // (0x1C)
	uint32_t RAND0;         // (0x20)
	uint32_t RAND1;         // (0x24)
	uint32_t RAND2;         // (0x28)
	uint32_t RAND3;         // (0x2C)
	uint32_t RAND4;         // (0x30)
	uint32_t RAND5;         // (0x34)
	uint32_t RAND6;         // (0x38)
	uint32_t RAND7;         // (0x3C)
	uint32_t SEED0;         // (0x40)
	uint32_t SEED1;         // (0x44)
	uint32_t SEED2;         // (0x48)
	uint32_t SEED3;         // (0x4C)
	uint32_t SEED4;         // (0x50)
	uint32_t SEED5;         // (0x54)
	uint32_t SEED6;         // (0x58)
	uint32_t SEED7;         // (0x5C)
	uint32_t AUTO_RQSTS;    // (0x60)
	uint32_t AUTO_AGE;      // (0x64)
	uint32_t BUILD_CONFIG;  // (0x68)
} DW_TRNG_REG, *DW_TRNG_REG_PTR;

typedef struct {
	DW_TRNG_REG_PTR dw_trng_regs;           /*!< dw trng register */
	uint32_t intno;                         /*!< interrupt vector number */
	INT_HANDLER_T dw_trng_int_handler;      /*!< interrupt handler */
	uint32_t int_status;                    /*!< interrupt status */
} DW_TRNG_CTRL, *DW_TRNG_CTRL_PTR;

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t dw_trng_open(DEV_TRNG_PTR trng_obj);
extern int32_t dw_trng_close(DEV_TRNG_PTR trng_obj);
extern int32_t dw_trng_control(DEV_TRNG_PTR trng_obj, uint32_t ctrl_cmd, void *param);
extern int32_t dw_trng_read(DEV_TRNG_PTR trng_obj, uint32_t *data_buf);
extern void dw_trng_isr(DEV_TRNG_PTR trng_obj, void *ptr);

#ifdef __cplusplus
}
#endif

#endif  /* TRNG_VERSION_NIST */

#endif  /* H_DW_TRNG */
