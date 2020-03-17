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
#ifndef H_DW_SDIO_HAL
#define H_DW_SDIO_HAL

/* register offset */
#define DWSDIO_REG_CTRL             (0x000)
#define DWSDIO_REG_PWREN            (0x004)
#define DWSDIO_REG_CLKDIV           (0x008)
#define DWSDIO_REG_CLKSRC           (0x00C)
#define DWSDIO_REG_CLKENA           (0x010)
#define DWSDIO_REG_TMOUT            (0x014)
#define DWSDIO_REG_CTYPE            (0x018)
#define DWSDIO_REG_BLKSIZ           (0x01C)
#define DWSDIO_REG_BYTCNT           (0x020)
#define DWSDIO_REG_INTMASK          (0x024)
#define DWSDIO_REG_CMDARG           (0x028)
#define DWSDIO_REG_CMD              (0x02C)
#define DWSDIO_REG_RESP0            (0x030)
#define DWSDIO_REG_RESP1            (0x034)
#define DWSDIO_REG_RESP2            (0x038)
#define DWSDIO_REG_RESP3            (0x03C)
#define DWSDIO_REG_MINTSTS          (0x040)
#define DWSDIO_REG_RINTSTS          (0x044)
#define DWSDIO_REG_STATUS           (0x048)
#define DWSDIO_REG_FIFOTH           (0x04C)
#define DWSDIO_REG_CDETECT          (0x050)
#define DWSDIO_REG_WRTPRT           (0x054)
#define DWSDIO_REG_GPIO             (0x058)
#define DWSDIO_REG_TCMCNT           (0x05C)
#define DWSDIO_REG_TBBCNT           (0x060)
#define DWSDIO_REG_DEBNCE           (0x064)
#define DWSDIO_REG_USRID            (0x068)
#define DWSDIO_REG_VERID            (0x06C)
#define DWSDIO_REG_HCON             (0x070)
#define DWSDIO_REG_UHS_REG          (0x074)
#define DWSDIO_REG_RST_N            (0x078)
#define DWSDIO_REG_BMOD             (0x080)
#define DWSDIO_REG_PLDMND           (0x084)
#define DWSDIO_REG_DBADDR           (0x088)
#define DWSDIO_REG_IDSTS            (0x08C)
#define DWSDIO_REG_IDINTEN          (0x090)
#define DWSDIO_REG_DSCADDR          (0x094)
#define DWSDIO_REG_BUFADDR          (0x098)
#define DWSDIO_REG_CARD_THR_CTRL    (0x100)
#define DWSDIO_REG_UHS_REG_EXT      (0x108)
#define DWSDIO_REG_DATA             (0x200)

/* DW AHB SDIO bit definitions */
#define DWSDIO_CTRL_BIT_RESET_CTRL  0
#define DWSDIO_CTRL_RESET_CTRL      (1 << DWSDIO_CTRL_BIT_RESET_CTRL)
#define DWSDIO_CTRL_BIT_RESET_FIFO  1
#define DWSDIO_CTRL_RESET_FIFO      (1 << DWSDIO_CTRL_BIT_RESET_FIFO)
#define DWSDIO_CTRL_BIT_RESET_DMA   2
#define DWSDIO_CTRL_RESET_DMA       (1 << DWSDIO_CTRL_BIT_RESET_DMA)

#define DWSDIO_CTRL_RESET_ALL       (DWSDIO_CTRL_RESET_CTRL | DWSDIO_CTRL_RESET_FIFO | DWSDIO_CTRL_RESET_DMA)
#define DWSDIO_CTRL_BIT_INT_ENABLE  4
#define DWSDIO_CTRL_INT_ENABLE      (1 << DWSDIO_CTRL_BIT_INT_ENABLE)

#define DWSDIO_CTRL_BIT_DMA_ENABLE  5
#define DWSDIO_CTRL_DMA_ENABLE      (1 << DWSDIO_CTRL_BIT_DMA_ENABLE)

#define DWSDIO_CTRL_BIT_ODPULLUP    24
#define DWSDIO_CTRL_ODPULLUP        (1 << DWSDIO_CTRL_BIT_ODPULLUP)

#define DWSDIO_CTRL_BIT_INTERNAL_DMA    25
#define DWSDIO_CTRL_INTERNAL_DMA        (1 << DWSDIO_CTRL_BIT_INTERNAL_DMA)

/* Register CMD field CMD_INDEX */
#define DWSDIO_CMD_BIT_CMD_INDEX    0
#define DWSDIO_CMD_MASK_CMD_INDEX   0x0000003FU

/* Register CMD field RESPONSE_EXPECT */
#define DWSDIO_CMD_BIT_RESP_EXP     6
#define DWSDIO_CMD_RESP_EXP         (1 << DWSDIO_CMD_BIT_RESP_EXP)

/* Register CMD field RESPONSE_LENGTH */
#define DWSDIO_CMD_BIT_RESP_LENGTH  7
#define DWSDIO_CMD_RESP_LENGTH      (1 << DWSDIO_CMD_BIT_RESP_LENGTH)

/* Register CMD field CHECK_RESPONSE_CRC */
#define DWSDIO_CMD_BIT_CHECK_CRC    8
#define DWSDIO_CMD_CHECK_CRC        (1 << DWSDIO_CMD_BIT_CHECK_CRC)

/* Register CMD field DATA_EXPECTED */
#define DWSDIO_CMD_BIT_DATA_EXP     9
#define DWSDIO_CMD_DATA_EXP         (1 << DWSDIO_CMD_BIT_DATA_EXP)

/* Register CMD field READ-WRITE */
#define DWSDIO_CMD_BIT_RW           10
#define DWSDIO_CMD_RW               (1 << DWSDIO_CMD_BIT_RW)

/* Register CMD field TRANSFER_MODE */
#define DWSDIO_CMD_BIT_XFER_MODE    11
#define DWSDIO_CMD_XFER_MODE        (1 << DWSDIO_CMD_BIT_XFER_MODE)

/* Register CMD field SEND_AUTO_STOP */
#define DWSDIO_CMD_BIT_AUTO_STOP    12
#define DWSDIO_CMD_AUTO_STOP        (1 << DWSDIO_CMD_BIT_AUTO_STOP)

/* Register CMD field WAIT_PRVDATA_COMPLETE */
#define DWSDIO_CMD_BIT_PRV_DAT_WAIT 13
#define DWSDIO_CMD_PRV_DAT_WAIT     (1 << DWSDIO_CMD_BIT_PRV_DAT_WAIT)

/* Register CMD field STOP_ABORT_CMD */
#define DWSDIO_CMD_BIT_ABORT_STOP   14
#define DWSDIO_CMD_ABORT_STOP       (1 << DWSDIO_CMD_BIT_ABORT_STOP)

/* Register CMD field SEND_INITIALIZATION */
#define DWSDIO_CMD_BIT_SEND_INIT    15
#define DWSDIO_CMD_SEND_INIT        (1 << DWSDIO_CMD_BIT_SEND_INIT)

/* Register CMD field CARD_NUMBER */
#define DWSDIO_CMD_BIT_CARD_NO      16
#define DWSDIO_CMD_MASK_CARD_NO     0x001F0000U

/* Register CMD field UPDATE_CLOCK_REGISTERS_ONLY */
#define DWSDIO_CMD_BIT_BIT_UPD_CLK  21
#define DWSDIO_CMD_UPD_CLK          (1 << DWSDIO_CMD_BIT_BIT_UPD_CLK)

/* Register CMD field READ_CEATA_DEVICE */
#define DWSDIO_CMD_BIT_READ_CEATA_DEVICE 22
#define DWSDIO_CMD_READ_CEATA_DEVICE    (1 << DWSDIO_CMD_BIT_READ_CEATA_DEVICE)

/* Register CMD field CCS_EXPECTED */
#define DWSDIO_CMD_BIT_CCS_EXP      23
#define DWSDIO_CMD_CCS_EXP          (1 << DWSDIO_CMD_BIT_CCS_EXP)

/* Register CMD field ENABLE_BOOT */
#define DWSDIO_CMD_BIT_ENABLE_BOOT  24
#define DWSDIO_CMD_ENABLE_BOOT      (1 << DWSDIO_CMD_BIT_ENABLE_BOOT)

/* Register CMD field EXPECT_BOOT_ACK */
#define DWSDIO_CMD_BIT_BOOT_ACK_EXP 25
#define DWSDIO_CMD_BOOT_ACK_EXP     (1 << DWSDIO_CMD_BIT_BOOT_ACK_EXP)

/* Register CMD field DISABLE_BOOT */
#define DWSDIO_CMD_BIT_DISABLE_BOOT 25
#define DWSDIO_CMD_DISBALE_BOOT     (1 << DWSDIO_CMD_BIT_DISABLE_BOOT)

/* Register CMD field BOOT_MODE */
#define DWSDIO_CMD_BIT_BOOT_MODE    27
#define DWSDIO_CMD_BOOT_MODE        (1 << DWSDIO_CMD_BIT_BOOT_MODE)

/* Register CMD field VOLT_SWITCH */
#define DWSDIO_CMD_BIT_VOLT_SWITCH  28
#define DWSDIO_CMD_VOLT_SWITCH      (1 << DWSDIO_CMD_BIT_VOLT_SWITCH)

/* Register CMD field USE_HOLD_REG */
#define DWSDIO_CMD_BIT_USE_HOLD_REG 29
#define DWSDIO_CMD_USE_HOLD_REG     (1 << DWSDIO_CMD_BIT_USE_HOLD_REG)

/* Register CMD field START_CMD */
#define DWSDIO_CMD_BIT_START        31
#define DWSDIO_CMD_START            (1 << DWSDIO_CMD_BIT_START)

/* Register UHS_REG_EXT */
#define DWSDIO_UHS_REG_EXT_MASK_REFCLK    (0xC0000000)
#define DWSDIO_UHS_REG_EXT_BIT_REFCLK     30

#define DWSDIO_UHS_REG_EXT_MASK_CLKDRIVE  (0x01800000)
#define DWSDIO_UHS_REG_EXT_BIT_CLKDRIVE   23

#define DWSDIO_UHX_REG_EXT_MASK_CLKSAMPLE (0x007f0000)
#define DWSDIO_UHX_REG_EXT_BIT_CLKSAMPLE  16

#define DWSDIO_STATUS_BIT_FIFO      (17)
#define DWSDIO_STATUS_FIFO_FULL     (0x8)
#define DWSDIO_STATUS_FIFO_EMPTY    (0x4)
#define DWSDIO_STATUS_MASK_FIFO     (0x3ffe0000)

/* status bit */
#define DWSDIO_STATUS_DATA_BUSY     (1 << 9)

#define DWSDIO_FIFOTH_BIT_RX_WMARK     16
#define DWSDIO_FIFOTH_MASK_RX_WMARK    (0xfff << DWSDIO_FIFOTH_BIT_RX_WMARK)
#define DWSDIO_FIFOTH_BIT_TX_WMARK     0
#define DWSDIO_FIFOTH_MASK_TX_WMARK    (0xfff)
#define DWSDIO_FIFOTH_BIT_DMA_M_SIZE   28
#define DWSDIO_FIFOTH_MASK_DMA_M_SIZE  (0x7 << DWSDIO_FIFOTH_BIT_DMA_M_SIZE)
#define DWSDIO_FIFOTH_M_SIZE(x)        ((x) << DWSDIO_FIFOTH_BIT_DMA_M_SIZE)
#define DWSDIO_FIFOTH_TX_WMASK(x)      ((x))
#define DWSDIO_FIFOTH_RX_WMASK(x)      ((x) << DWSDIO_FIFOTH_BIT_RX_WMARK)

/* interrupt status bit */
#define DWSDIO_INT_CAD      (0x1U)      // Card Detected
#define DWSDIO_INT_RE       (0x2U)      // Response error
#define DWSDIO_INT_CD       (0x4U)      // Command Done
#define DWSDIO_INT_DTO      (0x8U)      // Data Transfer Over
#define DWSDIO_INT_TXDR     (0x10U)     // Transmit FiFo Data Request
#define DWSDIO_INT_RXDR     (0x20U)     // Receive Fifo Data request
#define DWSDIO_INT_RCRC     (0x40U)     // Response CRC error
#define DWSDIO_INT_DCRC     (0x80U)     // Data CRC error
#define DWSDIO_INT_RTO      (0x100U)    // Response Timeout
#define DWSDIO_INT_DRTO     (0x200U)    // Data read Timeout
#define DWSDIO_INT_HTO      (0x400U)    // Data starvation by Host timeout
#define DWSDIO_INT_FRUN     (0x800U)    // Fifo underrun / overrun error
#define DWSDIO_INT_HLE      (0x1000U)   // Hardware Locked write error
#define DWSDIO_INT_SBE      (0x2000U)   // Start bit error
#define DWSDIO_INT_ACD      (0x4000U)   // Auto Command Done
#define DWSDIO_INT_EBE      (0x8000U)   // End bit error
#define DWSDIO_INT_ALL      (0xFFFF)
#define DWSDIO_INT_DATA_ERR (DWSDIO_INT_SBE | DWSDIO_INT_EBE | DWSDIO_INT_HLE |	\
			     DWSDIO_INT_FRUN | DWSDIO_INT_DCRC)
#define DWSDIO_INT_DATA_TMO (DWSDIO_INT_DRTO | DWSDIO_INT_HTO)

#define DWSDIO_ENUMERATION_FREQ (400000) /* 400 Khz for card enumeration */

#endif /* H_DW_SDIO_HAL */