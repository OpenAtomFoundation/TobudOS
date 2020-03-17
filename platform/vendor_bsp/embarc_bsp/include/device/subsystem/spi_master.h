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

#ifndef H_SPI_MASTER
#define H_SPI_MASTER

#include "device/ip_hal/ioctl.h"
#include "device/ip_hal/dev_spi.h"

/* SPI control commands */
#define IO_SPI_MASTER_SET_SRL_MODE          (0x20)
#define IO_SPI_MASTER_SET_TRANSFER_MODE     (0x21)
#define IO_SPI_MASTER_SET_BAUD              (0x22)
#define IO_SPI_MASTER_SET_SCPH              (0x23)
#define IO_SPI_MASTER_SET_SCPL              (0x24)
#define IO_SPI_MASTER_SET_DFS               (0x25)
#define IO_SPI_MASTER_SET_SE                (0x26)
#define IO_SPI_MASTER_SET_RX_THRESHOLD      (0x28)
#define IO_SPI_MASTER_SET_TX_THRESHOLD      (0x29)
#define IO_SPI_MASTER_SET_RX_DELAY          (0x2a)
#define IO_SPI_MASTER_SET_PACKING           (0x2b)

#define IO_SPI_MASTER_GET_XFER_SIZE         (0x2c)  // undocumented: for verification purposes

/* SPI shift register modes */
#define SPI_NORMAL_SRL_MODE                 (0)
#define SPI_TEST_SRL_MODE                   (1)

/* SPI transfer modes       */
#define SPI_TRANSMIT_RECEIVE_MODE           (0)
#define SPI_TRANSMIT_ONLY_MODE              (1)
#define SPI_RECEIVE_ONLY_MODE               (2)
#define SPI_EEPROM_RDONLY_MODE              (3)
#define SPI_RECEIVE_AFTER_TRANSMIT_MODE     (SPI_EEPROM_RDONLY_MODE)

/* SPI clock phase          */
#define SPI_SCPH_LOW                        (0)
#define SPI_SCPH_HIGH                       (1)
/* SPI clock phase          */
#define SPI_SCPOL_LOW                       (0)
#define SPI_SCPOL_HIGH                      (1)

/* SPI Slave Select line codes */
#define SPI_SE_1        (0x01)
#define SPI_SE_2        (0x02)
#define SPI_SE_3        (0x04)
#define SPI_SE_4        (0x08)

extern uint32_t io_spi_master_open(uint32_t dev_id);
extern void io_spi_master_close(uint32_t dev_id);
extern void io_spi_master_read(uint32_t dev_id, uint8_t *data, uint32_t *size);
extern void io_spi_master_write(uint32_t dev_id, uint8_t *data, uint32_t *size);
/* I2C master valid IOCTLs
   cmd                              arg type        arg value
   IO_SET_CB_RX                     io_cb_t         Callback function invoked from ISR when read successfully finishes
   IO_SET_CB_TX                     io_cb_t         Callback function invoked from ISR when write successfully finishes
   IO_SET_CB_ERR                    io_cb_t         Callback function invoked from ISR when read/write fails
   IO_SPI_MASTER_SET_SRL_MODE       uint32_t        SPI shift register operation mode:
                                                    SPI_NORMAL_SRL_MODE - normal operation mode, Tx to Rx register loop disabled
                                                    SPI_TEST_SRL_MODE - test operation mode, Tx to Rx register loop enabled
   IO_SPI_MASTER_SET_TRANSFER_MODE  uint32_t        SPI transfer mode:
                                                    SPI_TRAMSMIT_RECEIVE_MODE - transmit and receive
                                                    SPI_TRANSMIT_ONLY_MODE - transmit only
                                                    SPI_RECEIVE_ONLY_MODE - receive only
                                                    SPI_EEPROM_RDONLY_MODE - EEPROM read
   IO_SPI_MASTER_SET_BAUD           uint32_t        SPI clock divider corresponding to desired bitrate, in the range from 2 to 65534
   IO_SPI_MASTER_SET_SCPH           uint32_t        SPI closk phase:
                                                    SPI_SCPH_LOW - inactive state of SPI clock is low
                                                    SPI_SCPH_HIGH - inactive state of SPI clock is high
   IO_SPI_MASTER_SET_SCPL           uint32_t        SPI clock polarity:
                                                    SPI_SCPOL_LOW - SPI clock toggles in middle of first data bit
                                                    SPI_SCPOL_HIGH - SPI clock toggles at start of first data bit
   IO_SPI_MASTER_SET_DFS            uint32_t        SPI frame size in bits
   IO_SPI_MASTER_SET_SE             uint32_t        SPI Slave Select line:
                                                    SPI_SE_1
                                                    SPI_SE_2
                                                    SPI_SE_3
                                                    SPI_SE_4
   IO_SPI_MASTER_SET_RX_THRESHOLD   uint32_t        threshold value for the Rx FIFO
   IO_SPI_MASTER_SET_TX_THRESHOLD   uint32_t        threshold value for the Tx FIFO
   IO_SPI_MASTER_SET_RX_DELAY       uint32_t        rx sample delay
   IO_SPI_MASTER_SET_PACKING        uint32_t        1|0 - enable|disable data packing in Rx/Tx data buffer.
 */

extern void io_spi_master_ioctl(uint32_t dev_id, uint32_t cmd, void *arg);

extern void io_spi_master_polling(uint32_t dev_id, DEV_SPI_TRANSFER *xfer);

#endif              /* H_SPI_MASTER */
