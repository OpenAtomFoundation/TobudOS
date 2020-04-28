/*
 * THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
 * REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY,
 * INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *
 * COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE.
 * TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET
 * POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY
 * INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR
 * YOUR USE OF THE PROGRAM.
 *
 * IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY
 * THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT
 * OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM.
 * EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF
 * REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS
 * OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF
 * USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S
 * AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF
 * YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS
 * (U.S.$500).
 *
 * Unless otherwise stated, the Program written and copyrighted
 * by Texas Instruments is distributed as "freeware".  You may,
 * only under TI's copyright in the Program, use and modify the
 * Program without any charge or restriction.  You may
 * distribute to third parties, provided that you transfer a
 * copy of this license to the third party and the third party
 * agrees to these terms by its first use of the Program. You
 * must reproduce the copyright notice and any other legend of
 * ownership on each copy or partial copy, of the Program.
 *
 * You acknowledge and agree that the Program contains
 * copyrighted material, trade secrets and other TI proprietary
 * information and is protected by copyright laws,
 * international copyright treaties, and trade secret laws, as
 * well as other intellectual property laws.  To protect TI's
 * rights in the Program, you agree not to decompile, reverse
 * engineer, disassemble or otherwise translate any object code
 * versions of the Program to a human-readable form.  You agree
 * that in no event will you alter, remove or destroy any
 * copyright notice included in the Program.  TI reserves all
 * rights not specifically granted under this license. Except
 * as specifically provided herein, nothing in this agreement
 * shall be construed as conferring by implication, estoppel,
 * or otherwise, upon you, any license or other right under any
 * TI patents, copyrights or trade secrets.
 *
 * You may not use the Program in non-TI devices.
 */

/* ----------------------- MSP430 registers ---------------------------------*/
DCOCTL    .equ  0x0056          /* DCO Clock Frequency Control */
TACTL     .equ  0x0160          /* Timer A 0 Control */
TACCTL2   .equ  0x0166          /* Timer A 0 Capture/Compare Control 2 */
TACCR2    .equ  0x0176          /* Timer A 0 Capture/Compare 2 */
BCSCTL1   .equ  0x0057          /* Basic Clock Module Register */

XT2OFF    .equ  0x80
DIVA_3    .equ  0x30
CCIFG     .equ  0x0001          /* Capture/compare interrupt flag */
TACLR     .equ  0x0004          /*  Timer A counter clear */

CM_1      .equ  (1*0x4000)      /* Capture mode: 1 - pos. edge */
CCIS_1    .equ  (1*0x1000)      /* Capture input select : 1 - CCIxB */
CAP       .equ  0x0100          /* Capture mode: 1, Compare mode: 0 */

TASSEL_2  .equ  (2*0x100)       /* Timer A clock source select */
MC_2      .equ  (2*0x10)        /* Timer A mode control: 2 - Continous up */

/* ----------------------- MSP430 registers ---------------------------------*/

/*
 * Subroutine: Sets DCO to selected frequency based on Delta.
 *  R14 and R15 are used, ACLK = 32768/8 Timer_A clocked by DCOCLK
 *  R13 used to hold old BCSCTL1 value
 *  R15 is the parameter specifying the delta value, MCLK = delta * 4096
 *  returns in R15:
 *   0 - DCO Set Correctly
 *   1 - DCO running at slowest setting
 *   2 - DCO running at fastest setting
 *   0xff - Timeout Occured while setting DCO
 */

    CODE
    ALIGN 2
    EXPORT _cTISetDCO

_cTISetDCO:
    push.w  r10
    push.w  r11
    push.w  &TACTL
    push.w  &TACCTL2
    mov.w   #10000, r11         /* max number of times to loop */
    mov.w   r15, r12
    clr.w   r15                 
    mov.b   &BCSCTL1, r13       /* Copy BC settings to preserve */
    and.w   #0xF8,    r13       /* Mask out bits which will change (0-2) 1xx */
SetupBC:
    /* ACLK=LFXT1CLK/8, XT2OFF to make a known state */
    bis.b   #(XT2OFF + DIVA_3),&BCSCTL1
SetupCC2:
    /* CAP, ACLK */
    mov.w   #(CM_1 + CCIS_1 + CAP), &TACCTL2
    /* SMCLK, cont-mode, clear */
    mov.w   #(TASSEL_2 + MC_2 + TACLR), &TACTL
firstTest:
    bit.w   #CCIFG, &TACCTL2    /* Test capture flag, skip first short ACLK to avoid error */
    jz      firstTest
    bic.w   #CCIFG, &TACCTL2    /* Clear capture flag */
TestDCO:
    dec.w   r11
    jz      ExitTimeOutError
TestDCO2:
    bit.w   #CCIFG, &TACCTL2    /* Test capture flag */
    jz      TestDCO2            
    bic.w   #CCIFG, &TACCTL2    /* Clear capture flag */

AdjDCO:
    mov.w   &TACCR2, r14        /* R14 = captured SMCLK */
    sub.w   r15, r14            /* R14 = capture difference */
    mov.w   &TACCR2, r15        /* R15 = captured SMCLK */
    mov.w   r14, r10            /* R10 = capture difference */
    sub.w   r12, r10            /* R10 = difference between desired and actual */
    jz      ExitNoError         /* if R12 = R10, DCO set exactly */
    cmp.w   #0xFFFF, r10        /* DCO can be off by 1 for a tolerance */
    jeq     ExitNoError
    cmp.w   r12,   r14          /* Delta = SMCLK/(32768/8) */
    jlo     IncDCO                  
DecDCO:
    dec.b   &DCOCTL             /* Slow DCO with DCO and MOD */
    jc      TestDCO             /* Slower? */
    bit.b   #0x7, &BCSCTL1      /* Can RSEL.x be decremented? 1xx */
    jz      ExitSlowestSetting  /* jmp>DCO at slowest setting */
    dec.b   &BCSCTL1            /* Decrement RSEL.x */
    jmp     TestDCO
IncDCO:
    inc.b   &DCOCTL             /* Speed DCO with DCO and MOD */
    jnc     TestDCO             /* Faster? */
    /* Can RSEL.x be increased? 1xx */
    cmp.b   #(XT2OFF + DIVA_3 + 0x7), &BCSCTL1
    jeq     ExitFastestSetting  /* jmp> DCO at fastest setting */
    inc.b   &BCSCTL1            /* Increment RSEL.x */
    jmp     TestDCO             
ExitSlowestSetting:
    clr.b   &DCOCTL
    mov.w   #0x01, r15
    jmp     SetDCO_Exit
ExitFastestSetting:
    mov.b   #0xFF, &DCOCTL      /* since the DCO previously rolled over */
ExitFastestSetting2xx:
    mov.w   #0x02, r15
    jmp     SetDCO_Exit
ExitTimeOutError:
    mov.w   #0xff, r15
    jmp     SetDCO_Exit
ExitNoError:
    clr.w   r15
SetDCO_Exit:
    clr.w   TACCTL2             /* Stop CCR2 */
    bic.b   #0xF8, &BCSCTL1     /* Mask out bits which will change (0-2) 1xx */
    bis.b   r13, &BCSCTL1       /* Copy in old non-DCO BC settings */
    pop.w   &TACTL
    pop.w   &TACCTL2
    pop.w   r11
    pop.w   r10
    ret                         /* Return from subroutine */

    .end
