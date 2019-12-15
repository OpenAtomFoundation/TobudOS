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

#if defined (TOS_CFG_CPU_ARM_FPU_EN) && (TOS_CFG_CPU_ARM_FPU_EN == 1U)
typedef struct fault_fpu_frame_st {
    cpu_data_t s0;
    cpu_data_t s1;
    cpu_data_t s2;
    cpu_data_t s3;
    cpu_data_t s4;
    cpu_data_t s5;
    cpu_data_t s6;
    cpu_data_t s7;
    cpu_data_t s8;
    cpu_data_t s9;
    cpu_data_t s10;
    cpu_data_t s11;
    cpu_data_t s12;
    cpu_data_t s13;
    cpu_data_t s14;
    cpu_data_t s15;
    cpu_data_t fpscr;
} fault_fpu_frame_t;
#endif

typedef struct fault_cpu_frame_st {
    cpu_data_t r0;
    cpu_data_t r1;
    cpu_data_t r2;
    cpu_data_t r3;
    cpu_data_t r12;
    cpu_data_t lr;
    cpu_data_t pc;
    cpu_data_t spsr;
} fault_cpu_frame_t;

typedef struct fault_exc_frame_st {
    fault_cpu_frame_t cpu_frame;

#if defined (TOS_CFG_CPU_ARM_FPU_EN) && (TOS_CFG_CPU_ARM_FPU_EN == 1U)
    fault_fpu_frame_t fpu_frame;
#endif
} fault_exc_frame_t;

/**
 * information we need to do fault backtrace
 */
typedef struct fault_information_st {
    int is_thumb : 1;           /**< whether it is thumb we use when we fall into fault? */
    int is_on_task : 1;         /**< whether we are on a task when fall into fault? */
    int is_stk_ovrf : 1;        /**< whether we get a stack overflow */

#if defined (TOS_CFG_CPU_ARM_FPU_EN) && (TOS_CFG_CPU_ARM_FPU_EN == 1U)
    int is_ext_stk_frm : 1;     /**< whether it is a extended stack frame?(whether the cpu has pushed fpu registers onto the stack) */
#endif

    cpu_addr_t pc;              /**< just where fault happens */

    cpu_addr_t sp_before_fault; /**< original sp just before the cpu push the fault exception frame */

    /**
     * we need main_stack_start & main_stack_limit to do call stack backtrace
     * when we fall into fault during a task, we should do the call stack backtrace on the task's stack
     * but if not, which means we are in kernel, we should do the call stack backtrace on the main stack
     * in arm v7-m, this should be the MSP's start and limit
     * in arm v7-a, call stack backtrace is another story(much more elegant because we have FP).
     */
    cpu_addr_t stack_start;     /**< current sp start address we use. if on task, it'll be the task's stack, otherwise it'll be the msp */
    cpu_addr_t stack_limit;     /**< current sp limit address */
    cpu_addr_t code_start;      /**< current code start address */
    cpu_addr_t code_limit;      /**< current code limit address */
} fault_info_t;

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

#define DEFAULT_CODE_SECTION_NAME       ER_IROM1
#define DEFAULT_CSTACK_SECTION_NAME     STACK

#define SECTION_START(_name_)                _name_##$$Base
#define SECTION_END(_name_)                  _name_##$$Limit
#define IMAGE_SECTION_START(_name_)          Image$$##_name_##$$Base
#define IMAGE_SECTION_END(_name_)            Image$$##_name_##$$Limit
#define CSTACK_BLOCK_START(_name_)           SECTION_START(_name_)
#define CSTACK_BLOCK_END(_name_)             SECTION_END(_name_)
#define CODE_SECTION_START(_name_)           IMAGE_SECTION_START(_name_)
#define CODE_SECTION_END(_name_)             IMAGE_SECTION_END(_name_)

extern const int CSTACK_BLOCK_START(DEFAULT_CSTACK_SECTION_NAME);
extern const int CSTACK_BLOCK_END(DEFAULT_CSTACK_SECTION_NAME);
extern const int CODE_SECTION_START(DEFAULT_CODE_SECTION_NAME);
extern const int CODE_SECTION_END(DEFAULT_CODE_SECTION_NAME);

__STATIC_INLINE__ cpu_addr_t fault_code_start(void)
{
    return (cpu_addr_t)&CODE_SECTION_START(DEFAULT_CODE_SECTION_NAME);
}

__STATIC_INLINE__ cpu_addr_t fault_code_limit(void)
{
    return (cpu_addr_t)&CODE_SECTION_END(DEFAULT_CODE_SECTION_NAME);
}

__STATIC_INLINE__ cpu_addr_t fault_msp_start(void)
{
    return (cpu_addr_t)&CSTACK_BLOCK_START(DEFAULT_CSTACK_SECTION_NAME);
}

__STATIC_INLINE__ cpu_addr_t fault_msp_limit(void)
{
    return (cpu_addr_t)&CSTACK_BLOCK_END(DEFAULT_CSTACK_SECTION_NAME);
}

#elif defined(__ICCARM__)

#define DEFAULT_CODE_SECTION_NAME       ".text"
#define DEFAULT_CSTACK_SECTION_NAME     "CSTACK"

#pragma section=DEFAULT_CSTACK_SECTION_NAME
#pragma section=DEFAULT_CODE_SECTION_NAME

__STATIC_INLINE__ cpu_addr_t fault_code_start(void)
{
    return (cpu_addr_t)__section_begin(DEFAULT_CODE_SECTION_NAME);
}

__STATIC_INLINE__ cpu_addr_t fault_code_limit(void)
{
    return (cpu_addr_t)__section_end(DEFAULT_CODE_SECTION_NAME);
}

__STATIC_INLINE__ cpu_addr_t fault_msp_start(void)
{
    return (cpu_addr_t)__section_begin(DEFAULT_CSTACK_SECTION_NAME);
}

__STATIC_INLINE__ cpu_addr_t fault_msp_limit(void)
{
    return (cpu_addr_t)__section_end(DEFAULT_CSTACK_SECTION_NAME);
}

#elif defined(__GNUC__)

/**
 * if we are using keil(armcc) or mdk(iccarm), we probably use the defult link script supplied by the IDE.
 * the way to locate the text/stack section start and limit is to find them in default link script.
 * but if we build our project by makefile(or something like scons, cmake, etc), we probably need to write
 * our own link scrpit, if so, we should do like this(just a demo):
 *
    _stext = .;
    .text : {
        *(.text.startup)
        *(.text)
        *(.text.*)
    }
    _etext = .;

     __bss_start = .;
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
        _sstack = .;
        *(.cstack)
        _estack = .;
    }
    __bss_end = .;
 * by this, we can locate text/stack section start and limit by _stext/_etext and _sstack/_estack
 */
#define DEFAULT_CODE_SECTION_START      _stext
#define DEFAULT_CODE_SECTION_END        _etext
#define DEFAULT_CSTACK_SECTION_START    _sstack
#define DEFAULT_CSTACK_SECTION_END      _estack

extern const int DEFAULT_CODE_SECTION_START;
extern const int DEFAULT_CODE_SECTION_END;

extern const int DEFAULT_CSTACK_SECTION_START;
extern const int DEFAULT_CSTACK_SECTION_END;

__STATIC_INLINE__ cpu_addr_t fault_code_start(void)
{
    return (cpu_addr_t)(&(DEFAULT_CODE_SECTION_START));
}

__STATIC_INLINE__ cpu_addr_t fault_code_limit(void)
{
    return (cpu_addr_t)(&(DEFAULT_CODE_SECTION_END));
}

__STATIC_INLINE__ cpu_addr_t fault_msp_start(void)
{
    return (cpu_addr_t)(&(DEFAULT_CSTACK_SECTION_START));
}

__STATIC_INLINE__ cpu_addr_t fault_msp_limit(void)
{
    return (cpu_addr_t)(&(DEFAULT_CSTACK_SECTION_END));
}

#endif

__API__ void tos_fault_log_writer_set(k_fault_log_writer_t log_writer);

__KERNEL__ int fault_default_log_writer(const char *format, ...);

__KERNEL__ void fault_backtrace(cpu_addr_t lr, fault_exc_frame_t *frame);

#endif

#endif /* _TOS_FAULT_H_ */

