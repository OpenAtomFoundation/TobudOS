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

#ifndef H_DW_I2S_HAL_CFG
#define H_DW_I2S_HAL_CFG

/** Top-level parameters */

/** APB data bus width, 8, 16 or 32 bits */
#ifndef APB_DATA_BUS_WIDTH
#define APB_DATA_BUS_WIDTH          32
#endif

/** Receiver block enabled, false (0) or ture (1) */
#ifndef I2S_RECEIVER_BLOCK
#define I2S_RECEIVER_BLOCK          1
#endif

/** Number of receive channels, 1, 2, 3 or 4 channels */
#ifndef I2S_RX_CHANNELS
#define I2S_RX_CHANNELS             1
#endif

/** Transmitter block enabled, false (0) or ture (1) */
#ifndef I2S_TRANSMITTER_BLOCK
#define I2S_TRANSMITTER_BLOCK       1
#endif

/** Number of transmit channels, 1, 2, 3 or 4 channels */
#ifndef I2S_TX_CHANNELS
#define I2S_TX_CHANNELS             1
#endif

/** Set an I2S master, false (0) or ture (1) */
#ifndef I2S_MODE_EN
#define I2S_MODE_EN                 1
#endif

/** FIFO depth for RX and TX channels, 2, 4, 8 and 16 words */
#ifndef I2S_FIFO_DEPTH_GLOBAL
#define I2S_FIFO_DEPTH_GLOBAL       16
#endif

/** Word select length, 0 - 16 sclk cycles, 1 - 24 sclk cycles, 2 - 32 sclk cycles */
#ifndef I2S_WS_LENGTH
#define I2S_WS_LENGTH               32
#endif

/** Serial clock gating, 0 - no gating, 1 - 12 clock cycles, 2 - 16 clock cycles,
 *			 3 - 20 clock cycles, 4 - 24 clock cycles
 */
#ifndef I2S_SCLK_GATE
#define I2S_SCLK_GATE               4
#endif

/** Multiple interrupt output ports present, false (0) or ture (1) */
#ifndef I2S_INTERRUPT_SIGNALS
#define I2S_INTERRUPT_SIGNALS       0
#endif

/** Polarity of interrupt signals is acitve high, false (0) or ture (1) */
#ifndef I2S_INTR_POL
#define I2S_INTR_POL                1
#endif

/** Clock domain crossing synchronization depth
 * 1 - two-stage synchronization; first stage negative edge, second stage positive edge
 * 2 - two-stage synchronization; both stages positive edge
 * 3 - three-stage synchronization; all stage positive edge
 */
#ifndef I2S_SYNC_DEPTH
#define I2S_SYNC_DEPTH              2
#endif

/** Receiver channel parameters */

/** Receiver block DMA enabled, false (0) or ture (1) */
#ifndef I2S_RX_DMA
#define I2S_RX_DMA                  0
#endif

/** Max audio resolution - receive channel x, 12, 16, 20, 24 or 32 bits
 * only support 0 in this version
 */
#ifndef I2S_RX_WORDSIZE_0
#define I2S_RX_WORDSIZE_0           16
#endif

/** FIFO depth - receive channel x, 2, 4, 8 and 16 words
 * only support 0 in this version
 */
#ifndef I2S_RX_FIFO_0
#define I2S_RX_FIFO_0               I2S_FIFO_DEPTH_GLOBAL
#endif

/** RX FIFO data available trigger, 0 to I2S_RX_FIFO_x - 1; maximum = 15
 * only support 0 in this version
 */
#ifndef I2S_RX_FIFO_THRE_0
#define I2S_RX_FIFO_THRE_0          7
#endif

/** Transmitter channel parameters */

/** Receiver block DMA enabled, false (0) or ture (1) */
#ifndef I2S_TX_DMA
#define I2S_TX_DMA                  0
#endif

/** Max audio resolution - transmit channel x, 12, 16, 20, 24 or 32 bits
 * only support 0 in this version
 */
#ifndef I2S_TX_WORDSIZE_0
#define I2S_TX_WORDSIZE_0           16
#endif

/** FIFO depth - transmit channel x, 2, 4, 8 and 16 words
 * only support 0 in this version
 */
#ifndef I2S_TX_FIFO_0
#define I2S_TX_FIFO_0               I2S_FIFO_DEPTH_GLOBAL
#endif

/** TX FIFO data available trigger, 0 to I2S_TX_FIFO_x - 1; maximum = 15
 * only support 0 in this version
 */
#ifndef I2S_TX_FIFO_THRE_0
#define I2S_TX_FIFO_THRE_0          7
#endif

#endif /* H_DW_I2S_HAL_CFG */
