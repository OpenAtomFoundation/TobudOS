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

#ifndef _PORT_CONFIG_H_
#define _PORT_CONFIG_H_

/* FPU is used */
#define ARCH_RISCV_FPU                          0u

#define TOS_CFG_CPU_ADDR_SIZE                   CPU_WORD_SIZE_32
#define TOS_CFG_CPU_DATA_SIZE                   CPU_WORD_SIZE_32
#define TOS_CFG_CPU_STK_GROWTH                  CPU_STK_GROWTH_DESCENDING
#define TOS_CFG_CPU_HRTIMER_EN                  0u
#define TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT      0u
#define TOS_CFG_CPU_BYTE_ORDER                  CPU_BYTE_ORDER_LITTLE_ENDIAN



/* int reg offset table */
#define __reg_mepc_OFFSET       0x00
#define __reg_mstatus_OFFSET    0x04
#define __reg_x1_OFFSET         0x08
#define __reg_x3_OFFSET         0x0C
#define __reg_x4_OFFSET         0x10
#define __reg_x5_OFFSET         0x14
#define __reg_x6_OFFSET         0x18
#define __reg_x7_OFFSET         0x1C
#define __reg_x8_OFFSET         0x20
#define __reg_x9_OFFSET         0x24
#define __reg_x10_OFFSET        0x28
#define __reg_x11_OFFSET        0x2C
#define __reg_x12_OFFSET        0x30
#define __reg_x13_OFFSET        0x34
#define __reg_x14_OFFSET        0x38
#define __reg_x15_OFFSET        0x3C
#define __reg_x16_OFFSET        0x40
#define __reg_x17_OFFSET        0x44
#define __reg_x18_OFFSET        0x48
#define __reg_x19_OFFSET        0x4C
#define __reg_x20_OFFSET        0x50
#define __reg_x21_OFFSET        0x54
#define __reg_x22_OFFSET        0x58
#define __reg_x23_OFFSET        0x5C
#define __reg_x24_OFFSET        0x60
#define __reg_x25_OFFSET        0x64
#define __reg_x26_OFFSET        0x68
#define __reg_x27_OFFSET        0x6C
#define __reg_x28_OFFSET        0x70
#define __reg_x29_OFFSET        0x74
#define __reg_x30_OFFSET        0x78
#define __reg_x31_OFFSET        0x7C

#define __reg_mepc__OFFSET      __reg_mepc_OFFSET
#define __reg_mstatus__OFFSET   __reg_mstatus_OFFSET
#define __reg_ra__OFFSET        __reg_x1_OFFSET
#define __reg_gp__OFFSET        __reg_x3_OFFSET
#define __reg_tp__OFFSET        __reg_x4_OFFSET
#define __reg_t0__OFFSET        __reg_x5_OFFSET
#define __reg_t1__OFFSET        __reg_x6_OFFSET
#define __reg_t2__OFFSET        __reg_x7_OFFSET
#define __reg_s0__OFFSET        __reg_x8_OFFSET
#define __reg_fp__OFFSET        __reg_x8_OFFSET
#define __reg_s1__OFFSET        __reg_x9_OFFSET
#define __reg_a0__OFFSET        __reg_x10_OFFSET
#define __reg_a1__OFFSET        __reg_x11_OFFSET
#define __reg_a2__OFFSET        __reg_x12_OFFSET
#define __reg_a3__OFFSET        __reg_x13_OFFSET
#define __reg_a4__OFFSET        __reg_x14_OFFSET
#define __reg_a5__OFFSET        __reg_x15_OFFSET
#define __reg_a6__OFFSET        __reg_x16_OFFSET
#define __reg_a7__OFFSET        __reg_x17_OFFSET
#define __reg_s2__OFFSET        __reg_x18_OFFSET
#define __reg_s3__OFFSET        __reg_x19_OFFSET
#define __reg_s4__OFFSET        __reg_x20_OFFSET
#define __reg_s5__OFFSET        __reg_x21_OFFSET
#define __reg_s6__OFFSET        __reg_x22_OFFSET
#define __reg_s7__OFFSET        __reg_x23_OFFSET
#define __reg_s8__OFFSET        __reg_x24_OFFSET
#define __reg_s9__OFFSET        __reg_x25_OFFSET
#define __reg_s10__OFFSET       __reg_x26_OFFSET
#define __reg_s11__OFFSET       __reg_x27_OFFSET
#define __reg_t3__OFFSET        __reg_x28_OFFSET
#define __reg_t4__OFFSET        __reg_x29_OFFSET
#define __reg_t5__OFFSET        __reg_x30_OFFSET
#define __reg_t6__OFFSET        __reg_x31_OFFSET


#if ARCH_RISCV_FPU
/* float reg offset table */
#define __reg_f0_OFFSET    0x00
#define __reg_f1_OFFSET    0x04
#define __reg_f2_OFFSET    0x08
#define __reg_f3_OFFSET    0x0C
#define __reg_f4_OFFSET    0x10
#define __reg_f5_OFFSET    0x14
#define __reg_f6_OFFSET    0x18
#define __reg_f7_OFFSET    0x1C
#define __reg_f8_OFFSET    0x20
#define __reg_f9_OFFSET    0x24
#define __reg_f10_OFFSET   0x28
#define __reg_f11_OFFSET   0x2C
#define __reg_f12_OFFSET   0x30
#define __reg_f13_OFFSET   0x34
#define __reg_f14_OFFSET   0x38
#define __reg_f15_OFFSET   0x3C
#define __reg_f16_OFFSET   0x40
#define __reg_f17_OFFSET   0x44
#define __reg_f18_OFFSET   0x48
#define __reg_f19_OFFSET   0x4C
#define __reg_f20_OFFSET   0x50
#define __reg_f21_OFFSET   0x54
#define __reg_f22_OFFSET   0x58
#define __reg_f23_OFFSET   0x5C
#define __reg_f24_OFFSET   0x60
#define __reg_f25_OFFSET   0x64
#define __reg_f26_OFFSET   0x68
#define __reg_f27_OFFSET   0x6C
#define __reg_f28_OFFSET   0x70
#define __reg_f29_OFFSET   0x74
#define __reg_f30_OFFSET   0x78
#define __reg_f31_OFFSET   0x7C

#endif



#endif /* _PORT_CONFIG_H_ */

