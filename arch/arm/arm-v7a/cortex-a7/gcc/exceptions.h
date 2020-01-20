/* macros to align and unalign the stack on 8 byte boundary for ABI compliance */
.macro stack_align, tempreg
    /* make sure the stack is aligned */
    mov     \tempreg, sp
    tst     sp, #4
    subeq   sp, #4
    push    { \tempreg }

    /* tempreg holds the original stack */
.endm

.macro stack_restore, tempreg
    /* restore the potentially unaligned stack */
    pop     { \tempreg }
    mov     sp, \tempreg
.endm

/* save and disable the vfp unit */
.macro vfp_save, temp1
    /* save old fpexc */
    vmrs    \temp1, fpexc

    push    { \temp1 }

    /* hard disable the vfp unit */
    bic     \temp1, #(1<<30)
    vmsr    fpexc, \temp1
.endm

/* restore the vfp enable/disable state */
.macro vfp_restore, temp1
    /* restore fpexc */
    pop     { \temp1 }

    vmsr    fpexc, \temp1
.endm

/* Save callee trashed registers.
 * At exit r0 contains a pointer to the register frame.
 */
.macro save
    /* save spsr and r14 onto the svc stack */
    srsdb   #0x13!

    /* switch to svc mode, interrupts disabled */
    cpsid   i, #0x13

    /* save callee trashed regs and lr */
    push    { r0-r3, r12, lr }

#if 0
    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* save and disable the vfp unit */
    vfp_save    r0
    #endif
#endif

    /* make sure the stack is 8 byte aligned */
    stack_align r0

    /* r0 now holds the pointer to the original iframe (before alignment) */
.endm

.macro save_offset, offset
    sub     lr, \offset
    save
.endm

.macro restore
    /* undo the stack alignment we did before */
    stack_restore r0

#if 0
    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* restore the old state of the vfp unit */
    vfp_restore r0
    #endif
#endif

    pop     { r0-r3, r12, lr }

    /* return to whence we came from */
    rfeia   sp!
.endm

/* Save all registers.
 * At exit r0 contains a pointer to the register frame.
 */
.macro saveall
    /* save spsr and r14 onto the svc stack */
    srsdb   #0x13!

    /* switch to svc mode, interrupts disabled */
    cpsid   i,#0x13

    /* save all regs */
    push    { r0-r12, lr }

    #if 0
    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* save and disable the vfp unit */
    vfp_save    r0
    #endif
    #endif

    /* make sure the stack is 8 byte aligned */
    stack_align r0

    /* r0 now holds the pointer to the original iframe (before alignment) */
.endm

.macro saveall_offset, offset
    sub     lr, \offset
    saveall
.endm

.macro restoreall
    /* undo the stack alignment we did before */
    stack_restore r0

    #if 0
    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* restore the old state of the vfp unit */
    vfp_restore r0
    #endif
    #endif

    pop     { r0-r12, r14 }

    /* return to whence we came from */
    rfeia   sp!
.endm

