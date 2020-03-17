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
 * Library DW_DFSS-2.1.13
 * ========================================================================== */

#include <stdint.h>
#include "device/subsystem/creg_master.h"
#include "io_config.h"
#include "apexextensions.h"

/* determine whether there is at least a single CREG master instance */
#ifdef IO_CREG_MST0_PRESENT
#define CREG_MASTER_DEV_PRESENT
#endif
#ifdef IO_CREG_MST1_PRESENT
#define CREG_MASTER_DEV_PRESENT
#endif
#ifdef IO_CREG_MST2_PRESENT
#define CREG_MASTER_DEV_PRESENT
#endif
#ifdef IO_CREG_MST3_PRESENT
#define CREG_MASTER_DEV_PRESENT
#endif

#ifdef CREG_MASTER_DEV_PRESENT

/* Addresses of CREG master instances */
static const uint32_t creg_masters[4] = {
#ifdef IO_CREG_MST0_PRESENT
	AR_IO_CREG_MST0_CTRL,
#else
	0,
#endif
#ifdef IO_CREG_MST1_PRESENT
	AR_IO_CREG_MST1_CTRL,
#else
	0,
#endif
#ifdef IO_CREG_MST2_PRESENT
	AR_IO_CREG_MST2_CTRL,
#else
	0,
#endif
#ifdef IO_CREG_MST3_PRESENT
	AR_IO_CREG_MST3_CTRL,
#else
	0,
#endif
};

#define CREG_WRITE(x)   _sr((unsigned)(x), (unsigned)creg_masters[dev_id])
#define CREG_READ( )      _lr((unsigned)creg_masters[dev_id])

void io_creg_master_read(uint32_t dev_id, uint32_t *reg_val)
{
	*reg_val = CREG_READ();
}

void io_creg_master_write(uint32_t dev_id, uint32_t reg_val)
{
	CREG_WRITE(reg_val);
}

#endif
