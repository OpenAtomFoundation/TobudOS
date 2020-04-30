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

#ifndef _TOS_FAULT_H_
#define  _TOS_FAULT_H_

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u

typedef int (*k_fault_log_writer_t)(const char *format, ...);

#define K_FAULT_STACK_DUMP_DEPTH                10u

#define K_FAULT_CALL_STACK_BACKTRACE_DEPTH          5u

__API__ void tos_fault_log_writer_set(k_fault_log_writer_t log_writer);

__KERNEL__ int fault_default_log_writer(const char *format, ...);

__KERNEL__ void fault_backtrace(cpu_addr_t lr, fault_exc_frame_t *frame);

#endif

#endif /* _TOS_FAULT_H_ */

