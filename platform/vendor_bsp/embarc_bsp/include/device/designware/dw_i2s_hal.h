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

#ifndef H_DW_I2S_HAL
#define H_DW_I2S_HAL

#include "device/designware/dw_i2s_hal_cfg.h"

/** IER (I2S Enable Register) */
#define DW_I2S_IER_IEN              1

/** IRER (I2S Receiver Block Enable Register) */
#define DW_I2S_IRER_RXEN            1

/** ITER (I2S Transmitter Block Enable Register) */
#define DW_I2S_ITER_TXEN            1

/** CER (I2S Clock Enable Register) */
#define DW_I2S_CER_CLKEN            1

/** CCR (Clock Configuration Register) */
#define DW_I2S_CCR_WSS(x)           ((x & 0x03) << 3)
#define DW_I2S_CCR_SCLKG(x)         (x & 0x07)

/** RXFFR (Receiver Block FIFO Reset Register) */
#define DW_I2S_RXFFR_RXFFR          1

/** TXFFR (Transmitter Block FIFO Reset Register) */
#define DW_I2S_TXFFR_TXFFR          1

/** LRBRx (Left Receive Buffer Register) */
#define DW_I2S_LRBR0_LRBR0(x)       (x & ((1 << I2S_RX_WORDSIZE_0) - 1))

/** LTHRx (Left Transmit Holding Register) */
#define DW_I2S_LTHR0_LTHR0(x)       (x & ((1 << I2S_TX_WORDSIZE_0) - 1))

/** RRBRx (Right Receive Buffer Register) */
#define DW_I2S_RRBR0_RRBR0(x)       (x & ((1 << I2S_RX_WORDSIZE_0) - 1))

/** RTHRx (Right Transmit Holding Register) */
#define DW_I2S_RTHR0_RTHR0(x)       (x & ((1 << I2S_TX_WORDSIZE_0) - 1))

/** RERx (Receive Enable Register) */
#define DW_I2S_RERX_RXCHENX         1

/** TERx (Transmit Enable Register) */
#define DW_I2S_TERX_TXCHENX         1

/** RCRx (Receive Configuration Register) */
#define DW_I2S_RCRX_WLEN(x)         (x & 0x07)

/** TCRx (Transmit Configuration Register) */
#define DW_I2S_TCRX_WLEN(x)         (x & 0x07)

/** ISRx (Interrupt Status Register) */
#define DW_I2S_ISRX_TXFO            (1 << 5)
#define DW_I2S_ISRX_TXFE            (1 << 4)
#define DW_I2S_ISRX_RXFO            (1 << 1)
#define DW_I2S_ISRX_RXDA            1

/** IMRx (Interrupt Mask Register) */
#define DW_I2S_IMRX_TXFOM           (1 << 5)
#define DW_I2S_IMRX_TXFEM           (1 << 4)
#define DW_I2S_IMRX_RXFOM           (1 << 1)
#define DW_I2S_IMRX_RXDAM           1

/** RORx (Receive Overrun Register) */
#define DW_I2S_RORX_RXCHO           1

/** TORx (Transmit Overrun Register) */
#define DW_I2S_TORX_TXCHO           1

/** RFCRx (Receive FIFO Configuration Register) */
#define DW_I2S_RFCRX_RXCHDT(x)      (x & 0x0f)

/** TFCRx (Transmit FIFO Configuration Register) */
#define DW_I2S_TFCRX_TXCHET(x)      (x & 0x0f)

/** RFFx (Receive FIFO Flush Register) */
#define DW_I2S_RFFX_RXCHFR          1

/** TFFx (Transmit FIFO Flush Register) */
#define DW_I2S_TFFX_TXCHFR          1

/** RXDMA (Receiver Block DMA Register) */
#define DW_I2S_RXDMA_RXDMA          (x & ((1 << APB_DATA_BUS_WIDTH) - 1))

/** RRXDMA (Reset Receiver Block DMA Register) */
#define DW_I2S_RRXDMA_RRXDMA        1

/** TXDMA (Transmitter Block DMA Register) */
#define DW_I2S_TXDMA_TXDMA          (x & ((1 << APB_DATA_BUS_WIDTH) - 1))

/** RTXDMA (Reset Transmitter Block DMA Register) */
#define DW_I2S_RTXDMA_RTXDMA        1

#endif /* H_DW_I2S_HAL */
