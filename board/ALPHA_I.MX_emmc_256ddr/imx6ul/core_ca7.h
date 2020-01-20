#ifndef __CORTEX_CA7_H
#define __CORTEX_CA7_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 core_ca7.h
作者	   : 左忠凯
版本	   : V1.0
描述	   : Cortex-A7内核通用文件。
其他	   : 本文件主要实现了对GIC操作函数
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/4 左忠凯创建
***************************************************************/
#include <stdint.h>
#include <string.h>


#define FORCEDINLINE  __attribute__((always_inline))
#define __ASM            __asm                         	/* GNU C语言内嵌汇编关键字 */ 
#define __INLINE         inline                      	/* GNU内联关键字 */             
#define __STATIC_INLINE  static inline					


#define     __IM     volatile const      /* 只读 */
#define     __OM     volatile            /* 只写 */
#define     __IOM    volatile            /* 读写 */
#define __STRINGIFY(x) #x

/* C语言实现MCR指令 */
#define __MCR(coproc, opcode_1, src, CRn, CRm, opcode_2)                          \
    __ASM volatile ("MCR " __STRINGIFY(p##coproc) ", " __STRINGIFY(opcode_1) ", " \
                    "%0, " __STRINGIFY(c##CRn) ", " __STRINGIFY(c##CRm) ", "      \
                    __STRINGIFY(opcode_2)                                         \
                    : : "r" (src) )

/* C语言实现MRC指令 */                    
#define __MRC(coproc, opcode_1, CRn, CRm, opcode_2)                               \
  ({                                                                              \
    uint32_t __dst;                                                               \
    __ASM volatile ("MRC " __STRINGIFY(p##coproc) ", " __STRINGIFY(opcode_1) ", " \
                    "%0, " __STRINGIFY(c##CRn) ", " __STRINGIFY(c##CRm) ", "      \
                    __STRINGIFY(opcode_2)                                         \
                    : "=r" (__dst) );                                             \
    __dst;                                                                        \
  })

/* 其他一些C语言内嵌汇编 */  
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_APSR(uint32_t apsr)
{
  __ASM volatile ("MSR apsr, %0" : : "r" (apsr) : "cc");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_CPSR(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, cpsr" : "=r" (result) );
  return(result);
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_CPSR(uint32_t cpsr)
{
  __ASM volatile ("MSR cpsr, %0" : : "r" (cpsr) : "cc");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_FPEXC(void)
{
  uint32_t result;

  __ASM volatile ("VMRS %0, fpexc" : "=r" (result) );
  return result;
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_FPEXC(uint32_t fpexc)
{
  __ASM volatile ("VMSR fpexc, %0" : : "r" (fpexc));
}


/*******************************************************************************
 *        		一些内核寄存器定义和抽象
  定义如下几个内核寄存器:
  - CPSR
  - CP15
 ******************************************************************************/

/* CPSR寄存器 
 * 参考资料：ARM Cortex-A(armV7)编程手册V4.0.pdf P46
 */
typedef union
{
  struct
  {
    uint32_t M:5;                        /*!< bit:  0.. 4  Mode field */
    uint32_t T:1;                        /*!< bit:      5  Thumb execution state bit */
    uint32_t F:1;                        /*!< bit:      6  FIQ mask bit */
    uint32_t I:1;                        /*!< bit:      7  IRQ mask bit */
    uint32_t A:1;                        /*!< bit:      8  Asynchronous abort mask bit */
    uint32_t E:1;                        /*!< bit:      9  Endianness execution state bit */
    uint32_t IT1:6;                      /*!< bit: 10..15  If-Then execution state bits 2-7 */
    uint32_t GE:4;                       /*!< bit: 16..19  Greater than or Equal flags */
    uint32_t _reserved0:4;               /*!< bit: 20..23  Reserved */
    uint32_t J:1;                        /*!< bit:     24  Jazelle bit */
    uint32_t IT0:2;                      /*!< bit: 25..26  If-Then execution state bits 0-1 */
    uint32_t Q:1;                        /*!< bit:     27  Saturation condition flag */
    uint32_t V:1;                        /*!< bit:     28  Overflow condition code flag */
    uint32_t C:1;                        /*!< bit:     29  Carry condition code flag */
    uint32_t Z:1;                        /*!< bit:     30  Zero condition code flag */
    uint32_t N:1;                        /*!< bit:     31  Negative condition code flag */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} CPSR_Type;


/* CP15的SCTLR寄存器 
 * 参考资料：Cortex-A7 Technical ReferenceManua.pdf P105
 */
typedef union
{
  struct
  {
    uint32_t M:1;                        /*!< bit:     0  MMU enable */
    uint32_t A:1;                        /*!< bit:     1  Alignment check enable */
    uint32_t C:1;                        /*!< bit:     2  Cache enable */
    uint32_t _reserved0:2;               /*!< bit: 3.. 4  Reserved */
    uint32_t CP15BEN:1;                  /*!< bit:     5  CP15 barrier enable */
    uint32_t _reserved1:1;               /*!< bit:     6  Reserved */
    uint32_t B:1;                        /*!< bit:     7  Endianness model */
    uint32_t _reserved2:2;               /*!< bit: 8.. 9  Reserved */
    uint32_t SW:1;                       /*!< bit:    10  SWP and SWPB enable */
    uint32_t Z:1;                        /*!< bit:    11  Branch prediction enable */
    uint32_t I:1;                        /*!< bit:    12  Instruction cache enable */
    uint32_t V:1;                        /*!< bit:    13  Vectors bit */
    uint32_t RR:1;                       /*!< bit:    14  Round Robin select */
    uint32_t _reserved3:2;               /*!< bit:15..16  Reserved */
    uint32_t HA:1;                       /*!< bit:    17  Hardware Access flag enable */
    uint32_t _reserved4:1;               /*!< bit:    18  Reserved */
    uint32_t WXN:1;                      /*!< bit:    19  Write permission implies XN */
    uint32_t UWXN:1;                     /*!< bit:    20  Unprivileged write permission implies PL1 XN */
    uint32_t FI:1;                       /*!< bit:    21  Fast interrupts configuration enable */
    uint32_t U:1;                        /*!< bit:    22  Alignment model */
    uint32_t _reserved5:1;               /*!< bit:    23  Reserved */
    uint32_t VE:1;                       /*!< bit:    24  Interrupt Vectors Enable */
    uint32_t EE:1;                       /*!< bit:    25  Exception Endianness */
    uint32_t _reserved6:1;               /*!< bit:    26  Reserved */
    uint32_t NMFI:1;                     /*!< bit:    27  Non-maskable FIQ (NMFI) support */
    uint32_t TRE:1;                      /*!< bit:    28  TEX remap enable. */
    uint32_t AFE:1;                      /*!< bit:    29  Access flag enable */
    uint32_t TE:1;                       /*!< bit:    30  Thumb Exception enable */
    uint32_t _reserved7:1;               /*!< bit:    31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} SCTLR_Type;

/* CP15 寄存器SCTLR各个位定义 */
#define SCTLR_TE_Pos                     30U                                    /*!< SCTLR: TE Position */
#define SCTLR_TE_Msk                     (1UL << SCTLR_TE_Pos)                  /*!< SCTLR: TE Mask */

#define SCTLR_AFE_Pos                    29U                                    /*!< SCTLR: AFE Position */
#define SCTLR_AFE_Msk                    (1UL << SCTLR_AFE_Pos)                 /*!< SCTLR: AFE Mask */

#define SCTLR_TRE_Pos                    28U                                    /*!< SCTLR: TRE Position */
#define SCTLR_TRE_Msk                    (1UL << SCTLR_TRE_Pos)                 /*!< SCTLR: TRE Mask */

#define SCTLR_NMFI_Pos                   27U                                    /*!< SCTLR: NMFI Position */
#define SCTLR_NMFI_Msk                   (1UL << SCTLR_NMFI_Pos)                /*!< SCTLR: NMFI Mask */

#define SCTLR_EE_Pos                     25U                                    /*!< SCTLR: EE Position */
#define SCTLR_EE_Msk                     (1UL << SCTLR_EE_Pos)                  /*!< SCTLR: EE Mask */

#define SCTLR_VE_Pos                     24U                                    /*!< SCTLR: VE Position */
#define SCTLR_VE_Msk                     (1UL << SCTLR_VE_Pos)                  /*!< SCTLR: VE Mask */

#define SCTLR_U_Pos                      22U                                    /*!< SCTLR: U Position */
#define SCTLR_U_Msk                      (1UL << SCTLR_U_Pos)                   /*!< SCTLR: U Mask */

#define SCTLR_FI_Pos                     21U                                    /*!< SCTLR: FI Position */
#define SCTLR_FI_Msk                     (1UL << SCTLR_FI_Pos)                  /*!< SCTLR: FI Mask */

#define SCTLR_UWXN_Pos                   20U                                    /*!< SCTLR: UWXN Position */
#define SCTLR_UWXN_Msk                   (1UL << SCTLR_UWXN_Pos)                /*!< SCTLR: UWXN Mask */

#define SCTLR_WXN_Pos                    19U                                    /*!< SCTLR: WXN Position */
#define SCTLR_WXN_Msk                    (1UL << SCTLR_WXN_Pos)                 /*!< SCTLR: WXN Mask */

#define SCTLR_HA_Pos                     17U                                    /*!< SCTLR: HA Position */
#define SCTLR_HA_Msk                     (1UL << SCTLR_HA_Pos)                  /*!< SCTLR: HA Mask */

#define SCTLR_RR_Pos                     14U                                    /*!< SCTLR: RR Position */
#define SCTLR_RR_Msk                     (1UL << SCTLR_RR_Pos)                  /*!< SCTLR: RR Mask */

#define SCTLR_V_Pos                      13U                                    /*!< SCTLR: V Position */
#define SCTLR_V_Msk                      (1UL << SCTLR_V_Pos)                   /*!< SCTLR: V Mask */

#define SCTLR_I_Pos                      12U                                    /*!< SCTLR: I Position */
#define SCTLR_I_Msk                      (1UL << SCTLR_I_Pos)                   /*!< SCTLR: I Mask */

#define SCTLR_Z_Pos                      11U                                    /*!< SCTLR: Z Position */
#define SCTLR_Z_Msk                      (1UL << SCTLR_Z_Pos)                   /*!< SCTLR: Z Mask */

#define SCTLR_SW_Pos                     10U                                    /*!< SCTLR: SW Position */
#define SCTLR_SW_Msk                     (1UL << SCTLR_SW_Pos)                  /*!< SCTLR: SW Mask */

#define SCTLR_B_Pos                      7U                                     /*!< SCTLR: B Position */
#define SCTLR_B_Msk                      (1UL << SCTLR_B_Pos)                   /*!< SCTLR: B Mask */

#define SCTLR_CP15BEN_Pos                5U                                     /*!< SCTLR: CP15BEN Position */
#define SCTLR_CP15BEN_Msk                (1UL << SCTLR_CP15BEN_Pos)             /*!< SCTLR: CP15BEN Mask */

#define SCTLR_C_Pos                      2U                                     /*!< SCTLR: C Position */
#define SCTLR_C_Msk                      (1UL << SCTLR_C_Pos)                   /*!< SCTLR: C Mask */

#define SCTLR_A_Pos                      1U                                     /*!< SCTLR: A Position */
#define SCTLR_A_Msk                      (1UL << SCTLR_A_Pos)                   /*!< SCTLR: A Mask */

#define SCTLR_M_Pos                      0U                                     /*!< SCTLR: M Position */
#define SCTLR_M_Msk                      (1UL << SCTLR_M_Pos)                   /*!< SCTLR: M Mask */

/* CP15的ACTLR寄存器
 * 参考资料:Cortex-A7 Technical ReferenceManua.pdf P113
 */
typedef union
{
  struct
  {
    uint32_t _reserved0:6;               /*!< bit: 0.. 5  Reserved */
    uint32_t SMP:1;                      /*!< bit:     6  Enables coherent requests to the processor */
    uint32_t _reserved1:3;               /*!< bit: 7.. 9  Reserved */
    uint32_t DODMBS:1;                   /*!< bit:    10  Disable optimized data memory barrier behavior */
    uint32_t L2RADIS:1;                  /*!< bit:    11  L2 Data Cache read-allocate mode disable */
    uint32_t L1RADIS:1;                  /*!< bit:    12  L1 Data Cache read-allocate mode disable */
    uint32_t L1PCTL:2;                   /*!< bit:13..14  L1 Data prefetch control */
    uint32_t DDVM:1;                     /*!< bit:    15  Disable Distributed Virtual Memory (DVM) transactions */
    uint32_t _reserved3:12;              /*!< bit:16..27  Reserved */
    uint32_t DDI:1;                      /*!< bit:    28  Disable dual issue */
    uint32_t _reserved7:3;               /*!< bit:29..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} ACTLR_Type;

#define ACTLR_DDI_Pos                    28U                                     /*!< ACTLR: DDI Position */
#define ACTLR_DDI_Msk                    (1UL << ACTLR_DDI_Pos)                  /*!< ACTLR: DDI Mask */

#define ACTLR_DDVM_Pos                   15U                                     /*!< ACTLR: DDVM Position */
#define ACTLR_DDVM_Msk                   (1UL << ACTLR_DDVM_Pos)                 /*!< ACTLR: DDVM Mask */

#define ACTLR_L1PCTL_Pos                 13U                                     /*!< ACTLR: L1PCTL Position */
#define ACTLR_L1PCTL_Msk                 (3UL << ACTLR_L1PCTL_Pos)               /*!< ACTLR: L1PCTL Mask */

#define ACTLR_L1RADIS_Pos                12U                                     /*!< ACTLR: L1RADIS Position */
#define ACTLR_L1RADIS_Msk                (1UL << ACTLR_L1RADIS_Pos)              /*!< ACTLR: L1RADIS Mask */

#define ACTLR_L2RADIS_Pos                11U                                     /*!< ACTLR: L2RADIS Position */
#define ACTLR_L2RADIS_Msk                (1UL << ACTLR_L2RADIS_Pos)              /*!< ACTLR: L2RADIS Mask */

#define ACTLR_DODMBS_Pos                 10U                                     /*!< ACTLR: DODMBS Position */
#define ACTLR_DODMBS_Msk                 (1UL << ACTLR_DODMBS_Pos)               /*!< ACTLR: DODMBS Mask */

#define ACTLR_SMP_Pos                    6U                                      /*!< ACTLR: SMP Position */
#define ACTLR_SMP_Msk                    (1UL << ACTLR_SMP_Pos)                  /*!< ACTLR: SMP Mask */


/* CP15的CPACR寄存器
 * 参考资料：Cortex-A7 Technical ReferenceManua.pdf P115
 */
typedef union
{
  struct
  {
    uint32_t _reserved0:20;              /*!< bit: 0..19  Reserved */
    uint32_t cp10:2;                     /*!< bit:20..21  Access rights for coprocessor 10 */
    uint32_t cp11:2;                     /*!< bit:22..23  Access rights for coprocessor 11 */
    uint32_t _reserved1:6;               /*!< bit:24..29  Reserved */
    uint32_t D32DIS:1;                   /*!< bit:    30  Disable use of registers D16-D31 of the VFP register file */
    uint32_t ASEDIS:1;                   /*!< bit:    31  Disable Advanced SIMD Functionality */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} CPACR_Type;

#define CPACR_ASEDIS_Pos                 31U                                    /*!< CPACR: ASEDIS Position */
#define CPACR_ASEDIS_Msk                 (1UL << CPACR_ASEDIS_Pos)              /*!< CPACR: ASEDIS Mask */

#define CPACR_D32DIS_Pos                 30U                                    /*!< CPACR: D32DIS Position */
#define CPACR_D32DIS_Msk                 (1UL << CPACR_D32DIS_Pos)              /*!< CPACR: D32DIS Mask */

#define CPACR_cp11_Pos                   22U                                    /*!< CPACR: cp11 Position */
#define CPACR_cp11_Msk                   (3UL << CPACR_cp11_Pos)                /*!< CPACR: cp11 Mask */

#define CPACR_cp10_Pos                   20U                                    /*!< CPACR: cp10 Position */
#define CPACR_cp10_Msk                   (3UL << CPACR_cp10_Pos)                /*!< CPACR: cp10 Mask */


/* CP15的DFSR寄存器
 * 参考资料：Cortex-A7 Technical ReferenceManua.pdf P128
 */
typedef union
{
  struct
  {
    uint32_t FS0:4;                      /*!< bit: 0.. 3  Fault Status bits bit 0-3 */
    uint32_t Domain:4;                   /*!< bit: 4.. 7  Fault on which domain */
    uint32_t _reserved0:2;               /*!< bit: 8.. 9  Reserved */
    uint32_t FS1:1;                      /*!< bit:    10  Fault Status bits bit 4 */
    uint32_t WnR:1;                      /*!< bit:    11  Write not Read bit */
    uint32_t ExT:1;                      /*!< bit:    12  External abort type */
    uint32_t CM:1;                       /*!< bit:    13  Cache maintenance fault */
    uint32_t _reserved1:18;              /*!< bit:14..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} DFSR_Type;

#define DFSR_CM_Pos                      13U                                    /*!< DFSR: CM Position */
#define DFSR_CM_Msk                      (1UL << DFSR_CM_Pos)                   /*!< DFSR: CM Mask */

#define DFSR_Ext_Pos                     12U                                    /*!< DFSR: Ext Position */
#define DFSR_Ext_Msk                     (1UL << DFSR_Ext_Pos)                  /*!< DFSR: Ext Mask */

#define DFSR_WnR_Pos                     11U                                    /*!< DFSR: WnR Position */
#define DFSR_WnR_Msk                     (1UL << DFSR_WnR_Pos)                  /*!< DFSR: WnR Mask */

#define DFSR_FS1_Pos                     10U                                    /*!< DFSR: FS1 Position */
#define DFSR_FS1_Msk                     (1UL << DFSR_FS1_Pos)                  /*!< DFSR: FS1 Mask */

#define DFSR_Domain_Pos                  4U                                     /*!< DFSR: Domain Position */
#define DFSR_Domain_Msk                  (0xFUL << DFSR_Domain_Pos)             /*!< DFSR: Domain Mask */

#define DFSR_FS0_Pos                     0U                                     /*!< DFSR: FS0 Position */
#define DFSR_FS0_Msk                     (0xFUL << DFSR_FS0_Pos)                /*!< DFSR: FS0 Mask */


/* CP15的IFSR寄存器 
 * 参考资料：Cortex-A7 Technical ReferenceManua.pdf P131
 */
typedef union
{
  struct
  {
    uint32_t FS0:4;                      /*!< bit: 0.. 3  Fault Status bits bit 0-3 */
    uint32_t _reserved0:6;               /*!< bit: 4.. 9  Reserved */
    uint32_t FS1:1;                      /*!< bit:    10  Fault Status bits bit 4 */
    uint32_t _reserved1:1;               /*!< bit:    11  Reserved */
    uint32_t ExT:1;                      /*!< bit:    12  External abort type */
    uint32_t _reserved2:19;              /*!< bit:13..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} IFSR_Type;

#define IFSR_ExT_Pos                     12U                                    /*!< IFSR: ExT Position */
#define IFSR_ExT_Msk                     (1UL << IFSR_ExT_Pos)                  /*!< IFSR: ExT Mask */

#define IFSR_FS1_Pos                     10U                                    /*!< IFSR: FS1 Position */
#define IFSR_FS1_Msk                     (1UL << IFSR_FS1_Pos)                  /*!< IFSR: FS1 Mask */

#define IFSR_FS0_Pos                     0U                                     /*!< IFSR: FS0 Position */
#define IFSR_FS0_Msk                     (0xFUL << IFSR_FS0_Pos)                /*!< IFSR: FS0 Mask */


/* CP15的ISR寄存器
 * 参考资料：ARM ArchitectureReference Manual ARMv7-A and ARMv7-R edition.pdf P1640
 */
typedef union
{
  struct
  {
    uint32_t _reserved0:6;               /*!< bit: 0.. 5  Reserved */
    uint32_t F:1;                        /*!< bit:     6  FIQ pending bit */
    uint32_t I:1;                        /*!< bit:     7  IRQ pending bit */
    uint32_t A:1;                        /*!< bit:     8  External abort pending bit */
    uint32_t _reserved1:23;              /*!< bit:14..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32_t w;                            /*!< Type      used for word access */
} ISR_Type;

#define ISR_A_Pos                        13U                                    /*!< ISR: A Position */
#define ISR_A_Msk                        (1UL << ISR_A_Pos)                     /*!< ISR: A Mask */

#define ISR_I_Pos                        12U                                    /*!< ISR: I Position */
#define ISR_I_Msk                        (1UL << ISR_I_Pos)                     /*!< ISR: I Mask */

#define ISR_F_Pos                        11U                                    /*!< ISR: F Position */
#define ISR_F_Msk                        (1UL << ISR_F_Pos)                     /*!< ISR: F Mask */


/* Mask and shift a bit field value for use in a register bit range. */
#define _VAL2FLD(field, value)    ((value << field ## _Pos) & field ## _Msk)

/* Mask and shift a register value to extract a bit filed value. */
#define _FLD2VAL(field, value)    ((value & field ## _Msk) >> field ## _Pos)


/*******************************************************************************
 *       			CP15 访问函数
 ******************************************************************************/

FORCEDINLINE __STATIC_INLINE uint32_t __get_SCTLR(void)
{
  return __MRC(15, 0, 1, 0, 0);
}

FORCEDINLINE __STATIC_INLINE void __set_SCTLR(uint32_t sctlr)
{
  __MCR(15, 0, sctlr, 1, 0, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_ACTLR(void)
{
  return __MRC(15, 0, 1, 0, 1);
}

FORCEDINLINE __STATIC_INLINE void __set_ACTLR(uint32_t actlr)
{
  __MCR(15, 0, actlr, 1, 0, 1);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_CPACR(void)
{
  return __MRC(15, 0, 1, 0, 2);
}

FORCEDINLINE __STATIC_INLINE void __set_CPACR(uint32_t cpacr)
{
  __MCR(15, 0, cpacr, 1, 0, 2);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_TTBR0(void)
{
  return __MRC(15, 0, 2, 0, 0);
}

FORCEDINLINE __STATIC_INLINE void __set_TTBR0(uint32_t ttbr0)
{
  __MCR(15, 0, ttbr0, 2, 0, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_TTBR1(void)
{
  return __MRC(15, 0, 2, 0, 1);
}

FORCEDINLINE __STATIC_INLINE void __set_TTBR1(uint32_t ttbr1)
{
  __MCR(15, 0, ttbr1, 2, 0, 1);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_TTBCR(void)
{
  return __MRC(15, 0, 2, 0, 2);
}

FORCEDINLINE __STATIC_INLINE void __set_TTBCR(uint32_t ttbcr)
{
  __MCR(15, 0, ttbcr, 2, 0, 2);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_DACR(void)
{
  return __MRC(15, 0, 3, 0, 0);
}

FORCEDINLINE __STATIC_INLINE void __set_DACR(uint32_t dacr)
{
  __MCR(15, 0, dacr, 3, 0, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_DFSR(void)
{
  return __MRC(15, 0, 5, 0, 0);
}

FORCEDINLINE __STATIC_INLINE void __set_DFSR(uint32_t dfsr)
{
  __MCR(15, 0, dfsr, 5, 0, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_IFSR(void)
{
  return __MRC(15, 0, 5, 0, 1);
}

FORCEDINLINE __STATIC_INLINE void __set_IFSR(uint32_t ifsr)
{
  __MCR(15, 0, ifsr, 5, 0, 1);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_DFAR(void)
{
  return __MRC(15, 0, 6, 0, 0);
}

FORCEDINLINE __STATIC_INLINE void __set_DFAR(uint32_t dfar)
{
  __MCR(15, 0, dfar, 6, 0, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_IFAR(void)
{
  return __MRC(15, 0, 6, 0, 2);
}

FORCEDINLINE __STATIC_INLINE void __set_IFAR(uint32_t ifar)
{
  __MCR(15, 0, ifar, 6, 0, 2);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_VBAR(void)
{
  return __MRC(15, 0, 12, 0, 0);
}

FORCEDINLINE __STATIC_INLINE void __set_VBAR(uint32_t vbar)
{
  __MCR(15, 0, vbar, 12, 0, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_ISR(void)
{
  return __MRC(15, 0, 12, 1, 0);
}

FORCEDINLINE __STATIC_INLINE void __set_ISR(uint32_t isr)
{
  __MCR(15, 0, isr, 12, 1, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_CONTEXTIDR(void)
{
  return __MRC(15, 0, 13, 0, 1);
}

FORCEDINLINE __STATIC_INLINE void __set_CONTEXTIDR(uint32_t contextidr)
{
  __MCR(15, 0, contextidr, 13, 0, 1);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_CBAR(void)
{
  return __MRC(15, 4, 15, 0, 0);
}

/*******************************************************************************
 *                 GIC相关内容
 *有关GIC的内容，参考：ARM Generic Interrupt Controller(ARM GIC控制器)V2.0.pdf
 ******************************************************************************/

/*
 * GIC寄存器描述结构体，
 * GIC分为分发器端和CPU接口端
 */
typedef struct
{
        uint32_t RESERVED0[1024];
  __IOM uint32_t D_CTLR;                 /*!< Offset: 0x1000 (R/W) Distributor Control Register */
  __IM  uint32_t D_TYPER;                /*!< Offset: 0x1004 (R/ )  Interrupt Controller Type Register */
  __IM  uint32_t D_IIDR;                 /*!< Offset: 0x1008 (R/ )  Distributor Implementer Identification Register */
        uint32_t RESERVED1[29];
  __IOM uint32_t D_IGROUPR[16];          /*!< Offset: 0x1080 - 0x0BC (R/W) Interrupt Group Registers */
        uint32_t RESERVED2[16];
  __IOM uint32_t D_ISENABLER[16];        /*!< Offset: 0x1100 - 0x13C (R/W) Interrupt Set-Enable Registers */
        uint32_t RESERVED3[16];
  __IOM uint32_t D_ICENABLER[16];        /*!< Offset: 0x1180 - 0x1BC (R/W) Interrupt Clear-Enable Registers */
        uint32_t RESERVED4[16];
  __IOM uint32_t D_ISPENDR[16];          /*!< Offset: 0x1200 - 0x23C (R/W) Interrupt Set-Pending Registers */
        uint32_t RESERVED5[16];
  __IOM uint32_t D_ICPENDR[16];          /*!< Offset: 0x1280 - 0x2BC (R/W) Interrupt Clear-Pending Registers */
        uint32_t RESERVED6[16];
  __IOM uint32_t D_ISACTIVER[16];        /*!< Offset: 0x1300 - 0x33C (R/W) Interrupt Set-Active Registers */
        uint32_t RESERVED7[16];
  __IOM uint32_t D_ICACTIVER[16];        /*!< Offset: 0x1380 - 0x3BC (R/W) Interrupt Clear-Active Registers */
        uint32_t RESERVED8[16];
  __IOM uint8_t  D_IPRIORITYR[512];      /*!< Offset: 0x1400 - 0x5FC (R/W) Interrupt Priority Registers */
        uint32_t RESERVED9[128];
  __IOM uint8_t  D_ITARGETSR[512];       /*!< Offset: 0x1800 - 0x9FC (R/W) Interrupt Targets Registers */
        uint32_t RESERVED10[128];
  __IOM uint32_t D_ICFGR[32];            /*!< Offset: 0x1C00 - 0xC7C (R/W) Interrupt configuration registers */
        uint32_t RESERVED11[32];
  __IM  uint32_t D_PPISR;                /*!< Offset: 0x1D00 (R/ ) Private Peripheral Interrupt Status Register */
  __IM  uint32_t D_SPISR[15];            /*!< Offset: 0x1D04 - 0xD3C (R/ ) Shared Peripheral Interrupt Status Registers */
        uint32_t RESERVED12[112];
  __OM  uint32_t D_SGIR;                 /*!< Offset: 0x1F00 ( /W) Software Generated Interrupt Register */
        uint32_t RESERVED13[3];
  __IOM uint8_t  D_CPENDSGIR[16];        /*!< Offset: 0x1F10 - 0xF1C (R/W) SGI Clear-Pending Registers */
  __IOM uint8_t  D_SPENDSGIR[16];        /*!< Offset: 0x1F20 - 0xF2C (R/W) SGI Set-Pending Registers */
        uint32_t RESERVED14[40];
  __IM  uint32_t D_PIDR4;                /*!< Offset: 0x1FD0 (R/ ) Peripheral ID4 Register */
  __IM  uint32_t D_PIDR5;                /*!< Offset: 0x1FD4 (R/ ) Peripheral ID5 Register */
  __IM  uint32_t D_PIDR6;                /*!< Offset: 0x1FD8 (R/ ) Peripheral ID6 Register */
  __IM  uint32_t D_PIDR7;                /*!< Offset: 0x1FDC (R/ ) Peripheral ID7 Register */
  __IM  uint32_t D_PIDR0;                /*!< Offset: 0x1FE0 (R/ ) Peripheral ID0 Register */
  __IM  uint32_t D_PIDR1;                /*!< Offset: 0x1FE4 (R/ ) Peripheral ID1 Register */
  __IM  uint32_t D_PIDR2;                /*!< Offset: 0x1FE8 (R/ ) Peripheral ID2 Register */
  __IM  uint32_t D_PIDR3;                /*!< Offset: 0x1FEC (R/ ) Peripheral ID3 Register */
  __IM  uint32_t D_CIDR0;                /*!< Offset: 0x1FF0 (R/ ) Component ID0 Register */
  __IM  uint32_t D_CIDR1;                /*!< Offset: 0x1FF4 (R/ ) Component ID1 Register */
  __IM  uint32_t D_CIDR2;                /*!< Offset: 0x1FF8 (R/ ) Component ID2 Register */
  __IM  uint32_t D_CIDR3;                /*!< Offset: 0x1FFC (R/ ) Component ID3 Register */

  __IOM uint32_t C_CTLR;                 /*!< Offset: 0x2000 (R/W) CPU Interface Control Register */
  __IOM uint32_t C_PMR;                  /*!< Offset: 0x2004 (R/W) Interrupt Priority Mask Register */
  __IOM uint32_t C_BPR;                  /*!< Offset: 0x2008 (R/W) Binary Point Register */
  __IM  uint32_t C_IAR;                  /*!< Offset: 0x200C (R/ ) Interrupt Acknowledge Register */
  __OM  uint32_t C_EOIR;                 /*!< Offset: 0x2010 ( /W) End Of Interrupt Register */
  __IM  uint32_t C_RPR;                  /*!< Offset: 0x2014 (R/ ) Running Priority Register */
  __IM  uint32_t C_HPPIR;                /*!< Offset: 0x2018 (R/ ) Highest Priority Pending Interrupt Register */
  __IOM uint32_t C_ABPR;                 /*!< Offset: 0x201C (R/W) Aliased Binary Point Register */
  __IM  uint32_t C_AIAR;                 /*!< Offset: 0x2020 (R/ ) Aliased Interrupt Acknowledge Register */
  __OM  uint32_t C_AEOIR;                /*!< Offset: 0x2024 ( /W) Aliased End Of Interrupt Register */
  __IM  uint32_t C_AHPPIR;               /*!< Offset: 0x2028 (R/ ) Aliased Highest Priority Pending Interrupt Register */
        uint32_t RESERVED15[41];
  __IOM uint32_t C_APR0;                 /*!< Offset: 0x20D0 (R/W) Active Priority Register */
        uint32_t RESERVED16[3];
  __IOM uint32_t C_NSAPR0;               /*!< Offset: 0x20E0 (R/W) Non-secure Active Priority Register */
        uint32_t RESERVED17[6];
  __IM  uint32_t C_IIDR;                 /*!< Offset: 0x20FC (R/ ) CPU Interface Identification Register */
        uint32_t RESERVED18[960];
  __OM  uint32_t C_DIR;                  /*!< Offset: 0x3000 ( /W) Deactivate Interrupt Register */
} GIC_Type;


/* 
 * GIC初始化
 * 为了简单使用GIC的group0
 */
FORCEDINLINE __STATIC_INLINE void GIC_Init(void)
{
  uint32_t i;
  uint32_t irqRegs;
  GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);

  irqRegs = (gic->D_TYPER & 0x1FUL) + 1;

  /* On POR, all SPI is in group 0, level-sensitive and using 1-N model */

  /* Disable all PPI, SGI and SPI */
  for (i = 0; i < irqRegs; i++)
    gic->D_ICENABLER[i] = 0xFFFFFFFFUL;

  /* Make all interrupts have higher priority */
  gic->C_PMR = (0xFFUL << (8 - __GIC_PRIO_BITS)) & 0xFFUL;

  /* No subpriority, all priority level allows preemption */
  gic->C_BPR = 7 - __GIC_PRIO_BITS;

  /* Enable group0 distribution */
  gic->D_CTLR = 1UL;

  /* Enable group0 signaling */
  gic->C_CTLR = 1UL;
}

/*  
 * 使能指定的中断
 */
FORCEDINLINE __STATIC_INLINE void GIC_EnableIRQ(IRQn_Type IRQn)
{
  	GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);
  	gic->D_ISENABLER[((uint32_t)(int32_t)IRQn) >> 5] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

/*  
 * 关闭指定的中断
 */

FORCEDINLINE __STATIC_INLINE void GIC_DisableIRQ(IRQn_Type IRQn)
{
  	GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);
  	gic->D_ICENABLER[((uint32_t)(int32_t)IRQn) >> 5] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

/* 
 * 返回中断号 
 */
FORCEDINLINE __STATIC_INLINE uint32_t GIC_AcknowledgeIRQ(void)
{
  	GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);
  	return gic->C_IAR & 0x1FFFUL;
}

/* 
 * 向EOIR写入发送中断的中断号来释放中断
 */
FORCEDINLINE __STATIC_INLINE void GIC_DeactivateIRQ(uint32_t value)
{
  	GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);
  	gic->C_EOIR = value;
}

/*
 * 获取运行优先级
 */
FORCEDINLINE __STATIC_INLINE uint32_t GIC_GetRunningPriority(void)
{
  	GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);
  	return gic->C_RPR & 0xFFUL;
}

/*
 * 设置组优先级
 */
FORCEDINLINE __STATIC_INLINE void GIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  	GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);
  	gic->C_BPR = PriorityGroup & 0x7UL;
}

/*
 * 获取组优先级
 */
FORCEDINLINE __STATIC_INLINE uint32_t GIC_GetPriorityGrouping(void)
{
  GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);

  return gic->C_BPR & 0x7UL;
}

/*
 * 设置优先级
 */
FORCEDINLINE __STATIC_INLINE void GIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  	GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);
  	gic->D_IPRIORITYR[((uint32_t)(int32_t)IRQn)] = (uint8_t)((priority << (8UL - __GIC_PRIO_BITS)) & (uint32_t)0xFFUL);
}

/*
 * 获取优先级
 */
FORCEDINLINE __STATIC_INLINE uint32_t GIC_GetPriority(IRQn_Type IRQn)
{
  GIC_Type *gic = (GIC_Type *)(__get_CBAR() & 0xFFFF0000UL);
  return(((uint32_t)gic->D_IPRIORITYR[((uint32_t)(int32_t)IRQn)] >> (8UL - __GIC_PRIO_BITS)));
}


#endif 
