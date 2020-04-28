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

/**
 * \file
 *
 * Interface for Serial Peripheral Interface (SPI) controller.
 *
 */

#ifndef SPI_H
#define SPI_H

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <board.h>

/*----------------------------------------------------------------------------
 *        Macros
 *----------------------------------------------------------------------------*/

/**
 * \page spi_configuration_macros SPI configuration macros
 *
 * This page lists several macros which should be used when configuring a SPI
 * peripheral.
 *
 * \par Macros:
 * - \ref SPI_PCS
 * - \ref SPI_SCBR
 * - \ref SPI_DLYBS
 * - \ref SPI_DLYBCT
 */

/** Calculate the PCS field value given the chip select NPCS value */
#define SPI_PCS(npcs)       ((~(1 << npcs) & 0xF) << 16)

/** Calculates the value of the CSR SCBR field given the baudrate and MCK. */
#define SPI_SCBR(baudrate, masterClock) \
            ((uint32_t) (masterClock / baudrate) << 8)

/** Calculates the value of the CSR DLYBS field given the desired delay (in ns) */
#define SPI_DLYBS(delay, masterClock) \
            ((uint32_t) (((masterClock / 1000000) * delay) / 1000) << 16)

/** Calculates the value of the CSR DLYBCT field given the desired delay (in ns) */
#define SPI_DLYBCT(delay, masterClock) \
            ((uint32_t) (((masterClock / 1000000) * delay) / 32000) << 24)
/*------------------------------------------------------------------------------ */

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern void SPI_Enable(Spi *spi);
extern void SPI_Disable(Spi *spi);
extern void SPI_Configure(Spi *spi,
                          uint32_t id,
                          uint32_t configuration);
extern void SPI_ConfigureNPCS(Spi *spi,
                              uint32_t npcs,
                              uint32_t configuration);
extern void SPI_Write(Spi *spi, uint32_t npcs, uint16_t data);
extern uint8_t SPI_WriteBuffer(Spi *spi,
                                     void *buffer,
                                     uint32_t length);

extern uint8_t SPI_IsFinished(Spi *pSpi);

extern uint16_t SPI_Read(Spi *spi);
extern uint8_t SPI_ReadBuffer(Spi *spi,
                                    void *buffer,
                                    uint32_t length);

#endif /* #ifndef SPI_H */

