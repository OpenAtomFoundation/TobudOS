/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file pal_os_lock.c
*
* \brief   This file implements the platform abstraction layer APIs for os locks (e.g. semaphore).
*
* \addtogroup  grPAL
* @{
*/

#include "optiga/pal/pal_os_lock.h"
#include "nrf_atomic.h"
#include "nrf_pwr_mgmt.h"

/**
 * @brief PAL OS lock structure. Might be extended if needed
 */
typedef struct pal_os_lock
{
    nrf_atomic_flag_t lock;
} pal_os_lock_t;

static volatile pal_os_lock_t pal_os_lock = {.lock = 0};

pal_status_t pal_os_lock_acquire(void)
{
    // wait until previous value was false, this indicates the lock was free and we own it now.
    while(nrf_atomic_flag_set_fetch(&pal_os_lock.lock)) {
        nrf_pwr_mgmt_run();
    }

    return PAL_STATUS_SUCCESS;
}

void pal_os_lock_release(void)
{
    (void)nrf_atomic_flag_clear(&pal_os_lock.lock);
}

/**
* @}
*/
