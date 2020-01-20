/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#include <tos_k.h>
#include "riscv_port.h"

__PORT__ void port_systick_config(uint32_t cycle_per_tick)
{

    // it cost cpu read two times, first mtime_lo and then mtime_hi
    // if mtime_lo == 0xFFFFFFFF and mtime_hi = 0 at first read
    // then mtime_lo == 0 and mtime_hi = 1 at next read
    // the result will be  0x1FFFFFFFF, not 0x100000000
    uint64_t mtime = 0;
    while (1) {
        uint32_t mtime_hi = *(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIME + 4);
        uint32_t mtime_lo = *(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIME + 0);
        uint32_t mtime_hn = *(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIME + 4);
        if(mtime_hi == mtime_hn) {
            mtime += ((uint64_t)mtime_hi << 32) | mtime_lo;
            break;
        }
    }

    // just set mtime to mtimecmp does not accurately reflect the passage of time
    // cause some time cost on the path to deal with the interrupt
    // so, we need to to fix the value with a multiple of cycle_per_tick
    uint64_t tick = mtime / cycle_per_tick;
    uint64_t mtimecmp = (tick + 1) * cycle_per_tick;


    // write to mtimecmp register
    *(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP + 0) = 0xFFFFFFFF;
    *(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP + 4) = 0xFFFFFFFF & (mtimecmp >> 32);
    *(volatile uint32_t *)(CLINT_CTRL_ADDR + CLINT_MTIMECMP + 0) = 0xFFFFFFFF & (mtimecmp >>  0);
}
