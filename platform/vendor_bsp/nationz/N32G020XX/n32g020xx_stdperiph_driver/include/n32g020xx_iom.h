/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32g020xx_iom.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief IO管理模块接口声明文件。
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup IOM IOM管理模块
 * @{
 */
#ifndef __N32G020XX_IOM_H__
#define __N32G020XX_IOM_H__
//------------------------------------------------------------------
//                        Headers
//------------------------------------------------------------------
#include "n32g020xx.h"
#include "n32g020xx_common.h"

//------------------------------------------------------------------
//                        Definitions
//------------------------------------------------------------------
#define IOM_SET_REG_MSK_VAL(reg, msk, val) SET_REG_BIT_MSK(reg, val, msk)

#define GPIO_MODE (0)

/******  GPIO0_Ctrl *********************************************************/
/*!< GPIO0 Mode Definition */
#define GPIO0_GPIO (GPIO_MODE)
#define GPIO0_PWM0 (1)
#define GPIO0_TM0_CH (2)
#define GPIO0_UART0_TX (3)
/*!< GPIO0 Mode Configuration */
#define IOM_Gpio0ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR1, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO1_Ctrl *********************************************************/
/*!< GPIO1 Mode Definition */
#define GPIO1_GPIO (GPIO_MODE)
#define GPIO1_PWM1 (1)
#define GPIO1_TM1_CH (2)
#define GPIO1_UART0_RX (3)
/*!< GPIO1 Mode Configuration */
#define IOM_Gpio1ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR1, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO2_Ctrl *********************************************************/
/*!< GPIO2 Mode Definition */
#define GPIO2_GPIO (GPIO_MODE)
#define GPIO2_PWM2 (1)
#define GPIO2_TM2_CH (2)
#define GPIO2_UART1_TX (3)
/*!< GPIO2 Mode Configuration */
#define IOM_Gpio2ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR2, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO3_Ctrl *********************************************************/
/*!< GPIO3 Mode Definition */
#define GPIO3_GPIO (GPIO_MODE)
#define GPIO3_PWM3 (1)
#define GPIO3_TM3_CH (2)
#define GPIO3_UART1_RX (3)
/*!< GPIO3 Mode Configuration */
#define IOM_Gpio3ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR2, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO4_Ctrl *********************************************************/
/*!< GPIO4 Mode Definition */
#define GPIO4_GPIO (GPIO_MODE)
#define GPIO4_PWM4 (1)
#define GPIO4_TM4_CH (2)
#define GPIO4_UART2_TX (3)
/*!< GPIO4 Mode Configuration */
#define IOM_Gpio4ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR3, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO5_Ctrl *********************************************************/
/*!< GPIO5 Mode Definition */
#define GPIO5_GPIO (GPIO_MODE)
#define GPIO5_PWM5 (1)
#define GPIO5_SCD_RST (2)
#define GPIO5_SCC_RST (3)
#define GPIO5_UART2_RX (4)
/*!< GPIO5 Mode Configuration */
#define IOM_Gpio5ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR3, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO6_Ctrl *********************************************************/
/*!< GPIO6 Mode Definition */
#define GPIO6_GPIO (GPIO_MODE)
#define GPIO6_PWM6 (1)
#define GPIO6_ADC_IN0 (7)
/*!< GPIO6 Mode Configuration */
#define IOM_Gpio6ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR4, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO7_Ctrl *********************************************************/
/*!< GPIO7 Mode Definition */
#define GPIO7_GPIO (GPIO_MODE)
#define GPIO7_PWM7 (1)
#define GPIO7_ADC_IN1 (7)
/*!< GPIO7 Mode Configuration */
#define IOM_Gpio7ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR4, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO8_Ctrl *********************************************************/
/*!< GPIO8 Mode Definition */
#define GPIO8_GPIO (GPIO_MODE)
#define GPIO8_SPIM0_CLK (1)
#define GPIO8_SPIS0_CLK (2)
#define GPIO8_ADC_IN2 (7)
/*!< GPIO8 Mode Configuration */
#define IOM_Gpio8ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR5, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO9_Ctrl *********************************************************/
/*!< GPIO9 Mode Definition */
#define GPIO9_GPIO (GPIO_MODE)
#define GPIO9_SPIM0_CSN (1)
#define GPIO9_SPIS0_CSN (2)
#define GPIO9_ADC_IN3 (7)
/*!< GPIO9 Mode Configuration */
#define IOM_Gpio9ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR5, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO10_Ctrl *********************************************************/
/*!< GPIO10 Mode Definition */
#define GPIO10_GPIO (GPIO_MODE)
#define GPIO10_SPIM0_MOSI (1)
#define GPIO10_SPIS0_MISO (2)
#define GPIO10_ADC_IN4 (7)
/*!< GPIO10 Mode Configuration */
#define IOM_Gpio10ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR6, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO11_Ctrl *********************************************************/
/*!< GPIO11 Mode Definition */
#define GPIO11_GPIO (GPIO_MODE)
#define GPIO11_SPIM0_MISO (1)
#define GPIO11_SPIS0_MOSI (2)
#define GPIO11_ADC_IN5 (7)
/*!< GPIO11 Mode Configuration */
#define IOM_Gpio11ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR6, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO12_Ctrl *********************************************************/
/*!< GPIO12 Mode Definition */
#define GPIO12_GPIO (GPIO_MODE)
#define GPIO12_SPIM0_IO2 (1)
#define GPIO12_UART1_RTS (2)
#define GPIO12_TM0_CH (3)
#define GPIO12_ADC_IN6 (7)
/*!< GPIO12 Mode Configuration */
#define IOM_Gpio12ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR7, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO13_Ctrl *********************************************************/
/*!< GPIO13 Mode Definition */
#define GPIO13_GPIO (GPIO_MODE)
#define GPIO13_SPIM0_IO3 (1)
#define GPIO13_UART1_CTS (2)
#define GPIO13_TM1_CH (3)
#define GPIO13_ADC_IN7 (7)
/*!< GPIO13 Mode Configuration */
#define IOM_Gpio13ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR7, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO14_Ctrl *********************************************************/
/*!< GPIO14 Mode Definition */
#define GPIO14_GPIO (GPIO_MODE)
#define GPIO14_SPIM1_CLK (1)
#define GPIO14_SPIS1_CLK (2)
/*!< GPIO14 Mode Configuration */
#define IOM_Gpio14ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR8, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO15_Ctrl *********************************************************/
/*!< GPIO15 Mode Definition */
#define GPIO15_GPIO (GPIO_MODE)
#define GPIO15_SPIM1_CSN (1)
#define GPIO15_SPIS1_CSN (2)
#define GPIO15_COMP_OUT (3)
/*!< GPIO15 Mode Configuration */
#define IOM_Gpio15ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR8, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO16_Ctrl *********************************************************/
/*!< GPIO16 Mode Definition */
#define GPIO16_GPIO (GPIO_MODE)
#define GPIO16_SPIM1_MISO (1)
#define GPIO16_SPIS1_MOSI (2)
/*!< GPIO16 Mode Configuration */
#define IOM_Gpio16ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR9, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO17_Ctrl *********************************************************/
/*!< GPIO17 Mode Definition */
#define GPIO17_GPIO (GPIO_MODE)
#define GPIO17_SPIM1_MOSI (1)
#define GPIO17_SPIS1_MISO (2)
#define GPIO17_USBPHY_COMP_OUT (3)
/*!< GPIO17 Mode Configuration */
#define IOM_Gpio17ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR9, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO18_Ctrl *********************************************************/
/*!< GPIO18 Mode Definition */
#define GPIO18_GPIO (GPIO_MODE)
#define GPIO18_I2C0_MS_CLK (1)
#define GPIO18_TM2_CH (2)
#define GPIO18_UART1_TX (3)
#define GPIO18_ADUSBRXDP_TEST_OUT (4)
#define GPIO18_12MCRYSTAL_TEST_OUT (5)
/*!< GPIO18 Mode Configuration */
#define IOM_Gpio18ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR10, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO19_Ctrl *********************************************************/
/*!< GPIO19 Mode Definition */
#define GPIO19_GPIO (GPIO_MODE)
#define GPIO19_I2C0_MS_DA (1)
#define GPIO19_UART1_RX (3)
#define GPIO19_TM3_CH (4)
#define GPIO19_ADUSBRXDM_TEST_OUT (5)
#define GPIO19_XO32K_RC32K_TEST_OUT (6)
/*!< GPIO19 Mode Configuration */
#define IOM_Gpio19ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR10, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO20_Ctrl *********************************************************/
/*!< GPIO20 Mode Definition */
#define GPIO20_GPIO (GPIO_MODE)
#define GPIO20_I2C1_MS_CLK (1)
#define GPIO20_UART2_TX (3)
#define GPIO20_XIN_12M (7)
/*!< GPIO20 Mode Configuration */
#define IOM_Gpio20ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR11, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO21_Ctrl *********************************************************/
/*!< GPIO21 Mode Definition */
#define GPIO21_GPIO (GPIO_MODE)
#define GPIO21_I2C1_MS_DA (1)
#define GPIO21_UART2_RX (3)
#define GPIO21_XOUT_12M (7)
/*!< GPIO21 Mode Configuration */
#define IOM_Gpio21ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR11, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO22_Ctrl *********************************************************/
/*!< GPIO22 Mode Definition */
#define GPIO22_GPIO (GPIO_MODE)
#define GPIO22_UART0_TX (1)
#define GPIO22_SCD_CLK (2)
#define GPIO22_SCC_CLK (3)
/*!< GPIO22 Mode Configuration */
#define IOM_Gpio22ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR12, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO23_Ctrl *********************************************************/
/*!< GPIO23 Mode Definition */
#define GPIO23_GPIO (GPIO_MODE)
#define GPIO23_SCD_SIO (1)
#define GPIO23_SCC_SIO (2)
#define GPIO23_UART0_RX (3)
/*!< GPIO23 Mode Configuration */
#define IOM_Gpio23ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR12, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO24_Ctrl *********************************************************/
/*!< GPIO24 Mode Definition */
#define GPIO24_GPIO (GPIO_MODE)
#define GPIO24_UART0_RTS (1)
#define GPIO24_TM4_CH (2)
#define GPIO24_CHECK_SCDCARD_INSERT_IN (3)
#define GPIO24_ADC_IN8 (7)
/*!< GPIO24 Mode Configuration */
#define IOM_Gpio24ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR13, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO25_Ctrl *********************************************************/
/*!< GPIO25 Mode Definition */
#define GPIO25_GPIO (GPIO_MODE)
#define GPIO25_UART0_CTS (1)
#define GPIO25_SCD_RST (2)
#define GPIO25_SCC_RST (3)
#define GPIO25_ADC_IN9 (7)
/*!< GPIO25 Mode Configuration */
#define IOM_Gpio25ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR13, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO26_Ctrl *********************************************************/
/*!< GPIO26 Mode Definition */
#define GPIO26_GPIO (GPIO_MODE)
#define GPIO26_COMP_P (7)
/*!< GPIO26 Mode Configuration */
#define IOM_Gpio26ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR14, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO27_Ctrl *********************************************************/
/*!< GPIO27 Mode Definition */
#define GPIO27_GPIO (GPIO_MODE)
#define GPIO27_COMP_M (7)
/*!< GPIO27 Mode Configuration */
#define IOM_Gpio27ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR14, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  GPIO28_Ctrl *********************************************************/
/*!< GPIO28 Mode Definition */
#define GPIO28_GPIO (GPIO_MODE)
#define GPIO28_SCD_CLK (1)
#define GPIO28_SCC_CLK (2)
#define GPIO28_I2C1_MS_CLK (3)
#define GPIO28_ADC_VREF_POSITIVE (7)
/*!< GPIO28 Mode Configuration */
#define IOM_Gpio28ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR15, (BIT2 | BIT1 | BIT0), mode)

/******  GPIO29_Ctrl *********************************************************/
/*!< GPIO29 Mode Definition */
#define GPIO29_GPIO (GPIO_MODE)
#define GPIO29_SCD_SIO (1)
#define GPIO29_SCC_SIO (2)
#define GPIO29_I2C1_MS_DA (3)
#define GPIO29_ADC_VREF_NEGATIVE (7)
/*!< GPIO29 Mode Configuration */
#define IOM_Gpio29ConfigMode(mode) IOM_SET_REG_MSK_VAL(IOM->CR15, (BIT5 | BIT4 | BIT3), (mode << 3))

/******  PWM *********************************************************/
#define IOM_ConfigGpio0Pwm0Out() IOM_Gpio0ConfigMode(GPIO0_PWM0)
#define IOM_ConfigGpio1Pwm1Out() IOM_Gpio1ConfigMode(GPIO1_PWM1)
#define IOM_ConfigGpio2Pwm2Out() IOM_Gpio2ConfigMode(GPIO2_PWM2)
#define IOM_ConfigGpio3Pwm3Out() IOM_Gpio3ConfigMode(GPIO3_PWM3)
#define IOM_ConfigGpio4Pwm4Out() IOM_Gpio4ConfigMode(GPIO4_PWM4)
#define IOM_ConfigGpio5Pwm5Out() IOM_Gpio5ConfigMode(GPIO5_PWM5)
#define IOM_ConfigGpio6Pwm6Out() IOM_Gpio6ConfigMode(GPIO6_PWM6)
#define IOM_ConfigGpio7Pwm7Out() IOM_Gpio7ConfigMode(GPIO7_PWM7)

/******  TIMER *********************************************************/

/******  Timer0 *********************************************************/
#define IOM_ConfigGpio0Tm0ChIn() IOM_Gpio0ConfigMode(GPIO0_TM0_CH)
#define IOM_ConfigGpio12Tm0ChIn() IOM_Gpio12ConfigMode(GPIO12_TM0_CH)
/******  Timer1 *********************************************************/
#define IOM_ConfigGpio1Tm1ChIn() IOM_Gpio1ConfigMode(GPIO1_TM1_CH)
#define IOM_ConfigGpio13Tm1ChIn() IOM_Gpio13ConfigMode(GPIO13_TM1_CH)
/******  Timer2 *********************************************************/
#define IOM_ConfigGpio2Tm2ChIn() IOM_Gpio2ConfigMode(GPIO2_TM2_CH)
#define IOM_ConfigGpio18Tm2ChIn() IOM_Gpio18ConfigMode(GPIO18_TM2_CH)
/******  Timer3 *********************************************************/
#define IOM_ConfigGpio3Tm3ChIn() IOM_Gpio3ConfigMode(GPIO3_TM3_CH)
#define IOM_ConfigGpio19Tm3ChIn() IOM_Gpio19ConfigMode(GPIO19_TM3_CH)
/******  Timer4 *********************************************************/
#define IOM_ConfigGpio4Tm4ChIn() IOM_Gpio4ConfigMode(GPIO4_TM4_CH)
#define IOM_ConfigGpio24Tm4ChIn() IOM_Gpio24ConfigMode(GPIO24_TM4_CH)

/******  UART *********************************************************/

/******  UART0 *********************************************************/
#define IOM_ConfigGpio0Uart0Tx() IOM_Gpio0ConfigMode(GPIO0_UART0_TX)
#define IOM_ConfigGpio1Uart0Rx() IOM_Gpio1ConfigMode(GPIO1_UART0_RX)
#define IOM_ConfigGpio22Uart0Tx() IOM_Gpio22ConfigMode(GPIO22_UART0_TX)
#define IOM_ConfigGpio23Uart0Rx() IOM_Gpio23ConfigMode(GPIO23_UART0_RX)
#define IOM_ConfigGpio24Uart0RtsOutput() IOM_Gpio24ConfigMode(GPIO24_UART0_RTS)
#define IOM_ConfigGpio25Uart0CtsInput() IOM_Gpio25ConfigMode(GPIO25_UART0_CTS)
/*!< UART0_Rxd Source Definition */
#define UART0_RXD_SRC_ADCOMP (1) // the highest priority
#define UART0_RXD_SRC_USBRXD (2)
#define UART0_RXD_SRC_GPIO1 (3)
#define UART0_RXD_SRC_GPIO23 (0)
/*!< UART0_Rxd Source Configuration */
#define IOM_UART0RxdConfigSrc(src) IOM_SET_REG_MSK_VAL(IOM->CR12, (BIT7 | BIT6), (src << 6))
/*!< IOM_UART0RX Configuration */
#define IOM_UART0RX_EnableWakeUp() (SET_REG_BIT(IOM->WUENC, BIT0))
#define IOM_UART0RX_DisableWakeUp() (CLR_REG_BIT(IOM->WUENC, BIT0))

/******  UART1 *********************************************************/
#define IOM_ConfigGpio2Uart1Tx() IOM_Gpio2ConfigMode(GPIO2_UART1_TX)
#define IOM_ConfigGpio3Uart1Rx() IOM_Gpio3ConfigMode(GPIO3_UART1_RX)
#define IOM_ConfigGpio18Uart1Tx() IOM_Gpio18ConfigMode(GPIO18_UART1_TX)
#define IOM_ConfigGpio19Uart1Rx() IOM_Gpio19ConfigMode(GPIO19_UART1_RX)
#define IOM_ConfigGpio12Uart1RtsOut() IOM_Gpio12ConfigMode(GPIO12_UART1_RTS)
#define IOM_ConfigGpio13Uart1CtsIn() IOM_Gpio13ConfigMode(GPIO13_UART1_CTS)
/*!< IOM_UART1RX Configuration */
#define IOM_UART1RX_EnableWakeUp() (SET_REG_BIT(IOM->WUENC, BIT1))
#define IOM_UART1RX_DisableWakeUp() (CLR_REG_BIT(IOM->WUENC, BIT1))

/******  UART2 *********************************************************/
#define IOM_ConfigGpio4Uart2Tx() IOM_Gpio4ConfigMode(GPIO4_UART2_TX)
#define IOM_ConfigGpio5Uart2Rx() IOM_Gpio5ConfigMode(GPIO5_UART2_RX)
#define IOM_ConfigGpio20Uart2Tx() IOM_Gpio20ConfigMode(GPIO20_UART2_TX)
#define IOM_ConfigGpio21Uart2Rx() IOM_Gpio21ConfigMode(GPIO21_UART2_RX)
/******  UART2_Rxd_Ctrl
 * *********************************************************/
/*!< UART2_Rxd Source Definition */
#define UART2_RXD_SRC_ADCOMP (1) // the highest priority
#define UART2_RXD_SRC_USBRXD (2)
#define UART2_RXD_SRC_GPIO5 (3)
#define UART2_RXD_SRC_GPIO21 (0)
/*!< UART2_Rxd Source Configuration */
#define IOM_UART2RxdConfigSrc(src) IOM_SET_REG_MSK_VAL(IOM->CR11, (BIT7 | BIT6), (src << 6))
/*!< IOM_UART2RX Configuration */
#define IOM_UART2RX_EnableWakeUp() (SET_REG_BIT(IOM->WUENC, BIT2))
#define IOM_UART2RX_DisableWakeUp() (CLR_REG_BIT(IOM->WUENC, BIT2))

/******  SCD *********************************************************/
#define IOM_ConfigGpio5ScdRstIn() IOM_Gpio5ConfigMode(GPIO5_SCD_RST)
#define IOM_ConfigGpio25ScdRstIn() IOM_Gpio25ConfigMode(GPIO25_SCD_RST)
#define IOM_ConfigGpio22ScdClkIn() IOM_Gpio22ConfigMode(GPIO22_SCD_CLK)
#define IOM_ConfigGpio28ScdClkIn() IOM_Gpio28ConfigMode(GPIO28_SCD_CLK)
#define IOM_ConfigGpio23ScdSio() IOM_Gpio23ConfigMode(GPIO23_SCD_SIO)
#define IOM_ConfigGpio29ScdSio() IOM_Gpio29ConfigMode(GPIO29_SCD_SIO)
#define IOM_ConfigGpio24ScdCardInsertCheckIn() IOM_Gpio24ConfigMode(GPIO24_CHECK_SCDCARD_INSERT_IN)
/*!< IOM_SCDRST Configuration */
#define IOM_SCDRST_EnableIntPosEdge() (SET_REG_BIT(IOM->INTPEGD, BIT6))
#define IOM_SCDRST_DisableIntPosEdge() (CLR_REG_BIT(IOM->INTPEGD, BIT6))
#define IOM_SCDRST_EnableIntNegEdge() (SET_REG_BIT(IOM->INTNEGD, BIT6))
#define IOM_SCDRST_DisableIntNegEdge() (CLR_REG_BIT(IOM->INTNEGD, BIT6))
#define IOM_SCDRST_ClearIntPosState() (SET_REG_VAL(IOM->INTSTATD, BIT7))
#define IOM_SCDRST_GetIntPosState() (GET_REG_BIT(IOM->INTSTATD, BIT7))
#define IOM_SCDRST_ClearIntNegState() (SET_REG_VAL(IOM->INTSTATD, BIT6))
#define IOM_SCDRST_GetIntNegState() (GET_REG_BIT(IOM->INTSTATD, BIT6))
#define IOM_SCDRST_EnableWakeUp() (SET_REG_BIT(IOM->WUENC, BIT6))
#define IOM_SCDRST_DisableWakeUp() (CLR_REG_BIT(IOM->WUENC, BIT6))
#define IOM_SCDRST_SetWakeUpHighLevel() (SET_REG_BIT(IOM->WUENC, BIT0))
#define IOM_SCDRST_SetWakeUpLowLevel() (CLR_REG_BIT(IOM->WUENC, BIT0))
#define IOM_SCDRST_GetLevel() (GET_REG_BIT(IOM->GPIODRD, BIT6))
/*!< IOM_SCDPLUG Configuration */
#define IOM_SCDPLUG_EnableIntPosEdge() (SET_REG_BIT(IOM->INTPEGD, BIT7))
#define IOM_SCDPLUG_DisableIntPosEdge() (CLR_REG_BIT(IOM->INTPEGD, BIT7))
#define IOM_SCDPLUG_EnableIntNegEdge() (SET_REG_BIT(IOM->INTNEGD, BIT7))
#define IOM_SCDPLUG_DisableIntNegEdge() (CLR_REG_BIT(IOM->INTNEGD, BIT7))
#define IOM_SCDPLUG_ClearIntPosState() (SET_REG_VAL(IOM->INTSTATE, BIT1))
#define IOM_SCDPLUG_GetIntPosState() (GET_REG_BIT(IOM->INTSTATE, BIT1))
#define IOM_SCDPLUG_ClearIntNegState() (SET_REG_VAL(IOM->INTSTATE, BIT0))
#define IOM_SCDPLUG_GetIntNegState() (GET_REG_BIT(IOM->INTSTATE, BIT0))
#define IOM_SCDPLUG_EnableWakeUp() (SET_REG_BIT(IOM->WUEND, BIT2))
#define IOM_SCDPLUG_DisableWakeUp() (CLR_REG_BIT(IOM->WUEND, BIT2))
#define IOM_SCDPLUG_SetWakeUpHighLevel() (SET_REG_BIT(IOM->WUENC, BIT1))
#define IOM_SCDPLUG_SetWakeUpLowLevel() (CLR_REG_BIT(IOM->WUENC, BIT1))
#define IOM_SCDPLUG_GetLevel() (GET_REG_BIT(IOM->GPIODRD, BIT7))
/*!< IOM_SCDUNPLUG Configuration */
#define IOM_SCDUNPLUG_EnableWakeUp() (SET_REG_BIT(IOM->WUEND, BIT3))
#define IOM_SCDUNPLUG_DisableWakeUp() (CLR_REG_BIT(IOM->WUEND, BIT3))
#define IOM_SCDUNPLUG_SetWakeUpHighLevel() (SET_REG_BIT(IOM->WUENC, BIT2))
#define IOM_SCDUNPLUG_SetWakeUpLowLevel() (CLR_REG_BIT(IOM->WUENC, BIT2))
/*!< IOM_SCDSIO Configuration */
#define IOM_SCDSIO_EnableWakeUp() (SET_REG_BIT(IOM->WUENC, BIT5))
#define IOM_SCDSIO_DisableWakeUp() (CLR_REG_BIT(IOM->WUENC, BIT5))

/******  SCC *********************************************************/
#define IOM_ConfigGpio5SccRstOut() IOM_Gpio5ConfigMode(GPIO5_SCC_RST)
#define IOM_ConfigGpio25SccRstOut() IOM_Gpio25ConfigMode(GPIO25_SCC_RST)
#define IOM_ConfigGpio22SccClkOut() IOM_Gpio22ConfigMode(GPIO22_SCC_CLK)
#define IOM_ConfigGpio28SccClkOut() IOM_Gpio28ConfigMode(GPIO28_SCC_CLK)
#define IOM_ConfigGpio23SccSio() IOM_Gpio23ConfigMode(GPIO23_SCC_SIO)
#define IOM_ConfigGpio29SccSio() IOM_Gpio29ConfigMode(GPIO29_SCC_SIO)

/******  ADC *********************************************************/
#define IOM_ConfigGpio6AdcIn0() IOM_Gpio6ConfigMode(GPIO6_ADC_IN0)
#define IOM_ConfigGpio7AdcIn1() IOM_Gpio7ConfigMode(GPIO7_ADC_IN1)
#define IOM_ConfigGpio8AdcIn2() IOM_Gpio8ConfigMode(GPIO8_ADC_IN2)
#define IOM_ConfigGpio9AdcIn3() IOM_Gpio9ConfigMode(GPIO9_ADC_IN3)
#define IOM_ConfigGpio10AdcIn4() IOM_Gpio10ConfigMode(GPIO10_ADC_IN4)
#define IOM_ConfigGpio11AdcIn5() IOM_Gpio11ConfigMode(GPIO11_ADC_IN5)
#define IOM_ConfigGpio12AdcIn6() IOM_Gpio12ConfigMode(GPIO12_ADC_IN6)
#define IOM_ConfigGpio13AdcIn7() IOM_Gpio13ConfigMode(GPIO13_ADC_IN7)
#define IOM_ConfigGpio24AdcIn8() IOM_Gpio24ConfigMode(GPIO24_ADC_IN8)
#define IOM_ConfigGpio25AdcIn9() IOM_Gpio25ConfigMode(GPIO25_ADC_IN9)
#define IOM_ConfigGpio28AdcVrefPositive() IOM_Gpio28ConfigMode(GPIO28_ADC_VREF_POSITIVE)
#define IOM_ConfigGpio29AdcVrefNegative() IOM_Gpio29ConfigMode(GPIO29_ADC_VREF_NEGATIVE)

/******  SPIM *********************************************************/

/******  SPIM0 *********************************************************/
#define IOM_ConfigGpio8Spim0ClkOut() IOM_Gpio8ConfigMode(GPIO8_SPIM0_CLK)
#define IOM_ConfigGpio9Spim0CsnOut() IOM_Gpio9ConfigMode(GPIO9_SPIM0_CSN)
#define IOM_ConfigGpio10Spim0MOSI() IOM_Gpio10ConfigMode(GPIO10_SPIM0_MOSI)
#define IOM_ConfigGpio11Spim0MISO() IOM_Gpio11ConfigMode(GPIO11_SPIM0_MISO)
#define IOM_ConfigGpio12Spim0IO2() IOM_Gpio12ConfigMode(GPIO12_SPIM0_IO2)
#define IOM_ConfigGpio13Spim0IO3() IOM_Gpio13ConfigMode(GPIO13_SPIM0_IO3)

/******  SPIM1 *********************************************************/
#define IOM_ConfigGpio14Spim1ClkOut() IOM_Gpio14ConfigMode(GPIO14_SPIM1_CLK)
#define IOM_ConfigGpio15Spim1CsnOut() IOM_Gpio15ConfigMode(GPIO15_SPIM1_CSN)
#define IOM_ConfigGpio16Spim1MISO() IOM_Gpio16ConfigMode(GPIO16_SPIM1_MISO)
#define IOM_ConfigGpio17Spim1MOSI() IOM_Gpio17ConfigMode(GPIO17_SPIM1_MOSI)

/******  SPIS *********************************************************/

/******  SPIS0 *********************************************************/
#define IOM_ConfigGpio8Spis0ClkIn() IOM_Gpio8ConfigMode(GPIO8_SPIS0_CLK)
#define IOM_ConfigGpio9Spis0CsnIn() IOM_Gpio9ConfigMode(GPIO9_SPIS0_CSN)
#define IOM_ConfigGpio10Spis0MISO() IOM_Gpio10ConfigMode(GPIO10_SPIS0_MISO)
#define IOM_ConfigGpio11Spis0MOSI() IOM_Gpio11ConfigMode(GPIO11_SPIS0_MOSI)
/*!< IOM_SPIS0CSN Configuration */
#define IOM_SPIS0CSN_EnableWakeUp() (SET_REG_BIT(IOM->WUENC, BIT3))
#define IOM_SPIS0CSN_DisableWakeUp() (CLR_REG_BIT(IOM->WUENC, BIT3))

/******  SPIS1 *********************************************************/
#define IOM_ConfigGpio14Spis1ClkIn() IOM_Gpio14ConfigMode(GPIO14_SPIS1_CLK)
#define IOM_ConfigGpio15Spis1CsnIn() IOM_Gpio15ConfigMode(GPIO15_SPIS1_CSN)
#define IOM_ConfigGpio16SPis1MOSI() IOM_Gpio16ConfigMode(GPIO16_SPIS1_MOSI)
#define IOM_ConfigGpio17Spis1MISO() IOM_Gpio17ConfigMode(GPIO17_SPIS1_MISO)
/*!< IOM_SPIS1CSN Configuration */
#define IOM_SPIS1CSN_EnableWakeUp() (SET_REG_BIT(IOM->WUENC, BIT4))
#define IOM_SPIS1CSN_DisableWakeUp() (CLR_REG_BIT(IOM->WUENC, BIT4))

/******  I2C *********************************************************/

/******  I2C0 *********************************************************/
#define IOM_ConfigGpio18I2c0MSClk() IOM_Gpio18ConfigMode(GPIO18_I2C0_MS_CLK)
#define IOM_ConfigGpio19I2c0MSDa() IOM_Gpio19ConfigMode(GPIO19_I2C0_MS_DA)
/*!< I2C0SDA WakeUp Configuration */
#define IOM_I2C0SDA_EnableWakeUp() (SET_REG_BIT(IOM->WUEND, BIT0))
#define IOM_I2C0SDA_DisableWakeUp() (CLR_REG_BIT(IOM->WUEND, BIT0))

/******  I2C1 *********************************************************/
#define IOM_ConfigGpio20I2c1MSClk() IOM_Gpio20ConfigMode(GPIO20_I2C1_MS_CLK)
#define IOM_ConfigGpio28I2c1MSClk() IOM_Gpio28ConfigMode(GPIO28_I2C1_MS_CLK)
#define IOM_ConfigGpio21I2c1MSDa() IOM_Gpio21ConfigMode(GPIO21_I2C1_MS_DA)
#define IOM_ConfigGpio29I2c1MSDa() IOM_Gpio29ConfigMode(GPIO29_I2C1_MS_DA)
/*!< I2C1SDA WakeUp Configuration */
#define IOM_I2C1SDA_EnableWakeUp() (SET_REG_BIT(IOM->WUEND, BIT1))
#define IOM_I2C1SDA_DisableWakeUp() (CLR_REG_BIT(IOM->WUEND, BIT1))

/******  Xin/Xout *********************************************************/
#define IOM_ConfigGpio20Xin12M() IOM_Gpio20ConfigMode(GPIO20_XIN_12M)
#define IOM_ConfigGpio21Xout12M() IOM_Gpio21ConfigMode(GPIO21_XOUT_12M)

/******  Comparator *********************************************************/
#define IOM_ConfigGpio15CompOut() IOM_Gpio15ConfigMode(GPIO15_COMP_OUT)
#define IOM_ConfigGpio17UsbphyCompOut() IOM_Gpio17ConfigMode(GPIO17_USBPHY_COMP_OUT)
#define IOM_ConfigGpio26CompP() IOM_Gpio26ConfigMode(GPIO26_COMP_P)
#define IOM_ConfigGpio27CompM() IOM_Gpio27ConfigMode(GPIO27_COMP_M)

/******  For Test *********************************************************/
#define IOM_ConfigGpio18AdUsbRxDpTestOut() IOM_Gpio18ConfigMode(GPIO18_ADUSBRXDP_TEST_OUT)
#define IOM_ConfigGpio19AdUsbRxDmpTestOut() IOM_Gpio19ConfigMode(GPIO19_ADUSBRXDM_TEST_OUT)
#define IOM_ConfigGpio1812MTestOut() IOM_Gpio18ConfigMode(GPIO18_12MCRYSTAL_TEST_OUT)
#define IOM_ConfigGpio19Xo32KRc32KTestOut() IOM_Gpio19ConfigMode(GPIO19_XO32K_RC32K_TEST_OUT)

#define IOM_EnableRcvOut() SET_REG_BIT(IOM->CRTST1, BIT0)  // 使能后，Comparator输出到GPIO19
#define IOM_DisableRcvOut() CLR_REG_BIT(IOM->CRTST1, BIT0) // Comparator输出禁止
// GPIO19需设置为输出模式，sel为0，选中Crysta,32K，sel为1，选中RC32K
#define IOM_Clk32KSrc(sel) IOM_SET_REG_MSK_VAL(IOM->CRTST1, BIT1, ((sel & 0x01UL) << 0x01UL))
#define IOM_EnableDivDll48MOut() SET_REG_BIT(IOM->CRTST1, BIT2)  // 使能后，Dll48M 4分频输出到GPIO10
#define IOM_DisableDivDll48MOut() CLR_REG_BIT(IOM->CRTST1, BIT2) // Dll48M 4分频输出禁止
#define IOM_EnableUsb300MOut() SET_REG_BIT(IOM->CRTST1, BIT3)  // 使能后，300MHz（分频到1MHz）输出到GPIO22
#define IOM_DisableUsb300MOut() CLR_REG_BIT(IOM->CRTST1, BIT3) // 300MHz（分频到1MHz）输出禁止
#define IOM_EnableOcpFlag()                                                                                            \
    SET_REG_BIT(IOM->CRTST1, BIT4)) // 使能后，Adocp_flag_33ext输出到GPIO22
#define IOM_DisableOcpFlag()                                                                                           \
    CLR_REG_BIT(IOM->CRTST1, BIT4)) // Adocp_flag_33ext输出禁止
#define IOM_EnableOsc80MDivOut() SET_REG_BIT(IOM->CRTST1, BIT5)  // 使能后，Osc80M分频输出到GPIO22
#define IOM_DisableOsc80MDivOut() CLR_REG_BIT(IOM->CRTST1, BIT5) // Osc80M分频输出禁止

//------------------------------------------------------------------
//                        TypeDefs
//------------------------------------------------------------------
typedef enum
{
    IOM_SUCCESS  = 0, ///< 操作成功
    IOM_GPIO_ERR = 1, ///< GPIO超出范围
} IOM_ERROR_CODE;

typedef enum
{
    GPIO0 = 0,
    GPIO1,
    GPIO2,
    GPIO3,
    GPIO4,
    GPIO5,
    GPIO6,
    GPIO7,
    GPIO8,
    GPIO9,
    GPIO10,
    GPIO11,
    GPIO12,
    GPIO13,
    GPIO14,
    GPIO15,
    GPIO16,
    GPIO17,
    GPIO18,
    GPIO19,
    GPIO20,
    GPIO21,
    GPIO22,
    GPIO23,
    GPIO24,
    GPIO25,
    GPIO26,
    GPIO27,
    GPIO28,
    GPIO29,
} IOM_GPIO;

typedef enum
{
    PADS_DRV_4500uA  = 0,
    PADS_DRV_9000uA  = 1,
    PADS_DRV_13500uA = 2,
    PADS_DRV_18000uA = 3,
} IOM_PADS_DRV_CAPA;

//------------------------------------------------------------------
//                        Exported variables
//------------------------------------------------------------------

//------------------------------------------------------------------
//                        Exported functions
//------------------------------------------------------------------
/**
 * @brief GPIO正沿中断触发开关函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：正沿中断禁止，其他：正沿中断使能
 */
void IOM_ConfigGpioIntPosEdge(IOM_GPIO GPIOx, uint32_t enable);

/**
 * @brief GPIO负沿中断触发开关函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：负沿中断禁止，其他：负沿中断使能
 */
void IOM_ConfigGpioIntNegEdge(IOM_GPIO GPIOx, uint32_t enable);

/**
 * @brief GPIO中断状态清除函数
 *
 * @param GPIOx 支持GPIO0~29
 */
void IOM_ClearGpioIntState(IOM_GPIO GPIOx);

/**
 * @brief GPIO中断状态获取函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param state 输出GPIOx的中断状态
 */
void IOM_GetGpioIntStat(IOM_GPIO GPIOx, uint32_t* state);

/**
 * @brief GPIO唤醒开关函数
 * 
 * @param GPIOx 支持GPIO0~15
 * @param enable 0：GPIO唤醒禁止，其他：GPIO唤醒使能
 * @return IOM_SUCCESS ：成功，IOM_GPIO_ERR ：GPIO超出范围
 */
uint32_t IOM_ConfigGpioWakeUp(IOM_GPIO GPIOx, uint32_t enable);

/**
 * @brief GPIO唤醒电平配置函数
 * 
 * @param GPIOx 支持GPIO0~15
 * @param level 0：低电平，其他：高电平
 * @return IOM_SUCCESS ：成功，IOM_GPIO_ERR ：GPIO超出范围
 */
uint32_t IOM_SetGpioWakeUpLevel(IOM_GPIO GPIOx, uint32_t level);

/**
 * @brief GPIO功能配置函数
 *
 * @param GPIOx 支持GPIO0~29
 */
void IOM_ConfigGpioMode(IOM_GPIO GPIOx);

/**
 * @brief GPIO输出电平配置函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param level 0：低电平，其他：高电平
 */
void IOM_SetGpioLevel(IOM_GPIO GPIOx, uint32_t level);

/**
 * @brief GPIO电平状态获取函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param level 输出GPIOx电平状态
 */
void IOM_GetGpioLevel(IOM_GPIO GPIOx, uint32_t* level);

/**
 * @brief GPIO输出开关函数
 * 
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO输出禁止，其他：GPIO输出使能
 */
void IOM_ConfigGpioOut(IOM_GPIO GPIOx, uint32_t enable);

/**
 * @brief GPIO输入开关函数
 * 
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO输入禁止，其他：GPIO输入使能
 */
void IOM_ConfigGpioIn(IOM_GPIO GPIOx, uint32_t enable);

/**
 * @brief GPIO上拉开关函数
 *
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO上拉禁止，其他：GPIO上拉使能
 */
void IOM_ConfigGpioPullUp(IOM_GPIO GPIOx, uint32_t enable);

/**
 * @brief GPIO下拉开关函数
 * 
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO下拉禁止，其他：GPIO下拉使能
 */
void IOM_ConfigGpioPullDown(IOM_GPIO GPIOx, uint32_t enable);

/**
 * @brief GPIO SMT开关函数
 * 
 * @param GPIOx 支持GPIO0~29
 * @param enable 0：GPIO SMT禁止，其他：GPIO SMT使能
 */
void IOM_ConfigGpioSmt(IOM_GPIO GPIOx, uint32_t enable);

/**
 * @brief GPIO驱动能力配置函数
 * 
 * @param GPIOx 支持GPIO0~29
 * @param val 四选一，0：驱动能力4.5mA，1：驱动能力9mA，2：驱动能力13.5mA，3：驱动能力18mA
 */
void IOM_ConfigGpioPinDrvCapa(IOM_GPIO GPIOx, IOM_PADS_DRV_CAPA val);

/**
 * @brief PWM紧急停车输入源配置函数
 * 
 * @param GPIOx 支持GPIO14~18和GPIO20~23
 * @return IOM_SUCCESS ：成功，IOM_GPIO_ERR ：GPIO超出范围
 */
uint32_t IOM_SetPwmEmergencyStopSrc(IOM_GPIO GPIOx);

#endif // __N32G020XX_IOM_H__

/** @} */
