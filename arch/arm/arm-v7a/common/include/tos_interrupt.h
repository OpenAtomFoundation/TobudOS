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

#ifndef _TOS_INTERRUPT_H_
#define _TOS_INTERRUPT_H_

typedef void (*int_handler_t)(void *arg);

typedef struct int_handle_st {
    int_handler_t       handler;
    void               *arg;
} int_handle_t;

typedef struct int_frame_st {
#if 0
#if defined (TOS_CFG_CPU_ARM_FPU_EN) && (TOS_CFG_CPU_ARM_FPU_EN == 1U)
    uint32_t fpexc;
#endif
#endif
    cpu_data_t r0;
    cpu_data_t r1;
    cpu_data_t r2;
    cpu_data_t r3;
    cpu_data_t r4;
    cpu_data_t r5;
    cpu_data_t r6;
    cpu_data_t r7;
    cpu_data_t r8;
    cpu_data_t r9;
    cpu_data_t r10;
    cpu_data_t r11;
    cpu_data_t r12;

    cpu_data_t lr;

    cpu_data_t pc;
    cpu_data_t spsr;
} int_frame_t;

__KNL__ void interrupt_irq(int_frame_t *int_frame);

__KNL__ int interrupt_init(void);

__API__ int tos_interrupt_handler_register(uint32_t vector, int_handler_t handler, void *arg);

__API__ void tos_interrupt_enable(uint32_t vector);

__API__ void tos_interrupt_disable(uint32_t vector);

#endif /* _TOS_INTERRUPT_H_ */

