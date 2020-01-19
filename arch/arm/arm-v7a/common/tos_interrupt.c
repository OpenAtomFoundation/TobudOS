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

#include "tos_k.h"

__STATIC__ int_handle_t int_handle_table[INTERRUPT_MAX];

__KERNEL__ void interrupt_irq(int_frame_t *int_frame)
{
    uint32_t vector;
    int_handle_t *handle;

    vector = gic_interrupt_id_get(0u);

    if (vector > INTERRUPT_MAX || vector == (uint32_t)-1) {
        return;
    }

    handle = &int_handle_table[vector];
    if (handle->handler) {
        handle->handler(handle->arg);
    }

    gic_interrupt_end(0u, vector);
}

__KERNEL__ int interrupt_init(void)
{
    gic_init(0u);
    return 0;
}

__API__ int tos_interrupt_handler_register(uint32_t vector, int_handler_t handler, void *arg)
{
    if (vector < INTERRUPT_MAX) {
        int_handle_table[vector].handler    = handler;
        int_handle_table[vector].arg        = arg;
        return 0;
    }

    return -1;
}

__API__ void tos_interrupt_enable(uint32_t vector)
{
    gic_interrupt_enable(0u, vector);
}

__API__ void tos_interrupt_disable(uint32_t vector)
{
    gic_interrupt_disable(0u, vector);
}

