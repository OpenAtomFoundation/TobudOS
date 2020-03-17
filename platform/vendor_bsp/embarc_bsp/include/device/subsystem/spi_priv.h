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

#ifndef H_SPI_PRIV
#define H_SPI_PRIV

#include "device/ip_hal/ioctl.h"

/*  EIA SPI device registers */
#define CTRL0           (0x00)
#define CTRL1           (0x01)  // master only
#define SPIEN           (0x02)
#define SER             (0x04)  // master only
#define BAUDR           (0x05)  // master only
#define TXFTLR          (0x06)
#define RXFTLR          (0x07)
#define TXFLR           (0x08)
#define RXFLR           (0x09)
#define SR              (0x0a)
#define IMR             (0x0b)
#define ISR             (0x0c)
#define RISR            (0x0d)
#define TXOICR          (0x0e)
#define RXOICR          (0x0f)
#define RXUICR          (0x10)
#define ICR             (0x12)
#define CLK_ENA         (0x16)
#define DR              (0x18)
#define RX_SAMPLE_DLY   (0x3c)  // master only

/* Interrupt Register Fields */
#define R_IDLE                  (0x1 << 6)
#define R_TX_UNDER              (0x1 << 5)  // slave only
#define R_RX_FULL               (0x1 << 4)
#define R_RX_OVER               (0x1 << 3)
#define R_RX_UNDER              (0x1 << 2)
#define R_TX_OVER               (0x1 << 1)
#define R_TX_EMPTY              (0x1 << 0)

/* Other macros. */
#define DMA_NONE    (0xff)

typedef _Interrupt void (*SPI_ISR) ();

/* Private data structure maintained by the driver. */
typedef struct spi_info_struct {
	uint32_t reg_base;      /* base address of device register set */
	uint8_t instID;
	/* TX & RX Buffer and lengths */
	uint32_t rx_size;
	uint32_t *p_rxsize;
	uint32_t rx_count;
	void *rx_buff;
	uint32_t tx_size;
	uint32_t tx_count;
	void *tx_buff;
	uint8_t handling_rx, handling_tx;
	uint16_t log_xfer_size;
	uint16_t max_xfer_size;
	uint16_t fifo_depth;
	uint16_t dmarxchanid, dmatxchanid;
#ifdef __Xdmac
	_Uncached uint32_t *dmarxdescriptor, *dmatxdescriptor;  /* 2 descriptors each: 2nd one for future use */
	uint16_t dmarx_last16, dmatx_last16;
	uint32_t dmarx_last32, dmatx_last32;
#endif
	/* Callbacks */
	IO_CB_FUNC tx_cb;
	IO_CB_FUNC rx_cb;
	IO_CB_FUNC err_cb;
	/* Interrupt numbers and handlers */
	uint8_t rx_vector;      /* ISR vectors */
	uint8_t tx_vector;
	uint8_t err_vector;
	uint8_t idle_vector;
	SPI_ISR rx_isr;     /* SPI device ISRs */
	SPI_ISR tx_isr;
	SPI_ISR err_isr;
	SPI_ISR idle_isr;
} spi_info_t, *spi_info_pt;

#endif  /* H_SPI_PRIV */
