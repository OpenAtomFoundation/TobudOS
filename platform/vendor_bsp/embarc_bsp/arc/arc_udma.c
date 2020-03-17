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
 * @brief ARC DMA Controller implementation
 */

/**
 * @addtogroup ARC_HAL_MISC_UDMA
 * @{
 */
#include "arc/arc_udma.h"

/* pointer to DMA controller instance, only one dma controller is supported now */
static DMA_STATE_T *g_dmac = NULL;

#if ARC_FEATURE_DCACHE_PRESENT
#define DCACHE_FLUSH_MLINES(addr, size)    dcache_flush_mlines((uint32_t)(addr), (uint32_t)(size))
#else
#define DCACHE_FLUSH_MLINES(addr, size)
#endif
#define MEMORY_FENCE             arc_sync()

#define DMA_INT_LEVEL_SENS             0

#if DMA_MULTI_IRQ /* Multiple-Internal interrupt case */
#define DMA_INT_OK_VECTOR(channel)      ((channel) + DMA_IRQ_NUM_START)
#define DMA_INT_ERR_VECTOR(channel)     ((channel) + DMA_IRQ_NUM_START + DMA_ALL_CHANNEL_NUM)
#else /* Single-Internal interrupt case */
#define DMA_INT_OK_VECTOR(channel)      ((channel) + DMA_IRQ_NUM_START)
#define DMA_INT_ERR_VECTOR(channel)     ((channel) + DMA_IRQ_NUM_START + 1)
#endif

/*! Interrupt about DMA transaction completion
 *! Sets the the "complete" status for completed DMA transaction and
 *! starts next transaction from queue. Reset DMA completion IRQ flags
 */
static void  dmac_interrupt_completed(void *ptr);
static void  dmac_interrupt_completed_channel(uint32_t channel);

/*! Interrupt about DMA transaction completion with error
 *! Sets the the "complete with error" status for completed DMA transaction
 *! Reset the DMA channel.
 */
static void dmac_interrupt_error(void *ptr);
static void dmac_interrupt_error_channel(uint32_t channel);


/**
 * @fn void dmac_enable(void)
 */
Inline void dmac_enable(void)
{
	arc_aux_write(AUX_DMACTRL, 1);
#ifdef DMA_MEMORY_HEADER
	arc_aux_write(AUX_DMACBASE, (uint32_t) (g_dmac->mem_dma_descs));
#endif
#if CORE_DMAC_INTERNAL_VERSION > 1
	arc_aux_write(AUX_DMACBASE, (uint32_t) (g_dmac->mem_dma_ptrs));
#endif
}

/**
 * @fn void dmac_disable(void)
 */
Inline void dmac_disable(void)
{
	arc_aux_write(AUX_DMACTRL, 0);
}

/**
 * @fn void dmac_enable_channel(uint32_t channel)
 */
Inline void dmac_enable_channel(uint32_t channel)
{
	uint32_t mask = DMACHANNEL(channel);

	arc_aux_write(AUX_DMACENB, mask);
}

/**
 * @fn void dmac_enable_channels(uint32_t mask)
 */
Inline void dmac_enable_channels(uint32_t mask)
{
	arc_aux_write(AUX_DMACENB, mask);
}

/**
 * @fn void dmac_enable_all_channels(void)
 */
Inline void dmac_enable_all_channels(void)
{
	arc_aux_write(AUX_DMACENB, DMA_ALL_CHANNEL_MASK);
}

/**
 * @fn void dmac_disable_channel(uint32_t channel)
 */
Inline void dmac_disable_channel(uint32_t channel)
{
	uint32_t mask = DMACHANNEL(channel);

	arc_aux_write(AUX_DMACDSB, mask);
}

/**
 * @fn void dmac_disable_channels(uint32_t mask)
 */
Inline void dmac_disable_channels(uint32_t mask)
{
	arc_aux_write(AUX_DMACDSB, mask);
}

/**
 * @fn void dmac_disable_all_channels(void)
 */
Inline void dmac_disable_all_channels(void)
{
	arc_aux_write(AUX_DMACDSB, DMA_ALL_CHANNEL_MASK);
}

/**
 * @fn void dmac_reset_channel(uint32_t channel)
 */
Inline void dmac_reset_channel(uint32_t channel)
{
	uint32_t mask = DMACHANNEL(channel);

	arc_aux_write(AUX_DMACRST, mask);
}

/**
 * @fn void dmac_reset_channels(uint32_t mask)
 */
Inline void dmac_reset_channels(uint32_t mask)
{
	arc_aux_write(AUX_DMACRST, mask);
}

/**
 * @fn void dmac_reset_all_channels(void)
 */
Inline void dmac_reset_all_channels(void)
{
	arc_aux_write(AUX_DMACRST, DMA_ALL_CHANNEL_MASK);
}

/**
 * @fn uint32_t dmac_reset_status(void)
 */
Inline uint32_t dmac_reset_status(void)
{
	return arc_aux_read(AUX_DMACRST);
}

/**
 * @fn void dmac_set_high_pri_channels(uint32_t mask)
 */
Inline void dmac_set_high_pri_channels(uint32_t mask)
{
	arc_aux_write(AUX_DMACHPRI, mask);
}

/**
 * @fn void dmac_set_normal_pri_channels(uint32_t mask)
 */
Inline void dmac_set_normal_pri_channels(uint32_t mask)
{
	arc_aux_write(AUX_DMACNPRI, mask);
}

/**
 * @fn void dmac_start_trigger(uint32_t channel)
 */
Inline void dmac_start_trigger(uint32_t channel)
{
	arc_aux_write(AUX_DMACREQ, DMACHANNEL(channel));
}

/**
 * @fn void dmac_start_trigger_mask(uint32_t mask)
 */
Inline void dmac_start_trigger_mask(uint32_t mask)
{
	arc_aux_write(AUX_DMACREQ, mask);
}

/**
 * @fn void dmac_irq_clear(uint32_t channel)
 */
Inline void dmac_irq_clear(uint32_t channel)
{
	arc_aux_write(AUX_DMACIRQ, DMACHANNEL(channel));
}

/**
 * @fn void dmac_irq_clear_all(void)
 */
Inline void dmac_irq_clear_all(void)
{
	arc_aux_write(AUX_DMACIRQ, DMA_ALL_CHANNEL_MASK);
}

/**
 * @fn uint32_t dmac_irq_status(void)
 */
Inline uint32_t dmac_irq_status(void)
{
	return arc_aux_read(AUX_DMACIRQ);
}

/**
 * @fn uint32_t dmac_channel_status(void)
 */
Inline uint32_t dmac_channel_status(void)
{
	return arc_aux_read(AUX_DMACSTAT0);
}

/**
 * @fn uint32_t dmac_complete_status(void)
 */
Inline uint32_t dmac_complete_status(void)
{
	return arc_aux_read(AUX_DMACSTAT1);
}

/**
 * @fn void dmac_clear_error(uint32_t channel)
 */
Inline void dmac_clear_error(uint32_t channel)
{
	arc_aux_write(AUX_DMACSTAT1, DMACHANNEL(channel) << 16);
}

/**
 * @fn void dmac_clear_all_error(void)
 */
Inline void dmac_clear_all_error(void)
{
	arc_aux_write(AUX_DMACSTAT1, 0xFFFF0000);
}

static uint32_t dmac_wait_channel_status(uint32_t channel)
{
	uint32_t status = arc_aux_read(AUX_DMACSTAT1);
	uint32_t mask = DMACHANNEL(channel);
	uint32_t ret = DMA_BUSY;

	if (arc_compiler_usually(status & mask)) {
		if (arc_compiler_rarely(status & (mask << 16))) {
			ret = DMA_ERROR;
		} else {
			status = arc_aux_read(AUX_DMACSTAT0);
			if (arc_compiler_rarely(status & mask)) {
				ret = DMA_BUSY;
			} else {
				ret = DMA_IDLE;
			}
		}
	}

	return ret;
}

static uint32_t dmac_wait_mask(uint32_t mask)
{
	uint32_t status = arc_aux_read(AUX_DMACSTAT1);
	uint32_t ret = DMA_BUSY;

	if (arc_compiler_rarely(status & mask)) {
		if (arc_compiler_rarely(status & (mask << 16))) {
			ret = DMA_ERROR;
		} else {
			ret = DMA_IDLE;
		}
	}

	return ret;
}

/* calc the true size of dma area, e.g. data width: 1 byte, address inc: 2 bytes
 * ---data0---
 * ---gap0---
 * --data1---
 * --gap1---
 * case: 1 byte, address inc: 4 bytes
 * ---data0---
 * ---gap0---
 * ---gap0---
 * ---gap0---
 * ---data1---
 * ---gap1---
 * ---gap1---
 * ---gap1---
 */
static uint32_t dmac_memory_addr_gap(uint32_t dmac_mode)
{
	uint32_t size = (dmac_mode & DMACTRLx_SIZE_MASK) >> DMACTRLx_SIZE_OFS;
	uint32_t dwinc = (dmac_mode & DMACTRLx_DWINC_MASK) >> DMACTRLx_DWINC_OFS;

	switch (dwinc) {
	/* the following are cases needed to be adjusted */
	case DMA_DW1INC2:
		size <<= 1;
		break;
	case DMA_DW1INC4:
		size <<= 2;
		break;
	case DMA_DW2INC2:
		size &= ~0x1;
		break;
	case DMA_DW2INC4:
		size &= ~0x1;
		size <<= 1;
		break;
	case DMA_DW4INC4:
		size &= ~0x3;
		break;
	case DMA_DW8INC8:
		size &= ~0x7;
		break;
	case DMA_DW1INC1:
	case DMA_DWINC_CLR:
	default:
		break;
	}
	return size;
}

/* calc the real size in aux address space */
static uint32_t dmac_aux_addr_gap(uint32_t dmac_mode)
{
	uint32_t size = (dmac_mode & DMACTRLx_SIZE_MASK) >> DMACTRLx_SIZE_OFS;
	uint32_t dwinc = (dmac_mode & DMACTRLx_DWINC_MASK) >> DMACTRLx_DWINC_OFS;

	switch (dwinc) {
	case DMA_DW2INC2:
	case DMA_DW2INC4:
		size >>= 1;
		break;
	case DMA_DW4INC4:
	case DMA_DW8INC8:
		size >>= 2;
		break;
	case DMA_DW1INC1:
	case DMA_DW1INC2:
	case DMA_DW1INC4:
	case DMA_DWINC_CLR:
	default:
		break;
	}
	return size;
}

static uint32_t dmac_calc_dst_endaddr(uint32_t dst_addr, uint32_t dmac_mode)
{
	uint32_t addr = dst_addr;

	/*
	 * Destination address is not incremented
	 */
	if (arc_compiler_rarely((dmac_mode & DMACTRLx_AM(2)) == 0)) {
		return addr;
	}

	if (arc_compiler_rarely(DMACTRLx_DTT(0x1) & dmac_mode)) {       /* Destination is Auxiliary */
		addr = dst_addr + dmac_aux_addr_gap(dmac_mode);
	} else {                                                        /* Destination is Memory */
		addr = dst_addr + dmac_memory_addr_gap(dmac_mode);
	}
	return addr;
}

static uint32_t dmac_calc_src_endaddr(uint32_t src_addr, uint32_t dmac_mode)
{
	uint32_t addr = src_addr;

	/*
	 * Source address is not incremented
	 */
	if (arc_compiler_rarely((dmac_mode & DMACTRLx_AM(1)) == 0)) {
		return addr;
	}

	if (arc_compiler_rarely(DMACTRLx_DTT(0x2) & dmac_mode)) {       /* Source is Auxiliary */
		addr = src_addr + dmac_aux_addr_gap(dmac_mode);
	} else {                                                        /* Source is Memory */
		addr = src_addr + dmac_memory_addr_gap(dmac_mode);
	}

	return addr;
}

static void dmac_set_desc(DMA_DESC_T *desc, uint32_t *source, uint32_t *dest, uint32_t size,
			  uint32_t dmac_mode)
{
	uint32_t src_addr, dst_addr;

	size = DMA_CTRL_BLKSZ(size);
	dmac_mode &= ~DMACTRLx_SIZE_MASK;
	dmac_mode |= DMACTRLx_SIZE(size);

	src_addr = dmac_calc_src_endaddr((uint32_t)source, dmac_mode);
	dst_addr = dmac_calc_dst_endaddr((uint32_t)dest, dmac_mode);

	desc->DMACTRLx = dmac_mode;
	desc->DMASARx = src_addr;
	desc->DMADARx = dst_addr;
	desc->DMALLPx = 0;
}

static void dmac_fill_descriptor(uint32_t channel, DMA_DESC_T *desc)
{
	if (arc_compiler_usually(DMA_CHECK_REGISTER(channel))) {
		uint32_t channel_shift = 3 * channel;
		arc_aux_write(AUX_DMACTRL0 + channel_shift, desc->DMACTRLx);
		arc_aux_write(AUX_DMASAR0 + channel_shift, desc->DMASARx);
		arc_aux_write(AUX_DMADAR0 + channel_shift, desc->DMADARx);
#if CORE_DMAC_INTERNAL_VERSION > 1
		uint32_t *dma_llps = (uint32_t *) arc_aux_read(AUX_DMACBASE);
		dma_llps[channel] = desc->DMALLPx;
		MEMORY_FENCE;
		DCACHE_FLUSH_MLINES(&dma_llps[channel], sizeof(uint32_t));
#endif
	} else {
#if CORE_DMAC_INTERNAL_VERSION > 1
		uint32_t *dma_ptrs = (uint32_t *) arc_aux_read(AUX_DMACBASE);
		dma_ptrs[channel] = (uint32_t)desc;
		MEMORY_FENCE;
		DCACHE_FLUSH_MLINES(&dma_ptrs[channel], sizeof(uint32_t));
#else
		DMA_DESC_T *dmac_desc = (DMA_DESC_T *) arc_aux_read(AUX_DMACBASE);
		dmac_desc = (DMA_DESC_T *) (&dmac_desc[channel]);
		/* more efficient to let compiler do this copy */
		*dmac_desc = *desc;
		MEMORY_FENCE;
		DCACHE_FLUSH_MLINES(dmac_desc, sizeof(DMA_DESC_T));
#endif
	}
}

/**
 * @brief Initialize uDMA controller with a valid DMA_STATE_T structure
 *
 * If you want to use this uDMA driver, you need to call the dmac_init function
 * with a valid state, dmac will init the valid state. If initialized successfully,
 * you can use the other uDMA APIs.
 *
 * @param state DMA state structure to maintain uDMA resources, this should not be NULL
 *
 * @retval -1 State is NULL
 * @retval 0 Initialize successfully
 */
int32_t dmac_init(DMA_STATE_T *state)
{
	if (state == NULL) {
		return -1;
	}

	g_dmac = state;

	memset((void *) g_dmac, 0, sizeof(DMA_STATE_T));

	dmac_disable();
	dmac_disable_all_channels();
#if CORE_DMAC_INTERNAL_VERSION > 1
	dmac_reset_all_channels();
#endif
	dmac_enable();

#if !DMA_MULTI_IRQ
	int_level_config(DMA_INT_OK_VECTOR(0), DMA_INT_LEVEL_SENS);
	int_enable(DMA_INT_OK_VECTOR(0));
	int_handler_install(DMA_INT_OK_VECTOR(0),  dmac_interrupt_completed);
	int_pri_set(DMA_INT_OK_VECTOR(0), DMA_IRQ_PRIO);

	int_level_config(DMA_INT_ERR_VECTOR(0), DMA_INT_LEVEL_SENS);
	int_enable(DMA_INT_ERR_VECTOR(0));
	int_handler_install(DMA_INT_ERR_VECTOR(0), dmac_interrupt_error);
	int_pri_set(DMA_INT_ERR_VECTOR(0), DMA_IRQ_PRIO);
#else
	for (int32_t i = 0; i < DMA_ALL_CHANNEL_NUM; i++) {
		int_level_config(DMA_INT_OK_VECTOR(i), DMA_INT_LEVEL_SENS);
		int_enable(DMA_INT_OK_VECTOR(i));
		int_handler_install(DMA_INT_OK_VECTOR(i),  dmac_interrupt_completed);
		int_pri_set(DMA_INT_OK_VECTOR(i), DMA_IRQ_PRIO);

		int_level_config(DMA_INT_ERR_VECTOR(i), DMA_INT_LEVEL_SENS);
		int_enable(DMA_INT_ERR_VECTOR(i));
		int_handler_install(DMA_INT_ERR_VECTOR(i), dmac_interrupt_error);
		int_pri_set(DMA_INT_ERR_VECTOR(i), DMA_IRQ_PRIO);
	}

#endif
	return 0;
}

static void dmac_process_desc(DMA_DESC_T *desc, uint32_t ienable)
{
	do {
		if (arc_compiler_rarely(desc->DMALLPx)) {
			/*
			 * Force it to be auto-linked transfer when it is not
			 * DMA_MANUAL_LINKED_TRANSFER
			 */
			if (arc_compiler_usually(((desc->DMACTRLx & DMACTRLx_OP_MASK) >> DMACTRLx_OP_OFS) !=
						 DMA_MANUAL_LINKED_TRANSFER)) {
				desc->DMACTRLx &= ~DMACTRLx_OP_MASK;
				desc->DMACTRLx |= DMACTRLx_OP(DMA_AUTO_LINKED_TRANSFER);
			}
		} else {
			desc->DMACTRLx &= ~DMACTRLx_OP_MASK;
			desc->DMACTRLx |= DMACTRLx_OP(DMA_SINGLE_TRANSFER);
			/* Only forcely change the interrupt bit of the last dma descriptor */
			desc->DMACTRLx &= ~DMACTRLx_INT_MASK;
			desc->DMACTRLx |= DMACTRLx_INT(ienable);
		}
		/*
		 * TODO: Make sure all linked DMA channel descriptors
		 * stored into memory, not just in data cache
		 */
		MEMORY_FENCE;
		DCACHE_FLUSH_MLINES(desc, sizeof(DMA_DESC_T));
		desc = (DMA_DESC_T *) desc->DMALLPx;
	} while (desc != 0);
}

static int32_t dmac_valid_channel(int32_t channel, DMA_DESC_T *desc)
{
#if CORE_DMAC_INTERNAL_VERSION == 1
	if (DMA_CHECK_REGISTER(channel)) {
		if (arc_compiler_rarely(desc->DMALLPx)) {
			/** For AUX Mapped registers, Linked transfer is not supported */
			return -1;
		}
	}
#endif
	return 0;
}

static void  dmac_interrupt_completed_channel(uint32_t channel)
{
	dmac_irq_clear(channel);
	dmac_disable_channel(channel);

	if (channel >= DMA_ALL_CHANNEL_NUM) {
		return;
	}

	DMA_CHANNEL_T *dma_chn = (DMA_CHANNEL_T *) g_dmac->dma_chns[channel];
	dma_chn->status = DMA_IDLE;
	if (arc_compiler_usually(dma_chn->callback)) {
		dma_chn->callback((void *) dma_chn);
	}
}

static void dmac_interrupt_error_channel(uint32_t channel)
{
	dmac_irq_clear(channel);
	dmac_clear_error(channel);
	dmac_disable_channel(channel);

	if (channel >= DMA_ALL_CHANNEL_NUM) {
		return;
	}

	DMA_CHANNEL_T *dma_chn = (DMA_CHANNEL_T *) g_dmac->dma_chns[channel];
	dma_chn->status = DMA_ERROR;
	if (arc_compiler_usually(dma_chn->callback)) {
		dma_chn->callback((void *) dma_chn);
	}
}

static void dma_claim_channel(int32_t channel, DMA_CHANNEL_T *dma_chn,
			      uint32_t source)
{
	g_dmac->dma_chns[channel] = dma_chn;
	dma_chn->source = source;
	dma_chn->callback = NULL;
	dma_chn->status = DMA_IDLE;
	dma_chn->channel = channel;
}

#if !DMA_MULTI_IRQ
static void  dmac_interrupt_completed(void *ptr)
{
	/* In complete interrupt, the DMACIRQ and DMACSTAT1 complete bit are both set */
	uint32_t status = dmac_complete_status();

	for (int32_t channel = 0; channel < DMA_ALL_CHANNEL_NUM;
	     ++channel, status >>= 1) {
		if (arc_compiler_rarely(status & 0x1)) {
			dmac_interrupt_completed_channel(channel);
		}
	}
}

static void dmac_interrupt_error(void *ptr)
{
	/* In error interrupt, the DMACIRQ is not set, only the DMACSTAT1 error bit is set */
	uint32_t status = dmac_complete_status() >> 16;

	for (int32_t channel = 0; channel < DMA_ALL_CHANNEL_NUM;
	     ++channel, status >>= 1) {
		if (arc_compiler_rarely(status & 0x1)) {
			dmac_interrupt_error_channel(channel);
		}
	}
}
#else
static void  dmac_interrupt_completed(void *ptr)
{
	uint32_t channel;

	channel = arc_aux_read(AUX_IRQ_CAUSE) - DMA_IRQ_NUM_START;

	dmac_interrupt_completed_channel(channel);
}

static void dmac_interrupt_error(void *ptr)
{
	uint32_t channel;

	channel = arc_aux_read(AUX_IRQ_CAUSE) - DMA_IRQ_NUM_START -
		  DMA_ALL_CHANNEL_NUM;

	dmac_interrupt_error_channel(channel);
}
#endif

/**
 * @brief Close uDMA controller
 *
 * @details This function disable all DMA channels.
 */
void dmac_close(void)
{
	if (arc_compiler_rarely(!g_dmac)) {
		return;
	}

	dmac_disable_all_channels();
#if CORE_DMAC_INTERNAL_VERSION > 1
	dmac_reset_all_channels();
#endif
	dmac_disable();
}

/**
 * @brief Configure uDMA descriptor with source/destination address,
 * transfer size in bytes and ctrl mode.
 *
 * @param desc uDMA descriptor, this should not be NULL
 * @param src Source address for the uDMA transfer
 * @param dst Destination address for the uDMA transfer
 * @param size Actual transfer size in bytes
 * @param ctrl uDMA channel control value
 *
 * @retval -1 desc is NULL
 * @retval 0 OK
 * @note The DMALLPx of desc will be set to NULL in this function, and DMACTRLx
 * of desc will be set with right transfer size
 */
int32_t dmac_config_desc(DMA_DESC_T *desc, void *src, void *dst, uint32_t size,
			 DMA_CTRL_T *ctrl)
{
	if (arc_compiler_rarely(desc == NULL)) {
		return -1;
	}

	dmac_set_desc(desc, src, dst, size, ctrl->value);

	return 0;
}

/**
 * @brief Set uDMA channel linked-list pointer register, head -> next
 *
 * @param head uDMA descriptor, should not be NULL
 * @param next uDMA descriptor, could be NULL
 *
 * @retval -1 head is NULL
 * @retval 0 OK
 */
int32_t dmac_desc_add_linked(DMA_DESC_T *head, DMA_DESC_T *next)
{
	if (arc_compiler_rarely(head == NULL)) {
		return -1;
	}
	head->DMALLPx = (uint32_t) next;
	return 0;
}

/**
 * @brief Initialize a channel for DMA transfer
 *
 * @param dma_chn  Data structure containing the default configuration for the selected channel, should not be NULL
 *
 * @retval -1 dma_chn is NULL
 * @retval 0 OK
 * @note Channel will be set to DMA_CHN_INVALID
 */
int32_t dmac_init_channel(DMA_CHANNEL_T *dma_chn)
{
	if (arc_compiler_rarely(dma_chn == NULL)) {
		return -1;
	}

	dma_chn->desc = NULL;
	dma_chn->source = DMA_REQ_SOFT;
	dma_chn->callback = NULL;
	dma_chn->status = DMA_IDLE;
	dma_chn->channel = DMA_CHN_INVALID;
	return 0;
}

/**
 * @brief Configure channel for DMA transfer
 *
 * @param dma_chn Data structure containing the intended configuration for the selected channel, should not be NULL
 * @param desc uDMA descriptor
 *
 * @retval -1 dma_chn is NULL
 * @retval 0 OK
 */
int32_t dmac_config_channel(DMA_CHANNEL_T *dma_chn, DMA_DESC_T *desc)
{
	if (arc_compiler_rarely(dma_chn == NULL)) {
		return -1;
	}

	dma_chn->desc = desc;
	return 0;
}

/**
 * @brief Reserve a DMA channel, bind it with dma_chn, and set the dma
 *        trigger source
 *
 * @param channel This can be DMA_CHN_ANY or any valid
 *                channel id. For DMA_CHN_ANY, it will try
 *                to peek an available channel. For any
 *                valid channel id, it will try to reserve
 *                that channel.
 * @param dma_chn uDMA channel structure, should not be NULL
 * @param source  DMA trigger source, this can be any value in uint32_t enum
 *
 * @retval DMA_CHN_INVALID dma_chn is NULL, or channel is not a
 *                         valid one, or there is no channel available now
 * @retval 0-DMA_ALL_CHANNEL_NUM The channel id that reserved
 */
int32_t dmac_reserve_channel(int32_t channel, DMA_CHANNEL_T *dma_chn,
			     uint32_t source)
{
	if (arc_compiler_rarely(dma_chn == NULL)) {
		return DMA_CHN_INVALID;
	}

	if (arc_compiler_usually(channel == DMA_CHN_ANY)) {
		for (int32_t i = 0; i < DMA_ALL_CHANNEL_NUM; i++) {
			if (arc_compiler_usually(!g_dmac->dma_chns[i])) {
				g_dmac->dma_chns[i] = dma_chn;
				dma_claim_channel(i, dma_chn, source);
				return i;
			}
		}
		return DMA_CHN_INVALID;
	}
	if (arc_compiler_rarely((uint32_t) channel >= DMA_ALL_CHANNEL_NUM)) {
		return DMA_CHN_INVALID;
	}

	if (arc_compiler_usually(!g_dmac->dma_chns[channel])) {
		dma_claim_channel(channel, dma_chn, source);
		return channel;
	}

	return DMA_CHN_INVALID;
}

/**
 * @brief Start uDMA transfer for dma_chn, set handler function for uDAM callback and transfer
 *        priority
 *
 * @param dma_chn uDMA channel structure, should not be NULL
 * @param callback Handler function, when DMA transfer is done, it will
 *                 be called with parameter which value is dma_chn
 * @param priority  uDMA transfer priority
 *
 * @retval -1 dma_chn is NULL or dma_chn->desc is NULL or channel of
 *            dma_chn is not a valid one or dma_chn is still in transfer
 * @retval -2 When channel is a aux-based channel, the dma descriptor should
 *            not be a linked list
 * @retval 0 OK
 * @note When callback is NULL, then all INT field in DMA descriptor will be set
 * to interrupt disable, otherwise it will be set to interrupt enable
 */
int32_t dmac_start_channel(DMA_CHANNEL_T *dma_chn, DMA_CALLBACK_T callback,
			   uint32_t priority)
{
	int32_t channel;

	if (arc_compiler_rarely(dma_chn == NULL)) {
		return -1;
	}
	if (arc_compiler_rarely(dma_chn->desc == NULL)) {
		return -1;
	}

	channel = dma_chn->channel;

	if (arc_compiler_rarely(!((uint32_t) channel < DMA_ALL_CHANNEL_NUM))) {
		return -1;
	}

	if (arc_compiler_rarely(dma_chn->status == DMA_BUSY)) {
		return -1;
	}
	/** Check if aux based registers and linked transfer is not supported */

	if (arc_compiler_rarely(dmac_valid_channel(channel, dma_chn->desc) != 0)) {
		return -2;
	}

	dma_chn->callback = callback;
	dma_chn->priority = priority;
	/*
	 * When callback function is NULL, disable interrupt, use simple poll
	 * instead
	 */
	if (arc_compiler_rarely(callback)) {
		dma_chn->int_en = DMA_INT_ENABLE;
	} else {
		dma_chn->int_en = DMA_INT_DISABLE;
	}
	if (arc_compiler_usually(priority == DMA_CHN_HIGH_PRIO)) {
		dmac_set_high_pri_channels(DMACHANNEL(channel));
	} else {
		dmac_set_normal_pri_channels(DMACHANNEL(channel));
	}

	dmac_process_desc(dma_chn->desc, dma_chn->int_en);
	dmac_fill_descriptor(channel, dma_chn->desc);
	dmac_clear_error(channel);
#if CORE_DMAC_INTERNAL_VERSION > 1
	dmac_reset_channel(channel);
#endif
	dmac_disable_channel(channel);
	dmac_enable_channel(channel);
	dma_chn->status = DMA_BUSY;
	/*
	 * only trigger when software
	 */
	if (arc_compiler_usually(dma_chn->source == DMA_REQ_SOFT)) {
		dmac_start_trigger(channel);
	}
	return 0;
}

/**
 * @brief Stop uDMA transfer
 *
 * @param dma_chn  uDMA channel structure, should not be NULL
 *
 * @retval -1 dma_chn is NULL or dma_chn->desc is NULL or channel of
 *            dma_chn is not a valid one
 * @retval 0 OK
 */
int32_t dmac_stop_channel(DMA_CHANNEL_T *dma_chn)
{
	int32_t channel;

	if (arc_compiler_rarely(dma_chn == NULL)) {
		return -1;
	}
	if (arc_compiler_rarely(dma_chn->desc == NULL)) {
		return -1;
	}

	channel = dma_chn->channel;

	if (arc_compiler_rarely(!((uint32_t) channel < DMA_ALL_CHANNEL_NUM))) {
		return -1;
	}

	dmac_disable_channel(channel);
	dmac_clear_error(channel);
	if (arc_compiler_usually(dma_chn->status == DMA_BUSY)) {
		dma_chn->status = DMA_STOP;
	}
	return 0;
}

/**
 * @brief Clear channel transfer status and set it to DMA_IDLE
 *
 * @param dma_chn  uDMA channel structure, should not be NULL
 *
 * @retval -1 dma_chn is NULL or dma_chn->desc is NULL or channel of
 *            dma_chn is not a valid one
 * @retval 0 OK
 */
int32_t dmac_clear_channel(DMA_CHANNEL_T *dma_chn)
{
	int32_t channel;

	if (arc_compiler_rarely(dma_chn == NULL)) {
		return -1;
	}
	if (arc_compiler_rarely(dma_chn->desc == NULL)) {
		return -1;
	}

	channel = dma_chn->channel;

	if (arc_compiler_rarely(!((uint32_t) channel < DMA_ALL_CHANNEL_NUM))) {
		return -1;
	}

	dmac_clear_error(channel);
	dmac_disable_channel(channel);
	dmac_enable_channel(channel);
	dma_chn->status = DMA_IDLE;

	return 0;
}

/**
 * @brief Release an uDMA channel
 *
 * @param dma_chn Channel tansfer structure of uDMA channel to be released, should not be NULL
 *
 * @retval -1 dma_chn is NULL or dma_chn->desc is NULL or channel of
 *            dma_chn is not a valid one
 * @retval 0 OK
 */
int32_t dmac_release_channel(DMA_CHANNEL_T *dma_chn)
{
	int32_t channel;

	if (arc_compiler_rarely(dma_chn == NULL)) {
		return -1;
	}
	if (arc_compiler_rarely(dma_chn->desc == NULL)) {
		return -1;
	}
	channel = dma_chn->channel;

	if (arc_compiler_rarely(!((uint32_t) channel < DMA_ALL_CHANNEL_NUM))) {
		return -1;
	}

	dmac_clear_error(channel);
	dmac_disable_channel(channel);
	g_dmac->dma_chns[channel] = NULL;
	dma_chn->channel = DMA_CHN_INVALID;

	return 0;
}

/**
 * @brief Wait until uDMA channel job is completed
 *
 * @param dma_chn Channel tansfer structure of uDMA channel, should not be NULL
 *
 * @retval -1 dma_chn is NULL or dma_chn->desc is NULL or channel
 *            of dma_chn is not a valid one
 * @retval DMA_IDLE Transfer is done without error
 * @retval DMA_ERROR Transfer is done with error
 */
int32_t dmac_wait_channel(DMA_CHANNEL_T *dma_chn)
{
	int32_t channel;

	if (arc_compiler_rarely(dma_chn == NULL)) {
		return -1;
	}
	if (arc_compiler_rarely(dma_chn->desc == NULL)) {
		return -1;
	}

	channel = dma_chn->channel;

	if (arc_compiler_rarely(!((uint32_t) channel < DMA_ALL_CHANNEL_NUM))) {
		return -1;
	}

	while (dma_chn->status == DMA_BUSY) {
		if (arc_compiler_usually(dma_chn->int_en == DMA_INT_DISABLE)) {
			dma_chn->status = dmac_wait_channel_status(channel);
		}
	}
	return dma_chn->status;
}

/**
 * @brief Check channel transfer status
 *
 * @param dma_chn Channel tansfer structure of uDMA channel, should not be NULL
 *
 * @retval -1 dma_chn is NULL or dma_chn->desc is NULL or channel
 *            of dma_chn is not a valid one
 * @retval DMA_BUSY Still in transfer state
 * @retval DMA_IDLE Transfer is done without error
 * @retval DMA_ERROR Transfer is done with error
 */
int32_t dmac_check_channel(DMA_CHANNEL_T *dma_chn)
{
	int32_t channel;

	if (arc_compiler_rarely(dma_chn == NULL)) {
		return -1;
	}
	if (arc_compiler_rarely(dma_chn->desc == NULL)) {
		return -1;
	}

	channel = dma_chn->channel;

	if (arc_compiler_rarely(!((uint32_t) channel < DMA_ALL_CHANNEL_NUM))) {
		return -1;
	}

	if (arc_compiler_usually(dma_chn->int_en == DMA_INT_DISABLE)) {
		dma_chn->status = dmac_wait_channel_status(channel);
	}
	return dma_chn->status;
}

/** @} end of group ARC_HAL_MISC_UDMA */