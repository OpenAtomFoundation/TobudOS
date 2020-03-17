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

#ifndef H_GPIO
#define H_GPIO

#include "device/ip_hal/ioctl.h"

// convenience macro to get unique device IDs for 32b/8b/4b GPIO; max nr of instances per kind = 4
#define IO_GPIO_DEV_ID(id, width)            (uint8_t)((width == 32) ? (id) : (12 - width + id))

#define IO_GPIO_SET_DIRECTION               (0x20)
#define IO_GPIO_SET_INT_ENABLE              (0x21)
#define IO_GPIO_GET_INT_ENABLE              (0x22)
#define IO_GPIO_SET_INT_MASK                (0x23)
#define IO_GPIO_GET_INT_MASK                (0x24)
#define IO_GPIO_SET_INT_TYPE                (0x25)
#define IO_GPIO_SET_INT_POLARITY            (0x26)
#define IO_GPIO_SET_INT_BOTHEDGE            (0x27)
#define IO_GPIO_SET_DEBOUNCE                (0x28)
#define IO_GPIO_GET_DIRECTION               (0x29)
#define IO_GPIO_GET_GPO                     (0x30)

extern uint32_t io_gpio_open(uint32_t dev_id);
extern void io_gpio_close(uint32_t dev_id);
extern void io_gpio_read(uint32_t dev_id, uint32_t *data);
extern void io_gpio_write(uint32_t dev_id, uint32_t data);

/* GPIO valid IOCTLs
   cmd                          arg type        arg value
   IO_SET_CB_RX                 io_cb_t         Callback function invoked from ISR whenever the interrupt is triggered on one of
                                                    GPIO inputs configured to operate as interrupts. The callback's argument is initialized
                                                    with bitmask where each bit represents corresponding GPIO signal and is set to 1 if
                                                    that signal raises interrupt.
   IO_GPIO_SET_DIRECTION        uint32_t        Bitmask, where each bit's value controls the direction of the corresponding GPIO signal as follows:
                                                    0 - input, 1 - output. Default value of the control is zero that corresponds to all signals are inputs
                                                    on device opening.
   IO_GPIO_SET_INT_ENABLE       uint32_t        Bitmask, where each bit's value specifies if corresponding GPIO input shall act as interrupt or as a normal
   (IO_GPIO_GET_INT_ENABLE)                     GPIO signal as follows: 0 - normal signal, 1 - interrupt. Default value of the control is zero that corresponds
                                                    to all inputs are normal GPIO signals on device opening.
   IO_GPIO_SET_INT_MASK         uint32_t        Bitmask, where each bit's value specifies if corresponding GPIO interrupt signal is masked:
   (IO_GPIO_GET_INT_MASK)                       0 - unmasked, 1 - masked. Default value of the control is zero that corresponds to
                                                    all interrupts are unmasked on device opening.
   IO_GPIO_SET_INT_TYPE         uint32_t        Bitmask, where each bit's value specifies the type of corresponding GPIO interrupt as follows:
                                                    0 - level-sensitive, 1 - edge-sensitive. Default value of the control is zero that corresponds to
                                                    all interrupts are level-sensitive on device opening.
   IO_GPIO_SET_INT_POLARITY     uint32_t        Bitmask, where each bit's value specifies the polarity of corresponding GPIO interrupt as follows:
                                                    0 - active-low (falling edge), 1 - active-high(raising edge). Default value of the control is zero
                                                    that corresponds to all interrupts are active-low on device opening.
   IO_GPIO_SET_INT_BOTHEDGE     uint32_t        Bitmask, where each bit's value specifies the type of corresponding GPIO interrupt as follows:
                                                    0 - interrupt defined by INT_TYPE and INT_POLARITY, 1 - interrupt edge sensitive for both rising and falling edge
                                                    that corresponds to all interrupts are defined by INT_TYPE and INT_POLARITY.
   IO_GPIO_SET_DEBOUNCE         uint32_t        Bitmask, where each bit's value specify if corresponding GPIO signal configured as interrupt source needs
                                                    to be debounced as follows: 0 - no debounce, 1 - enable debounce. Default value of the control is zero
                                                    that corresponds to all GPIO signals aren't debounced on device opening.
   IO_GPIO_GET_DIRECTION        uint32_t        Same as IO_GPIO_SET_DIRECTION.
   IO_GPIO_GET_GPO              uint32_t        Last value written to the port data register using io_gpio_write() service, does not depend on GPIO direction
                                                    settings and can be different compare to data returened by io_gpio_read() service.
 */
extern void io_gpio_ioctl(uint32_t dev_id, uint32_t cmd, void *arg);

#endif        /* H_GPIO */
