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

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "device/subsystem/i2s_tx_master.h"
#include "io_config.h"
#include "apexextensions.h"

/* I2S Tx Master device registers */
#define I2S_IER         0x000
#define I2S_ITER        0x008
#define I2S_CER         0x00c
#define I2S_CCR         0x010
#define I2S_TXFFR       0x018
#define I2S_LTHR        0x020
#define I2S_RTHR        0x024
#define I2S_TER         0x02C
#define I2S_TCR         0x034   // Transmit configuration register
#define I2S_ISR         0x038   // Interrupt status register
#define I2S_IMR         0x03c   // Interrupt mask register
#define I2S_TOR         0x044
#define I2S_TFCR        0x04c
#define I2S_TFF         0x054
#define I2S_TXDMA       0x1c8

/* I2S Tx Master device specific macros */
#define I2S_TX_MASTER_DMA_CTRL              (0x6000006F)    /* am=b01, i=b1, dw/inc=b011, dtt=b01, r=b1, op=b11 */
#define I2S_TX_MASTER_DMA_CTRL_SIZE_POS     (8)
#define I2S_TX_MASTER_DMA_CTRL_XFER_POS     (21)

#define DMA_NONE                            (0xff)

typedef _Interrupt void (*ISR) ();

typedef struct i2s_tx_master_info_struct {
	/* I2S peripheral configuration values */
	const uint32_t reg_base;                // base address of device register set
	const uint8_t instID;                   // I2S module instance ID
	const uint16_t fifo_size;               // FIFO depth
	const uint16_t dmachanid;               // DMA channel ID
	/* I2S data buffer related fields */
	uint32_t xfr_len;                       // number of transferred samples in current buffer
	uint32_t xfr_started;                   // I2S transfer started flag   (used for nonDMA xfer)
	uint32_t *buffer[2];                    // two pointers to the user data buffers
	uint32_t *bufsize[2];                   // two pointers to the user data buffer sizes
	volatile uint32_t sys_cnt;              // read access buffer counter  (incremented by ISR)
	volatile uint32_t usr_cnt;              // write access buffer counter (incremented from write service)
#ifdef __Xdmac
	_Uncached uint32_t *dmadescriptor;      // pointer to DMA descriptors location
#endif
	/* User callbacks */
	IO_CB_FUNC tx_cb;               // user' TX completion callback
	IO_CB_FUNC err_cb;              // user' error callback
	/* Interrupt numbers and handlers */
	const uint8_t tx_vector;        // TX interrupt vector number
	const uint8_t err_vector;       // Error interrupt vector number
	ISR tx_isr;                     // I2S data available ISRs
	ISR err_isr;                    // I2S FIFO overrun ISRs
} i2s_tx_master_info_t, *i2s_tx_master_info_pt;

#ifdef IO_I2S_TX_MST0_PRESENT
#define I2S_TX_MASTER_DEV_PRESENT
#endif
#ifdef IO_I2S_TX_MST1_PRESENT
#define I2S_TX_MASTER_DEV_PRESENT
#endif
#ifdef IO_I2S_TX_MST2_PRESENT
#define I2S_TX_MASTER_DEV_PRESENT
#endif
#ifdef IO_I2S_TX_MST3_PRESENT
#define I2S_TX_MASTER_DEV_PRESENT
#endif
#ifdef IO_I2S_TX_MST4_PRESENT
#define I2S_TX_MASTER_DEV_PRESENT
#endif

#ifdef I2S_TX_MASTER_DEV_PRESENT

#define I2S_TX_MASTER_MAX_CNT   (4)

static void i2s_tx_master_tx_ISR_proc(uint32_t dev_id);
static void i2s_tx_master_err_ISR_proc(uint32_t dev_id);

#ifdef IO_I2S_TX_MST0_PRESENT
static _Interrupt void i2s_tx_master0_tx_ISR()
{
	i2s_tx_master_tx_ISR_proc(0);
}

static _Interrupt void i2s_tx_master0_err_ISR()
{
	i2s_tx_master_err_ISR_proc(0);
}
#endif
#ifdef IO_I2S_TX_MST1_PRESENT
static _Interrupt void i2s_tx_master1_tx_ISR()
{
	i2s_tx_master_tx_ISR_proc(1);
}

static _Interrupt void i2s_tx_master1_err_ISR()
{
	i2s_tx_master_err_ISR_proc(1);
}
#endif
#ifdef IO_I2S_TX_MST2_PRESENT
static _Interrupt void i2s_tx_master2_tx_ISR()
{
	i2s_tx_master_tx_ISR_proc(2);
}

static _Interrupt void i2s_tx_master2_err_ISR()
{
	i2s_tx_master_err_ISR_proc(2);
}
#endif
#ifdef IO_I2S_TX_MST3_PRESENT

static _Interrupt void i2s_tx_master3_tx_ISR()
{
	i2s_tx_master_tx_ISR_proc(3);
}

static _Interrupt void i2s_tx_master3_err_ISR()
{
	i2s_tx_master_err_ISR_proc(3);
}
#endif

/* I2S TX Master devices private data structures */
static i2s_tx_master_info_pt i2s_tx_master_handles[I2S_TX_MASTER_MAX_CNT] =
{ 0 };
#ifdef __Xdmac
static __attribute__ ((aligned(32)))
_Uncached uint32_t i2s_tx_master_dma_descriptors[8 *
						 I2S_TX_MASTER_MAX_CNT];
#endif

static i2s_tx_master_info_t i2s_tx_master_devs[] = {
#ifdef IO_I2S_TX_MST0_PRESENT
	{ .instID = 0,
	  .reg_base = AR_IO_I2S_TX_MST0_IER,
	  .tx_vector = IO_I2S_TX_MST0_INT_TX_REQ,
	  .err_vector = IO_I2S_TX_MST0_INT_ERR,
	  .tx_isr = i2s_tx_master0_tx_ISR,
	  .err_isr = i2s_tx_master0_err_ISR,
#ifdef IO_I2S_TX_MST0_DMA
	  .dmachanid = IO_I2S_TX_MST0_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifo_size = IO_I2S_TX_MST0_FS },
#endif
#ifdef IO_I2S_TX_MST1_PRESENT
	{ .instID = 1,
	  .reg_base = AR_IO_I2S_TX_MST1_IER,
	  .tx_vector = IO_I2S_TX_MST1_INT_TX_REQ,
	  .err_vector = IO_I2S_TX_MST1_INT_ERR,
	  .tx_isr = i2s_tx_master1_tx_ISR,
	  .err_isr = i2s_tx_master1_err_ISR,
#ifdef IO_I2S_TX_MST1_DMA
	  .dmachanid = IO_I2S_TX_MST1_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifo_size = IO_I2S_TX_MST1_FS },
#endif
#ifdef IO_I2S_TX_MST2_PRESENT
	{ .instID = 2,
	  .reg_base = AR_IO_I2S_TX_MST2_IER,
	  .tx_vector = IO_I2S_TX_MST2_INT_TX_REQ,
	  .err_vector = IO_I2S_TX_MST2_INT_ERR,
	  .tx_isr = i2s_tx_master2_tx_ISR,
	  .err_isr = i2s_tx_master2_err_ISR,
#ifdef IO_I2S_TX_MST2_DMA
	  .dmachanid = IO_I2S_TX_MST2_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifo_size = IO_I2S_TX_MST2_FS },
#endif
#ifdef IO_I2S_TX_MST3_PRESENT
	{ .instID = 3,
	  .reg_base = AR_IO_I2S_TX_MST3_IER,
	  .tx_vector = IO_I2S_TX_MST3_INT_TX_REQ,
	  .err_vector = IO_I2S_TX_MST3_INT_ERR,
	  .tx_isr = i2s_tx_master3_tx_ISR,
	  .err_isr = i2s_tx_master3_err_ISR,
#ifdef IO_I2S_TX_MST3_DMA
	  .dmachanid = IO_I2S_TX_MST3_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifo_size = IO_I2S_TX_MST3_FS },
#endif
	{ .instID = I2S_TX_MASTER_MAX_CNT }
};

#define REG_WRITE(reg, x)   _sr((uint32_t)((x)), (uint32_t)(dev->reg_base + reg))
#define REG_READ(reg)       _lr((uint32_t)(dev->reg_base + (reg)))

/* API functions */
uint32_t io_i2s_tx_master_open(uint32_t dev_id)
{
	i2s_tx_master_info_pt dev;
	uint32_t h = 0;

	/* check device descriptor availability */
	while ((i2s_tx_master_devs[h].instID != dev_id)
	       && (i2s_tx_master_devs[h].instID != I2S_TX_MASTER_MAX_CNT)) {
		h++;
	}
	if ((i2s_tx_master_devs[h].instID == I2S_TX_MASTER_MAX_CNT) || (0 != i2s_tx_master_handles[dev_id])) {  /* dev_id not part of design, or still open */
		return 1;
	}
	i2s_tx_master_handles[dev_id] = &i2s_tx_master_devs[h];
	dev = i2s_tx_master_handles[dev_id];

	dev->xfr_started = 0;                   // clear transfer started flag
	dev->xfr_len = 0;                       // clear transfer lenght
	dev->sys_cnt = dev->usr_cnt = 0;        // reset buffer' access counters

#ifdef __Xdmac
	/* initialize DMA descriptors */
	dev->dmadescriptor = &i2s_tx_master_dma_descriptors[8 * h];

	if (dev->dmachanid != DMA_NONE) {
		dev->dmadescriptor[0] = dev->dmadescriptor[4] = 0;
		dev->dmadescriptor[1] = dev->dmadescriptor[5] = 0;
		dev->dmadescriptor[2] = dev->dmadescriptor[6] =
			dev->reg_base + I2S_TXDMA;
		dev->dmadescriptor[3] = (uint32_t) &(dev->dmadescriptor[4]);
		dev->dmadescriptor[7] = (uint32_t) &(dev->dmadescriptor[0]);

		/* assign DMA interrupt vectors */
		_setvecti(dev->err_vector, dev->err_isr);

		_setvecti(DMAC_INT_BASE + dev->dmachanid, dev->tx_isr);
		_setvecti(DMAC_ERR_BASE + dev->dmachanid, dev->err_isr);
	} else {
#endif
	_setvecti(dev->tx_vector, dev->tx_isr);
	_setvecti(dev->err_vector, dev->err_isr);
#ifdef __Xdmac
}
#endif

	REG_WRITE(I2S_IER, 0x01);   // I2S: enable device

	return (0);
}

void io_i2s_tx_master_close(uint32_t dev_id)
{
	i2s_tx_master_info_pt dev = i2s_tx_master_handles[dev_id];

	/* put I2S registers into "after reset" state */
	REG_WRITE(I2S_IMR, 0x30);       // mask TX interrupts
	REG_WRITE(I2S_TER, 0x00);       // disable channel
	REG_WRITE(I2S_TFCR, 0x00);      // reset threshold to default (0)
	REG_WRITE(I2S_TCR, 0x02);       // reset wlen to default (2)
	REG_WRITE(I2S_TER, 0x01);       // reset channel enable to default (1)
	REG_WRITE(I2S_CER, 0x00);       // disable clk
	REG_WRITE(I2S_ITER, 0x00);      // disable receive block
	REG_WRITE(I2S_IER, 0x00);       // disable device and flushes fifo

#ifdef __Xdmac
	/* reset DMA channel */
	_dma_chan_enable((0x1 << dev->dmachanid), 0);   // aux-DMA channel reset bug fix
	_dma_chan_reset(0x1 << dev->dmachanid);
	/* deinitialize DMA interrupt handlers */
	_setvecti(DMAC_INT_BASE + dev->dmachanid, NULL);
	_setvecti(DMAC_ERR_BASE + dev->dmachanid, NULL);
#endif
	/* deinitialize non DMA interrupt handlers */
	_setvecti(dev->tx_vector, NULL);
	_setvecti(dev->err_vector, NULL);

	/* clear driver internal variables */
	dev->tx_cb = NULL;
	dev->err_cb = NULL;
	dev->xfr_started = 0;

	/* reset device handler */
	i2s_tx_master_handles[dev_id] = 0;
}

void io_i2s_tx_master_write(uint32_t dev_id, uint32_t *data,
			    uint32_t *size)
{
	i2s_tx_master_info_pt dev = i2s_tx_master_handles[dev_id];
	uint32_t idx = dev->usr_cnt;

	if (((idx - dev->sys_cnt) != 2) && (*size != 0)) {
		idx = idx & 0x0001; // current index in dev->buffer[] & dev->bufsize[]
#ifdef __Xdmac
		if (dev->dmachanid != DMA_NONE) {
			uint32_t burst_size = (dev->fifo_size - REG_READ(I2S_TFCR)) << 1;       // actual FIFO trigger level
			uint32_t xfer_size = (*size) << 2;                                      // transfer size in bytes (ToDo: limited by 8KB)
			idx = idx << 2;                                                         // now this is index in the array of DMA descriptors

			/* DMA: update DMACTRL and DMASAR part of descriptor */
			dev->dmadescriptor[idx + 0] =
				I2S_TX_MASTER_DMA_CTRL |
				(burst_size << I2S_TX_MASTER_DMA_CTRL_XFER_POS) |
				((xfer_size - 1) << I2S_TX_MASTER_DMA_CTRL_SIZE_POS);
			dev->dmadescriptor[idx + 1] = (uint32_t) data + xfer_size - 2;
			dev->usr_cnt++; // update user' buffer access counter;

			/* init DMA if required */
			if (dev->xfr_started == 0) { /* I2S transfer is not yet started -> init I2S Tx and DMA channel */
				dev->xfr_started = 1;

				REG_WRITE(I2S_TXFFR, 0x01);     // I2S: reset TX FIFO
				REG_WRITE(I2S_TFF, 0x01);       // I2S: flush TX FIFO
				REG_WRITE(I2S_CER, 0x01);       // I2S: start the clock
				REG_WRITE(I2S_ITER, 0x01);      // I2S: enable transmitter block
				REG_WRITE(I2S_IMR, ~0x20);      // I2S: Unmask TX FIFO write overrun interrupts

				if (dev->dmachanid < DMAC_MEM_CHAN_CNT) {
					_dma_chan_desc(dev->dmachanid,
						       &(dev->dmadescriptor[0]));
				} else {
					_dma_chan_desc_aux(dev->dmachanid,
							   &(dev->dmadescriptor[0]));
				}

				/* Start DMA channel */
				_dma_chan_enable((0x1 << dev->dmachanid), 1);
			}
		} else {
#endif              /* __Xdmac */
		/* Store data buffer parameters */
		dev->buffer[idx] = data;
		dev->bufsize[idx] = size;
		dev->usr_cnt++;                 // update user' buffer access counter;

		if (dev->xfr_started == 0) {    /* I2S transfer not yet started -> init I2S Rx */
			dev->xfr_started = 1;

			REG_WRITE(I2S_TXFFR, 0x01);     // I2S: reset TX FIFO
			REG_WRITE(I2S_TFF, 0x01);       // I2S: flush TX FIFO
			REG_WRITE(I2S_CER, 0x01);       // I2S: start the clock
			REG_WRITE(I2S_ITER, 0x01);      // I2S: enable transmitter block
			REG_WRITE(I2S_IMR, ~0x30);      // I2S: Unmask TX interrupts
		}
#ifdef __Xdmac
	}
#endif              /* __Xdmac */

	}
}

void io_i2s_tx_master_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
	i2s_tx_master_info_pt dev = i2s_tx_master_handles[dev_id];

	switch (cmd) {
	case IO_SET_CB_TX:
		dev->tx_cb = ((io_cb_t *) arg)->cb;
		break;

	case IO_SET_CB_ERR:
		dev->err_cb = ((io_cb_t *) arg)->cb;
		break;

	case IO_I2S_TX_MASTER_SET_SAMPLE_WIDTH:
		/* channel must be disabled before programming I2S_TCR */
		REG_WRITE(I2S_TCR, (*(uint32_t *) arg));
		break;

	case IO_I2S_TX_MASTER_SET_FIFO_THRESHOLD:
		/* channel must be disabled before programming I2S_TFCR */
		REG_WRITE(I2S_TFCR, (*(uint32_t *) arg));
		break;

	case IO_I2S_TX_MASTER_SET_BITCLOCK:
		REG_WRITE(I2S_CER, *((uint32_t *) arg));
		break;

	default:
		break;
	}
}

static uint32_t i2s_tx_master_write_samples(uint32_t dev_id,
					    uint32_t *count)
{
	i2s_tx_master_info_pt dev = i2s_tx_master_handles[dev_id];
	uint32_t avail_cnt = 0;

	if (dev->sys_cnt == dev->usr_cnt) { /* no more data buffer available (buffer underflow error) */
		/* if no buffer will be available until FIFO is full then error callback will be called from error ISR */
		return (1);
	} else {
		uint32_t idx = dev->sys_cnt & 0x0001;
		uint32_t *buff = dev->buffer[idx];
		uint32_t *size = dev->bufsize[idx];

		avail_cnt = (*size - dev->xfr_len);
		if (*count <= avail_cnt) { /* enough space in current buffer */
			avail_cnt = *count;
			*count = 0;
		} else {    /* *count > avail_cnt: there will be samples remaining for the next buffer */
			*count -= avail_cnt;
		}

		/* fill current buffer */
		for (int32_t i = 0; i != avail_cnt; i++) {
			REG_WRITE(I2S_LTHR, buff[dev->xfr_len + i]);
			REG_WRITE(I2S_RTHR, buff[dev->xfr_len + i] >> 16);
		}
		dev->xfr_len += avail_cnt;

		/* if current buffer is full: call user' callback, and increment read access counter */
		if (dev->xfr_len == *size) {
			dev->sys_cnt++;
			dev->xfr_len = 0;
			if (NULL != dev->tx_cb) {
				dev->tx_cb(dev_id);
			}
		}
	}

	return (0);
}

/* I2S TX data available interrupt handler */
static void i2s_tx_master_tx_ISR_proc(uint32_t dev_id)
{
	i2s_tx_master_info_pt dev = i2s_tx_master_handles[dev_id];
	uint32_t sample_cnt = 0;
	uint32_t error = 0;

	/* Tx FIFO trigger level reached - data available interrupt */
#ifdef __Xdmac
	if (dev->dmachanid == DMA_NONE) {       /* DMA either disabled or does not exist */
#endif                                          /* __Xdmac */

	sample_cnt = dev->fifo_size;

	if (dev->xfr_started == 0) {
		dev->xfr_started = 1;
	} else {
		sample_cnt -= REG_READ(I2S_TFCR);   // available data space in FIFO
	}

	while (sample_cnt != 0) {
		error = i2s_tx_master_write_samples(dev_id, &sample_cnt);
		if (error != 0) {                       /* buffer underrun - no user' buffer left to receive data */
			REG_WRITE(I2S_IMR, 0x10);       // I2S: mask TXFE interrupt
			break;
		}
	}

#ifdef __Xdmac
} else {                                        /* DMA enabled for I2S peripheral */
	if (++dev->sys_cnt == dev->usr_cnt) {   /* no data buffers left - disable dma channel and mask data available interrupt */
		_dma_chan_enable((0x1 << dev->dmachanid), 0);
	} else {
		// update buffer descriptor
		_dma_chan_enable((0x1 << dev->dmachanid), 1);
	}

	if (NULL != dev->tx_cb) {   /* call for Tx callback if any  */
		dev->tx_cb(dev_id);
	}

	_dma_int_clear(0x1 << dev->dmachanid);  // clear DMA interrupt flag

}
#endif              /* __Xdmac */
}

/* I2S TX error (FIFO overflow) interrupt handler */
static void i2s_tx_master_err_ISR_proc(uint32_t dev_id)
{
	i2s_tx_master_info_pt dev = i2s_tx_master_handles[dev_id];

	/* Tx FIFO overrun case */
#ifdef __Xdmac
	if (dev->dmachanid != DMA_NONE) {
		/* DMA: disable DMA channel */
		_dma_chan_enable((0x1 << dev->dmachanid), 0);
		_dma_int_clear((0x1 << dev->dmachanid));
	}
#endif

	/* I2S: stop I2S device */
	REG_WRITE(I2S_IMR, 0x30);       // I2S: mask TX interrupts
	REG_WRITE(I2S_CER, 0x00);       // I2S: disable clk
	REG_WRITE(I2S_IER, 0x00);       // I2S: disable device

	/* call error callback if any */
	if (NULL != dev->err_cb) {
		dev->err_cb(dev_id);
	}
}

#endif
