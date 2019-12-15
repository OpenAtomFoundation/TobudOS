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

#ifndef _RISCV_PORT_H_
#define _RISCV_PORT_H_

#define CLINT_CTRL_ADDR 0x2000000
#define CLINT_MSIP      0x0000
#define CLINT_MTIMECMP  0x4000
#define CLINT_MTIME     0xBFF8

#define MCAUSE_EXP_CODE_MASK 0x7FFFFFFF

#endif /* _RISCV_PORT_H_ */
