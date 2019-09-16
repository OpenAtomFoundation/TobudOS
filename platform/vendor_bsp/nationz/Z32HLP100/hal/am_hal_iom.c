/** ****************************************************************************
 * @copyright                  Nationz Co.Ltd
 *               Copyright (c) 2013－2018 All Rights Reserved
 *******************************************************************************
 * @file     am_hal_iom.c
 * @author   
 * @date     
 * @version  v1.0
 * @brief   Functions for interfacing with the IO Master module
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup iom IO Master (SPI/I2C)
 * @ingroup hal
 * @{
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_Z32HLP.h"
#include "am_util_delay.h"

//! ASSERT(1) or Correct(0) invalid IOM R/W Thresholds.
#ifndef AM_ASSERT_INVALID_THRESHOLD
#define AM_ASSERT_INVALID_THRESHOLD    (1)
#endif

#define AM_HAL_IOM_TIMEOUT		100000

//*****************************************************************************
//
// Forward declarations.
//
//*****************************************************************************
static uint32_t
internal_am_hal_iom_spi_cmd_construct(uint32_t ui32Operation,
                                      uint32_t ui32ChipSelect,
                                      uint32_t ui32NumBytes,
                                      uint32_t ui32Options);

//*****************************************************************************
//
// IOM Buffer states.
//
//*****************************************************************************
#define BUFFER_IDLE                         0x0
#define BUFFER_SENDING                      0x1
#define BUFFER_RECEIVING                    0x2

//*****************************************************************************
//
// Global state variables
//
//*****************************************************************************
uint32_t g_ui32Mod0Interface = 0;
uint32_t g_ui32Mod1Interface = 0;
//
// Save error status from ISR, particularly for use in I2C queue mode.
//
uint32_t g_iom_error_status = 0;

//*****************************************************************************
//
// Non-blocking buffer and buffer-management variables.
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32State;
    uint32_t *pui32Data;
    uint32_t ui32BytesLeft;
    uint32_t ui32Options;
    void (*pfnCallback)(void);
}
am_hal_iom_nb_buffer;

//
// Global State to keep track if there is an ongoing transaction
//
volatile bool g_bIomBusy[AM_REG_IOMSTR_NUM_MODULES] = {0};

am_hal_iom_nb_buffer g_psIOMBuffers[AM_REG_IOMSTR_NUM_MODULES];

//*****************************************************************************
//
// Computed timeout.
//
// The IOM may not always respond to events (e.g., CMDCMP).  This is a
// timeout value in cycles to be used when waiting on status changes.
//*****************************************************************************
uint32_t ui32StatusTimeout[AM_REG_IOMSTR_NUM_MODULES] = {AM_HAL_IOM_TIMEOUT, AM_HAL_IOM_TIMEOUT};

//*****************************************************************************
//
// Queue management variables.
//
//*****************************************************************************
am_hal_queue_t g_psIOMQueue[AM_REG_IOMSTR_NUM_MODULES];

//*****************************************************************************
//
// Default queue flush function
//
//*****************************************************************************
am_hal_iom_queue_flush_t am_hal_iom_queue_flush = am_hal_iom_sleeping_queue_flush;

//*****************************************************************************
//
//! @brief Enables the IOM module
//!
//! @param ui32Module - The number of the IOM module to be enabled.
//!
//! This function enables the IOM module using the IFCEN bitfield in the
//! IOMSTR_CFG register.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_enable(uint32_t ui32Module)
{
    if ( ui32Module < AM_REG_IOMSTR_NUM_MODULES )
    {
        /*使能IOM Master*/
        AM_REGn(IOMSTR, ui32Module, CFG) |= AM_REG_IOMSTR_CFG_IFCEN(1);
        g_bIomBusy[ui32Module] = false;
    }
    //
    // Make sure the MISO and SCLK inputs are enabled.
    // 设置SPI模式下MISO和SCLK的io配置
    if ( ui32Module == 0 )
    {
        if ( g_ui32Mod0Interface == AM_HAL_IOM_SPIMODE )
        {
            AM_REGn(GPIO, 0, PADKEY) = AM_REG_GPIO_PADKEY_KEYVAL;
            AM_BFW(GPIO, PADREGB, PAD5INPEN, 1);
            AM_BFW(GPIO, PADREGB, PAD6INPEN, 1);
            AM_REGn(GPIO, 0, PADKEY) = 0;
        }
    }
    else
    {
        if ( g_ui32Mod1Interface == AM_HAL_IOM_SPIMODE )
        {
            AM_REGn(GPIO, 0, PADKEY) = AM_REG_GPIO_PADKEY_KEYVAL;
            AM_BFW(GPIO, PADREGC, PAD8INPEN, 1);
            AM_BFW(GPIO, PADREGC, PAD9INPEN, 1);
            AM_REGn(GPIO, 0, PADKEY) = 0;
        }
    }
}

//*****************************************************************************
//
//! @brief Disables the IOM module.
//!
//! @param ui32Module - The number of the IOM module to be disabled.
//!
//! This function disables the IOM module using the IFCEN bitfield in the
//! IOMSTR_CFG register.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_disable(uint32_t ui32Module)
{
    if ( ui32Module < AM_REG_IOMSTR_NUM_MODULES )
    {
        //
        // Wait until the bus is idle.
        //
        am_hal_iom_poll_complete(ui32Module);

        //
        // Disable the interface.
        //
        AM_REGn(IOMSTR, ui32Module, CFG) &= ~(AM_REG_IOMSTR_CFG_IFCEN(1));

        //
        // Disable the MISO and SCLK inputs. (Power optimization)
        //
        if ( ui32Module == 0 )
        {
            if ( g_ui32Mod0Interface == AM_HAL_IOM_SPIMODE )
            {
                AM_REGn(GPIO, 0, PADKEY) = AM_REG_GPIO_PADKEY_KEYVAL;
                AM_BFW(GPIO, PADREGB, PAD5INPEN, 0);
                AM_BFW(GPIO, PADREGB, PAD6INPEN, 0);
                AM_REGn(GPIO, 0, PADKEY) = 0;
            }
        }
        else
        {
            if ( g_ui32Mod1Interface == AM_HAL_IOM_SPIMODE )
            {
                AM_REGn(GPIO, 0, PADKEY) = AM_REG_GPIO_PADKEY_KEYVAL;
                AM_BFW(GPIO, PADREGC, PAD8INPEN, 0);
                AM_BFW(GPIO, PADREGC, PAD9INPEN, 0);
                AM_REGn(GPIO, 0, PADKEY) = 0;
            }
        }
    }
}


//
//! Check and correct the IOM FIFO threshold.
//
#define MAX_RW_THRESHOLD    (AM_HAL_IOM_MAX_FIFO_SIZE - 4)
#define MIN_RW_THRESHOLD    (4)
#if ( AM_ASSERT_INVALID_THRESHOLD == 0 )
static uint8_t check_iom_threshold(const uint8_t iom_threshold)
{
    uint8_t corrected_threshold = iom_threshold;

    if ( corrected_threshold < MIN_RW_THRESHOLD )
    {
        corrected_threshold = MIN_RW_THRESHOLD;
    }

    if ( corrected_threshold > MAX_RW_THRESHOLD )
    {
        corrected_threshold = MAX_RW_THRESHOLD;
    }

    return corrected_threshold;
}
#endif

//*****************************************************************************
//
//! @brief Sets module-wide configuration options for the IOM module.
//!
//! @param ui32Module - The instance number for the module to be configured
//! (zero or one)
//!
//! @param psConfig - Pointer to an IOM configuration structure.
//!
//! This function is used to set the interface mode (SPI or I2C), clock
//! frequency, SPI format (when relevant), and FIFO read/write interrupt
//! thresholds for the IO master. For more information on specific
//! configuration options, please see the documentation for the configuration
//! structure.
//!
//! @note The IOM module should be disabled before configuring or
//! re-configuring. This function will not re-enable the module when it
//! completes. Call the am_hal_iom_enable function when the module is
//! configured and ready to use.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_config(uint32_t ui32Module, const am_hal_iom_config_t *psConfig)
{
    uint32_t ui32Config;

    //
    // Start by checking the interface mode (I2C or SPI), and writing it to the
    // configuration word.
    //
    ui32Config = psConfig->ui32InterfaceMode;

    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    //
    // Also store the interface mode to a global state variable.
    //
    if ( ui32Module == 0 )
    {
        g_ui32Mod0Interface = psConfig->ui32InterfaceMode;
    }
    else
    {
        g_ui32Mod1Interface = psConfig->ui32InterfaceMode;
    }

    //
    // Check the SPI format, and OR in the bits for SPHA (clock phase) and SPOL
    // (polarity). These shouldn't have any effect in I2C mode, so it should be
    // ok to write them without checking exactly which mode we're in.
    //
    if ( psConfig->bSPHA )
    {
        ui32Config |= AM_REG_IOMSTR_CFG_SPHA(1);
    }

    if ( psConfig->bSPOL )
    {
        ui32Config |= AM_REG_IOMSTR_CFG_SPOL(1);
    }

    //
    // Write the resulting configuration word to the IO master CFG register for
    // the module number we were provided.
    //
    AM_REGn(IOMSTR, ui32Module, CFG) = ui32Config;

    //
    // Write the FIFO write and read thresholds to the appropriate registers.
    //
#if     (AM_ASSERT_INVALID_THRESHOLD == 1)
    am_hal_debug_assert_msg(
        (psConfig->ui8WriteThreshold <= MAX_RW_THRESHOLD), "IOM write threshold too big.");
    am_hal_debug_assert_msg(
        (psConfig->ui8ReadThreshold <= MAX_RW_THRESHOLD), "IOM read threshold too big.");
    am_hal_debug_assert_msg(
        (psConfig->ui8WriteThreshold >= MIN_RW_THRESHOLD), "IOM write threshold too small.");
    am_hal_debug_assert_msg(
        (psConfig->ui8ReadThreshold >= MIN_RW_THRESHOLD), "IOM read threshold too small.");

    AM_REGn(IOMSTR, ui32Module, FIFOTHR) =
        (AM_REG_IOMSTR_FIFOTHR_FIFOWTHR(psConfig->ui8WriteThreshold) |
         AM_REG_IOMSTR_FIFOTHR_FIFORTHR(psConfig->ui8ReadThreshold));
#elif   (AM_ASSERT_INVALID_THRESHOLD == 0)
    AM_REGn(IOMSTR, ui32Module, FIFOTHR) =
        (AM_REG_IOMSTR_FIFOTHR_FIFOWTHR(check_iom_threshold(psConfig->ui8WriteThreshold)) |
         AM_REG_IOMSTR_FIFOTHR_FIFORTHR(check_iom_threshold(psConfig->ui8ReadThreshold)));
#else
#error AM_ASSERT_INVALID_THRESHOLD must be 0 or 1.
#endif

    // Finally, write the clock configuration register with the caller-supplied
    // value.
    //
    AM_REGn(IOMSTR, ui32Module, CLKCFG) = psConfig->ui32ClockFrequency;
}


//*****************************************************************************
//
//! @brief Perform a simple write to the SPI interface.
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32ChipSelect - Chip-select number for this transaction.
//! @param pui32Data - Pointer to the bytes that will be sent.
//! @param ui32NumBytes - Number of bytes to send.
//! @param ui32Options - Additional SPI transfer options.
//!
//! This function performs SPI writes to a selected SPI device.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui8Data array.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_spi_write(uint32_t ui32Module, uint32_t ui32ChipSelect,
                     uint32_t *pui32Data, uint32_t ui32NumBytes,
                     uint32_t ui32Options)
{
    //
    // Validate parameters
    //
    am_hal_debug_assert_msg(ui32Module < AM_REG_IOMSTR_NUM_MODULES,
                            "Trying to use an IOM module that doesn't exist.");
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Check to see if queues have been enabled. If they are, we'll actually
    // switch to the queued interface.
    //
    if ( g_psIOMQueue[ui32Module].pui8Data != NULL )
    {
        //
        // If the queue is on, go ahead and add this transaction to the queue.
        //
        am_hal_iom_queue_spi_write(ui32Module, ui32ChipSelect, pui32Data,
                                   ui32NumBytes, ui32Options, 0);

        //
        // Wait until the transaction actually clears.
        //
        am_hal_iom_queue_flush(ui32Module);

        //
        // At this point, we've completed the transaction, and we can return.
        //
        return;
    }
    else
    {
        //
        // Otherwise, we'll just do a polled transaction.
        //
        am_hal_iom_spi_write_nq(ui32Module, ui32ChipSelect, pui32Data,
                                ui32NumBytes, ui32Options);
    }
}

//*****************************************************************************
//
//! @brief Perform simple SPI read operations.
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32ChipSelect - Chip-select number for this transaction.
//! @param pui32Data - Pointer to the array where received bytes should go.
//! @param ui32NumBytes - Number of bytes to read.
//! @param ui32Options - Additional SPI transfer options.
//!
//! This function performs simple SPI read operations. The caller is
//! responsible for ensuring that the receive buffer is large enough to hold
//! the requested amount of data.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This function will pack the individual bytes from the physical interface
//! into 32-bit words, which are then placed into the \e pui32Data array. Only
//! the first \e ui32NumBytes bytes in this array will contain valid data.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_spi_read(uint32_t ui32Module, uint32_t ui32ChipSelect,
                    uint32_t *pui32Data, uint32_t ui32NumBytes,
                    uint32_t ui32Options)
{
    //
    // Validate parameters
    //
    am_hal_debug_assert_msg(ui32Module < AM_REG_IOMSTR_NUM_MODULES,
                            "Trying to use an IOM module that doesn't exist.");
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 4096, "SPI transfer too big.");

    //
    // Check to see if queues have been enabled. If they are, we'll actually
    // switch to the queued interface.
    //
    if ( g_psIOMQueue[ui32Module].pui8Data != NULL )
    {
        //
        // If the queue is on, go ahead and add this transaction to the queue.
        //
        am_hal_iom_queue_spi_read(ui32Module, ui32ChipSelect, pui32Data,
                                  ui32NumBytes, ui32Options, 0);

        //
        // Wait until the transaction actually clears.
        //
        am_hal_iom_queue_flush(ui32Module);

        //
        // At this point, we've completed the transaction, and we can return.
        //
        return;
    }
    else
    {
        //
        // Otherwise, just perform a polled transaction.
        //
        am_hal_iom_spi_read_nq(ui32Module, ui32ChipSelect, pui32Data,
                               ui32NumBytes, ui32Options);
    }
}

//*****************************************************************************
//
//! @brief Perform a simple write to the SPI interface (without queuing)
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32ChipSelect - Chip-select number for this transaction.
//! @param pui32Data - Pointer to the bytes that will be sent.
//! @param ui32NumBytes - Number of bytes to send.
//! @param ui32Options - Additional SPI transfer options.
//!
//! This function performs SPI writes to a selected SPI device.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui8Data array.
//!
//! @return None.
//
//*****************************************************************************
uint32_t
am_hal_iom_spi_write_nq(uint32_t ui32Module, uint32_t ui32ChipSelect,
                        uint32_t *pui32Data, uint32_t ui32NumBytes,
                        uint32_t ui32Options)
{
    uint32_t ui32TransferSize;
    uint32_t ui32SpaceInFifo;
    uint32_t ui32IntConfig;
    uint32_t ui32Status = 1;

    //
    // Validate parameters
    //
    am_hal_debug_assert_msg(ui32Module < AM_REG_IOMSTR_NUM_MODULES,
                            "Trying to use an IOM module that doesn't exist.");
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 4096, "SPI transfer too big.");

    //
    // Wait until any earlier transactions have completed.
    //
    am_hal_iom_poll_complete(ui32Module);
    //
    // Disable interrupts so that we don't get any undesired interrupts.
    //
    ui32IntConfig = AM_REGn(IOMSTR, ui32Module, INTEN);
    AM_REGn(IOMSTR, ui32Module, INTEN) = 0;
    // Clear CMDCMP status
    AM_BFWn(IOMSTR, ui32Module, INTCLR, CMDCMP, 1);

    //
    // Figure out how many bytes we can write to the FIFO immediately.
    //
    ui32TransferSize = (ui32NumBytes <= AM_HAL_IOM_MAX_FIFO_SIZE ? ui32NumBytes :
                        AM_HAL_IOM_MAX_FIFO_SIZE);
    //
    // write our first word to the fifo.
    //

    am_hal_iom_fifo_write(ui32Module, pui32Data, ui32TransferSize);

    //
    // Start the write on the bus.
    //
    am_hal_iom_spi_cmd_run(AM_HAL_IOM_WRITE, ui32Module, ui32ChipSelect,
                           ui32NumBytes, ui32Options);

    //
    // Update the pointer and data counter.
    //
    ui32NumBytes -= ui32TransferSize;
    pui32Data += ui32TransferSize >> 2;

    //
    // Keep looping until we're out of bytes to send or command complete (error).
    //
    while ( ui32NumBytes && !AM_BFRn(IOMSTR, ui32Module, INTSTAT, CMDCMP) )
    {
        //
        // This will always return a multiple of four.
        //
        ui32SpaceInFifo =  am_hal_iom_fifo_empty_slots(ui32Module);

        if ( ui32NumBytes <= ui32SpaceInFifo )
        {
            //
            // If the entire message will fit in the fifo, prepare to copy
            // everything.
            //
            ui32TransferSize = ui32NumBytes;
        }
        else
        {
            //
            // If only a portion of the message will fit in the fifo, prepare
            // to copy the largest number of 4-byte blocks possible.
            //
            ui32TransferSize = ui32SpaceInFifo & ~(0x3);
        }

        //
        // Write this chunk to the fifo.
        //
        am_hal_iom_fifo_write(ui32Module, pui32Data, ui32TransferSize);

        //
        // Update the data pointer and bytes-left count.
        //
        ui32NumBytes -= ui32TransferSize;
        pui32Data += ui32TransferSize >> 2;
    }

    //
    // Make sure CMDCMP was raised with standard timeout
    //
    ui32Status = am_util_wait_status_change(ui32StatusTimeout[ui32Module],
    		AM_REG_IOMSTRn(ui32Module) + AM_REG_IOMSTR_INTSTAT_O,
    		AM_REG_IOMSTR_INTEN_CMDCMP_M, AM_REG_IOMSTR_INTEN_CMDCMP_M);

    //
    // Re-enable IOM interrupts. Make sure CMDCMP is cleared
    //
    AM_REGn(IOMSTR, ui32Module, INTCLR) = (ui32IntConfig | AM_REG_IOMSTR_INTSTAT_CMDCMP_M);
    AM_REGn(IOMSTR, ui32Module, INTEN) = ui32IntConfig;

    am_hal_debug_assert_msg(ui32Status == 1,"IOM CMDCMP was not seen");
    
    //
    // Return the status (0 = timeout; 1 = success)
    //
    return ui32Status;

}

//*****************************************************************************
//
//! @brief Perform simple SPI read operations (without queuing).
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32ChipSelect - Chip-select number for this transaction.
//! @param pui32Data - Pointer to the array where received bytes should go.
//! @param ui32NumBytes - Number of bytes to read.
//! @param ui32Options - Additional SPI transfer options.
//!
//! This function performs simple SPI read operations. The caller is
//! responsible for ensuring that the receive buffer is large enough to hold
//! the requested amount of data.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This function will pack the individual bytes from the physical interface
//! into 32-bit words, which are then placed into the \e pui32Data array. Only
//! the first \e ui32NumBytes bytes in this array will contain valid data.
//!
//! @return None.
//
//*****************************************************************************
uint32_t
am_hal_iom_spi_read_nq(uint32_t ui32Module, uint32_t ui32ChipSelect,
                       uint32_t *pui32Data, uint32_t ui32NumBytes,
                       uint32_t ui32Options)
{
    uint32_t ui32BytesInFifo;
    uint32_t ui32IntConfig;
    uint32_t bCmdCmp = false;
    uint32_t ui32Status = 1;

    //
    // Validate parameters
    //
    am_hal_debug_assert_msg(ui32Module < AM_REG_IOMSTR_NUM_MODULES,
                            "Trying to use an IOM module that doesn't exist.");
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 4096, "SPI transfer too big.");

    //
    // Wait until the bus is idle, then start the requested READ transfer on
    // the physical interface.
    //
    am_hal_iom_poll_complete(ui32Module);

    //
    // Disable interrupts so that we don't get any undesired interrupts.
    //
    ui32IntConfig = AM_REGn(IOMSTR, ui32Module, INTEN);

    //
    // Disable IOM interrupts as we'll be polling
    //
    AM_REGn(IOMSTR, ui32Module, INTEN) = 0;

    //
    // Clear CMDCMP status
    //
    AM_BFWn(IOMSTR, ui32Module, INTCLR, CMDCMP, 1);

    am_hal_iom_spi_cmd_run(AM_HAL_IOM_READ, ui32Module, ui32ChipSelect,
                           ui32NumBytes, ui32Options);

    //
    // Start a loop to catch the Rx data.
    //
    while ( ui32NumBytes )
    {
        ui32BytesInFifo =  am_hal_iom_fifo_full_slots(ui32Module);

        if ( ui32BytesInFifo >= ui32NumBytes )
        {
            //
            // If the fifo contains our entire message, just copy the whole
            // thing out.
            //
            am_hal_iom_fifo_read(ui32Module, pui32Data, ui32NumBytes);
            ui32NumBytes = 0;
        }
        else if ( ui32BytesInFifo >= 4 )
        {
            //
            // If the fifo has at least one 32-bit word in it, copy whole
            // words out.
            //
            am_hal_iom_fifo_read(ui32Module, pui32Data, ui32BytesInFifo & ~0x3);
            ui32NumBytes -= ui32BytesInFifo & ~0x3;
            pui32Data += ui32BytesInFifo >> 2;
        }
        if ( bCmdCmp == true )
        {
            //
            // No more data expected. Get out of the loop
            //
            break;
        }

        bCmdCmp = AM_BFRn(IOMSTR, ui32Module, INTSTAT, CMDCMP);
    }

    //
    // Make sure CMDCMP was raised,
    //
    ui32Status = am_util_wait_status_change(ui32StatusTimeout[ui32Module],
    		AM_REG_IOMSTRn(ui32Module) + AM_REG_IOMSTR_INTSTAT_O,
    		AM_REG_IOMSTR_INTEN_CMDCMP_M, AM_REG_IOMSTR_INTEN_CMDCMP_M);

    //
    // Re-enable IOM interrupts. Make sure CMDCMP is cleared
    //
    AM_REGn(IOMSTR, ui32Module, INTCLR) = (ui32IntConfig | AM_REG_IOMSTR_INTSTAT_CMDCMP_M);
    AM_REGn(IOMSTR, ui32Module, INTEN) = ui32IntConfig;

    am_hal_debug_assert_msg(ui32Status == 1,"IOM CMDCMP was not seen");
    
    //
    // Return the status (0 = timeout; 1 = success)
    //
    return ui32Status;
}

//*****************************************************************************
//
//! @brief Perform a non-blocking write to the SPI interface.
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32ChipSelect - Chip-select number for this transaction.
//! @param pui32Data - Pointer to the bytes that will be sent.
//! @param ui32NumBytes - Number of bytes to send.
//! @param ui32Options - Additional SPI transfer options.
//! @param pfnCallback - Function to call when the transaction completes.
//!
//! This function performs SPI writes to the selected SPI device.
//!
//! This function call is a non-blocking implementation. It will write as much
//! data to the FIFO as possible immediately, store a pointer to the remaining
//! data, start the transfer on the bus, and then immediately return. The
//! caller will need to make sure that \e am_hal_iom_int_service() is called
//! for IOM FIFO interrupt events and "command complete" interrupt events. The
//! \e am_hal_iom_int_service() function will refill the FIFO as necessary and
//! call the \e pfnCallback function when the transaction is finished.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui8Data array.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_spi_write_nb(uint32_t ui32Module, uint32_t ui32ChipSelect,
                        uint32_t *pui32Data, uint32_t ui32NumBytes,
                        uint32_t ui32Options,
                        am_hal_iom_callback_t pfnCallback)
{
    uint32_t ui32TransferSize;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 4096, "SPI transfer too big.");
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");


    //
    // Wait until the bus is idle
    //
    am_hal_iom_poll_complete(ui32Module);

    //
    // Need to mark IOM busy to avoid another transaction to be scheduled.
    // This is to take care of a race condition in Queue mode, where the IDLE
    // set is not a guarantee that the CMDCMP has been received
    //
    g_bIomBusy[ui32Module] = true;

    //
    // Clear CMDCMP status
    //
    AM_BFWn(IOMSTR, ui32Module, INTCLR, CMDCMP, 1);

    //
    // Figure out how many bytes we can write to the FIFO immediately.
    //
    ui32TransferSize = (ui32NumBytes <= AM_HAL_IOM_MAX_FIFO_SIZE ? ui32NumBytes :
                        AM_HAL_IOM_MAX_FIFO_SIZE);

    if ( am_hal_iom_fifo_write(ui32Module, pui32Data, ui32TransferSize) > 0 )
    {
        //
        // Prepare the global IOM buffer structure.
        //
        g_psIOMBuffers[ui32Module].ui32State = BUFFER_SENDING;
        g_psIOMBuffers[ui32Module].pui32Data = pui32Data;
        g_psIOMBuffers[ui32Module].ui32BytesLeft = ui32NumBytes;
        g_psIOMBuffers[ui32Module].pfnCallback = pfnCallback;
        g_psIOMBuffers[ui32Module].ui32Options = ui32Options;

        //
        // Update the pointer and the byte counter based on the portion of
        // the transfer we just sent to the fifo.
        //
        g_psIOMBuffers[ui32Module].ui32BytesLeft -= ui32TransferSize;
        g_psIOMBuffers[ui32Module].pui32Data += (ui32TransferSize / 4);

        //
        // Start the write on the bus.
        //
        am_hal_iom_spi_cmd_run(AM_HAL_IOM_WRITE, ui32Module, ui32ChipSelect,
                               ui32NumBytes, ui32Options);
    }
}

//*****************************************************************************
//
//! @brief Perform a non-blocking SPI read.
//!
//! @param ui32Module - Module number for the IOM.
//! @param ui32ChipSelect - Chip select number of the target device.
//! @param pui32Data - Pointer to the array where received bytes should go.
//! @param ui32NumBytes - Number of bytes to read.
//! @param ui32Options - Additional SPI transfer options.
//! @param pfnCallback - Function to call when the transaction completes.
//!
//! This function performs SPI reads to a selected SPI device.
//!
//! This function call is a non-blocking implementation. It will start the SPI
//! transaction on the bus and store a pointer for the destination for the read
//! data, but it will not wait for the SPI transaction to finish.  The caller
//! will need to make sure that \e am_hal_iom_int_service() is called for IOM
//! FIFO interrupt events and "command complete" interrupt events. The \e
//! am_hal_iom_int_service() function will empty the FIFO as necessary,
//! transfer the data to the \e pui32Data buffer, and call the \e pfnCallback
//! function when the transaction is finished.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This function will pack the individual bytes from the physical interface
//! into 32-bit words, which are then placed into the \e pui32Data array. Only
//! the first \e ui32NumBytes bytes in this array will contain valid data.
//!
//! @return None.
//
//*****************************************************************************
uint32_t
am_hal_iom_spi_read_nb(uint32_t ui32Module, uint32_t ui32ChipSelect,
                       uint32_t *pui32Data, uint32_t ui32NumBytes,
                       uint32_t ui32Options,
                       am_hal_iom_callback_t pfnCallback)
{
    uint32_t ui32Status = 1;
    
    //
    // Validate parameters
    //
    am_hal_debug_assert_msg(ui32Module < AM_REG_IOMSTR_NUM_MODULES,
                            "Trying to use an IOM module that doesn't exist.");
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 4096, "SPI transfer too big.");

    //
    // Wait until the bus is idle
    //
    am_hal_iom_poll_complete(ui32Module);

    //
    // Need to mark IOM busy to avoid another transaction to be scheduled.
    // This is to take care of a race condition in Queue mode, where the IDLE
    // set is not a guarantee that the CMDCMP has been received
    //

    g_bIomBusy[ui32Module] = true;

    //
    // Clear CMDCMP status
    //
    AM_BFWn(IOMSTR, ui32Module, INTCLR, CMDCMP, 1);

    //
    // Prepare the global IOM buffer structure.
    //
    g_psIOMBuffers[ui32Module].ui32State = BUFFER_RECEIVING;
    g_psIOMBuffers[ui32Module].pui32Data = pui32Data;
    g_psIOMBuffers[ui32Module].ui32BytesLeft = ui32NumBytes;
    g_psIOMBuffers[ui32Module].pfnCallback = pfnCallback;
    g_psIOMBuffers[ui32Module].ui32Options = ui32Options;

    //
    // Start the read transaction on the bus.
    //
    am_hal_iom_spi_cmd_run(AM_HAL_IOM_READ, ui32Module, ui32ChipSelect,
                           ui32NumBytes, ui32Options);
    
    am_hal_debug_assert_msg(ui32Status == 1,"IOM CMDCMP was not seen");
    
    return ui32Status;
}

static uint32_t
internal_am_hal_iom_spi_cmd_construct(uint32_t ui32Operation,
                                      uint32_t ui32ChipSelect,
                                      uint32_t ui32NumBytes,
                                      uint32_t ui32Options)
{
    uint32_t ui32Command;
    //
    // Start building the command from the operation parameter.
    //
    ui32Command = ui32Operation;

    //
    // Set the transfer length (the length field is split, so this requires
    // some swizzling).
    //
    ui32Command |= ((ui32NumBytes & 0xF00) << 15);
    ui32Command |= (ui32NumBytes & 0xFF);

    //
    // Set the chip select number.
    //
    ui32Command |= ((ui32ChipSelect << 16) & 0x00070000);

    //
    // Finally, OR in the rest of the options. This mask should make sure that
    // erroneous option values won't interfere with the other transfer
    // parameters.
    //
    ui32Command |= ui32Options & 0x5C00FF00;
    return ui32Command;
}
//*****************************************************************************
//
//! @brief Runs a SPI "command" through the IO master.
//!
//! @param ui32Operation - SPI action to be performed.
//!
//! @param psDevice - Structure containing information about the slave device.
//!
//! @param ui32NumBytes - Number of bytes to move (transmit or receive) with
//! this command.
//!
//! @param ui32Options - Additional SPI options to apply to this command.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_spi_cmd_run(uint32_t ui32Operation, uint32_t ui32Module,
                       uint32_t ui32ChipSelect, uint32_t ui32NumBytes,
                       uint32_t ui32Options)
{
    uint32_t ui32Command;

    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");
    ui32Command = internal_am_hal_iom_spi_cmd_construct(ui32Operation,
                        ui32ChipSelect, ui32NumBytes, ui32Options);


    //
    // Write the complete command word to the IOM command register.
    //
    AM_REGn(IOMSTR, ui32Module, CMD) = ui32Command;
}

//*****************************************************************************
//
//! @brief Perform a simple write to the I2C interface (without queuing)
//!
//! @param ui32Module - Module number for the IOM.
//! @param ui32BusAddress - I2C address of the target device.
//! @param pui32Data - Pointer to the bytes that will be sent.
//! @param ui32NumBytes - Number of bytes to send.
//! @param ui32Options - Additional I2C transfer options.
//!
//! This function performs I2C writes to a selected I2C device.
//!
//! This function call is a blocking implementation. It will write as much
//! data to the FIFO as possible immediately, and then refill the FIFO as data
//! is transmiitted.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui32Data array.
//!
//! @return None.
//
//*****************************************************************************
uint32_t
am_hal_iom_i2c_write_nq(uint32_t ui32Module, uint32_t ui32BusAddress,
                        uint32_t *pui32Data, uint32_t ui32NumBytes,
                        uint32_t ui32Options)
{
    uint32_t ui32TransferSize;
    uint32_t ui32SpaceInFifo;
    uint32_t ui32IntConfig;
    uint32_t ui32Status = 1;

    //
    // Validate parameters
    //
    if ( ui32Module > AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Redirect to the bit-bang interface if the module number matches the
    // software I2C module.
    //
    if ( ui32Module == AM_HAL_IOM_I2CBB_MODULE )
    {
        if ( ui32Options & AM_HAL_IOM_RAW )
        {
            am_hal_i2c_bit_bang_send(ui32BusAddress << 1, ui32NumBytes,
                                     (uint8_t *)pui32Data, 0, false,
                                     (ui32Options & AM_HAL_IOM_NO_STOP));
        }
        else
        {
            am_hal_i2c_bit_bang_send(ui32BusAddress << 1, ui32NumBytes,
                                     (uint8_t *)pui32Data,
                                     ((ui32Options & 0xFF00) >> 8),
                                     true,
                                     (ui32Options & AM_HAL_IOM_NO_STOP));
        }

        //
        // Return.
        //
        return 0;
    }

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 256, "I2C transfer too big.");


    //
    // Wait until any earlier transactions have completed.
    //
    am_hal_iom_poll_complete(ui32Module);

    //
    // Disable interrupts so that we don't get any undesired interrupts.
    //
    ui32IntConfig = AM_REGn(IOMSTR, ui32Module, INTEN);
    AM_REGn(IOMSTR, ui32Module, INTEN) = 0;

    //
    // Clear CMDCMP status
    //
    AM_BFWn(IOMSTR, ui32Module, INTCLR, CMDCMP, 1);

    //
    // Figure out how many bytes we can write to the FIFO immediately.
    //
    ui32TransferSize = (ui32NumBytes <= AM_HAL_IOM_MAX_FIFO_SIZE ? ui32NumBytes :
                        AM_HAL_IOM_MAX_FIFO_SIZE);

    am_hal_iom_fifo_write(ui32Module, pui32Data, ui32TransferSize);

    //
    // Start the write on the bus.
    //
    am_hal_iom_i2c_cmd_run(AM_HAL_IOM_WRITE, ui32Module, ui32BusAddress,
                           ui32NumBytes, ui32Options);

    //
    // Update the pointer and data counter.
    //
    ui32NumBytes -= ui32TransferSize;
    pui32Data += ui32TransferSize >> 2;

    //
    // Keep looping until we're out of bytes to send or command complete (error).
    //
    while ( ui32NumBytes && !AM_BFRn(IOMSTR, ui32Module, INTSTAT, CMDCMP) )
    {
        //
        // This will always return a multiple of four.
        //
        ui32SpaceInFifo =  am_hal_iom_fifo_empty_slots(ui32Module);

        if ( ui32NumBytes <= ui32SpaceInFifo )
        {
            //
            // If the entire message will fit in the fifo, prepare to copy
            // everything.
            //
            ui32TransferSize = ui32NumBytes;
        }
        else
        {
            //
            // If only a portion of the message will fit in the fifo, prepare
            // to copy the largest number of 4-byte blocks possible.
            //
            ui32TransferSize = ui32SpaceInFifo;
        }

        //
        // Write this chunk to the fifo.
        //
        am_hal_iom_fifo_write(ui32Module, pui32Data, ui32TransferSize);

        //
        // Update the data pointer and bytes-left count.
        //
        ui32NumBytes -= ui32TransferSize;
        pui32Data += ui32TransferSize >> 2;
    }

    //
    // Make sure CMDCMP was raised,
    //
    ui32Status = am_util_wait_status_change(ui32StatusTimeout[ui32Module],
    		AM_REG_IOMSTRn(ui32Module) + AM_REG_IOMSTR_INTSTAT_O,
    		AM_REG_IOMSTR_INTEN_CMDCMP_M, AM_REG_IOMSTR_INTEN_CMDCMP_M);

    //
    // Re-enable IOM interrupts. Make sure CMDCMP is cleared
    //
    AM_REGn(IOMSTR, ui32Module, INTCLR) = (ui32IntConfig | AM_REG_IOMSTR_INTSTAT_CMDCMP_M);
    AM_REGn(IOMSTR, ui32Module, INTEN) = ui32IntConfig;
    
    am_hal_debug_assert_msg(ui32Status == 1,"IOM CMDCMP was not seen");
    
    //
    // Return the status (0 = timeout; 1 = success)
    //
    return ui32Status;
}

//*****************************************************************************
//
//! @brief Perform simple I2C read operations (without queuing).
//!
//! @param ui32Module - Module number for the IOM.
//! @param ui32BusAddress - I2C address of the target device.
//! @param pui32Data - Pointer to the array where received bytes should go.
//! @param ui32NumBytes - Number of bytes to read.
//! @param ui32Options - Additional I2C transfer options.
//!
//! This function performs an I2C read to a selected I2C device.
//!
//! This function call is a blocking implementation. It will read as much
//! data from the FIFO as possible immediately, and then re-read the FIFO as more
//! data is available.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This function will pack the individual bytes from the physical interface
//! into 32-bit words, which are then placed into the \e pui32Data array. Only
//! the first \e ui32NumBytes bytes in this array will contain valid data.
//!
//! @return None.
//
//*****************************************************************************
uint32_t
am_hal_iom_i2c_read_nq(uint32_t ui32Module, uint32_t ui32BusAddress,
                       uint32_t *pui32Data, uint32_t ui32NumBytes,
                       uint32_t ui32Options)
{
    uint32_t ui32BytesInFifo;
    uint32_t ui32IntConfig;
    uint32_t bCmdCmp = false;
    uint32_t ui32Status = 1;

    //
    // Validate parameters
    //
    if ( ui32Module > AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Redirect to the bit-bang interface if the module number matches the
    // software I2C module.
    //
    if ( ui32Module == AM_HAL_IOM_I2CBB_MODULE )
    {
        if ( ui32Options & AM_HAL_IOM_RAW )
        {
            am_hal_i2c_bit_bang_receive((ui32BusAddress << 1) | 1, ui32NumBytes,
                                        (uint8_t *)pui32Data, 0, false,
                                        (ui32Options & AM_HAL_IOM_NO_STOP));
        }
        else
        {
            am_hal_i2c_bit_bang_receive((ui32BusAddress << 1) | 1, ui32NumBytes,
                                        (uint8_t *)pui32Data,
                                        ((ui32Options & 0xFF00) >> 8),
                                        true,
                                        (ui32Options & AM_HAL_IOM_NO_STOP));
        }

        //
        // Return.
        //
        return 0;
    }

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 256, "I2C transfer too big.");

    //
    // Wait until the bus is idle
    //
    am_hal_iom_poll_complete(ui32Module);

    //
    // Disable interrupts so that we don't get any undesired interrupts.
    //
    ui32IntConfig = AM_REGn(IOMSTR, ui32Module, INTEN);
    AM_REGn(IOMSTR, ui32Module, INTEN) = 0;

    //
    // Clear CMDCMP status
    //
    AM_BFWn(IOMSTR, ui32Module, INTCLR, CMDCMP, 1);

    am_hal_iom_i2c_cmd_run(AM_HAL_IOM_READ, ui32Module, ui32BusAddress,
                           ui32NumBytes, ui32Options);

    //
    // Start a loop to catch the Rx data.
    //
    while ( ui32NumBytes )
    {
        ui32BytesInFifo =  am_hal_iom_fifo_full_slots(ui32Module);

        if ( ui32BytesInFifo >= ui32NumBytes )
        {
            //
            // If the fifo contains our entire message, just copy the whole
            // thing out.
            //
            am_hal_iom_fifo_read(ui32Module, pui32Data, ui32NumBytes);
            ui32NumBytes = 0;
        }
        else if ( ui32BytesInFifo >= 4 )
        {
            //
            // If the fifo has at least one 32-bit word in it, copy whole
            // words out.
            //
            am_hal_iom_fifo_read(ui32Module, pui32Data, ui32BytesInFifo & ~0x3);

            ui32NumBytes -= ui32BytesInFifo & ~0x3;
            pui32Data += ui32BytesInFifo >> 2;
        }

        if ( bCmdCmp == true )
        {
            // No more data expected - exit out of loop
            break;
        }

        bCmdCmp = AM_BFRn(IOMSTR, ui32Module, INTSTAT, CMDCMP);
    }

    //
    // Make sure CMDCMP was raised,
    //
    ui32Status = am_util_wait_status_change(ui32StatusTimeout[ui32Module],
    		AM_REG_IOMSTRn(ui32Module) + AM_REG_IOMSTR_INTSTAT_O,
    		AM_REG_IOMSTR_INTEN_CMDCMP_M, AM_REG_IOMSTR_INTEN_CMDCMP_M);

    //
    // Re-enable IOM interrupts. Make sure CMDCMP is cleared
    //
    AM_REGn(IOMSTR, ui32Module, INTCLR) = (ui32IntConfig | AM_REG_IOMSTR_INTSTAT_CMDCMP_M);
    AM_REGn(IOMSTR, ui32Module, INTEN) = ui32IntConfig;
    
    am_hal_debug_assert_msg(ui32Status == 1,"IOM CMDCMP was not seen");
    
    //
    // Return the status (0 = timeout; 1 = success)
    //
    return ui32Status;
}

//*****************************************************************************
//
//! @brief Perform a simple write to the I2C interface.
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32BusAddress - I2C bus address for this transaction.
//! @param pui32Data - Pointer to the bytes that will be sent.
//! @param ui32NumBytes - Number of bytes to send.
//! @param ui32Options - Additional options
//!
//! Performs a write to the I2C interface using the provided parameters.
//!
//! See the "Command Options" section for parameters that may be ORed together
//! and used in the \b ui32Options parameter.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_i2c_write(uint32_t ui32Module, uint32_t ui32BusAddress,
                     uint32_t *pui32Data, uint32_t ui32NumBytes,
                     uint32_t ui32Options)
{
    //
    // Validate parameters
    //
    if ( ui32Module > AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Redirect to the bit-bang interface if the module number matches the
    // software I2C module.
    //
    if ( ui32Module == AM_HAL_IOM_I2CBB_MODULE )
    {
        if ( ui32Options & AM_HAL_IOM_RAW )
        {
            am_hal_i2c_bit_bang_send(ui32BusAddress << 1, ui32NumBytes,
                                     (uint8_t *)pui32Data, 0, false,
                                     (ui32Options & AM_HAL_IOM_NO_STOP));
        }
        else
        {
            am_hal_i2c_bit_bang_send(ui32BusAddress << 1, ui32NumBytes,
                                     (uint8_t *)pui32Data,
                                     ((ui32Options & 0xFF00) >> 8),
                                     true,
                                     (ui32Options & AM_HAL_IOM_NO_STOP));
        }

        //
        // Return.
        //
        return;
    }

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 256, "I2C transfer too big.");

    //
    // Check to see if queues have been enabled. If they are, we'll actually
    // switch to the queued interface.
    //
    if ( g_psIOMQueue[ui32Module].pui8Data != NULL )
    {
        //
        // If the queue is on, go ahead and add this transaction to the queue.
        //
        am_hal_iom_queue_i2c_write(ui32Module, ui32BusAddress, pui32Data,
                                   ui32NumBytes, ui32Options, 0);

        //
        // Wait until the transaction actually clears.
        //
        am_hal_iom_queue_flush(ui32Module);

        //
        // At this point, we've completed the transaction, and we can return.
        //
        return;
    }
    else
    {
        //
        // Otherwise, we'll just do a polled transaction.
        //
        am_hal_iom_i2c_write_nq(ui32Module, ui32BusAddress, pui32Data,
                                ui32NumBytes, ui32Options);
    }
}

//*****************************************************************************
//
//! @brief Perform simple I2C read operations.
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32BusAddress - I2C bus address for this transaction.
//! @param pui32Data - Pointer to the array where received bytes should go.
//! @param ui32NumBytes - Number of bytes to read.
//! @param ui32Options - Additional I2C transfer options.
//!
//! This function performs simple I2C read operations. The caller is
//! responsible for ensuring that the receive buffer is large enough to hold
//! the requested amount of data. If \e bPolled is true, this function will
//! block until all of the requested data has been received and placed in the
//! user-supplied buffer. Otherwise, the function will execute the I2C read
//! command and return immediately. The user-supplied buffer will be filled
//! with the received I2C data as it comes in over the physical interface, and
//! the "command complete" interrupt bit will become active once the entire
//! message is available.
//!
//! See the "Command Options" section for parameters that may be ORed together
//! and used in the \b ui32Options parameter.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This function will pack the individual bytes from the physical interface
//! into 32-bit words, which are then placed into the \e pui32Data array. Only
//! the first \e ui32NumBytes bytes in this array will contain valid data.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_i2c_read(uint32_t ui32Module, uint32_t ui32BusAddress,
                    uint32_t *pui32Data, uint32_t ui32NumBytes,
                    uint32_t ui32Options)
{
    //
    // Validate parameters
    //
    if ( ui32Module > AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Redirect to the bit-bang interface if the module number matches the
    // software I2C module.
    //
    if ( ui32Module == AM_HAL_IOM_I2CBB_MODULE )
    {
        if ( ui32Options & AM_HAL_IOM_RAW )
        {
            am_hal_i2c_bit_bang_receive((ui32BusAddress << 1) | 1, ui32NumBytes,
                                        (uint8_t *)pui32Data, 0, false,
                                        (ui32Options & AM_HAL_IOM_NO_STOP));
        }
        else
        {
            am_hal_i2c_bit_bang_receive((ui32BusAddress << 1) | 1, ui32NumBytes,
                                        (uint8_t *)pui32Data,
                                        ((ui32Options & 0xFF00) >> 8),
                                        true,
                                        (ui32Options & AM_HAL_IOM_NO_STOP));
        }

        //
        // Return.
        //
        return;
    }

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 256, "I2C transfer too big.");

    //
    // Check to see if queues have been enabled. If they are, we'll actually
    // switch to the queued interface.
    //
    if ( g_psIOMQueue[ui32Module].pui8Data != NULL )
    {
        //
        // If the queue is on, go ahead and add this transaction to the queue.
        //
        am_hal_iom_queue_i2c_read(ui32Module, ui32BusAddress, pui32Data,
                                  ui32NumBytes, ui32Options, 0);

        //
        // Wait until the transaction actually clears.
        //
        am_hal_iom_queue_flush(ui32Module);

        //
        // At this point, we've completed the transaction, and we can return.
        //
        return;
    }
    else
    {
        //
        // Otherwise, just perform a polled transaction.
        //
        am_hal_iom_i2c_read_nq(ui32Module, ui32BusAddress, pui32Data,
                               ui32NumBytes, ui32Options);
    }
}

//*****************************************************************************
//
//! @brief Perform a non-blocking write to the I2C interface.
//!
//! @param ui32Module - Module number for the IOM.
//! @param ui32BusAddress - I2C address of the target device.
//! @param pui32Data - Pointer to the bytes that will be sent.
//! @param ui32NumBytes - Number of bytes to send.
//! @param ui32Options - Additional I2C transfer options.
//! @param pfnCallback - Function to call when the transaction completes.
//!
//! This function performs I2C writes to a selected I2C device.
//!
//! This function call is a non-blocking implementation. It will write as much
//! data to the FIFO as possible immediately, store a pointer to the remaining
//! data, start the transfer on the bus, and then immediately return. The
//! caller will need to make sure that \e am_hal_iom_int_service() is called
//! for IOM FIFO interrupt events and "command complete" interrupt events. The
//! \e am_hal_iom_int_service() function will refill the FIFO as necessary and
//! call the \e pfnCallback function when the transaction is finished.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui32Data array.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_i2c_write_nb(uint32_t ui32Module, uint32_t ui32BusAddress,
                        uint32_t *pui32Data, uint32_t ui32NumBytes,
                        uint32_t ui32Options,
                        am_hal_iom_callback_t pfnCallback)
{
    uint32_t ui32TransferSize;

    //
    // Validate parameters
    //
    if ( ui32Module > AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Redirect to the bit-bang interface if the module number matches the
    // software I2C module.
    //
    if ( ui32Module == AM_HAL_IOM_I2CBB_MODULE )
    {
        if ( ui32Options & AM_HAL_IOM_RAW )
        {
            am_hal_i2c_bit_bang_send(ui32BusAddress << 1, ui32NumBytes,
                                     (uint8_t *)pui32Data, 0, false,
                                     (ui32Options & AM_HAL_IOM_NO_STOP));
        }
        else
        {
            am_hal_i2c_bit_bang_send(ui32BusAddress << 1, ui32NumBytes,
                                     (uint8_t *)pui32Data,
                                     ((ui32Options & 0xFF00) >> 8),
                                     true,
                                     (ui32Options & AM_HAL_IOM_NO_STOP));
        }

        //
        // The I2C bit-bang interface is actually a blocking transfer, and it
        // doesn't trigger the interrupt handler, so we have to call the
        // callback function manually.
        //
        if ( pfnCallback )
        {
            pfnCallback();
        }
        //
        // Return.
        //
        return;
    }

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 256, "I2C transfer too big.");


    //
    // Figure out how many bytes we can write to the FIFO immediately.
    //
    ui32TransferSize = (ui32NumBytes <= AM_HAL_IOM_MAX_FIFO_SIZE ? ui32NumBytes :
                        AM_HAL_IOM_MAX_FIFO_SIZE);

    //
    // Wait until any earlier transactions have completed, and then write our
    // first word to the fifo.
    //
    am_hal_iom_poll_complete(ui32Module);

    // Need to mark IOM busy to avoid another transaction to be scheduled.
    // This is to take care of a race condition in Queue mode, where the IDLE
    // set is not a guarantee that the CMDCMP has been received
    g_bIomBusy[ui32Module] = true;

    //
    // Clear CMDCMP status
    //
    AM_BFWn(IOMSTR, ui32Module, INTCLR, CMDCMP, 1);

    if ( am_hal_iom_fifo_write(ui32Module, pui32Data, ui32TransferSize) > 0 )
    {
        //
        // Prepare the global IOM buffer structure.
        //
        g_psIOMBuffers[ui32Module].ui32State = BUFFER_SENDING;
        g_psIOMBuffers[ui32Module].pui32Data = pui32Data;
        g_psIOMBuffers[ui32Module].ui32BytesLeft = ui32NumBytes;
        g_psIOMBuffers[ui32Module].pfnCallback = pfnCallback;

        //
        // Update the pointer and the byte counter based on the portion of the
        // transfer we just sent to the fifo.
        //
        g_psIOMBuffers[ui32Module].ui32BytesLeft -= ui32TransferSize;
        g_psIOMBuffers[ui32Module].pui32Data += (ui32TransferSize / 4);

        //
        // Start the write on the bus.
        //
        am_hal_iom_i2c_cmd_run(AM_HAL_IOM_WRITE, ui32Module, ui32BusAddress,
                               ui32NumBytes, ui32Options);
    }
}

//*****************************************************************************
//
//! @brief Perform a non-blocking I2C read.
//!
//! @param ui32Module - Module number for the IOM.
//! @param ui32ChipSelect - I2C address of the target device.
//! @param pui32Data - Pointer to the array where received bytes should go.
//! @param ui32NumBytes - Number of bytes to read.
//! @param ui32Options - Additional I2C transfer options.
//! @param pfnCallback - Function to call when the transaction completes.
//!
//! This function performs an I2C read to a selected I2C device.
//!
//! This function call is a non-blocking implementation. It will start the I2C
//! transaction on the bus and store a pointer for the destination for the read
//! data, but it will not wait for the I2C transaction to finish.  The caller
//! will need to make sure that \e am_hal_iom_int_service() is called for IOM
//! FIFO interrupt events and "command complete" interrupt events. The \e
//! am_hal_iom_int_service() function will empty the FIFO as necessary,
//! transfer the data to the \e pui32Data buffer, and call the \e pfnCallback
//! function when the transaction is finished.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This function will pack the individual bytes from the physical interface
//! into 32-bit words, which are then placed into the \e pui32Data array. Only
//! the first \e ui32NumBytes bytes in this array will contain valid data.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_i2c_read_nb(uint32_t ui32Module, uint32_t ui32BusAddress,
                       uint32_t *pui32Data, uint32_t ui32NumBytes,
                       uint32_t ui32Options,
                       am_hal_iom_callback_t pfnCallback)
{
    //
    // Validate parameters
    //
    if ( ui32Module > AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Redirect to the bit-bang interface if the module number matches the
    // software I2C module.
    //
    if ( ui32Module == AM_HAL_IOM_I2CBB_MODULE )
    {
        if ( ui32Options & AM_HAL_IOM_RAW )
        {
            am_hal_i2c_bit_bang_receive((ui32BusAddress << 1) | 1, ui32NumBytes,
                                        (uint8_t *)pui32Data, 0, false,
                                        (ui32Options & AM_HAL_IOM_NO_STOP));
        }
        else
        {
            am_hal_i2c_bit_bang_receive((ui32BusAddress << 1) | 1, ui32NumBytes,
                                        (uint8_t *)pui32Data,
                                        ((ui32Options & 0xFF00) >> 8),
                                        true,
                                        (ui32Options & AM_HAL_IOM_NO_STOP));
        }

        //
        // The I2C bit-bang interface is actually a blocking transfer, and it
        // doesn't trigger the interrupt handler, so we have to call the
        // callback function manually.
        //
        if ( pfnCallback )
        {
            pfnCallback();
        }

        //
        // Return.
        //
        return;
    }

    //
    // Make sure the transfer isn't too long for the hardware to support.
    //
    am_hal_debug_assert_msg(ui32NumBytes < 256, "I2C transfer too big.");

    //
    // Wait until the bus is idle
    //
    am_hal_iom_poll_complete(ui32Module);

    //
    // Need to mark IOM busy to avoid another transaction to be scheduled.
    // This is to take care of a race condition in Queue mode, where the IDLE
    // set is not a guarantee that the CMDCMP has been received
    //
    g_bIomBusy[ui32Module] = true;

    //
    // Clear CMDCMP status
    //
    AM_BFWn(IOMSTR, ui32Module, INTCLR, CMDCMP, 1);

    //
    // Prepare the global IOM buffer structure.
    //
    g_psIOMBuffers[ui32Module].ui32State = BUFFER_RECEIVING;
    g_psIOMBuffers[ui32Module].pui32Data = pui32Data;
    g_psIOMBuffers[ui32Module].ui32BytesLeft = ui32NumBytes;
    g_psIOMBuffers[ui32Module].pfnCallback = pfnCallback;

    //
    // Start the read transaction on the bus.
    //
    am_hal_iom_i2c_cmd_run(AM_HAL_IOM_READ, ui32Module, ui32BusAddress,
                           ui32NumBytes, ui32Options);
}

//*****************************************************************************
//
//! @brief Runs a I2C "command" through the IO master.
//!
//! @param ui32Operation - I2C action to be performed. This should either be
//! AM_HAL_IOM_WRITE or AM_HAL_IOM_READ.
//! @param psDevice - Structure containing information about the slave device.
//! @param ui32NumBytes - Number of bytes to move (transmit or receive) with
//! this command.
//! @param ui32Options - Additional I2C options to apply to this command.
//!
//! This function may be used along with am_hal_iom_fifo_write and
//! am_hal_iom_fifo_read to perform more complex I2C reads and writes. This
//! function
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_i2c_cmd_run(uint32_t ui32Operation, uint32_t ui32Module,
                       uint32_t ui32BusAddress, uint32_t ui32NumBytes,
                       uint32_t ui32Options)
{
    uint32_t ui32Command;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Start building the command from the operation parameter.
    //
    ui32Command = ui32Operation;

    //
    // Set the transfer length.
    //
    ui32Command |= (ui32NumBytes & 0xFF);

    //
    // Set the chip select number.
    //
    ui32Command |= ((ui32BusAddress << 16) & 0x03FF0000);

    //
    // Finally, OR in the rest of the options. This mask should make sure that
    // erroneous option values won't interfere with the other transfer
    // parameters.
    //
    ui32Command |= (ui32Options & 0x5C00FF00);

    //
    // Write the complete command word to the IOM command register.
    //
    AM_REGn(IOMSTR, ui32Module, CMD) = ui32Command;
}

//*****************************************************************************
//
//! @brief Sets the repeat count for the next IOM command.
//!
//! @param ui32Module is the IOM module number.
//! @param ui32CmdCount is the number of times the next command should be
//! executed.
//!
//! @note This function is not compatible with the am_hal_iom_spi_read/write()
//! or am_hal_iom_i2c_read/write() functions. Instead, you will need to use the
//! am_hal_iom_fifo_read/write() functions and the am_hal_iom_spi/i2c_cmd_run()
//! functions.
//!
//! Example usage:
//! @code
//!
//! //
//! // Create a buffer and add 3 bytes of data to it.
//! //
//! am_hal_iom_buffer(3) psBuffer;
//! psBuffer.bytes[0] = 's';
//! psBuffer.bytes[1] = 'p';
//! psBuffer.bytes[2] = 'i';
//!
//! //
//! // Send three different bytes to the same SPI register on a remote device.
//! //
//! am_hal_iom_fifo_write(ui32Module, psBuffer.words, 3);
//!
//! am_hal_command_repeat_set(ui32Module, 3);
//!
//! am_hal_iom_spi_cmd_run(AM_HAL_IOM_WRITE, psDevice, 1,
//!                        AM_HAL_IOM_OFFSET(0x5));
//!
//! //
//! // The sequence "0x5, 's', 0x5, 'p', 0x5, 'i'" should be written to the SPI
//! // bus.
//! //
//!
//! @endcode
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_command_repeat_set(uint32_t ui32Module, uint32_t ui32CmdCount)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    AM_REGn(IOMSTR, ui32Module, CMDRPT) = ui32CmdCount;
}

//*****************************************************************************
//
//! @brief Writes data to the IOM FIFO.
//!
//! @param ui32Module - Selects the IOM module to use (zero or one).
//! @param pui32Data - Pointer to an array of the data to be written.
//! @param ui32NumBytes - Number of BYTES to copy into the FIFO.
//!
//! This function copies data from the array \e pui32Data into the IOM FIFO.
//! This prepares the data to eventually be sent as SPI or I2C data by an IOM
//! "command".
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui8Data array.
//!
//! @note This function may be used to write partial or complete SPI or I2C
//! messages into the IOM FIFO. When writing partial messages to the FIFO, make
//! sure that the number of bytes written is a multiple of four. Only the last
//! 'part' of a message may consist of a number of bytes that is not a multiple
//! of four. If this rule is not followed, the IOM will not be able to send
//! these bytes correctly.
//!
//! @return Number of bytes actually written to the FIFO.
//
//*****************************************************************************
uint32_t
am_hal_iom_fifo_write(uint32_t ui32Module, uint32_t *pui32Data,
                      uint32_t ui32NumBytes)
{
    uint32_t ui32Index;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }

    //
    // Make sure we check the number of bytes we're writing to the FIFO.
    //
    am_hal_debug_assert_msg((am_hal_iom_fifo_empty_slots(ui32Module) >= ui32NumBytes),
                            "The fifo couldn't fit the requested number of bytes");

    //
    // Loop over the words in the array until we have the correct number of
    // bytes.
    //
    for ( ui32Index = 0; (4 * ui32Index) < ui32NumBytes; ui32Index++ )
    {
        //
        // Write the word to the FIFO.
        //
        AM_REGn(IOMSTR, ui32Module, FIFO) = pui32Data[ui32Index];
    }

    return ui32NumBytes;
}

//*****************************************************************************
//
//! @brief Reads data from the IOM FIFO.
//!
//! @param ui32Module - Selects the IOM module to use (zero or one).
//! @param pui32Data - Pointer to an array where the FIFO data will be copied.
//! @param ui32NumBytes - Number of bytes to copy into array.
//!
//! This function copies data from the IOM FIFO into the array \e pui32Data.
//! This is how input data from SPI or I2C transactions may be retrieved.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This function will pack the individual bytes from the physical interface
//! into 32-bit words, which are then placed into the \e pui32Data array. Only
//! the first \e ui32NumBytes bytes in this array will contain valid data.
//!
//! @return Number of bytes read from the fifo.
//
//*****************************************************************************
uint32_t
am_hal_iom_fifo_read(uint32_t ui32Module, uint32_t *pui32Data,
                     uint32_t ui32NumBytes)
{
    am_hal_iom_buffer(4) sTempBuffer;
    uint32_t i, j, ui32NumWords, ui32Leftovers;
    uint8_t *pui8Data;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }

    //
    // Make sure we check the number of bytes we're reading from the FIFO.
    //
    am_hal_debug_assert_msg((am_hal_iom_fifo_full_slots(ui32Module) >= ui32NumBytes),
                            "The fifo doesn't contain the requested number of bytes.");

    //
    // Figure out how many whole words we're reading from the fifo, and how
    // many bytes will be left over when we're done.
    //
    ui32NumWords = ui32NumBytes / 4;
    ui32Leftovers = ui32NumBytes - (ui32NumWords * 4);

    //
    // Copy out as many full words as we can.
    //
    for ( i = 0; i < ui32NumWords; i++ )
    {
        //
        // Copy data out of the FIFO, one word at a time.
        //
        pui32Data[i] = AM_REGn(IOMSTR, ui32Module, FIFO);
    }

    //
    // If there were leftovers, we'll copy them carefully. Pull the last word
    // from the fifo (there should only be one) into a temporary buffer. Also,
    // create an 8-bit pointer to help us copy the remaining bytes one at a
    // time.
    //
    // Note: If the data buffer we were given was truly a word pointer like the
    // definition requests, we wouldn't need to do this. It's possible to call
    // this function with a re-cast or packed pointer instead though. If that
    // happens, we want to be careful not to overwrite any data that might be
    // sitting just past the end of the destination array.
    //
    if ( ui32Leftovers )
    {
        sTempBuffer.words[0] = AM_REGn(IOMSTR, ui32Module, FIFO);
        pui8Data = (uint8_t *) (&pui32Data[i]);

        //
        // If we had leftover bytes, copy them out one byte at a time.
        //
        for ( j = 0; j < ui32Leftovers; j++ )
        {
            pui8Data[j] = sTempBuffer.bytes[j];
        }
    }

    return ui32NumBytes;
}

//*****************************************************************************
//
//! @brief Check amount of empty space in the IOM fifo.
//!
//! @param ui32Module - Module number of the IOM whose fifo should be checked.
//!
//! Returns the number of bytes that could be written to the IOM fifo without
//! causing an overflow.
//!
//! @return Amount of space available in the fifo (in bytes).
//
//*****************************************************************************
uint8_t
am_hal_iom_fifo_empty_slots(uint32_t ui32Module)
{

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }


    //
    // Calculate the FIFO Remaining from the FIFO size. This will be different
    // depending on whether the IOM is configured for half-duplex or
    // full-duplex.
    //
    return (AM_HAL_IOM_MAX_FIFO_SIZE - AM_BFRn(IOMSTR, ui32Module, FIFOPTR, FIFOSIZ)) & (~0x3);
}

//*****************************************************************************
//
//! @brief Check to see how much data is in the IOM fifo.
//!
//! @param ui32Module - Module number of the IOM whose fifo should be checked.
//!
//! Returns the number of bytes of data that are currently in the IOM fifo.
//!
//! @return Number of bytes in the fifo.
//
//*****************************************************************************
uint8_t
am_hal_iom_fifo_full_slots(uint32_t ui32Module)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }

    return AM_BFRn(IOMSTR, ui32Module, FIFOPTR, FIFOSIZ);
}

//*****************************************************************************
//
//! @brief Wait for the current IOM command to complete.
//!
//! @param ui32Module - The module number of the IOM to use.
//!
//! This function polls until the IOM bus becomes idle.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_poll_complete(uint32_t ui32Module)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    //
    // Poll on the IDLE bit in the status register.
    //
    while ( g_bIomBusy[ui32Module] );
}

//*****************************************************************************
//
//! @brief Returns the contents of the IOM status register.
//!
//! @param ui32Module IOM instance to check the status of.
//!
//! This function is just a wrapper around the IOM status register.
//!
//! @return 32-bit contents of IOM status register.
//
//*****************************************************************************
uint32_t
am_hal_iom_status_get(uint32_t ui32Module)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }

    return AM_REGn(IOMSTR, ui32Module, STATUS);
}

//*****************************************************************************
//
//! @brief Returns current error state of the IOM.
//!
//! @param ui32Module IOM instance to check the status of.
//!
//! This function returns status indicating whether the IOM has incurred any
//! errors or not.
//!
//! @return 0 if all is well.
//!         Otherwise error status as a bitmask of:
//!             AM_HAL_IOM_ERR_INVALID_MODULE
//!             AM_HAL_IOM_INT_ARB      Another master initiated an operation
//!                                     simultaenously and the IOM lost.  Or
//!                                     the IOM started an operation but found
//!                                     SDA already low.
//!             AM_HAL_IOM_INT_START    A START from another master detected.
//!                                     SW must wait for STOP before continuing.
//!             AM_HAL_IOM_INT_ICMD     Attempt to issue a CMD while another
//!                                     CMD was already in progress, or issue a
//!                                     non-zero-len write CMD with empty FIFO.
//!             AM_HAL_IOM_INT_IACC     Attempt to read the FIFO on a write. Or
//!                                     an attempt to write the FIFO on a read.
//!             AM_HAL_IOM_INT_NAK      Expected ACK from slave not received.
//!             AM_HAL_IOM_INT_FOVFL    Attempt to write the FIFO while full
//!                                     (FIFOSIZ > 124).
//!             AM_HAL_IOM_INT_FUNDFL   Attempt to read FIFO when empty (that is
//!                                     FIFOSIZ < 4).
//!         Note - see the datasheet text for full explanations of the INT errs.
//
//*****************************************************************************
uint32_t
am_hal_iom_error_status_get(uint32_t ui32Module)
{
    uint32_t ui32intstat = 0;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        //
        // AM_HAL_IOM_ERR_INVALID_MODULE is defined as an unused interrupt bit.
        //
        return AM_HAL_IOM_ERR_INVALID_MODULE;
    }

    if ( AM_REGn(IOMSTR, ui32Module, STATUS) & AM_REG_IOMSTR_STATUS_ERR_ERROR )
    {
        //
        // The IOM is currently indicating an error condition.
        // Let's figure out what is going on.
        //
        ui32intstat = AM_REGn(IOMSTR, ui32Module, INTSTAT);

        //
        // Filter out non-error bits.
        //
        ui32intstat &=  AM_REG_IOMSTR_INTSTAT_ARB_M     |
                        AM_REG_IOMSTR_INTSTAT_START_M   |
                        AM_REG_IOMSTR_INTSTAT_ICMD_M    |
                        AM_REG_IOMSTR_INTSTAT_IACC_M    |
                        AM_REG_IOMSTR_INTSTAT_NAK_M     |
                        AM_REG_IOMSTR_INTSTAT_FOVFL_M   |
                        AM_REG_IOMSTR_INTSTAT_FUNDFL_M;
    }

    return ui32intstat;
}

//*****************************************************************************
//
//! @brief Service interrupts from the IOM.
//!
//! @param ui32Status is the IOM interrupt status as returned from
//! am_hal_iom_int_status_get()
//!
//! This function performs the necessary operations to facilitate non-blocking
//! IOM writes and reads.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_iom_int_service(uint32_t ui32Module, uint32_t ui32Status)
{
    am_hal_iom_nb_buffer *psBuffer;
    uint32_t ui32NumBytes;
    uint32_t ui32SpaceInFifo;
    uint32_t thresh;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    //
    // Find the buffer information for the chosen IOM module.
    //
    psBuffer = &g_psIOMBuffers[ui32Module];

    //
    // Figure out what type of interrupt this was.
    //
    if ( ui32Status & AM_HAL_IOM_INT_CMDCMP )
    {
        //
        // Need to mark IOM Free
        //
        g_bIomBusy[ui32Module] = false;

        //
        // If we're not in the middle of a non-blocking call right now, there's
        // nothing for this routine to do.
        //
        if ( psBuffer->ui32State == BUFFER_IDLE )
        {
            return;
        }

        //
        // If a command just completed, we need to transfer all available data.
        //
        if ( psBuffer->ui32State == BUFFER_RECEIVING )
        {
            //
            // If we were receiving, we need to copy any remaining data out of
            // the IOM FIFO before calling the callback.
            //
            ui32NumBytes = am_hal_iom_fifo_full_slots(ui32Module);
            am_hal_iom_fifo_read(ui32Module, psBuffer->pui32Data, ui32NumBytes);
        }

        //
        // A command complete event also means that we've already transferred
        // all of the data we need, so we can mark the data buffer as IDLE.
        //
        psBuffer->ui32State = BUFFER_IDLE;

        //
        // If we have a callback, call it now.
        //
        if ( psBuffer->pfnCallback )
        {
            psBuffer->pfnCallback();
        }
    }
    else if ( ui32Status & AM_HAL_IOM_INT_THR )
    {
        //
        // If we're not in the middle of a non-blocking call right now, there's
        // nothing for this routine to do.
        //
        if ( psBuffer->ui32State == BUFFER_IDLE )
        {
            return;
        }
        //
        // If we received a threshold event in the middle of a command, we need
        // to transfer data.
        //
        if ( psBuffer->ui32State == BUFFER_SENDING )
        {
            thresh = AM_BFRn(IOMSTR, ui32Module, FIFOTHR, FIFOWTHR);
            do
            {
                ui32SpaceInFifo = am_hal_iom_fifo_empty_slots(ui32Module);

                //
                // Figure out how much data we can send.
                //
                if ( psBuffer->ui32BytesLeft <= ui32SpaceInFifo )
                {
                    //
                    // If the whole transfer will fit in the fifo, send it all.
                    //
                    ui32NumBytes = psBuffer->ui32BytesLeft;
                }
                else
                {
                    //
                    // If the transfer won't fit in the fifo completely, send as
                    // much as we can (rounded down to a multiple of four bytes).
                    //
                    ui32NumBytes = ui32SpaceInFifo;
                }

                //
                // Perform the transfer.
                //
                am_hal_iom_fifo_write(ui32Module, psBuffer->pui32Data, ui32NumBytes);

                // Clear any spurious THR interrupt that might have got raised
                // while we were adding data to FIFO
                AM_BFWn(IOMSTR, ui32Module, INTCLR, THR, 1);
                //
                // Update the pointer and the byte counter.
                //
                psBuffer->ui32BytesLeft -= ui32NumBytes;
                psBuffer->pui32Data += (ui32NumBytes / 4);

                if ( 0 == psBuffer->ui32BytesLeft )
                {
                    //
                    // Done with this transaction
                    //
                    break;
                }
            } while ( am_hal_iom_fifo_full_slots(ui32Module) <= thresh );
        }
        else
        {
            thresh = AM_BFRn(IOMSTR, ui32Module, FIFOTHR, FIFORTHR);
            while ( (ui32NumBytes = am_hal_iom_fifo_full_slots(ui32Module)) >= thresh )
            {
                //
                // If we get here, we're in the middle of a read. Transfer as much
                // data as possible out of the FIFO and into our buffer.
                //
                if ( ui32NumBytes == psBuffer->ui32BytesLeft )
                {
                    //
                    // If the fifo contains our entire message, just copy the whole
                    // thing out.
                    //
                    am_hal_iom_fifo_read(ui32Module, psBuffer->pui32Data,
                                         psBuffer->ui32BytesLeft);

                    break;
                }
                else if ( ui32NumBytes >= 4 )
                {
                    //
                    // If the fifo has at least one 32-bit word in it, copy out the
                    // biggest block we can.
                    //
                    ui32NumBytes = (ui32NumBytes & (~0x3));

                    am_hal_iom_fifo_read(ui32Module, psBuffer->pui32Data, ui32NumBytes);

                    //
                    // Update the pointer and the byte counter.
                    //
                    psBuffer->ui32BytesLeft -= ui32NumBytes;
                    psBuffer->pui32Data += (ui32NumBytes / 4);

                    // Clear any spurious THR interrupt that might have got raised
                    // while we were reading the data from FIFO
                    AM_BFWn(IOMSTR, ui32Module, INTCLR, THR, 1);
                }
            }
        }
    }
}

//*****************************************************************************
//
//! @brief Initialize the IOM queue system.
//!
//! @param ui32Module - IOM module to be initialized for queue transfers.
//! @param psQueueMemory - Memory to be used for queueing IOM transfers.
//! @param ui32QueueMemSize - Size of the queue memory.
//!
//! This function prepares the selected IOM interface for use with the IOM
//! queue system. The IOM queue system allows the caller to start multiple IOM
//! transfers in a non-blocking way. In order to do this, the HAL requires some
//! amount of memory dedicated to keeping track of IOM transactions before they
//! can be sent to the hardware registers. This function tells the HAL what
//! memory it should use for this purpose. For more information on the IOM
//! queue interface, please see the documentation for
//! am_hal_iom_queue_spi_write().
//!
//! @note This function only needs to be called once (per module), but it must
//! be called before any other am_hal_iom_queue function.
//!
//! @note Each IOM module will need its own working space. If you intend to use
//! the queueing mechanism with more than one IOM module, you will need to
//! provide separate queue memory for each module.
//!
//! Example usage:
//!
//! @code
//!
//! //
//! // Declare an array to be used for IOM queue transactions. This array will
//! // be big enough to handle 32 IOM transactions.
//! //
//! am_hal_iom_queue_entry_t g_psQueueMemory[32];
//!
//! //
//! // Attach the IOM0 queue system to the memory we just allocated.
//! //
//! am_hal_iom_queue_init(0, g_psQueueMemory, sizeof(g_psQueueMemory));
//!
//! @endcode
//
//*****************************************************************************
void
am_hal_iom_queue_init(uint32_t ui32Module, am_hal_iom_queue_entry_t *psQueueMemory,
                      uint32_t ui32QueueMemSize)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    am_hal_queue_init(&g_psIOMQueue[ui32Module], psQueueMemory,
                      sizeof(am_hal_iom_queue_entry_t), ui32QueueMemSize);
}

//*****************************************************************************
//
//! @brief Check to see how many transactions are in the queue.
//!
//! @param ui32Module Module number for the queue to check
//!
//! This function will check to see how many transactions are in the IOM queue
//! for the selected IOM module.
//!
//! @return Number of transactions in the queue.
//
//*****************************************************************************
uint32_t
am_hal_iom_queue_length_get(uint32_t ui32Module)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }

    return am_hal_queue_data_left(&g_psIOMQueue[ui32Module]);
}

//*****************************************************************************
//
//! @brief Executes the next operation in the IOM queue.
//!
//! @param ui32ModuleNum - Module number for the IOM to use.
//!
//! This function checks the IOM queue to see if there are any remaining
//! transactions. If so, it will start the next available transaction in a
//! non-blocking way.
//!
//! @note This function is called automatically by am_hal_iom_queue_service().
//! You should not call this function standalone in a normal application.
//
//*****************************************************************************
void
am_hal_iom_queue_start_next_msg(uint32_t ui32Module)
{
  am_hal_iom_queue_entry_t sIOMTransaction = {0};

    uint32_t ui32ChipSelect;
    uint32_t *pui32Data;
    uint32_t ui32NumBytes;
    uint32_t ui32Options;
    am_hal_iom_callback_t pfnCallback;

    uint32_t ui32Critical;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    //
    // Start a critical section.
    //
    ui32Critical = am_hal_interrupt_master_disable();

    //
    // Try to get the next IOM operation from the queue.
    //
    if ( am_hal_queue_item_get(&g_psIOMQueue[ui32Module], &sIOMTransaction, 1) )
    {
        //
        // Read the operation parameters
        //
        ui32ChipSelect = sIOMTransaction.ui32ChipSelect;
        pui32Data = sIOMTransaction.pui32Data;
        ui32NumBytes = sIOMTransaction.ui32NumBytes;
        ui32Options = sIOMTransaction.ui32Options;
        pfnCallback = sIOMTransaction.pfnCallback;

        //
        // Figure out if this was a SPI or I2C write or read, and call the
        // appropriate non-blocking function.
        //
        switch ( sIOMTransaction.ui32Operation )
        {
            case AM_HAL_IOM_QUEUE_SPI_WRITE:
                am_hal_iom_spi_write_nb(ui32Module, ui32ChipSelect, pui32Data,
                                        ui32NumBytes, ui32Options, pfnCallback);
                break;

            case AM_HAL_IOM_QUEUE_SPI_READ:
                am_hal_iom_spi_read_nb(ui32Module, ui32ChipSelect, pui32Data,
                                       ui32NumBytes, ui32Options, pfnCallback);
                break;

            case AM_HAL_IOM_QUEUE_I2C_WRITE:
                am_hal_iom_i2c_write_nb(ui32Module, ui32ChipSelect, pui32Data,
                                        ui32NumBytes, ui32Options, pfnCallback);
                break;

            case AM_HAL_IOM_QUEUE_I2C_READ:
                am_hal_iom_i2c_read_nb(ui32Module, ui32ChipSelect, pui32Data,
                                       ui32NumBytes, ui32Options, pfnCallback);
                break;
        }
    }

    //
    // Exit the critical section.
    //
    am_hal_interrupt_master_set(ui32Critical);
}

//*****************************************************************************
//
//! @brief Send a SPI frame using the IOM queue.
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32ChipSelect - Chip-select number for this transaction.
//! @param pui32Data - Pointer to the bytes that will be sent.
//! @param ui32NumBytes - Number of bytes to send.
//! @param ui32Options - Additional SPI transfer options.
//!
//! This function performs SPI writes to a selected SPI device.
//!
//! This function call is a queued implementation. It will write as much
//! data to the FIFO as possible immediately, store a pointer to the remaining
//! data, start the transfer on the bus, and then immediately return. If the
//! FIFO is already in use, this function will save its arguments to the IOM
//! queue and execute the transaction when the FIFO becomes available.
//!
//! The caller will need to make sure that \e am_hal_iom_queue_service() is
//! called for IOM FIFO interrupt events and "command complete" interrupt
//! events. The \e am_hal_iom_queue_service() function will refill the FIFO as
//! necessary and call the \e pfnCallback function when the transaction is
//! finished.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui8Data array.
//
//*****************************************************************************
void
am_hal_iom_queue_spi_write(uint32_t ui32Module, uint32_t ui32ChipSelect,
                           uint32_t *pui32Data, uint32_t ui32NumBytes,
                           uint32_t ui32Options, am_hal_iom_callback_t pfnCallback)
{
    uint32_t ui32Critical;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Start a critical section.
    //
    ui32Critical = am_hal_interrupt_master_disable();

    //
    // Check to see if we need to use the queue. If the IOM is idle, and
    // there's nothing in the queue already, we can go ahead and start the
    // transaction in the physical IOM. Need to check for the g_bIomBusy to
    // avoid a race condition where IDLE is set - but the command complete
    // for previous transaction has not been processed yet
    //
    if ( (g_bIomBusy[ui32Module] == false) &&
        am_hal_queue_empty(&g_psIOMQueue[ui32Module]) )
    {
        //
        // Send the packet.
        //
        am_hal_iom_spi_write_nb(ui32Module, ui32ChipSelect, pui32Data,
                                ui32NumBytes, ui32Options, pfnCallback);
    }
    else
    {
        //
        // Otherwise, we'll build a transaction structure and add it to the queue.
        //
        am_hal_iom_queue_entry_t sIOMTransaction;

        sIOMTransaction.ui32Operation = AM_HAL_IOM_QUEUE_SPI_WRITE;
        sIOMTransaction.ui32Module = ui32Module;
        sIOMTransaction.ui32ChipSelect = ui32ChipSelect;
        sIOMTransaction.pui32Data = pui32Data;
        sIOMTransaction.ui32NumBytes = ui32NumBytes;
        sIOMTransaction.ui32Options = ui32Options;
        sIOMTransaction.pfnCallback = pfnCallback;

        //
        // Make sure the item actually makes it into the queue
        //
        if ( am_hal_queue_item_add(&g_psIOMQueue[ui32Module], &sIOMTransaction, 1) == false )
        {
            //
            // Didn't have enough memory.
            //
            am_hal_debug_assert_msg(0,
                                    "The IOM queue is full. Allocate more"
                                    "memory to the IOM queue, or allow it more"
                                    "time to empty between transactions.");
        }
    }

    //
    // Exit the critical section.
    //
    am_hal_interrupt_master_set(ui32Critical);
}

//*****************************************************************************
//
//! @brief Read a SPI frame using the IOM queue.
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32ChipSelect - Chip select number for this transaction.
//! @param pui32Data - Pointer to the array where received bytes should go.
//! @param ui32NumBytes - Number of bytes to read.
//! @param ui32Options - Additional SPI transfer options.
//!
//! This function performs SPI reads to a selected SPI device.
//!
//! This function call is a queued implementation. It will write as much
//! data to the FIFO as possible immediately, store a pointer to the remaining
//! data, start the transfer on the bus, and then immediately return. If the
//! FIFO is already in use, this function will save its arguments to the IOM
//! queue and execute the transaction when the FIFO becomes available.
//!
//! The caller will need to make sure that \e am_hal_iom_queue_service() is
//! called for IOM FIFO interrupt events and "command complete" interrupt
//! events. The \e am_hal_iom_queue_service() function will empty the FIFO as
//! necessary and call the \e pfnCallback function when the transaction is
//! finished.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui8Data array.
//
//*****************************************************************************
void
am_hal_iom_queue_spi_read(uint32_t ui32Module, uint32_t ui32ChipSelect,
                          uint32_t *pui32Data, uint32_t ui32NumBytes,
                          uint32_t ui32Options, am_hal_iom_callback_t pfnCallback)
{
    uint32_t ui32Critical;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    // Start a critical section.
    //
    ui32Critical = am_hal_interrupt_master_disable();

    //
    // Check to see if we need to use the queue. If the IOM is idle, and
    // there's nothing in the queue already, we can go ahead and start the
    // transaction in the physical IOM. Need to check for the g_bIomBusy to
    // avoid a race condition where IDLE is set - but the command complete
    // for previous transaction has not been processed yet
    //
    if ( (g_bIomBusy[ui32Module] == false) &&
        am_hal_queue_empty(&g_psIOMQueue[ui32Module]) )
    {
        //
        // Send the packet.
        //
        am_hal_iom_spi_read_nb(ui32Module, ui32ChipSelect, pui32Data,
                               ui32NumBytes, ui32Options, pfnCallback);
    }
    else
    {
        //
        // Otherwise, we'll build a transaction structure and add it to the queue.
        //
        am_hal_iom_queue_entry_t sIOMTransaction;

        sIOMTransaction.ui32Operation = AM_HAL_IOM_QUEUE_SPI_READ;
        sIOMTransaction.ui32Module = ui32Module;
        sIOMTransaction.ui32ChipSelect = ui32ChipSelect;
        sIOMTransaction.pui32Data = pui32Data;
        sIOMTransaction.ui32NumBytes = ui32NumBytes;
        sIOMTransaction.ui32Options = ui32Options;
        sIOMTransaction.pfnCallback = pfnCallback;

        //
        // Make sure the item actually makes it into the queue
        //
        if ( am_hal_queue_item_add(&g_psIOMQueue[ui32Module], &sIOMTransaction, 1) == false )
        {
            //
            // Didn't have enough memory.
            //
            am_hal_debug_assert_msg(0,
                                    "The IOM queue is full. Allocate more"
                                    "memory to the IOM queue, or allow it more"
                                    "time to empty between transactions.");
        }
    }

    //
    // Exit the critical section.
    //
    am_hal_interrupt_master_set(ui32Critical);
}

//*****************************************************************************
//
//! @brief Send an I2C frame using the IOM queue.
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32BusAddress - I2C address of the target device.
//! @param pui32Data - Pointer to the bytes that will be sent.
//! @param ui32NumBytes - Number of bytes to send.
//! @param ui32Options - Additional I2C transfer options.
//!
//! This function performs I2C writes to a selected I2C device.
//!
//! This function call is a queued implementation. It will write as much
//! data to the FIFO as possible immediately, store a pointer to the remaining
//! data, start the transfer on the bus, and then immediately return. If the
//! FIFO is already in use, this function will save its arguments to the IOM
//! queue and execute the transaction when the FIFO becomes available.
//!
//! The caller will need to make sure that \e am_hal_iom_queue_service() is
//! called for IOM FIFO interrupt events and "command complete" interrupt
//! events. The \e am_hal_iom_queue_service() function will refill the FIFO as
//! necessary and call the \e pfnCallback function when the transaction is
//! finished.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui8Data array.
//
//*****************************************************************************
void
am_hal_iom_queue_i2c_write(uint32_t ui32Module, uint32_t ui32BusAddress,
                           uint32_t *pui32Data, uint32_t ui32NumBytes,
                           uint32_t ui32Options, am_hal_iom_callback_t pfnCallback)
{
    uint32_t ui32Critical;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Start a critical section.
    //
    ui32Critical = am_hal_interrupt_master_disable();

    //
    // Check to see if we need to use the queue. If the IOM is idle, and
    // there's nothing in the queue already, we can go ahead and start the
    // transaction in the physical IOM. Need to check for the g_bIomBusy to
    // avoid a race condition where IDLE is set - but the command complete
    // for previous transaction has not been processed yet
    //
    if ( (g_bIomBusy[ui32Module] == false) &&
        am_hal_queue_empty(&g_psIOMQueue[ui32Module]) )
    {
        //
        // Send the packet.
        //
        am_hal_iom_i2c_write_nb(ui32Module, ui32BusAddress, pui32Data,
                                ui32NumBytes, ui32Options, pfnCallback);
    }
    else
    {
        //
        // Otherwise, we'll build a transaction structure and add it to the queue.
        //
        am_hal_iom_queue_entry_t sIOMTransaction;

        sIOMTransaction.ui32Operation = AM_HAL_IOM_QUEUE_I2C_WRITE;
        sIOMTransaction.ui32Module = ui32Module;
        sIOMTransaction.ui32ChipSelect = ui32BusAddress;
        sIOMTransaction.pui32Data = pui32Data;
        sIOMTransaction.ui32NumBytes = ui32NumBytes;
        sIOMTransaction.ui32Options = ui32Options;
        sIOMTransaction.pfnCallback = pfnCallback;

        //
        // Make sure the item actually makes it into the queue
        //
        if ( am_hal_queue_item_add(&g_psIOMQueue[ui32Module], &sIOMTransaction, 1) == false )
        {
            //
            // Didn't have enough memory.
            //
            am_hal_debug_assert_msg(0,
                                    "The IOM queue is full. Allocate more"
                                    "memory to the IOM queue, or allow it more"
                                    "time to empty between transactions.");
        }
    }

    //
    // Exit the critical section.
    //
    am_hal_interrupt_master_set(ui32Critical);
}

//*****************************************************************************
//
//! @brief Read a I2C frame using the IOM queue.
//!
//! @param ui32Module - Module number for the IOM
//! @param ui32BusAddress - I2C address of the target device.
//! @param pui32Data - Pointer to the array where received bytes should go.
//! @param ui32NumBytes - Number of bytes to read.
//! @param ui32Options - Additional I2C transfer options.
//!
//! This function performs I2C reads to a selected I2C device.
//!
//! This function call is a queued implementation. It will write as much
//! data to the FIFO as possible immediately, store a pointer to the remaining
//! data, start the transfer on the bus, and then immediately return. If the
//! FIFO is already in use, this function will save its arguments to the IOM
//! queue and execute the transaction when the FIFO becomes available.
//!
//! The caller will need to make sure that \e am_hal_iom_queue_service() is
//! called for IOM FIFO interrupt events and "command complete" interrupt
//! events. The \e am_hal_iom_queue_service() function will empty the FIFO as
//! necessary and call the \e pfnCallback function when the transaction is
//! finished.
//!
//! @note The actual SPI and I2C interfaces operate in BYTES, not 32-bit words.
//! This means that you will need to byte-pack the \e pui32Data array with the
//! data you intend to send over the interface. One easy way to do this is to
//! declare the array as a 32-bit integer array, but use an 8-bit pointer to
//! put your actual data into the array. If there are not enough bytes in your
//! desired message to completely fill the last 32-bit word, you may pad that
//! last word with bytes of any value. The IOM hardware will only read the
//! first \e ui32NumBytes in the \e pui8Data array.
//
//*****************************************************************************
void
am_hal_iom_queue_i2c_read(uint32_t ui32Module, uint32_t ui32BusAddress,
                          uint32_t *pui32Data, uint32_t ui32NumBytes,
                          uint32_t ui32Options, am_hal_iom_callback_t pfnCallback)
{
    uint32_t ui32Critical;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }
    am_hal_debug_assert_msg(ui32NumBytes > 0,
                            "Trying to do a 0 byte transaction");

    //
    // Start a critical section.
    //
    ui32Critical = am_hal_interrupt_master_disable();

    //
    // Check to see if we need to use the queue. If the IOM is idle, and
    // there's nothing in the queue already, we can go ahead and start the
    // transaction in the physical IOM. Need to check for the g_bIomBusy to
    // avoid a race condition where IDLE is set - but the command complete
    // for previous transaction has not been processed yet
    //
    if ( (g_bIomBusy[ui32Module] == false) &&
        am_hal_queue_empty(&g_psIOMQueue[ui32Module]) )
    {
        //
        // Send the packet.
        //
        am_hal_iom_i2c_read_nb(ui32Module, ui32BusAddress, pui32Data,
                               ui32NumBytes, ui32Options, pfnCallback);
    }
    else
    {
        //
        // Otherwise, we'll build a transaction structure and add it to the queue.
        //
        am_hal_iom_queue_entry_t sIOMTransaction;

        sIOMTransaction.ui32Operation = AM_HAL_IOM_QUEUE_I2C_READ;
        sIOMTransaction.ui32Module = ui32Module;
        sIOMTransaction.ui32ChipSelect = ui32BusAddress;
        sIOMTransaction.pui32Data = pui32Data;
        sIOMTransaction.ui32NumBytes = ui32NumBytes;
        sIOMTransaction.ui32Options = ui32Options;
        sIOMTransaction.pfnCallback = pfnCallback;

        //
        // Make sure the item actually makes it into the queue
        //
        if ( am_hal_queue_item_add(&g_psIOMQueue[ui32Module], &sIOMTransaction, 1) == false )
        {
            //
            // Didn't have enough memory.
            //
            am_hal_debug_assert_msg(0, "The IOM queue is full. Allocate more"
                                       "memory to the IOM queue, or allow it more"
                                       "time to empty between transactions.");
        }
    }

    //
    // Exit the critical section.
    //
    am_hal_interrupt_master_set(ui32Critical);
}

//*****************************************************************************
//
//! @brief "Block" until the queue of IOM transactions is over.
//!
//! @param ui32Module - Module number for the IOM.
//!
//! This function will sleep the core block until the queue for the selected
//! IOM is empty. This is mainly useful for non-RTOS applications where the
//! caller needs to know that a certain IOM transaction is complete before
//! continuing with the main program flow.
//!
//! @note This function will put the core to sleep while it waits for the
//! queued IOM transactions to complete. This will save power, in most
//! situations, but it may not be the best option in all cases. \e Do \e not
//! call this function from interrupt context (the core may not wake up again).
//! \e Be \e careful using this function from an RTOS task (many RTOS
//! implementations use hardware interrupts to switch contexts, and most RTOS
//! implementations expect to control sleep behavior).
//
//*****************************************************************************
void
am_hal_iom_sleeping_queue_flush(uint32_t ui32Module)
{
    bool bWaiting = true;
    uint32_t ui32Critical;

    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    //
    // Loop forever waiting for the IOM to be idle and the queue to be empty.
    //
    while ( bWaiting )
    {
        //
        // Start a critical section.
        //
        ui32Critical = am_hal_interrupt_master_disable();

        //
        // Check the queue and the IOM itself.
        //
        if ( (g_bIomBusy[ui32Module] == false) &&
            am_hal_queue_empty(&g_psIOMQueue[ui32Module]) )
        {
            //
            // If the queue is empty and the IOM is idle, we can go ahead and
            // return.
            //
            bWaiting = false;
        }
        else
        {
            //
            // Otherwise, we should sleep until the interface is actually free.
            //
            am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_NORMAL);
        }

        //
        // End the critical section.
        //
        am_hal_interrupt_master_set(ui32Critical);
    }
}

//*****************************************************************************
//
//! @brief Service IOM transaction queue.
//!
//! @param ui32Module - Module number for the IOM to be used.
//! @param ui32Status - Interrupt status bits for the IOM module being used.
//!
//! This function handles the operation of FIFOs and the IOM queue during
//! queued IOM transactions. If you are using \e am_hal_iom_queue_spi_write()
//! or similar functions, you will need to call this function in your interrupt
//! handler.
//!
//! @note This interrupt service routine relies on the user to enable the IOM
//! interrupts for FIFO threshold and CMD complete.
//!
//! Example:
//!
//! @code
//! void
//! am_iomaster0_isr(void)
//! {
//!     uint32_t ui32Status;
//!
//!     //
//!     // Check to see which interrupt caused us to enter the ISR.
//!     //
//!     ui32Status = am_hal_iom_int_status(0, true);
//!
//!     //
//!     // Fill or empty the FIFO, and either continue the current operation or
//!     // start the next one in the queue. If there was a callback, it will be
//!     // called here.
//!     //
//!     am_hal_iom_queue_service(0, ui32Status);
//!
//!     //
//!     // Clear the interrupts before leaving the ISR.
//!     //
//!     am_hal_iom_int_clear(ui32Status);
//! }
//! @endcode
//!
//! @return
//
//*****************************************************************************
void
am_hal_iom_queue_service(uint32_t ui32Module, uint32_t ui32Status)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    //
    // Service the FIFOs in case this was a threshold interrupt.
    //
    am_hal_iom_int_service(ui32Module, ui32Status);

    //
    // If the last interrupt was a "command complete", then the IOM should be
    // idle already or very soon. Make absolutely sure that the IOM is not in
    // use, and then start the next transaction in the queue.
    //
    if ( ui32Status & AM_HAL_IOM_INT_CMDCMP )
    {
        if ( g_psIOMQueue[ui32Module].pui8Data != NULL )
        {
            am_hal_iom_queue_start_next_msg(ui32Module);
        }
    }
}

//*****************************************************************************
//
//! @brief Enable selected IOM Interrupts.
//!
//! @param ui32Module - Module number.
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_iom.h
//!
//! Use this function to enable the IOM interrupts.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_iom_int_enable(uint32_t ui32Module, uint32_t ui32Interrupt)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    AM_REGn(IOMSTR, ui32Module, INTEN) |= ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Return the enabled IOM Interrupts.
//!
//! @param ui32Module - Module number.
//!
//! Use this function to return all enabled IOM interrupts.
//!
//! @return all enabled IOM interrupts.
//
//*****************************************************************************
uint32_t
am_hal_iom_int_enable_get(uint32_t ui32Module)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }

    return AM_REGn(IOMSTR, ui32Module, INTEN);
}

//*****************************************************************************
//
//! @brief Disable selected IOM Interrupts.
//!
//! @param ui32Module - Module number.
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_iom.h
//!
//! Use this function to disable the IOM interrupts.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_iom_int_disable(uint32_t ui32Module, uint32_t ui32Interrupt)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    AM_REGn(IOMSTR, ui32Module, INTEN) &= ~ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Clear selected IOM Interrupts.
//!
//! @param ui32Module - Module number.
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_iom.h
//!
//! Use this function to clear the IOM interrupts.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_iom_int_clear(uint32_t ui32Module, uint32_t ui32Interrupt)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    AM_REGn(IOMSTR, ui32Module, INTCLR) = ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Set selected IOM Interrupts.
//!
//! @param ui32Module - Module number.
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_iom.h
//!
//! Use this function to set the IOM interrupts.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_iom_int_set(uint32_t ui32Module, uint32_t ui32Interrupt)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return;
    }

    AM_REGn(IOMSTR, ui32Module, INTSET) = ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Return the IOM Interrupt status.
//!
//! @param ui32Module - Module number.
//! @param bEnabledOnly - return only the enabled interrupts.
//!
//! Use this function to get the IOM interrupt status.
//!
//! @return interrupt status
//
//*****************************************************************************
uint32_t
am_hal_iom_int_status_get(uint32_t ui32Module, bool bEnabledOnly)
{
    //
    // Validate parameters
    //
    if ( ui32Module >= AM_REG_IOMSTR_NUM_MODULES )
    {
        return 0;
    }

    if ( bEnabledOnly )
    {
        uint32_t u32RetVal = AM_REGn(IOMSTR, ui32Module, INTSTAT);
        return u32RetVal & AM_REGn(IOMSTR, ui32Module, INTEN);
    }
    else
    {
        return AM_REGn(IOMSTR, ui32Module, INTSTAT);
    }
}


//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
