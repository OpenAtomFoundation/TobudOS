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


#define CLINT_CTRL_ADDR 0xD1000000
#define CLINT_MSIP      0x0FFC
#define CLINT_MTIMECMP  0x0008
#define CLINT_MTIME     0x0000

// the bumblebee mstatus register is different
// name             bit         detail
// INTERRUPT        31          0: exception or nmi, 1 irq
// MINHV            30          reading irq vector table
// MPP              29:28       == mstatus.MPP
// MPIE             27          == mstatus.MPIE
// Reserved         26:24       0
// MPIL             23:16       previous interrupt level
// Reserved         15:12       0
// EXCCODE          11:0        exception code
#define MCAUSE_EXP_CODE_MASK    0x00000FFF

#endif // _RISCV_PORT_H_
