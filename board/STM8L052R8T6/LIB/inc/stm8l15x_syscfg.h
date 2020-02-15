/**
  ******************************************************************************
  * @file    stm8l15x_syscfg.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the SYSCFG firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************  
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_SYSCFG_H
#define __STM8L15x_SYSCFG_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @addtogroup SYSCFG
  * @{
  */ 
/* Exported types ------------------------------------------------------------*/
/** @defgroup SYSCFG_Exported_Types
  * @{
  */

/** @defgroup RI_Input_Capture
  * @{
  */
typedef enum
{
  RI_InputCapture_IC2 = ((uint8_t) 0x02), /*!< TIM1 Input Capture 2 is routed */
  RI_InputCapture_IC3 = ((uint8_t) 0x03)  /*!< TIM1 Input Capture 3 is routed */
}RI_InputCapture_TypeDef;

/**
  * @}
  */

/** @defgroup RI_Input_Capture_Routing
  * @{
  */
typedef enum
{
  RI_InputCaptureRouting_0  = ((uint8_t) 0x00), /*!< TIM1 IC2 is routed to PD4, IC3 to PD5 */
  RI_InputCaptureRouting_1  = ((uint8_t) 0x01), /*!< TIM1 IC2 is routed to PF0, IC3 to PF1 */
  RI_InputCaptureRouting_2  = ((uint8_t) 0x02), /*!< TIM1 IC2 is routed to PF2, IC3 to PF3 */
  RI_InputCaptureRouting_3  = ((uint8_t) 0x03), /*!< TIM1 IC2 is routed to PE0, IC3 to PE1 */
  RI_InputCaptureRouting_4  = ((uint8_t) 0x04), /*!< TIM1 IC2 is routed to PE2, IC3 to PE3 */
  RI_InputCaptureRouting_5  = ((uint8_t) 0x05), /*!< TIM1 IC2 is routed to PE4, IC3 to PE5 */
  RI_InputCaptureRouting_6  = ((uint8_t) 0x06), /*!< TIM1 IC2 is routed to PE6, IC3 to PE7 */
  RI_InputCaptureRouting_7  = ((uint8_t) 0x07), /*!< TIM1 IC2 is routed to PD0, IC3 to PD1 */
  RI_InputCaptureRouting_8  = ((uint8_t) 0x08), /*!< TIM1 IC2 is routed to PD2, IC3 to PD3 */
  RI_InputCaptureRouting_9  = ((uint8_t) 0x09), /*!< TIM1 IC2 is routed to PD4, IC3 to PD5 */
  RI_InputCaptureRouting_10 = ((uint8_t) 0x0A), /*!< TIM1 IC2 is routed to PD6, IC3 to PD7 */
  RI_InputCaptureRouting_11 = ((uint8_t) 0x0B), /*!< TIM1 IC2 is routed to PC0, IC3 to PC1 */
  RI_InputCaptureRouting_12 = ((uint8_t) 0x0C), /*!< TIM1 IC2 is routed to PC2, IC3 to PC3 */
  RI_InputCaptureRouting_13 = ((uint8_t) 0x0D), /*!< TIM1 IC2 is routed to PC4, IC3 to PC5 */
  RI_InputCaptureRouting_14 = ((uint8_t) 0x0E), /*!< TIM1 IC2 is routed to PC6, IC3 to PC7 */
  RI_InputCaptureRouting_15 = ((uint8_t) 0x0F), /*!< TIM1 IC2 is routed to PB0, IC3 to PB1 */
  RI_InputCaptureRouting_16 = ((uint8_t) 0x10), /*!< TIM1 IC2 is routed to PB2, IC3 to PB3 */
  RI_InputCaptureRouting_17 = ((uint8_t) 0x11), /*!< TIM1 IC2 is routed to PB4, IC3 to PB5 */
  RI_InputCaptureRouting_18 = ((uint8_t) 0x12), /*!< TIM1 IC2 is routed to PB6, IC3 to PB7 */
  RI_InputCaptureRouting_19 = ((uint8_t) 0x13), /*!< TIM1 IC2 is routed to PA0, IC3 to PA2 */
  RI_InputCaptureRouting_20 = ((uint8_t) 0x14), /*!< TIM1 IC2 is routed to PA3, IC3 to PA4 */
  RI_InputCaptureRouting_21 = ((uint8_t) 0x15), /*!< TIM1 IC2 is routed to PA5, IC3 to PA6 */
  RI_InputCaptureRouting_22 = ((uint8_t) 0x16)  /*!< TIM1 IC2 is routed to PA7, IC3 to PD5 */
}RI_InputCaptureRouting_TypeDef;

/**
  * @}
  */

/** @defgroup RI_Analog_Switch
  * @{
  */
/**
  * @brief Definition of the Analog Switch to be controlled.
  * Values are coded in 0xXY format where
  * X: the register index (1: RI_ASCR1, 2: RI_ASCR2)
  * Y: the bit position which corresponds with the Analog Switch
  */
typedef enum
{
  RI_AnalogSwitch_0  = ((uint8_t) 0x10), /*!< Analog switch 0 */
  RI_AnalogSwitch_1  = ((uint8_t) 0x11), /*!< Analog switch 1 */
  RI_AnalogSwitch_2  = ((uint8_t) 0x12), /*!< Analog switch 2 */
  RI_AnalogSwitch_3  = ((uint8_t) 0x13), /*!< Analog switch 3 */
  RI_AnalogSwitch_4  = ((uint8_t) 0x14), /*!< Analog switch 4 */
  RI_AnalogSwitch_5  = ((uint8_t) 0x15), /*!< Analog switch 5 */
  RI_AnalogSwitch_6  = ((uint8_t) 0x16), /*!< Analog switch 6 */
  RI_AnalogSwitch_7  = ((uint8_t) 0x17), /*!< Analog switch 7 */
  RI_AnalogSwitch_8  = ((uint8_t) 0x20), /*!< Analog switch 8 */
  RI_AnalogSwitch_9  = ((uint8_t) 0x21), /*!< Analog switch 9 */
  RI_AnalogSwitch_10 = ((uint8_t) 0x22), /*!< Analog switch 10 */
  RI_AnalogSwitch_11 = ((uint8_t) 0x23), /*!< Analog switch 11 */
  RI_AnalogSwitch_14 = ((uint8_t) 0x26)  /*!< Analog switch 14 */
}RI_AnalogSwitch_TypeDef;

/**
  * @}
  */

/** @defgroup RI_IO_Switch
  * @{
  */
/**
  * @brief Definition of the I/O Switch to be controlled.
  * Values are coded in 0xXY format where
  * X: the register index (1: RI_IOSR1, 2: RI_IOSR2, 3: RI_IOSR3 or 4: RI_IOSR4)
  * Y: the bit index of the Input Output Switch in RI_IOSRx register
  */
typedef enum
{
  RI_IOSwitch_1   = ((uint8_t) 0x10),  /*!< Input Output Switch switch 1  */
  RI_IOSwitch_2   = ((uint8_t) 0x20),  /*!< Input Output Switch switch 2  */
  RI_IOSwitch_3   = ((uint8_t) 0x30),  /*!< Input Output Switch switch 3  */
  RI_IOSwitch_4   = ((uint8_t) 0x11),  /*!< Input Output Switch switch 4  */
  RI_IOSwitch_5   = ((uint8_t) 0x21),  /*!< Input Output Switch switch 4  */
  RI_IOSwitch_6   = ((uint8_t) 0x31),  /*!< Input Output Switch switch 6  */
  RI_IOSwitch_7   = ((uint8_t) 0x12),  /*!< Input Output Switch switch 7  */
  RI_IOSwitch_8   = ((uint8_t) 0x22),  /*!< Input Output Switch switch 8  */
  RI_IOSwitch_9   = ((uint8_t) 0x32),  /*!< Input Output Switch switch 9  */
  RI_IOSwitch_10  = ((uint8_t) 0x13),  /*!< Input Output Switch switch 10 */
  RI_IOSwitch_11  = ((uint8_t) 0x23),  /*!< Input Output Switch switch 11 */
  RI_IOSwitch_12  = ((uint8_t) 0x33),  /*!< Input Output Switch switch 12 */
  RI_IOSwitch_13  = ((uint8_t) 0x14),  /*!< Input Output Switch switch 13 */
  RI_IOSwitch_14  = ((uint8_t) 0x24),  /*!< Input Output Switch switch 14 */
  RI_IOSwitch_15  = ((uint8_t) 0x34),  /*!< Input Output Switch switch 15 */
  RI_IOSwitch_16  = ((uint8_t) 0x15),  /*!< Input Output Switch switch 16 */
  RI_IOSwitch_17  = ((uint8_t) 0x25),  /*!< Input Output Switch switch 17 */
  RI_IOSwitch_18  = ((uint8_t) 0x35),  /*!< Input Output Switch switch 18 */
  RI_IOSwitch_19  = ((uint8_t) 0x16),  /*!< Input Output Switch switch 19 */
  RI_IOSwitch_20  = ((uint8_t) 0x26),  /*!< Input Output Switch switch 20 */
  RI_IOSwitch_21  = ((uint8_t) 0x36),  /*!< Input Output Switch switch 21 */
  RI_IOSwitch_22  = ((uint8_t) 0x17),  /*!< Input Output Switch switch 22 */
  RI_IOSwitch_23  = ((uint8_t) 0x27),  /*!< Input Output Switch switch 23 */
  RI_IOSwitch_24  = ((uint8_t) 0x37),  /*!< Input Output Switch switch 24 */
  RI_IOSwitch_26  = ((uint8_t) 0x41),  /*!< Input Output Switch switch 26 */
  RI_IOSwitch_27  = ((uint8_t) 0x46),  /*!< Input Output Switch switch 27 */
  RI_IOSwitch_28  = ((uint8_t) 0x47),  /*!< Input Output Switch switch 28 */
  RI_IOSwitch_29  = ((uint8_t) 0x40)   /*!< Input Output Switch switch 29 */
}RI_IOSwitch_TypeDef;

/**
  * @}
  */

/** @defgroup RI_Resistor
  * @{
  */
/**
  * @brief Definition of the pull-up and pull-down resistors for COMP1 and ADC.
  */
typedef enum
{
  RI_Resistor_10KPU  = ((uint8_t) 0x01),
  RI_Resistor_400KPU = ((uint8_t) 0x02),
  RI_Resistor_10KPD  = ((uint8_t) 0x04),
  RI_Resistor_400KPD = ((uint8_t) 0x08)
}RI_Resistor_TypeDef;

/**
  * @}
  */

/** @defgroup REMAP_Pin
  * @{
  */
/**
  * @brief Definition of the REMAP pins.
  * Elements values convention: 0xXY
  *  X = RMPCRx registers index
  *      X = 0x01 : RMPCR1
  *      X = 0x02 : RMPCR2
  *      X = 0x03 : RMPCR3
  *  Y = Mask for setting/resetting bits in RMPCRx register
  */
typedef enum
{
  /* RMPCR1 register bits */
  REMAP_Pin_USART1TxRxPortA = ((uint16_t)0x011C), /*!< USART1 Tx- Rx (PC3- PC2) remapping to PA2- PA3 */
  REMAP_Pin_USART1TxRxPortC = ((uint16_t)0x012C), /*!< USART1 Tx- Rx (PC3- PC2) remapping to PC5- PC6 */
  REMAP_Pin_USART1Clk       = ((uint16_t)0x014B), /*!< USART1 CK (PC4) remapping to PA0 */
  REMAP_Pin_SPI1Full        = ((uint16_t)0x0187), /*!< SPI1 MISO- MOSI- SCK- NSS(PB7- PB6- PB5- PB4)
                                                       remapping to PA2- PA3- PC6- PC5 */
  /* RMPCR2 register bits */
  REMAP_Pin_ADC1ExtTRIG1    = ((uint16_t)0x0201), /*!< ADC1 External Trigger 1 (PA6) remapping to PD0 */
  REMAP_Pin_TIM2TRIGPortA   = ((uint16_t)0x0202), /*!< TIM2 Trigger (PB3) remapping to PA4 */
  REMAP_Pin_TIM3TRIGPortA   = ((uint16_t)0x0204), /*!< TIM3 Trigger (PD1) remapping to PA5 */
  REMAP_Pin_TIM2TRIGLSE     = ((uint16_t)0x0208), /*!< TIM2 Trigger remapping to LSE */
  REMAP_Pin_TIM3TRIGLSE     = ((uint16_t)0x0210), /*!< TIM3 Trigger remapping to LSE */
  REMAP_Pin_SPI2Full        = ((uint16_t)0x0220), /*!< SPI2 MISO- MOSI- SCK- NSS(PG7- PG6- PG5- PG4)
                                                       remapping to PI3- PI2- PI1- PI0 */
  REMAP_Pin_TIM3TRIGPortG   = ((uint16_t)0x0240), /*!< TIM3 Trigger (PD1) remapping to PG3 */
  REMAP_Pin_TIM23BKIN       = ((uint16_t)0x0280), /*!< TIM2 Break Input (PA4) remapping to PG0
                                                           and TIM3 Break Input (PA5) remapping to PG1 */
  /* RMPCR3 register bits */
  REMAP_Pin_SPI1PortF       = ((uint16_t)0x0301), /*!< SPI1 MISO- MOSI- SCK- NSS(PB7- PB6- PB5- PB4)
                                                       remapping to PF0- PF1- PF2- PF3   */
  REMAP_Pin_USART3TxRxPortF = ((uint16_t)0x0302), /*!< USART3 Tx- Rx (PG1- PG0) remapping to PF0- PF1 */
  REMAP_Pin_USART3Clk       = ((uint16_t)0x0304), /*!< USART3 CK (PG2) remapping to PF2 */
  REMAP_Pin_TIM3Channel1    = ((uint16_t)0x0308), /*!< TIM3 Channel 1 (PB1) remapping to PI0 */
  REMAP_Pin_TIM3Channel2    = ((uint16_t)0x0310), /*!< TIM3 Channel 2 (PD0) remapping to PI3 */
  REMAP_Pin_CCO             = ((uint16_t)0x0320), /*!< CCO (PC4) remapping to PE2 */
  REMAP_Pin_TIM2Channel1    = ((uint16_t)0x0340), /*!< TIM2 Channel 1 (PB0) remapping to PC5 */
  REMAP_Pin_TIM2Channel2    = ((uint16_t)0x0380)  /*!< TIM2 Channel 2 (PB2) remapping to PC6 */
}REMAP_Pin_TypeDef;

/**
  * @}
  */

/** @defgroup REMAP_DMA_Channel
  * @{
  */
typedef enum
{
  REMAP_DMA1Channel_ADC1ToChannel0  = ((uint8_t)0x00), /*!< ADC1 DMA1 req/ack mapped on DMA1 channel 0 */
  REMAP_DMA1Channel_ADC1ToChannel1  = ((uint8_t)0x01), /*!< ADC1 DMA1 req/ack mapped on DMA1 channel 1 */
  REMAP_DMA1Channel_ADC1ToChannel2  = ((uint8_t)0x02), /*!< ADC1 DMA1 req/ack mapped on DMA1 channel 2 */
  REMAP_DMA1Channel_ADC1ToChannel3  = ((uint8_t)0x03), /*!< ADC1 DMA1 req/ack mapped on DMA1 channel 3 */
  REMAP_DMA1Channel_TIM4ToChannel0  = ((uint8_t)0xF0), /*!< TIM4 DMA1 req/ack mapped on DMA1 channel 0 */
  REMAP_DMA1Channel_TIM4ToChannel1  = ((uint8_t)0xF4), /*!< TIM4 DMA1 req/ack mapped on DMA1 channel 1 */
  REMAP_DMA1Channel_TIM4ToChannel2  = ((uint8_t)0xF8), /*!< TIM4 DMA1 req/ack mapped on DMA1 channel 2 */
  REMAP_DMA1Channel_TIM4ToChannel3  = ((uint8_t)0xFC)  /*!< TIM4 DMA1 req/ack mapped on DMA1 channel 3 */
}REMAP_DMAChannel_TypeDef;

/**
  * @}
  */

/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/** @defgroup SYSCFG_Exported_Macros
  * @{
  */

/**
  * @brief Macro used by the assert function in order to check the different
  * values of @ref RI_InputCaptureTypeDef enum.
  */
#define IS_RI_INPUTCAPTURE(RI_IC) (((RI_IC) == RI_InputCapture_IC2) || \
                                   ((RI_IC) == RI_InputCapture_IC3))

/**
  * @brief Macro used by the assert function in order to check the different
  * values of @ref RI_InputCaptureRoutingTypeDef enum.
  */
#define IS_RI_INPUTCAPTUREROUTING(RI_IC_ROUTING) (((RI_IC_ROUTING) == RI_InputCaptureRouting_0) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_1) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_2) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_3) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_4) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_5) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_6) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_7) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_8) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_9) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_10) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_11) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_12) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_13) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_14) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_15) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_16) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_17) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_18) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_19) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_20) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_21) || \
    ((RI_IC_ROUTING) == RI_InputCaptureRouting_22))

/**
  * @brief Macro used by the assert function in order to check the different
  * values of @ref RI_AnalogSwitch_TypeDef enum.
  */
#define IS_RI_ANALOGSWITCH(RI_ANALOGSWITCH) (((RI_ANALOGSWITCH) == RI_AnalogSwitch_0) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_1) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_2) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_3) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_4) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_5) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_6) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_7) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_8) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_9) || \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_10)|| \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_11)|| \
    ((RI_ANALOGSWITCH) == RI_AnalogSwitch_14))

/**
  * @brief Macro used by the assert function in order to check the different
  * values of @ref RI_IOSwitch_TypeDef enum.
  */
#define IS_RI_IOSWITCH(RI_IOSWITCH) (((RI_IOSWITCH) == RI_IOSwitch_1) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_2) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_3) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_4) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_5) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_6) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_7) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_8) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_9) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_10) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_11) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_12) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_13) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_14) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_15) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_16) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_17) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_18) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_19) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_20) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_21) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_22) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_23) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_24) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_26) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_27) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_28) || \
                                     ((RI_IOSWITCH) == RI_IOSwitch_29))

/**
  * @brief Macro used by the assert function in order to check the different
  * values of @ref RI_ResistorTypeDef enum.
  */
#define IS_RI_RESISTOR(RI_RESISTOR) (((RI_RESISTOR) ==  RI_Resistor_10KPU)  || \
                                     ((RI_RESISTOR) ==  RI_Resistor_400KPU) || \
                                     ((RI_RESISTOR) ==  RI_Resistor_10KPD)  || \
                                     ((RI_RESISTOR) ==  RI_Resistor_400KPD))
/**
  * @brief Macro used by the assert function in order to check the different
  * values of @ref REMAP_Pin_TypeDef enum.
  */
#define IS_REMAP_PIN(PIN) (((PIN) == REMAP_Pin_USART1TxRxPortA) || \
                           ((PIN) == REMAP_Pin_USART1TxRxPortC) || \
                           ((PIN) == REMAP_Pin_USART1Clk) || \
                           ((PIN) == REMAP_Pin_SPI1Full) || \
                           ((PIN) == REMAP_Pin_ADC1ExtTRIG1) || \
                           ((PIN) == REMAP_Pin_TIM2TRIGPortA) || \
                           ((PIN) == REMAP_Pin_TIM3TRIGPortA) || \
                           ((PIN) == REMAP_Pin_TIM2TRIGLSE) || \
                           ((PIN) == REMAP_Pin_TIM3TRIGLSE) || \
                           ((PIN) == REMAP_Pin_SPI2Full) || \
                           ((PIN) == REMAP_Pin_TIM3TRIGPortG) || \
                           ((PIN) == REMAP_Pin_TIM23BKIN) || \
                           ((PIN) == REMAP_Pin_SPI1PortF) || \
                           ((PIN) == REMAP_Pin_USART3TxRxPortF) || \
                           ((PIN) == REMAP_Pin_USART3Clk) || \
                           ((PIN) == REMAP_Pin_TIM3Channel1) || \
                           ((PIN) == REMAP_Pin_TIM3Channel2) || \
                           ((PIN) == REMAP_Pin_CCO) || \
                           ((PIN) == REMAP_Pin_TIM2Channel1) || \
                           ((PIN) == REMAP_Pin_TIM2Channel2))


/**
  * @brief Macro used by the assert function in order to check the different
  * values of the @ref REMAP_DMAChannel_TypeDef enum.
  */
#define IS_REMAP_DMACHANNEL(MAP) (((MAP) == REMAP_DMA1Channel_ADC1ToChannel0) || \
                                  ((MAP) == REMAP_DMA1Channel_ADC1ToChannel1) || \
                                  ((MAP) == REMAP_DMA1Channel_ADC1ToChannel2) || \
                                  ((MAP) == REMAP_DMA1Channel_ADC1ToChannel3) || \
                                  ((MAP) == REMAP_DMA1Channel_TIM4ToChannel0) || \
                                  ((MAP) == REMAP_DMA1Channel_TIM4ToChannel1) || \
                                  ((MAP) == REMAP_DMA1Channel_TIM4ToChannel2) || \
                                  ((MAP) == REMAP_DMA1Channel_TIM4ToChannel3))
/**
  * @}
  */
  
/* Exported functions ------------------------------------------------------- */
/* Routing Interface (RI) configuration ***************************************/
void SYSCFG_RIDeInit(void);
void SYSCFG_RITIMInputCaptureConfig(RI_InputCapture_TypeDef RI_InputCapture,
                                    RI_InputCaptureRouting_TypeDef RI_InputCaptureRouting);
void SYSCFG_RIAnalogSwitchConfig(RI_AnalogSwitch_TypeDef RI_AnalogSwitch,
                                 FunctionalState NewState);
void SYSCFG_RIIOSwitchConfig(RI_IOSwitch_TypeDef RI_IOSwitch, FunctionalState NewState);
void SYSCFG_RIResistorConfig(RI_Resistor_TypeDef RI_Resistor, FunctionalState NewState);

/* SYSCFG configuration *******************************************************/
void SYSCFG_REMAPDeInit(void);
void SYSCFG_REMAPPinConfig(REMAP_Pin_TypeDef REMAP_Pin, FunctionalState NewState);
void SYSCFG_REMAPDMAChannelConfig(REMAP_DMAChannel_TypeDef REMAP_DMAChannel);

#endif /* __STM8L15x_SYSCFG_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
