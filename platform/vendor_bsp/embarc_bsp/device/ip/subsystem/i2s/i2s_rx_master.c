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
#include "device/subsystem/i2s_rx_master.h"
#include "io_config.h"
#include "apexextensions.h"

/* I2S Rx Master device registers */
#define I2S_IER         0x000
#define I2S_IRER        0x004
#define I2S_CER         0x00c
#define I2S_CCR         0x010
#define I2S_RXFFR       0x014
#define I2S_LRBR0       0x020
#define I2S_RRBR0       0x024
#define I2S_RER0        0x028
#define I2S_RCR0        0x030   // Receive  configuration register
#define I2S_ISR0        0x038   // Interrupt status register
#define I2S_IMR0        0x03c   // Interrupt mask register
#define I2S_ROR0        0x040
#define I2S_RFCR0       0x048
#define I2S_RFF0        0x050
#define I2S_RXDMA       0x1c0

/* I2S Rx Master device specific macros */
#define I2S_RX_MASTER_DMA_CTRL              (0xa0000077)    /* am=b10, i=b1, dw/inc=b011, dtt=b10, r=b1, op=b11 */
#define I2S_RX_MASTER_DMA_CTRL_SIZE_POS     (8)
#define I2S_RX_MASTER_DMA_CTRL_XFER_POS     (21)

#define DMA_NONE                            (0xff)

typedef _Interrupt void (*ISR) ();

typedef struct i2s_rx_master_info_struct {
	/* I2S Rx master configuration settings */
	const uint32_t reg_base;                // base address of device register set
	const uint8_t instID;                   // module instance ID
	const uint16_t fifo_size;               // FIFO depth
	const uint16_t dmachanid;               // DMA channel ID
	/* I2S data buffer related fields */
	uint32_t xfr_len;                       // number of samples transferred in current buffer
	uint32_t xfr_started;                   // transfer started flag
	uint32_t *buffer[2];                    // two pointers to the user' buffers
	uint32_t *bufsize[2];                   // two sizes of user buffers
	volatile uint32_t usr_cnt;              // user' buffer access counter (incremented from read service)
	volatile uint32_t sys_cnt;              // system' buffer access counter (incremented from ISR)
#ifdef __Xdmac
	_Uncached uint32_t *dmadescriptor;      // pointer to the DMA descriptor location
#endif
	/* User' callbacks */
	IO_CB_FUNC rx_cb;               // user' RX completion callback
	IO_CB_FUNC err_cb;              // user' error callback
	/* Interrupt numbers and handlers */
	const uint8_t rx_vector;        // RX interrupt vector number
	const uint8_t err_vector;       // RX Error inetrrupt vector number
	ISR rx_isr;                     // I2S data available interrupt handler
	ISR err_isr;                    // I2S FIFO overrun interrupt handler
} i2s_rx_master_info_t, *i2s_rx_master_info_pt;

#ifdef IO_I2S_RX_MST0_PRESENT
#define I2S_RX_MASTER_DEV_PRESENT
#endif
#ifdef IO_I2S_RX_MST1_PRESENT
#define I2S_RX_MASTER_DEV_PRESENT
#endif
#ifdef IO_I2S_RX_MST2_PRESENT
#define I2S_RX_MASTER_DEV_PRESENT
#endif
#ifdef IO_I2S_RX_MST3_PRESENT
#define I2S_RX_MASTER_DEV_PRESENT
#endif
#ifdef IO_I2S_RX_MST4_PRESENT
#define I2S_RX_MASTER_DEV_PRESENT
#endif

#ifdef I2S_RX_MASTER_DEV_PRESENT

#define I2S_RX_MASTER_MAX_CNT   (4)

static void i2s_rx_master_rx_ISR_proc(uint32_t dev_id);
static void i2s_rx_master_err_ISR_proc(uint32_t dev_id);

#ifdef IO_I2S_RX_MST0_PRESENT
static _Interrupt void i2s_rx_master0_rx_ISR()
{
	i2s_rx_master_rx_ISR_proc(0);
}

static _Interrupt void i2s_rx_master0_err_ISR()
{
	i2s_rx_master_err_ISR_proc(0);
}
#endif
#ifdef IO_I2S_RX_MST1_PRESENT
static _Interrupt void i2s_rx_master1_rx_ISR()
{
	i2s_rx_master_rx_ISR_proc(1);
}

static _Interrupt void i2s_rx_master1_err_ISR()
{
	i2s_rx_master_err_ISR_proc(1);
}
#endif
#ifdef IO_I2S_RX_MST2_PRESENT
static _Interrupt void i2s_rx_master2_rx_ISR()
{
	i2s_rx_master_rx_ISR_proc(2);
}

static _Interrupt void i2s_rx_master2_err_ISR()
{
	i2s_rx_master_err_ISR_proc(2);
}
#endif
#ifdef IO_I2S_RX_MST3_PRESENT

static _Interrupt void i2s_rx_master3_rx_ISR()
{
	i2s_rx_master_rx_ISR_proc(3);
}

static _Interrupt void i2s_rx_master3_err_ISR()
{
	i2s_rx_master_err_ISR_proc(3);
}
#endif

/* I2S RX Master devices private data structures */
static i2s_rx_master_info_pt i2s_rx_master_handles[I2S_RX_MASTER_MAX_CNT] =
{ 0 };
#ifdef __Xdmac
static __attribute__ ((aligned(32)))
_Uncached uint32_t i2s_rx_master_dma_descriptors[8 *
						 I2S_RX_MASTER_MAX_CNT];
#endif

static i2s_rx_master_info_t i2s_rx_master_devs[] = {
#ifdef IO_I2S_RX_MST0_PRESENT
	{ .instID = 0,
	  .reg_base = AR_IO_I2S_RX_MST0_IER,
	  .rx_vector = IO_I2S_RX_MST0_INT_RX_AVAIL,
	  .err_vector = IO_I2S_RX_MST0_INT_ERR,
	  .rx_isr = i2s_rx_master0_rx_ISR,
	  .err_isr = i2s_rx_master0_err_ISR,
#ifdef IO_I2S_RX_MST0_DMA
	  .dmachanid = IO_I2S_RX_MST0_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifo_size = IO_I2S_RX_MST0_FS },
#endif
#ifdef IO_I2S_RX_MST1_PRESENT
	{ .instID = 1,
	  .reg_base = AR_IO_I2S_RX_MST1_IER,
	  .rx_vector = IO_I2S_RX_MST1_INT_RX_AVAIL,
	  .err_vector = IO_I2S_RX_MST1_INT_ERR,
	  .rx_isr = i2s_rx_master1_rx_ISR,
	  .err_isr = i2s_rx_master1_err_ISR,
#ifdef IO_I2S_RX_MST1_DMA
	  .dmachanid = IO_I2S_RX_MST1_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifo_size = IO_I2S_RX_MST1_FS },
#endif
#ifdef IO_I2S_RX_MST2_PRESENT
	{ .instID = 2,
	  .reg_base = AR_IO_I2S_RX_MST2_IER,
	  .rx_vector = IO_I2S_RX_MST2_INT_RX_AVAIL,
	  .err_vector = IO_I2S_RX_MST2_INT_ERR,
	  .rx_isr = i2s_rx_master2_rx_ISR,
	  .err_isr = i2s_rx_master2_err_ISR,
#ifdef IO_I2S_RX_MST2_DMA
	  .dmachanid = IO_I2S_RX_MST2_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifo_size = IO_I2S_RX_MST2_FS },
#endif
#ifdef IO_I2S_RX_MST3_PRESENT
	{ .instID = 3,
	  .reg_base = AR_IO_I2S_RX_MST3_IER,
	  .rx_vector = IO_I2S_RX_MST3_INT_RX_AVAIL,
	  .err_vector = IO_I2S_RX_MST3_INT_ERR,
	  .rx_isr = i2s_rx_master3_rx_ISR,
	  .err_isr = i2s_rx_master3_err_ISR,
#ifdef IO_I2S_RX_MST3_DMA
	  .dmachanid = IO_I2S_RX_MST3_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifo_size = IO_I2S_RX_MST3_FS },
#endif
	{ .instID = I2S_RX_MASTER_MAX_CNT }
};

#define REG_WRITE(reg, x)   _sr((uint32_t)((x)), (uint32_t)(dev->reg_base + reg))
#define REG_READ(reg)       _lr((uint32_t)(dev->reg_base + (reg)))

/* API functions */
uint32_t io_i2s_rx_master_open(uint32_t dev_id)
{
	i2s_rx_master_info_pt dev;
	uint32_t h = 0;

	/* check device descriptor availability */
	while ((i2s_rx_master_devs[h].instID != dev_id)
	       && (i2s_rx_master_devs[h].instID != I2S_RX_MASTER_MAX_CNT)) {
		h++;
	}
	if ((i2s_rx_master_devs[h].instID == I2S_RX_MASTER_MAX_CNT) || (0 != i2s_rx_master_handles[dev_id])) {  /* dev_id not part of design, or still open */
		return 1;
	}
	i2s_rx_master_handles[dev_id] = &i2s_rx_master_devs[h];
	dev = i2s_rx_master_handles[dev_id];

	/* initialize driver internal variables */
	dev->xfr_started = 0;                   // clear transfer started flag
	dev->xfr_len = 0;                       // clear transfer length
	dev->usr_cnt = dev->sys_cnt = 0;        // reset buffer access counters

#ifdef __Xdmac
	/* initialize DMA descriptors */
	dev->dmadescriptor = &i2s_rx_master_dma_descriptors[8 * h];

	if (dev->dmachanid != DMA_NONE) {
		dev->dmadescriptor[0] = dev->dmadescriptor[4] = 0;
		dev->dmadescriptor[1] = dev->dmadescriptor[5] =
			dev->reg_base + I2S_RXDMA;
		dev->dmadescriptor[2] = dev->dmadescriptor[6] = 0;
		dev->dmadescriptor[3] = (uint32_t) &(dev->dmadescriptor[4]);
		dev->dmadescriptor[7] = (uint32_t) &(dev->dmadescriptor[0]);

		/* assign DMA interrupt handlers */
		_setvecti(dev->err_vector, dev->err_isr);

		_setvecti(DMAC_INT_BASE + dev->dmachanid, dev->rx_isr);
		_setvecti(DMAC_ERR_BASE + dev->dmachanid, dev->err_isr);
	} else {
#endif
	/* assign non DMA interrupt handlers */
	_setvecti(dev->rx_vector, dev->rx_isr);
	_setvecti(dev->err_vector, dev->err_isr);
#ifdef __Xdmac
}
#endif

	REG_WRITE(I2S_IER, 0x01);   // I2S: enable device

	return (0);
}

void io_i2s_rx_master_close(uint32_t dev_id)
{
	i2s_rx_master_info_pt dev = i2s_rx_master_handles[dev_id];

	/* reset I2S hardware */
	REG_WRITE(I2S_IMR0, 0x03);      // mask RX interrupts
	REG_WRITE(I2S_RER0, 0x01);      // reset channel enable to default (1)
	REG_WRITE(I2S_CER, 0x00);       // disable clk
	REG_WRITE(I2S_IRER, 0x00);      // disable receive block
	REG_WRITE(I2S_RCR0, 0x02);      // reset wlen to default (2)
	REG_WRITE(I2S_RFCR0, 0x00);     // reset threshold
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
	_setvecti(dev->rx_vector, NULL);
	_setvecti(dev->err_vector, NULL);

	/* reset driver internal variables */
	dev->rx_cb = NULL;
	dev->err_cb = NULL;
	dev->xfr_started = 0;

	/* reset device handler */
	i2s_rx_master_handles[dev_id] = 0;
}

void io_i2s_rx_master_read(uint32_t dev_id, uint32_t *data,
			   uint32_t *size)
{
	i2s_rx_master_info_pt dev = i2s_rx_master_handles[dev_id];
	uint32_t idx = dev->usr_cnt;    // copy user access counter to the temporary variable

	if (((idx - dev->sys_cnt) != 2) && (*size != 0)) {
		idx = idx & 0x0001; // current index in dev->buffer[] & dev->bufsize[]
#ifdef __Xdmac
		if (dev->dmachanid != DMA_NONE) {

			uint32_t burst_size = (REG_READ(I2S_RFCR0) + 1) << 1;   // actual FIFO trigger level
			uint32_t xfer_size = (*size) << 2;                      // transfer size in bytes (ToDo: limited by 8KB)
			idx = idx << 2;                                         // calculate available buffer' index

			/* DMA: update DMACTRL and DMADAR part of descriptor */
			dev->dmadescriptor[idx + 0] =
				I2S_RX_MASTER_DMA_CTRL |
				(burst_size << I2S_RX_MASTER_DMA_CTRL_XFER_POS) |
				((xfer_size - 1) << I2S_RX_MASTER_DMA_CTRL_SIZE_POS);
			dev->dmadescriptor[idx + 2] = (uint32_t) data + xfer_size - 2;

			if (dev->xfr_started == 0) { /* I2S transfer not yet started -> init DMA and I2S Rx */
				dev->xfr_started = 1;

				REG_WRITE(I2S_RXFFR, 0x01);     // I2S: reset RX FIFO
				REG_WRITE(I2S_RFF0, 0x01);      // I2S: flush RX FIFO
				REG_WRITE(I2S_CER, 0x01);       // I2S: start the clock
				REG_WRITE(I2S_IRER, 0x01);      // I2S: enable receive block
				REG_WRITE(I2S_IMR0, ~0x02);     // I2S: Unmask RX overrun interrupt

				if (dev->dmachanid < DMAC_MEM_CHAN_CNT) {
					_dma_chan_desc(dev->dmachanid, &(dev->dmadescriptor[0]));       // dev->dmaidx must be 0 !!!
				} else {
					_dma_chan_desc_aux(dev->dmachanid, &(dev->dmadescriptor[0]));   // dev->dmaidx must be 0 !!!
				}

				/* Start DMA channel / renew descriptor */
				_dma_chan_enable((0x1 << dev->dmachanid), 1);
			}
		} else {
#endif              /* __Xdmac */
		/* Update data buffer parameters */
		dev->buffer[idx] = data;
		dev->bufsize[idx] = size;

		if (dev->xfr_started == 0) {    /* I2S transfer not yet started -> init I2S Rx */
			dev->xfr_started = 1;

			/* Start I2S non DMA interrupt flow */
			REG_WRITE(I2S_RXFFR, 0x01);     // I2S: reset RX FIFO
			REG_WRITE(I2S_RFF0, 0x01);      // I2S: flush RX FIFO
			REG_WRITE(I2S_CER, 0x01);       // I2S: start the clock
			REG_WRITE(I2S_IRER, 0x01);      // I2S: enable receive block
			REG_WRITE(I2S_IMR0, ~0x03);     // I2S: Unmask all RX interrupts
		}
#ifdef __Xdmac
	}
#endif                          /* __Xdmac */

		dev->usr_cnt++; // update user' buffer access counter
	}
}

void io_i2s_rx_master_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
	i2s_rx_master_info_pt dev = i2s_rx_master_handles[dev_id];

	switch (cmd) {
	case IO_SET_CB_RX:
		dev->rx_cb = ((io_cb_t *) arg)->cb;
		break;

	case IO_SET_CB_ERR:
		dev->err_cb = ((io_cb_t *) arg)->cb;
		break;

	case IO_I2S_RX_MASTER_SET_SAMPLE_WIDTH:
		/* channel must be disabled before programming I2S_RCR0 */
		REG_WRITE(I2S_RCR0, (*(uint32_t *) arg));
		break;

	case IO_I2S_RX_MASTER_SET_FIFO_THRESHOLD:
		/* channel must be disabled before programming I2S_RFCR0 */
		REG_WRITE(I2S_RFCR0, (*(uint32_t *) arg));
		break;

	case IO_I2S_RX_MASTER_SET_BITCLOCK:
		REG_WRITE(I2S_CER, *((uint32_t *) arg));
		break;

	default:
		break;
	}
}

static uint32_t i2s_rx_master_read_samples(uint32_t dev_id,
					   uint32_t *count)
{
	i2s_rx_master_info_pt dev = i2s_rx_master_handles[dev_id];
	uint32_t free_cnt = 0;

	if (dev->usr_cnt == dev->sys_cnt) { /* no more data buffer available (buffer underflow error) */
		/* if no buffer will be available until FIFO is full then error callback will be called from error ISR */
		return (1);
	} else {
		uint32_t idx = dev->sys_cnt & 0x0001;
		uint32_t *buff = dev->buffer[idx];
		uint32_t *size = dev->bufsize[idx];

		free_cnt = (*size - dev->xfr_len);
		if (*count <= free_cnt) { /* enough space in buffer[0] */
			free_cnt = *count;
			*count = 0;
		} else {    /* *count > free_cnt: there will be samples remaining for the next buffer */
			*count -= free_cnt;
		}
		/* fill current buffer  */
		for (int32_t i = 0; i != free_cnt; i++) {
			buff[dev->xfr_len + i] = REG_READ(I2S_LRBR0);
			buff[dev->xfr_len + i] |= (REG_READ(I2S_RRBR0) << 16);
		}
		dev->xfr_len += free_cnt;

		/* if current buffer is full: rx callback, and increment system counter */
		if (dev->xfr_len == *size) {
			dev->sys_cnt++;
			dev->xfr_len = 0;
			if (NULL != dev->rx_cb) {
				dev->rx_cb(dev_id);
			}
		}
	}

	return (0);
}

/* I2S RX data available interrupt handler */
static void i2s_rx_master_rx_ISR_proc(uint32_t dev_id)
{
	i2s_rx_master_info_pt dev = i2s_rx_master_handles[dev_id];
	uint32_t sample_cnt = 0;
	uint32_t error = 0;

	/* Rx FIFO trigger level reached - data available interrupt */
#ifdef __Xdmac
	if (dev->dmachanid == DMA_NONE) {       /* DMA either disabled or does not exist */
#endif                                          /* __Xdmac */

	sample_cnt = REG_READ(I2S_RFCR0) + 1;   // actual FIFO trigger level

	while (sample_cnt != 0) {
		error = i2s_rx_master_read_samples(dev_id, &sample_cnt);
		if (error != 0) {                       /* buffer underrun - no user' buffer left to receive data */
			REG_WRITE(I2S_IMR0, 0x0001);    // I2S: mask RXDA interrupt
			break;
		}
	}

#ifdef __Xdmac
} else {                                        /* DMA enabled for I2S peripheral */
	if (++dev->sys_cnt == dev->usr_cnt) {   /* no data buffers left - disable dma channel and mask data available interrupt */
		_dma_chan_enable((0x1 << dev->dmachanid), 0);
	} else {
		// update channel descriptor
		_dma_chan_enable((0x1 << dev->dmachanid), 1);
	}

	if (NULL != dev->rx_cb) {   /* call for Rx callback if any  */
		dev->rx_cb(dev_id);
	}

	_dma_int_clear((0x1 << dev->dmachanid));        // clear DMA interrupt flag
}
#endif                                                  /* __Xdmac */
}

/* I2S RX error (FIFO overflow) interrupt handler */
static void i2s_rx_master_err_ISR_proc(uint32_t dev_id)
{
	i2s_rx_master_info_pt dev = i2s_rx_master_handles[dev_id];

	/* Rx FIFO overrun case */
#ifdef __Xdmac
	if (dev->dmachanid != DMA_NONE) {
		/* DMA: disable DMA channel */
		_dma_chan_enable((0x1 << dev->dmachanid), 0);
		_dma_int_clear((0x1 << dev->dmachanid));
	}
#endif

	/* I2S: stop I2S device */
	REG_WRITE(I2S_IMR0, 0x03);      // I2S: mask RX interrupts
	REG_WRITE(I2S_CER, 0x00);       // I2S: disable clk
	REG_WRITE(I2S_IER, 0x00);       // I2S: disable device

	/* call error callback if any */
	if (NULL != dev->err_cb) {
		dev->err_cb(dev_id);
	}
}

#endif
