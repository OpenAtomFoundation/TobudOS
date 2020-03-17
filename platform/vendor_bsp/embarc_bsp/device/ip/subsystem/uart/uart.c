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
#include "device/subsystem/uart.h"
#include "io_config.h"
#include "apexextensions.h"

/* APEX UART device registers  */
#define UART_RBR    (0x00)
#define UART_THR    (0x00)
#define UART_DLL    (0x00)
#define UART_IER    (0x04)
#define UART_DLH    (0x04)
#define UART_IIR    (0x08)
#define UART_FCR    (0x08)
#define UART_LCR    (0x0c)
#define UART_MCR    (0x10)
#define UART_LSR    (0x14)      // unused
#define UART_MSR    (0x18)      // unused
#define UART_USR    (0x7c)      // unused
#define UART_CLKEN  (0xc0)

#define DMA_NONE    (0xff)

#define FLOW_CONTROL_OFF            0   // Flow Control is OFF
#define FLOW_CONTROL_ON             1   // Using software implementation of CTS/RTS handling

#define nCTS_LOW                    0
#define nCTS_HIGH                   1

typedef _Interrupt void (*ISR) ();

/* Private data structure maintained by the driver. */
typedef struct uart_info_struct {
	uint32_t reg_base;      /* base address of device register set */
	/* TX & RX Buffer and lengths */
	uint8_t *tx_data;
	uint32_t tx_size;
	uint32_t tx_count;
	uint8_t *rx_data;
	uint32_t rx_size, *p_rxsize;
	uint32_t rx_count;

	uint8_t instID;
	uint16_t dmarxchanid, dmatxchanid;
#ifdef __Xdmac
	_Uncached uint32_t *dmarxdescriptor, *dmatxdescriptor;
	uint8_t dmarxidx;
	uint8_t dmatxidx;
#endif
	/* Callbacks */
	IO_CB_FUNC tx_cb;
	IO_CB_FUNC rx_cb;
	IO_CB_FUNC err_cb;
	/* Interrupt numbers and handlers */
	uint8_t isr_vector;     /* ISR vector */
	ISR uart_isr;           /* UART device ISRs */
	ISR uart_dmatx_isr;     /* UART DMA TX ISRs */
	ISR uart_dmarx_isr;     /* UART DMA RX ISRs */

	uint32_t fifo_depth, rx_threshold, tx_threshold;
	uint8_t fcr_mirror;
	uint8_t flow_control;
	uint8_t nCTS_state;
} uart_info_t, *uart_info_pt;

#ifdef IO_UART0_PRESENT
#define UART_DEV_PRESENT
#endif
#ifdef IO_UART1_PRESENT
#define UART_DEV_PRESENT
#endif
#ifdef IO_UART2_PRESENT
#define UART_DEV_PRESENT
#endif
#ifdef IO_UART3_PRESENT
#define UART_DEV_PRESENT
#endif

#ifdef UART_DEV_PRESENT

#define UART_MAX_CNT    (4)

static void uart_isr_proc(uint32_t dev_id);
static void uart_dmatx_isr_proc(uint32_t dev_id);
static void uart_dmarx_isr_proc(uint32_t dev_id);

#ifdef IO_UART0_PRESENT
static _Interrupt void uart0_ISR()
{
	uart_isr_proc(0);
}

static _Interrupt void uart0_dmatx_ISR()
{
	uart_dmatx_isr_proc(0);
}

static _Interrupt void uart0_dmarx_ISR()
{
	uart_dmarx_isr_proc(0);
}
#endif
#ifdef IO_UART1_PRESENT
static _Interrupt void uart1_ISR()
{
	uart_isr_proc(1);
}

static _Interrupt void uart1_dmatx_ISR()
{
	uart_dmatx_isr_proc(1);
}

static _Interrupt void uart1_dmarx_ISR()
{
	uart_dmarx_isr_proc(1);
}
#endif
#ifdef IO_UART2_PRESENT
static _Interrupt void uart2_ISR()
{
	uart_isr_proc(2);
}

static _Interrupt void uart2_dmatx_ISR()
{
	uart_dmatx_isr_proc(2);
}

static _Interrupt void uart2_dmarx_ISR()
{
	uart_dmarx_isr_proc(2);
}
#endif
#ifdef IO_UART3_PRESENT
static _Interrupt void uart3_ISR()
{
	uart_isr_proc(3);
}

static _Interrupt void uart3_dmatx_ISR()
{
	uart_dmatx_isr_proc(3);
}

static _Interrupt void uart3_dmarx_ISR()
{
	uart_dmarx_isr_proc(3);
}
#endif

#ifdef __Xdmac
static void create_dma_descriptor_rx(uart_info_pt dev, uint32_t burst,
				     uint8_t idx);
static void create_dma_descriptor_tx(uart_info_pt dev, uint32_t burst,
				     uint8_t idx);
#endif

/* UART devices private data structures */
static uart_info_pt uart_handles[UART_MAX_CNT] = { 0 };

#ifdef __Xdmac
static _Uncached uint32_t uart_dmarx_descriptors[8 * UART_MAX_CNT];
static _Uncached uint32_t uart_dmatx_descriptors[8 * UART_MAX_CNT];
#endif

static uart_info_t uart_devs[] = {
#ifdef IO_UART0_PRESENT
	{ .instID = 0,
	  .reg_base = AR_IO_UART0_RBR_THR_DLL,
	  .isr_vector = IO_UART0_INTR,
	  .uart_isr = uart0_ISR,
	  .uart_dmatx_isr = uart0_dmatx_ISR,
	  .uart_dmarx_isr = uart0_dmarx_ISR,
#ifdef IO_UART0_DMA_RX
	  .dmarxchanid = IO_UART0_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_UART0_DMA_TX
	  .dmatxchanid = IO_UART0_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .fifo_depth = IO_UART0_FS },
#endif
#ifdef IO_UART1_PRESENT
	{ .instID = 1,
	  .reg_base = AR_IO_UART1_RBR_THR_DLL,
	  .isr_vector = IO_UART1_INTR,
	  .uart_isr = uart1_ISR,
	  .uart_dmatx_isr = uart1_dmatx_ISR,
	  .uart_dmarx_isr = uart1_dmarx_ISR,
#ifdef IO_UART1_DMA_RX
	  .dmarxchanid = IO_UART1_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_UART1_DMA_TX
	  .dmatxchanid = IO_UART1_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .fifo_depth = IO_UART1_FS },
#endif
#ifdef IO_UART2_PRESENT
	{ .instID = 2,
	  .reg_base = AR_IO_UART2_RBR_THR_DLL,
	  .isr_vector = IO_UART2_INTR,
	  .uart_isr = uart2_ISR,
	  .uart_dmatx_isr = uart2_dmatx_ISR,
	  .uart_dmarx_isr = uart2_dmarx_ISR,
#ifdef IO_UART2_DMA_RX
	  .dmarxchanid = IO_UART2_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_UART2_DMA_TX
	  .dmatxchanid = IO_UART2_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .fifo_depth = IO_UART2_FS },
#endif
#ifdef IO_UART3_PRESENT
	{ .instID = 3,
	  .reg_base = AR_IO_UART3_RBR_THR_DLL,
	  .isr_vector = IO_UART3_INTR,
	  .uart_isr = uart3_ISR,
	  .uart_dmatx_isr = uart3_dmatx_ISR,
	  .uart_dmarx_isr = uart3_dmarx_ISR,
#ifdef IO_UART3_DMA_RX
	  .dmarxchanid = IO_UART3_DMA_RX,
#else
	  .dmarxchanid = DMA_NONE,
#endif
#ifdef IO_UART3_DMA_TX
	  .dmatxchanid = IO_UART3_DMA_TX,
#else
	  .dmatxchanid = DMA_NONE,
#endif
	  .fifo_depth = IO_UART3_FS },
#endif
	{ .instID = UART_MAX_CNT }
};

#define     REG_WRITE(reg, x)     _sr((unsigned)(x), (unsigned)(dev->reg_base + reg))
#define     REG_READ(reg)         _lr((unsigned)(dev->reg_base + reg))

uint32_t io_uart_open(uint32_t dev_id)
{
	uint32_t h = 0;
	uint32_t val = 0;
	uart_info_pt dev;

	h = 0;
	while ((uart_devs[h].instID != dev_id)
	       && (uart_devs[h].instID != UART_MAX_CNT)) {
		h++;
	}
	if ((uart_devs[h].instID == UART_MAX_CNT) || (0 != uart_handles[dev_id])) { /* dev_id not part of design, or still open */
		return 1;
	}
	uart_handles[dev_id] = &uart_devs[h];
	dev = uart_handles[dev_id];

	REG_WRITE(UART_CLKEN, 0x1);
	REG_WRITE(UART_IER, 0x0);
	REG_READ(UART_RBR);

	/* when there are fifos available, always enable and reset these */
	if (dev->fifo_depth != 0) {
		REG_WRITE(UART_FCR, 0x07);
		dev->fcr_mirror = 0;
		dev->rx_threshold = 1;
		dev->tx_threshold = 0;
	}

	/* save nCTS state */
	val = REG_READ(UART_MSR);
	dev->nCTS_state = (val & 0x10) ? nCTS_LOW : nCTS_HIGH;

	_setvecti(dev->isr_vector, dev->uart_isr);

#ifdef __Xdmac
	dev->dmarxdescriptor = &uart_dmarx_descriptors[8 * h];
	dev->dmatxdescriptor = &uart_dmatx_descriptors[8 * h];

	if (dev->dmarxchanid != DMA_NONE) {
		dev->dmarxdescriptor[1] = dev->dmarxdescriptor[4 + 1] =
			dev->reg_base + UART_RBR;
		dev->dmarxdescriptor[3] = (uint32_t) &(dev->dmarxdescriptor[4]);
		dev->dmarxdescriptor[4 + 3] =
			(uint32_t) &(dev->dmarxdescriptor[0]);

		_setvecti(DMAC_INT_BASE + dev->dmarxchanid, dev->uart_dmarx_isr);
		_setvecti(DMAC_ERR_BASE + dev->dmarxchanid, dev->uart_isr);
	}

	if (dev->dmatxchanid != DMA_NONE) {
		dev->dmatxdescriptor[2] = dev->dmatxdescriptor[4 + 2] =
			dev->reg_base + UART_THR;
		dev->dmatxdescriptor[3] = (uint32_t) &(dev->dmatxdescriptor[4]);
		dev->dmatxdescriptor[4 + 3] =
			(uint32_t) &(dev->dmatxdescriptor[0]);

		_setvecti(DMAC_INT_BASE + dev->dmatxchanid, dev->uart_dmatx_isr);
		_setvecti(DMAC_ERR_BASE + dev->dmatxchanid, dev->uart_isr);
	}
#endif

	return 0;
}

void io_uart_close(uint32_t dev_id)
{
	uart_info_pt dev = uart_handles[dev_id];

	// bring uart instance to reset state
	if (dev->fifo_depth != 0) {
		REG_WRITE(UART_FCR, 0x07);      // reset FIFOs
	}
	REG_WRITE(UART_IER, 0x0);               // clear interrupt enable register

	/* gate the UART clock */
	REG_WRITE(UART_CLKEN, 0x0);

	_setvecti(dev->isr_vector, NULL);

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

	uart_handles[dev_id] = 0;
}

void io_uart_read(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	uart_info_pt dev = uart_handles[dev_id];
	uint32_t val = 0;

	dev->rx_data = data;
	dev->rx_size = *size;
	dev->p_rxsize = size;
	dev->rx_count = 0;

#ifdef __Xdmac
	if (dev->dmarxchanid == DMA_NONE) {
#endif
	if (dev->rx_size > 0) {
		if ((dev->fifo_depth != 0) && (dev->rx_size < dev->rx_threshold)) {
			/* temporary reset the rx_threshold to 1 */
			REG_WRITE(UART_FCR, (((dev->fcr_mirror & ~0xC) << 4) | 0x01));
		}
		/* enable ERBFI and ELSI interrupt */
		val = REG_READ(UART_IER) | 0x5;
		REG_WRITE(UART_IER, val);
	}
#ifdef __Xdmac
} else {                                        /* DMA: create descriptor */
	if (dev->rx_size != 0) {
		if (dev->fifo_depth == 0) {     /* fifos are not available */
			val = 1;
		} else {
			val = dev->rx_threshold;
		}
		create_dma_descriptor_rx(dev, val, 0);
		if (dev->rx_size != dev->rx_count) {
			create_dma_descriptor_rx(dev, val, 4);
		}

		/* init dma */
		if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
			_dma_chan_desc(dev->dmarxchanid,
				       &(dev->dmarxdescriptor[0]));
		} else {
			_dma_chan_desc_aux(dev->dmarxchanid,
					   &(dev->dmarxdescriptor[0]));
		}

		dev->dmarxidx = 0;

		_dma_chan_enable((0x1 << dev->dmarxchanid), 1);

		/* enable ELSI interrupt */
		val = REG_READ(UART_IER) | 0x4;
		REG_WRITE(UART_IER, val);
	}
}
/*
 * De-assert nRTS line if flow control is enabled
 * Note that UART controller inverts the MCR bit value
 */
if (dev->flow_control == FLOW_CONTROL_ON) {
	val = REG_READ(UART_MCR) | 0x2;
	REG_WRITE(UART_MCR, val);
}
#endif
}

void io_uart_write(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	uart_info_pt dev = uart_handles[dev_id];
	uint32_t cnt = 0;
	uint32_t h = 0;
	uint32_t val = 0;

	dev->tx_data = data;
	dev->tx_size = *size;
	dev->tx_count = 0;

#ifdef __Xdmac
	if (dev->dmatxchanid == DMA_NONE) {
#endif
	/* If...
	 *   - flow control is disabled or
	 *   - flow control is enabled && nCTS is already LOW
	 * ...start data transmit immediately, wait for nCTS change IRQ otherwise  */
	if ((dev->flow_control == FLOW_CONTROL_OFF) ||
	    ((dev->flow_control == FLOW_CONTROL_ON)
	     && (dev->nCTS_state == nCTS_LOW))) {
		/* write first bytes to fifo (if any) */
		cnt = dev->tx_size - dev->tx_count;
		if (cnt > 0) {
			if (dev->fifo_depth == 0) {             /* fifos are not available */
				cnt = 1;
			} else if (cnt > dev->fifo_depth) {     /* fifos enabled (and by definition != FIFO_NONE) */
				cnt = dev->fifo_depth;
			}
			for (h = 0; h != cnt; h++) {
				REG_WRITE(UART_THR, (dev->tx_data[dev->tx_count++]));
			}
			/* enable ETBEI interrupt and enable use of interrupt for TX threshold */
			val = REG_READ(UART_IER) | 0x82;
			REG_WRITE(UART_IER, val);
		}
	}
#ifdef __Xdmac
} else {
	/* DMA: create descriptor */
	if (dev->fifo_depth == 0) { /* fifos are not available */
		cnt = 1;
	} else {
		cnt = dev->fifo_depth - dev->tx_threshold;
	}
	create_dma_descriptor_tx(dev, cnt, 0);
	if (dev->tx_size != dev->tx_count) {
		create_dma_descriptor_tx(dev, cnt, 4);
	}
	/* init dma */
	if (dev->dmatxchanid < DMAC_MEM_CHAN_CNT) {
		_dma_chan_desc(dev->dmatxchanid, &(dev->dmatxdescriptor[0]));
	} else {
		_dma_chan_desc_aux(dev->dmatxchanid,
				   &(dev->dmatxdescriptor[0]));
	}

	dev->dmatxidx = 0;

	/* If nCTS is already LOW and flow control is enabled - start data transmit immediately,
	 * wait for nCTS change IRQ otherwise  */
	if (!
	    ((dev->flow_control == FLOW_CONTROL_ON)
	     && (dev->nCTS_state == nCTS_HIGH))) {
		_dma_chan_enable((0x1 << dev->dmatxchanid), 1);
	}
}
#endif
}

void io_uart_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
	uart_info_pt dev = uart_handles[dev_id];
	uint32_t val = 0;

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
#if 0
	case IO_UART_SET_MCR:   // not part of public API (yet)
		REG_WRITE(UART_MCR, *((uint32_t *) arg));
		break;
	case IO_UART_GET_MCR:   // not part of public API (yet)
		*((uint32_t *) arg) = REG_READ(UART_MCR);
		break;
#endif
	case IO_UART_SET_LCR:
		/* leave the LDAB bit cleared */
		REG_WRITE(UART_LCR, (*((uint32_t *) arg) & 0x7f));
		break;
	case IO_UART_GET_LCR:
		*((uint32_t *) arg) = REG_READ(UART_LCR);
		break;
	case IO_UART_SET_DLx:
		/* 16 bit divisor --> DLL (lower 8) and DLH (upper 8) registers */
		/* set LDAB bit for DLx register access, first */
		val = REG_READ(UART_LCR);
		REG_WRITE(UART_LCR, (val | (0x1 << 7)));
		REG_WRITE(UART_DLL, (*((uint32_t *) arg) & 0x00ff));
		REG_WRITE(UART_DLH, ((*((uint32_t *) arg) & 0xff00) >> 8));
		REG_WRITE(UART_LCR, val);
		break;
	case IO_UART_GET_DLx:
		/* set LDAB bit for DLx register access, first */
		val = REG_READ(UART_LCR);
		REG_WRITE(UART_LCR, (val | (0x1 << 7)));
		*((uint32_t *) arg) =
			(REG_READ(UART_DLL) | (REG_READ(UART_DLH) << 8));
		REG_WRITE(UART_LCR, val);
		break;
	case IO_UART_SET_RX_THRESHOLD:
		/* precondition: can only be called for dev->fifo_depth != 0 */
		val = *((uint32_t *) arg);
		dev->fcr_mirror = (dev->fcr_mirror & ~0xC) | (val << 2);
		REG_WRITE(UART_FCR, ((dev->fcr_mirror << 4) | 0x01));
		if (val == 0) {
			dev->rx_threshold = 1;
		} else if (val == 1) {
			dev->rx_threshold = dev->fifo_depth >> 2;
		} else if (val == 2) {
			dev->rx_threshold = dev->fifo_depth >> 1;
		} else if (val == 3) {
			dev->rx_threshold = dev->fifo_depth - 2;
		}
		break;
	case IO_UART_SET_TX_THRESHOLD:
		/* precondition: can only be called for dev->fifo_depth != 0 */
		val = *((uint32_t *) arg);
		dev->fcr_mirror = (dev->fcr_mirror & ~0x3) | val;
		REG_WRITE(UART_FCR, ((dev->fcr_mirror << 4) | 0x01));
		if (val == 0) {
			dev->tx_threshold = 0;
		} else if (val == 1) {
			dev->tx_threshold = 2;
		} else if (val == 2) {
			dev->tx_threshold = dev->fifo_depth >> 2;
		} else if (val == 3) {
			dev->tx_threshold = dev->fifo_depth >> 1;
		}
		break;
	case IO_UART_SET_FLOW_CONTROL:
		if ((uint32_t) (*((uint32_t *) arg)) == 1) {
			// Enable Flow Control
			dev->flow_control = FLOW_CONTROL_ON;    // Use software implementation of CTS/RTS control
			val = REG_READ(UART_IER) | 0x08;
			REG_WRITE(UART_IER, val);               // Enable modem status interrupt

			val = REG_READ(UART_MSR);
			if (val & 0x10) {
				dev->nCTS_state = nCTS_LOW;
			} else {
				dev->nCTS_state = nCTS_HIGH;
			}
		} else {
			// Diable Flow Control
			dev->flow_control = FLOW_CONTROL_OFF;
			val = REG_READ(UART_IER) & ~0x08;
			REG_WRITE(UART_IER, val); // Disable modem status interrupt
		}
		break;
	default:
		break;
	}
}

static void uart_isr_proc(uint32_t dev_id)
{
	uart_info_pt dev = uart_handles[dev_id];
	uint32_t intid = 0;
	uint32_t cnt = 0;
	uint32_t h = 0;
	volatile uint32_t val = 0;

	intid = REG_READ(UART_IIR) & 0x0f;

	/* workaround for break detect: break interrupt may have been cleared by DMAC reading from rxfifo... */
	if (intid == 0x1) {     /* break detect + dma: copy source from while loop */
		/* line break detected; complete read immediately */
#ifdef __Xdmac
		if (dev->dmatxchanid == DMA_NONE) {
#endif
		*(dev->p_rxsize) = dev->rx_count;
		REG_READ(UART_RBR);
#ifdef __Xdmac
	} else {
		_dma_chan_enable((0x1 << dev->dmarxchanid), 0);
		if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
			_dma_chan_desc_get_ctrl(dev->dmarxchanid, &val);
		} else {
			_dma_chan_desc_aux_get_ctrl(dev->dmarxchanid, &val);
		}
		*(dev->p_rxsize) = dev->rx_count - 1 - ((val >> 8) & 0x1fff);
		*(dev->p_rxsize) -= 1;
	}
#endif
		/* disable (ERBFI and) ELSI interrupt */
		val = REG_READ(UART_IER) & ~0x5;
		REG_WRITE(UART_IER, val);
		if (dev->fifo_depth != 0) { /* restore threshold to user defined value */
			REG_WRITE(UART_FCR, ((dev->fcr_mirror << 4) | 0x01));
		}
		/* call user callback function, if any */
		if (dev->rx_cb != 0) {
			dev->rx_cb(dev_id);
		}
	}

	while (intid != 0x1) {
		switch (intid) {
		case 0x2:
		{           /* tx empty */
			if (dev->tx_data == NULL) {
				if (dev->tx_cb != 0) {
					dev->tx_cb(dev_id);
				} else {
					val = REG_READ(UART_IER) & ~0x82;
					REG_WRITE(UART_IER, val);
				}
			} else {
				if (dev->tx_count == dev->tx_size) { /* disable ETBEI interrupt (and disable threshold interrupt IER[7]) */
					val = REG_READ(UART_IER) & ~0x82;
					REG_WRITE(UART_IER, val);
					/* call user callback function, if any */
					if (dev->tx_cb != 0) {
						dev->tx_cb(dev_id);
					}
					dev->tx_size = 0; // indicates no TX pending
				} else {
					/* write next bytes to fifo (if any) */
					cnt = dev->tx_size - dev->tx_count;
					if (dev->fifo_depth == 0) {             /* fifos are not available */
						cnt = 1;
					} else if (cnt > dev->fifo_depth) {     /* fifos enabled (and by definition != FIFO_NONE) */
						cnt = dev->fifo_depth;
					}
					if (REG_READ(UART_IER) & 0x80) { /* if IER[7] is set, threshold shall be taken into account */
						if (cnt > (dev->fifo_depth - dev->tx_threshold)) {
							cnt = dev->fifo_depth - dev->tx_threshold;
						}
					}
					for (h = 0; h != cnt; h++) {
						REG_WRITE(UART_THR,
							  (dev->tx_data[dev->tx_count++]));
					}
				}
			}
			break;
		}
		case 0x4:
		{           /* rx data available; at least one, more is uncertain */
			if (dev->rx_data == NULL) {
				if (dev->rx_cb != 0) {
					dev->rx_cb(dev_id);
				} else {
					REG_READ(UART_RBR);
				}
			} else {
				if (dev->fifo_depth == 0) {
					dev->rx_data[dev->rx_count++] = REG_READ(UART_RBR);
				} else {
					cnt = dev->rx_size - dev->rx_count;
					if (cnt < dev->rx_threshold) {
						dev->rx_data[dev->rx_count++] = REG_READ(UART_RBR);
					} else {
						for (h = 0; h != dev->rx_threshold; h++) {
							dev->rx_data[dev->rx_count++] =
								REG_READ(UART_RBR);
						}
						cnt -= dev->rx_threshold;
						if (cnt < dev->rx_threshold) {
							/* temporary reset the rx_threshold to 1 */
							REG_WRITE(UART_FCR,
								  (((dev->
								     fcr_mirror & ~0xC) << 4) | 0x01));
						}
					}
				}
				if (dev->rx_count == dev->rx_size) { /* disable ERBFI and ELSI interrupt */
					val = REG_READ(UART_IER) & ~0x5;
					REG_WRITE(UART_IER, val);
					if (dev->fifo_depth != 0) { /* restore threshold to user defined value */
						REG_WRITE(UART_FCR,
							  ((dev->fcr_mirror << 4) | 0x01));
					}
					/* call user callback function, if any */
					if (dev->rx_cb != 0) {
						dev->rx_cb(dev_id);
					}
				}
			}
			break;
		}
		case 0x0:   // Modem control status
		{
			val = REG_READ(UART_MSR);
			if ((val & 0x10) != 0) {
				dev->nCTS_state = nCTS_LOW;
				// If there is a data buffer, it is time to start sending - nCTS turns low
				if (dev->tx_size > 0) {
#ifdef __Xdmac
					if (dev->dmatxchanid == DMA_NONE) {
#endif
					/* enable ETBEI interrupt - starting TX */
					val = REG_READ(UART_IER) | 0x2;
					REG_WRITE(UART_IER, val);
#ifdef __Xdmac
				} else {
					_dma_chan_enable((0x1 << dev->dmatxchanid), 1); // !!!!!
				}
#endif
				}
			} else {
				// nCTS was switched high back
				dev->nCTS_state = nCTS_HIGH;
			}

			break;
		}
		case 0x6:
		{
			val = REG_READ(UART_LSR);
			if (val & (0x1 << 4)) {
				/* line break detected; complete read immediately */
#ifdef __Xdmac
				if (dev->dmatxchanid == DMA_NONE) {
#endif
				*(dev->p_rxsize) = dev->rx_count;
				REG_READ(UART_RBR);
#ifdef __Xdmac
			} else {
				_dma_chan_enable((0x1 << dev->dmarxchanid), 0);
				if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
					_dma_chan_desc_get_ctrl(dev->dmarxchanid,
								&val);
				} else {
					_dma_chan_desc_aux_get_ctrl(dev->dmarxchanid,
								    &val);
				}
				*(dev->p_rxsize) =
					dev->rx_count - 1 - ((val >> 8) & 0x1fff);
				*(dev->p_rxsize) -= 1;
			}
#endif
				/* disable (ERBFI and) ELSI interrupt */
				val = REG_READ(UART_IER) & ~0x5;
				REG_WRITE(UART_IER, val);
				if (dev->fifo_depth != 0) { /* restore threshold to user defined value */
					REG_WRITE(UART_FCR,
						  ((dev->fcr_mirror << 4) | 0x01));
				}
				/* call user callback function, if any */
				if (dev->rx_cb != 0) {
					dev->rx_cb(dev_id);
				}
			}

			break;
		}
		case 0x7:
		{
			val = REG_READ(UART_USR);// update IIR busy status by read Uart Status Register
			break;
		}
		default:
		{           /* something wrong; at least, disable interrupts */
			REG_WRITE(UART_IER, 0x0);
			if (dev->err_cb != 0) {
				dev->err_cb(dev_id);
			}
			break;
		}
		}
		intid = REG_READ(UART_IIR) & 0x0f;
	}

}

static void uart_dmatx_isr_proc(uint32_t dev_id)
{
#ifdef __Xdmac
	uart_info_pt dev = uart_handles[dev_id];
	uint32_t cnt = 0;
	volatile uint32_t val = 0;

	if (dev->tx_count == dev->tx_size) {
		/* DMA TX xfer done */
		_dma_chan_enable((0x1 << dev->dmatxchanid), 0);
		_dma_int_clear((0x1 << dev->dmatxchanid));

		/* enable ETBEI interrupt, to detect FIFO empty for end of tx */
		val = REG_READ(UART_IER) | 0x2;
		REG_WRITE(UART_IER, val);
	} else {
		/* clear DMA interrupt */
		_dma_int_clear((0x1 << dev->dmatxchanid));

		if (dev->fifo_depth == 0) { /* fifos are not available */
			cnt = 1;
		} else {
			cnt = dev->fifo_depth - dev->tx_threshold;
		}
		create_dma_descriptor_tx(dev, cnt, dev->dmatxidx);

		dev->dmatxidx = 4 - dev->dmatxidx;
	}
#endif
}

static void uart_dmarx_isr_proc(uint32_t dev_id)
{
#ifdef __Xdmac
	uart_info_pt dev = uart_handles[dev_id];
	uint32_t val = 0;

	if (dev->rx_count == dev->rx_size) {
		/* Buffer transfer completed. Assert RTS line back if flow control is enabled */
		if (dev->flow_control == FLOW_CONTROL_ON) {
			val = REG_READ(UART_MCR) & ~0x2;
			REG_WRITE(UART_MCR, val);
		}
		/* DMA RX xfer done */
		_dma_chan_enable((0x1 << dev->dmarxchanid), 0);
		_dma_int_clear((0x1 << dev->dmarxchanid));

		/* call user callback function, if any */
		if (dev->rx_cb != NULL) {
			dev->rx_cb(dev_id);
		}
	} else {
		/* clear DMA interrupt */
		_dma_int_clear((0x1 << dev->dmarxchanid));

		if (dev->fifo_depth == 0) { /* fifos are not available */
			val = 1;
		} else {
			val = dev->rx_threshold;
		}
		create_dma_descriptor_rx(dev, val, dev->dmarxidx);

		dev->dmarxidx = 4 - dev->dmarxidx;
	}
#endif
}

#ifdef __Xdmac
#define UART_DMA_RX_END_CTRL        (0xa0000015)        /* am=b10, i=b1,             dw/inc=b000, dtt=b10, r=b1, op=b01 */
#define UART_DMA_RX_CTRL            (0x801fff17)        /* am=b10, i=b0, size=h1fff, dw/inc=b000, dtt=b10, r=b1, op=b11 */
#define UART_DMA_RX_INT_CTRL        (0xa01fff17)        /* am=b10, i=b1, size=h1fff, dw/inc=b000, dtt=b10, r=b1, op=b11 */
#define UART_DMA_TX_END_CTRL        (0x6000000d)        /* am=b01, i=b1,             dw/inc=b000, dtt=b01, r=b1, op=b01 */
#define UART_DMA_TX_CTRL            (0x401fff0f)        /* am=b01, i=b0, size=h1fff, dw/inc=b000, dtt=b01, r=b1, op=b11 */
#define UART_DMA_TX_INT_CTRL        (0x601fff0f)        /* am=b01, i=b1, size=h1fff, dw/inc=b000, dtt=b01, r=b1, op=b11 */
#define UART_DMA_CTRL_SIZE_POS      (8)
#define UART_DMA_CTRL_XFER_POS      (21)

/* use maximum block size of 4kB per DMA transfer */

static void create_dma_descriptor_rx(uart_info_pt dev, uint32_t burst,
				     uint8_t idx)
{
	dev->dmarxdescriptor[idx + 0] = (burst << UART_DMA_CTRL_XFER_POS);
	if ((dev->rx_size - dev->rx_count) <= 8 * 1024) {
		dev->dmarxdescriptor[idx + 0] |=
			UART_DMA_RX_END_CTRL | (((dev->rx_size - dev->rx_count) - 1) <<
						UART_DMA_CTRL_SIZE_POS);
		dev->rx_count = dev->rx_size;
	} else {
		dev->dmarxdescriptor[idx + 0] |=
			((dev->rx_size - dev->rx_count) <=
			 2 * 8 * 1024 ? UART_DMA_RX_CTRL : UART_DMA_RX_INT_CTRL);
		dev->rx_count += 8 * 1024;
	}
	dev->dmarxdescriptor[idx + 2] =
		(uint32_t) dev->rx_data + (dev->rx_count - 1);
}

static void create_dma_descriptor_tx(uart_info_pt dev, uint32_t burst,
				     uint8_t idx)
{
	dev->dmatxdescriptor[idx + 0] = (burst << UART_DMA_CTRL_XFER_POS);
	if ((dev->tx_size - dev->tx_count) <= 8 * 1024) {
		dev->dmatxdescriptor[idx + 0] |=
			UART_DMA_TX_END_CTRL | (((dev->tx_size - dev->tx_count) - 1) <<
						UART_DMA_CTRL_SIZE_POS);
		dev->tx_count = dev->tx_size;
	} else {
		dev->dmatxdescriptor[idx + 0] |=
			((dev->tx_size - dev->tx_count) <=
			 2 * 8 * 1024 ? UART_DMA_TX_CTRL : UART_DMA_TX_INT_CTRL);
		dev->tx_count += 8 * 1024;
	}
	dev->dmatxdescriptor[idx + 1] =
		(uint32_t) dev->tx_data + (dev->tx_count - 1);
}
#endif

#endif
