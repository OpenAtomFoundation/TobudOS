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
#ifndef H_DW_SDIO
#define H_DW_SDIO

#include "device/ip_hal/dev_sdio.h"
#include "arc/arc_builtin.h"

/**
 * @brief	Structure definitions for DesignWare SDIO control
 * @details	Structure type for DesignWare SDIO implementation
 */
typedef struct {
	void *reg_base;         /*!< sdio registers base */
	uint32_t ref_clk;       /* reference clk */
	uint32_t intno;         /* interrupt no */
	uint32_t fifo_depth;
	/* Variables which should be set during object implementation */
} DW_SDIO_CTRL, *DW_SDIO_CTRL_PTR;

/**
 * @fn uint32_t dw_sdio_reg_read(DW_SDIO_CTRL_PTR sdio, uint32_t reg)
 */
Inline uint32_t dw_sdio_reg_read(DW_SDIO_CTRL_PTR sdio, uint32_t reg)
{
	return arc_read_uncached_32((void *)((uint32_t)sdio->reg_base + reg));
}

/**
 * @fn void dw_sdio_reg_write(DW_SDIO_CTRL_PTR sdio, uint32_t reg, uint32_t val)
 */
Inline void dw_sdio_reg_write(DW_SDIO_CTRL_PTR sdio, uint32_t reg, uint32_t val)
{
	arc_write_uncached_32((void *)((uint32_t)sdio->reg_base + reg), val);
}

extern void dw_sdio_isr(DEV_SDIO *sdio_obj, void *ptr);
extern int32_t dw_sdio_cmd_poll(DEV_SDIO *sdio_obj, SDIO_CMD_PTR cmd, SDIO_DATA_PTR data);
extern int32_t dw_sdio_open(DEV_SDIO *sdio_obj, uint32_t card_number);
extern int32_t dw_sdio_close(DEV_SDIO *sdio_obj, uint32_t card_number);
extern int32_t dw_sdio_cd(DEV_SDIO *sdio_obj, uint32_t card_number);
extern int32_t dw_sdio_wp(DEV_SDIO *sdio_obj, uint32_t card_number);
extern int32_t dw_sdio_control(DEV_SDIO *sdio_obj, SDIO_CTRL_CMD_PTR ctrl_cmd, void *param);

#endif /* H_DW_SDIO */
