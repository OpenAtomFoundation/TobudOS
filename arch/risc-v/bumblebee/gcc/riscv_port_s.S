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

.global eclic_mtip_handler
.global irq_entry
.global trap_entry

.extern SysTick_IRQHandler

.align 2
eclic_mtip_handler:
	/* normal code */
    // add sp, sp, -4
    // sw ra, (sp)
    // call SysTick_IRQHandler
    // lw ra, (sp)
    // add sp, sp, 4
    // ret

	/* the most efficient code */
	j SysTick_IRQHandler
	// the code will return to caller directly from SysTick_IRQHandler

.align 2
irq_entry:
    j irq_entry

.align 2
trap_entry:
    j trap_entry

