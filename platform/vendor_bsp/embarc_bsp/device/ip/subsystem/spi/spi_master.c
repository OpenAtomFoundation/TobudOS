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
#include "device/subsystem/spi_master.h"
#include "device/subsystem/spi_priv.h"
#include "io_config.h"
#include "apexextensions.h"

#ifdef IO_SPI_MST0_PRESENT
#define SPI_DEV_PRESENT
#endif
#ifdef IO_SPI_MST1_PRESENT
#define SPI_DEV_PRESENT
#endif
#ifdef IO_SPI_MST2_PRESENT
#define SPI_DEV_PRESENT
#endif
#ifdef IO_SPI_MST3_PRESENT
#define SPI_DEV_PRESENT
#endif

#ifdef SPI_DEV_PRESENT

#define SPI_MAX_CNT (4)

static void spi_mst_rx_ISR_proc(uint32_t dev_id);
static void spi_mst_tx_ISR_proc(uint32_t dev_id);
static void spi_mst_idle_ISR_proc(uint32_t dev_id);
static void spi_mst_err_ISR_proc(uint32_t dev_id);

#ifdef IO_SPI_MST0_PRESENT
static _Interrupt void spi_mst0_rx_ISR()
{
	spi_mst_rx_ISR_proc(0);
}

static _Interrupt void spi_mst0_tx_ISR()
{
	spi_mst_tx_ISR_proc(0);
}

static _Interrupt void spi_mst0_idle_ISR()
{
	spi_mst_idle_ISR_proc(0);
}

static _Interrupt void spi_mst0_err_ISR()
{
	spi_mst_err_ISR_proc(0);
}
#endif
#ifdef IO_SPI_MST1_PRESENT
static _Interrupt void spi_mst1_rx_ISR()
{
	spi_mst_rx_ISR_proc(1);
}

static _Interrupt void spi_mst1_tx_ISR()
{
	spi_mst_tx_ISR_proc(1);
}

static _Interrupt void spi_mst1_idle_ISR()
{
	spi_mst_idle_ISR_proc(1);
}

static _Interrupt void spi_mst1_err_ISR()
{
	spi_mst_err_ISR_proc(1);
}
#endif
#ifdef IO_SPI_MST2_PRESENT
static _Interrupt void spi_mst2_rx_ISR()
{
	spi_mst_rx_ISR_proc(2);
}

static _Interrupt void spi_mst2_tx_ISR()
{
	spi_mst_tx_ISR_proc(2);
}

static _Interrupt void spi_mst2_idle_ISR()
{
	spi_mst_idle_ISR_proc(2);
}

static _Interrupt void spi_mst2_err_ISR()
{
	spi_mst_err_ISR_proc(2);
}
#endif
#ifdef IO_SPI_MST3_PRESENT
static _Interrupt void spi_mst3_rx_ISR()
{
	spi_mst_rx_ISR_proc(3);
}

static _Interrupt void spi_mst3_tx_ISR()
{
	spi_mst_tx_ISR_proc(3);
}

static _Interrupt void spi_mst3_idle_ISR()
{
	spi_mst_idle_ISR_proc(3);
}

static _Interrupt void spi_mst3_err_ISR()
{
	spi_mst_err_ISR_proc(3);
}
#endif

static void fill_txfifo(spi_info_pt dev);
static void retrieve_rxfifo(spi_info_pt dev);

#ifdef __Xdmac
static void create_dma_descriptors_rx(spi_info_pt dev, uint32_t burst,
				      uint8_t idx);
static void create_dma_descriptors_tx(spi_info_pt dev, uint32_t burst,
				      uint8_t idx);
#endif

/* SPI master devices private data structures */
static spi_info_pt spi_handles[SPI_MAX_CNT] = { 0 };

#ifdef __Xdmac
static _Uncached uint32_t spi_dmarx_descriptors[8 * SPI_MAX_CNT];
static _Uncached uint32_t spi_dmatx_descriptors[8 * SPI_MAX_CNT];
#endif

static spi_info_t spi_master_devs[] = {
#ifdef IO_SPI_MST0_PRESENT
	{ .instID = 0,
	  .reg_base = AR_IO_SPI_MST0_CTRLR0,
#ifdef IO_SPI_MST0_DMA_RX
	  .dmarxchanid = IO_SPI_MST0_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_SPI_MST0_DMA_TX
	  .dmatxchanid = IO_SPI_MST0_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .rx_vector = IO_SPI_MST0_INT_RX_AVAIL,
	  .tx_vector = IO_SPI_MST0_INT_TX_REQ,
	  .idle_vector = IO_SPI_MST0_INT_IDLE,
	  .err_vector = IO_SPI_MST0_INT_ERR,
	  .rx_isr = spi_mst0_rx_ISR,
	  .tx_isr = spi_mst0_tx_ISR,
	  .idle_isr = spi_mst0_idle_ISR,
	  .err_isr = spi_mst0_err_ISR,
	  .log_xfer_size = (IO_SPI_MST0_MAX_XFER_SIZE >> 4),
	  .max_xfer_size = (IO_SPI_MST0_MAX_XFER_SIZE),
	  .fifo_depth = IO_SPI_MST0_FS },
#endif
#ifdef IO_SPI_MST1_PRESENT
	{ .instID = 1,
	  .reg_base = AR_IO_SPI_MST1_CTRLR0,
#ifdef IO_SPI_MST1_DMA_RX
	  .dmarxchanid = IO_SPI_MST1_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_SPI_MST1_DMA_TX
	  .dmatxchanid = IO_SPI_MST1_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .rx_vector = IO_SPI_MST1_INT_RX_AVAIL,
	  .tx_vector = IO_SPI_MST1_INT_TX_REQ,
	  .idle_vector = IO_SPI_MST1_INT_IDLE,
	  .err_vector = IO_SPI_MST1_INT_ERR,
	  .rx_isr = spi_mst1_rx_ISR,
	  .tx_isr = spi_mst1_tx_ISR,
	  .idle_isr = spi_mst1_idle_ISR,
	  .err_isr = spi_mst1_err_ISR,
	  .log_xfer_size = (IO_SPI_MST1_MAX_XFER_SIZE >> 4),
	  .max_xfer_size = (IO_SPI_MST1_MAX_XFER_SIZE),
	  .fifo_depth = IO_SPI_MST1_FS },
#endif
#ifdef IO_SPI_MST2_PRESENT
	{ .instID = 2,
	  .reg_base = AR_IO_SPI_MST2_CTRLR0,
#ifdef IO_SPI_MST2_DMA_RX
	  .dmarxchanid = IO_SPI_MST2_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_SPI_MST2_DMA_TX
	  .dmatxchanid = IO_SPI_MST2_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .rx_vector = IO_SPI_MST2_INT_RX_AVAIL,
	  .tx_vector = IO_SPI_MST2_INT_TX_REQ,
	  .idle_vector = IO_SPI_MST2_INT_IDLE,
	  .err_vector = IO_SPI_MST2_INT_ERR,
	  .rx_isr = spi_mst2_rx_ISR,
	  .tx_isr = spi_mst2_tx_ISR,
	  .idle_isr = spi_mst2_idle_ISR,
	  .err_isr = spi_mst2_err_ISR,
	  .log_xfer_size = (IO_SPI_MST2_MAX_XFER_SIZE >> 4),
	  .max_xfer_size = (IO_SPI_MST2_MAX_XFER_SIZE),
	  .fifo_depth = IO_SPI_MST2_FS },
#endif
#ifdef IO_SPI_MST3_PRESENT
	{ .instID = 3,
	  .reg_base = AR_IO_SPI_MST3_CTRLR0,
#ifdef IO_SPI_MST3_DMA_RX
	  .dmarxchanid = IO_SPI_MST3_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_SPI_MST3_DMA_TX
	  .dmatxchanid = IO_SPI_MST3_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .rx_vector = IO_SPI_MST3_INT_RX_AVAIL,
	  .tx_vector = IO_SPI_MST3_INT_TX_REQ,
	  .idle_vector = IO_SPI_MST3_INT_IDLE,
	  .err_vector = IO_SPI_MST3_INT_ERR,
	  .rx_isr = spi_mst3_rx_ISR,
	  .tx_isr = spi_mst3_tx_ISR,
	  .idle_isr = spi_mst3_idle_ISR,
	  .err_isr = spi_mst3_err_ISR,
	  .log_xfer_size = (IO_SPI_MST3_MAX_XFER_SIZE >> 4),
	  .max_xfer_size = (IO_SPI_MST3_MAX_XFER_SIZE),
	  .fifo_depth = IO_SPI_MST3_FS },
#endif
	{ .instID = SPI_MAX_CNT }
};

#define REG_WRITE(reg, x)   _sr((unsigned)(x), (unsigned)(dev->reg_base + reg))
#define REG_READ(reg)       _lr((unsigned)(dev->reg_base + reg))
#define REG_WRITE_BITS(reg, x, y, len, pos)   REG_WRITE(reg, ((((x)          & ~((~(0xffffffff << len)) << pos)) \
							       | (((y) << pos)  &  ((~(0xffffffff << len)) << pos)))))
#define REG_GET_BITS(reg, len, pos)       ((REG_READ(reg) >> pos) & ~(0xffffffff << len))

uint32_t io_spi_master_open(uint32_t dev_id)
{
	uint32_t h = 0;
	spi_info_pt dev;

	h = 0;
	while ((spi_master_devs[h].instID != dev_id)
	       && (spi_master_devs[h].instID != SPI_MAX_CNT)) {
		h++;
	}
	if ((spi_master_devs[h].instID == SPI_MAX_CNT) || (0 != spi_handles[dev_id])) { /* dev_id not part of design, or still open */
		return 1;
	}
	spi_handles[dev_id] = &spi_master_devs[h];
	dev = spi_handles[dev_id];

	/* enable clock */
	REG_WRITE(CLK_ENA, 0x1);

	if (dev->log_xfer_size == 1) {
		REG_WRITE_BITS(CTRL0, 0x0, 15, 4, 0);
	} else {            /* log_xfer_size == 2 */
		REG_WRITE_BITS(CTRL0, 0x0, 31, 5, 16);
	}

	/* mask RX-available, TX-empty ,and Idle interrupts */
	REG_WRITE(IMR, ~(R_TX_EMPTY | R_RX_FULL | R_IDLE));

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

void io_spi_master_close(uint32_t dev_id)
{
	spi_info_pt dev = spi_handles[dev_id];

	REG_WRITE(IMR, 0);
	REG_WRITE(SPIEN, 0);

	REG_WRITE(CTRL0, 0x7);
	REG_WRITE(CTRL1, 0);
	REG_WRITE(SER, 0);
	REG_WRITE(BAUDR, 0);
	REG_WRITE(TXFTLR, 0);
	REG_WRITE(RXFTLR, 0);
	REG_WRITE(RX_SAMPLE_DLY, 0);
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

void io_spi_master_read(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t val = 0;

	dev->rx_size = *size;
	dev->p_rxsize = size;
	dev->rx_count = 0;

	dev->rx_buff = (void *) data;

	dev->handling_rx = 1;

	/* Configure the Number of Data frames to be received */
	REG_WRITE(SPIEN, 0);
	val =
		(dev->rx_size + (dev->log_xfer_size << 1) -
		 1) >> dev->log_xfer_size;
	REG_WRITE(CTRL1, (val - 1));
	while (REG_READ(SPIEN) != 0) {
		;
	}
	REG_WRITE(SPIEN, 0x1);

#ifdef __Xdmac
	if (dev->dmarxchanid == DMA_NONE) {
#endif
	if (REG_GET_BITS(CTRL0, 2, 8) == SPI_RECEIVE_ONLY_MODE) {
		/* Write dummy to trigger the transfer */
		REG_WRITE(DR, 0xdeadbeef);

		/* Enable RX-available and Idle interrupt */
		val = REG_READ(IMR);
		REG_WRITE(IMR, (val | R_RX_FULL | R_IDLE));
	}
#ifdef __Xdmac
} else {                /* RX descriptor - postpone for SPI_TRANSMIT_RECEIVE_MODE, since size depends on tx-size */
	if ((dev->rx_size != 0)
	    && (REG_GET_BITS(CTRL0, 2, 8) != SPI_TRANSMIT_RECEIVE_MODE)) {
		/* rx data available; at least one */
		create_dma_descriptors_rx(dev, (1 + REG_READ(RXFTLR)), 0);
		if (dev->rx_size > (dev->rx_count << dev->log_xfer_size)) {
			create_dma_descriptors_rx(dev, (1 + REG_READ(RXFTLR)), 4);
		}

		/* init dma */
		if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
			_dma_chan_desc(dev->dmarxchanid,
				       &(dev->dmarxdescriptor[0]));
		} else {
			_dma_chan_desc_aux(dev->dmarxchanid,
					   &(dev->dmarxdescriptor[0]));
		}

		_dma_chan_enable((0x1 << dev->dmarxchanid), 1);

		if (REG_GET_BITS(CTRL0, 2, 8) == SPI_RECEIVE_ONLY_MODE) {
			/* Write dummy to trigger the transfer */
			REG_WRITE(DR, 0xdeadbeef);
		}
	}
}
#endif
}

void io_spi_master_write(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t val = 0;

	dev->tx_size = *size;
	dev->tx_count = 0;
	dev->tx_buff = (void *) data;

	/* modify rx_size when in TXRX mode */
	if (REG_GET_BITS(CTRL0, 2, 8) == SPI_TRANSMIT_RECEIVE_MODE) {
		/* if receive buffer not completely filled yet, it contains tx-size of data */
		if (dev->rx_size > dev->tx_size) {
			dev->rx_size = *(dev->p_rxsize) = dev->tx_size;
		}
#ifdef __Xdmac
		if ((dev->dmatxchanid != DMA_NONE) && (dev->rx_size != 0)) {
			/* create RX DMA descriptors, now the size is known */
			create_dma_descriptors_rx(dev, (1 + REG_READ(RXFTLR)), 0);
			if (dev->rx_size > (dev->rx_count << dev->log_xfer_size)) {
				create_dma_descriptors_rx(dev, (1 + REG_READ(RXFTLR)), 4);
			}

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
#endif
	}

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
	} else {        /* TX-RX mode or EEPROM mode */
		val = REG_READ(IMR);
		REG_WRITE(IMR, (val | R_TX_EMPTY | R_IDLE | R_RX_FULL));
	}
#ifdef __Xdmac
} else {                /* TX descriptor */
	if (*size != 0) {
		/* tx data available; at least one, more is uncertain */
		create_dma_descriptors_tx(dev,
					  (dev->fifo_depth - REG_READ(TXFTLR)),
					  0);
		if (dev->tx_size > (dev->tx_count << dev->log_xfer_size)) {
			create_dma_descriptors_tx(dev,
						  (dev->fifo_depth -
						   REG_READ(TXFTLR)), 4);
		}

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

void io_spi_master_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
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

	case IO_SPI_MASTER_SET_SE:
		REG_WRITE(SER, *((uint32_t *) arg));
		break;
	case IO_SPI_MASTER_SET_RX_THRESHOLD:
		REG_WRITE(RXFTLR, *((uint32_t *) arg));
		break;
	case IO_SPI_MASTER_SET_TX_THRESHOLD:
		REG_WRITE(TXFTLR, *((uint32_t *) arg));
		break;
	case IO_SPI_MASTER_GET_XFER_SIZE:
		*((uint32_t *) arg) = (8 << dev->log_xfer_size);
		break;

	case IO_SPI_MASTER_SET_PACKING:
	{                                                               /* DFS must be set before applying this option */
		if (*((uint32_t *) arg) == 0) {                         /* reset packing */
			dev->log_xfer_size = dev->max_xfer_size >> 4;
		} else {                                                /* set packing */
			regval = REG_READ(CTRL0);
			if (dev->max_xfer_size == 32) {                 /* 32-bits SPI */
				regval = (regval >> 16) & 0x01F;        // Get DFS field from CTRL0 value
				dev->log_xfer_size =
					(regval == 15) ? 1 : (regval == 7) ? 0 : 2;
			} else {                                /* 16-bits SPI */
				regval = regval & 0x00F;        // Get DFS field from CTRL0 value
				dev->log_xfer_size = (regval == 7) ? 0 : 1;
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
		case IO_SPI_MASTER_SET_BAUD:
			REG_WRITE(BAUDR, *((uint32_t *) arg));
			break;
		case IO_SPI_MASTER_SET_RX_DELAY:
			REG_WRITE(RX_SAMPLE_DLY, *((uint32_t *) arg));
			break;
		case IO_SPI_MASTER_SET_TRANSFER_MODE:
			REG_WRITE_BITS(CTRL0, regval, *((uint32_t *) arg), 2, 8);
			break;
		case IO_SPI_MASTER_SET_SCPH:
			REG_WRITE_BITS(CTRL0, regval, *((uint32_t *) arg), 1, 6);
			break;
		case IO_SPI_MASTER_SET_SCPL:
			REG_WRITE_BITS(CTRL0, regval, *((uint32_t *) arg), 1, 7);
			break;
		case IO_SPI_MASTER_SET_SRL_MODE:
			REG_WRITE_BITS(CTRL0, regval, *((uint32_t *) arg), 1, 11);
			break;

		case IO_SPI_MASTER_SET_DFS:
			REG_WRITE_BITS(CTRL0, regval, ((*((uint32_t *) arg)) - 1),
				       (3 + dev->log_xfer_size),
				       ((dev->log_xfer_size - 1) << 4));
			break;

		default:
			break;
		}

		while (REG_READ(SPIEN) != 0) {
			;
		}
		REG_WRITE(SPIEN, 0x1);

		break;
	}
	}
}

static void spi_mst_rx_ISR_proc(uint32_t dev_id)
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

	/* Enable Idle interrupt */
	val = REG_READ(IMR);
	REG_WRITE(IMR, (val | R_IDLE));
}
#endif
}

static void spi_mst_tx_ISR_proc(uint32_t dev_id)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t val = 0;

#ifdef __Xdmac
	if (dev->dmatxchanid == DMA_NONE) {
#endif
	if ((dev->tx_size <= (dev->tx_count << dev->log_xfer_size))) {
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

	/* Enable Idle interrupt */
	val = REG_READ(IMR);
	REG_WRITE(IMR, (val | R_IDLE));
}
#endif
}

static void spi_mst_idle_ISR_proc(uint32_t dev_id)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint32_t val = 0;

	if (dev->handling_tx == 1) {
		dev->handling_tx = 0;

		if (NULL != dev->tx_cb) {
			dev->tx_cb(dev_id);
		}

		/* mask TX-empty (if not done yet) and Idle interrupt */
		val = REG_READ(IMR);
		REG_WRITE(IMR, (val & ~R_TX_EMPTY & ~R_IDLE));

	}

	if (dev->handling_rx == 1) {
		dev->handling_rx = 0;

#ifdef __Xdmac
		if (dev->dmarxchanid == DMA_NONE) {
#endif
		/* read remaining data from fifo */
		retrieve_rxfifo(dev);
#ifdef __Xdmac
	}
#endif

		if (NULL != dev->rx_cb) {
			dev->rx_cb(dev_id);
		}

		/* mask RX-available (if not done yet) and Idle interrupt */
		val = REG_READ(IMR);
		REG_WRITE(IMR, (val & ~R_RX_FULL & ~R_IDLE));
	}

}

static void spi_mst_err_ISR_proc(uint32_t dev_id)
{
	spi_info_pt dev = spi_handles[dev_id];

	REG_WRITE(IMR, 0);

	if (NULL != dev->err_cb) {
		dev->err_cb(dev_id);
	}

	REG_WRITE(ICR, R_RX_OVER | R_RX_UNDER | R_TX_OVER);
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

	} else {            /* log_xfer_size == 2; 32-bits data */
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

void io_spi_master_polling(uint32_t dev_id, DEV_SPI_TRANSFER *xfer)
{
	spi_info_pt dev = spi_handles[dev_id];
	uint8_t tmp;
	uint32_t tx_left, tx_free;
	uint32_t rx_left, rx_avail;

	REG_WRITE(SPIEN, 0);
	REG_WRITE(SPIEN, 1); /* disable then enable SPI to reset the TX and RX FIFO */

	while (xfer->tx_idx < xfer->tot_len || xfer->rx_idx < xfer->tot_len) {
		tx_left = (xfer->tot_len - xfer->tx_idx);
		tx_free = dev->fifo_depth - REG_READ(TXFLR);
		if (tx_free > tx_left) {
			tx_free = tx_left;
		}
		while (tx_free) {
			if (xfer->tx_idx >= xfer->tx_ofs && xfer->tx_idx < xfer->tx_totlen) {
				REG_WRITE(DR, xfer->tx_buf[xfer->tx_idx]);
			} else {
				REG_WRITE(DR, 0xFF);
			}
			xfer->tx_idx++;
			tx_free--;
		}
		rx_left = (xfer->tot_len - xfer->rx_idx);
		rx_avail = REG_READ(RXFLR);
		if (rx_avail > rx_left) {
			rx_avail = rx_left;
		}
		while (rx_avail) {
			if (xfer->rx_idx >= xfer->rx_ofs && xfer->rx_idx < xfer->rx_totlen) {
				xfer->rx_buf[xfer->rx_idx - xfer->rx_ofs] = (uint8_t)REG_READ(DR);
			} else {
				tmp = (uint8_t)REG_READ(DR);
			}
			xfer->rx_idx++;
			rx_avail--;
		}
	}
}

#ifdef __Xdmac
#define     SPI_MST_DMA_RX_CTRL            (0x80000017) /* am=b10, i=b0, dw/inc=b..., dtt=b10, r=b1, op=b11 */
#define     SPI_MST_DMA_RX_END_CTRL        (0xa0000015) /* am=b10, i=b1, dw/inc=b..., dtt=b10, r=b1, op=b01 */
#define     SPI_MST_DMA_RX_REM_CTRL        (0xa0000015) /* am=b10, i=b1, dw/inc=b..., dtt=b10, r=b1, op=b01 */
#define     SPI_MST_DMA_TX_CTRL            (0x4000000f) /* am=b01, i=b0, dw/inc=b..., dtt=b01, r=b1, op=b11 */
#define     SPI_MST_DMA_TX_END_CTRL        (0x6000000d) /* am=b01, i=b1, dw/inc=b..., dtt=b01, r=b1, op=b01 */
#define     SPI_MST_DMA_TX_REM_CTRL        (0x6000000d) /* am=b01, i=b1, dw/inc=b..., dtt=b01, r=b1, op=b01 */
#define     SPI_MST_DMA_CTRL_DWINC_POS     (5)
#define     SPI_MST_DMA_CTRL_SIZE_POS      (8)
#define     SPI_MST_DMA_CTRL_XFER_POS      (21)
#define     SPI_MST_DMA_FORCE_INT          (0x1 << 29)

static void create_dma_descriptors_rx(spi_info_pt dev, uint32_t burst,
				      uint8_t idx)
{
	uint32_t divisor = 0, remainder = 0;
	uint32_t ctrl = 0;

	ctrl = burst << SPI_MST_DMA_CTRL_XFER_POS;
	if (dev->log_xfer_size == 0) {
		ctrl |= (0x0 << SPI_MST_DMA_CTRL_DWINC_POS);
	} else if (dev->log_xfer_size == 1) {
		ctrl |= (0x3 << SPI_MST_DMA_CTRL_DWINC_POS);
	} else {            /* dev->log_xfer_size == 2 */

		ctrl |= (0x5 << SPI_MST_DMA_CTRL_DWINC_POS);
	}

	if ((dev->rx_size - (dev->rx_count << dev->log_xfer_size)) <= 8 * 1024) {
		divisor = (dev->rx_size >> dev->log_xfer_size) - dev->rx_count;
		remainder =
			dev->rx_size -
			((dev->rx_size >> dev->log_xfer_size) << dev->log_xfer_size);

		if ((divisor > 0) && (remainder == 0)) {
			dev->dmarxdescriptor[idx + 0] =
				SPI_MST_DMA_RX_END_CTRL | ctrl |
				(((dev->rx_size - (dev->rx_count << dev->log_xfer_size)) -
				  1) << SPI_MST_DMA_CTRL_SIZE_POS);
			dev->rx_count += divisor;
			dev->dmarxdescriptor[idx + 2] =
				(uint32_t) dev->rx_buff +
				((dev->rx_count << dev->log_xfer_size) - 1);
		} else if ((divisor > 0) && (remainder > 0)) {
			dev->dmarxdescriptor[idx + 0] =
				SPI_MST_DMA_RX_CTRL | ctrl |
				(((dev->rx_size - (dev->rx_count << dev->log_xfer_size)) -
				  1 - remainder) << SPI_MST_DMA_CTRL_SIZE_POS);
			dev->rx_count += divisor;
			dev->dmarxdescriptor[idx + 2] =
				(uint32_t) dev->rx_buff +
				((dev->rx_count << dev->log_xfer_size) - 1);
		} else {    /* divisor == 0, remainder > 0 */

			dev->dmarxdescriptor[idx + 0] =
				SPI_MST_DMA_RX_REM_CTRL | ctrl |
				((2 * dev->log_xfer_size -
				  1) << SPI_MST_DMA_CTRL_SIZE_POS);
			dev->rx_count++;
			if (dev->log_xfer_size == 1) {
				dev->dmarxdescriptor[idx + 2] =
					(uint32_t) &(dev->dmarx_last16) + 1;
			} else {
				dev->dmarxdescriptor[idx + 2] =
					(uint32_t) &(dev->dmarx_last32) + 3;
			}
		}
	}
}

static void create_dma_descriptors_tx(spi_info_pt dev, uint32_t burst,
				      uint8_t idx)
{
	uint32_t divisor = 0, remainder = 0;
	uint32_t ctrl = 0;

	ctrl = burst << SPI_MST_DMA_CTRL_XFER_POS;
	if (dev->log_xfer_size == 0) {
		ctrl |= (0x0 << SPI_MST_DMA_CTRL_DWINC_POS);
	} else if (dev->log_xfer_size == 1) {
		ctrl |= (0x3 << SPI_MST_DMA_CTRL_DWINC_POS);
	} else {            /* dev->log_xfer_size == 2 */

		ctrl |= (0x5 << SPI_MST_DMA_CTRL_DWINC_POS);
	}

	if ((dev->tx_size - (dev->tx_count << dev->log_xfer_size)) <= 8 * 1024) {
		divisor = (dev->tx_size >> dev->log_xfer_size) - dev->tx_count;
		remainder =
			dev->tx_size -
			((dev->tx_size >> dev->log_xfer_size) << dev->log_xfer_size);

		if (dev->log_xfer_size == 1) {
			dev->dmatx_last16 =
				((uint16_t *) (dev->tx_buff))[(dev->tx_size >> dev->
							       log_xfer_size)] & ~((~0x0)
										   <<
										   (remainder
										    * 8));
		} else if (dev->log_xfer_size == 2) {
			dev->dmatx_last32 =
				((uint32_t *) (dev->tx_buff))[(dev->tx_size >> dev->
							       log_xfer_size)] & ~((~0x0)
										   <<
										   (remainder
										    * 8));
		}

		if ((divisor > 0) && (remainder == 0)) {
			dev->dmatxdescriptor[idx + 0] =
				SPI_MST_DMA_TX_END_CTRL | ctrl |
				((dev->tx_size - (dev->tx_count << dev->log_xfer_size) -
				  1) << SPI_MST_DMA_CTRL_SIZE_POS);
			dev->tx_count += divisor;
			dev->dmatxdescriptor[idx + 1] =
				(uint32_t) dev->tx_buff +
				((dev->tx_count << dev->log_xfer_size) - 1);
		} else if ((divisor > 0) && (remainder > 0)) {
			dev->dmatxdescriptor[idx + 0] =
				SPI_MST_DMA_TX_CTRL | ctrl |
				((dev->tx_size - (dev->tx_count << dev->log_xfer_size) -
				  1 - remainder) << SPI_MST_DMA_CTRL_SIZE_POS);
			dev->tx_count += divisor;
			dev->dmatxdescriptor[idx + 1] =
				(uint32_t) dev->tx_buff +
				((dev->tx_count << dev->log_xfer_size) - 1);
		} else {    /* divisor == 0, remainder > 0 */

			dev->dmatxdescriptor[idx + 0] =
				SPI_MST_DMA_TX_REM_CTRL | ctrl |
				((2 * dev->log_xfer_size -
				  1) << SPI_MST_DMA_CTRL_SIZE_POS);
			dev->tx_count++;
			if (dev->log_xfer_size == 1) {
				dev->dmatxdescriptor[idx + 1] =
					(uint32_t) &(dev->dmatx_last16) + 1;
			} else {
				dev->dmatxdescriptor[idx + 1] =
					(uint32_t) &(dev->dmatx_last32) + 3;
			}
		}
	}
}
#endif

#endif
