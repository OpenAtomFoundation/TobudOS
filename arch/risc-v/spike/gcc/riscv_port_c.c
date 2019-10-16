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

#include "tos.h"

__PORT__ void port_cpu_init() {
    // DO NOTHING
}

__PORT__ void port_systick_priority_set(uint32_t priority) {
    // DO NOTHING
}


extern cpu_data_t irq_vector_table_base;
__PORT__ void* port_get_irq_vector_table() {
    void *base = (void *) &irq_vector_table_base;
    return base;
}
