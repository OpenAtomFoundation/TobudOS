/* ==========================================================================
 * Synopsys DesignWare Sensor and Control IP Subsystem IO Software Driver and
 * documentation (hereinafter, "Software") is an Unsupported proprietary work
 * of Synopsys, Inc. unless otherwise expressly agreed to in writing between
 * Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */
/*==========================================================================
 * Library DW_DFSS-1.1.6
 * ========================================================================== */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "device/subsystem/i2c_master.h"
#include "device/subsystem/i2c_priv.h"
#include "io_config.h"
#include "apexextensions.h"

/* Notes:
 *
 */

#ifdef IO_I2C_MST0_PRESENT
#define I2C_DEV_PRESENT
#endif
#ifdef IO_I2C_MST1_PRESENT
#define I2C_DEV_PRESENT
#endif
#ifdef IO_I2C_MST2_PRESENT
#define I2C_DEV_PRESENT
#endif
#ifdef IO_I2C_MST3_PRESENT
#define I2C_DEV_PRESENT
#endif

#ifdef I2C_DEV_PRESENT

#define I2C_MAX_CNT (4)

static void i2c_mst_err_ISR_proc(uint32_t dev_id);
static void i2c_mst_rx_avail_ISR_proc(uint32_t dev_id);
static void i2c_mst_tx_req_ISR_proc(uint32_t dev_id);
static void i2c_mst_stop_det_ISR_proc(uint32_t dev_id);

#ifdef IO_I2C_MST0_PRESENT
static _Interrupt void i2c_mst0_err_ISR()
{
	i2c_mst_err_ISR_proc(0);
}

static _Interrupt void i2c_mst0_rx_avail_ISR()
{
	i2c_mst_rx_avail_ISR_proc(0);
}

static _Interrupt void i2c_mst0_tx_req_ISR()
{
	i2c_mst_tx_req_ISR_proc(0);
}

static _Interrupt void i2c_mst0_stop_det_ISR()
{
	i2c_mst_stop_det_ISR_proc(0);
}
#endif
#ifdef IO_I2C_MST1_PRESENT
static _Interrupt void i2c_mst1_err_ISR()
{
	i2c_mst_err_ISR_proc(1);
}

static _Interrupt void i2c_mst1_rx_avail_ISR()
{
	i2c_mst_rx_avail_ISR_proc(1);
}

static _Interrupt void i2c_mst1_tx_req_ISR()
{
	i2c_mst_tx_req_ISR_proc(1);
}

static _Interrupt void i2c_mst1_stop_det_ISR()
{
	i2c_mst_stop_det_ISR_proc(1);
}
#endif
#ifdef IO_I2C_MST2_PRESENT
static _Interrupt void i2c_mst2_err_ISR()
{
	i2c_mst_err_ISR_proc(2);
}

static _Interrupt void i2c_mst2_rx_avail_ISR()
{
	i2c_mst_rx_avail_ISR_proc(2);
}

static _Interrupt void i2c_mst2_tx_req_ISR()
{
	i2c_mst_tx_req_ISR_proc(2);
}

static _Interrupt void i2c_mst2_stop_det_ISR()
{
	i2c_mst_stop_det_ISR_proc(2);
}
#endif
#ifdef IO_I2C_MST3_PRESENT
static _Interrupt void i2c_mst3_err_ISR()
{
	i2c_mst_err_ISR_proc(3);
}

static _Interrupt void i2c_mst3_rx_avail_ISR()
{
	i2c_mst_rx_avail_ISR_proc(3);
}

static _Interrupt void i2c_mst3_tx_req_ISR()
{
	i2c_mst_tx_req_ISR_proc(3);
}

static _Interrupt void i2c_mst3_stop_det_ISR()
{
	i2c_mst_stop_det_ISR_proc(3);
}
#endif

static void fill_txfifo(i2c_info_pt dev);
static void fill_txfifo_for_rx(i2c_info_pt dev);
static void retrieve_rxfifo(i2c_info_pt dev);

#ifdef __Xdmac
static void create_dma_descriptors_rx(i2c_info_pt dev, uint8_t *dest,
				      uint32_t size, uint32_t burst);
static void create_dma_descriptors_tx(i2c_info_pt dev, uint8_t *src,
				      uint32_t size, uint32_t burst);
static void create_dma_descriptors_tx_for_rx(i2c_info_pt dev,
					     uint32_t size,
					     uint32_t burst);
#endif

/* I2C master devices private data structures */
static i2c_info_pt i2c_handles[I2C_MAX_CNT] = { 0 };

#ifdef __Xdmac
static _Uncached uint32_t i2c_dmarx_descriptors[4 * I2C_MAX_CNT];
static _Uncached uint32_t i2c_dmatx_descriptors[8 * I2C_MAX_CNT];
#endif

static i2c_info_t i2c_master_devs[] = {
#ifdef IO_I2C_MST0_PRESENT
	{ .instID = 0,
	  .reg_base = AR_IO_I2C_MST0_CON,
#ifdef IO_I2C_MST0_DMA_RX
	  .dmarxchanid = IO_I2C_MST0_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_I2C_MST0_DMA_TX
	  .dmatxchanid = IO_I2C_MST0_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .fifo_depth = IO_I2C_MST0_FS,
	  .vector_err = IO_I2C_MST0_INT_ERR,
	  .isr_err = i2c_mst0_err_ISR,
	  .vector_rx_avail = IO_I2C_MST0_INT_RX_AVAIL,
	  .isr_rx_avail = i2c_mst0_rx_avail_ISR,
	  .vector_tx_req = IO_I2C_MST0_INT_TX_REQ,
	  .isr_tx_req = i2c_mst0_tx_req_ISR,
	  .vector_stop_det = IO_I2C_MST0_INT_STOP_DET,
	  .isr_stop_det = i2c_mst0_stop_det_ISR },
#endif
#ifdef IO_I2C_MST1_PRESENT
	{ .instID = 1,
	  .reg_base = AR_IO_I2C_MST1_CON,
#ifdef IO_I2C_MST1_DMA_RX
	  .dmarxchanid = IO_I2C_MST1_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_I2C_MST1_DMA_TX
	  .dmatxchanid = IO_I2C_MST1_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .fifo_depth = IO_I2C_MST1_FS,
	  .vector_err = IO_I2C_MST1_INT_ERR,
	  .isr_err = i2c_mst1_err_ISR,
	  .vector_rx_avail = IO_I2C_MST1_INT_RX_AVAIL,
	  .isr_rx_avail = i2c_mst1_rx_avail_ISR,
	  .vector_tx_req = IO_I2C_MST1_INT_TX_REQ,
	  .isr_tx_req = i2c_mst1_tx_req_ISR,
	  .vector_stop_det = IO_I2C_MST1_INT_STOP_DET,
	  .isr_stop_det = i2c_mst1_stop_det_ISR },
#endif
#ifdef IO_I2C_MST2_PRESENT
	{ .instID = 2,
	  .reg_base = AR_IO_I2C_MST2_CON,
#ifdef IO_I2C_MST2_DMA_RX
	  .dmarxchanid = IO_I2C_MST2_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_I2C_MST2_DMA_TX
	  .dmatxchanid = IO_I2C_MST2_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .fifo_depth = IO_I2C_MST2_FS,
	  .vector_err = IO_I2C_MST2_INT_ERR,
	  .isr_err = i2c_mst2_err_ISR,
	  .vector_rx_avail = IO_I2C_MST2_INT_RX_AVAIL,
	  .isr_rx_avail = i2c_mst2_rx_avail_ISR,
	  .vector_tx_req = IO_I2C_MST2_INT_TX_REQ,
	  .isr_tx_req = i2c_mst2_tx_req_ISR,
	  .vector_stop_det = IO_I2C_MST2_INT_STOP_DET,
	  .isr_stop_det = i2c_mst2_stop_det_ISR },
#endif
#ifdef IO_I2C_MST3_PRESENT
	{ .instID = 3,
	  .reg_base = AR_IO_I2C_MST3_CON,
#ifdef IO_I2C_MST3_DMA_RX
	  .dmarxchanid = IO_I2C_MST3_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_I2C_MST3_DMA_TX
	  .dmatxchanid = IO_I2C_MST3_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .fifo_depth = IO_I2C_MST3_FS,
	  .vector_err = IO_I2C_MST3_INT_ERR,
	  .isr_err = i2c_mst3_err_ISR,
	  .vector_rx_avail = IO_I2C_MST3_INT_RX_AVAIL,
	  .isr_rx_avail = i2c_mst3_rx_avail_ISR,
	  .vector_tx_req = IO_I2C_MST3_INT_TX_REQ,
	  .isr_tx_req = i2c_mst3_tx_req_ISR,
	  .vector_stop_det = IO_I2C_MST3_INT_STOP_DET,
	  .isr_stop_det = i2c_mst3_stop_det_ISR },
#endif
	{ .instID = I2C_MAX_CNT }
};

#define REG_WRITE(reg, x)   _sr((unsigned)(x), (unsigned)(dev->reg_base + reg))
#define REG_READ(reg)       _lr((unsigned)(dev->reg_base + reg))
#define REG_WRITE_BITS(reg, x, y, len, pos)   REG_WRITE(reg, ((((x)          & ~((~(0xffffffff << len)) << pos)) \
							       | (((y) << pos)  &  ((~(0xffffffff << len)) << pos)))))

/* API functions */
uint32_t io_i2c_master_open(uint32_t dev_id)
{
	uint32_t h = 0;
	i2c_info_pt dev;

	h = 0;
	while ((i2c_master_devs[h].instID != dev_id)
	       && (i2c_master_devs[h].instID != I2C_MAX_CNT)) {
		h++;
	}
	if ((i2c_master_devs[h].instID == I2C_MAX_CNT) || (0 != i2c_handles[dev_id])) { /* dev_id not part of design, or still open */
		return 1;
	}
	i2c_handles[dev_id] = &i2c_master_devs[h];
	dev = i2c_handles[dev_id];

	/* enable clock */
	REG_WRITE(I2C_CLKEN, 0x1);

	/* mask all interrupts */
	REG_WRITE(I2C_INTR_MASK, 0);

	/* initialize */
	dev->handling_tx = dev->handling_rx = 0;
	dev->tx_data = dev->rx_data = NULL;
	dev->next_cond = I2C_STOP_CMD;

	/* set interrupt vector */
	_setvecti(dev->vector_err, dev->isr_err);
	_setvecti(dev->vector_rx_avail, dev->isr_rx_avail);
	_setvecti(dev->vector_tx_req, dev->isr_tx_req);
	_setvecti(dev->vector_stop_det, dev->isr_stop_det);

#ifdef __Xdmac
	dev->dmarxdescriptor = &i2c_dmarx_descriptors[4 * h];
	dev->dmatxdescriptor = &i2c_dmatx_descriptors[8 * h];

	if (dev->dmarxchanid != DMA_NONE) {
		dev->dmarxdescriptor[1] = dev->reg_base + I2C_DATA_CMD;
		dev->dmarxdescriptor[3] = 0;

		_setvecti(DMAC_INT_BASE + dev->dmarxchanid, dev->isr_rx_avail);
		_setvecti(DMAC_ERR_BASE + dev->dmarxchanid, dev->isr_err);
	}

	if (dev->dmatxchanid != DMA_NONE) {
		dev->dmatxdescriptor[2] = dev->dmatxdescriptor[6] =
			dev->reg_base + I2C_DATA_CMD;
		dev->dmatxdescriptor[3] = (uint32_t) &(dev->dmatxdescriptor[4]);
		dev->dmatxdescriptor[7] = 0;

		_setvecti(DMAC_INT_BASE + dev->dmatxchanid, dev->isr_tx_req);
		_setvecti(DMAC_ERR_BASE + dev->dmatxchanid, dev->isr_err);
	}
#endif

	/* enable device */
	REG_WRITE(I2C_ENABLE, 0x1);

	/* unmask error interrupt: stop-detection, tx-abort, rx-over */
	REG_WRITE(I2C_INTR_MASK, R_TX_ABRT | R_RX_OVER);

	return 0;
}

void io_i2c_master_close(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t enable = REG_READ(I2C_ENABLE);

	/* clear all interrupts */
	REG_WRITE(I2C_INTR_MASK, 0);
	REG_READ(I2C_CLR_INTR);

	/* abort (and wait) */
	REG_WRITE(I2C_ENABLE, (enable | (0x1 << 1)));
	while ((REG_READ(I2C_TX_ABRT_SOURCE) & (0x1 << 16)) == 0) {
		;
	}

	/* disable */
	REG_WRITE(I2C_ENABLE, 0);

	/* reset regs */
	REG_WRITE(I2C_CON, 0x65);
	REG_WRITE(I2C_TAR, 0x55);
	REG_WRITE(I2C_SS_SCL_HCNT, 0x30);
	REG_WRITE(I2C_SS_SCL_LCNT, 0x38);
	REG_WRITE(I2C_FS_SCL_HCNT, 0);
	REG_WRITE(I2C_FS_SCL_LCNT, 0);
	REG_WRITE(I2C_RX_TL, 0);
	REG_WRITE(I2C_TX_TL, 0);
	REG_WRITE(I2C_SDA_HOLD, 0x1);
	REG_WRITE(I2C_FS_SPKLEN, 0x1);

	/* gate the I2C clock */
	REG_WRITE(I2C_CLKEN, 0);

	_setvecti(dev->vector_err, NULL);
	_setvecti(dev->vector_rx_avail, NULL);
	_setvecti(dev->vector_tx_req, NULL);
	_setvecti(dev->vector_stop_det, NULL);

#ifdef __Xdmac
	if (dev->dmarxchanid != DMA_NONE) {
		_setvecti(DMAC_INT_BASE + dev->dmarxchanid, NULL);
		_setvecti(DMAC_ERR_BASE + dev->dmarxchanid, NULL);
		_dma_chan_reset(0x1 << dev->dmarxchanid);
	}

	if (dev->dmatxchanid != DMA_NONE) {
		_setvecti(DMAC_INT_BASE + dev->dmatxchanid, NULL);
		_setvecti(DMAC_ERR_BASE + dev->dmatxchanid, NULL);
		_dma_chan_reset(0x1 << dev->dmatxchanid);
	}
#endif

	dev->rx_cb = NULL;
	dev->tx_cb = NULL;
	dev->err_cb = NULL;

	i2c_handles[dev_id] = 0;
}

void io_i2c_master_read(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t val = 0;

	dev->rx_size = *size;
	dev->rx_count = dev->rx_req_count = 0;
	dev->rx_data = data;

	/* check whether a master-tx is in progress; if so, postpone until tx completion */
	if (dev->handling_tx == 0) {
		dev->handling_rx = 1;

#ifdef __Xdmac
		if (dev->dmarxchanid == DMA_NONE) {
#endif
		/* write first requests to fifo, assuming size > 0 */
		fill_txfifo_for_rx(dev);
		/* unmask interrupts: tx-threshold (tx-empty), rx-threshold (rx-full) */
		val = REG_READ(I2C_INTR_MASK) | R_TX_EMPTY | R_RX_FULL;
		REG_WRITE(I2C_INTR_MASK, val);
#ifdef __Xdmac
	} else {
		/* RX descriptor - to fetch data */
		if (*size != 0) {
			/* rx data available; at least one */
			create_dma_descriptors_rx(dev, data, *size,
						  (1 + REG_READ(I2C_RX_TL)));

			/* init dma */
			if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
				_dma_chan_desc(dev->dmarxchanid,
					       &(dev->dmarxdescriptor[0]));
			} else {
				_dma_chan_desc_aux(dev->dmarxchanid,
						   &(dev->dmarxdescriptor[0]));
			}

			_dma_chan_enable((0x1 << dev->dmarxchanid), 1);
		}
		/* TX descriptor - for read command and a closing read+stop command */
		if (*size != 0) {
			/* tx data available; at least one, more is uncertain */
			create_dma_descriptors_tx_for_rx(dev, *size,
							 (dev->fifo_depth -
							  REG_READ(I2C_TX_TL)));

			/* init dma */
			if (dev->dmatxchanid < DMAC_MEM_CHAN_CNT) {
				_dma_chan_desc(dev->dmatxchanid,
					       &(dev->dmatxdescriptor[0]));
			} else {
				_dma_chan_desc_aux(dev->dmatxchanid,
						   &(dev->dmatxdescriptor[0]));
			}

			_dma_chan_enable((0x1 << dev->dmatxchanid), 1);
		}
	}
#endif
	}
}

void io_i2c_master_write(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t val = 0;

	dev->tx_size = *size;
	dev->tx_count = 0;
	dev->tx_data = data;

	/* check whether a master-rx is in progress; if so, postpone until rx completion */
	if (dev->handling_rx == 0) {
		dev->handling_tx = 1;

#ifdef __Xdmac
		if (dev->dmatxchanid == DMA_NONE) {
#endif
		/* write first bytes to fifo, assuming size > 0 */
		fill_txfifo(dev);
		/* unmask interrupt: tx-threshold (tx-empty) */
		val = REG_READ(I2C_INTR_MASK) | R_TX_EMPTY;
		REG_WRITE(I2C_INTR_MASK, val);
#ifdef __Xdmac
	} else {
		/* TX descriptor - data+write command and a closing data+write+stop command */
		if (*size != 0) {
			/* tx data available; at least one, more is uncertain */
			create_dma_descriptors_tx(dev, data, *size,
						  (dev->fifo_depth -
						   REG_READ(I2C_TX_TL)));

			/* init dma */
			if (dev->dmatxchanid < DMAC_MEM_CHAN_CNT) {
				_dma_chan_desc(dev->dmatxchanid,
					       &(dev->dmatxdescriptor[0]));
			} else {
				_dma_chan_desc_aux(dev->dmatxchanid,
						   &(dev->dmatxdescriptor[0]));
			}

			_dma_chan_enable((0x1 << dev->dmatxchanid), 1);
		}
	}
#endif
	}
}

void io_i2c_master_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
	uint32_t data = 0, enable = 0;
	i2c_info_pt dev = i2c_handles[dev_id];

	switch (cmd) {
	case IO_SET_CB_RX:
		dev->rx_cb = ((io_cb_t *) arg)->cb;
		break;
	case IO_SET_CB_TX:
		dev->tx_cb = ((io_cb_t *) arg)->cb;
		break;
	case IO_SET_CB_ERR:
		dev->err_cb = ((io_cb_t *) arg)->cb;
		break;
	case IO_I2C_MASTER_SET_NEXT_COND:
		dev->next_cond = (uint16_t)*((uint32_t *) arg);
		break;

	default:
	{
		enable = REG_READ(I2C_ENABLE);
		REG_WRITE(I2C_ENABLE, (enable & ~(0x1)));

		switch (cmd) {
		case IO_I2C_MASTER_SET_HOLD_TIME_RX:
			data = REG_READ(I2C_SDA_HOLD);
			REG_WRITE_BITS(I2C_SDA_HOLD, data, *((uint32_t *) arg), 8,
				       16);
			break;
		case IO_I2C_MASTER_SET_HOLD_TIME_TX:
			data = REG_READ(I2C_SDA_HOLD);
			REG_WRITE_BITS(I2C_SDA_HOLD, data, *((uint32_t *) arg), 16,
				       0);
			break;
		case IO_I2C_MASTER_SET_SPEED:
			data = REG_READ(I2C_CON);
			REG_WRITE_BITS(I2C_CON, data, *((uint32_t *) arg), 2, 1);
			break;
		case IO_I2C_MASTER_SET_10BIT_ADDR:
			data = REG_READ(I2C_CON);
			REG_WRITE_BITS(I2C_CON, data, *((uint32_t *) arg), 1, 3);
			break;

		case IO_I2C_MASTER_SET_SPKLEN:
			REG_WRITE(I2C_FS_SPKLEN, *((uint32_t *) arg));
			break;
		case IO_I2C_MASTER_SET_TARGET_ADDR:
			REG_WRITE(I2C_TAR, *((uint32_t *) arg));
			break;
		case IO_I2C_MASTER_SET_SS_SCL_HCNT:
			REG_WRITE(I2C_SS_SCL_HCNT, *((uint32_t *) arg));
			break;
		case IO_I2C_MASTER_SET_SS_SCL_LCNT:
			REG_WRITE(I2C_SS_SCL_LCNT, *((uint32_t *) arg));
			break;
		case IO_I2C_MASTER_SET_FS_SCL_HCNT:
			REG_WRITE(I2C_FS_SCL_HCNT, *((uint32_t *) arg));
			break;
		case IO_I2C_MASTER_SET_FS_SCL_LCNT:
			REG_WRITE(I2C_FS_SCL_LCNT, *((uint32_t *) arg));
			break;
		case IO_I2C_MASTER_SET_RX_THRESHOLD:
			REG_WRITE(I2C_RX_TL, *((uint32_t *) arg));
			break;
		case IO_I2C_MASTER_SET_TX_THRESHOLD:
			REG_WRITE(I2C_TX_TL, *((uint32_t *) arg));
			break;

		default:
			break;
		}

		while ((0x1 & REG_READ(I2C_ENABLE_STATUS)) != 0) {
			;
		}
		enable = REG_READ(I2C_ENABLE);
		REG_WRITE(I2C_ENABLE, (enable | 0x1));

		break;
	}
	}

}

static void i2c_mst_err_ISR_proc(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];

	if (REG_READ(I2C_CLR_TX_ABRT)) {
		if (NULL != dev->err_cb) {
			dev->err_cb(dev_id);
		}
	}

	if (REG_READ(I2C_CLR_RX_OVER)) {
		if (NULL != dev->err_cb) {
			dev->err_cb(dev_id);
		}
	}
}

static void i2c_mst_rx_avail_ISR_proc(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t val = 0;

#ifdef __Xdmac
	if (dev->dmarxchanid == DMA_NONE) {
#endif
	/* read data from fifo */
	retrieve_rxfifo(dev);
	if (dev->rx_size == dev->rx_count) {    /* read buffer completely filled, mask rx-avail interrupt */
		val = REG_READ(I2C_INTR_MASK) & ~R_RX_FULL;
		REG_WRITE(I2C_INTR_MASK, val);

		dev->handling_rx = 0;
		dev->rx_data = NULL;
		if (NULL != dev->rx_cb) {
			dev->rx_cb(dev_id);
		}
	}
#ifdef __Xdmac
} else {
	/* DMA RX xfer done */
	_dma_chan_enable((0x1 << dev->dmarxchanid), 0);
	_dma_int_clear((0x1 << dev->dmarxchanid));
}
#endif
}

static void i2c_mst_tx_req_ISR_proc(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t val = 0;

	if (dev->handling_tx == 1) {
#ifdef __Xdmac
		if (dev->dmatxchanid == DMA_NONE) {
#endif
		if (dev->tx_size == dev->tx_count) {    /* no data left to put into the fifo, mask tx-threshold (tx-empty) */
			val = REG_READ(I2C_INTR_MASK) & ~R_TX_EMPTY;
			REG_WRITE(I2C_INTR_MASK, val);
			/* tx done */
			dev->handling_tx = 0;
			dev->tx_data = NULL;
			if (NULL != dev->tx_cb) {
				dev->tx_cb(dev_id);
			}
		} else {        /* write data into fifo */
			fill_txfifo(dev);
		}
#ifdef __Xdmac
	} else {
		/* DMA TX xfer done */
		_dma_chan_enable((0x1 << dev->dmatxchanid), 0);
		_dma_int_clear((0x1 << dev->dmatxchanid));
	}
#endif
	}

	if (dev->handling_rx == 1) {
#ifdef __Xdmac
		if (dev->dmatxchanid == DMA_NONE) {
#endif
		if (dev->rx_size == dev->rx_req_count) {    /* no data left to put into the fifo, mask tx-threshold (tx-empty) */
			val = REG_READ(I2C_INTR_MASK) & ~R_TX_EMPTY;
			REG_WRITE(I2C_INTR_MASK, val);
		} else {        /* write data into fifo */
			fill_txfifo_for_rx(dev);
		}
#ifdef __Xdmac
	} else {
		/* DMA TX xfer done */
		dev->rx_req_count = dev->rx_size;

		_dma_chan_enable((0x1 << dev->dmatxchanid), 0);
		_dma_int_clear((0x1 << dev->dmatxchanid));
	}
#endif
	}

}

static void i2c_mst_stop_det_ISR_proc(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t val = 0;

	if (dev->handling_tx == 1) {
		/* tx done */
		dev->handling_tx = 0;

		dev->tx_data = NULL;
		if (NULL != dev->tx_cb) {
			dev->tx_cb(dev_id);
		}

		/* handle pending rx, if any */
		if (dev->rx_data != NULL) {
			dev->handling_rx = 1;

#ifdef __Xdmac
			if (dev->dmarxchanid == DMA_NONE) {
#endif
			/* write first requests to fifo, assuming size > 0 */
			fill_txfifo_for_rx(dev);
			/* unmask interrupts: tx-threshold (tx-empty), rx-threshold (rx-full) */
			val = REG_READ(I2C_INTR_MASK) | R_TX_EMPTY | R_RX_FULL;
			REG_WRITE(I2C_INTR_MASK, val);
#ifdef __Xdmac
		} else {
			/* RX descriptor - to fetch data */
			if (dev->rx_size != 0) {
				/* rx data available; at least one */
				create_dma_descriptors_rx(dev, dev->rx_data,
							  dev->rx_size,
							  (1 + REG_READ(I2C_RX_TL)));

				/* init dma */
				if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
					_dma_chan_desc(dev->dmarxchanid,
						       &(dev->dmarxdescriptor[0]));
				} else {
					_dma_chan_desc_aux(dev->dmarxchanid,
							   &(dev->dmarxdescriptor[0]));
				}

				_dma_chan_enable((0x1 << dev->dmarxchanid), 1);
			}
			/* TX descriptor - for read command and a closing read+stop command */
			if (dev->rx_size != 0) {
				/* tx data available; at least one, more is uncertain */
				create_dma_descriptors_tx_for_rx(dev, dev->rx_size,
								 (dev->fifo_depth -
								  REG_READ
									  (I2C_TX_TL)));

				/* init dma */
				if (dev->dmatxchanid < DMAC_MEM_CHAN_CNT) {
					_dma_chan_desc(dev->dmatxchanid,
						       &(dev->dmatxdescriptor[0]));
				} else {
					_dma_chan_desc_aux(dev->dmatxchanid,
							   &(dev->dmatxdescriptor[0]));
				}

				_dma_chan_enable((0x1 << dev->dmatxchanid), 1);
			}
		}
#endif
		} else {    /* done, prevent threshold interrupts */
			val = REG_READ(I2C_INTR_MASK) & ~R_TX_EMPTY & ~R_RX_FULL;
			REG_WRITE(I2C_INTR_MASK, val);
		}
	} else if (dev->handling_rx == 1) {
#ifdef __Xdmac
		if (dev->dmarxchanid == DMA_NONE) {
#endif
		/* read remaining data from fifo */
		retrieve_rxfifo(dev);
#ifdef __Xdmac
	}
#endif

		/* rx done */
		dev->handling_rx = 0;

		dev->rx_data = NULL;
		if (NULL != dev->rx_cb) {
			dev->rx_cb(dev_id);
		}

		/* handle pending tx, if any */
		if (dev->tx_data != NULL) {
			dev->handling_tx = 1;

#ifdef __Xdmac
			if (dev->dmatxchanid == DMA_NONE) {
#endif
			/* write first bytes to fifo, assuming size > 0 */
			fill_txfifo(dev);
			/* unmask interrupt: tx-threshold (tx-empty) */
			val = REG_READ(I2C_INTR_MASK) | R_TX_EMPTY;
			REG_WRITE(I2C_INTR_MASK, val);
#ifdef __Xdmac
		} else {
			/* TX descriptor - data+write command and a closing data+write+stop command */
			if (dev->tx_size != 0) {
				/* tx data available; at least one, more is uncertain */
				create_dma_descriptors_tx(dev, dev->tx_data,
							  dev->tx_size,
							  (dev->fifo_depth -
							   REG_READ(I2C_TX_TL)));

				/* init dma */
				if (dev->dmatxchanid < DMAC_MEM_CHAN_CNT) {
					_dma_chan_desc(dev->dmatxchanid,
						       &(dev->dmatxdescriptor[0]));
				} else {
					_dma_chan_desc_aux(dev->dmatxchanid,
							   &(dev->dmatxdescriptor[0]));
				}

				_dma_chan_enable((0x1 << dev->dmatxchanid), 1);
			}
		}
#endif
		} else {    /* done, prevent threshold interrupts */
			val = REG_READ(I2C_INTR_MASK) & ~R_TX_EMPTY & ~R_RX_FULL;
			REG_WRITE(I2C_INTR_MASK, val);
		}
	}

	REG_READ(I2C_CLR_STOP_DET);
}

static void fill_txfifo(i2c_info_pt dev)
{
	uint32_t h = 0, free = 0, cnt = 0;

	cnt = dev->tx_size - dev->tx_count;
	free = dev->fifo_depth - REG_READ(I2C_TXFLR);
	if (cnt > free) {
		cnt = free;
	}
	for (h = 0; h != (cnt - 1); h++) {
		REG_WRITE(I2C_DATA_CMD, (dev->tx_data[dev->tx_count++]));
	}
	if ((dev->tx_size - 1) == dev->tx_count) {  /* end of message, insert stop condition */
		REG_WRITE(I2C_DATA_CMD,
			  (dev->tx_data[dev->tx_count++] | dev->next_cond));
	} else {            /* continue */
		REG_WRITE(I2C_DATA_CMD, (dev->tx_data[dev->tx_count++]));
	}
}

static void fill_txfifo_for_rx(i2c_info_pt dev)
{
	uint32_t h = 0, free = 0, cnt = 0;

	cnt = dev->rx_size - dev->rx_req_count;
	free = dev->fifo_depth - REG_READ(I2C_TXFLR);
	if (cnt > free) {
		cnt = free;
	}
	for (h = 0; h != (cnt - 1); h++) {
		REG_WRITE(I2C_DATA_CMD, I2C_READ_CMD);
	}
	dev->rx_req_count += cnt;
	if (dev->rx_size == dev->rx_req_count) {        /* end of message, insert stop condition */
		REG_WRITE(I2C_DATA_CMD, (I2C_READ_CMD | dev->next_cond));
	} else {                                        /* continue */
		REG_WRITE(I2C_DATA_CMD, I2C_READ_CMD);
	}
}

static void retrieve_rxfifo(i2c_info_pt dev)
{
	uint32_t h = 0, avail = 0, cnt = 0;

	cnt = dev->rx_size - dev->rx_count;
	avail = REG_READ(I2C_RXFLR);
	if (cnt > avail) {
		cnt = avail;
	}
	for (h = 0; h != cnt; h++) {
		dev->rx_data[dev->rx_count++] = REG_READ(I2C_DATA_CMD);
	}
}

#ifdef __Xdmac
#define     I2C_MST_DMA_RX_CTRL            (0xa0000015) /* am=b10, i=b1, dw/inc=b000, dtt=b10, r=b1, op=b01 */
#define     I2C_MST_DMA_TX_FOR_RX_CTRL     (0x0000006f) /* am=b00, i=b0, dw/inc=b011, dtt=b01, r=b1, op=b11 */
#define     I2C_MST_DMA_TX_END_CTRL        (0x2000006d) /* am=b00, i=b1, dw/inc=b011, dtt=b01, r=b1, op=b01 */
#define     I2C_MST_DMA_TX_CTRL            (0x4000000f) /* am=b01, i=b0, dw/inc=b000, dtt=b01, r=b1, op=b11 */
#define     I2C_MST_DMA_CTRL_SIZE_POS      (8)
#define     I2C_MST_DMA_CTRL_XFER_POS      (21)

static uint16_t readCommand = I2C_READ_CMD;
static uint16_t readstopCommand = I2C_READ_CMD | I2C_STOP_CMD;

static void create_dma_descriptors_rx(i2c_info_pt dev, uint8_t *dest,
				      uint32_t size, uint32_t burst)
{
	dev->dmarxdescriptor[0] =
		I2C_MST_DMA_RX_CTRL | (burst << I2C_MST_DMA_CTRL_XFER_POS) |
		((size - 1) << I2C_MST_DMA_CTRL_SIZE_POS);
	dev->dmarxdescriptor[2] = (uint32_t) dest + (size - 1);
}

static void create_dma_descriptors_tx(i2c_info_pt dev, uint8_t *src,
				      uint32_t size, uint32_t burst)
{
	if (size > 1) {
		dev->dmatxdescriptor[0] =
			I2C_MST_DMA_TX_CTRL | (burst << I2C_MST_DMA_CTRL_XFER_POS) |
			((size - 2) << I2C_MST_DMA_CTRL_SIZE_POS);
		dev->dmatxdescriptor[1] = (uint32_t) src + (size - 2);

		dev->dmatx_last = src[size - 1] | I2C_STOP_CMD;
		dev->dmatxdescriptor[4 + 0] =
			I2C_MST_DMA_TX_END_CTRL | (burst << I2C_MST_DMA_CTRL_XFER_POS)
			| (1 << I2C_MST_DMA_CTRL_SIZE_POS);
		dev->dmatxdescriptor[4 + 1] = (uint32_t) &(dev->dmatx_last);
	} else {            /* size == 1 */
		dev->dmatx_last = src[0] | I2C_STOP_CMD;
		dev->dmatxdescriptor[0] =
			I2C_MST_DMA_TX_END_CTRL | (burst << I2C_MST_DMA_CTRL_XFER_POS)
			| (1 << I2C_MST_DMA_CTRL_SIZE_POS);
		dev->dmatxdescriptor[1] = (uint32_t) &(dev->dmatx_last);
	}
}

static void create_dma_descriptors_tx_for_rx(i2c_info_pt dev,
					     uint32_t size, uint32_t burst)
{
	if (size > 1) {
		dev->dmatxdescriptor[0] =
			I2C_MST_DMA_TX_FOR_RX_CTRL | (burst <<
						      I2C_MST_DMA_CTRL_XFER_POS) |
			(((size - 1) * 2 - 1) << I2C_MST_DMA_CTRL_SIZE_POS);
		dev->dmatxdescriptor[1] = (uint32_t) &readCommand;

		dev->dmatxdescriptor[4 + 0] =
			I2C_MST_DMA_TX_END_CTRL | (burst << I2C_MST_DMA_CTRL_XFER_POS)
			| (1 << I2C_MST_DMA_CTRL_SIZE_POS);
		dev->dmatxdescriptor[4 + 1] = (uint32_t) &readstopCommand;
	} else {            /* size == 1 */
		dev->dmatxdescriptor[0] =
			I2C_MST_DMA_TX_END_CTRL | (burst << I2C_MST_DMA_CTRL_XFER_POS)
			| (1 << I2C_MST_DMA_CTRL_SIZE_POS);
		dev->dmatxdescriptor[1] = (uint32_t) &readstopCommand;
	}
}

#endif

#endif
