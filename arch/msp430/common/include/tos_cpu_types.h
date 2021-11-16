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

#if( __DATA_MODEL__ == __DATA_MODEL_SMALL__ )
/* CPU data type based on data bus size.          */
typedef uint16_t    cpu_data_t;
/* CPU address type based on address bus size.          */
typedef uint16_t    cpu_addr_t;
#else
/* CPU data type based on data bus size.          */
typedef uint32_t    cpu_data_t;
/* CPU address type based on address bus size.          */
typedef uint32_t    cpu_addr_t;
#endif

typedef cpu_addr_t  cpu_cpsr_t;

#endif

