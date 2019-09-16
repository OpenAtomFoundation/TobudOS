/** ****************************************************************************
 * @copyright                  Nationz Co.Ltd
 *               Copyright (c) 2013гн2018 All Rights Reserved
 *******************************************************************************
 * @file     am_hal_uart.c
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for interfacing with the UART.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup uart UART
 * @ingroup hal
 * @{
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_Z32HLP.h"

//*****************************************************************************
//
// Function-like macros.
//
//*****************************************************************************
#define am_hal_uart_ring_buffer_empty(psBuffer)                             \
    ((psBuffer)->ui32Length == 0)

#define am_hal_uart_ring_buffer_full(psBuffer)                              \
    ((psBuffer)->ui32Length == (psBuffer)->ui32Capacity)

#define am_hal_uart_ring_buffer_data_left(psBuffer)                         \
    ((psBuffer)->ui32Length)

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
am_hal_uart_ring_buffer_t g_sTxBuffer;
am_hal_uart_ring_buffer_t g_sRxBuffer;

//*****************************************************************************
//
//! @brief Return the space remaining in the buffer.
//!
//! This function returns the remaining space in the buffer.
//!
//! @note This function was originally implemented as a macro, thus it is
//!  declared as static.  It was converted to a function in order to resolve
//!  compiler warnings concerning order of volatile accesses.
//!
//! @return Space remaining in the buffer.
//
//*****************************************************************************
static uint32_t
am_hal_uart_ring_buffer_space_left(am_hal_uart_ring_buffer_t *psBuffer)
{
    uint32_t u32RetVal = psBuffer->ui32Capacity;
    u32RetVal -= psBuffer->ui32Length;
    return u32RetVal;
}

//*****************************************************************************
//
// Set Baud Rate based on the UART clock frequency.
//
//*****************************************************************************

#define BAUDCLK     (16)

static void
config_baudrate(uint32_t ui32Module, uint32_t ui32Baudrate, uint32_t ui32UartClkFreq)
{
    uint64_t ui64FractionDivisorLong;
    uint64_t ui64IntermediateLong;
    uint32_t ui32IntegerDivisor;
    uint32_t ui32FractionDivisor;
    uint32_t ui32BaudClk;

    //
    // Calculate register values.
    //
    ui32BaudClk = BAUDCLK * ui32Baudrate;
    ui32IntegerDivisor = (uint32_t)(ui32UartClkFreq / ui32BaudClk);
    ui64IntermediateLong = (ui32UartClkFreq * 64) / ui32BaudClk;
    ui64FractionDivisorLong = ui64IntermediateLong - (ui32IntegerDivisor * 64);
    ui32FractionDivisor = (uint32_t)ui64FractionDivisorLong;

    //
    // Integer divisor MUST be greater than or equal to 1.
    //
    if ( ui32IntegerDivisor == 0 )
    {
        //
        // Spin in a while because the selected baudrate is not possible.
        //
        while(1);
    }

    //
    // Write the UART regs.
    //
    AM_REGn(UART, 0, IBRD) = ui32IntegerDivisor;
    AM_REGn(UART, 0, IBRD) = ui32IntegerDivisor;
    AM_REGn(UART, 0, FBRD) = ui32FractionDivisor;
}

//*****************************************************************************
//
//! @brief Set up the UART.
//!
//! @param psConfig pointer to a structure that holds the settings for the UART.
//! @param ui32UartclkFreq is clock frequency that the UART is running at.
//!
//! This function should be used to perform the initial set-up of the UART.
//!
//! @return none.
//
//*****************************************************************************
void
am_hal_uart_config(uint32_t ui32Module, am_hal_uart_config_t *psConfig)

{
    uint32_t ui32ConfigVal = 0;

    //
    // Configure the Baudrate.
    //
    config_baudrate(0, psConfig->ui32BaudRate, am_hal_clkgen_sysclk_get());

    //
    // OR in the Data bits.
    //
    ui32ConfigVal |= psConfig->ui32DataBits;

    //
    // OR in the Two Stop bit if used.
    //
    ui32ConfigVal |= psConfig->bTwoStopBits ? AM_REG_UART_LCRH_STP2_M : 0;

    //
    // OR in the Parity.
    //
    ui32ConfigVal |= psConfig->ui32Parity;

    //
    // Write config to Line control register.
    //
    AM_REGn(UART, 0, LCRH) |= ui32ConfigVal;

    //
    // Write the flow control settings to the control register.
    //
    AM_REGn(UART, 0, CR) |= psConfig->ui32FlowCtrl;

    //
    // Set the clock select field for 24MHz from the HFRC
    //
    AM_REGn(UART, 0, CR) |= AM_REG_UART_CR_CLKSEL_24MHZ;//AM_REG_UART_CR_CLKSEL_6MHZ;//
}

//*****************************************************************************
//
//! @brief Gets the status.
//!
//! This function returns the current status.
//!
//! @return current status.
//
//*****************************************************************************
uint32_t
am_hal_uart_status_get(uint32_t ui32Module)
{
    //
    // Read and return the Status.
    //
    return AM_REGn(UART, 0, RSR);
}

//*****************************************************************************
//
//! @brief Gets the interrupt status.
//!
//! @param bEnabledOnly - If true returns the enabled interrupt status.
//!
//! This function returns the masked or raw interrupt status.
//!
//! @return Bitwise representation of the current interrupt status.
//!
//! The return value will be the logical OR of one or more of the following
//! values:
//!
//!     AM_HAL_UART_INT_OVER_RUN
//!     AM_HAL_UART_INT_BREAK_ERR
//!     AM_HAL_UART_INT_PARITY_ERR
//!     AM_HAL_UART_INT_FRAME_ERR
//!     AM_HAL_UART_INT_RX_TMOUT
//!     AM_HAL_UART_INT_TX
//!     AM_REG_UART_IER_TXIM_M
//!     AM_HAL_UART_INT_RX
//!     AM_HAL_UART_INT_DSRM
//!     AM_HAL_UART_INT_DCDM
//!     AM_HAL_UART_INT_CTSM
//!     AM_HAL_UART_INT_RIM
//
//*****************************************************************************
uint32_t
am_hal_uart_int_status_get(uint32_t ui32Module, bool bEnabledOnly)
{
    if ( bEnabledOnly )
    {
        //
        // Read and return the Masked Interrupt Status.
        //
        return AM_REGn(UART, 0, MIS);
    }
    else
    {
        //
        // Read and return the Raw Interrupt Status.
        //
        return AM_REGn(UART, 0, IES);
    }
}

//*****************************************************************************
//
//! @brief Clears the desired interrupts.
//!
//! @param ui32Interrupt - Interrupt bits to clear.
//!
//! This function clears the desired interrupts.
//!
//! ui32Interrupt should be a logical or of the following:
//!
//!     AM_HAL_UART_INT_OVER_RUN
//!     AM_HAL_UART_INT_BREAK_ERR
//!     AM_HAL_UART_INT_PARITY_ERR
//!     AM_HAL_UART_INT_FRAME_ERR
//!     AM_HAL_UART_INT_RX_TMOUT
//!     AM_HAL_UART_INT_TX
//!     AM_REG_UART_IER_TXIM_M
//!     AM_HAL_UART_INT_RX
//!     AM_HAL_UART_INT_DSRM
//!     AM_HAL_UART_INT_DCDM
//!     AM_HAL_UART_INT_CTSM
//!     AM_HAL_UART_INT_RIM
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_int_clear(uint32_t ui32Module, uint32_t ui32Interrupt)
{
    //
    // Clear the bits.
    //
    AM_REGn(UART, 0, IEC) = ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Disables the desired interrupts.
//!
//! @param ui32Interrupt - Interrupt bits to disable.
//!
//! This function disables the desired interrupts.
//!
//! ui32Interrupt should be a logical or of the following:
//!
//!     AM_HAL_UART_INT_OVER_RUN
//!     AM_HAL_UART_INT_BREAK_ERR
//!     AM_HAL_UART_INT_PARITY_ERR
//!     AM_HAL_UART_INT_FRAME_ERR
//!     AM_HAL_UART_INT_RX_TMOUT
//!     AM_HAL_UART_INT_TX
//!     AM_REG_UART_IER_TXIM_M
//!     AM_HAL_UART_INT_RX
//!     AM_HAL_UART_INT_DSRM
//!     AM_HAL_UART_INT_DCDM
//!     AM_HAL_UART_INT_CTSM
//!     AM_HAL_UART_INT_RIM
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_int_disable(uint32_t ui32Module, uint32_t ui32Interrupt)
{
    //
    // Disable the bits.
    //
    AM_REGn(UART, 0, IER) &= ~ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Enables the desired interrupts.
//!
//! @param ui32Interrupt - Interrupt bits to enable.
//!
//! This function enables the desired interrupts.
//!
//! ui32Interrupt should be a logical or of the following:
//!
//!     AM_HAL_UART_INT_OVER_RUN
//!     AM_HAL_UART_INT_BREAK_ERR
//!     AM_HAL_UART_INT_PARITY_ERR
//!     AM_HAL_UART_INT_FRAME_ERR
//!     AM_HAL_UART_INT_RX_TMOUT
//!     AM_HAL_UART_INT_TX
//!     AM_REG_UART_IER_TXIM_M
//!     AM_HAL_UART_INT_RX
//!     AM_HAL_UART_INT_DSRM
//!     AM_HAL_UART_INT_DCDM
//!     AM_HAL_UART_INT_CTSM
//!     AM_HAL_UART_INT_RIM
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_int_enable(uint32_t ui32Module, uint32_t ui32Interrupt)
{
    //
    // Enable the interrupts.
    //
    AM_REGn(UART, 0, IER) |= ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Returns the enabled interrupts.
//!
//! This function return the enabled interrupts.
//!
//! @return the enabled interrupts. This will be a logical or of the following:
//!
//!     AM_HAL_UART_INT_OVER_RUN
//!     AM_HAL_UART_INT_BREAK_ERR
//!     AM_HAL_UART_INT_PARITY_ERR
//!     AM_HAL_UART_INT_FRAME_ERR
//!     AM_HAL_UART_INT_RX_TMOUT
//!     AM_HAL_UART_INT_TX
//!     AM_REG_UART_IER_TXIM_M
//!     AM_HAL_UART_INT_RX
//!     AM_HAL_UART_INT_DSRM
//!     AM_HAL_UART_INT_DCDM
//!     AM_HAL_UART_INT_CTSM
//!     AM_HAL_UART_INT_RIM
//!
//! @return Returns the enabled interrupts.
//
//*****************************************************************************
uint32_t
am_hal_uart_int_enable_get(uint32_t ui32Module)
{
    //
    // Return the enabled interrupts.
    //
    return AM_REGn(UART, 0, IER);
}

//*****************************************************************************
//
//! @brief Enable the UART, RX, and TX.
//!
//! This function enables the UART, RX, and TX.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_enable(uint32_t ui32Module)
{
    //
    // Enable the UART, RX, and TX.
    //
    AM_REGn(UART, 0, CR) |= (AM_REG_UART_CR_UARTEN_M   |
                             AM_REG_UART_CR_RXE_M      |
                             AM_REG_UART_CR_TXE_M);
}

//*****************************************************************************
//
//! @brief Disable the UART, RX, and TX.
//!
//! This function disables the UART, RX, and TX.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_disable(uint32_t ui32Module)
{
    //
    // Disable the UART.
    //
    AM_REGn(UART, 0, CR) &= ~(AM_REG_UART_CR_UARTEN_M  |
                              AM_REG_UART_CR_RXE_M     |
                              AM_REG_UART_CR_TXE_M);
}

//*****************************************************************************
//
//! @brief Enable the UART clock.
//!
//! This function enables the clock to the UART.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_clock_enable(uint32_t ui32Module)
{
    //
    // Enable the UART clock in the MCUCTRL module.
    //
    AM_REGn(CLKGEN, 0, UARTEN) |= AM_REG_CLKGEN_UARTEN_UARTEN_M;

    //
    // Enable the UART clock.
    //
    AM_REGn(UART, 0, CR) |= AM_REG_UART_CR_CLKEN_M;

    //
    // Select default UART clock source
    //
    AM_REGn(UART, 0, CR) |= AM_REG_UART_CR_CLKSEL_24MHZ;
}

//*****************************************************************************
//
//! @brief Disable the UART clock.
//!
//! This function disables the clock to the UART.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_clock_disable(uint32_t ui32Module)
{
    //
    // Disable the UART clock.
    //
    AM_REGn(UART, 0, CR) &= ~AM_REG_UART_CR_CLKEN_M;

    //
    // Disable the UART clock in the CLKGEN module.
    //
    AM_REGn(CLKGEN, 0, UARTEN) &= ~AM_REG_CLKGEN_UARTEN_UARTEN_M;
}

//*****************************************************************************
//
//! @brief Set and enable the desired interrupt levels for the RX/TX fifo.
//!
//! @param ui32LvlCfg - Desired FIFO RX/TX levels.
//!
//! This function sets the desired interrupt levels for the RX/TX fifo and
//! enables the use of transmit and receive FIFO buffers.
//!
//! Valid values for ui32LvlCfg are:
//!
//!     AM_HAL_UART_TX_FIFO_1_8
//!     AM_HAL_UART_TX_FIFO_1_4
//!     AM_HAL_UART_TX_FIFO_1_2
//!     AM_HAL_UART_TX_FIFO_3_4
//!     AM_HAL_UART_TX_FIFO_7_8
//!
//!     AM_HAL_UART_RX_FIFO_1_8
//!     AM_HAL_UART_RX_FIFO_1_4
//!     AM_HAL_UART_RX_FIFO_1_2
//!     AM_HAL_UART_RX_FIFO_3_4
//!     AM_HAL_UART_RX_FIFO_7_8
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_fifo_config(uint32_t ui32Module, uint32_t ui32LvlCfg)
{
    //
    // Enable the use of FIFOs.
    //
    AM_REGn(UART, 0, LCRH) |= AM_REG_UART_LCRH_FEN_M;

    //
    // Write the FIFO level register.
    //
    AM_REGn(UART, 0, IFLS) = ui32LvlCfg;
}

//*****************************************************************************
//
//! @brief Return the UART Flags.
//!
//! This function reads and returns the UART flags.
//!
//! @return Returns the Flags.
//
//*****************************************************************************
uint32_t
am_hal_uart_flags_get(uint32_t ui32Module)
{
    //
    // Read and return the Flags.
    //
    return AM_REGn(UART, 0, FR);
}

//*****************************************************************************
//
//! @brief Outputs a single character using polling.
//!
//! @param cChar - Character to send.
//!
//! This function outputs a single character using polling.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_char_transmit_polled(uint32_t ui32Module, char cChar)
{
    //
    // Wait for space, i.e.  TX FIFO EMPTY
    //
    while ( AM_BFRn(UART, 0, FR, TXFF) );

    //
    // Write the char.
    //
    AM_REGn(UART, 0, DR) = cChar;
}

//*****************************************************************************
//
//! @brief Outputs a zero terminated string using polling.
//!
//! @param pcString - Pointer to character string to send.
//!
//! This function outputs a zero terminated string using polling.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_string_transmit_polled(uint32_t ui32Module, char *pcString)
{
    while ( *pcString )
    {
        //
        // Wait for space, i.e.  TX FIFO EMPTY.
        //
        while ( AM_BFRn(UART, 0, FR, TXFF) );

        //
        // Write the char.
        //
        AM_REGn(UART, 0, DR) = *pcString++;
    }
}

//*****************************************************************************
//
//! @brief Receives a character using polling.
//!
//! @param pcChar - Pointer to character to store received char.
//!
//! This function receives a character using polling.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_char_receive_polled(uint32_t ui32Module, char *pcChar)
{
    //
    // Wait for data, i.e. RX FIFO NOT EMPTY.
    //
    while ( AM_BFRn(UART, 0, FR, RXFE) );

    //
    // Save the char.
    //
    *pcChar = AM_REGn(UART, 0, DR);
}

//*****************************************************************************
//
//! @brief Receives one line using polling.
//!
//! @param ui32MaxChars - Maximum number of characters to receive.
//! @param pcChar - Pointer to character string to store received line.
//!
//! This function receives a line (delimited by '/n' or '/r') using polling.
//! Line buffer is 0 (NULL) terminated.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_line_receive_polled(uint32_t ui32Module,
                                uint32_t ui32MaxChars,
                                char *pcChar)
{
    char cRecChar;
    uint32_t i;

    //
    // Loop until we receive ui32MaxChars or receive a line ending.
    //
    for ( i = 0; i < (ui32MaxChars - 1); i++ )
    {
        //
        // Get char.
        //
        am_hal_uart_char_receive_polled(0, &cRecChar);

        if ( (cRecChar == '\n') || (cRecChar == '\r') )
        {
            //
            // Zero terminate the buffer.
            //
            *pcChar = 0;

            return;
        }

        *pcChar++ = cRecChar;
    }
}

//*****************************************************************************
//
//  Initializes a ring buffer structure.
//
//*****************************************************************************
static void
ring_buffer_init(am_hal_uart_ring_buffer_t *psBuffer,
                 void *pvArray,
                 uint32_t ui32Bytes)
{
    psBuffer->ui32WriteIndex = 0;
    psBuffer->ui32ReadIndex = 0;
    psBuffer->ui32Length = 0;
    psBuffer->ui32Capacity = ui32Bytes;
    psBuffer->pui8Data = (uint8_t *)pvArray;
}

//*****************************************************************************
//
//  Write a single byte to the ring buffer.
//
//*****************************************************************************
static bool
ring_buffer_write(am_hal_uart_ring_buffer_t *psBuffer,
                  void *pvSource,
                  uint32_t ui32Bytes)
{
    uint32_t i;
    uint8_t *pui8Source;

    pui8Source = (uint8_t *) pvSource;

    //
    // Check to make sure that the buffer isn't already full
    //
    if ( am_hal_uart_ring_buffer_space_left(psBuffer) >= ui32Bytes )
    {
        uint32_t ui32WrIdx, ui32Cap;

        //
        // Loop over the bytes in the source array.
        //
        for ( i = 0; i < ui32Bytes; i++ )
        {
            //
            // Write the value to the buffer.
            //
            psBuffer->pui8Data[psBuffer->ui32WriteIndex] = pui8Source[i];

            //
            // Advance the write index, making sure to wrap if necessary.
            //
            ui32WrIdx = psBuffer->ui32WriteIndex + 1;
            ui32Cap   = psBuffer->ui32Capacity;
            psBuffer->ui32WriteIndex = ui32WrIdx % ui32Cap;
        }

        //
        // Update the length value appropriately.
        //
        psBuffer->ui32Length += ui32Bytes;

        //
        // Report a success.
        //
        return true;
    }
    else
    {
        //
        // The ring buffer can't fit the amount of data requested. Return a
        // failure.
        //
        return false;
    }
}

//*****************************************************************************
//
//  Read a single byte from the ring buffer.
//
//*****************************************************************************
static bool
ring_buffer_read(am_hal_uart_ring_buffer_t *psBuffer,
                 void *pvDest,
                 uint32_t ui32Bytes)
{
    uint32_t i;
    uint8_t *pui8Dest;

    pui8Dest = (uint8_t *) pvDest;

    //
    // Check to make sure that the buffer isn't empty
    //
    if ( am_hal_uart_ring_buffer_data_left(psBuffer) >= ui32Bytes )
    {
        //
        // Loop over the bytes in the destination array.
        //
        for ( i = 0; i < ui32Bytes; i++ )
        {
            uint32_t ui32RdIdx, ui32Cap;

            //
            // Grab the next value from the buffer.
            //
            pui8Dest[i] = psBuffer->pui8Data[psBuffer->ui32ReadIndex];

            //
            // Advance the read index, wrapping if needed.
            //
            ui32RdIdx = psBuffer->ui32ReadIndex + 1;
            ui32Cap   = psBuffer->ui32Capacity;
            psBuffer->ui32ReadIndex = ui32RdIdx % ui32Cap;
        }

        //
        // Adjust the length value to reflect the change.
        //
        psBuffer->ui32Length--;

        //
        // Report a success.
        //
        return true;
    }
    else
    {
        //
        // If the buffer didn't have enough data, just return a zero.
        //
        return false;
    }
}

//*****************************************************************************
//
//! @brief Initialize the buffered UART.
//!
//! @param pui8RxArray - Pointer to the RX buffer to fill.
//! @param ui32RxSize - size of RX buffer.
//! @param pui8TxArray - Pointer to the TX buffer to fill.
//! @param ui32TxSize - size of TX buffer.
//!
//! This function initializes the buffered UART.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_init_buffered(uint32_t ui32Module,
                          uint8_t *pui8RxArray, uint32_t ui32RxSize,
                          uint8_t *pui8TxArray, uint32_t ui32TxSize)
{
    //
    // Enable the UART RX timeout interrupt.
    //
    AM_REGn(UART, 0, IER) |= (AM_REG_UART_IES_RTRIS_M |
                              AM_REG_UART_IES_TXRIS_M);

    //
    // Initialize the ring buffers.
    //
    ring_buffer_init(&g_sRxBuffer, pui8RxArray, ui32RxSize);
    ring_buffer_init(&g_sTxBuffer, pui8TxArray, ui32TxSize);
}
//*****************************************************************************
//
//! @brief Get the status of the buffered UART.
//!
//! @param pui32RxSize - Pointer to variable to return the Rx ring data size.
//! @param pui32TxSize - Pointer to variable to return the Tx ring data size.
//!
//! This function gets the status of the buffered UART.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_get_status_buffered(uint32_t ui32Module,
                                uint32_t *pui32RxSize,
                                uint32_t *pui32TxSize)
{
    //
    // Return the current size of ring buffers.
    //
    if (pui32RxSize)
    {
        *pui32RxSize = am_hal_uart_ring_buffer_data_left(&g_sRxBuffer);
    }
    if (pui32TxSize)
    {
        *pui32TxSize = am_hal_uart_ring_buffer_data_left(&g_sTxBuffer);
    }
}



//*****************************************************************************
//
//! @brief Services the buffered UART.
//!
//! @param ui32Status is the contents of the UART interrupt status register.
//!
//! This function is responsible for servicing the buffered UART. Designed to
//! be called from the UART interrupt handler.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_uart_service_buffered(uint32_t ui32Module, uint32_t ui32Status)
{
    uint8_t ui8Character = '\x00';

    //
    // Check to see if we have filled the Rx FIFO past the configured limit, or
    // if we have an 'old' character or two sitting in the FIFO.
    //
    if ( ui32Status & (AM_REG_UART_IES_RXRIS_M | AM_REG_UART_IES_RTRIS_M) )
    {
        //
        // While there's stuff in the RX fifo....
        //
        while ( !AM_BFRn(UART, 0, FR, RXFE) )
        {
            //
            // Read each character out one by one, and add it to the ring
            // buffer. This will start losing bytes if the fifo ever overflows.
            //
            ui8Character = AM_REGn(UART, 0 , DR);
            ring_buffer_write(&g_sRxBuffer, &ui8Character, 1);
        }
    }

    //
    // Check to see if our TX buffer has been recently emptied. If so, we
    // should refill it from the TX ring buffer.
    //
    if ( ui32Status & AM_REG_UART_IES_TXRIS_M )
    {
        //
        // Keep refilling until the fifo is full, or the ring buffer is empty,
        // whichever happens first.
        //
        while ( am_hal_uart_ring_buffer_data_left(&g_sTxBuffer) &&
                !AM_BFRn(UART, 0, FR, TXFF) )
        {
            ring_buffer_read(&g_sTxBuffer, &ui8Character, 1);
            AM_REGn(UART, 0 , DR) = ui8Character;
        }
    }
}

//*****************************************************************************
//
//! @brief Puts a char in the buffer or directly to the fifo if available.
//!
//! @param cChar - Character to send.
//!
//! This function puts a character in the buffer or directly to the fifo.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_char_transmit_buffered(uint32_t ui32Module, char cChar)
{
    //
    // Check the status of the Tx fifo and the Tx ring buffer.
    //
    if ( am_hal_uart_ring_buffer_empty(&g_sTxBuffer) &&
         !AM_BFRn(UART, 0, FR, TXFF) )
    {
        //
        // If the fifo isn't full yet, and the ring buffer isn't being used,
        // just write the new character directly to the fifo.
        //
        AM_REGn(UART, 0, DR) = cChar;
    }
    else
    {
        //
        // If we get here, either the fifo is full, or the ring buffer is
        // already in use. In either case, we need to use the ring buffer
        // to make sure that the transmitted data gets sent in the right
        // order. If the buffer is already full, we will simply lose this
        // byte.
        //
        ring_buffer_write(&g_sTxBuffer, &cChar, 1);
    }
}

//*****************************************************************************
//
//! @brief Puts a null terminaled string in the buffer or directly to the fifo.
//!
//! @param pcString - Pointer to buffer used for sending.
//!
//! This function puts a string in the buffer or directly to the fifo if there
//! is space available.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_uart_string_transmit_buffered(uint32_t ui32Module, char *pcString)
{
    //
    // Check the status of the Tx fifo and the Tx ring buffer.
    //
    while ( *pcString )
    {
        if ( am_hal_uart_ring_buffer_empty(&g_sTxBuffer)    &&
            !AM_BFRn(UART, 0, FR, TXFF) )
        {
            //
            // If the fifo isn't full yet, and the ring buffer isn't being used,
            // just write the new character directly to the fifo.
            //
            AM_REGn(UART, 0, DR) = *pcString;
        }
        else
        {
            //
            // If we get here, either the fifo is full, or the ring buffer is
            // already in use. In either case, we need to use the ring buffer
            // to make sure that the transmitted data gets sent in the right
            // order. If the buffer is already full, we will simply lose this
            // byte.
            //
            ring_buffer_write(&g_sTxBuffer, pcString, 1);
        }

        //
        // Move the pointer to the next character.
        //
        pcString++;
    }
}

//*****************************************************************************
//
//! @brief Returns n number of characters from the ring buffer or until empty.
//!
//! @param pcString - Pointer to buffer for putting received characters.
//! @param ui32MaxChars - Maximum number of characters to receive.
//!
//! This function puts a char string in the buffer.
//!
//! @return Returns the number of chars received.
//
//*****************************************************************************
uint32_t
am_hal_uart_char_receive_buffered(uint32_t ui32Module,
                                  char *pcString,
                                  uint32_t ui32MaxChars)
{
    uint32_t ui32NumChars = 0;

    //
    // Loop until ui32MaxChars or until empty.
    //
    //DCB while (am_hal_uart_ring_buffer_data_left(&g_sRxBuffer) && ui32MaxChars-- )
    while ( am_hal_uart_ring_buffer_data_left(&g_sRxBuffer) && ui32MaxChars )
    {
        //
        // Pull a char out of the ring buffer.
        //
        ring_buffer_read(&g_sRxBuffer, pcString, 1);

        //
        // Subtract from ui32MaxChars.
        // Add to ui32NumChars.
        // Move pointer in buffer.
        //
        ui32MaxChars--;
        ui32NumChars++;
        pcString++;
    }

    //
    // return the number of chars received.
    //
    return ui32NumChars;
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
