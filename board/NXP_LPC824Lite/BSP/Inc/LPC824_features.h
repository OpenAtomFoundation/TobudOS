/*
** ###################################################################
**     Version:             rev. 1.2, 2017-06-08
**     Build:               b180327
**
**     Abstract:
**         Chip specific module features.
**
**     The Clear BSD License
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018 NXP
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without
**     modification, are permitted (subject to the limitations in the
**     disclaimer below) provided that the following conditions are met:
**
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**
**     * Neither the name of the copyright holder nor the names of its
**       contributors may be used to endorse or promote products derived from
**       this software without specific prior written permission.
**
**     NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
**     GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
**     HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
**     WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
**     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
**     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
**     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**     WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
**     OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
**     IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2016-08-12)
**         Initial version.
**     - rev. 1.1 (2016-11-25)
**         Update CANFD and Classic CAN register.
**         Add MAC TIMERSTAMP registers.
**     - rev. 1.2 (2017-06-08)
**         Remove RTC_CTRL_RTC_OSC_BYPASS.
**         SYSCON_ARMTRCLKDIV rename to SYSCON_ARMTRACECLKDIV.
**         Remove RESET and HALT from SYSCON_AHBCLKDIV.
**
** ###################################################################
*/

#ifndef _LPC824_FEATURES_H_
#define _LPC824_FEATURES_H_

/* SOC module features */

/* @brief ADC availability on the SoC. */
#define FSL_FEATURE_SOC_ADC_COUNT (1)
/* @brief CRC availability on the SoC. */
#define FSL_FEATURE_SOC_CRC_COUNT (1)
/* @brief DMA availability on the SoC. */
#define FSL_FEATURE_SOC_DMA_COUNT (1)
/* @brief GPIO availability on the SoC. */
#define FSL_FEATURE_SOC_GPIO_COUNT (1)
/* @brief I2C availability on the SoC. */
#define FSL_FEATURE_SOC_I2C_COUNT (4)
/* @brief INPUTMUX availability on the SoC. */
#define FSL_FEATURE_SOC_INPUTMUX_COUNT (1)
/* @brief IOCON availability on the SoC. */
#define FSL_FEATURE_SOC_IOCON_COUNT (1)
/* @brief MRT availability on the SoC. */
#define FSL_FEATURE_SOC_MRT_COUNT (1)
/* @brief MTB availability on the SoC. */
#define FSL_FEATURE_SOC_MTB_COUNT (1)
/* @brief PINT availability on the SoC. */
#define FSL_FEATURE_SOC_PINT_COUNT (1)
/* @brief PMU availability on the SoC. */
#define FSL_FEATURE_SOC_PMU_COUNT (1)
/* @brief SCT availability on the SoC. */
#define FSL_FEATURE_SOC_SCT_COUNT (1)
/* @brief SPI availability on the SoC. */
#define FSL_FEATURE_SOC_SPI_COUNT (2)
/* @brief SWM availability on the SoC. */
#define FSL_FEATURE_SOC_SWM_COUNT (1)
/* @brief SYSCON availability on the SoC. */
#define FSL_FEATURE_SOC_SYSCON_COUNT (1)
/* @brief USART availability on the SoC. */
#define FSL_FEATURE_SOC_USART_COUNT (3)
/* @brief WWDT availability on the SoC. */
#define FSL_FEATURE_SOC_WWDT_COUNT (1)

/* ACOMP module features */

/* @brief Has INTENA bitfile in CTRL reigster. */
#define FSL_FEATURE_ACOMP_HAS_CTRL_INTENA (0)

/* ADC module features */

/* @brief Do not has input select (register INSEL). */
#define FSL_FEATURE_ADC_HAS_NO_INSEL  (1)
/* @brief Has ASYNMODE bitfile in CTRL reigster. */
#define FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE (0)
/* @brief Has ASYNMODE bitfile in CTRL reigster. */
#define FSL_FEATURE_ADC_HAS_CTRL_RESOL (0)
/* @brief Has ASYNMODE bitfile in CTRL reigster. */
#define FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL (0)
/* @brief Has ASYNMODE bitfile in CTRL reigster. */
#define FSL_FEATURE_ADC_HAS_CTRL_TSAMP (0)
/* @brief Has ASYNMODE bitfile in CTRL reigster. */
#define FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE (1)
/* @brief Has ASYNMODE bitfile in CTRL reigster. */
#define FSL_FEATURE_ADC_HAS_CTRL_CALMODE (1)
/* @brief Has startup register. */
#define FSL_FEATURE_ADC_HAS_STARTUP_REG (0)
/* @brief Has ADTrim register */
#define FSL_FEATURE_ADC_HAS_TRIM_REG (0)
/* @brief Has Calibration register. */
#define FSL_FEATURE_ADC_HAS_CALIB_REG (0)

/* CLOCK module features */

/* @brief GPIOINT clock source. */
#define FSL_FEATURE_CLOCK_HAS_GPIOINT_CLOCK_SOURCE (0)

/* DMA module features */

/* @brief Number of channels */
#define FSL_FEATURE_DMA_NUMBER_OF_CHANNELS (25)

/* INPUTMUX module features */

/* @brief Inputmux clock source. */
#define FSL_FEATURE_INPUTMUX_HAS_NO_INPUTMUX_CLOCK_SOURCE (1)

/* IOCON module features */

/* No feature definitions */

/* MRT module features */

/* @brief Writing a zero asserts the MRT reset. */
#define FSL_FEATURE_MRT_WRITE_ZERO_ASSERT_RESET (1)
/* @brief Has no MULTITASK bitfile in MODCFG reigster. */
#define FSL_FEATURE_MRT_HAS_NO_MODCFG_MULTITASK (1)
/* @brief Has no INUSE bitfile in STAT reigster. */
#define FSL_FEATURE_MRT_HAS_NO_CHANNEL_STAT_INUSE (1)
/* @brief number of channels. */
#define FSL_FEATURE_MRT_NUMBER_OF_CHANNELS  (4)

/* NVIC module features */

/* @brief Number of connected outputs. */
#define FSL_FEATURE_NVIC_HAS_SHARED_INTERTTUPT_NUMBER (1)

/* PINT module features */

/* @brief Number of connected outputs */
#define FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS (8)

/* SCT module features */

/* @brief Number of events */
#define FSL_FEATURE_SCT_NUMBER_OF_EVENTS (8)
/* @brief Number of states */
#define FSL_FEATURE_SCT_NUMBER_OF_STATES (8)
/* @brief Number of match capture */
#define FSL_FEATURE_SCT_NUMBER_OF_MATCH_CAPTURE (8)
/* @brief Writing a zero asserts the SCT reset. */
#define FSL_FEATURE_SCT_WRITE_ZERO_ASSERT_RESET (1)
/* @brief Number of outputs */
#define FSL_FEATURE_SCT_NUMBER_OF_OUTPUTS (6)

/* SPI module features */

/* @brief Has SPOL0 bitfile in CFG reigster. */
#define FSL_FEATURE_SPI_HAS_SSEL0 (1)
/* @brief Has SPOL1 bitfile in CFG reigster. */
#define FSL_FEATURE_SPI_HAS_SSEL1 (1)
/* @brief Has SPOL2 bitfile in CFG reigster. */
#define FSL_FEATURE_SPI_HAS_SSEL2 (1)
/* @brief Has SPOL3 bitfile in CFG reigster. */
#define FSL_FEATURE_SPI_HAS_SSEL3 (1)

/* SWM module features */

/* @brief Has SWM PINENABLE0 ACMP I3. */
#define FSL_FEATURE_SWM_HAS_PINENABLE0_ACMP_I3 (1)
/* @brief Has SWM PINENABLE0 ACMP I4. */
#define FSL_FEATURE_SWM_HAS_PINENABLE0_ACMP_I4 (1)
/* @brief Has SWM PINENABLE0 ACMP I5. */
#define FSL_FEATURE_SWM_HAS_PINENABLE0_ACMP_I5 (0)

/* SYSCON module features */

/* @brief Starter register discontinuous. */
#define FSL_FEATURE_SYSCON_STARTER_DISCONTINUOUS (1)
/* @brief Has PINTSEL register. */
#define FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER (1)

/* USART module features */

/* @brief Has OSR (register OSR). */
#define FSL_FEATURE_USART_HAS_OSR_REGISTER (1)
/* @brief Has TXIDLEEN bitfile in INTENSET reigster. */
#define FSL_FEATURE_USART_HAS_INTENSET_TXIDLEEN (1)
/* @brief Has ABERREN bitfile in INTENSET reigster. */
#define FSL_FEATURE_USART_HAS_ABERR_CHECK (1)

/* WKT module features */

/* @brief Has SEL_EXTCLK bitfile in CTRL reigster. */
#define FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK (1)

/* WWDT module features */

/* @brief Has no RESET register. */
#define FSL_FEATURE_WWDT_HAS_NO_RESET (1)

#endif /* _LPC824_FEATURES_H_ */

