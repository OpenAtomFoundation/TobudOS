;/*!
;    \file  startup_gd32f1x0.s
;    \brief start up file
;*/

;/*
;    Copyright (C) 2017 GigaDevice

;    2014-12-26, V1.0.0, firmware for GD32F1x0(x=3,5)
;    2016-01-15, V2.0.0, firmware for GD32F1x0(x=3,5,7,9)
;    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
;    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
;*/

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000400

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

;               /* reset Vector Mapped to at Address 0 */
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                      ; Top of Stack
                DCD     Reset_Handler                     ; Reset Handler
                DCD     NMI_Handler                       ; NMI Handler
                DCD     HardFault_Handler                 ; Hard Fault Handler
                DCD     MemManage_Handler                 ; MPU Fault Handler
                DCD     BusFault_Handler                  ; Bus Fault Handler
                DCD     UsageFault_Handler                ; Usage Fault Handler
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     SVC_Handler                       ; SVCall Handler
                DCD     DebugMon_Handler                  ; Debug Monitor Handler
                DCD     0                                 ; Reserved
                DCD     PendSV_Handler                    ; PendSV Handler
                DCD     SysTick_Handler                   ; SysTick Handler

;               /* external interrupts handler */
                DCD     WWDGT_IRQHandler                  ; 16:Window Watchdog Timer
                DCD     LVD_IRQHandler                    ; 17:LVD through EXTI Line detect
                DCD     RTC_IRQHandler                    ; 18:RTC through EXTI Line
                DCD     FMC_IRQHandler                    ; 19:FMC
                DCD     RCU_IRQHandler                    ; 20:RCU
                DCD     EXTI0_1_IRQHandler                ; 21:EXTI Line 0 and EXTI Line 1
                DCD     EXTI2_3_IRQHandler                ; 22:EXTI Line 2 and EXTI Line 3
                DCD     EXTI4_15_IRQHandler               ; 23:EXTI Line 4 to EXTI Line 15
                DCD     TSI_IRQHandler                    ; 24:TSI
                DCD     DMA_Channel0_IRQHandler           ; 25:DMA Channel 0 
                DCD     DMA_Channel1_2_IRQHandler         ; 26:DMA Channel 1 and DMA Channel 2
                DCD     DMA_Channel3_4_IRQHandler         ; 27:DMA Channel 3 and DMA Channel 4
                DCD     ADC_CMP_IRQHandler                ; 28:ADC and Comparator 0-1
                DCD     TIMER0_BRK_UP_TRG_COM_IRQHandler  ; 29:TIMER0 Break,Update,Trigger and Commutation
                DCD     TIMER0_Channel_IRQHandler         ; 30:TIMER0 Channel
                DCD     TIMER1_IRQHandler                 ; 31:TIMER1
                DCD     TIMER2_IRQHandler                 ; 32:TIMER2
                DCD     TIMER5_DAC_IRQHandler             ; 33:TIMER5 and DAC
                DCD     0                                 ; Reserved
                DCD     TIMER13_IRQHandler                ; 35:TIMER13
                DCD     TIMER14_IRQHandler                ; 36:TIMER14
                DCD     TIMER15_IRQHandler                ; 37:TIMER15
                DCD     TIMER16_IRQHandler                ; 38:TIMER16
                DCD     I2C0_EV_IRQHandler                ; 39:I2C0 Event
                DCD     I2C1_EV_IRQHandler                ; 40:I2C1 Event
                DCD     SPI0_IRQHandler                   ; 41:SPI0
                DCD     SPI1_IRQHandler                   ; 42:SPI1
                DCD     USART0_IRQHandler                 ; 43:USART0
                DCD     USART1_IRQHandler                 ; 44:USART1
                DCD     0                                 ; Reserved
                DCD     CEC_IRQHandler                    ; 46:CEC
                DCD     0                                 ; Reserved
                DCD     I2C0_ER_IRQHandler                ; 48:I2C0 Error
                DCD     0                                 ; Reserved
                DCD     I2C1_ER_IRQHandler                ; 50:I2C1 Error
                DCD     I2C2_EV_IRQHandler                ; 51:I2C2 Event
                DCD     I2C2_ER_IRQHandler                ; 52:I2C2 Error
                DCD     USBD_LP_IRQHandler                ; 53:USBD LP
                DCD     USBD_HP_IRQHandler                ; 54:USBD HP
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     USBDWakeUp_IRQHandler             ; 58:USBD Wakeup
                DCD     CAN0_TX_IRQHandler                ; 59:CAN0 TX
                DCD     CAN0_RX0_IRQHandler               ; 60:CAN0 RX0
                DCD     CAN0_RX1_IRQHandler               ; 61:CAN0 RX1
                DCD     CAN0_SCE_IRQHandler               ; 62:CAN0 SCE
                DCD     SLCD_IRQHandler                   ; 63:SLCD
                DCD     DMA_Channel5_6_IRQHandler         ; 64:DMA Channel5 and Channel6 
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     SPI2_IRQHandler                   ; 67:SPI2
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     CAN1_TX_IRQHandler                ; 86:CAN1 TX
                DCD     CAN1_RX0_IRQHandler               ; 87:CAN1 RX0
                DCD     CAN1_RX1_IRQHandler               ; 88:CAN1 RX1
                DCD     CAN1_SCE_IRQHandler               ; 89:CAN1 SCE
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

;/* reset Handler */
Reset_Handler   PROC
                EXPORT  Reset_Handler                     [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

;/* dummy Exception Handlers */
NMI_Handler     PROC
                EXPORT  NMI_Handler                       [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler                 [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler                 [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler                  [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler                [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                       [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler                  [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler                    [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler                   [WEAK]
                B       .
                ENDP

Default_Handler PROC
;               /* external interrupts handler */
                EXPORT  WWDGT_IRQHandler                  [WEAK]
                EXPORT  LVD_IRQHandler                    [WEAK]
                EXPORT  RTC_IRQHandler                    [WEAK]
                EXPORT  FMC_IRQHandler                    [WEAK]
                EXPORT  RCU_IRQHandler                    [WEAK]
                EXPORT  EXTI0_1_IRQHandler                [WEAK]
                EXPORT  EXTI2_3_IRQHandler                [WEAK]
                EXPORT  EXTI4_15_IRQHandler               [WEAK]
                EXPORT  TSI_IRQHandler                    [WEAK]
                EXPORT  DMA_Channel0_IRQHandler           [WEAK]
                EXPORT  DMA_Channel1_2_IRQHandler         [WEAK]
                EXPORT  DMA_Channel3_4_IRQHandler         [WEAK]
                EXPORT  ADC_CMP_IRQHandler                [WEAK]
                EXPORT  TIMER0_BRK_UP_TRG_COM_IRQHandler  [WEAK]
                EXPORT  TIMER0_Channel_IRQHandler         [WEAK]
                EXPORT  TIMER1_IRQHandler                 [WEAK]
                EXPORT  TIMER2_IRQHandler                 [WEAK]
                EXPORT  TIMER5_DAC_IRQHandler             [WEAK]
                EXPORT  TIMER13_IRQHandler                [WEAK]
                EXPORT  TIMER14_IRQHandler                [WEAK]
                EXPORT  TIMER15_IRQHandler                [WEAK]
                EXPORT  TIMER16_IRQHandler                [WEAK]
                EXPORT  I2C0_EV_IRQHandler                [WEAK]
                EXPORT  I2C1_EV_IRQHandler                [WEAK]
                EXPORT  SPI0_IRQHandler                   [WEAK]
                EXPORT  SPI1_IRQHandler                   [WEAK]
                EXPORT  USART0_IRQHandler                 [WEAK]
                EXPORT  USART1_IRQHandler                 [WEAK]
                EXPORT  CEC_IRQHandler                    [WEAK]
                EXPORT  I2C0_ER_IRQHandler                [WEAK]
                EXPORT  I2C1_ER_IRQHandler                [WEAK]
                EXPORT  I2C2_EV_IRQHandler                [WEAK]
                EXPORT  I2C2_ER_IRQHandler                [WEAK]
                EXPORT  USBD_LP_IRQHandler                [WEAK]
                EXPORT  USBD_HP_IRQHandler                [WEAK]
                EXPORT  USBDWakeUp_IRQHandler             [WEAK]
                EXPORT  CAN0_TX_IRQHandler                [WEAK]
                EXPORT  CAN0_RX0_IRQHandler               [WEAK]
                EXPORT  CAN0_RX1_IRQHandler               [WEAK]
                EXPORT  CAN0_SCE_IRQHandler               [WEAK]
                EXPORT  SLCD_IRQHandler                   [WEAK]
                EXPORT  DMA_Channel5_6_IRQHandler         [WEAK]
                EXPORT  SPI2_IRQHandler                   [WEAK]
                EXPORT  CAN1_TX_IRQHandler                [WEAK]
                EXPORT  CAN1_RX0_IRQHandler               [WEAK]
                EXPORT  CAN1_RX1_IRQHandler               [WEAK]
                EXPORT  CAN1_SCE_IRQHandler               [WEAK]

;/* external interrupts handler */
WWDGT_IRQHandler
LVD_IRQHandler
RTC_IRQHandler
FMC_IRQHandler
RCU_IRQHandler
EXTI0_1_IRQHandler
EXTI2_3_IRQHandler
EXTI4_15_IRQHandler
TSI_IRQHandler
DMA_Channel0_IRQHandler
DMA_Channel1_2_IRQHandler
DMA_Channel3_4_IRQHandler
ADC_CMP_IRQHandler
TIMER0_BRK_UP_TRG_COM_IRQHandler
TIMER0_Channel_IRQHandler
TIMER1_IRQHandler
TIMER2_IRQHandler
TIMER5_DAC_IRQHandler
TIMER13_IRQHandler
TIMER14_IRQHandler
TIMER15_IRQHandler
TIMER16_IRQHandler
I2C0_EV_IRQHandler
I2C1_EV_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
USART0_IRQHandler
USART1_IRQHandler
CEC_IRQHandler
I2C0_ER_IRQHandler
I2C1_ER_IRQHandler
I2C2_EV_IRQHandler
I2C2_ER_IRQHandler
USBD_LP_IRQHandler
USBD_HP_IRQHandler
USBDWakeUp_IRQHandler
CAN0_TX_IRQHandler
CAN0_RX0_IRQHandler
CAN0_RX1_IRQHandler
CAN0_SCE_IRQHandler
SLCD_IRQHandler
DMA_Channel5_6_IRQHandler
SPI2_IRQHandler
CAN1_TX_IRQHandler
CAN1_RX0_IRQHandler
CAN1_RX1_IRQHandler
CAN1_SCE_IRQHandler
                B       .
                ENDP

                ALIGN

; user Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF

                END
