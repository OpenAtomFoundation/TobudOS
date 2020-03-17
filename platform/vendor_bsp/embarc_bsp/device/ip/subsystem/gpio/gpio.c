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

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "device/subsystem/gpio.h"
#include "io_config.h"
#include "apexextensions.h"

/* EIA GPIO device registers  */
#define SWPORTA_DR      (0x00)  /* GPIO Port A Data Register */
#define SWPORTA_DDR     (0x04)  /* GPIO Port A Data Direction Register */
#define INTEN           (0x30)  /* GPIO Interrupt Enable Register */
#define INTMASK         (0x34)  /* GPIO Interrupt Mask Register */
#define INTTYPE_LEVEL   (0x38)  /* GPIO Interrupt Type Register */
#define INT_POLARITY    (0x3c)  /* GPIO Interrupt Polarity Register */
#define INTSTATUS       (0x44)  /* GPIO Interrupt Status Register */
#define DEBOUNCE        (0x48)  /* GPIO Debounce Enable Register */
#define PORTA_EOI       (0x4c)  /* GPIO Port A Clear Interrupt Register */
#define EXT_PORTA       (0x50)  /* GPIO External Port A Register */
#define LS_SYNC         (0x60)  /* GPIO Level-Sensitive Sync Enable Register */
#define INT_BOTHEDGE    (0x68)  /* GPIO Both Edge Register */
#define CLKEN           (0x80)  /* GPIO Clock Enable Register */

typedef _Interrupt void (*ISR) ();

/* Private data structure maintained by the driver. */
typedef struct gpio_info_struct {
	uint32_t reg_base;      /* base address of device register set */
	/* Callbacks */
	IO_CB_FUNC rx_cb;
	uint8_t opened;
	uint8_t instID;
	/* Interrupt numbers and handlers */
	uint8_t vector;         /* GPIO ISR vector */
	ISR isr;                /* GPIO ISR */
	/* Config option */
	uint8_t enableDebounce;
} gpio_info_t, *gpio_info_pt;

#ifdef IO_GPIO0_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO1_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO2_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO3_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO_8B0_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO_8B1_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO_8B2_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO_8B3_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO_4B0_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO_4B1_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO_4B2_PRESENT
#define GPIO_DEV_PRESENT
#endif
#ifdef IO_GPIO_4B3_PRESENT
#define GPIO_DEV_PRESENT
#endif

#ifdef GPIO_DEV_PRESENT

#define GPIO_32_MAX_CNT (4)
#define GPIO_8B_MAX_CNT (4)
#define GPIO_4B_MAX_CNT (4)
#define GPIO_MAX_CNT    ((GPIO_32_MAX_CNT) + (GPIO_8B_MAX_CNT) + (GPIO_4B_MAX_CNT))

static void gpio_ISR_proc(uint32_t dev_id);

#ifdef IO_GPIO0_PRESENT
static _Interrupt void gpio0_ISR()
{
	gpio_ISR_proc(0);
}
#endif
#ifdef IO_GPIO1_PRESENT
static _Interrupt void gpio1_ISR()
{
	gpio_ISR_proc(1);
}
#endif
#ifdef IO_GPIO2_PRESENT
static _Interrupt void gpio2_ISR()
{
	gpio_ISR_proc(2);
}
#endif
#ifdef IO_GPIO3_PRESENT
static _Interrupt void gpio3_ISR()
{
	gpio_ISR_proc(3);
}
#endif

#ifdef IO_GPIO_8B0_PRESENT
static _Interrupt void gpio_8B_0_ISR()
{
	gpio_ISR_proc(0 + GPIO_32_MAX_CNT);
}
#endif
#ifdef IO_GPIO_8B1_PRESENT
static _Interrupt void gpio_8B_1_ISR()
{
	gpio_ISR_proc(1 + GPIO_32_MAX_CNT);
}
#endif
#ifdef IO_GPIO_8B2_PRESENT
static _Interrupt void gpio_8B_2_ISR()
{
	gpio_ISR_proc(2 + GPIO_32_MAX_CNT);
}
#endif
#ifdef IO_GPIO_8B3_PRESENT
static _Interrupt void gpio_8B_3_ISR()
{
	gpio_ISR_proc(3 + GPIO_32_MAX_CNT);
}
#endif

#ifdef IO_GPIO_4B0_PRESENT
static _Interrupt void gpio_4B_0_ISR()
{
	gpio_ISR_proc(0 + GPIO_32_MAX_CNT + GPIO_8B_MAX_CNT);
}
#endif
#ifdef IO_GPIO_4B1_PRESENT
static _Interrupt void gpio_4B_1_ISR()
{
	gpio_ISR_proc(1 + GPIO_32_MAX_CNT + GPIO_8B_MAX_CNT);
}
#endif
#ifdef IO_GPIO_4B2_PRESENT
static _Interrupt void gpio_4B_2_ISR()
{
	gpio_ISR_proc(2 + GPIO_32_MAX_CNT + GPIO_8B_MAX_CNT);
}
#endif
#ifdef IO_GPIO_4B3_PRESENT
static _Interrupt void gpio_4B_3_ISR()
{
	gpio_ISR_proc(3 + GPIO_32_MAX_CNT + GPIO_8B_MAX_CNT);
}
#endif

/* GPIO devices private data structures */
static gpio_info_pt gpio_handles[GPIO_MAX_CNT] = { 0 };

static gpio_info_t gpio_devs[] = {
#ifdef IO_GPIO0_PRESENT
	{ .instID = IO_GPIO_DEV_ID(0, 32),
	  .reg_base = AR_IO_GPIO0_SWPORTA_DR,
	  .vector = IO_GPIO0_INT_INTR_FLAG,
	  .isr = gpio0_ISR,
	  .enableDebounce = IO_GPIO0_DEBOUNCE },
#endif
#ifdef IO_GPIO1_PRESENT
	{ .instID = IO_GPIO_DEV_ID(1, 32),
	  .reg_base = AR_IO_GPIO1_SWPORTA_DR,
	  .vector = IO_GPIO1_INT_INTR_FLAG,
	  .isr = gpio1_ISR,
	  .enableDebounce = IO_GPIO1_DEBOUNCE },
#endif
#ifdef IO_GPIO2_PRESENT
	{ .instID = IO_GPIO_DEV_ID(2, 32),
	  .reg_base = AR_IO_GPIO2_SWPORTA_DR,
	  .vector = IO_GPIO2_INT_INTR_FLAG,
	  .isr = gpio2_ISR,
	  .enableDebounce = IO_GPIO2_DEBOUNCE },
#endif
#ifdef IO_GPIO3_PRESENT
	{ .instID = IO_GPIO_DEV_ID(3, 32),
	  .reg_base = AR_IO_GPIO3_SWPORTA_DR,
	  .vector = IO_GPIO3_INT_INTR_FLAG,
	  .isr = gpio3_ISR,
	  .enableDebounce = IO_GPIO3_DEBOUNCE },
#endif

#ifdef IO_GPIO_8B0_PRESENT
	{ .instID = IO_GPIO_DEV_ID(0, 8),
	  .reg_base = AR_IO_GPIO_8B0_SWPORTA_DR,
	  .vector = IO_GPIO_8B0_INT_INTR_FLAG,
	  .isr = gpio_8B_0_ISR,
	  .enableDebounce = IO_GPIO_8B0_DEBOUNCE },
#endif
#ifdef IO_GPIO_8B1_PRESENT
	{ .instID = IO_GPIO_DEV_ID(1, 8),
	  .reg_base = AR_IO_GPIO_8B1_SWPORTA_DR,
	  .vector = IO_GPIO_8B1_INT_INTR_FLAG,
	  .isr = gpio_8B_1_ISR,
	  .enableDebounce = IO_GPIO_8B1_DEBOUNCE },
#endif
#ifdef IO_GPIO_8B2_PRESENT
	{ .instID = IO_GPIO_DEV_ID(2, 8),
	  .reg_base = AR_IO_GPIO_8B2_SWPORTA_DR,
	  .vector = IO_GPIO_8B2_INT_INTR_FLAG,
	  .isr = gpio_8B_2_ISR,
	  .enableDebounce = IO_GPIO_8B2_DEBOUNCE },
#endif
#ifdef IO_GPIO_8B3_PRESENT
	{ .instID = IO_GPIO_DEV_ID(3, 8),
	  .reg_base = AR_IO_GPIO_8B3_SWPORTA_DR,
	  .vector = IO_GPIO_8B3_INT_INTR_FLAG,
	  .isr = gpio_8B_3_ISR,
	  .enableDebounce = IO_GPIO_8B3_DEBOUNCE },
#endif

#ifdef IO_GPIO_4B0_PRESENT
	{ .instID = IO_GPIO_DEV_ID(0, 4),
	  .reg_base = AR_IO_GPIO_4B0_SWPORTA_DR,
	  .vector = IO_GPIO_4B0_INT_INTR_FLAG,
	  .isr = gpio_4B_0_ISR,
	  .enableDebounce = IO_GPIO_4B0_DEBOUNCE },
#endif
#ifdef IO_GPIO_4B1_PRESENT
	{ .instID = IO_GPIO_DEV_ID(1, 4),
	  .reg_base = AR_IO_GPIO_4B1_SWPORTA_DR,
	  .vector = IO_GPIO_4B1_INT_INTR_FLAG,
	  .isr = gpio_4B_1_ISR,
	  .enableDebounce = IO_GPIO_4B1_DEBOUNCE },
#endif
#ifdef IO_GPIO_4B2_PRESENT
	{ .instID = IO_GPIO_DEV_ID(2, 4),
	  .reg_base = AR_IO_GPIO_4B2_SWPORTA_DR,
	  .vector = IO_GPIO_4B2_INT_INTR_FLAG,
	  .isr = gpio_4B_2_ISR,
	  .enableDebounce = IO_GPIO_4B2_DEBOUNCE },
#endif
#ifdef IO_GPIO_4B3_PRESENT
	{ .instID = IO_GPIO_DEV_ID(3, 4),
	  .reg_base = AR_IO_GPIO_4B3_SWPORTA_DR,
	  .vector = IO_GPIO_4B3_INT_INTR_FLAG,
	  .isr = gpio_4B_3_ISR,
	  .enableDebounce = IO_GPIO_4B3_DEBOUNCE },
#endif

	{ .instID = GPIO_MAX_CNT }
};

#define REG_WRITE(reg, x)   _sr((unsigned)(x), (unsigned)(dev->reg_base + reg))
#define REG_READ(reg)       _lr((unsigned)(dev->reg_base + reg))

uint32_t io_gpio_open(uint32_t dev_id)
{
	uint32_t h = 0;
	gpio_info_pt dev;

	h = 0;
	while ((gpio_devs[h].instID != dev_id)
	       && (gpio_devs[h].instID != GPIO_MAX_CNT)) {
		h++;
	}
	if ((gpio_devs[h].instID == GPIO_MAX_CNT) || (0 != gpio_handles[dev_id])) { /* dev_id not part of design, or still open */
		return 1;
	}
	gpio_handles[dev_id] = &gpio_devs[h];
	dev = gpio_handles[dev_id];

	dev->opened = 1;
	/* enable the clock and enable synchronization */
	REG_WRITE(CLKEN, 0x1);
	REG_WRITE(LS_SYNC, 0x1);
	_setvecti(dev->vector, dev->isr);

	return 0;
}

void io_gpio_close(uint32_t dev_id)
{
	gpio_info_pt dev = gpio_handles[dev_id];

	REG_WRITE(INTEN, 0x0);
	REG_WRITE(LS_SYNC, 0x0);
	REG_WRITE(CLKEN, 0x0);
	_setvecti(dev->vector, NULL);
	dev->rx_cb = NULL;
	dev->opened = 0;
	gpio_handles[dev_id] = 0;
}

void io_gpio_read(uint32_t dev_id, uint32_t *data)
{
	gpio_info_pt dev = gpio_handles[dev_id];

	*data = REG_READ(EXT_PORTA);
}

void io_gpio_write(uint32_t dev_id, uint32_t data)
{
	gpio_info_pt dev = gpio_handles[dev_id];

	REG_WRITE(SWPORTA_DR, data);
}

void io_gpio_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
	gpio_info_pt dev = gpio_handles[dev_id];

	switch (cmd) {
	case IO_SET_CB_RX:
		dev->rx_cb = ((io_cb_t *) arg)->cb;
		break;

	case IO_GPIO_SET_DIRECTION:
		REG_WRITE(SWPORTA_DDR, *((uint32_t *) arg));
		break;

	case IO_GPIO_SET_INT_ENABLE:
		REG_WRITE(INTEN, *((uint32_t *) arg));
		break;

	case IO_GPIO_GET_INT_ENABLE:
		*((uint32_t *) arg) = REG_READ(INTEN);
		break;

	case IO_GPIO_SET_INT_MASK:
		REG_WRITE(INTMASK, *((uint32_t *) arg));
		break;

	case IO_GPIO_GET_INT_MASK:
		*((uint32_t *) arg) = REG_READ(INTMASK);
		break;

	case IO_GPIO_SET_INT_TYPE:
		REG_WRITE(INTTYPE_LEVEL, *((uint32_t *) arg));
		break;

	case IO_GPIO_SET_INT_POLARITY:
		REG_WRITE(INT_POLARITY, *((uint32_t *) arg));
		break;

	case IO_GPIO_SET_INT_BOTHEDGE:
		REG_WRITE(INT_BOTHEDGE, *((uint32_t *) arg));
		break;

	case IO_GPIO_SET_DEBOUNCE:
		if (dev->enableDebounce == 1) {
			REG_WRITE(DEBOUNCE, *((uint32_t *) arg));
		}
		break;

	case IO_GPIO_GET_DIRECTION:
		*((uint32_t *) arg) = REG_READ(SWPORTA_DDR);
		break;

	case IO_GPIO_GET_GPO:
		*((uint32_t *) arg) = REG_READ(SWPORTA_DR);
		break;

	default:
		break;
	}
}

static void gpio_ISR_proc(uint32_t dev_id)
{
	gpio_info_pt dev = gpio_handles[dev_id];
	uint32_t status = REG_READ(INTSTATUS);

	if (!status) {
		return;     /* Spurious interrupts. */

	}
	if (_Usually(NULL != dev->rx_cb)) {
		dev->rx_cb(status);
	}

	REG_WRITE(PORTA_EOI, status);   /* clear status register. */
}

#endif
