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

#ifndef _TOS_CPU_TYPES_H_
#define  _TOS_CPU_TYPES_H_

/* CPU address type based on address bus size.          */
#if     (TOS_CFG_CPU_ADDR_SIZE == CPU_WORD_SIZE_32)
typedef uint32_t    cpu_addr_t;
#elif   (TOS_CFG_CPU_ADDR_SIZE == CPU_WORD_SIZE_16)
typedef uint16_t    cpu_addr_t;
#else
typedef uint8_t     cpu_addr_t;
#endif

/* CPU data type based on data bus size.          */
#if     (TOS_CFG_CPU_DATA_SIZE == CPU_WORD_SIZE_32)
typedef uint32_t    cpu_data_t;
#elif   (TOS_CFG_CPU_DATA_SIZE == CPU_WORD_SIZE_16)
typedef uint16_t    cpu_data_t;
#else
typedef uint8_t     cpu_data_t;
#endif

#if     (TOS_CFG_CPU_HRTIMER_EN > 0)
#if     (TOS_CFG_CPU_HRTIMER_SIZE == CPU_WORD_SIZE_08)
typedef uint8_t     cpu_hrtimer_t;
#elif   (TOS_CFG_CPU_HRTIMER_SIZE == CPU_WORD_SIZE_16)
typedef uint16_t    cpu_hrtimer_t;
#elif   (TOS_CFG_CPU_HRTIMER_SIZE == CPU_WORD_SIZE_64)
typedef uint64_t    cpu_hrtimer_t;
#else
typedef uint32_t    cpu_hrtimer_t;
#endif
#else
typedef uint32_t    cpu_hrtimer_t;
#endif

typedef cpu_addr_t  cpu_cpsr_t;

#endif

