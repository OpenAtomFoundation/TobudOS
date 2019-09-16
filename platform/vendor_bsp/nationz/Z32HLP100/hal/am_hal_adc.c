/** ****************************************************************************
 * @copyright                  Nationz Co.Ltd
 *               Copyright (c) 2013гн2018 All Rights Reserved
 *******************************************************************************
 * @file     am_hal_adc.c
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for interfacing with the Analog to Digital Converter.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup adc Analog-to-Digital Converter (ADC)
 * @ingroup hal
 * @{
 ******************************************************************************/

#include "am_mcu_Z32HLP.h"

//*****************************************************************************
//
//! @brief Private SRAM view of temperature trims from OTP.
//!
//! These three words have to be loaded via code running only in SRAM.
//!
//! This static SRAM union is private to the ADC HAL functions.
//
//*****************************************************************************
static union
{
    //! These values are loaded as uint32_t values from OTP.
    struct
    {
        //! Temperature of the package test head (in degrees Kelvin)
        uint32_t ui32CalibrationTemperature;

        //! Voltage corresponding to temperature measured on test head.
        uint32_t ui32CalibrationVoltage;

        //! ADC offset voltage measured on the package test head.
        uint32_t ui32CalibrationOffset;
    } ui32;
    //! These values are accessed as floats when used for temperature calculation.
    struct
    {
        //! Temperature of the package test head in degrees Kelvin
        float    fCalibrationTemperature;

        //! Voltage corresponding to temperature measured on test head.
        float    fCalibrationVoltage;

        //! ADC offset voltage measured on the package test head.
        float    fCalibrationOffset;
    } flt;
} priv_temp_trims;

//*****************************************************************************
//
//! @brief Configure the ADC.
//!
//! @param psConfig - pointer to the configuration structure for the ADC.
//!
//! This function may be used to perform the initial setup of the ADC based on
//! setting found in a configuration structure. In addition, calling this
//! function copies the temperature trim values from OTP to SRAM.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_config(am_hal_adc_config_t *psConfig)
{
    __asm("");

    //
    // Set general ADC configuration parameters.
    //
    AM_REG(ADC, CFG) = (psConfig->ui32Clock         |
                        psConfig->ui32TriggerConfig |
                        psConfig->ui32Reference     |
                        psConfig->ui32MaxSampleRate |
                        psConfig->ui32PowerMode     |
                        psConfig->ui32Repeat        |
                        psConfig-> ui32POnTemp      |
                        AM_REG_ADC_CFG_ADCEN(1));

    //
    // Set the window limits for the ADC.
    //
    AM_REG(ADC, WLIM) = psConfig->ui32Window;

    //
    // Grab the temperature trims from OTP once.
    //
    priv_temp_trims.ui32.ui32CalibrationTemperature =
        am_hal_flash_load_ui32(AM_HAL_ADC_CALIB_TEMP_ADDR);
    priv_temp_trims.ui32.ui32CalibrationVoltage     =
        am_hal_flash_load_ui32(AM_HAL_ADC_CALIB_AMBIENT_ADDR);
    priv_temp_trims.ui32.ui32CalibrationOffset      =
        am_hal_flash_load_ui32(AM_HAL_ADC_CALIB_ADC_OFFSET_ADDR);

    //
    // If any of the OTP calibration values are invalid, set up some
    //  obviously invalid defaults.  For example, with these defaults the
    //  function am_hal_adc_volts_to_celsius() will return the value for
    //  absolute zero while avoiding a divide-by-zero error.
    //
    if ((priv_temp_trims.ui32.ui32CalibrationTemperature == 0xffffffff)    ||
        (priv_temp_trims.ui32.ui32CalibrationVoltage     == 0xffffffff)    ||
        (priv_temp_trims.ui32.ui32CalibrationOffset      == 0xffffffff))
    {
        priv_temp_trims.flt.fCalibrationOffset      = 0.0F;
        priv_temp_trims.flt.fCalibrationTemperature = 0.0F;
        priv_temp_trims.flt.fCalibrationVoltage     = 1.0F;
    }
}

//*****************************************************************************
//
//! @brief Get the temperature trim parameters after configuring the ADC.
//!
//! @param pfTemp - pointer to a location to store the calibration temperature.
//! @param pfVoltage - pointer to a location to store the calibration voltage.
//! @param pfOffsetV - pointer to a location to store the calibration offset.
//!
//! This function may be used to access the actual temperature sensor trim
//! values from the private structure.
//!
//! WARNING: only call this after the ADC has been configured with
//!          am_hal_adc_config.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_temp_trims_get(float * pfTemp, float * pfVoltage, float * pfOffsetV)
{
    //
    // Return trim temperature as a float, if you can.
    //
    if (pfTemp != NULL)
    {
        *pfTemp = priv_temp_trims.flt.fCalibrationTemperature;
    }

    //
    // Return trim voltage as a float, if you can.
    //
    if (pfVoltage != NULL)
    {
        *pfVoltage = priv_temp_trims.flt.fCalibrationVoltage;
    }

    //
    // Return trim ADC offset voltage as a float, if you can.
    //
    if (pfOffsetV != NULL)
    {
        *pfOffsetV = priv_temp_trims.flt.fCalibrationOffset;
    }
}

//*****************************************************************************
//
//! @brief Set the ADC window parameters.
//!
//! @param ui32Upper - the upper limit for the ADC window.
//! @param ui32Upper - the lower limit for the ADC window.
//!
//! This function may be used to change the ADC window parameters. Please note
//! that the upper and lower limits are only 16-bits wide in the ADC hardware.
//! This function will ignore the upper 16 bits of these arguments.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_window_set(uint32_t ui32Upper, uint32_t ui32Lower)
{
    //
    // Set the window limits for the ADC.
    //
    AM_REG(ADC, WLIM) = AM_HAL_ADC_WINDOW(ui32Upper, ui32Lower);
}

//*****************************************************************************
//
//! @brief Configure a single ADC slot.
//!
//! @param ui32SlotNumber - the number of the ADC slot to be configured.
//! @param ui32SlotConfig - contains slot-specific options.
//!
//! This function may be used to configure the settings for an individual ADC
//! slot. The parameter \b ui32SlotConfig should be the logical 'OR' of a slot
//! average macro, a slot hold-time macro, a slot channel macro, and
//! optionally, the slot window enable macro.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_slot_config(uint32_t ui32SlotNumber, uint32_t ui32SlotConfig)
{
    uint32_t ui32RegOffset;

    //
    // Locate the correct register for this ADC slot.
    //
    ui32RegOffset = (AM_REG_ADCn(0) + AM_REG_ADC_SL0CFG_O +
                     (4 * ui32SlotNumber));

    //
    // Write the register with the caller's configuration value.
    //
    AM_REGVAL(ui32RegOffset) = ui32SlotConfig;
}

//*****************************************************************************
//
//! @brief Read One ADC FIFO Entry.
//!
//! @param psFifoReadValue - pointer to FIFO Read Value Structure.
//!
//! This function reads the FIFO data register, extracts the bit fields and
//! puts them into a device independent view of the values. It also grabs the
//! interrupt status bits at the time the FIFO is read and returns them in
//! the structure. Caller should check the return value to see if the FIFO
//! contained anything useful. Caller can also use the return value to see if
//! there are any additional values available at the time of call.
//! Any interrupt status bits are cleared as a result of this call.
//! The top of FIFO entry is popped off of the FIFO by this call, if any thing
//! was present.
//!
//! This function can be called either from an ISR or from the Base level.
//!
//! This function can be called repeatedly on the ISR or Base level, until the
//! the FIFO contains no more data, as indicated by a zero in the returned
//! value.
//!
//! @return FIFO depth from reading FIFO register. Non-zero --> valid data
//
//*****************************************************************************
uint32_t
am_hal_adc_fifo_read(am_hal_adc_fifo_read_t *psFifoReadValue)
{
    uint32_t ui32FIFOValue;
    uint32_t ui32Count;

    //
    // Grab a value from the ADC FIFO.
    //
    psFifoReadValue-> ui8IntStatus = AM_REG(ADC, INTSTAT);

    //
    // Clear any current interrupts.
    //
    AM_REG(ADC, INTCLR) = psFifoReadValue-> ui8IntStatus;

    //
    // Grab a value from the ADC FIFO.
    //
    ui32FIFOValue = AM_REG(ADC, FIFO);

    //
    // Read once, extract the fields.
    //
    psFifoReadValue-> ui8Slot = AM_READ_SM(AM_REG_ADC_FIFO_SLOTNUM,
                                           ui32FIFOValue);
    psFifoReadValue->ui16Data = AM_READ_SM(AM_REG_ADC_FIFO_DATA, ui32FIFOValue);

    //
    // Check FIFO valid bits.
    //
    ui32Count = AM_READ_SM(AM_REG_ADC_FIFO_COUNT, ui32FIFOValue);

    if (ui32Count)
    {
        //
        // Pop the FIFO.
        //
        AM_REG(ADC, FIFO) = 0; // write anything to it.
    }

    //
    // Return FIFO valid bits.
    //
    return ui32Count;
}

//*****************************************************************************
//
//! @brief Issue Software Trigger to the ADC.
//!
//! This function issues the software trigger to the ADC.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_trigger(void)
{
    //
    // Write to the Software trigger register in the ADC.
    //
    AM_REG(ADC, SWT) = 0x37;
}

//*****************************************************************************
//
//! @brief Enable the ADC.
//!
//! Use this function to enable the ADC.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_enable(void)
{
    //
    // Enable the ADC.
    //
    AM_BFW(ADC, CFG, ADCEN, 0x1);
}

//*****************************************************************************
//
//! @brief Disable the ADC.
//!
//! Use this function to disable the ADC.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_disable(void)
{
    //
    // Disable the ADC.
    //
    AM_BFW(ADC, CFG, ADCEN, 0x0);
}

//*****************************************************************************
//
//! @brief Enable the ADC battery load resistor.
//!
//! Use this function to enable the ADC battery load resistor.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_batt_load_enable(void)
{
    //
    // Enable the ADC battery load resistor.
    //
    AM_BFW(ADC, CFG, BATTLOAD,
           AM_REG_ADC_CFG_BATTLOAD_EN >> AM_REG_ADC_CFG_BATTLOAD_S);
}

//*****************************************************************************
//
//! @brief Disable the ADC battery load resistor.
//!
//! Use this function to disable the ADC battery load resistor.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_batt_load_disable(void)
{
    //
    // Disable the ADC battery load resistor.
    //
    AM_BFW(ADC, CFG, BATTLOAD,
           AM_REG_ADC_CFG_BATTLOAD_DIS >> AM_REG_ADC_CFG_BATTLOAD_S);
}

//*****************************************************************************
//
//! @brief Enable selected ADC Interrupts.
//!
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_adc.h.
//!
//! Use this function to enable the ADC interrupts.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_int_enable(uint32_t ui32Interrupt)
{
    //
    // Enable the interrupts.
    //
    AM_REG(ADC, INTEN) |= ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Return enabled ADC Interrupts.
//!
//! Use this function to get all enabled ADC interrupts.
//!
//! @return enabled ADC Interrupts.
//
//*****************************************************************************
uint32_t
am_hal_adc_int_enable_get(void)
{
    //
    // Return enabled interrupts.
    //
    return AM_REG(ADC, INTEN);
}

//*****************************************************************************
//
//! @brief Disable selected ADC Interrupts.
//!
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_adc.h.
//!
//! Use this function to disable the ADC interrupts.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_int_disable(uint32_t ui32Interrupt)
{
    //
    // Disable the interrupts.
    //
    AM_REG(ADC, INTEN) &= ~ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Clear selected ADC Interrupts.
//!
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_adc.h.
//!
//! Use this function to clear the ADC interrupts.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_int_clear(uint32_t ui32Interrupt)
{
    //
    // Clear the interrupts.
    //
    AM_REG(ADC, INTCLR) = ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Set selected ADC Interrupts.
//!
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_adc.h.
//!
//! Use this function to set the ADC interrupts.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_adc_int_set(uint32_t ui32Interrupt)
{
    //
    // Set the interrupts.
    //
    AM_REG(ADC, INTSET) = ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Return either enabled or raw selected ADC interrupt status.
//!
//! @param bEnabledOnly - return the status of only the enabled interrupts.
//!
//! Use this function to get the ADC interrupt status.
//!
//! @return enabled or raw ADC interrupt status.
//
//*****************************************************************************
uint32_t
am_hal_adc_int_status_get(bool bEnabledOnly)
{
    //
    // Return the status.
    //
    if (bEnabledOnly)
    {
        uint32_t u32RetVal = AM_REG(ADC, INTEN);
        u32RetVal &= AM_REG(ADC, INTSTAT);
        return u32RetVal;
    }
    else
    {
        return AM_REG(ADC, INTSTAT);
    }
}

//*****************************************************************************
//
//! @brief Return temperature in degrees C of supplied voltage.
//!
//! @param fVoltage - return the temperature corresponding to this voltage.
//!
//! Use this function to convert volts from the temperature sensor into degrees
//! C.  Caller converts ADC binary code to volts based on reference used.
//! This routine looks up the trim parameters from OTP and returns corrected
//! temperature.
//!
//! The computation is based on a line running through 0 degrees K.
//! We find the slope from the trimmed temperature calibration point.
//!
//!
//! @return the temperature in degrees C.
//
//*****************************************************************************
float
am_hal_adc_volts_to_celsius(float fVoltage)
{
    float fTemp;

    //
    // Get calibration temperature from trimmed values & convert to degrees K.
    //
    float fCalibration_temp = priv_temp_trims.flt.fCalibrationTemperature;

    //
    // Get remaining trimmed values.
    //
    float fCalibration_voltage = priv_temp_trims.flt.fCalibrationVoltage;
    float fCalibration_offset  = priv_temp_trims.flt.fCalibrationOffset;

    //
    // Compute the temperature.
    //
    fTemp  = fCalibration_temp;
    fTemp /= (fCalibration_voltage - fCalibration_offset);
    fTemp *= (fVoltage - fCalibration_offset);

    //
    // Give it back to the caller in Celsius.
    //
    return fTemp - 273.15f;
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
