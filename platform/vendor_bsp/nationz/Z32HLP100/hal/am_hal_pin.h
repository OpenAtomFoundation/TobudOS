/** ----------------------------------------------------------------------------
 *         Nationz Technology Software Support  -  NATIONZ  -
 * -----------------------------------------------------------------------------
 * Copyright (c) 2013гн2018, Nationz Corporation  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaiimer below.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the disclaimer below in the documentation and/or
 * other materials provided with the distribution. 
 * 
 * Nationz's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission. 
 * 
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONZ "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
 * -----------------------------------------------------------------------------
 */

/** ****************************************************************************
 * @copyright      Nationz Co.,Ltd 
 *        Copyright (c) 2013гн2018 All Rights Reserved 
 *******************************************************************************
 * @file     am_hal_pin.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Macros for configuring specific pins.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup pin PIN
 * @ingroup hal
 * @{
 ******************************************************************************/

#ifndef AM_HAL_PIN_H
#define AM_HAL_PIN_H

//*****************************************************************************
//
// Pin definition helper macros.
//
//*****************************************************************************
#define AM_HAL_PIN_DIR_INPUT      (AM_HAL_GPIO_INPEN)
#define AM_HAL_PIN_DIR_INPUT1      (AM_HAL_GPIO_INPEN1)
#define AM_HAL_PIN_DIR_OUTPUT     (AM_HAL_GPIO_OUT_PUSHPULL)
#define AM_HAL_PIN_DIR_OPENDRAIN  (AM_HAL_GPIO_OUT_OPENDRAIN | AM_HAL_GPIO_INPEN)
#define AM_HAL_PIN_DIR_3STATE     (AM_HAL_GPIO_OUT_3STATE)

//*****************************************************************************
//
// Pin definition helper macros.
//
//*****************************************************************************
#define AM_HAL_PIN_DISABLE        (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_INPUT          (AM_HAL_GPIO_FUNC(3) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_INPUT1          (AM_HAL_GPIO_FUNC(3) | AM_HAL_PIN_DIR_INPUT1)
#define AM_HAL_PIN_OUTPUT         (AM_HAL_GPIO_FUNC(3) | AM_HAL_PIN_DIR_OUTPUT)
#define AM_HAL_PIN_OPENDRAIN      (AM_HAL_GPIO_FUNC(3) | AM_HAL_PIN_DIR_OPENDRAIN)
#define AM_HAL_PIN_3STATE         (AM_HAL_GPIO_FUNC(3) | AM_HAL_PIN_DIR_3STATE)

//*****************************************************************************
//
// Pin definition macros.
//
//*****************************************************************************
#define AM_HAL_PIN_0_SLSCL        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_0_SLSCK        (AM_HAL_GPIO_FUNC(1) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_0_UARTTX       (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_0_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_0_M0SCK_LOOP   (AM_HAL_GPIO_FUNC(4))
#define AM_HAL_PIN_0_M1SCK_LOOP   (AM_HAL_GPIO_FUNC(5))
#define AM_HAL_PIN_0_M0SCL_LOOP   (AM_HAL_GPIO_FUNC(6))
#define AM_HAL_PIN_0_M1SCL_LOOP   (AM_HAL_GPIO_FUNC(7))

#define AM_HAL_PIN_1_SLSDA        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_OPENDRAIN)
#define AM_HAL_PIN_1_SLMISO       (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_1_UARTRX       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_1_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_1_M0MISO_LOOP  (AM_HAL_GPIO_FUNC(4))
#define AM_HAL_PIN_1_M1MISO_LOOP  (AM_HAL_GPIO_FUNC(5))
#define AM_HAL_PIN_1_M0SDA_LOOP   (AM_HAL_GPIO_FUNC(6))
#define AM_HAL_PIN_1_M1SDA_LOOP   (AM_HAL_GPIO_FUNC(7))

#define AM_HAL_PIN_2_SLWIR3       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_3STATE)
#define AM_HAL_PIN_2_SLMOSI       (AM_HAL_GPIO_FUNC(1) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_2_CLKOUT       (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_2_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_2_M0MOSI_LOOP  (AM_HAL_GPIO_FUNC(4))
#define AM_HAL_PIN_2_M1MOSI_LOOP  (AM_HAL_GPIO_FUNC(5))
#define AM_HAL_PIN_2_M0WIR3_LOOP  (AM_HAL_GPIO_FUNC(6))
#define AM_HAL_PIN_2_M1WIR3_LOOP  (AM_HAL_GPIO_FUNC(7))

#define AM_HAL_PIN_3_TRIG0        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_3_SLnCE        (AM_HAL_GPIO_FUNC(1) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_3_M1nCE4       (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_3_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_3_M0nCE_LOOP   (AM_HAL_GPIO_FUNC(4))
#define AM_HAL_PIN_3_M1nCE_LOOP   (AM_HAL_GPIO_FUNC(5))
#define AM_HAL_PIN_3_PSOURCE      (AM_HAL_GPIO_FUNC(3) | AM_HAL_PIN_DIR_OUTPUT | AM_HAL_GPIO_POWER)

#define AM_HAL_PIN_4_TRIG1        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_4_SLINT        (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_4_M0nCE5       (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_4_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_4_SLINTGP_LOOP (AM_HAL_GPIO_FUNC(4))
#define AM_HAL_PIN_4_SWO          (AM_HAL_GPIO_FUNC(5))
#define AM_HAL_PIN_4_CLKOUT       (AM_HAL_GPIO_FUNC(6))
#define AM_HAL_PIN_4_PSOURCE      (AM_HAL_GPIO_FUNC(3) | AM_HAL_PIN_DIR_OUTPUT | AM_HAL_GPIO_POWER)

#define AM_HAL_PIN_5_M0SCL        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_OPENDRAIN)
#define AM_HAL_PIN_5_M0SCK        (AM_HAL_GPIO_FUNC(1) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_5_UARTS        (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_5_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_5_M0SCK_LOOP   (AM_HAL_GPIO_FUNC(4))
#define AM_HAL_PIN_5_M0SCL_LOOP   (AM_HAL_GPIO_FUNC(6))

#define AM_HAL_PIN_6_M0SDA        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_OPENDRAIN)
#define AM_HAL_PIN_6_M0MISO       (AM_HAL_GPIO_FUNC(1) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_6_UACTS        (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_6_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_6_SLMISO_LOOP  (AM_HAL_GPIO_FUNC(4))
#define AM_HAL_PIN_6_SLSDA_LOOP   (AM_HAL_GPIO_FUNC(6))

#define AM_HAL_PIN_7_M0WIR3       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_3STATE)
#define AM_HAL_PIN_7_M0MOSI       (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_7_CLKOUT       (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_7_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_7_SLWIR3_LOOP  (AM_HAL_GPIO_FUNC(6))

#define AM_HAL_PIN_8_M1SCL        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_OPENDRAIN)
#define AM_HAL_PIN_8_M1SCK        (AM_HAL_GPIO_FUNC(1) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_8_M0nCE4       (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_8_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_8_M1SCK_LOOP   (AM_HAL_GPIO_FUNC(5))
#define AM_HAL_PIN_8_M1SCL_LOOP   (AM_HAL_GPIO_FUNC(7))

#define AM_HAL_PIN_9_M1SDA        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_OPENDRAIN)
#define AM_HAL_PIN_9_M1MISO       (AM_HAL_GPIO_FUNC(1) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_9_M0nCE5       (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_9_GPIO         (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_9_SLMISO_LOOP  (AM_HAL_GPIO_FUNC(5))
#define AM_HAL_PIN_9_SLSDA_LOOP   (AM_HAL_GPIO_FUNC(7))

#define AM_HAL_PIN_10_M1WIR3      (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_3STATE)
#define AM_HAL_PIN_10_M1MOSI      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_10_M0nCE6      (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_10_GPIO        (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_10_EXTHFA      (AM_HAL_GPIO_FUNC(5) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_10_SLWIR3_LOOP (AM_HAL_GPIO_FUNC(7))

#define AM_HAL_PIN_11_RESERVED    (AM_HAL_GPIO_FUNC(0))
#define AM_HAL_PIN_11_M0nCE0      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_11_CLKOUT      (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_11_GPIO        (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_11_PSINK       (AM_HAL_GPIO_FUNC(3))

#define AM_HAL_PIN_12_ADC0        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_12_M1nCE0      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_12_TCTA0       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_12_GPIO        (AM_HAL_GPIO_FUNC(3))

#define AM_HAL_PIN_13_ADC1        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_13_M1nCE1      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_13_TCTB0       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_13_GPIO        (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_13_EXTHFB      (AM_HAL_GPIO_FUNC(5) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_13_SWO         (AM_HAL_GPIO_FUNC(6))

#define AM_HAL_PIN_14_ADC2        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_14_M1nCE2      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_14_UARTTX      (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_14_GPIO        (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_14_EXTHFS      (AM_HAL_GPIO_FUNC(5) | AM_HAL_PIN_DIR_INPUT)

#define AM_HAL_PIN_15_ADC3        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_15_M1nCE3      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_15_UARTRX      (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_15_GPIO        (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_15_EXTXT       (AM_HAL_GPIO_FUNC(5) | AM_HAL_PIN_DIR_INPUT)

#define AM_HAL_PIN_16_ADCREF      (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_16_M0nCE4      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_16_TRIG2       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_16_GPIO        (AM_HAL_GPIO_FUNC(3))

#define AM_HAL_PIN_17_CMPAD0      (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_17_M0nCE1      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_17_TRIG3       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_17_GPIO        (AM_HAL_GPIO_FUNC(3))
#define AM_HAL_PIN_17_EXTLF       (AM_HAL_GPIO_FUNC(5) | AM_HAL_PIN_DIR_INPUT)

#define AM_HAL_PIN_18_CMPAD1      (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_18_M0nCE2      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_18_TCTA1       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_18_GPIO        (AM_HAL_GPIO_FUNC(3))

#define AM_HAL_PIN_19_CMPRF0      (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_19_M0nCE3      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_19_TCTB1       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_19_GPIO        (AM_HAL_GPIO_FUNC(3))

#define AM_HAL_PIN_20_SWDCK       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_20_M1nCE5      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_20_TCTA2       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_20_GPIO        (AM_HAL_GPIO_FUNC(3))

#define AM_HAL_PIN_21_SWDIO       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_21_M1nCE6      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_21_TCTB2       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_21_GPIO        (AM_HAL_GPIO_FUNC(3))

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_22_UARTTX      (AM_HAL_GPIO_FUNC(0))
#define AM_HAL_PIN_22_M1nCE7      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_22_TCTA3       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_22_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_23_UARTRX      (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_23_M0nCE0      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_23_TCTB3       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_23_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_24_M0nCE1      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_24_CLKOUT      (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_24_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_25_EXTXT       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_25_M0nCE2      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_25_TCTA0       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_25_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_26_EXTLF       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_26_M0nCE3      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_26_TCTB0       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_26_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_27_EXTHF       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_27_M1nCE4      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_27_TCTA1       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_27_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_28_M1nCE5      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_28_TCTB1       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_28_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#define AM_HAL_PIN_29_ADC4        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_29_M1nCE6      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_29_TCTA2       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_29_GPIO        (AM_HAL_GPIO_FUNC(3))

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_30_ADC5        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_30_M1nCE7      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_30_TCTB2       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_30_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#define AM_HAL_PIN_31_ADC6        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_31_M0nCE4      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_31_TCTA3       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_31_GPIO        (AM_HAL_GPIO_FUNC(3))

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_32_ADC7        (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_32_M0nCE5      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_32_TCTB3       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_32_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_33_CMPRF1      (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_33_M0nCE6      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_33_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_34_CMPRF2      (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_34_M0nCE7      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_34_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#define AM_HAL_PIN_35_M1nCE0      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_35_UARTTX      (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_35_GPIO        (AM_HAL_GPIO_FUNC(3))

#define AM_HAL_PIN_36_M1nCE1      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_36_UARTRX      (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_36_GPIO        (AM_HAL_GPIO_FUNC(3))

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_37_TRIG0       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_37_M1nCE2      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_37_UARTS       (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_37_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_38_TRIG1       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_38_M1nCE3      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_38_UACTS       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_38_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_39_TRIG2       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_39_UARTTX      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_39_CLKOUT      (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_39_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_40_TRIG3       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_40_UARTRX      (AM_HAL_GPIO_FUNC(1) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_40_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#define AM_HAL_PIN_41_TRIG4       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_41_SWO         (AM_HAL_GPIO_FUNC(2))
#define AM_HAL_PIN_41_GPIO        (AM_HAL_GPIO_FUNC(3))

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_42_TRIG5       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_42_M0nCE0      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_42_TCTA0       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_42_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_43_TRIG6       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_43_M0nCE1      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_43_TCTB0       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_43_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_44_TRIG7       (AM_HAL_GPIO_FUNC(0) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_44_M0nCE2      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_44_TCTA1       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_44_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_45_M0nCE3      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_45_TCTB1       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_45_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_46_M0nCE4      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_46_TCTA2       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_46_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_47_M0nCE5      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_47_TCTB2       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_47_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_48_M0nCE6      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_48_TCTA3       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_48_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#if defined (AM_PACKAGE_BGA)
#define AM_HAL_PIN_49_M0nCE7      (AM_HAL_GPIO_FUNC(1))
#define AM_HAL_PIN_49_TCTB3       (AM_HAL_GPIO_FUNC(2) | AM_HAL_PIN_DIR_INPUT)
#define AM_HAL_PIN_49_GPIO        (AM_HAL_GPIO_FUNC(3))
#endif // defined (AM_PACKAGE_BGA)

#endif  // AM_HAL_PIN_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
