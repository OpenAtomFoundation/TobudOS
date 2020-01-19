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

#ifndef _TOS_ARM_H_
#define  _TOS_ARM_H_

#define DSB __ASM__ __VOLATILE__("dsb" ::: "memory")
#define DMB __ASM__ __VOLATILE__("dmb" ::: "memory")
#define ISB __ASM__ __VOLATILE__("isb" ::: "memory")

#define __STRINGIFY(x) #x

#define ARM_MCR(coproc, opcode_1, src, CRn, CRm, opcode_2)                          \
     __ASM__ __VOLATILE__ ("MCR " __STRINGIFY(p##coproc) ", " __STRINGIFY(opcode_1) ", " \
                     "%0, " __STRINGIFY(c##CRn) ", " __STRINGIFY(c##CRm) ", "      \
                     __STRINGIFY(opcode_2)                                         \
                     : : "r" (src) )

#define ARM_MRC(coproc, opcode_1, CRn, CRm, opcode_2)                               \
   ({                                                                              \
     uint32_t __dst;                                                               \
     __ASM__ __VOLATILE__ ("MRC " __STRINGIFY(p##coproc) ", " __STRINGIFY(opcode_1) ", " \
                     "%0, " __STRINGIFY(c##CRn) ", " __STRINGIFY(c##CRm) ", "      \
                     __STRINGIFY(opcode_2)                                         \
                     : "=r" (__dst) );                                             \
     __dst;                                                                        \
   })

#endif /* _TOS_ARM_H_ */
