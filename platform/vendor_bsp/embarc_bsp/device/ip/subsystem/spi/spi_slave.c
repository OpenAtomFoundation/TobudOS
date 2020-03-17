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
#include "device/subsystem/spi_slave.h"
#include "device/subsystem/spi_priv.h"
#include "io_config.h"
#include "apexextensions.h"

/* local defines (!!) */
#define SPI_TRANSMIT_RECEIVE_MODE   (0)
#define SPI_TRANSMIT_ONLY_MODE      (1)
#define SPI_RECEIVE_ONLY_MODE       (2)

#ifdef IO_SPI_SLV0_PRESENT
#define SPI_DEV_PRESENT
#endif
#ifdef IO_SPI_SLV1_PRESENT
#define SPI_DEV_PRESENT
#endif
#ifdef IO_SPI_SLV2_PRESENT
#define SPI_DEV_PRESENT
#endif
#ifdef IO_SPI_SLV3_PRESENT
#define SPI_DEV_PRESENT
#endif

#ifdef SPI_DEV_PRESENT

#define SPI_MAX_CNT (4)

static void spi_slv_rx_ISR_proc(uint32_t dev_id);
static void spi_slv_tx_ISR_proc(uint32_t dev_id);
static void spi_slv_idle_ISR_proc(uint32_t dev_id);
static void spi_slv_err_ISR_proc(uint32_t dev_id);

#ifdef IO_SPI_SLV0_PRESENT
static _Interrupt void spi_slv0_rx_ISR()
{
	spi_slv_rx_ISR_proc(0);
}

static _Interrupt void spi_slv0_tx_ISR()
{
	spi_slv_tx_ISR_proc(0);
}

static _Interrupt void spi_slv0_idle_ISR()
{
	spi_slv_idle_ISR_proc(0);
}

static _Interrupt void spi_slv0_err_ISR()
{
	spi_slv_err_ISR_proc(0);
}
#endif
#ifdef IO_SPI_SLV1_PRESENT
static _Interrupt void spi_slv1_rx_ISR()
{
	spi_slv_rx_ISR_proc(1);
}

static _Interrupt void spi_slv1_tx_ISR()
{
	spi_slv_tx_ISR_proc(1);
}

static _Interrupt void spi_slv1_idle_ISR()
{
	spi_slv_idle_ISR_proc(1);
}

static _Interrupt void spi_slv1_err_ISR()
{
	spi_slv_err_ISR_proc(1);
}
#endif
#ifdef IO_SPI_SLV2_PRESENT
static _Interrupt void spi_slv2_rx_ISR()
{
	spi_slv_rx_ISR_proc(2);
}

static _Interrupt void spi_slv2_tx_ISR()
{
	spi_slv_tx_ISR_proc(2);
}

static _Interrupt void spi_slv2_idle_ISR()
{
	spi_slv_idle_ISR_proc(2);
}

static _Interrupt void spi_slv2_err_ISR()
{
	spi_slv_err_ISR_proc(2);
}
#endif
#ifdef IO_SPI_SLV3_PRESENT
static _Interrupt void spi_slv3_rx_ISR()
{
	spi_slv_rx_ISR_proc(3);
}

static _Interrupt void spi_slv3_tx_ISR()
{
	spi_slv_tx_ISR_proc(3);
}

static _Interrupt void spi_slv3_idle_ISR()
{
	spi_slv_idle_ISR_proc(3);
}

static _Interrupt void spi_slv3_err_ISR()
{
	spi_slv_err_ISR_proc(3);
}
#endif

static void fill_txfifo(spi_info_pt dev);
static void retrieve_rxfifo(spi_info_pt dev);

#ifdef __Xdmac
static void create_dma_descriptors_rx(spi_info_pt dev, uint8_t *dest,
				      uint32_t size, uint32_t burst);
static void create_dma_descriptors_tx(spi_info_pt dev, uint8_t *src,
				      uint32_t size, uint32_t burst);
#endif

/* SPI slave devices private data structures */
static spi_info_pt spi_handles[SPI_MAX_CNT] = { 0 };

#ifdef __Xdmac
static _Uncached uint32_t spi_dmarx_descriptors[8 * SPI_MAX_CNT];
static _Uncached uint32_t spi_dmatx_descriptors[8 * SPI_MAX_CNT];
#endif

static spi_info_t spi_slave_devs[] = {
#ifdef IO_SPI_SLV0_PRESENT
	{ .instID = 0,
	  .reg_base = AR_IO_SPI_SLV0_CTRLR0,
#ifdef IO_SPI_SLV0_DMA_RX
	  .dmarxchanid = IO_SPI_SLV0_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_SPI_SLV0_DMA_TX
	  .dmatxchanid = IO_SPI_SLV0_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .rx_vector = IO_SPI_SLV0_INT_RX_AVAIL,
	  .tx_vector = IO_SPI_SLV0_INT_TX_REQ,
	  .idle_vector = IO_SPI_SLV0_INT_IDLE,
	  .err_vector = IO_SPI_SLV0_INT_ERR,
	  .rx_isr = spi_slv0_rx_ISR,
	  .tx_isr = spi_slv0_tx_ISR,
	  .idle_isr = spi_slv0_idle_ISR,
	  .err_isr = spi_slv0_err_ISR,
	  .log_xfer_size = (IO_SPI_SLV0_MAX_XFER_SIZE >> 4),
	  .max_xfer_size = (IO_SPI_SLV0_MAX_XFER_SIZE),
	  .fifo_depth = IO_SPI_SLV0_FS },
#endif
#ifdef IO_SPI_SLV1_PRESENT
	{ .instID = 1,
	  .reg_base = AR_IO_SPI_SLV1_CTRLR0,
#ifdef IO_SPI_SLV1_DMA_RX
	  .dmarxchanid = IO_SPI_SLV1_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_SPI_SLV1_DMA_TX
	  .dmatxchanid = IO_SPI_SLV1_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .rx_vector = IO_SPI_SLV1_INT_RX_AVAIL,
	  .tx_vector = IO_SPI_SLV1_INT_TX_REQ,
	  .idle_vector = IO_SPI_SLV1_INT_IDLE,
	  .err_vector = IO_SPI_SLV1_INT_ERR,
	  .rx_isr = spi_slv1_rx_ISR,
	  .tx_isr = spi_slv1_tx_ISR,
	  .idle_isr = spi_slv1_idle_ISR,
	  .err_isr = spi_slv1_err_ISR,
	  .log_xfer_size = (IO_SPI_SLV1_MAX_XFER_SIZE >> 4),
	  .max_xfer_size = (IO_SPI_SLV1_MAX_XFER_SIZE),
	  .fifo_depth = IO_SPI_SLV1_FS },
#endif
#ifdef IO_SPI_SLV2_PRESENT
	{ .instID = 2,
	  .reg_base = AR_IO_SPI_SLV2_CTRLR0,
#ifdef IO_SPI_SLV2_DMA_RX
	  .dmarxchanid = IO_SPI_SLV2_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_SPI_SLV2_DMA_TX
	  .dmatxchanid = IO_SPI_SLV2_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .rx_vector = IO_SPI_SLV2_INT_RX_AVAIL,
	  .tx_vector = IO_SPI_SLV2_INT_TX_REQ,
	  .idle_vector = IO_SPI_SLV2_INT_IDLE,
	  .err_vector = IO_SPI_SLV2_INT_ERR,
	  .rx_isr = spi_slv2_rx_ISR,
	  .tx_isr = spi_slv2_tx_ISR,
	  .idle_isr = spi_slv2_idle_ISR,
	  .err_isr = spi_slv2_err_ISR,
	  .log_xfer_size = (IO_SPI_SLV2_MAX_XFER_SIZE >> 4),
	  .max_xfer_size = (IO_SPI_SLV2_MAX_XFER_SIZE),
	  .fifo_depth = IO_SPI_SLV2_FS },
#endif
#ifdef IO_SPI_SLV3_PRESENT
	{ .instID = 3,
	  .reg_base = AR_IO_SPI_SLV3_CTRLR0,
#ifdef IO_SPI_SLV3_DMA_RX
	  .dmarxchanid = IO_SPI_SLV3_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_SPI_SLV3_DMA_TX
	  .dmatxchanid = IO_SPI_SLV3_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .rx_vector = IO_SPI_SLV3_INT_RX_AVAIL,
	  .tx_vector = IO_SPI_SLV3_INT_TX_REQ,
	  .idle_vector = IO_SPI_SLV3_INT_IDLE,
	  .err_vector = IO_SPI_SLV3_INT_ERR,
	  .rx_isr = spi_slv3_rx_ISR,
	  .tx_isr = spi_slv3_tx_ISR,
	  .idle_isr = spi_slv3_idle_ISR,
	  .err_isr = spi_slv3_err_ISR,
	  .log_xfer_size = (IO_SPI_SLV3_MAX_XFER_SIZE >> 4),
	  .max_xfer_size = (IO_SPI_SLV3_MAX_XFER_SIZE),
	  .fifo_depth = IO_SPI_SLV3_FS },
#endif
	{ .instID = SPI_MAX_CNT }
};

#define     REG_WRITE(reg, x)   _sr((unsigned)(x), (unsigned)(dev->reg_base + reg))
#define     REG_READ(reg)       _lr((unsigned)(dev->reg_base + reg))
#define     REG_WRITE_BITS(reg, x, y, len, pos)   REG_WRITE(reg, ((((x)          & ~((~(0xffffffff << len)) << pos)) \
								   | (((y) << pos)  &  ((~(0xffffffff << len)) << pos)))))
#define     REG_GET_BITS(reg, len, pos)       ((REG_READ(reg) >> pos) & ~(0xffffffff << len))

uint32_t io_spi_slave_open(uint32_t dev_id)
{
	uint32_t h = 0;
	spi_info_pt dev;

	h = 0;
	while ((spi_slave_devs[h].instID != dev_id)
	       && (spi_slave_devs[h].instID != SPI_MAX_CNT)) {
		h++;
	}
	if ((spi_slave_devs[h].instID == SPI_MAX_CNT) || (0 != spi_handles[dev_id])) {  /* dev_id not part of design, or still open */
		return 1;
	}
	spi_handles[dev_id] = &spi_slave_devs[h];
	dev = spi_handles[dev_id];

	/* enable clock */
	REG_WRITE(CLK_ENA, 0x1);

	if (dev->max_xfer_size == 16) { /* 16-bits SPI; log_xfer_size == 1 */
		dev->log_xfer_size = dev->max_xfer_size >> 4;
		REG_WRITE_BITS(CTRL0, 0x0, 15, 4, 0);
	} else {
		/* 32-bits SPI; log_xfer_size == 2 */
		dev->log_xfer_size = dev->max_xfer_size >> 4;
		REG_WRITE_BITS(CTRL0, 0x0, 31, 5, 16);
	}

	/* mask RX-available and TX-empty interrupts, always mask TX-underflow */
	REG_WRITE(IMR, ~(R_TX_EMPTY | R_RX_FULL | R_TX_UNDER));

	dev->handling_rx = dev->handling_tx = 0;

	_setvecti(dev->rx_vector, dev->rx_isr);
	_setvecti(dev->tx_vector, dev->tx_isr);
	_setvecti(dev->idle_vector, dev->idle_isr);
	_setvecti(dev->err_vector, dev->err_isr);

#ifdef __Xdmac
	dev->dmarxdescriptor = &spi_dmarx_descriptors[8 * h];
	dev->dmatxdescriptor = &spi_dmatx_descriptors[8 * h];

	if (dev->dmarxchanid != DMA_NONE) {
		dev->dmarxdescriptor[1] = dev->dmarxdescriptor[5] =
			dev->reg_base + DR;
		dev->dmarxdescriptor[3] = (uint32_t) &(dev->dmarxdescriptor[4]);
		dev->dmarxdescriptor[7] = 0;

		_setvecti(DMAC_INT_BASE + dev->dmarxchanid, dev->rx_isr);
		_setvecti(DMAC_ERR_BASE + dev->dmarxchanid, dev->err_isr);
	}

	if (dev->dmatxchanid != DMA_NONE) {
		dev->dmatxdescriptor[2] = dev->dmatxdescriptor[6] =
			dev->reg_base + DR;
		dev->dmatxdescriptor[3] = (uint32_t) &(dev->dmatxdescriptor[4]);
		dev->dmatxdescriptor[7] = 0;

		_setvecti(DMAC_INT_BASE + dev->dmatxchanid, dev->tx_isr);
		_setvecti(DMAC_ERR_BASE + dev->dmatxchanid, dev->err_isr);
	}
#endif

	REG_WRITE(SPIEN, 0x1);

	return 0;
}

void io_spi_slave_close(uint32_t dev_id)
{
	spi_info_pt dev = spi_handles[dev_id];

	REG_WRITE(IMR, 0);
	REG_WRITE(SPIEN, 0);

	REG_WRITE(CTRL0, 0x7);
	REG_WRITE(TXFTLR, 0);
	REG_WRITE(RXFTLR, 0);
	REG_WRITE(IMR, 0xff);

	/* gate clock */
	REG_WRITE(CLK_ENA, 0);

	_setvecti(dev->rx_vector, NULL);
	_setvecti(dev->tx_vector, NULL);
	_setvecti(dev->idle_vector, NULL);
	_setvecti(dev->err_vector, NULL);

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

	spi_handles[dev_id] = 0;
}

void io_spi_slave_read(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t val = 0;

	dev->rx_size = *size;
	dev->p_rxsize = size;
	dev->rx_count = 0;

	dev->rx_buff = (void *) data;

	dev->handling_rx = 1;

#ifdef __Xdmac
	if (dev->dmarxchanid == DMA_NONE) {
#endif
	/* Enable RX-available and Idle interrupt */
	val = REG_READ(IMR);
	REG_WRITE(IMR, (val | R_RX_FULL | R_IDLE));
#ifdef __Xdmac
} else {                /* RX descriptor */
	if (*size != 0) {
		/* rx data available; at least one */
		create_dma_descriptors_rx(dev, data, *size,
					  (1 + REG_READ(RXFTLR)));

		/* init dma */
		if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
			_dma_chan_desc(dev->dmarxchanid,
				       &(dev->dmarxdescriptor[0]));
		} else {
			_dma_chan_desc_aux(dev->dmarxchanid,
					   &(dev->dmarxdescriptor[0]));
		}

		_dma_chan_enable((0x1 << dev->dmarxchanid), 1);

		/* enable the Idle interrupt */
		val = REG_READ(IMR);
		REG_WRITE(IMR, val | R_IDLE);
	}
}
#endif
}

void io_spi_slave_write(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t val = 0;

	dev->tx_size = *size;
	dev->tx_count = 0;
	dev->tx_buff = (void *) data;

	dev->handling_tx = 1;

#ifdef __Xdmac
	if (dev->dmatxchanid == DMA_NONE) {
#endif
	/* write first bytes to fifo, assuming size > 0 */
	fill_txfifo(dev);

	/* Enable TX-empty, Idle interrupt, and (optional) RX-available */
	if (REG_GET_BITS(CTRL0, 2, 8) == SPI_TRANSMIT_ONLY_MODE) {
		val = REG_READ(IMR);
		REG_WRITE(IMR, (val | R_TX_EMPTY | R_IDLE));
	} else {        /* TX-RX mode */
		val = REG_READ(IMR);
		REG_WRITE(IMR, (val | R_TX_EMPTY | R_IDLE | R_RX_FULL));
	}
#ifdef __Xdmac
} else {                /* TX descriptor */
	if (*size != 0) {
		/* tx data available; at least one, more is uncertain */
		create_dma_descriptors_tx(dev, data, *size,
					  (dev->fifo_depth -
					   REG_READ(TXFTLR)));

		/* init dma */
		if (dev->dmatxchanid < DMAC_MEM_CHAN_CNT) {
			_dma_chan_desc(dev->dmatxchanid,
				       &(dev->dmatxdescriptor[0]));
		} else {
			_dma_chan_desc_aux(dev->dmatxchanid,
					   &(dev->dmatxdescriptor[0]));
		}

		_dma_chan_enable((0x1 << dev->dmatxchanid), 1);

		/* enable the Idle interrupt */
		val = REG_READ(IMR);
		REG_WRITE(IMR, val | R_IDLE);
	}
}
#endif
}

void io_spi_slave_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t regval;

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

	case IO_SPI_SLAVE_SET_RX_THRESHOLD:
		REG_WRITE(RXFTLR, *((uint32_t *) arg));
		break;
	case IO_SPI_SLAVE_SET_TX_THRESHOLD:
		REG_WRITE(TXFTLR, *((uint32_t *) arg));
		break;

	case IO_SPI_SLAVE_SET_PACKING:
	{                                                               /* DFS must be set prior applying this option */
		regval = REG_READ(CTRL0);
		if (*((uint32_t *) arg) == 0) {                         /* reset packing */
			dev->log_xfer_size = dev->max_xfer_size >> 4;
		} else {                                                /* set packing */
			if (dev->max_xfer_size == 32) {                 /* 32-bits SPI */
				regval = (regval >> 16) & 0x01F;        // Get DFS field from CTRL0 value
				dev->log_xfer_size =
					(regval == 15) ? 1 : (regval ==
							      7) ? 0 : dev->
					max_xfer_size >> 4;
			} else {                                /* 16-bits SPI */
				regval = regval & 0x00F;        // Get DFS field from CTRL0 value
				dev->log_xfer_size =
					(regval == 7) ? 0 : dev->max_xfer_size >> 4;
			}
		}
		break;
	}
	default:
	{
		/* ioctl calls that require disabling of the SPI first */
		REG_WRITE(SPIEN, 0);

		regval = REG_READ(CTRL0);
		switch (cmd) {
		case IO_SPI_SLAVE_SET_TRANSFER_MODE:
			REG_WRITE_BITS(CTRL0, regval, *((uint32_t *) arg), 2, 8);
			break;
		case IO_SPI_SLAVE_SET_SCPH:
			REG_WRITE_BITS(CTRL0, regval, *((uint32_t *) arg), 1, 6);
			break;
		case IO_SPI_SLAVE_SET_SCPL:
			REG_WRITE_BITS(CTRL0, regval, *((uint32_t *) arg), 1, 7);
			break;
		case IO_SPI_SLAVE_SET_SRL_MODE:
			REG_WRITE_BITS(CTRL0, regval, *((uint32_t *) arg), 1, 11);
			break;
		case IO_SPI_SLAVE_SET_OUTPUT_ENABLE:
			REG_WRITE_BITS(CTRL0, regval, *((uint32_t *) arg), 1, 10);
			break;

		case IO_SPI_SLAVE_SET_DFS:
			REG_WRITE_BITS(CTRL0, regval, ((*((uint32_t *) arg)) - 1),
				       (3 + dev->log_xfer_size),
				       ((dev->log_xfer_size - 1) << 4));
			break;

		default:
			break;
		}

		REG_WRITE(SPIEN, 0x1);

		break;
	}
	}
}

static void spi_slv_rx_ISR_proc(uint32_t dev_id)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t val = 0;

#ifdef __Xdmac
	if (dev->dmarxchanid == DMA_NONE) {
#endif
	/* read remaining data from fifo */
	retrieve_rxfifo(dev);

	if (dev->rx_size <= (dev->rx_count << dev->log_xfer_size)) {    /* read buffer completely filled, mask RX-avail interrupt */
		val = REG_READ(IMR);
		REG_WRITE(IMR, (val & ~R_RX_FULL));
	}
#ifdef __Xdmac
} else {
	uint32_t divisor = 0, remainder = 0;

	/* DMA RX xfer done */
	_dma_chan_enable((0x1 << dev->dmarxchanid), 0);
	_dma_int_clear((0x1 << dev->dmarxchanid));

	/* process final data, if it is a partial (half)word */
	divisor = dev->rx_size >> dev->log_xfer_size;
	remainder = dev->rx_size - (divisor << dev->log_xfer_size);

	if (remainder != 0) {
		uint32_t mask = ~((~0x0) << (remainder * 8));

		if (dev->log_xfer_size == 1) {
			((uint16_t *) (dev->rx_buff))[divisor] =
				(((uint16_t *) (dev->
						rx_buff))[divisor] & ~mask) | (dev->
									       dmarx_last16
									       & mask);
		} else if (dev->log_xfer_size == 2) {
			((uint32_t *) (dev->rx_buff))[divisor] =
				(((uint32_t *) (dev->
						rx_buff))[divisor] & ~mask) | (dev->
									       dmarx_last32
									       & mask);
		}
	}
}
#endif
}

static void spi_slv_tx_ISR_proc(uint32_t dev_id)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t val = 0;

#ifdef __Xdmac
	if (dev->dmatxchanid == DMA_NONE) {
#endif
	if (dev->tx_size <= (dev->tx_count << dev->log_xfer_size)) {
		/* no data left to put into the fifo, mask TX-request */
		val = REG_READ(IMR);
		REG_WRITE(IMR, (val & ~R_TX_EMPTY));
	} else {        /* write data into the fifo */
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

static void spi_slv_idle_ISR_proc(uint32_t dev_id)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t val = 0;

	if (dev->handling_tx == 1) {
		if ((dev->tx_size <= (dev->tx_count << dev->log_xfer_size)) || (dev->dmatxchanid != DMA_NONE)) { /* buffer completely processed */

			dev->handling_tx = 0;

			if (NULL != dev->tx_cb) {
				dev->tx_cb(dev_id);
			}

			/* mask TX-empty (if not done yet) and Idle interrupt */
			val = REG_READ(IMR);
			REG_WRITE(IMR, (val & ~R_TX_EMPTY & ~R_IDLE));
		}
	}

	if (dev->handling_rx == 1) {
#ifdef __Xdmac
		if (dev->dmarxchanid == DMA_NONE) {
#endif
		/* read remaining data from fifo */
		retrieve_rxfifo(dev);
#ifdef __Xdmac
	}
#endif

		if ((dev->rx_size <= (dev->rx_count << dev->log_xfer_size)) || (dev->dmarxchanid != DMA_NONE)) { /* buffer completely processed */

			dev->handling_rx = 0;

			if (NULL != dev->rx_cb) {
				dev->rx_cb(dev_id);
			}

			/* mask RX-available (if not done yet) and Idle interrupt */
			val = REG_READ(IMR);
			REG_WRITE(IMR, (val & ~R_RX_FULL & ~R_IDLE));
		}
	}

	/* clear idle interrupt */
	REG_WRITE(ICR, R_IDLE);
}

static void spi_slv_err_ISR_proc(uint32_t dev_id)
{
	spi_info_pt dev = spi_handles[dev_id];

	REG_WRITE(IMR, 0);

	if (NULL != dev->err_cb) {
		dev->err_cb(dev_id);
	}

	REG_WRITE(ICR, R_RX_OVER | R_RX_UNDER | R_TX_OVER | R_TX_UNDER);
}

static void fill_txfifo(spi_info_pt dev)
{
	uint32_t cnt = 0, h = 0, free = 0;

	cnt = dev->tx_size - (dev->tx_count << dev->log_xfer_size);
	free = (dev->fifo_depth - REG_READ(TXFLR)) << dev->log_xfer_size;
	if (cnt > free) {
		cnt = free;
	}

	if (dev->log_xfer_size == 0) {  /* 8-bits data */
		for (h = 0; h != cnt; h++) {
			REG_WRITE(DR, ((uint8_t *) (dev->tx_buff))[dev->tx_count++]);
		}
	} else if (dev->log_xfer_size == 1) {   /* 16-bits data */
		for (h = 0; h != cnt / 2; h++) {
			REG_WRITE(DR, ((uint16_t *) (dev->tx_buff))[dev->tx_count++]);
		}
		/* if 2h != cnt: partial tx left at the end */
		if (2 * h != cnt) {
			REG_WRITE(DR,
				  ((((uint16_t *) (dev->tx_buff))[dev->
								  tx_count++]) &
				   0x00ff));
		}
	} else {            /* log_xfer_size == 2; 32-bits data */
		for (h = 0; h != cnt / 4; h++) {
			REG_WRITE(DR, ((uint32_t *) (dev->tx_buff))[dev->tx_count++]);
		}
		/* if 4h != cnt: partial tx left at the end */
		if (4 * h != cnt) {
			uint32_t mask32 = ~((~0x0) << ((cnt - 4 * h) * 8));
			REG_WRITE(DR,
				  ((((uint32_t *) (dev->tx_buff))[dev->
								  tx_count++]) &
				   mask32));
		}
	}

}

static void retrieve_rxfifo(spi_info_pt dev)
{
	uint32_t cnt = 0, h = 0, avail = 0;

	cnt = dev->rx_size - (dev->rx_count << dev->log_xfer_size);
	avail = REG_READ(RXFLR) << dev->log_xfer_size;
	if (cnt > avail) {
		cnt = avail;
	}

	if (dev->log_xfer_size == 0) {  /* 8-bits data */
		for (h = 0; h != cnt; h++) {
			((uint8_t *) (dev->rx_buff))[dev->rx_count++] =
				(uint8_t) REG_READ(DR);
		}
	} else if (dev->log_xfer_size == 1) {   /* 16-bits data */
		for (h = 0; h != cnt / 2; h++) {
			((uint16_t *) (dev->rx_buff))[dev->rx_count++] =
				(uint16_t) REG_READ(DR);
		}
		/* if 2h != cnt: partial rx left at the end */
		if (2 * h != cnt) {
			((uint16_t *) (dev->rx_buff))[dev->rx_count] =
				(((uint16_t *) (dev->rx_buff))[dev->
							       rx_count] & ~0x00ff) |
				(REG_READ(DR) & 0x00ff);
			dev->rx_count++;
		}

	} else {            /* 32-bits data; log_xfer_size == 2 */
		for (h = 0; h != cnt / 4; h++) {
			((uint32_t *) (dev->rx_buff))[dev->rx_count++] = REG_READ(DR);
		}
		/* if 4h != cnt: partial rx left at the end */
		if (4 * h != cnt) {
			uint32_t mask32 = ~((~0x0) << ((cnt - 4 * h) * 8));
			((uint32_t *) (dev->rx_buff))[dev->rx_count] =
				(((uint32_t *) (dev->rx_buff))[dev->
							       rx_count] & ~mask32) |
				(REG_READ(DR) & mask32);
			dev->rx_count++;
		}
	}

}

#ifdef __Xdmac
#define     SPI_SLV_DMA_RX_CTRL            (0x80000017) /* am=b10, i=b0, dw/inc=b..., dtt=b10, r=b1, op=b11 */
#define     SPI_SLV_DMA_RX_END_CTRL        (0xa0000015) /* am=b10, i=b1, dw/inc=b..., dtt=b10, r=b1, op=b01 */
#define     SPI_SLV_DMA_RX_REM_CTRL        (0xa0000015) /* am=b10, i=b1, dw/inc=b..., dtt=b10, r=b1, op=b01 */
#define     SPI_SLV_DMA_TX_CTRL            (0x4000000f) /* am=b01, i=b0, dw/inc=b..., dtt=b01, r=b1, op=b11 */
#define     SPI_SLV_DMA_TX_END_CTRL        (0x6000000d) /* am=b01, i=b1, dw/inc=b..., dtt=b01, r=b1, op=b01 */
#define     SPI_SLV_DMA_TX_REM_CTRL        (0x6000000d) /* am=b01, i=b1, dw/inc=b..., dtt=b01, r=b1, op=b01 */
#define     SPI_SLV_DMA_CTRL_DWINC_POS     (5)
#define     SPI_SLV_DMA_CTRL_SIZE_POS      (8)
#define     SPI_SLV_DMA_CTRL_XFER_POS      (21)
#define     SPI_SLV_DMA_FORCE_INT          (0x1 << 29)

static void create_dma_descriptors_rx(spi_info_pt dev, uint8_t *dest,
				      uint32_t size, uint32_t burst)
{
	uint32_t divisor = 0, remainder = 0;

	divisor = size >> dev->log_xfer_size;
	remainder = size - (divisor << dev->log_xfer_size);

	if (dev->log_xfer_size == 0) {
		dev->dmarxdescriptor[0] =
			SPI_SLV_DMA_RX_END_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((size - 1) << SPI_SLV_DMA_CTRL_SIZE_POS);
		dev->dmarxdescriptor[2] = (uint32_t) dest + (size - 1);
		if (dev->dmarxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmarxdescriptor[3] = 0;
		}
	} else if ((divisor > 0) && (remainder == 0)) {
		dev->dmarxdescriptor[0] =
			SPI_SLV_DMA_RX_END_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((size - 1) << SPI_SLV_DMA_CTRL_SIZE_POS)
			| ((2 * dev->log_xfer_size + 1) << SPI_SLV_DMA_CTRL_DWINC_POS);
		dev->dmarxdescriptor[2] = (uint32_t) dest + (size - 1);
		if (dev->dmarxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmarxdescriptor[3] = 0;
		}
	} else if ((divisor > 0) && (remainder > 0)) {
		dev->dmarxdescriptor[0] =
			SPI_SLV_DMA_RX_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((size - 1 - remainder) << SPI_SLV_DMA_CTRL_SIZE_POS)
			| ((2 * dev->log_xfer_size + 1) << SPI_SLV_DMA_CTRL_DWINC_POS);
		dev->dmarxdescriptor[2] = (uint32_t) dest + (size - 1 - remainder);
		if (dev->dmarxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmarxdescriptor[0] |= SPI_SLV_DMA_FORCE_INT;
			dev->dmarxdescriptor[3] =
				(uint32_t) &(dev->dmarxdescriptor[4]);
		}
		dev->dmarxdescriptor[4 + 0] =
			SPI_SLV_DMA_RX_REM_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((2 * dev->log_xfer_size - 1) << SPI_SLV_DMA_CTRL_SIZE_POS)
			| ((2 * dev->log_xfer_size + 1) << SPI_SLV_DMA_CTRL_DWINC_POS);
		if (dev->log_xfer_size == 1) {
			dev->dmarxdescriptor[4 + 2] =
				(uint32_t) &(dev->dmarx_last16) + 1;
		} else {
			dev->dmarxdescriptor[4 + 2] =
				(uint32_t) &(dev->dmarx_last32) + 3;
		}
		if (dev->dmarxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmarxdescriptor[4 + 3] = 0;
		}
	} else {            /* divisor == 0, remainder > 0 */

		dev->dmarxdescriptor[0] =
			SPI_SLV_DMA_RX_REM_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((2 * dev->log_xfer_size - 1) << SPI_SLV_DMA_CTRL_SIZE_POS)
			| ((2 * dev->log_xfer_size + 1) << SPI_SLV_DMA_CTRL_DWINC_POS);
		if (dev->log_xfer_size == 1) {
			dev->dmarxdescriptor[2] = (uint32_t) &(dev->dmarx_last16) + 1;
		} else {
			dev->dmarxdescriptor[2] = (uint32_t) &(dev->dmarx_last32) + 3;
		}
		if (dev->dmarxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmarxdescriptor[3] = 0;
		}
	}
}

static void create_dma_descriptors_tx(spi_info_pt dev, uint8_t *src,
				      uint32_t size, uint32_t burst)
{
	uint32_t divisor = 0, remainder = 0;

	divisor = size >> dev->log_xfer_size;
	remainder = size - (divisor << dev->log_xfer_size);

	if (dev->log_xfer_size == 1) {
		dev->dmatx_last16 =
			((uint16_t *) (dev->
				       tx_buff))[divisor] & ~((~0x0) << (remainder *
									 8));
	} else {
		dev->dmatx_last32 =
			((uint32_t *) (dev->
				       tx_buff))[divisor] & ~((~0x0) << (remainder *
									 8));
	}

	if (dev->log_xfer_size == 0) {
		dev->dmatxdescriptor[0] =
			SPI_SLV_DMA_TX_END_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((size - 1) << SPI_SLV_DMA_CTRL_SIZE_POS);
		dev->dmatxdescriptor[1] = (uint32_t) src + (size - 1);
		if (dev->dmatxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmatxdescriptor[3] = 0;
		}
	} else if ((divisor > 0) && (remainder == 0)) {
		dev->dmatxdescriptor[0] =
			SPI_SLV_DMA_TX_END_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((size - 1) << SPI_SLV_DMA_CTRL_SIZE_POS)
			| ((2 * dev->log_xfer_size + 1) << SPI_SLV_DMA_CTRL_DWINC_POS);
		dev->dmatxdescriptor[1] = (uint32_t) src + (size - 1);
		if (dev->dmatxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmatxdescriptor[3] = 0;
		}
	} else if ((divisor > 0) && (remainder > 0)) {
		dev->dmatxdescriptor[0] =
			SPI_SLV_DMA_TX_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((size - 1 - remainder) << SPI_SLV_DMA_CTRL_SIZE_POS)
			| ((2 * dev->log_xfer_size + 1) << SPI_SLV_DMA_CTRL_DWINC_POS);
		dev->dmatxdescriptor[1] = (uint32_t) src + (size - 1 - remainder);
		if (dev->dmatxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmatxdescriptor[0] |= SPI_SLV_DMA_FORCE_INT;
			dev->dmatxdescriptor[3] =
				(uint32_t) &(dev->dmatxdescriptor[4]);
		}
		dev->dmatxdescriptor[4 + 0] =
			SPI_SLV_DMA_TX_REM_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((2 * dev->log_xfer_size - 1) << SPI_SLV_DMA_CTRL_SIZE_POS)
			| ((2 * dev->log_xfer_size + 1) << SPI_SLV_DMA_CTRL_DWINC_POS);
		if (dev->log_xfer_size == 1) {
			dev->dmatxdescriptor[4 + 1] =
				(uint32_t) &(dev->dmatx_last16) + 1;
		} else {
			dev->dmatxdescriptor[4 + 1] =
				(uint32_t) &(dev->dmatx_last32) + 3;
		}
		if (dev->dmatxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmatxdescriptor[4 + 3] = 0;
		}
	} else {            /* divisor == 0, remainder > 0 */

		dev->dmatxdescriptor[0] =
			SPI_SLV_DMA_TX_REM_CTRL | (burst << SPI_SLV_DMA_CTRL_XFER_POS)
			| ((2 * dev->log_xfer_size - 1) << SPI_SLV_DMA_CTRL_SIZE_POS)
			| ((2 * dev->log_xfer_size + 1) << SPI_SLV_DMA_CTRL_DWINC_POS);
		if (dev->log_xfer_size == 1) {
			dev->dmatxdescriptor[1] = (uint32_t) &(dev->dmatx_last16) + 1;
		} else {
			dev->dmatxdescriptor[1] = (uint32_t) &(dev->dmatx_last32) + 3;
		}
		if (dev->dmatxchanid >= DMAC_MEM_CHAN_CNT) {
			dev->dmatxdescriptor[3] = 0;
		}
	}
}
#endif

#endif
