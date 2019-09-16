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
 * @file     am_hal_adc.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for interfacing with the Analog to Digital Converter
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup adc Analog-to-Digital Converter (ADC)
 * @ingroup hal
 * @{
 ******************************************************************************/
#ifndef AM_HAL_ADC_H
#define AM_HAL_ADC_H

//*****************************************************************************
//
//! @name Clock Selection
//! @brief These macros may be used to set the ADC module's clock source.
//! @{
//
//*****************************************************************************
#define AM_HAL_ADC_CLOCK_OFF                AM_REG_ADC_CFG_CLKSEL_OFF
#define AM_HAL_ADC_CLOCK_12MHZ              AM_REG_ADC_CFG_CLKSEL_12MHZ
#define AM_HAL_ADC_CLOCK_6MHZ               AM_REG_ADC_CFG_CLKSEL_6MHZ
#define AM_HAL_ADC_CLOCK_3MHZ               AM_REG_ADC_CFG_CLKSEL_3MHZ
#define AM_HAL_ADC_CLOCK_1_5MHZ             AM_REG_ADC_CFG_CLKSEL_1_5MHZ
//! @}

//*****************************************************************************
//
//! @name Trigger Settings
//! @brief ADC trigger setting macros.
//!
//! These macros alter the ADC's trigger source and trigger polarity. Note that
//! the external trigger setting needs to be ORed with a POS or NEG option to
//! define the desired trigger polarity.
//! @{
//
//*****************************************************************************
#define AM_HAL_ADC_TRIGGER_FALL             AM_REG_ADC_CFG_TRIGPOL_FALLING_EDGE
#define AM_HAL_ADC_TRIGGER_RISE             AM_REG_ADC_CFG_TRIGPOL_RISING_EDGE
#define AM_HAL_ADC_TRIGGER_SOFT             AM_REG_ADC_CFG_TRIGSEL(8)
#define AM_HAL_ADC_TRIGGER_EXT(n)           AM_REG_ADC_CFG_TRIGSEL(n)
//! @}

//*****************************************************************************
//
//! @name Reference Settings
//! @brief ADC reference voltage setting macros.
//!
//! These macros control the ADC reference voltage source.
//! @{
//
//*****************************************************************************
#define AM_HAL_ADC_REF_VDD                  AM_REG_ADC_CFG_REFSEL_VDD
#define AM_HAL_ADC_REF_EXT                  AM_REG_ADC_CFG_REFSEL_ADCREF
#define AM_HAL_ADC_REF_INT                  AM_REG_ADC_CFG_REFSEL_INTERNAL
//! @}

//*****************************************************************************
//
//! @name Sample mode
//! @brief ADC sample mode settings
//!
//! These macros control the maximum allowable sample rate of the ADC.
//! Selecting a higher maximum sample rate will cause the microcontroller to
//! allocate more current to the ADC. It is recommended that this setting is
//! kept at the lowest possible value unless the application requires a faster
//! sample rate.
//!
//! There are only two allowable settings to the mode register.
//! @{
//
//*****************************************************************************
#define AM_HAL_ADC_MODE_125KSPS             AM_REG_ADC_CFG_OPMODE(0)
#define AM_HAL_ADC_MODE_1MSPS               AM_REG_ADC_CFG_OPMODE(2)
//! @}

//*****************************************************************************
//
//! @name Low Power Mode
//! @brief ADC power conservation settings.
//!
//! These macros select the power state to enter between active scans. Each low
//! power mode has it's own set of timing constraints. Please see the datasheet
//! for additional timing information on each power mode.
//! @{
//
//*****************************************************************************
#define AM_HAL_ADC_LPMODE_NONE              AM_REG_ADC_CFG_LPMODE(0)
#define AM_HAL_ADC_LPMODE_1                 AM_REG_ADC_CFG_LPMODE(1)
#define AM_HAL_ADC_LPMODE_2                 AM_REG_ADC_CFG_LPMODE(2)
//! @}

//*****************************************************************************
//
//! @brief Enable repeating scan mode.
//!
//! Use this macro to enable repeating scans using a timer.
//
//*****************************************************************************
#define AM_HAL_ADC_REPEAT                   AM_REG_ADC_CFG_RPTEN(1)
#define AM_HAL_ADC_NO_REPEAT                AM_REG_ADC_CFG_RPTEN(0)

//*****************************************************************************
//
//! @brief Power On Temp Sensor
//!
//! Use this macro to turn on the temp sensor
//
//*****************************************************************************
#define AM_HAL_ADC_PON_TEMP                 AM_REG_ADC_CFG_TMPSPWR_EN
#define AM_HAL_ADC_POFF_TEMP                AM_REG_ADC_CFG_TMPSPWR_DIS

//*****************************************************************************
//
//! @brief Set ADC window limits.
//!
//! Use this macro to set the window limits on the ADC.
//
//*****************************************************************************
#define AM_HAL_ADC_WINDOW(upper, lower)                                       \
    ((upper << 16) | (lower & 0xFFFF))

//*****************************************************************************
//
//! @name Slot configuration
//! @brief Slot configuration macros
//!
//! These macros may be used to configure an individual ADC slot.
//! @{
//
//*****************************************************************************
#define AM_HAL_ADC_SLOT_AVG_1               AM_REG_ADC_SL0CFG_ADSEL0(0)
#define AM_HAL_ADC_SLOT_AVG_2               AM_REG_ADC_SL0CFG_ADSEL0(1)
#define AM_HAL_ADC_SLOT_AVG_4               AM_REG_ADC_SL0CFG_ADSEL0(2)
#define AM_HAL_ADC_SLOT_AVG_8               AM_REG_ADC_SL0CFG_ADSEL0(3)
#define AM_HAL_ADC_SLOT_AVG_16              AM_REG_ADC_SL0CFG_ADSEL0(4)
#define AM_HAL_ADC_SLOT_AVG_32              AM_REG_ADC_SL0CFG_ADSEL0(5)
#define AM_HAL_ADC_SLOT_AVG_64              AM_REG_ADC_SL0CFG_ADSEL0(6)
#define AM_HAL_ADC_SLOT_AVG_128             AM_REG_ADC_SL0CFG_ADSEL0(7)

#define AM_HAL_ADC_SLOT_HOLD_1              AM_REG_ADC_SL0CFG_THSEL0(0)
#define AM_HAL_ADC_SLOT_HOLD_2              AM_REG_ADC_SL0CFG_THSEL0(1)
#define AM_HAL_ADC_SLOT_HOLD_4              AM_REG_ADC_SL0CFG_THSEL0(2)
#define AM_HAL_ADC_SLOT_HOLD_8              AM_REG_ADC_SL0CFG_THSEL0(3)
#define AM_HAL_ADC_SLOT_HOLD_16             AM_REG_ADC_SL0CFG_THSEL0(4)
#define AM_HAL_ADC_SLOT_HOLD_32             AM_REG_ADC_SL0CFG_THSEL0(5)
#define AM_HAL_ADC_SLOT_HOLD_64             AM_REG_ADC_SL0CFG_THSEL0(6)
#define AM_HAL_ADC_SLOT_HOLD_128            AM_REG_ADC_SL0CFG_THSEL0(7)

#define AM_HAL_ADC_SLOT_CHANNEL(n)          AM_REG_ADC_SL0CFG_CHSEL0(n)

#define AM_HAL_ADC_SLOT_CHSEL_EXT0          AM_REG_ADC_SL0CFG_CHSEL0_EXT0
#define AM_HAL_ADC_SLOT_CHSEL_EXT1          AM_REG_ADC_SL0CFG_CHSEL0_EXT1
#define AM_HAL_ADC_SLOT_CHSEL_EXT2          AM_REG_ADC_SL0CFG_CHSEL0_EXT2
#define AM_HAL_ADC_SLOT_CHSEL_EXT3          AM_REG_ADC_SL0CFG_CHSEL0_EXT3
#define AM_HAL_ADC_SLOT_CHSEL_EXT4          AM_REG_ADC_SL0CFG_CHSEL0_EXT4
#define AM_HAL_ADC_SLOT_CHSEL_EXT5          AM_REG_ADC_SL0CFG_CHSEL0_EXT5
#define AM_HAL_ADC_SLOT_CHSEL_EXT6          AM_REG_ADC_SL0CFG_CHSEL0_EXT6
#define AM_HAL_ADC_SLOT_CHSEL_EXT7          AM_REG_ADC_SL0CFG_CHSEL0_EXT7
#define AM_HAL_ADC_SLOT_CHSEL_TEMP          AM_REG_ADC_SL0CFG_CHSEL0_TEMP
#define AM_HAL_ADC_SLOT_CHSEL_VDD           AM_REG_ADC_SL0CFG_CHSEL0_VDD
#define AM_HAL_ADC_SLOT_CHSEL_VSS           AM_REG_ADC_SL0CFG_CHSEL0_VSS
#define AM_HAL_ADC_SLOT_CHSEL_VBATT         AM_REG_ADC_SL0CFG_CHSEL0_VBATT

#define AM_HAL_ADC_SLOT_WINDOW_EN           AM_REG_ADC_SL0CFG_WCEN0(1)
#define AM_HAL_ADC_SLOT_ENABLE              AM_REG_ADC_SL0CFG_SLEN0(1)
//! @}

//*****************************************************************************
//
//! @name Interrupt Status Bits
//! @brief Interrupt Status Bits for enable/disble use
//!
//! These macros may be used to enable an individual ADC interrupt cause.
//! @{
//
//*****************************************************************************
#define AM_HAL_ADC_INTEN_WCINC              AM_REG_ADC_INTEN_WCINC(1)
#define AM_HAL_ADC_INTEN_WCEXC              AM_REG_ADC_INTEN_WCEXC(1)
#define AM_HAL_ADC_INTEN_FIFOOVR2           AM_REG_ADC_INTEN_FIFOOVR2(1)
#define AM_HAL_ADC_INTEN_FIFOOVR1           AM_REG_ADC_INTEN_FIFOOVR1(1)
#define AM_HAL_ADC_INTEN_SCNCMP             AM_REG_ADC_INTEN_SCNCMP(1)
#define AM_HAL_ADC_INTEN_CNVCMP             AM_REG_ADC_INTEN_CNVCMP(1)
//! @}

//*****************************************************************************
//
//! @name Temperature Trim Value Locations
//! @brief Temperature calibration trim value locations in OTP.
//!
//! These macros are used to access the temperature trim values in OTP.
//! @{
//
//*****************************************************************************
#define AM_HAL_ADC_CALIB_AMBIENT_ADDR       (0x5002004C)
#define AM_HAL_ADC_CALIB_AMBIENT_OFFSET     (0x0000004C)
#define AM_HAL_ADC_CALIB_TEMP_ADDR          (0x50020050)
#define AM_HAL_ADC_CALIB_TEMP_OFFSET        (0x00000050)
#define AM_HAL_ADC_CALIB_ADC_OFFSET_ADDR    (0x50020054)
#define AM_HAL_ADC_CALIB_ADC_OFFSET_OFFSET  (0x00000054)
//! @}

//*****************************************************************************
//
//! @brief Configuration structure for the ADC.
//
//*****************************************************************************
typedef struct
{
    //
    //! Select the ADC Clock source using one of the clock source macros.
    //
    uint32_t ui32Clock;

    //
    //! Select the ADC trigger source using a trigger source macro.
    //
    uint32_t ui32TriggerConfig;

    //
    //! Use a macro to select the ADC reference voltage.
    //
    uint32_t ui32Reference;

    //
    //! Use a macro to choose a maximum sample rate setting.
    //
    uint32_t ui32MaxSampleRate;

    //
    //! Use a macro to choose the power mode for the ADC's idle state.
    //
    uint32_t ui32PowerMode;

    //
    //! Use the Repeat macro to enable repeating samples using Timer3A
    //
    uint32_t ui32Repeat;

    //
    //! Power on the Temperature Sensor
    //
    uint32_t ui32POnTemp;

    //
    //! Set the ADC window limits using the window limit macro.
    //
    uint32_t ui32Window;
}
am_hal_adc_config_t;


//*****************************************************************************
//
//! @brief FIFO read value structure for the ADC.
//
//*****************************************************************************
typedef struct
{
    //
    //! Interrupt status bits at the time the FIFO was read
    //
    uint8_t ui8IntStatus;

    //
    //! 3 bit Slot # returning data for this FIFO read
    //
    uint8_t ui8Slot;

    //
    //! 16 bit (10.6) ADC data from this FIFO read
    //
    uint16_t ui16Data;
}
am_hal_adc_fifo_read_t;

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern void am_hal_adc_config(am_hal_adc_config_t *psConfig);
extern void am_hal_adc_window_set(uint32_t ui32Upper, uint32_t ui32Lower);
extern void am_hal_adc_slot_config(uint32_t ui32SlotNumber,
                                   uint32_t ui32SlotConfig);

extern uint32_t am_hal_adc_fifo_read(am_hal_adc_fifo_read_t *psFifoReadValue);
extern void am_hal_adc_trigger(void);
extern void am_hal_adc_enable(void);
extern void am_hal_adc_disable(void);
extern void am_hal_adc_batt_load_enable(void);
extern void am_hal_adc_batt_load_disable(void);
extern void am_hal_adc_int_enable(uint32_t ui32Interrupt);
extern uint32_t am_hal_adc_int_enable_get(void);
extern void am_hal_adc_int_disable(uint32_t ui32Interrupt);
extern void am_hal_adc_int_clear(uint32_t ui32Interrupt);
extern void am_hal_adc_int_set(uint32_t ui32Interrupt);
extern uint32_t am_hal_adc_int_status_get(bool bEnabledOnly);
extern float am_hal_adc_volts_to_celsius(float fVoltage);
extern void am_hal_adc_temp_trims_get(float * pfTemp, float * pfVoltage, float * pfOffsetV);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_ADC_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
