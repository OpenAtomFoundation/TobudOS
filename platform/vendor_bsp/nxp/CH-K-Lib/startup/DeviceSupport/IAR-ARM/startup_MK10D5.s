;/*****************************************************************************
; * @file:    startup_MK10D5.s
; * @purpose: CMSIS Cortex-M4 Core Device Startup File
; *           MK10D10
; * @version: 1.0
; * @date:    2014-4-2
; *----------------------------------------------------------------------------
; *
; * www.beyondcore.net
; *
; ******************************************************************************/


;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;


        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)
        SECTION .intvec:CODE:NOROOT(2)
        EXTERN  __iar_program_start
        EXTERN  SystemInit        
        PUBLIC  __vector_table
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA
		
		
		
__vector_table
        DCD     sfe(CSTACK)
                DCD     Reset_Handler  ; Reset Handler
                DCD     NMI_Handler  ; NMI Handler
                DCD     HardFault_Handler  ; Hard Fault Handler
                DCD     MemManage_Handler  ; MPU Fault Handler
                DCD     BusFault_Handler  ; Bus Fault Handler
                DCD     UsageFault_Handler  ; Usage Fault Handler
                DCD     0  ; Reserved
                DCD     0  ; Reserved
                DCD     0  ; Reserved
                DCD     0  ; Reserved
                DCD     SVC_Handler  ; SVCall Handler
                DCD     DebugMon_Handler  ; Debug Monitor Handler
                DCD     0  ; Reserved
                DCD     PendSV_Handler  ; PendSV Handler
                DCD     SysTick_Handler  ; SysTick Handler

                ; External Interrupts
                DCD     DMA0_IRQHandler  ; DMA channel 0 transfer complete interrupt
                DCD     DMA1_IRQHandler  ; DMA channel 1 transfer complete interrupt
                DCD     DMA2_IRQHandler  ; DMA channel 2 transfer complete interrupt
                DCD     DMA3_IRQHandler  ; DMA channel 3 transfer complete interrupt
                DCD     DMA_Error_IRQHandler  ; DMA error interrupt
                DCD     Reserved21_IRQHandler  ; Reserved interrupt 21
                DCD     FTFL_IRQHandler  ; FTFL interrupt
                DCD     Read_Collision_IRQHandler  ; Read collision interrupt
                DCD     LVD_LVW_IRQHandler  ; Low Voltage Detect, Low Voltage Warning
                DCD     LLW_IRQHandler  ; Low Leakage Wakeup
                DCD     Watchdog_IRQHandler  ; WDOG interrupt
                DCD     I2C0_IRQHandler  ; I2C0 interrupt
                DCD     SPI0_IRQHandler  ; SPI0 interrupt
                DCD     I2S0_Tx_IRQHandler  ; I2S0 transmit interrupt
                DCD     I2S0_Rx_IRQHandler  ; I2S0 receive interrupt
                DCD     UART0_LON_IRQHandler  ; UART0 LON interrupt
                DCD     UART0_RX_TX_IRQHandler  ; UART0 receive/transmit interrupt
                DCD     UART0_ERR_IRQHandler  ; UART0 error interrupt
                DCD     UART1_RX_TX_IRQHandler  ; UART1 receive/transmit interrupt
                DCD     UART1_ERR_IRQHandler  ; UART1 error interrupt
                DCD     UART2_RX_TX_IRQHandler  ; UART2 receive/transmit interrupt
                DCD     UART2_ERR_IRQHandler  ; UART2 error interrupt
                DCD     ADC0_IRQHandler  ; ADC0 interrupt
                DCD     CMP0_IRQHandler  ; CMP0 interrupt
                DCD     CMP1_IRQHandler  ; CMP1 interrupt
                DCD     FTM0_IRQHandler  ; FTM0 fault, overflow and channels interrupt
                DCD     FTM1_IRQHandler  ; FTM1 fault, overflow and channels interrupt
                DCD     CMT_IRQHandler  ; CMT interrupt
                DCD     RTC_IRQHandler  ; RTC interrupt
                DCD     RTC_Seconds_IRQHandler  ; RTC seconds interrupt
                DCD     PIT0_IRQHandler  ; PIT timer channel 0 interrupt
                DCD     PIT1_IRQHandler  ; PIT timer channel 1 interrupt
                DCD     PIT2_IRQHandler  ; PIT timer channel 2 interrupt
                DCD     PIT3_IRQHandler  ; PIT timer channel 3 interrupt
                DCD     PDB0_IRQHandler  ; PDB0 interrupt
                DCD     Reserved51_IRQHandler  ; Reserved interrupt 51
                DCD     Reserved52_IRQHandler  ; Reserved interrupt 52
                DCD     TSI0_IRQHandler  ; TSI0 interrupt
                DCD     MCG_IRQHandler  ; MCG interrupt
                DCD     LPTimer_IRQHandler  ; LPTimer interrupt
                DCD     PORTA_IRQHandler  ; Port A interrupt
                DCD     PORTB_IRQHandler  ; Port B interrupt
                DCD     PORTC_IRQHandler  ; Port C interrupt
                DCD     PORTD_IRQHandler  ; Port D interrupt
                DCD     PORTE_IRQHandler  ; Port E interrupt
                DCD     SWI_IRQHandler  ; Software interrupt
                DCD     DefaultISR  ; 62
                DCD     DefaultISR  ; 63
                DCD     DefaultISR  ; 64
                DCD     DefaultISR  ; 65
                DCD     DefaultISR  ; 66
                DCD     DefaultISR  ; 67
                DCD     DefaultISR  ; 68
                DCD     DefaultISR  ; 69
                DCD     DefaultISR  ; 70
                DCD     DefaultISR  ; 71
                DCD     DefaultISR  ; 72
                DCD     DefaultISR  ; 73
                DCD     DefaultISR  ; 74
                DCD     DefaultISR  ; 75
                DCD     DefaultISR  ; 76
                DCD     DefaultISR  ; 77
                DCD     DefaultISR  ; 78
                DCD     DefaultISR  ; 79
                DCD     DefaultISR  ; 80
                DCD     DefaultISR  ; 81
                DCD     DefaultISR  ; 82
                DCD     DefaultISR  ; 83
                DCD     DefaultISR  ; 84
                DCD     DefaultISR  ; 85
                DCD     DefaultISR  ; 86
                DCD     DefaultISR  ; 87
                DCD     DefaultISR  ; 88
                DCD     DefaultISR  ; 89
                DCD     DefaultISR  ; 90
                DCD     DefaultISR  ; 91
                DCD     DefaultISR  ; 92
                DCD     DefaultISR  ; 93
                DCD     DefaultISR  ; 94
                DCD     DefaultISR  ; 95
                DCD     DefaultISR  ; 96
                DCD     DefaultISR  ; 97
                DCD     DefaultISR  ; 98
                DCD     DefaultISR  ; 99
                DCD     DefaultISR  ; 100
                DCD     DefaultISR  ; 101
                DCD     DefaultISR  ; 102
                DCD     DefaultISR  ; 103
                DCD     DefaultISR  ; 104
                DCD     DefaultISR  ; 105
                DCD     DefaultISR  ; 106
                DCD     DefaultISR  ; 107
                DCD     DefaultISR  ; 108
                DCD     DefaultISR  ; 109
                DCD     DefaultISR  ; 110
                DCD     DefaultISR  ; 111
                DCD     DefaultISR  ; 112
                DCD     DefaultISR  ; 113
                DCD     DefaultISR  ; 114
                DCD     DefaultISR  ; 115
                DCD     DefaultISR  ; 116
                DCD     DefaultISR  ; 117
                DCD     DefaultISR  ; 118
                DCD     DefaultISR  ; 119
                DCD     DefaultISR  ; 120
                DCD     DefaultISR  ; 121
                DCD     DefaultISR  ; 122
                DCD     DefaultISR  ; 123
                DCD     DefaultISR  ; 124
                DCD     DefaultISR  ; 125
                DCD     DefaultISR  ; 126
                DCD     DefaultISR  ; 127
                DCD     DefaultISR  ; 128
                DCD     DefaultISR  ; 129
                DCD     DefaultISR  ; 130
                DCD     DefaultISR  ; 131
                DCD     DefaultISR  ; 132
                DCD     DefaultISR  ; 133
                DCD     DefaultISR  ; 134
                DCD     DefaultISR  ; 135
                DCD     DefaultISR  ; 136
                DCD     DefaultISR  ; 137
                DCD     DefaultISR  ; 138
                DCD     DefaultISR  ; 139
                DCD     DefaultISR  ; 140
                DCD     DefaultISR  ; 141
                DCD     DefaultISR  ; 142
                DCD     DefaultISR  ; 143
                DCD     DefaultISR  ; 144
                DCD     DefaultISR  ; 145
                DCD     DefaultISR  ; 146
                DCD     DefaultISR  ; 147
                DCD     DefaultISR  ; 148
                DCD     DefaultISR  ; 149
                DCD     DefaultISR  ; 150
                DCD     DefaultISR  ; 151
                DCD     DefaultISR  ; 152
                DCD     DefaultISR  ; 153
                DCD     DefaultISR  ; 154
                DCD     DefaultISR  ; 155
                DCD     DefaultISR  ; 156
                DCD     DefaultISR  ; 157
                DCD     DefaultISR  ; 158
                DCD     DefaultISR  ; 159
                DCD     DefaultISR  ; 160
                DCD     DefaultISR  ; 161
                DCD     DefaultISR  ; 162
                DCD     DefaultISR  ; 163
                DCD     DefaultISR  ; 164
                DCD     DefaultISR  ; 165
                DCD     DefaultISR  ; 166
                DCD     DefaultISR  ; 167
                DCD     DefaultISR  ; 168
                DCD     DefaultISR  ; 169
                DCD     DefaultISR  ; 170
                DCD     DefaultISR  ; 171
                DCD     DefaultISR  ; 172
                DCD     DefaultISR  ; 173
                DCD     DefaultISR  ; 174
                DCD     DefaultISR  ; 175
                DCD     DefaultISR  ; 176
                DCD     DefaultISR  ; 177
                DCD     DefaultISR  ; 178
                DCD     DefaultISR  ; 179
                DCD     DefaultISR  ; 180
                DCD     DefaultISR  ; 181
                DCD     DefaultISR  ; 182
                DCD     DefaultISR  ; 183
                DCD     DefaultISR  ; 184
                DCD     DefaultISR  ; 185
                DCD     DefaultISR  ; 186
                DCD     DefaultISR  ; 187
                DCD     DefaultISR  ; 188
                DCD     DefaultISR  ; 189
                DCD     DefaultISR  ; 190
                DCD     DefaultISR  ; 191
                DCD     DefaultISR  ; 192
                DCD     DefaultISR  ; 193
                DCD     DefaultISR  ; 194
                DCD     DefaultISR  ; 195
                DCD     DefaultISR  ; 196
                DCD     DefaultISR  ; 197
                DCD     DefaultISR  ; 198
                DCD     DefaultISR  ; 199
                DCD     DefaultISR  ; 200
                DCD     DefaultISR  ; 201
                DCD     DefaultISR  ; 202
                DCD     DefaultISR  ; 203
                DCD     DefaultISR  ; 204
                DCD     DefaultISR  ; 205
                DCD     DefaultISR  ; 206
                DCD     DefaultISR  ; 207
                DCD     DefaultISR  ; 208
                DCD     DefaultISR  ; 209
                DCD     DefaultISR  ; 210
                DCD     DefaultISR  ; 211
                DCD     DefaultISR  ; 212
                DCD     DefaultISR  ; 213
                DCD     DefaultISR  ; 214
                DCD     DefaultISR  ; 215
                DCD     DefaultISR  ; 216
                DCD     DefaultISR  ; 217
                DCD     DefaultISR  ; 218
                DCD     DefaultISR  ; 219
                DCD     DefaultISR  ; 220
                DCD     DefaultISR  ; 221
                DCD     DefaultISR  ; 222
                DCD     DefaultISR  ; 223
                DCD     DefaultISR  ; 224
                DCD     DefaultISR  ; 225
                DCD     DefaultISR  ; 226
                DCD     DefaultISR  ; 227
                DCD     DefaultISR  ; 228
                DCD     DefaultISR  ; 229
                DCD     DefaultISR  ; 230
                DCD     DefaultISR  ; 231
                DCD     DefaultISR  ; 232
                DCD     DefaultISR  ; 233
                DCD     DefaultISR  ; 234
                DCD     DefaultISR  ; 235
                DCD     DefaultISR  ; 236
                DCD     DefaultISR  ; 237
                DCD     DefaultISR  ; 238
                DCD     DefaultISR  ; 239
                DCD     DefaultISR  ; 240
                DCD     DefaultISR  ; 241
                DCD     DefaultISR  ; 242
                DCD     DefaultISR  ; 243
                DCD     DefaultISR  ; 244
                DCD     DefaultISR  ; 245
                DCD     DefaultISR  ; 246
                DCD     DefaultISR  ; 247
                DCD     DefaultISR  ; 248
                DCD     DefaultISR  ; 249
                DCD     DefaultISR  ; 250
                DCD     DefaultISR  ; 251
                DCD     DefaultISR  ; 252
                DCD     DefaultISR  ; 253
                DCD     DefaultISR  ; 254
                DCD     DefaultISR  ; 255
__Vectors_End
__FlashConfig
      	DCD	0xFFFFFFFF
      	DCD	0xFFFFFFFF
      	DCD	0xFFFFFFFF
      	DCD	0xFFFFFFFE
__FlashConfig_End

__Vectors       EQU   __vector_table
__Vectors_Size 	EQU 	__Vectors_End - __Vectors

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0


        PUBWEAK NMI_Handler  
        SECTION .text:CODE:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK DMA0_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA0_IRQHandler
        B DMA0_IRQHandler

        PUBWEAK DMA1_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA1_IRQHandler
        B DMA1_IRQHandler

        PUBWEAK DMA2_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA2_IRQHandler
        B DMA2_IRQHandler

        PUBWEAK DMA3_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA3_IRQHandler
        B DMA3_IRQHandler

        PUBWEAK DMA4_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA4_IRQHandler
        B DMA4_IRQHandler

        PUBWEAK DMA5_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA5_IRQHandler
        B DMA5_IRQHandler

        PUBWEAK DMA6_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA6_IRQHandler
        B DMA6_IRQHandler

        PUBWEAK DMA7_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA7_IRQHandler
        B DMA7_IRQHandler

        PUBWEAK DMA8_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA8_IRQHandler
        B DMA8_IRQHandler

        PUBWEAK DMA9_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA9_IRQHandler
        B DMA9_IRQHandler

        PUBWEAK DMA10_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA10_IRQHandler
        B DMA10_IRQHandler

        PUBWEAK DMA11_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA11_IRQHandler
        B DMA11_IRQHandler

        PUBWEAK DMA12_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA12_IRQHandler
        B DMA12_IRQHandler

        PUBWEAK DMA13_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA13_IRQHandler
        B DMA13_IRQHandler

        PUBWEAK DMA14_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA14_IRQHandler
        B DMA14_IRQHandler

        PUBWEAK DMA15_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA15_IRQHandler
        B DMA15_IRQHandler

        PUBWEAK DMA_Error_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA_Error_IRQHandler
        B DMA_Error_IRQHandler

        PUBWEAK Reserved21_IRQHandler
        SECTION .text:CODE:REORDER(1)
Reserved21_IRQHandler
        B Reserved21_IRQHandler

        PUBWEAK FTFL_IRQHandler
        SECTION .text:CODE:REORDER(1)
FTFL_IRQHandler
        B FTFL_IRQHandler

        PUBWEAK Read_Collision_IRQHandler
        SECTION .text:CODE:REORDER(1)
Read_Collision_IRQHandler
        B Read_Collision_IRQHandler

        PUBWEAK LVD_LVW_IRQHandler
        SECTION .text:CODE:REORDER(1)
LVD_LVW_IRQHandler
        B LVD_LVW_IRQHandler

        PUBWEAK LLW_IRQHandler
        SECTION .text:CODE:REORDER(1)
LLW_IRQHandler
        B LLW_IRQHandler

        PUBWEAK Watchdog_IRQHandler
        SECTION .text:CODE:REORDER(1)
Watchdog_IRQHandler
        B Watchdog_IRQHandler

        PUBWEAK RNG_IRQHandler
        SECTION .text:CODE:REORDER(1)
RNG_IRQHandler
        B RNG_IRQHandler

        PUBWEAK I2C0_IRQHandler
        SECTION .text:CODE:REORDER(1)
I2C0_IRQHandler
        B I2C0_IRQHandler

        PUBWEAK I2C1_IRQHandler
        SECTION .text:CODE:REORDER(1)
I2C1_IRQHandler
        B I2C1_IRQHandler

        PUBWEAK SPI0_IRQHandler
        SECTION .text:CODE:REORDER(1)
SPI0_IRQHandler
        B SPI0_IRQHandler

        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:REORDER(1)
SPI1_IRQHandler
        B SPI1_IRQHandler

        PUBWEAK SPI2_IRQHandler
        SECTION .text:CODE:REORDER(1)
SPI2_IRQHandler
        B SPI2_IRQHandler

        PUBWEAK CAN0_ORed_Message_buffer_IRQHandler
        SECTION .text:CODE:REORDER(1)   
CAN0_ORed_Message_buffer_IRQHandler
        B CAN0_ORed_Message_buffer_IRQHandler

        PUBWEAK CAN0_Bus_Off_IRQHandler
        SECTION .text:CODE:REORDER(1)  
CAN0_Bus_Off_IRQHandler
        B CAN0_Bus_Off_IRQHandler

        PUBWEAK CAN0_Error_IRQHandler
        SECTION .text:CODE:REORDER(1)  
CAN0_Error_IRQHandler
        B CAN0_Error_IRQHandler

        PUBWEAK CAN0_Tx_Warning_IRQHandler
        SECTION .text:CODE:REORDER(1)    
CAN0_Tx_Warning_IRQHandler
        B CAN0_Tx_Warning_IRQHandler

        PUBWEAK CAN0_Rx_Warning_IRQHandler
        SECTION .text:CODE:REORDER(1)   
CAN0_Rx_Warning_IRQHandler
        B CAN0_Rx_Warning_IRQHandler

        PUBWEAK CAN0_Wake_Up_IRQHandler
        SECTION .text:CODE:REORDER(1)   
CAN0_Wake_Up_IRQHandler
        B CAN0_Wake_Up_IRQHandler

        PUBWEAK I2S0_Tx_IRQHandler
        SECTION .text:CODE:REORDER(1)     
I2S0_Tx_IRQHandler
        B I2S0_Tx_IRQHandler

        PUBWEAK I2S0_Rx_IRQHandler
        SECTION .text:CODE:REORDER(1)     
I2S0_Rx_IRQHandler
        B I2S0_Rx_IRQHandler

        PUBWEAK CAN1_ORed_Message_buffer_IRQHandler
        SECTION .text:CODE:REORDER(1)     
CAN1_ORed_Message_buffer_IRQHandler
        B CAN1_ORed_Message_buffer_IRQHandler

        PUBWEAK CAN1_Bus_Off_IRQHandler
        SECTION .text:CODE:REORDER(1)   
CAN1_Bus_Off_IRQHandler
        B CAN1_Bus_Off_IRQHandler

        PUBWEAK CAN1_Error_IRQHandler
        SECTION .text:CODE:REORDER(1)   
CAN1_Error_IRQHandler
        B CAN1_Error_IRQHandler

        PUBWEAK CAN1_Tx_Warning_IRQHandler
        SECTION .text:CODE:REORDER(1)      
CAN1_Tx_Warning_IRQHandler
        B CAN1_Tx_Warning_IRQHandler

        PUBWEAK CAN1_Rx_Warning_IRQHandler
        SECTION .text:CODE:REORDER(1)      
CAN1_Rx_Warning_IRQHandler
        B CAN1_Rx_Warning_IRQHandler

        PUBWEAK CAN1_Wake_Up_IRQHandler
        SECTION .text:CODE:REORDER(1)        
CAN1_Wake_Up_IRQHandler
        B CAN1_Wake_Up_IRQHandler

        PUBWEAK Reserved59_IRQHandler
        SECTION .text:CODE:REORDER(1)     
Reserved59_IRQHandler
        B Reserved59_IRQHandler

        PUBWEAK UART0_LON_IRQHandler
        SECTION .text:CODE:REORDER(1)     
UART0_LON_IRQHandler
        B UART0_LON_IRQHandler

        PUBWEAK UART0_RX_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)     
UART0_RX_TX_IRQHandler
        B UART0_RX_TX_IRQHandler

        PUBWEAK UART0_ERR_IRQHandler
        SECTION .text:CODE:REORDER(1)        
UART0_ERR_IRQHandler
        B UART0_ERR_IRQHandler

        PUBWEAK UART1_RX_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)       
UART1_RX_TX_IRQHandler
        B UART1_RX_TX_IRQHandler

        PUBWEAK UART1_ERR_IRQHandler
        SECTION .text:CODE:REORDER(1)   
UART1_ERR_IRQHandler
        B UART1_ERR_IRQHandler

        PUBWEAK UART2_RX_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)      
UART2_RX_TX_IRQHandler
        B UART2_RX_TX_IRQHandler

        PUBWEAK UART2_ERR_IRQHandler
        SECTION .text:CODE:REORDER(1)      
UART2_ERR_IRQHandler
        B UART2_ERR_IRQHandler

        PUBWEAK UART3_RX_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)    
UART3_RX_TX_IRQHandler
        B UART3_RX_TX_IRQHandler

        PUBWEAK UART3_ERR_IRQHandler
        SECTION .text:CODE:REORDER(1)          
UART3_ERR_IRQHandler
        B UART3_ERR_IRQHandler

        PUBWEAK UART4_RX_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)           
UART4_RX_TX_IRQHandler
        B UART4_RX_TX_IRQHandler

        PUBWEAK UART4_ERR_IRQHandler
        SECTION .text:CODE:REORDER(1)       
UART4_ERR_IRQHandler
        B UART4_ERR_IRQHandler

        PUBWEAK UART5_RX_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)      
UART5_RX_TX_IRQHandler
        B UART5_RX_TX_IRQHandler

        PUBWEAK UART5_ERR_IRQHandler
        SECTION .text:CODE:REORDER(1)         
UART5_ERR_IRQHandler
        B UART5_ERR_IRQHandler

        PUBWEAK ADC0_IRQHandler
        SECTION .text:CODE:REORDER(1)        
ADC0_IRQHandler
        B ADC0_IRQHandler

        PUBWEAK ADC1_IRQHandler
        SECTION .text:CODE:REORDER(1)          
ADC1_IRQHandler
        B ADC1_IRQHandler

        PUBWEAK CMP0_IRQHandler
        SECTION .text:CODE:REORDER(1)           
CMP0_IRQHandler
        B CMP0_IRQHandler

        PUBWEAK CMP1_IRQHandler
        SECTION .text:CODE:REORDER(1)             
CMP1_IRQHandler
        B CMP1_IRQHandler

        PUBWEAK CMP2_IRQHandler
        SECTION .text:CODE:REORDER(1)           
CMP2_IRQHandler
        B CMP2_IRQHandler

        PUBWEAK FTM0_IRQHandler
        SECTION .text:CODE:REORDER(1)          
FTM0_IRQHandler
        B FTM0_IRQHandler

        PUBWEAK FTM1_IRQHandler
        SECTION .text:CODE:REORDER(1)               
FTM1_IRQHandler
        B FTM1_IRQHandler

        PUBWEAK FTM2_IRQHandler
        SECTION .text:CODE:REORDER(1)           
FTM2_IRQHandler
        B FTM2_IRQHandler

        PUBWEAK CMT_IRQHandler
        SECTION .text:CODE:REORDER(1)           
CMT_IRQHandler
        B CMT_IRQHandler

        PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:REORDER(1)           
RTC_IRQHandler
        B RTC_IRQHandler

        PUBWEAK RTC_Seconds_IRQHandler
        SECTION .text:CODE:REORDER(1)  
RTC_Seconds_IRQHandler
        B RTC_Seconds_IRQHandler

        PUBWEAK PIT0_IRQHandler
        SECTION .text:CODE:REORDER(1)      
PIT0_IRQHandler
        B PIT0_IRQHandler

        PUBWEAK PIT1_IRQHandler
        SECTION .text:CODE:REORDER(1)   
PIT1_IRQHandler
        B PIT1_IRQHandler

        PUBWEAK PIT2_IRQHandler
        SECTION .text:CODE:REORDER(1)         
PIT2_IRQHandler
        B PIT2_IRQHandler

        PUBWEAK PIT3_IRQHandler
        SECTION .text:CODE:REORDER(1)   
PIT3_IRQHandler
        B PIT3_IRQHandler

        PUBWEAK PDB0_IRQHandler
        SECTION .text:CODE:REORDER(1)  
PDB0_IRQHandler
        B PDB0_IRQHandler

        PUBWEAK Reserved51_IRQHandler
        SECTION .text:CODE:REORDER(1)   
Reserved51_IRQHandler
        B Reserved51_IRQHandler

        PUBWEAK Reserved52_IRQHandler
        SECTION .text:CODE:REORDER(1)       
Reserved52_IRQHandler
        B Reserved52_IRQHandler

        PUBWEAK ENET_1588_Timer_IRQHandler
        SECTION .text:CODE:REORDER(1)         
ENET_1588_Timer_IRQHandler
        B ENET_1588_Timer_IRQHandler

        PUBWEAK ENET_Transmit_IRQHandler
        SECTION .text:CODE:REORDER(1)             
ENET_Transmit_IRQHandler
        B ENET_Transmit_IRQHandler

        PUBWEAK ENET_Receive_IRQHandler
        SECTION .text:CODE:REORDER(1)         
ENET_Receive_IRQHandler
        B ENET_Receive_IRQHandler

        PUBWEAK ENET_Error_IRQHandler
        SECTION .text:CODE:REORDER(1)        
ENET_Error_IRQHandler
        B ENET_Error_IRQHandler

        PUBWEAK Reserved95_IRQHandler
        SECTION .text:CODE:REORDER(1)      
Reserved95_IRQHandler
        B Reserved95_IRQHandler

        PUBWEAK SDHC_IRQHandler
        SECTION .text:CODE:REORDER(1)         
SDHC_IRQHandler
        B SDHC_IRQHandler

        PUBWEAK DAC0_IRQHandler
        SECTION .text:CODE:REORDER(1)         
DAC0_IRQHandler
        B DAC0_IRQHandler

        PUBWEAK DAC1_IRQHandler
        SECTION .text:CODE:REORDER(1)          
DAC1_IRQHandler
        B DAC1_IRQHandler

        PUBWEAK TSI0_IRQHandler
        SECTION .text:CODE:REORDER(1)         
TSI0_IRQHandler
        B TSI0_IRQHandler

        PUBWEAK MCG_IRQHandler
        SECTION .text:CODE:REORDER(1)           
MCG_IRQHandler
        B MCG_IRQHandler

        PUBWEAK LPTimer_IRQHandler
        SECTION .text:CODE:REORDER(1)       
LPTimer_IRQHandler
        B LPTimer_IRQHandler

        PUBWEAK Reserved102_IRQHandler
        SECTION .text:CODE:REORDER(1)          
Reserved102_IRQHandler
        B Reserved102_IRQHandler

        PUBWEAK PORTA_IRQHandler
        SECTION .text:CODE:REORDER(1)            
PORTA_IRQHandler
        B PORTA_IRQHandler

        PUBWEAK PORTB_IRQHandler
        SECTION .text:CODE:REORDER(1)        
PORTB_IRQHandler
        B PORTB_IRQHandler

        PUBWEAK PORTC_IRQHandler
        SECTION .text:CODE:REORDER(1)         
PORTC_IRQHandler
        B PORTC_IRQHandler

        PUBWEAK PORTD_IRQHandler
        SECTION .text:CODE:REORDER(1)           
PORTD_IRQHandler
        B PORTD_IRQHandler

        PUBWEAK PORTE_IRQHandler
        SECTION .text:CODE:REORDER(1)          
PORTE_IRQHandler
        B PORTE_IRQHandler

        PUBWEAK Reserved108_IRQHandler
        SECTION .text:CODE:REORDER(1)     
Reserved108_IRQHandler
        B Reserved108_IRQHandler

        PUBWEAK Reserved109_IRQHandler
        SECTION .text:CODE:REORDER(1)      
Reserved109_IRQHandler
        B Reserved109_IRQHandler

        PUBWEAK SWI_IRQHandler
        SECTION .text:CODE:REORDER(1)        
SWI_IRQHandler
        B SWI_IRQHandler

        PUBWEAK Reserved111_IRQHandler
        SECTION .text:CODE:REORDER(1)           
Reserved111_IRQHandler
        B Reserved111_IRQHandler

        PUBWEAK Reserved112_IRQHandler
        SECTION .text:CODE:REORDER(1)   
Reserved112_IRQHandler
        B Reserved112_IRQHandler

        PUBWEAK Reserved113_IRQHandler
        SECTION .text:CODE:REORDER(1)  
Reserved113_IRQHandler
        B Reserved113_IRQHandler

        PUBWEAK Reserved114_IRQHandler
        SECTION .text:CODE:REORDER(1)       
Reserved114_IRQHandler
        B Reserved114_IRQHandler

        PUBWEAK Reserved115_IRQHandler
        SECTION .text:CODE:REORDER(1)       
Reserved115_IRQHandler
        B Reserved115_IRQHandler

        PUBWEAK Reserved116_IRQHandler
        SECTION .text:CODE:REORDER(1)      
Reserved116_IRQHandler
        B Reserved116_IRQHandler

        PUBWEAK Reserved117_IRQHandler
        SECTION .text:CODE:REORDER(1)          
Reserved117_IRQHandler
        B Reserved117_IRQHandler

        PUBWEAK Reserved118_IRQHandler
        SECTION .text:CODE:REORDER(1)         
Reserved118_IRQHandler
        B Reserved118_IRQHandler

        PUBWEAK Reserved119_IRQHandler
        SECTION .text:CODE:REORDER(1)   
Reserved119_IRQHandler
        B Reserved119_IRQHandler

        PUBWEAK DefaultISR
        SECTION .text:CODE:REORDER(1)       
DefaultISR
        B DefaultISR

        END
