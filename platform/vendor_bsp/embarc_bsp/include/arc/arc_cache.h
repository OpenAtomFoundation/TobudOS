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
 * @file
 * @ingroup	ARC_HAL_MISC_CACHE
 * @brief Header file of cache module
 */

#ifndef H_ARC_CACHE
#define H_ARC_CACHE

#include "arc/arc_builtin.h"
#include "arc/arc_exception.h"

/**
 * @addtogroup	ARC_HAL_MISC_CACHE
 * @{
 */
/**
 * @name Marco definitions for cache control
 * @todo These definitions will be reviewed.
 * @{
 */
#define IC_CTRL_IC_ENABLE              (0x0)            /*!< enable instruction cache */
#define IC_CTRL_IC_DISABLE             (0x1)            /*!< disable instruction cache */
#define IC_CTRL_DIRECT_ACCESS          (0x0)            /*!< direct access mode */
#define IC_CTRL_INDIRECT_ACCESS        (0x20)           /*!< indirect access mode */
#define IC_CTRL_OP_SUCCEEDED           (0x8)            /*!< instruction cache operation succeeded */
/** @} */

/**
 * @name Marco definitions for data cache control
 * @todo These definition will be reviewed.
 * @{
 */
#define IC_CTRL_I
#define DC_CTRL_DC_ENABLE              (0x0)            /*!< enable data cache */
#define DC_CTRL_DC_DISABLE             (0x1)            /*!< disable data cache */
#define DC_CTRL_INVALID_ONLY           (0x0)            /*!< invalid data cache only */
#define DC_CTRL_INVALID_FLUSH          (0x40)           /*!< invalid and flush data cache */
#define DC_CTRL_ENABLE_FLUSH_LOCKED    (0x80)           /*!< the locked data cache can be flushed */
#define DC_CTRL_DISABLE_FLUSH_LOCKED   (0x0)            /*!< the locked data cache cannot be flushed */
#define DC_CTRL_FLUSH_STATUS           (0x100)          /*!< flush status */
#define DC_CTRL_DIRECT_ACCESS          (0x0)            /*!< direct access mode  */
#define DC_CTRL_INDIRECT_ACCESS        (0x20)           /*!< indirect access mode */
#define DC_CTRL_OP_SUCCEEDED           (0x4)            /*!< data cache operation succeeded */
/** @} */

#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name instruction cache related inline function
 * @{
 */

/**
 * @fn uint32_t icache_available(void)
 * @brief Check whether instruction cache is available,
 * @return 0 for not available, >0 for available
 */
Inline uint32_t icache_available(void)
{
	return (arc_aux_read(AUX_BCR_I_CACHE) & 0xF);
}

/**
 * @fn void icache_enable(uint32_t icache_en_mask)
 * @brief Enable instruction cache
 * @param icache_en_mask operation mask
 */
Inline void icache_enable(uint32_t icache_en_mask)
{
	if (!icache_available()) {
		return;
	}
	arc_aux_write(AUX_IC_CTRL, icache_en_mask);
}

/**
 * @fn void icache_disable(void)
 * @brief Disable instruction cache
 */
Inline void icache_disable(void)
{
	arc_aux_write(AUX_IC_CTRL, IC_CTRL_IC_DISABLE);
}

/**
 * @fn void icache_invalidate(void)
 * @brief Invalidate the entire instruction cache
 */
Inline void icache_invalidate(void)
{
	/* invalidate the entire icache */
	arc_aux_write(AUX_IC_IVIC, 0);
	arc_nop();
	arc_nop();
	arc_nop();
}

/**
 * @fn void icache_invalidate_line(uint32_t address)
 * @brief Invalidate specific cache line
 * @param address Memory address
 */
Inline void icache_invalidate_line(uint32_t address)
{
	arc_aux_write(AUX_IC_IVIL, address);
	/* the 3 nops are required by ARCv2 ISA */
	arc_nop();
	arc_nop();
	arc_nop();
}

/**
 * @fn int32_t icache_lock_line(uint32_t address)
 * @brief Lock specific cache line
 * @param address Memory address
 * @return 0, succeeded, -1, failed
 */
Inline int32_t icache_lock_line(uint32_t address)
{
	arc_aux_write(AUX_IC_LIL, address);
	if (arc_aux_read(AUX_IC_CTRL) & IC_CTRL_OP_SUCCEEDED) {
		return 0;
	} else {
		return -1;
	}
}

/**
 * @fn void icache_access_mode(uint32_t mode)
 * @brief Set icache access mode
 * @param mode access mode, 1: indirect access 0:direct access
 */
Inline void icache_access_mode(uint32_t mode)
{
	if (mode) {
		arc_aux_write(AUX_IC_CTRL, arc_aux_read(AUX_IC_CTRL) | IC_CTRL_INDIRECT_ACCESS);
	} else {
		arc_aux_write(AUX_IC_CTRL, arc_aux_read(AUX_IC_CTRL) & (~IC_CTRL_INDIRECT_ACCESS));
	}
}
/** @} */

/**
 * @name data cache related inline functions
 * @{
 */

/**
 * @fn uint32_t dcache_available(void)
 * @brief Check whether data cache is available,
 * 0 for not available, >0 for available
 */
Inline uint32_t dcache_available(void)
{
	return (arc_aux_read(AUX_BCR_D_CACHE) & 0xF);
}

/**
 * @fn void dcache_invalidate(void)
 * @brief Invalidate the entire data cache
 */
Inline void dcache_invalidate(void)
{
	uint32_t status;

	status = cpu_lock_save();
	arc_aux_write(AUX_DC_IVDC, 1);
	/* wait for flush completion */
	while (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_FLUSH_STATUS) {
		;
	}
	cpu_unlock_restore(status);
}

/**
 * @fn void dcache_invalidate_line(uint32_t address)
 * @brief Invalidate the specific cache line
 * @param address Memory address
 */
Inline void dcache_invalidate_line(uint32_t address)
{
	arc_aux_write(AUX_DC_IVDL, address);
	arc_nop();
	arc_nop();
	arc_nop();
}

/**
 * @fn void dcache_enable(uint32_t dcache_en_mask)
 * @brief Enable data cache
 * @param dcache_en_mask Operation mask
 */
Inline void dcache_enable(uint32_t dcache_en_mask)
{
	arc_aux_write(AUX_DC_CTRL, dcache_en_mask);
}

/**
 * @fn void dcache_disable(void)
 * @brief Disable data cache
 */
Inline void dcache_disable(void)
{
	arc_aux_write(AUX_DC_CTRL, DC_CTRL_DC_DISABLE);
}

/**
 * @fn void dcache_flush(void)
 * @brief Flush data cache
 */
Inline void dcache_flush(void)
{
	uint32_t status;

	status = cpu_lock_save();
	arc_aux_write(AUX_DC_FLSH, 1);
	/* wait for flush completion */
	while (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_FLUSH_STATUS) {
		;
	}
	cpu_unlock_restore(status);
}

/**
 * @fn void dcache_flush_line(uint32_t address)
 * @brief Flush the specific data cache line
 * @param address Memory address
 */
Inline void dcache_flush_line(uint32_t address)
{
	uint32_t status;

	status = cpu_lock_save();
	arc_aux_write(AUX_DC_FLDL, address);
	while (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_FLUSH_STATUS) {
		;
	}
	cpu_unlock_restore(status);
}

/**
 * @fn int32_t dcache_lock_line(uint32_t address)
 * @brief Lock the specific data cache line
 * @param address Memory address
 * @return 0, succeeded, -1, failed
 */
Inline int32_t dcache_lock_line(uint32_t address)
{
	arc_aux_write(AUX_DC_LDL, address);
	if (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_OP_SUCCEEDED) {
		return 0;
	} else {
		return -1;
	}
}

/**
 * @fn void dcache_access_mode(uint32_t mode)
 * @brief Set dcache access mode
 * @param mode Access mode, 1: indirect access 0:direct access
 */
Inline void dcache_access_mode(uint32_t mode)
{
	if (mode) {
		arc_aux_write(AUX_DC_CTRL, arc_aux_read(AUX_DC_CTRL) | DC_CTRL_INDIRECT_ACCESS);
	} else {
		arc_aux_write(AUX_DC_CTRL, arc_aux_read(AUX_DC_CTRL) & (~DC_CTRL_INDIRECT_ACCESS));
	}
}

/** @} */

/**
 * @name declarations of cache related functions
 * @{
 */
extern int32_t icache_invalidate_mlines(uint32_t start_addr, uint32_t size);
extern int32_t icache_lock_mlines(uint32_t start_addr, uint32_t size);
extern int32_t icache_direct_write(uint32_t cache_addr, uint32_t tag, uint32_t data);
extern int32_t icache_direct_read(uint32_t cache_addr, uint32_t *tag, uint32_t *data);
extern int32_t icache_indirect_read(uint32_t mem_addr, uint32_t *tag, uint32_t *data);
extern int32_t dcache_invalidate_mlines(uint32_t start_addr, uint32_t size);
extern int32_t dcache_flush_mlines(uint32_t start_addr, uint32_t size);
extern int32_t dcache_lock_mlines(uint32_t start_addr, uint32_t size);
extern int32_t dcache_direct_write(uint32_t cache_addr, uint32_t tag, uint32_t data);
extern int32_t dcache_direct_read(uint32_t cache_addr, uint32_t *tag, uint32_t *data);
extern int32_t dcache_indirect_read(uint32_t mem_addr, uint32_t *tag, uint32_t *data);
extern void arc_cache_init(void);

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

/** @} */
#endif  /* H_ARC_CACHE */
/** @} end of group ARC_HAL_MISC_CACHE */
