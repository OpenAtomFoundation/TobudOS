/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/** \addtogroup spi_module Working with SPI
 * The SPI driver provides the interface to configure and use the SPI
 * peripheral.
 *
 * The Serial Peripheral Interface (SPI) circuit is a synchronous serial
 * data link that provides communication with external devices in Master
 * or Slave Mode.
 *
 * To use the SPI, the user has to follow these few steps:
 * -# Enable the SPI pins required by the application (see pio.h).
 * -# Configure the SPI using the \ref SPI_Configure(). This enables the
 *    peripheral clock. The mode register is loaded with the given value.
 * -# Configure all the necessary chip selects with \ref SPI_ConfigureNPCS().
 * -# Enable the SPI by calling \ref SPI_Enable().
 * -# Send/receive data using \ref SPI_Write() and \ref SPI_Read(). Note that \ref SPI_Read()
 *    must be called after \ref SPI_Write() to retrieve the last value read.
 * -# Send/receive data using the PDC with the \ref SPI_WriteBuffer() and
 *    \ref SPI_ReadBuffer() functions.
 * -# Disable the SPI by calling \ref SPI_Disable().
 *
 * For more accurate information, please look at the SPI section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref spi.c\n
 * \ref spi.h.\n
*/
/*@{*/
/*@}*/

/**
 * \file
 *
 * Implementation of Serial Peripheral Interface (SPI) controller.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include "spi.h"
#include "pmc/pmc.h"

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Enables a SPI peripheral.
 *
 * \param spi  Pointer to an Spi instance.
 */
void SPI_Enable(Spi *spi)
{
    spi->SPI_CR = SPI_CR_SPIEN;
}

/**
 * \brief Disables a SPI peripheral.
 *
 * \param spi  Pointer to an Spi instance.
 */
void SPI_Disable(Spi *spi)
{
    spi->SPI_CR = SPI_CR_SPIDIS;
}

/**
 * \brief Configures a SPI peripheral as specified. The configuration can be computed
 * using several macros (see \ref spi_configuration_macros).
 *
 * \param spi  Pointer to an Spi instance.
 * \param id   Peripheral ID of the SPI.
 * \param configuration  Value of the SPI configuration register.
 */
void SPI_Configure(Spi *spi,
                   uint32_t id,
                   uint32_t configuration)
{
    PMC_EnablePeripheral(ID_SPI);
    spi->SPI_CR = SPI_CR_SPIDIS;

    /* Execute a software reset of the SPI twice */
    spi->SPI_CR = SPI_CR_SWRST;
    spi->SPI_CR = SPI_CR_SWRST;
    spi->SPI_MR = configuration;
}

/**
 * \brief Configures a chip select of a SPI peripheral. The chip select configuration
 * is computed using several macros (see \ref spi_configuration_macros).
 *
 * \param spi   Pointer to an Spi instance.
 * \param npcs  Chip select to configure (0, 1, 2 or 3).
 * \param configuration  Desired chip select configuration.
 */
void SPI_ConfigureNPCS(Spi *spi,
                       uint32_t npcs,
                       uint32_t configuration)
{
    spi->SPI_CSR[npcs] = configuration;
}

/**
 * \brief Sends data through a SPI peripheral. If the SPI is configured to use a fixed
 * peripheral select, the npcs value is meaningless. Otherwise, it identifies
 * the component which shall be addressed.
 *
 * \param spi   Pointer to an Spi instance.
 * \param npcs  Chip select of the component to address (0, 1, 2 or 3).
 * \param data  Word of data to send.
 */
void SPI_Write(Spi *spi, uint32_t npcs, uint16_t data)
{
    /* Send data */
    while ((spi->SPI_SR & SPI_SR_TXEMPTY) == 0);
    spi->SPI_TDR = data | SPI_PCS(npcs);
    while ((spi->SPI_SR & SPI_SR_TDRE) == 0);
}

/**
 * \brief Sends the contents of buffer through a SPI peripheral, using the PDC to
 * take care of the transfer.
 *
 * \param spi     Pointer to an Spi instance.
 * \param buffer  Data buffer to send.
 * \param length  Length of the data buffer.
 */
uint8_t SPI_WriteBuffer(Spi *spi,
                              void *buffer,
                              uint32_t length)
{
    /* Check if first bank is free */
    if (spi->SPI_TCR == 0) {

        spi->SPI_TPR = (uint32_t) buffer;
        spi->SPI_TCR = length;
        spi->SPI_PTCR = PERIPH_PTCR_TXTEN;
        return 1;
    }
    /* Check if second bank is free */
    else if (spi->SPI_TNCR == 0) {

        spi->SPI_TNPR = (uint32_t) buffer;
        spi->SPI_TNCR = length;
        return 1;
    }

    /* No free banks */
    return 0;
}

/**
 * \brief Check if SPI transfer finish.
 *
 * \param spi  Pointer to an Spi instance.
 *
 * \return Returns 1 if there is no pending write operation on the SPI; otherwise
 * returns 0.
 */
uint8_t SPI_IsFinished(Spi *spi)
{
    return ((spi->SPI_SR & SPI_SR_TXEMPTY) != 0);
}

/**
 * \brief Reads and returns the last word of data received by a SPI peripheral. This
 * method must be called after a successful SPI_Write call.
 *
 * \param spi  Pointer to an Spi instance.
 *
 * \return readed data.
 */
uint16_t SPI_Read(Spi *spi)
{
    while ((spi->SPI_SR & SPI_SR_RDRF) == 0);
    return spi->SPI_RDR & 0xFFFF;
}

/**
 * \brief Reads data from a SPI peripheral until the provided buffer is filled. This
 * method does NOT need to be called after SPI_Write or SPI_WriteBuffer.
 *
 * \param spi     Pointer to an Spi instance.
 * \param buffer  Data buffer to store incoming bytes.
 * \param length  Length in bytes of the data buffer.
 */
uint8_t SPI_ReadBuffer(Spi *spi,
                             void *buffer,
                             uint32_t length)
{
    /* Check if the first bank is free */
    if (spi->SPI_RCR == 0) {

        spi->SPI_RPR = (uint32_t) buffer;
        spi->SPI_RCR = length;
        spi->SPI_PTCR = PERIPH_PTCR_RXTEN;
        return 1;
    }
    /* Check if second bank is free */
    else if (spi->SPI_RNCR == 0) {

        spi->SPI_RNPR = (uint32_t) buffer;
        spi->SPI_RNCR = length;
        return 1;
    }

    /* No free bank */
    return 0;
}
