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
#include "device/subsystem/i2c_slave.h"
#include "device/subsystem/i2c_priv.h"
#include "io_config.h"
#include "apexextensions.h"

/* Notes:
 *
 */
#ifdef IO_I2C_SLV0_PRESENT
#define I2C_DEV_PRESENT

static _Interrupt void i2c_slv0_err_ISR();
static _Interrupt void i2c_slv0_rx_avail_ISR();
static _Interrupt void i2c_slv0_tx_req_ISR();
static _Interrupt void i2c_slv0_stop_det_ISR();
static _Interrupt void i2c_slv0_rd_req_ISR();

/* I2C slave devices private data structures; at most 1 instance */
static i2c_info_pt i2c_handles[1] = { 0 };

#ifdef __Xdmac
static _Uncached uint32_t i2c_dmarx_descriptors[4 * 1];
static _Uncached uint32_t i2c_dmatx_descriptors[8 * 1];
#endif

#define I2C_SLV_DMA_RX_CTRL            (0xa0000015)     /* am=b10, i=b1, dw/inc=b000, dtt=b10, r=b1, op=b01 */
#define I2C_SLV_DMA_TX_CTRL            (0x6000000d)     /* am=b01, i=b1, dw/inc=b000, dtt=b01, r=b1, op=b01 */
#define I2C_SLV_DMA_CTRL_SIZE_POS      (8)
#define I2C_SLV_DMA_CTRL_XFER_POS      (21)

static i2c_info_t i2c_slave_devs[] = {
	{ .instID = 0,
	  .reg_base = AR_IO_I2C_SLV0_CON,
#ifdef IO_I2C_SLV0_DMA_RX
	  .dmarxchanid = IO_I2C_SLV0_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_I2C_SLV0_DMA_TX
	  .dmatxchanid = IO_I2C_SLV0_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .fifo_depth = IO_I2C_SLV0_FS,
	  .vector_err = IO_I2C_SLV0_INT_ERR,
	  .isr_err = i2c_slv0_err_ISR,
	  .vector_rx_avail = IO_I2C_SLV0_INT_RX_AVAIL,
	  .isr_rx_avail = i2c_slv0_rx_avail_ISR,
	  .vector_tx_req = IO_I2C_SLV0_INT_TX_REQ,
	  .isr_tx_req = i2c_slv0_tx_req_ISR,
	  .vector_stop_det = IO_I2C_SLV0_INT_STOP_DET,
	  .isr_stop_det = i2c_slv0_stop_det_ISR,
	  .vector_restart_det = IO_I2C_SLV0_INT_RESTART_DET,
	  .vector_rd_req = IO_I2C_SLV0_INT_RD_REQ,
	  .isr_rd_req = i2c_slv0_rd_req_ISR }
};
#endif

#ifdef I2C_DEV_PRESENT

#define REG_WRITE(reg, x)   _sr((unsigned)(x), (unsigned)(dev->reg_base + reg))
#define REG_READ(reg)       _lr((unsigned)(dev->reg_base + reg))
#define REG_WRITE_BITS(reg, x, y, len, pos)   REG_WRITE(reg, ((((x)          & ~((~(0xffffffff << len)) << pos)) \
							       | (((y) << pos)  &  ((~(0xffffffff << len)) << pos)))))

/* API functions */
uint32_t io_i2c_slave_open(uint32_t dev_id)
{
	i2c_info_pt dev;
	uint32_t h = 0;

	if ((0 != i2c_handles[0]) || (dev_id != 0)) {   /* device still open or non-zero id */
		return 1;
	}
	i2c_handles[0] = &i2c_slave_devs[0];
	dev = i2c_handles[0];

	/* enable clock; replicate code to make sure i2c enable is 255+ cycles later (see databook) */
	for (h = 0; h != 255; h++) {
		REG_WRITE(I2C_CLKEN, 0x1);
	}

	/* mask all interrupts */
	REG_WRITE(I2C_INTR_MASK, 0);
	/* get stop detect interrupt only when addressed */
	REG_WRITE(I2C_CON, (REG_READ(I2C_CON) | (0x1 << 7)));

	/* initialize */
	dev->handling_tx = dev->handling_rx = 0;
	dev->tx_data = dev->rx_data = NULL;

	/* set interrupt vector, mid/high priority */
	_setvecti(dev->vector_err, dev->isr_err);
	_setvecti(dev->vector_rx_avail, dev->isr_rx_avail);
	_setvecti(dev->vector_tx_req, dev->isr_tx_req);
	_setvecti(dev->vector_stop_det, dev->isr_stop_det);
	_setvecti(dev->vector_restart_det, dev->isr_stop_det);
	_setvecti(dev->vector_rd_req, dev->isr_rd_req);

#ifdef __Xdmac
	dev->dmarxdescriptor = &i2c_dmarx_descriptors[4 * 0];
	dev->dmatxdescriptor = &i2c_dmatx_descriptors[8 * 0];

	if (dev->dmarxchanid != DMA_NONE) {
		dev->dmarxdescriptor[1] = dev->reg_base + I2C_DATA_CMD;
		dev->dmarxdescriptor[3] = 0;

		_setvecti(DMAC_INT_BASE + dev->dmarxchanid, dev->isr_rx_avail);
		_setvecti(DMAC_ERR_BASE + dev->dmarxchanid, dev->isr_err);
	}

	if (dev->dmatxchanid != DMA_NONE) { /* one descriptor only */
		dev->dmatxdescriptor[2] = dev->reg_base + I2C_DATA_CMD;
		dev->dmatxdescriptor[3] = 0;

		_setvecti(DMAC_INT_BASE + dev->dmatxchanid, dev->isr_tx_req);
		_setvecti(DMAC_ERR_BASE + dev->dmatxchanid, dev->isr_err);
	}
#endif

	/* enable device */
	REG_WRITE(I2C_ENABLE, 0x1);

	/* unmask interrupt: stop-detect, rd-req, rx-over, tx-abort */
	REG_WRITE(I2C_INTR_MASK,
		  R_STOP_DET | R_RD_REQ | R_RX_OVER | R_TX_ABRT);

	return 0;
}

void io_i2c_slave_close(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];

	/* clear all interrupts */
	REG_WRITE(I2C_INTR_MASK, 0);
	REG_READ(I2C_CLR_INTR);

	/* disable */
	REG_WRITE(I2C_ENABLE, 0);

	/* reset regs */
	REG_WRITE(I2C_CON, 0x4);
	REG_WRITE(I2C_SAR, 0);
	REG_WRITE(I2C_RX_TL, 0);
	REG_WRITE(I2C_TX_TL, 0);
	REG_WRITE(I2C_SDA_HOLD, 0x1);
	REG_WRITE(I2C_SDA_SETUP, 0x64);
	REG_WRITE(I2C_FS_SPKLEN, 0x1);

	/* gate the I2C clock */
	REG_WRITE(I2C_CLKEN, 0);

	_setvecti(dev->vector_err, NULL);
	_setvecti(dev->vector_rx_avail, NULL);
	_setvecti(dev->vector_tx_req, NULL);
	_setvecti(dev->vector_stop_det, NULL);
	_setvecti(dev->vector_restart_det, NULL);
	_setvecti(dev->vector_rd_req, NULL);

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

void io_i2c_slave_read(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t cnt = 0, h = 0, val = 0, avail = 0;

	dev->rx_data = data;
	dev->rx_size = *size;
	dev->rx_count = 0;
	dev->p_rxsize = size;

#ifdef __Xdmac
	if (dev->dmarxchanid == DMA_NONE) {
#endif

	/* mask stop-condition interrupt (to prevent stop-det ISR from interrupting) */
	val = REG_READ(I2C_INTR_MASK);
	REG_WRITE(I2C_INTR_MASK, (val & ~R_STOP_DET));

	if (dev->handling_rx == 0) {
		dev->handling_rx = 1;
		dev->stop_detected = 0;

		/* unmask interrupt: rx threshold (rx-full) */
		val = REG_READ(I2C_INTR_MASK) | R_RX_FULL;
		REG_WRITE(I2C_INTR_MASK, val);
	} else {
		/* RX already in progress, due to earlier read() calls */
		if (dev->stop_detected == 0) {
			/* unmask interrupts: rx threshold (rx-full) and stop-condition */
			val = REG_READ(I2C_INTR_MASK) | R_RX_FULL;
			REG_WRITE(I2C_INTR_MASK, val);
		} else {
			/* there is data in the rx-fifo remaining: read it */
			cnt = dev->rx_size - dev->rx_count;
			avail = REG_READ(I2C_RXFLR);

			if (cnt >= avail) { /* after reading, all data processed, rx buffer not completely filled */
				cnt = avail;
				dev->handling_rx = 0;
			}
			for (h = 0; h != cnt; h++) {
				dev->rx_data[dev->rx_count++] = REG_READ(I2C_DATA_CMD);
			}
			if (dev->handling_rx == 0) {
				if (dev->rx_size != dev->rx_count) { /* read buffer NOT completely filled */
					*(dev->p_rxsize) = dev->rx_count;
				}
			}

			/* slave rx done */
			dev->rx_data = NULL;
			if (NULL != dev->rx_cb) {
				dev->rx_cb(dev_id);
			}
		}
	}

	/* unmask stop-condition interrupt again */
	val = REG_READ(I2C_INTR_MASK) | R_STOP_DET;
	REG_WRITE(I2C_INTR_MASK, val);
#ifdef __Xdmac
} else {                /* DMA: create descriptor */
	uint32_t threshold;

	dev->handling_rx = 1;
	dev->stop_detected = 0;

	if (*size != 0) {
		/* rx data available; at least one, more is uncertain */
		threshold = REG_READ(I2C_RX_TL);
		dev->dmarxdescriptor[0] =
			I2C_SLV_DMA_RX_CTRL | ((threshold + 1) <<
					       I2C_SLV_DMA_CTRL_XFER_POS) | ((*size
									      -
									      1) <<
									     I2C_SLV_DMA_CTRL_SIZE_POS);
		dev->dmarxdescriptor[2] = (uint32_t) data + (*size - 1);

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
}
#endif

}

void io_i2c_slave_write(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t cnt = 0, h = 0, val = 0, free = 0;

	dev->tx_data = data;
	dev->tx_size = *size;
	dev->tx_count = 0;
	dev->p_txsize = size;

#ifdef __Xdmac
	if (dev->dmatxchanid == DMA_NONE) {
#endif
	/* mask read request interrupt (to prevent read request ISR from interrupting) */
	val = REG_READ(I2C_INTR_MASK);
	REG_WRITE(I2C_INTR_MASK, (val & ~R_RD_REQ));

	if (dev->handling_tx == 0) {
		dev->handling_tx = 1;
		dev->rd_req_detected = 0;
	} else {
		/* TX already in progress, due to earlier write() calls */
		if (dev->rd_req_detected != 0) {
			/* there is space in the tx-fifo remaining: fill it */
			cnt = dev->tx_size - dev->tx_count;
			free = dev->fifo_depth - REG_READ(I2C_TXFLR);
			if (cnt > free) {
				cnt = free;
			}
			for (h = 0; h != cnt; h++) {
				REG_WRITE(I2C_DATA_CMD,
					  (dev->tx_data[dev->tx_count++]));
			}

			if (dev->tx_size == dev->tx_count) { /* write buffer completely emptied */
				dev->tx_data = NULL;
				if (NULL != dev->tx_cb) {
					dev->tx_cb(dev_id);
				}
			}

			/* unmask interrupt: tx-threshold (tx-empty) */
			val = REG_READ(I2C_INTR_MASK) | R_TX_EMPTY;
			REG_WRITE(I2C_INTR_MASK, val);
		}
	}

	/* unmask rd-req interrupt again */
	val = REG_READ(I2C_INTR_MASK) | R_RD_REQ;
	REG_WRITE(I2C_INTR_MASK, val);
#ifdef __Xdmac
} else {                /* DMA: create descriptor */
	uint32_t threshold;

	dev->handling_tx = 1;
	dev->rd_req_detected = 0;

	if (*size != 0) {
		/* tx data available; at least one, more is uncertain */
		threshold = REG_READ(I2C_TX_TL);
		dev->dmatxdescriptor[0] =
			I2C_SLV_DMA_TX_CTRL | ((dev->fifo_depth - threshold) <<
					       I2C_SLV_DMA_CTRL_XFER_POS) | ((*size
									      -
									      1) <<
									     I2C_SLV_DMA_CTRL_SIZE_POS);
		dev->dmatxdescriptor[1] = (uint32_t) data + (*size - 1);

		/* init dma */
		if (dev->dmatxchanid < DMAC_MEM_CHAN_CNT) {
			_dma_chan_desc(dev->dmatxchanid,
				       &(dev->dmatxdescriptor[0]));
		} else {
			_dma_chan_desc_aux(dev->dmatxchanid,
					   &(dev->dmatxdescriptor[0]));
		}
	}
}
#endif

}

void io_i2c_slave_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t data = 0, enable = 0;

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

	default:
	{
		enable = REG_READ(I2C_ENABLE);
		REG_WRITE(I2C_ENABLE, (enable & ~(0x1)));

		switch (cmd) {
		case IO_I2C_SLAVE_SET_HOLD_TIME_RX:
			data = REG_READ(I2C_SDA_HOLD);
			REG_WRITE_BITS(I2C_SDA_HOLD, data, *((uint32_t *) arg), 8,
				       16);
			break;
		case IO_I2C_SLAVE_SET_HOLD_TIME_TX:
			data = REG_READ(I2C_SDA_HOLD);
			REG_WRITE_BITS(I2C_SDA_HOLD, data, *((uint32_t *) arg), 16,
				       0);
			break;
		case IO_I2C_SLAVE_SET_10BIT_ADDR:
			data = REG_READ(I2C_CON);
			REG_WRITE_BITS(I2C_CON, data, *((uint32_t *) arg), 1, 3);
			break;

		case IO_I2C_SLAVE_SET_SETUP_TIME:
			REG_WRITE(I2C_SDA_SETUP, *((uint32_t *) arg));
			break;
		case IO_I2C_SLAVE_SET_SPKLEN:
			REG_WRITE(I2C_FS_SPKLEN, *((uint32_t *) arg));
			break;
		case IO_I2C_SLAVE_SET_ADDR:
			REG_WRITE(I2C_SAR, *((uint32_t *) arg));
			break;
		case IO_I2C_SLAVE_SET_RX_THRESHOLD:
			REG_WRITE(I2C_RX_TL, *((uint32_t *) arg));
			break;
		case IO_I2C_SLAVE_SET_TX_THRESHOLD:
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

static _Interrupt void i2c_slv0_err_ISR()
{
	uint32_t dev_id = 0;
	i2c_info_pt dev = i2c_handles[dev_id];

	if (REG_READ(I2C_CLR_RX_OVER)) {
		if (NULL != dev->err_cb) {
			dev->err_cb(0);
		}
	}

	if (REG_READ(I2C_CLR_TX_ABRT)) {
		/* must be reception of read command (rd_req) while tx-fifo non-empty: results in flush of fifo */
	}
}

static _Interrupt void i2c_slv0_rx_avail_ISR()
{
	uint32_t dev_id = 0;
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t cnt = 0, h = 0, val = 0, avail = 0;

#ifdef __Xdmac
	if (dev->dmarxchanid == DMA_NONE) {
#endif
	/* read data from fifo */
	cnt = dev->rx_size - dev->rx_count;
	avail = REG_READ(I2C_RXFLR);
	if (cnt > avail) {
		cnt = avail;
	}
	for (h = 0; h != cnt; h++) {
		dev->rx_data[dev->rx_count++] = REG_READ(I2C_DATA_CMD);
	}
	if (dev->rx_size == dev->rx_count) {    /* read buffer completely filled */
		dev->rx_data = NULL;
		if (NULL != dev->rx_cb) {
			dev->rx_cb(dev_id);
		}

		/* mask rx-avail interrupt */
		val = REG_READ(I2C_INTR_MASK) & ~R_RX_FULL;
		REG_WRITE(I2C_INTR_MASK, val);
	}
#ifdef __Xdmac
} else {
	/* DMA RX xfer done */
	_dma_chan_enable((0x1 << dev->dmarxchanid), 0);
	_dma_int_clear((0x1 << dev->dmarxchanid));

	/* call user callback function, if any */
	dev->rx_data = NULL;
	if (dev->rx_cb != NULL) {
		dev->rx_cb(dev_id);
	}
}
#endif
}

static _Interrupt void i2c_slv0_tx_req_ISR()
{
	uint32_t dev_id = 0;
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t cnt = 0, h = 0, val = 0, free = 0;

#ifdef __Xdmac
	if (dev->dmatxchanid == DMA_NONE) {
#endif
	/* write data into fifo */
	cnt = dev->tx_size - dev->tx_count;
	free = dev->fifo_depth - REG_READ(I2C_TXFLR);
	if (cnt > free) {
		cnt = free;
	}
	for (h = 0; h != cnt; h++) {
		REG_WRITE(I2C_DATA_CMD, (dev->tx_data[dev->tx_count++]));
	}
	if (dev->tx_size == dev->tx_count) {
		/* write buffer completely emptied */
		dev->tx_data = NULL;
		if (NULL != dev->tx_cb) {
			dev->tx_cb(dev_id);
		}

		/* mask tx-threshold (tx-empty) */
		val = REG_READ(I2C_INTR_MASK) & ~R_TX_EMPTY;
		REG_WRITE(I2C_INTR_MASK, val);
	}
#ifdef __Xdmac
} else {
	/* DMA TX xfer done */
	_dma_chan_enable((0x1 << dev->dmatxchanid), 0);
	_dma_int_clear((0x1 << dev->dmatxchanid));

	/* call user callback function, if any */
	dev->tx_data = NULL;
	if (dev->tx_cb != NULL) {
		dev->tx_cb(dev_id);
	}
}
#endif
}

static _Interrupt void i2c_slv0_stop_det_ISR()
{
	uint32_t dev_id = 0;
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t cnt = 0, h = 0, val = 0, avail = 0;

	if (dev->handling_rx == 1) {
		avail = REG_READ(I2C_RXFLR);

		if (dev->rx_data == NULL) {
			if (avail == 0) {       /* all data processed */
				dev->handling_rx = 0;
			} else {                /* still data in fifo to process, new data will not become available anymore */
				dev->stop_detected = 1;
			}
		} else {
#ifdef __Xdmac
			if (dev->dmarxchanid != DMA_NONE) { /* disable DMA transfer and determine how much data has been transferred */
				uint32_t ctrl;

				_dma_chan_enable((0x1 << dev->dmarxchanid), 0);
				_dma_chan_desc_get_ctrl(dev->dmarxchanid, &ctrl);
				if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
					_dma_chan_desc_get_ctrl(dev->dmarxchanid, &ctrl);
				} else {
					_dma_chan_desc_aux_get_ctrl(dev->dmarxchanid, &ctrl);
				}
				dev->rx_count =
					((dev->rx_size - 1) -
					 ((ctrl >> I2C_SLV_DMA_CTRL_SIZE_POS) & 0x1fff));
			}
#endif
			/* read remaining data from fifo */
			cnt = dev->rx_size - dev->rx_count;
			if (cnt < avail) {              /* after reading, still data in fifo to process, new data will not become available anymore */
				dev->stop_detected = 1;
			} else if (cnt == avail) {      /* after reading, all data processed */
				dev->handling_rx = 0;
			} else {                        /* after reading, all data processed, rx buffer not completely filled */
				cnt = avail;
				dev->handling_rx = 0;
			}
			for (h = 0; h != cnt; h++) {
				dev->rx_data[dev->rx_count++] = REG_READ(I2C_DATA_CMD);
			}
			if (dev->handling_rx == 0) {
				if (dev->rx_size != dev->rx_count) { /* read buffer NOT completely filled */
					*(dev->p_rxsize) = dev->rx_count;
				}
			}

			/* slave rx done */
			dev->rx_data = NULL;
			if (NULL != dev->rx_cb) {
				dev->rx_cb(dev_id);
			}
		}

		/* mask rx-avail interrupt, if not done yet */
		val = REG_READ(I2C_INTR_MASK) & ~R_RX_FULL;
		REG_WRITE(I2C_INTR_MASK, val);
	}

	if (dev->handling_tx == 1) {
		dev->handling_tx = 0;

		if (dev->tx_data != NULL) {
			/* slave tx done */
			*(dev->p_txsize) = dev->tx_count - REG_READ(I2C_TXFLR);

			dev->tx_data = NULL;
			if (NULL != dev->rx_cb) {
				dev->rx_cb(dev_id);
			}
		}

	}

	REG_READ(I2C_CLR_STOP_DET);
}

static _Interrupt void i2c_slv0_rd_req_ISR()
{
	uint32_t dev_id = 0;
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t cnt = 0, h = 0, val = 0;

	if (dev->tx_data != NULL) {
		dev->rd_req_detected = 1;

#ifdef __Xdmac
		if (dev->dmatxchanid == DMA_NONE) {
#endif
		/* write data into (empty) fifo */
		cnt = dev->tx_size - dev->tx_count;
		if (cnt > dev->fifo_depth) {
			cnt = dev->fifo_depth;
		}

		for (h = 0; h != cnt; h++) {
			REG_WRITE(I2C_DATA_CMD, (dev->tx_data[dev->tx_count++]));
		}

		if (dev->tx_size == dev->tx_count) {    /* write buffer completely emptied */
			dev->tx_data = NULL;
			if (NULL != dev->tx_cb) {
				dev->tx_cb(dev_id);
			}
		} else {
			/* unmask interrupts: tx-threshold (tx-empty) */
			val = REG_READ(I2C_INTR_MASK) | R_TX_EMPTY;
			REG_WRITE(I2C_INTR_MASK, val);
		}
#ifdef __Xdmac
	} else {        /* have DMAC fill the fifo */
		_dma_chan_enable((0x1 << dev->dmatxchanid), 1);
	}
#endif
	} else {
		/* no data available for tx --> the only thing to do is returning any data */
		REG_WRITE(I2C_DATA_CMD, 0xff);
	}

	REG_READ(I2C_CLR_RD_REQ);
}

#endif
