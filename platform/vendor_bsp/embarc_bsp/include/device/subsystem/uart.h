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

#ifndef H_UART
#define H_UART

#include "device/ip_hal/ioctl.h"

/* UART control commands */
#define IO_UART_SET_LCR             (0x20)
#define IO_UART_GET_LCR             (0x21)
#define IO_UART_SET_DLx             (0x22)
#define IO_UART_GET_DLx             (0x23)
#define IO_UART_SET_RX_THRESHOLD    (0x24)
#define IO_UART_SET_TX_THRESHOLD    (0x25)
#define IO_UART_SET_FLOW_CONTROL    (0x26)

extern uint32_t io_uart_open(uint32_t dev_id);
extern void io_uart_close(uint32_t dev_id);
extern void io_uart_read(uint32_t dev_id, uint8_t *data, uint32_t *size);
extern void io_uart_write(uint32_t dev_id, uint8_t *data,
			  uint32_t *size);
/* UART valid IOCTLs
   cmd                         arg type        arg value
   IO_SET_CB_RX                io_cb_t         Callback function invoked from ISR when read successfully finishes
   IO_SET_CB_TX                io_cb_t         Callback function invoked from ISR when write successfully finishes
   IO_SET_CB_ERR               io_cb_t         Callback function invoked from ISR when read/write fails
   IO_UART_SET_LCR             uint32_t        7 bits LCR[6:0] = { BC, Stick Parity, EPS, PEN, STOP, DLS[1:0] }
   IO_UART_GET_LCR             uint32_t        7 bits LCR[6:0] = { BC, Stick Parity, EPS, PEN, STOP, DLS[1:0] }
   IO_UART_SET_DLx             uint32_t        16 bits DLH[7:0];DLL[7:0]
   IO_UART_GET_DLx             uint32_t        16 bits DLH[7:0];DLL[7:0]
   IO_UART_SET_RX_THRESHOLD    uint32_t        2 bits encoding the rx-fifo (if any) threshold
   IO_UART_SET_TX_THRESHOLD    uint32_t        2 bits encoding the tx-fifo (if any) threshold
   IO_UART_SET_FLOW_CONTROL    uint32_t        0 - disable flow control; 1 - enable flow control
 */
extern void io_uart_ioctl(uint32_t dev_id, uint32_t cmd, void *arg);

#endif              /* H_UART */
