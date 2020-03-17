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

#ifndef H_I2S_RX_MASTER
#define H_I2S_RX_MASTER

#include "device/ip_hal/ioctl.h"

/* I2S Rx master IO control commands */
#define IO_I2S_RX_MASTER_SET_SAMPLE_WIDTH       (0x20)
#define IO_I2S_RX_MASTER_SET_FIFO_THRESHOLD     (0x21)
#define IO_I2S_RX_MASTER_SET_BITCLOCK           (0x22)

extern uint32_t io_i2s_rx_master_open(uint32_t dev_id);
extern void io_i2s_rx_master_close(uint32_t dev_id);

/* Function:         io_i2s_rx_master_read
 * Parameters:
 *   dev_id [In]     Identifier of I2S device instance.
 *   data   [In]     Address of input buffer where requested number of acquired samples shall be stored
 *   size   [In/Out] Address of variable that holds input buffer size specifying the number of samples to be acquired.
 *                   On invocation of callback, registerd by IO_SET_CB_RX command, the address contains the number
 *                   of samples actually acquired.
 * Returns:
 *   none
 */
extern void io_i2s_rx_master_read(uint32_t dev_id, uint32_t *data,
				  uint32_t *size);

/* I2S valid IOCTLs
   cmd                                  arg type    arg value
   IO_SET_CB_RX                         io_cb_t     Callback function invoked from ISR whenever a series of samples
                                                        is available in the buffer provided by io_i2s_rx_master_read
   IO_SET_CB_ERR                        io_cb_t     Callback function invoked from ISR whenever input buffer overflow condition whould occur
   IO_I2S_RX_MASTER_SET_FIFO_THRESHOLD  uint32_t    threshold value for the Rx FIFO (accepts command before first io_i2s_rx_master_read)
   IO_I2S_RX_MASTER_SET_SAMPLE_WIDTH    uint32_t    sample width value 16 or 12 bits (accepts command before first io_i2s_rx_master_read)
   IO_I2S_RX_MASTER_SET_BITCLOCK        uint32_t    0/1 - disable/enable i2s bit clock ans WS (accepts command before first io_i2s_rx_master_read)
 */
extern void io_i2s_rx_master_ioctl(uint32_t dev_id, uint32_t cmd, void *arg);

#endif              /* H_I2S_RX_MASTER */
