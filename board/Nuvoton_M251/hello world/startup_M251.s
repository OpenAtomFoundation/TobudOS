;/**************************************************************************//**
; * @file     startup_M251.s
; * @version  V0.10
; * @brief    M251 Series Startup Source File
; *
; * @note
; * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
; ******************************************************************************/


;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

    IF :LNOT: :DEF: Stack_Size
Stack_Size      EQU     0x00000800
    ENDIF

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

    IF :LNOT: :DEF: Heap_Size
Heap_Size       EQU     0x00000000
    ENDIF

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                    
                IMPORT  SendChar_ToUART          
                IMPORT  GetChar          
                    
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp               ;     Top of Stack
                DCD     Reset_Handler              ;     Reset Handler
                DCD     NMI_Handler                ;     NMI Handler
                DCD     HardFault_Handler          ;     Hard Fault Handler
                DCD     SendChar_ToUART            ;     Addr: 0x10, sendchar in BL retarget.c
                DCD     GetChar                    ;     Addr: 0x14, getchar in BL retarget.c
                DCD     0                          ;     Reserved
                DCD     0                          ;     Reserved
                DCD     0                          ;     Reserved
                DCD     0                          ;     Reserved
                DCD     0                          ;     Reserved
                DCD     SVC_Handler                ;     SVCall Handler
                DCD     0                          ;     Reserved
                DCD     0                          ;     Reserved
                DCD     PendSV_Handler             ;     PendSV Handler
                DCD     SysTick_Handler            ;     SysTick Handler

                ; External Interrupts
                                                   ; maximum of 64 External Interrupts are possible
                DCD     BOD_IRQHandler             ; 0  
                DCD     IRCTRIM_IRQHandler         ; 1  
                DCD     PWRWU_IRQHandler           ; 2  
                DCD     DEFAULT_IRQHandler         ; 3  
                DCD     CLKFAIL_IRQHandler         ; 4  
                DCD     DEFAULT_IRQHandler         ; 5 
                DCD     RTC_IRQHandler             ; 6  
                DCD     TAMPER_IRQHandler          ; 7  
                DCD     WDT_IRQHandler             ; 8  
                DCD     WWDT_IRQHandler            ; 9  
                DCD     EINT0_IRQHandler           ; 10 
                DCD     EINT1_IRQHandler           ; 11 
                DCD     EINT2_IRQHandler           ; 12 
                DCD     EINT3_IRQHandler           ; 13 
                DCD     EINT4_IRQHandler           ; 14 
                DCD     EINT5_IRQHandler           ; 15 
                DCD     GPA_IRQHandler             ; 16 
                DCD     GPB_IRQHandler             ; 17 
                DCD     GPC_IRQHandler             ; 18 
                DCD     GPD_IRQHandler             ; 19 
                DCD     GPE_IRQHandler             ; 20 
                DCD     GPF_IRQHandler             ; 21 
                DCD     QSPI0_IRQHandler           ; 22 
                DCD     SPI0_IRQHandler            ; 23 
                DCD     BRAKE0_IRQHandler          ; 24 
                DCD     PWM0_P0_IRQHandler         ; 25 
                DCD     PWM0_P1_IRQHandler         ; 26 
                DCD     PWM0_P2_IRQHandler         ; 27 
                DCD     BRAKE1_IRQHandler          ; 28 
                DCD     PWM1_P0_IRQHandler         ; 29 
                DCD     PWM1_P1_IRQHandler         ; 30 
                DCD     PWM1_P2_IRQHandler         ; 31 
                DCD     TMR0_IRQHandler            ; 32 
                DCD     TMR1_IRQHandler            ; 33 
                DCD     TMR2_IRQHandler            ; 34 
                DCD     TMR3_IRQHandler            ; 35 
                DCD     UART0_IRQHandler           ; 36 
                DCD     UART1_IRQHandler           ; 37 
                DCD     I2C0_IRQHandler            ; 38 
                DCD     I2C1_IRQHandler            ; 39 
                DCD     PDMA_IRQHandler            ; 40 
                DCD     DAC_IRQHandler             ; 41 
                DCD     EADC_INT0_IRQHandler       ; 42 
                DCD     EADC_INT1_IRQHandler       ; 43 
                DCD     ACMP01_IRQHandler          ; 44 
                DCD     BPWM0_IRQHandler           ; 45 
                DCD     EADC_INT2_IRQHandler       ; 46 
                DCD     EADC_INT3_IRQHandler       ; 47 
                DCD     UART2_IRQHandler           ; 48 
                DCD     DEFAULT_IRQHandler         ; 49 
                DCD     USCI0_IRQHandler           ; 50 
                DCD     DEFAULT_IRQHandler         ; 51 
                DCD     USCI1_IRQHandler           ; 52 
                DCD     USBD_IRQHandler            ; 53 
                DCD     BPWM1_IRQHandler           ; 54 
                DCD     PSIO_IRQHandler            ; 55 
                DCD     DEFAULT_IRQHandler         ; 56 
                DCD     CRPT_IRQHandler            ; 57 
                DCD     SC0_IRQHandler             ; 58 
                DCD     DEFAULT_IRQHandler         ; 59 
                DCD     USCI2_IRQHandler           ; 60 
                DCD     DEFAULT_IRQHandler         ; 61 
                DCD     OPA_IRQHandler             ; 62 
                DCD     DEFAULT_IRQHandler         ; 63 


                                                     
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main

                ; Unlock Register                
    ;IF :DEF: __ARM_FEATURE_CMSE
    ;IF __ARM_FEATURE_CMSE = 3
                LDR     R0, =0x40000100
                LDR     R1, =0x59
                STR     R1, [R0]
                LDR     R1, =0x16
                STR     R1, [R0]
                LDR     R1, =0x88
                STR     R1, [R0]

                LDR     R0, =SystemInit
                BLX     R0
                
                ; Init POR
                LDR     R2, =0x40000024
                LDR     R1, =0x5AA5
                STR     R1, [R2]
                
                LDR     R2, =0x400001EC
                STR     R1, [R2]

                ; Lock register
                LDR     R0, =0x40000100
                MOVS    R1, #0
                STR     R1, [R0]                
    ;ENDIF
    ;ENDIF

                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                IMPORT  ProcessHardFault
                EXPORT  HardFault_Handler         [WEAK]
                MOV     R0, LR               
                MRS     R1, MSP                
                MRS     R2, PSP                
                LDR     R3, =ProcessHardFault 
                BLX     R3                     
                BX      R0                     
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  BOD_IRQHandler            [WEAK] ; 0  
                EXPORT  IRCTRIM_IRQHandler        [WEAK] ; 1  
                EXPORT  PWRWU_IRQHandler          [WEAK] ; 2  
               ;EXPORT  0                         [WEAK] ; 3  
                EXPORT  CLKFAIL_IRQHandler        [WEAK] ; 4  
               ;EXPORT  0                         [WEAK] ; 5  
                EXPORT  RTC_IRQHandler            [WEAK] ; 6  
                EXPORT  TAMPER_IRQHandler         [WEAK] ; 7  
                EXPORT  WDT_IRQHandler            [WEAK] ; 8  
                EXPORT  WWDT_IRQHandler           [WEAK] ; 9  
                EXPORT  EINT0_IRQHandler          [WEAK] ; 10 
                EXPORT  EINT1_IRQHandler          [WEAK] ; 11 
                EXPORT  EINT2_IRQHandler          [WEAK] ; 12 
                EXPORT  EINT3_IRQHandler          [WEAK] ; 13 
                EXPORT  EINT4_IRQHandler          [WEAK] ; 14 
                EXPORT  EINT5_IRQHandler          [WEAK] ; 15 
                EXPORT  GPA_IRQHandler            [WEAK] ; 16 
                EXPORT  GPB_IRQHandler            [WEAK] ; 17 
                EXPORT  GPC_IRQHandler            [WEAK] ; 18 
                EXPORT  GPD_IRQHandler            [WEAK] ; 19 
                EXPORT  GPE_IRQHandler            [WEAK] ; 20 
                EXPORT  GPF_IRQHandler            [WEAK] ; 21 
                EXPORT  QSPI0_IRQHandler          [WEAK] ; 22 
                EXPORT  SPI0_IRQHandler           [WEAK] ; 23 
                EXPORT  BRAKE0_IRQHandler         [WEAK] ; 24 
                EXPORT  PWM0_P0_IRQHandler        [WEAK] ; 25 
                EXPORT  PWM0_P1_IRQHandler        [WEAK] ; 26 
                EXPORT  PWM0_P2_IRQHandler        [WEAK] ; 27 
                EXPORT  BRAKE1_IRQHandler         [WEAK] ; 28 
                EXPORT  PWM1_P0_IRQHandler        [WEAK] ; 29 
                EXPORT  PWM1_P1_IRQHandler        [WEAK] ; 30 
                EXPORT  PWM1_P2_IRQHandler        [WEAK] ; 31 
                EXPORT  TMR0_IRQHandler           [WEAK] ; 32 
                EXPORT  TMR1_IRQHandler           [WEAK] ; 33 
                EXPORT  TMR2_IRQHandler           [WEAK] ; 34 
                EXPORT  TMR3_IRQHandler           [WEAK] ; 35 
                EXPORT  UART0_IRQHandler          [WEAK] ; 36 
                EXPORT  UART1_IRQHandler          [WEAK] ; 37 
                EXPORT  I2C0_IRQHandler           [WEAK] ; 38 
                EXPORT  I2C1_IRQHandler           [WEAK] ; 39 
                EXPORT  PDMA_IRQHandler           [WEAK] ; 40 
                EXPORT  DAC_IRQHandler            [WEAK] ; 41 
                EXPORT  EADC_INT0_IRQHandler      [WEAK] ; 42 
                EXPORT  EADC_INT1_IRQHandler      [WEAK] ; 43 
                EXPORT  ACMP01_IRQHandler         [WEAK] ; 44 
                EXPORT  BPWM0_IRQHandler          [WEAK] ; 45 
                EXPORT  EADC_INT2_IRQHandler      [WEAK] ; 46 
                EXPORT  EADC_INT3_IRQHandler      [WEAK] ; 47 
                EXPORT  UART2_IRQHandler          [WEAK] ; 48 
               ;EXPORT  0                         [WEAK] ; 49 
                EXPORT  USCI0_IRQHandler          [WEAK] ; 50 
               ;EXPORT  0                         [WEAK] ; 51 
                EXPORT  USCI1_IRQHandler          [WEAK] ; 52 
                EXPORT  USBD_IRQHandler           [WEAK] ; 53 
                EXPORT  BPWM1_IRQHandler          [WEAK] ; 54 
                EXPORT  PSIO_IRQHandler           [WEAK] ; 55 
               ;EXPORT  0                         [WEAK] ; 56 
                EXPORT  CRPT_IRQHandler           [WEAK] ; 57 
                EXPORT  SC0_IRQHandler            [WEAK] ; 58 
               ;EXPORT  0                         [WEAK] ; 59 
                EXPORT  USCI2_IRQHandler          [WEAK] ; 60 
               ;EXPORT  0                         [WEAK] ; 61 
                EXPORT  OPA_IRQHandler            [WEAK] ; 62 
               ;EXPORT  0                         [WEAK] ; 63 



                EXPORT  DEFAULT_IRQHandler        [WEAK]                                                
                                                                
BOD_IRQHandler		       ; 0  
IRCTRIM_IRQHandler         ; 1  
PWRWU_IRQHandler           ; 2  
;0                         ; 3  
CLKFAIL_IRQHandler         ; 4  
;0                         ; 5  
RTC_IRQHandler             ; 6  
TAMPER_IRQHandler          ; 7  
WDT_IRQHandler             ; 8  
WWDT_IRQHandler            ; 9  
EINT0_IRQHandler           ; 10 
EINT1_IRQHandler           ; 11 
EINT2_IRQHandler           ; 12 
EINT3_IRQHandler           ; 13 
EINT4_IRQHandler           ; 14 
EINT5_IRQHandler           ; 15 
GPA_IRQHandler             ; 16 
GPB_IRQHandler             ; 17 
GPC_IRQHandler             ; 18 
GPD_IRQHandler             ; 19 
GPE_IRQHandler             ; 20 
GPF_IRQHandler             ; 21 
QSPI0_IRQHandler           ; 22 
SPI0_IRQHandler            ; 23 
BRAKE0_IRQHandler          ; 24 
PWM0_P0_IRQHandler         ; 25 
PWM0_P1_IRQHandler         ; 26 
PWM0_P2_IRQHandler         ; 27 
BRAKE1_IRQHandler          ; 28 
PWM1_P0_IRQHandler         ; 29 
PWM1_P1_IRQHandler         ; 30 
PWM1_P2_IRQHandler         ; 31 
TMR0_IRQHandler            ; 32 
TMR1_IRQHandler            ; 33 
TMR2_IRQHandler            ; 34 
TMR3_IRQHandler            ; 35 
UART0_IRQHandler           ; 36 
UART1_IRQHandler           ; 37 
I2C0_IRQHandler            ; 38 
I2C1_IRQHandler            ; 39 
PDMA_IRQHandler            ; 40 
DAC_IRQHandler             ; 41 
EADC_INT0_IRQHandler       ; 42 
EADC_INT1_IRQHandler       ; 43 
ACMP01_IRQHandler          ; 44 
BPWM0_IRQHandler           ; 45 
EADC_INT2_IRQHandler       ; 46 
EADC_INT3_IRQHandler       ; 47 
UART2_IRQHandler           ; 48 
;0                         ; 49 
USCI0_IRQHandler           ; 50   
;0                         ; 51 
USCI1_IRQHandler           ; 52 
USBD_IRQHandler            ; 53 
BPWM1_IRQHandler           ; 54
PSIO_IRQHandler            ; 55 
;0                         ; 56   
CRPT_IRQHandler            ; 57 
SC0_IRQHandler             ; 58 
;0                         ; 59 
USCI2_IRQHandler           ; 60 
;0                         ; 61
OPA_IRQHandler             ; 62 
;0                         ; 63

DEFAULT_IRQHandler
                B       .
                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0,  =Heap_Mem
                LDR     R1,  =(Stack_Mem + Stack_Size)
                LDR     R2,  =(Heap_Mem +  Heap_Size)
                LDR     R3,  =Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF

;int32_t SH_DoCommand(int32_t n32In_R0, int32_t n32In_R1, int32_t *pn32Out_R0)
SH_DoCommand    PROC
    
                EXPORT      SH_DoCommand
                IMPORT      SH_Return
                    
                BKPT   0xAB                ; Wait ICE or HardFault
                LDR    R3, =SH_Return
                PUSH   {R3 ,lr}
                BLX    R3                  ; Call SH_Return. The return value is in R0
                POP    {R3 ,PC}            ; Return value = R0
                
                ENDP

__PC            PROC
                EXPORT      __PC
                
                MOV     r0, lr
                BLX     lr
                ALIGN
                    
                ENDP
                    
                END
