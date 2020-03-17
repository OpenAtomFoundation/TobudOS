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

#ifndef H_DW_I2S
#define H_DW_I2S

#include "device/ip_hal/dev_i2s.h"
#include "stdio.h"
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"

/**
 * If this header file is included, will indicate that this designware I2S device is used.
 *
 * sclk - serial clock
 * ws - word select
 * sd - serial data
 * Transmitter - device that places data on the sd line and is clocked by sclk and ws
 * Receiver - device that receives data from the sd line and is clocked by sclk and ws
 * Master - when configured as a master, DW_apb_i2s initializes the ws signal and supplies the
 *		clock gating and clock enabling signals
 * Slave - when configured as a slave, DW_apb_i2s responds to externally generated sclk and
 *		ws signals
 */
#define DEVICE_USE_DESIGNWARE_I2S

/**
 * @defgroup	DEVICE_DW_I2S_INFO	DesignWare I2S Related Information
 * @ingroup	DEVICE_DW_I2S
 * @brief	Types and macro definitions for designware i2s
 * @{
 */
#define DW_I2S_MASTER_SUPPORTED     (0x1)       /*!< Support Designware I2S Master Mode */
#define DW_I2S_SLAVE_SUPPORTED      (0x2)       /*!< Support Designware I2S Slave Mode */

#define DW_I2S_CHANNEL0_SUPPORTED   (0x1)       /*!< Support Designware I2S TX/RX Channel 0 */
#define DW_I2S_CHANNEL1_SUPPORTED   (0x2)       /*!< Support Designware I2S TX/RX Channel 1 */
#define DW_I2S_CHANNEL2_SUPPORTED   (0x4)       /*!< Support Designware I2S TX/RX Channel 2 */
#define DW_I2S_CHANNEL3_SUPPORTED   (0x8)       /*!< Support Designware I2S TX/RX Channel 3 */

#define DW_I2S_DATA_LENGTH_16       (1)         /*!< Set the length of data 16 bits else 32 bits>*/

/*!< I2S TX/RX channel */
typedef enum {
	DW_I2S_CHANNEL0 = 0,    /*!< I2S TX/RX channel 0  */
	DW_I2S_CHANNEL1 = 1,    /*!< I2S TX/RX channel 1 */
	DW_I2S_CHANNEL2 = 2,    /*!< I2S TX/RX channel 2 */
	DW_I2S_CHANNEL3 = 3     /*!< I2S TX/RX channel 3 */
} DW_I2S_CHANNEL;

/*!< I2S word select line (ws_out) stays in the left or right sample mode */
typedef enum {
	DW_I2S_WSS_16_CLK       = 0,    /*!< 16 clock cycles */
	DW_I2S_WSS_24_CLK       = 1,    /*!< 24 clock cycles */
	DW_I2S_WSS_32_CLK       = 2     /*!< 32 clock cycles */
} DW_I2S_WSS;

/*!< I2S gating of sclk */
typedef enum {
	DW_I2S_SCLKG_NO         = 0,    /*!< No clock gating */
	DW_I2S_SCLKG_12_CLK     = 1,    /*!< Gate after 12 clock cycles */
	DW_I2S_SCLKG_16_CLK     = 2,    /*!< Gate after 16 clock cycles */
	DW_I2S_SCLKG_20_CLK     = 3,    /*!< Gate after 20 clock cycles */
	DW_I2S_SCLKG_24_CLK     = 4     /*!< Gate after 24 clock cycles */
} DW_I2S_SCLKG;

#define DW_I2S_GINT_DISABLED    (0)             /*!< DesignWare interrupt disabled for control I2S irq/fiq */
#define DW_I2S_GINT_ENABLE      (1 << 0)        /*!< DesignWare interrupt enabled for control I2S irq/fiq */
#define DW_I2S_TXINT_ENABLE     (1 << 1)        /*!< DesignWare interrupt enabled for control transmit process */
#define DW_I2S_RXINT_ENABLE     (1 << 2)        /*!< DesignWare interrupt enabled for control transmit process */

/** @} */

/**
 * @defgroup	DEVICE_DW_I2S_REGSTRUCT		DesignWare I2S Register Structure
 * @ingroup	DEVICE_DW_I2S
 * @brief	Structure definitions for register
 * @details	Description of DesignWare I2S register information
 * @{
 */
/**
 * @brief	DesignWare I2S receiver register structure
 * @details	Detailed struct description of DesignWare I2S receiver
 */
typedef volatile struct {
	uint32_t IER;                   /*!< (0x000) : Enable Register */
	uint32_t IRER;                  /*!< (0x004) : I2S Receiver Block Enable Register */
	uint32_t ITER;                  /*!< (0X008) : I2S Transmitter Block Enable Register*/
	uint32_t CER;                   /*!< (0x00C) : Clock Enable Register */
	uint32_t CCR;                   /*!< (0x010) : Clock Configuration Register */
	uint32_t RXFFR;                 /*!< (0x014) : Receiver Block FIFO Register */
	uint32_t TXFFR;                 /*!< (0x018) : Transmitter Block FIFO Register */
	uint32_t RESERVED0;             /*!< (0x01C) : Reserved */
	uint32_t LRBR0;                 /*!< (0x020) : Left Receive Buffer 0 */
	uint32_t RRBR0;                 /*!< (0x024) : Right Receive Buffer 0 */
	uint32_t RER0;                  /*!< (0x028) : Receive Enable Register 0 */
	uint32_t TER0;                  /*!< (0x02C) : Transmit Enable Register 0 */
	uint32_t RCR0;                  /*!< (0x030) : Receive Configuration Register 0 */
	uint32_t TCR0;                  /*!< (0x034) : Transmit Configuration Register 0 */
	uint32_t ISR0;                  /*!< (0x038) : Interrupt Status Register 0 */
	uint32_t IMR0;                  /*!< (0x03C) : Interrupt Mask Register 0 */
	uint32_t ROR0;                  /*!< (0x040) : Receive Overrun Register 0 */
	uint32_t TOR0;                  /*!< (0x044) : Transmit Overrun Register 0 */
	uint32_t RFCR0;                 /*!< (0x048) : Receive FIFO Configuration Register 0 */
	uint32_t TFCR0;                 /*!< (0x04C) : Transmit FIFO Configuration Register 0 */
	uint32_t RFF0;                  /*!< (0x050) : Receive FIFO Flush 0 */
	uint32_t TFF0;                  /*!< (0x054) : Transmit FIFO Flush 0 */
	uint32_t RESERVED1[2];          /*!< (0x058) : Reserved */
	uint32_t LRBR1;                 /*!< (0x060) : Left Receive Buffer 1 */
	uint32_t RRBR1;                 /*!< (0x064) : Right Receive Buffer 1 */
	uint32_t RER1;                  /*!< (0x068) : Receive Enable Register 1 */
	uint32_t TER1;                  /*!< (0x06C) : Transmit Enable Register 1 */
	uint32_t RCR1;                  /*!< (0x070) : Receive Configuration Register 1 */
	uint32_t TCR1;                  /*!< (0x074) : Transmit Configuration Register 1 */
	uint32_t ISR1;                  /*!< (0x078) : Interrupt Status Register 1 */
	uint32_t IMR1;                  /*!< (0x07C) : Interrupt Mask Register 1 */
	uint32_t ROR1;                  /*!< (0x080) : Receive Overrun Register 1 */
	uint32_t TOR1;                  /*!< (0x084) : Transmit Overrun Register 1 */
	uint32_t RFCR1;                 /*!< (0x088) : Receive FIFO Configuration Register 1 */
	uint32_t TFCR1;                 /*!< (0x08C) : Transmit FIFO Configuration Register 1 */
	uint32_t RFF1;                  /*!< (0x090) : Receive FIFO Flush 1 */
	uint32_t TFF1;                  /*!< (0x094) : Transmit FIFO Flush 1 */
	uint32_t RESERVED2[2];          /*!< (0x098) : Reserved */
	uint32_t LRBR2;                 /*!< (0x0A0) : Left Receive Buffer 2 */
	uint32_t RRBR2;                 /*!< (0x0A4) : Right Receive Buffer 2 */
	uint32_t RER2;                  /*!< (0x0A8) : Receive Enable Register 2 */
	uint32_t TER2;                  /*!< (0x0AC) : Transmit Enable Register 2 */
	uint32_t RCR2;                  /*!< (0x0B0) : Receive Configuration Register 2 */
	uint32_t TCR2;                  /*!< (0x0B4) : Transmit Configuration Register 2 */
	uint32_t ISR2;                  /*!< (0x0B8) : Interrupt Status Register 2 */
	uint32_t IMR2;                  /*!< (0x0BC) : Interrupt Mask Register 2 */
	uint32_t ROR2;                  /*!< (0x0C0) : Receive Overrun Register 2 */
	uint32_t TOR2;                  /*!< (0x0C4) : Transmit Overrun Register 2 */
	uint32_t RFCR2;                 /*!< (0x0C8) : Receive FIFO Configuration Register 2 */
	uint32_t TFCR2;                 /*!< (0x0CC) : Transmit FIFO Configuration Register 2 */
	uint32_t RFF2;                  /*!< (0x0D0) : Receive FIFO Flush 2 */
	uint32_t TFF2;                  /*!< (0x0D4) : Transmit FIFO Flush 2 */
	uint32_t RESERVED3[2];          /*!< (0x0D8) : Reserved */
	uint32_t LRBR3;                 /*!< (0x0E0) : Left Receive Buffer 3 */
	uint32_t RRBR3;                 /*!< (0x0E4) : Right Receive Buffer 3 */
	uint32_t RER3;                  /*!< (0x0E8) : Receive Enable Register 3 */
	uint32_t TER3;                  /*!< (0x0EC) : Transmit Enable Register 3 */
	uint32_t RCR3;                  /*!< (0x0F0) : Receive Configuration Register 3 */
	uint32_t TCR3;                  /*!< (0x0F4) : Transmit Configuration Register 3 */
	uint32_t ISR3;                  /*!< (0x0F8) : Interrupt Status Register 3 */
	uint32_t IMR3;                  /*!< (0x0FC) : Interrupt Mask Register 3 */
	uint32_t ROR3;                  /*!< (0x100) : Receive Overrun Register 3 */
	uint32_t TOR3;                  /*!< (0x104) : Transmit Overrun Register 3 */
	uint32_t RFCR3;                 /*!< (0x108) : Receive FIFO Configuration Register 3 */
	uint32_t TFCR3;                 /*!< (0x10C) : Transmit FIFO Configuration Register 3 */
	uint32_t RFF3;                  /*!< (0x110) : Receive FIFO Flush 3 */
	uint32_t TFF3;                  /*!< (0x114) : Transmit FIFO Flush 3 */
	uint32_t RESERVED4[42];         /*!< (0x118) : Reserved */
	uint32_t RXDMA;                 /*!< (0x1C0) : Receiver Block DMA Register */
	uint32_t RRXDMA;                /*!< (0x1C4) : Reset Receiver Block DMA Register */
	uint32_t TXDMA;                 /*!< (0x1C8) : Transmitter Block DMA Register */
	uint32_t RTXDMA;                /*!< (0x1CC) : Reset Transmitter Block DMA Register */
	uint32_t RESERVED5[8];          /*!< (0x1D0) : Reserved */
	uint32_t I2S_COMP_PARAM_2;      /*!< (0x1F0) : Component Parameter 2 Register */
	uint32_t I2S_COMP_PARAM_1;      /*!< (0x1F4) : Component Parameter 1 Register */
	uint32_t I2S_COMP_VERSION;      /*!< (0x1F8) : Component Version ID */
	uint32_t I2S_COMP_TYPE;         /*!< (0x1FC) : DesignWare Component Type */
} DW_I2S_RX_REG, *DW_I2S_RX_REG_PTR;

/**
 * @brief	Designware I2S transmitter register structure
 * @details	Detailed struct description of DesignWare I2S transmitter
 */
typedef volatile struct {
	uint32_t IER;                   /*!< (0x000) : Enable Register */
	uint32_t IRER;                  /*!< (0x004) : I2S Receiver Block Enable Register */
	uint32_t ITER;                  /*!< (0X008) : I2S Transmitter Block Enable Register*/
	uint32_t CER;                   /*!< (0x00C) : Clock Enable Register */
	uint32_t CCR;                   /*!< (0x010) : Clock Configuration Register */
	uint32_t RXFFR;                 /*!< (0x014) : Receiver Block FIFO Register */
	uint32_t TXFFR;                 /*!< (0x018) : Transmitter Block FIFO Register */
	uint32_t RESERVED0;             /*!< (0x01C) : Reserved */
	uint32_t LTHR0;                 /*!< (0x020) : Left Transmit Holding Register 0 */
	uint32_t RTHR0;                 /*!< (0x024) : Right Transmit Holding Register 0 */
	uint32_t RER0;                  /*!< (0x028) : Receive Enable Register 0 */
	uint32_t TER0;                  /*!< (0x02C) : Transmit Enable Register 0 */
	uint32_t RCR0;                  /*!< (0x030) : Receive Configuration Register 0 */
	uint32_t TCR0;                  /*!< (0x034) : Transmit Configuration Register 0 */
	uint32_t ISR0;                  /*!< (0x038) : Interrupt Status Register 0 */
	uint32_t IMR0;                  /*!< (0x03C) : Interrupt Mask Register 0 */
	uint32_t ROR0;                  /*!< (0x040) : Receive Overrun Register 0 */
	uint32_t TOR0;                  /*!< (0x044) : Transmit Overrun Register 0 */
	uint32_t RFCR0;                 /*!< (0x048) : Receive FIFO Configuration Register 0 */
	uint32_t TFCR0;                 /*!< (0x04C) : Transmit FIFO Configuration Register 0 */
	uint32_t RFF0;                  /*!< (0x050) : Receive FIFO Flush 0 */
	uint32_t TFF0;                  /*!< (0x054) : Transmit FIFO Flush 0 */
	uint32_t RESERVED1[2];          /*!< (0x058) : Reserved */
	uint32_t LTHR1;                 /*!< (0x060) : Left Transmit Holding Register 1 */
	uint32_t RTHR1;                 /*!< (0x064) : Right Transmit Holding Register 1 */
	uint32_t RER1;                  /*!< (0x068) : Receive Enable Register 1 */
	uint32_t TER1;                  /*!< (0x06C) : Transmit Enable Register 1 */
	uint32_t RCR1;                  /*!< (0x070) : Receive Configuration Register 1 */
	uint32_t TCR1;                  /*!< (0x074) : Transmit Configuration Register 1 */
	uint32_t ISR1;                  /*!< (0x078) : Interrupt Status Register 1 */
	uint32_t IMR1;                  /*!< (0x07C) : Interrupt Mask Register 1 */
	uint32_t ROR1;                  /*!< (0x080) : Receive Overrun Register 1 */
	uint32_t TOR1;                  /*!< (0x084) : Transmit Overrun Register 1 */
	uint32_t RFCR1;                 /*!< (0x088) : Receive FIFO Configuration Register 1 */
	uint32_t TFCR1;                 /*!< (0x08C) : Transmit FIFO Configuration Register 1 */
	uint32_t RFF1;                  /*!< (0x090) : Receive FIFO Flush 1 */
	uint32_t TFF1;                  /*!< (0x094) : Transmit FIFO Flush 1 */
	uint32_t RESERVED2[2];          /*!< (0x098) : Reserved */
	uint32_t LTHR2;                 /*!< (0x0A0) : Left Transmit Holding Register 2 */
	uint32_t RTHR2;                 /*!< (0x0A4) : Right Transmit Holding Register 2 */
	uint32_t RER2;                  /*!< (0x0A8) : Receive Enable Register 2 */
	uint32_t TER2;                  /*!< (0x0AC) : Transmit Enable Register 2 */
	uint32_t RCR2;                  /*!< (0x0B0) : Receive Configuration Register 2 */
	uint32_t TCR2;                  /*!< (0x0B4) : Transmit Configuration Register 2 */
	uint32_t ISR2;                  /*!< (0x0B8) : Interrupt Status Register 2 */
	uint32_t IMR2;                  /*!< (0x0BC) : Interrupt Mask Register 2 */
	uint32_t ROR2;                  /*!< (0x0C0) : Receive Overrun Register 2 */
	uint32_t TOR2;                  /*!< (0x0C4) : Transmit Overrun Register 2 */
	uint32_t RFCR2;                 /*!< (0x0C8) : Receive FIFO Configuration Register 2 */
	uint32_t TFCR2;                 /*!< (0x0CC) : Transmit FIFO Configuration Register 2 */
	uint32_t RFF2;                  /*!< (0x0D0) : Receive FIFO Flush 2 */
	uint32_t TFF2;                  /*!< (0x0D4) : Transmit FIFO Flush 2 */
	uint32_t RESERVED3[2];          /*!< (0x0D8) : Reserved */
	uint32_t LTHR3;                 /*!< (0x0E0) : Left Transmit Holding Register 3 */
	uint32_t RTHR3;                 /*!< (0x0E4) : Right Transmit Holding Register 3 */
	uint32_t RER3;                  /*!< (0x0E8) : Receive Enable Register 3 */
	uint32_t TER3;                  /*!< (0x0EC) : Transmit Enable Register 3 */
	uint32_t RCR3;                  /*!< (0x0F0) : Receive Configuration Register 3 */
	uint32_t TCR3;                  /*!< (0x0F4) : Transmit Configuration Register 3 */
	uint32_t ISR3;                  /*!< (0x0F8) : Interrupt Status Register 3 */
	uint32_t IMR3;                  /*!< (0x0FC) : Interrupt Mask Register 3 */
	uint32_t ROR3;                  /*!< (0x100) : Receive Overrun Register 3 */
	uint32_t TOR3;                  /*!< (0x104) : Transmit Overrun Register 3 */
	uint32_t RFCR3;                 /*!< (0x108) : Receive FIFO Configuration Register 3 */
	uint32_t TFCR3;                 /*!< (0x10C) : Transmit FIFO Configuration Register 3 */
	uint32_t RFF3;                  /*!< (0x110) : Receive FIFO Flush 3 */
	uint32_t TFF3;                  /*!< (0x114) : Transmit FIFO Flush 3 */
	uint32_t RESERVED4[42];         /*!< (0x118) : Reserved */
	uint32_t RXDMA;                 /*!< (0x1C0) : Receiver Block DMA Register */
	uint32_t RRXDMA;                /*!< (0x1C4) : Reset Receiver Block DMA Register */
	uint32_t TXDMA;                 /*!< (0x1C8) : Transmitter Block DMA Register */
	uint32_t RTXDMA;                /*!< (0x1CC) : Reset Transmitter Block DMA Register */
	uint32_t RESERVED5[8];          /*!< (0x1D0) : Reserved */
	uint32_t I2S_COMP_PARAM_2;      /*!< (0x1F0) : Component Parameter 2 Register */
	uint32_t I2S_COMP_PARAM_1;      /*!< (0x1F4) : Component Parameter 1 Register */
	uint32_t I2S_COMP_VERSION;      /*!< (0x1F8) : Component Version ID */
	uint32_t I2S_COMP_TYPE;         /*!< (0x1FC) : DesignWare Component Type */
} DW_I2S_TX_REG, *DW_I2S_TX_REG_PTR;
/** @} */

typedef struct {
#ifdef DW_I2S_DATA_LENGTH_16
	uint16_t *buf;
#else
	uint32_t *buf;
#endif
	uint32_t ofs;
	uint32_t len;
} DW_I2S_BUFFER, *DW_I2S_BUFFER_PTR;

typedef struct {
	uint32_t support_modes;                 /*!< Supported I2S modes, [DW_I2S_MASTER_SUPPORTED, DW_I2S_SLAVE_SUPPORTED] */
	uint32_t fifo_len;                      /*!< FIFO length (uint: word) */
	uint32_t channels;                      /*!< Suppported channels, [DW_I2S_CHANNEL0_SUPPORTED, DW_I2S_CHANNEL3_SUPPORTED] */
	/* Master mode only, clock generation, not available in slave mode */
	uint32_t ws_length;                     /*!< Number of sclk cycles for which ws_out stays, related to WSS */
	uint32_t sclk_gate;                     /*!< I2S gating of sclk */
	/* Channel setting */
	uint32_t data_res[4];                   /*!< Desired audio data resolution, which can be @ref I2S_AUD_DATA_RES, related WLEN */
	uint32_t sample_rate[4];                /*!< Audio sampling rate, which can be @ref I2S_AUD_SAMPLE_RATE, related to audio reference clk */
	uint32_t intno[4];                      /*!< I2S interrupt vector number, bit0-15 tx_emp_intr/rx_da_intr, bit16-31 tx_or_intr/rx_or_intr */
	INT_HANDLER_T dw_i2s_int_handler;       /*!< I2S interrupt handler */
} DW_I2S_CONFIG, *DW_I2S_CONFIG_PTR;

/**
 * @brief	Structure definitions for DesignWare I2S control
 * @details	Structure type for DesignWare I2S implementation
 */
typedef struct {
	DW_I2S_RX_REG *dw_i2s_regs;     /*!< I2S device registers */
	/* Variables which always change during I2S operation */
	uint32_t int_status[4];         /*!< I2S interrupt status */
	DW_I2S_BUFFER dw_i2s_buf;       /*!< I2S read buffer for receive data, left+right data */
} DW_I2S_RX_CTRL, *DW_I2S_RX_CTRL_PTR;

typedef struct {
	DW_I2S_TX_REG *dw_i2s_regs;     /*!< I2S device registers */
	/* Variables which always change during I2S operation */
	uint32_t int_status[4];         /*!< I2S interrupt status */
	DW_I2S_BUFFER dw_i2s_buf;       /*!< I2S write buffer for transmit data, left+right data */
} DW_I2S_TX_CTRL, *DW_I2S_TX_CTRL_PTR;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup	DEVICE_DW_I2S_FUNCDLR	DesignWare I2S Function Declaration
 * @ingroup	DEVICE_DW_I2S
 * @brief	Contains declarations of DesignWare I2S functions
 * @details	These are only used in I2S object implementation source file
 * @{
 */
extern int32_t dw_i2s_open(DEV_I2S *i2s_obj, uint32_t mode, uint32_t param);
extern int32_t dw_i2s_close(DEV_I2S *i2s_obj);
extern int32_t dw_i2s_control(DEV_I2S *i2s_obj, uint32_t ctrl_cmd, void *param);
extern int32_t dw_i2s_write(DEV_I2S *i2s_obj, const void *data, uint32_t len, uint32_t channel);
extern int32_t dw_i2s_read(DEV_I2S *i2s_obj, void *data, uint32_t len, uint32_t channel);
extern void dw_i2s_isr_tx(DEV_I2S *i2s_obj, void *ptr);
extern void dw_i2s_isr_rx(DEV_I2S *i2s_obj, void *ptr);
extern uint32_t dw_i2s_write2(DEV_I2S *i2s_obj, const void *data, uint32_t len, uint32_t channel);
#ifdef __cplusplus
}
#endif

/** @} */

#endif /* H_DW_I2S */
