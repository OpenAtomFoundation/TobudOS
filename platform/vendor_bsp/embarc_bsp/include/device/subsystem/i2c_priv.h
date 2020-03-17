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

#ifndef H_I2C_PRIV
#define H_I2C_PRIV

#include "device/ip_hal/ioctl.h"

/*  EIA I2C device registers */
#define I2C_CON                 (0x00)
#define I2C_TAR                 (0x04)  // master only
#define I2C_SAR                 (0x08)  // slave only
#define I2C_DATA_CMD            (0x10)
#define I2C_SS_SCL_HCNT         (0x14)
#define I2C_SS_SCL_LCNT         (0x18)
#define I2C_FS_SCL_HCNT         (0x1c)
#define I2C_FS_SCL_LCNT         (0x20)
#define I2C_INTR_STAT           (0x2c)
#define I2C_INTR_MASK           (0x30)
#define I2C_RX_TL               (0x38)
#define I2C_TX_TL               (0x3c)
#define I2C_CLR_INTR            (0x40)
#define I2C_CLR_RX_OVER         (0x48)
#define I2C_CLR_TX_OVER         (0x4c)
#define I2C_CLR_RD_REQ          (0x50)
#define I2C_CLR_TX_ABRT         (0x54)
#define I2C_CLR_STOP_DET        (0x60)
#define I2C_ENABLE              (0x6c)
#define I2C_STATUS              (0x70)
#define I2C_TXFLR               (0x74)
#define I2C_RXFLR               (0x78)
#define I2C_SDA_HOLD            (0x7c)
#define I2C_TX_ABRT_SOURCE      (0x80)
#define I2C_SDA_SETUP           (0x94)  // slave only
#define I2C_ENABLE_STATUS       (0x9c)
#define I2C_FS_SPKLEN           (0xa0)
#define I2C_CLKEN               (0xc0)

/* Interrupt Register Fields */
#define R_START_DET             (0x1 << 10)
#define R_STOP_DET              (0x1 << 9)
#define R_ACTIVITY              (0x1 << 8)
#define R_RX_DONE               (0x1 << 7)
#define R_TX_ABRT               (0x1 << 6)
#define R_RD_REQ                (0x1 << 5)
#define R_TX_EMPTY              (0x1 << 4)
#define R_TX_OVER               (0x1 << 3)
#define R_RX_FULL               (0x1 << 2)
#define R_RX_OVER               (0x1 << 1)
#define R_RX_UNDER              (0x1 << 0)

/* Other macros. */
#define I2C_RESTART_CMD         (0x1 << 10)
#define I2C_STOP_CMD            (0x1 << 9)
#define I2C_READ_CMD            (0x1 << 8)

#define DMA_NONE                (0xff)

typedef _Interrupt void (*I2C_ISR) ();

/* Private data structure maintained by the driver */
typedef struct i2c_info {
	uint32_t reg_base;      // base address of device register set
	uint8_t instID;
	/* TX & RX Buffer and lengths */
	uint8_t *tx_data;
	uint32_t tx_size;
	uint32_t tx_count;
	uint32_t *p_txsize;
	uint8_t *rx_data;
	uint32_t rx_size;
	uint32_t rx_count;
	uint32_t *p_rxsize;
	uint32_t rx_req_count;
	uint8_t handling_tx;
	uint8_t handling_rx;
	uint8_t stop_detected;
	uint8_t rd_req_detected;
	uint16_t fifo_depth;
	uint16_t dmarxchanid, dmatxchanid;
	uint16_t next_cond;
#ifdef __Xdmac
	_Uncached uint32_t *dmarxdescriptor, *dmatxdescriptor;
	uint16_t dmatx_last;
#endif
	/* Callbacks */
	IO_CB_FUNC tx_cb;
	IO_CB_FUNC rx_cb;
	IO_CB_FUNC err_cb;
	/* Interrupt numbers and handlers */
	uint8_t vector_err;
	uint8_t vector_rx_avail;
	uint8_t vector_tx_req;
	uint8_t vector_stop_det;
	uint8_t vector_rd_req;          // slave only
	uint8_t vector_restart_det;     // slave only
	I2C_ISR isr_err;
	I2C_ISR isr_rx_avail;
	I2C_ISR isr_tx_req;
	I2C_ISR isr_stop_det;
	I2C_ISR isr_rd_req;
} i2c_info_t, *i2c_info_pt;

/* I2C IOCTLs default values        */
#define     I2C_SS_SCL_HIGH_COUNT   (0x0190)    // WHY? hw-default is 0x30
#define     I2C_SS_SCL_LOW_COUNT    (0x01d6)    // WHY? hw-default is 0x38
#define     I2C_FS_SCL_HIGH_COUNT   (0x003c)
#define     I2C_FS_SCL_LOW_COUNT    (0x0082)

#endif  /* H_I2C_PRIV */
