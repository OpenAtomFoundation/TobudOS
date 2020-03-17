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
#ifndef H_DW_NIST_TRNG
#define H_DW_NIST_TRNG

#include "device/ip_hal/dev_trng.h"
#include "arc/arc_exception.h"

#ifdef TRNG_VERSION_NIST

#define DW_TRNG_GINT_DISABLE            (0)
#define DW_TRNG_GINT_ENABLE             (1 << 31)
#define DW_TRNG_INT_ZEROIZED_ENABLE     (1 << 0)
#define DW_TRNG_INT_KAT_ENABLE          (1 << 1)
#define DW_TRNG_INT_NOISE_RDY_ENABLE    (1 << 2)
#define DW_TRNG_INT_ALARMS_ENABLE       (1 << 3)
#define DW_TRNG_INT_DONE_ENABLE         (1 << 4)
#define DW_TRNG_INT_ALL                 (0x8000001F)
#define DW_TRNG_ISTATE_ALL              (0x1F)

#define DW_TRNG_INVALID_INTNO       (DEV_INTNO_INVALID)

// DesignWare NIST TRNG register structure
typedef volatile struct dw_nist_trng_reg {
	uint32_t CTRL;          // (0x0)
	uint32_t MODE;          // (0x4)
	union {
		uint32_t SMODE; // (0x8)
		struct {
			uint32_t nonce : 1;
			uint32_t secure_en : 1;
			uint32_t max_rejecets : 8;
			uint32_t rsvd : 22;
		} smode;
	};
	union {
		uint32_t STAT;      // (0xC)
		struct {
			uint32_t last_cmd : 4;
			uint32_t sec_alg : 1;
			uint32_t nonce_mode : 1;
			uint32_t secure : 1;
			uint32_t drbg_state : 2;
			uint32_t rsvd : 22;
			uint32_t busy : 1;
		} stat;
	};
	union {
		uint32_t IE;        // (0x10)
		struct {
			uint32_t zeroized : 1;
			uint32_t kat_completed : 1;
			uint32_t noise_rdy : 1;
			uint32_t alarms : 1;
			uint32_t done : 1;
			uint32_t rsvd : 26;
			uint32_t glbl : 1;
		} ie;
	};
	union {
		uint32_t ISTAT;     // (0x14)
		struct {
			uint32_t zeroized : 1;
			uint32_t kat_completed : 1;
			uint32_t noise_rdy : 1;
			uint32_t alarms : 1;
			uint32_t done : 1;
			uint32_t rsvd : 27;
		} istat;
	};
	union {
		uint32_t ALARMS;    // (0x18)
		struct {
			uint32_t failed_test_id : 4;
			uint32_t illegal_cmd_seq : 1;
			uint32_t rsvd : 27;
		} alarms;
	};
	uint32_t COREKIT_REL;   // (0x1C)
	uint32_t FEATURES;      // (0x20)
	uint32_t RAND0;         // (0x24)
	uint32_t RAND1;         // (0x28)
	uint32_t RAND2;         // (0x2C)
	uint32_t RAND3;         // (0x30)
	uint32_t NPA_DATA0;     // (0x34)
	uint32_t NPA_DATA1;     // (0x38)
	uint32_t NPA_DATA2;     // (0x3C)
	uint32_t NPA_DATA3;     // (0x40)
	uint32_t NPA_DATA4;     // (0x44)
	uint32_t NPA_DATA5;     // (0x48)
	uint32_t NPA_DATA6;     // (0x4C)
	uint32_t NPA_DATA7;     // (0x50)
	uint32_t NPA_DATA8;     // (0x54)
	uint32_t NPA_DATA9;     // (0x58)
	uint32_t NPA_DATA10;    // (0x5C)
	uint32_t NPA_DATA11;    // (0x60)
	uint32_t NPA_DATA12;    // (0x64)
	uint32_t NPA_DATA13;    // (0x68)
	uint32_t NPA_DATA14;    // (0x6C)
	uint32_t NPA_DATA15;    // (0x70)
	uint32_t SEED0;         // (0x74)
	uint32_t SEED1;         // (0x78)
	uint32_t SEED2;         // (0x7C)
	uint32_t SEED3;         // (0x80)
	uint32_t SEED4;         // (0x84)
	uint32_t SEED5;         // (0x88)
	uint32_t SEED6;         // (0x8C)
	uint32_t SEED7;         // (0x90)
	uint32_t SEED8;         // (0x94)
	uint32_t SEED9;         // (0x98)
	uint32_t SEED10;        // (0x9C)
	uint32_t SEED11;        // (0xA0)
	uint32_t IA_RDATA;      // (0xA4)
	uint32_t IA_WDATA;      // (0xA8)
	uint32_t IA_ADDR;       // (0xAC)
	uint32_t IA_CMD;        // (0xB0)
} DW_TRNG_REG, *DW_TRNG_REG_PTR;

typedef struct dw_trng_ctrl {
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

#endif  /* H_DW_NIST_TRNG */
