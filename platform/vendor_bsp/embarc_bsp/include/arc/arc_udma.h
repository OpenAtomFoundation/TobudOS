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
 * @ingroup ARC_HAL_MISC_UDMA
 * @brief header file for ARC uDMA Controller
 */

/**
 * @addtogroup ARC_HAL_MISC_UDMA
 * @{
 */
#ifndef H_ARC_UDMA
#define H_ARC_UDMA

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "arc/arc_builtin.h"
#include "arc/arc_cache.h"

#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

#ifndef CORE_DMAC_INTERNAL_VERSION
/** Version of the DMA controller */
#define CORE_DMAC_INTERNAL_VERSION      2
#endif

#ifndef DMA_MULTI_IRQ
/** Multiple interrupts for DMA */
#define DMA_MULTI_IRQ                   0
#endif

#ifndef DMA_IRQ_PRIO
/** DMA IRQ priority */
#define DMA_IRQ_PRIO                    (INT_PRI_MIN)
#endif

#ifndef DMA_IRQ_NUM_START
/** DMA IRQ start vector */
#define DMA_IRQ_NUM_START               20
#endif

#ifndef DMA_ALL_CHANNEL_NUM
/*!< Number of all DMA channels */
#ifdef  CORE_DMAC_CHANNELS
#define DMA_ALL_CHANNEL_NUM             CORE_DMAC_CHANNELS
#else
#define DMA_ALL_CHANNEL_NUM             2
#endif
#endif

#ifndef DMA_REGISTER_CHANNEL_NUM
/*!< Number of all aux-register based DMA channels */
#ifdef  CORE_DMAC_REGISTERS
#define DMA_REGISTER_CHANNEL_NUM        CORE_DMAC_REGISTERS
#else
#define DMA_REGISTER_CHANNEL_NUM        0
#endif
#endif

/*!< Mask of all DMA channels at the most */
#define DMA_ALL_CHANNEL_MASK            0xFFFF

#define DMACTRLx_OP_OFS     (0)                         /*!< bit field offset: bit 0,1 */
#define DMACTRLx_R_OFS      (2)                         /*!< bit field offset: bit 2 */
#define DMACTRLx_DTT_OFS    (3)                         /*!< bit field offset: bit 3,4 */
#define DMACTRLx_DWINC_OFS  (5)                         /*!< bit field offset: bit 5,6,7 */
#define DMACTRLx_SIZE_OFS   (8)                         /*!< bit field offset: bit 8..20 */
#define DMACTRLx_ARB_OFS    (21)                        /*!< bit field offset: bit 21..28 */
#define DMACTRLx_INT_OFS    (29)                        /*!< bit field offset: bit 29 */
#define DMACTRLx_AM_OFS     (30)                        /*!< bit field offset: bit 30,31 */

#define DMACTRLx_OP(x)      ((x) << DMACTRLx_OP_OFS)    /*!< bit field value: bit 0,1 */
#define DMACTRLx_R(x)       ((x) << DMACTRLx_R_OFS)     /*!< bit field value: bit 2 */
#define DMACTRLx_DTT(x)     ((x) << DMACTRLx_DTT_OFS)   /*!< bit field value: bit 3,4 */
#define DMACTRLx_DWINC(x)   ((x) << DMACTRLx_DWINC_OFS) /*!< bit field value: bit 5,6,7 */
#define DMACTRLx_SIZE(x)    ((x) << DMACTRLx_SIZE_OFS)  /*!< bit field value: bit 8..20 */
#define DMACTRLx_ARB(x)     ((x) << DMACTRLx_ARB_OFS)   /*!< bit field value: bit 21..28 */
#define DMACTRLx_INT(x)     ((x) << DMACTRLx_INT_OFS)   /*!< bit field value: bit 29 */
#define DMACTRLx_AM(x)      ((x) << DMACTRLx_AM_OFS)    /*!< bit field value: bit 30,31 */

#define DMACTRLx_OP_MASK    DMACTRLx_OP(0x3)            /*!< bit field mask: bit 0,1 */
#define DMACTRLx_R_MASK     DMACTRLx_R(0x1)             /*!< bit field mask: bit 2 */
#define DMACTRLx_DTT_MASK   DMACTRLx_DTT(0x3)           /*!< bit field mask: bit 3,4 */
#define DMACTRLx_DWINC_MASK DMACTRLx_DWINC(0x7)         /*!< bit field mask: bit 5,6,7 */
#define DMACTRLx_SIZE_MASK  DMACTRLx_SIZE(0x1FFF)       /*!< bit field mask: bit 8..20 */
#define DMACTRLx_ARB_MASK   DMACTRLx_ARB(0xFF)          /*!< bit field mask: bit 21..28 */
#define DMACTRLx_INT_MASK   DMACTRLx_INT(0x1)           /*!< bit field mask: bit 29 */
#define DMACTRLx_AM_MASK    DMACTRLx_AM(0x3)            /*!< bit field mask: bit 30,31 */

#define DMACHANNEL(x)       ((0x1 << (x)) & 0xFFFF)     /*!< Channel number to Bit */

/** Handler function for uDAM callback, param should be current dma channel transfer structure DMA_CHANNEL_T */
typedef void (*DMA_CALLBACK_T) (void *param);

typedef struct {
	uint32_t op    : 2;     /*!< operation field */
	uint32_t rt    : 1;     /*!< request type field */
	uint32_t dtt   : 2;     /*!< source and destination target types field */
	uint32_t dwinc : 3;     /*!< data width and address increment field */
	uint32_t blksz : 13;    /*!< data transfer in bytes field */
	uint32_t arb   : 8;     /*!< arbitration size */
	uint32_t intm  : 1;     /*!< interrupt mode bit */
	uint32_t am    : 2;     /*!< address mode field */
} DMA_CTRL_FIELD_T;

/** DMA channel control register union */
typedef union {
	DMA_CTRL_FIELD_T bits;  /*!< control register in bits format */
	uint32_t value;         /*!< control register in word format */
} DMA_CTRL_T;

/** DMA working status enumeration */
#define DMA_IDLE                0       /*!< Current DMA status is IDLE */
#define DMA_BUSY                1       /*!< Current DMA status is busy, in transfer */
#define DMA_ERROR               2       /*!< Current DMA status is error, in transfer error */
#define DMA_STOP                3       /*!< Current DMA is stop by user */

#define DMA_CHN_ANY             -1      /*!< Any channel, request one */
#define DMA_CHN_INVALID         -2      /*!< Invalid channel */

/** DMA channel request or trigger source enumeration */
#define DMA_REQ_SOFT            0       /*!< Software trigger */
#define DMA_REQ_PERIPHERAL      1       /*!< Peripheral trigger */

/** DMA descriptor (DMA channel registers) */
typedef struct {
	uint32_t DMACTRLx;      /*!< DMA channel control register */
	uint32_t DMASARx;       /*!< DMA channel end source address register */
	uint32_t DMADARx;       /*!< DMA channel end destination address register */
	uint32_t DMALLPx;       /*!< DMA channel link-list pointer (for chaining/linking separate DMA transfers only) */
} DMA_DESC_T;

/** DMA Channel transfer structure */
typedef struct {
	int32_t channel;                /*!< Channel ID binded to this transfer */
	int16_t priority;               /*!< Transfer priority */
	int16_t int_en;                 /*!< Interrupt enable status */
	uint32_t source;                /*!< DMA request or trigger source for this transfer */
	DMA_DESC_T *desc;               /*!< DMA transfer descriptor for this transfer */
	volatile uint32_t status;       /*!< DMA transfer status of this transfer */
	DMA_CALLBACK_T callback;        /*!< DMA transfer callback */
} DMA_CHANNEL_T;

/** DMA channel control bit field enumeration - DMA Operation (OP) */
#define DMA_INVALID_TRANSFER             0      /*!< Invalid channel */
#define DMA_SINGLE_TRANSFER              1      /*!< Single Block */
#define DMA_AUTO_LINKED_TRANSFER         2      /*!< Link-List (Auto-Request) */
#define DMA_MANUAL_LINKED_TRANSFER       3      /*!< Link-List (Manual-Request) */

/** DMA channel control bit field enumeration - Request Type (RT) */
#define DMA_AUTO_REQUEST        0       /*!< Auto-request following channel arbitration */
#define DMA_MANUAL_REQUEST      1       /*!< Manual-request following channel arbitration */

/** DMA channel control bit field enumeration - Data Transfer Type (DTT) */

#define DMA_MEM2MEM     0       /*!< Memory to Memory */
#define DMA_MEM2AUX     1       /*!< Memory to Auxiliary */
#define DMA_AUX2MEM     2       /*!< Auxiliary to Memory */
#define DMA_AUX2AUX     3       /*!< Auxiliary to Auxiliary */

/** DMA channel control bit field enumeration - Data Width/Increment (DW/INC) */

#define DMA_DW1INC1     0       /*!< dw=byte, inc=byte */
#define DMA_DW1INC2     1       /*!< dw=byte, inc=half-word */
#define DMA_DW1INC4     2       /*!< dw=byte, inc=word */
#define DMA_DW2INC2     3       /*!< dw=half-word, inc=half-word */
#define DMA_DW2INC4     4       /*!< dw=half-word, inc=word */
#define DMA_DW4INC4     5       /*!< dw=word, inc=word */
#define DMA_DWINC_CLR   6       /*!< clear mode (dw=word, inc=word) */
#define DMA_DW8INC8     7       /*!< dw=double-word, inc=double-word(Only supported in HS) */

/** DMA channel control bit field enumeration - Internal/External Interrupt enable (INT) */

#define DMA_INT_DISABLE 0       /*!< Interrupt disabled, no interrupt raised is on completion of a data transfer */
#define DMA_INT_ENABLE  1       /*!< Interrupt enabled, a level interrupt raised on completion of a data transfer */

/** DMA channel control bit field enumeration - Address update Mode (AM) */

#define DMA_AM_SRCNOT_DSTNOT     0      /*!< No Source or Destination Address increment */
#define DMA_AM_SRCINC_DSTNOT     1      /*!< Source Address incremented, Destination Address not incremented */
#define DMA_AM_SRCNOT_DSTINC     2      /*!< Source Address not incremented, Destination Address incremented */
#define DMA_AM_SRCINC_DSTINC     3      /*!< Source Address and Destination Address incremented */

/** DMA channel priority enumeration */

#define DMA_CHN_NORM_PRIO        0      /*!< Normal priority */
#define DMA_CHN_HIGH_PRIO        1      /*!< High priority */

/** Number of Memory based DMA Channel */
#define DMA_MEMORY_CHANNEL_NUM      ((DMA_ALL_CHANNEL_NUM) -(DMA_REGISTER_CHANNEL_NUM))

#if CORE_DMAC_INTERNAL_VERSION == 1
#if DMA_MEMORY_CHANNEL_NUM > 0
/** Memory based DMA channels existed */
#define DMA_MEMORY_HEADER
#endif
#endif

/** Check whether channel have register interface */
#define DMA_CHECK_REGISTER(channel) (channel >= (DMA_MEMORY_CHANNEL_NUM))

/** DMA state to maintain uDMA resources */
typedef struct {
#ifdef DMA_MEMORY_HEADER
	/*!< Memory based DMA descriptors */
	EMBARC_ALIGNED(256) volatile DMA_DESC_T mem_dma_descs[DMA_MEMORY_CHANNEL_NUM];
#endif
#if CORE_DMAC_INTERNAL_VERSION > 1
	EMBARC_ALIGNED(32) volatile uint32_t mem_dma_ptrs[DMA_ALL_CHANNEL_NUM];
#endif
	/*!< All uDMA channel resources */
	volatile DMA_CHANNEL_T *dma_chns[DMA_ALL_CHANNEL_NUM];
} DMA_STATE_T;

/** Set DMA_CTRL_T structure ctrl bit field - OP
 *
 * @param ctrl This should be DMA_CTRL_T structure, and not NULL
 * @param val Target value
 */
#define DMA_CTRL_SET_OP(ctrl, val)  (ctrl)->bits.op = val;
/** Set DMA_CTRL_T structure ctrl bit field - RT
 *
 * @param ctrl This should be DMA_CTRL_T structure, and not NULL
 * @param val Target value
 */
#define DMA_CTRL_SET_RT(ctrl, val)  (ctrl)->bits.rt = val;
/** Set DMA_CTRL_T structure ctrl bit field - DTT
 *
 * @param ctrl This should be DMA_CTRL_T structure, and not NULL
 * @param val Target value
 */
#define DMA_CTRL_SET_DTT(ctrl, val) (ctrl)->bits.dtt = val;
/** Set DMA_CTRL_T structure ctrl bit field - DW/INC
 *
 * @param ctrl This should be DMA_CTRL_T structure, and not NULL
 * @param val Target value
 */
#define DMA_CTRL_SET_DWINC(ctrl, val)   (ctrl)->bits.dwinc = val;
/** Calculate right size
 *
 * @param x transfer size
 * @retval (x-1) value after calculation
 */
#define DMA_CTRL_BLKSZ(x)       (x - 1)
/** Set DMA_CTRL_T structure ctrl bit field - BLOCKSIZE
 *
 * @param ctrl This should be DMA_CTRL_T structure, and not NULL
 * @param val Target value
 */
#define DMA_CTRL_SET_BLKSZ(ctrl, val)   (ctrl)->bits.blksz = val;
/** Set DMA_CTRL_T structure ctrl bit field - ARB
 *
 * @param ctrl This should be DMA_CTRL_T structure, and not NULL
 * @param val Target value
 */
#define DMA_CTRL_SET_ARB(ctrl, val) (ctrl)->bits.arb = val;
/** Set DMA_CTRL_T structure ctrl bit field - I
 *
 * @param ctrl This should be DMA_CTRL_T structure, and not NULL
 * @param val Target value
 */
#define DMA_CTRL_SET_INT(ctrl, val) (ctrl)->bits.intm = val;
/** Set DMA_CTRL_T structure ctrl bit field - AM
 *
 * @param ctrl This should be DMA_CTRL_T structure, and not NULL
 * @param val Target value
 */
#define DMA_CTRL_SET_AM(ctrl, val)  (ctrl)->bits.am = val;

/** Set DMA_CTRL_T structure ctrl value
 *
 * @param ctrl This should be DMA_CTRL_T structure, and not NULL
 * @param val Target value
 */
#define DMA_CTRL_SET_VALUE(ctrl, val)   (ctrl)->value = val;

extern int32_t dmac_init(DMA_STATE_T *state);
extern void dmac_close(void);
extern int32_t dmac_config_desc(DMA_DESC_T *desc, void *src, void *dst, uint32_t size, DMA_CTRL_T *ctrl);
extern int32_t dmac_desc_add_linked(DMA_DESC_T *head, DMA_DESC_T *next);
extern int32_t dmac_init_channel(DMA_CHANNEL_T *dma_chn);
extern int32_t dmac_config_channel(DMA_CHANNEL_T *dma_chn, DMA_DESC_T *desc);
extern int32_t dmac_reserve_channel(int32_t channel, DMA_CHANNEL_T *dma_chn, uint32_t source);
extern int32_t dmac_start_channel(DMA_CHANNEL_T *dma_chn, DMA_CALLBACK_T callback, uint32_t priority);
extern int32_t dmac_stop_channel(DMA_CHANNEL_T *dma_chn);
extern int32_t dmac_release_channel(DMA_CHANNEL_T *dma_chn);
extern int32_t dmac_wait_channel(DMA_CHANNEL_T *dma_chn);
extern int32_t dmac_check_channel(DMA_CHANNEL_T *dma_chn);
extern int32_t dmac_clear_channel(DMA_CHANNEL_T *dma_chn);

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

#endif  /* H_ARC_UDMA */
/** @}*/
