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

#ifndef H_I2C_SLAVE
#define H_I2C_SLAVE

#include "device/ip_hal/ioctl.h"

/* I2C devices common control commands */
#define IO_I2C_SLAVE_SET_SETUP_TIME     (0x05)
#define IO_I2C_SLAVE_SET_HOLD_TIME_RX   (0x06)
#define IO_I2C_SLAVE_SET_HOLD_TIME_TX   (0x07)
#define IO_I2C_SLAVE_SET_SPKLEN         (0x08)
#define IO_I2C_SLAVE_SET_RX_THRESHOLD   (0x09)
#define IO_I2C_SLAVE_SET_TX_THRESHOLD   (0x0a)
#define IO_I2C_SLAVE_SET_10BIT_ADDR     (0x0b)
/* I2C slave control commands */
#define IO_I2C_SLAVE_SET_ADDR           (0x10)

extern uint32_t io_i2c_slave_open(uint32_t dev_id);
extern void io_i2c_slave_close(uint32_t dev_id);
/* I2C slave valid IOCTLs
   cmd                                 arg type        arg value
   IO_SET_CB_RX                        io_cb_t         Callback function invoked from ISR when read successfully finishes
   IO_SET_CB_TX                        io_cb_t         Callback function invoked from ISR when write successfully finishes
   IO_SET_CB_ERR                       io_cb_t         Callback function invoked from ISR when read/write fails
   IO_I2C_SLAVE_SET_HOLD_TIME_RX       uint32_t        SDA hold delay, in I2C clocks
   IO_I2C_SLAVE_SET_HOLD_TIME_TX       uint32_t        SDA hold delay, in I2C clocks
   IO_I2C_SLAVE_SET_SETUP_TIME         uint32_t        SDA setup delay, in I2C clocks
   IO_I2C_SLAVE_SET_SPKLEN             uint32_t        Threshold of spike suppression logic counters, in I2C clocks
   IO_I2C_SLAVE_SET_ADDR               uint32_t        I2C slave address
   IO_I2C_SLAVE_SET_RX_THRESHOLD       uint32_t        threshold level rx fifo
   IO_I2C_SLAVE_SET_TX_THRESHOLD       uint32_t        threshold level tx fifo
   IO_I2C_SLAVE_SET_10BIT_ADDR         uint32_t        0 - 7 bit address, 1 - 10 bit address
 */
extern void io_i2c_slave_ioctl(uint32_t dev_id, uint32_t cmd, void *arg);
extern void io_i2c_slave_read(uint32_t dev_id, uint8_t *data, uint32_t *size);
extern void io_i2c_slave_write(uint32_t dev_id, uint8_t *data, uint32_t *size);

#endif        /* H_I2C_SLAVE */
