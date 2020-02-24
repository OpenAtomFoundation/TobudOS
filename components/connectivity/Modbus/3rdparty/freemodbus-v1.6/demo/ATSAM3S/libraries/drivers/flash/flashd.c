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

/** \addtogroup flashd_module Flash Memory Interface
 * The flash driver manages the programming, erasing, locking and unlocking sequences 
 * with dedicated commands.
 * 
 * To implement flash programing operation, the user has to follow these few steps :
 * <ul>
 * <li>Configue flash wait states to initializes the flash. </li>
 * <li>Checks whether a region to be programmed is locked. </li>
 * <li>Unlocks the user region to be programmed if the region have locked before.</li>
 * <li>Erases the user page before program (optional).</li>
 * <li>Writes the user page from the page buffer.</li>
 * <li>Locks the region of programmed area if any.</li>
 * </ul>
 *
 * Writing 8-bit and 16-bit data is not allowed and may lead to unpredictable data corruption.
 * A check of this validity and padding for 32-bit alignment should be done in write algorithm.
 
 * Lock/unlock range associated with the user address range is automatically translated.
 *
 * This security bit can be enabled through the command "Set General Purpose NVM Bit 0".
 *
 * A 128-bit factory programmed unique ID could be read to serve several purposes. 
 * 
 * The driver accesses the flash memory by calling the lowlevel module provided in \ref efc_module.
 * For more accurate information, please look at the EEFC section of the Datasheet.
 *
 * Related files :\n
 * \ref flashd.c\n
 * \ref flashd.h.\n
 * \ref efc.c\n
 * \ref efc.h.\n
*/
/*@{*/
/*@}*/


/**
 * \file
 *
 * The flash driver provides the unified interface for flash program operations.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "flashd.h"
#include <board.h>

#include <efc/efc.h>
#include <utility/math.h>
#include <utility/assert.h>
#include <utility/trace.h>

#include <string.h>


/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Computes the lock range associated with the given address range.
 *
 * \param start  Start address of lock range.
 * \param end  End address of lock range.
 * \param pActualStart  Actual start address of lock range.
 * \param pActualEnd  Actual end address of lock range.
 */
static void ComputeLockRange(
    uint32_t start,
    uint32_t end,
    uint32_t *pActualStart,
    uint32_t *pActualEnd)
{
    Efc *pStartEfc, *pEndEfc;
    uint16_t startPage, endPage;
    uint16_t numPagesInRegion;
    uint16_t actualStartPage, actualEndPage;

    // Convert start and end address in page numbers
    EFC_TranslateAddress(&pStartEfc, start, &startPage, 0);
    EFC_TranslateAddress(&pEndEfc, end, &endPage, 0);

    // Find out the first page of the first region to lock
    numPagesInRegion = AT91C_IFLASH_LOCK_REGION_SIZE / AT91C_IFLASH_PAGE_SIZE;
    actualStartPage = startPage - (startPage % numPagesInRegion);
    actualEndPage = endPage;
    if ((endPage % numPagesInRegion) != 0) {

        actualEndPage += numPagesInRegion - (endPage % numPagesInRegion);
    }
    // Store actual page numbers
    EFC_ComputeAddress(pStartEfc, actualStartPage, 0, pActualStart);
    EFC_ComputeAddress(pEndEfc, actualEndPage, 0, pActualEnd);
    TRACE_DEBUG("Actual lock range is 0x%06X - 0x%06X\n\r", *pActualStart, *pActualEnd);
}


/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Initializes the flash driver.
 *
 * \param mck  Master clock frequency in Hz.
 */

void FLASHD_Initialize(uint32_t mck)
{
    EFC_DisableFrdyIt(EFC);
    if ((mck/1000000) >= 64) {
        EFC_SetWaitState(EFC, 2);
    }
    else if ((mck/1000000) >= 50) {
        EFC_SetWaitState(EFC, 1);
    }
    else {
        EFC_SetWaitState(EFC, 0);
    }
}

/**
 * \brief Erases the entire flash.
 *
 * \param address  Flash start address.
 * \return 0 if successful; otherwise returns an error code.
 */
uint8_t FLASHD_Erase(uint32_t address)
{
    Efc *pEfc;
    uint16_t page;
    uint16_t offset;
    uint8_t error;
    SANITY_CHECK((address >=AT91C_IFLASH) || (address <= (AT91C_IFLASH + AT91C_IFLASH_SIZE)));
    // Translate write address
    EFC_TranslateAddress(&pEfc, address, &page, &offset);
    error = EFC_PerformCommand(pEfc, EFC_FCMD_EA, 0);
    return error;
}

    
static uint8_t pPageBuffer[AT91C_IFLASH_PAGE_SIZE];
/**
 * \brief Writes a data buffer in the internal flash
 *
 * \note This function works in polling mode, and thus only returns when the 
 * data has been effectively written.
 * \param address  Write address.
 * \param pBuffer  Data buffer.
 * \param size  Size of data buffer in bytes.
 * \return 0 if successful, otherwise returns an error code.
 */
uint8_t FLASHD_Write(
    uint32_t address,
    const void *pBuffer,
    uint32_t size)
{
    Efc *pEfc;
    uint16_t page;
    uint16_t offset;
    uint32_t writeSize;
    uint32_t pageAddress;
    uint16_t padding;
    uint8_t error;

    uint32_t sizeTmp;
    uint32_t *pAlignedDestination; 
    uint32_t *pAlignedSource;
    
    SANITY_CHECK(pBuffer);
    SANITY_CHECK(address >=AT91C_IFLASH);
    SANITY_CHECK((address + size) <= (AT91C_IFLASH + AT91C_IFLASH_SIZE));
    // Translate write address
    EFC_TranslateAddress(&pEfc, address, &page, &offset);

    // Write all pages
    while (size > 0) {

        // Copy data in temporary buffer to avoid alignment problems
        writeSize = min(AT91C_IFLASH_PAGE_SIZE - offset, size);
        EFC_ComputeAddress(pEfc, page, 0, &pageAddress);
        padding = AT91C_IFLASH_PAGE_SIZE - offset - writeSize;

        // Pre-buffer data
        memcpy(pPageBuffer, (void *) pageAddress, offset);

        // Buffer data
        memcpy(pPageBuffer + offset, pBuffer, writeSize);

        // Post-buffer data
        memcpy(pPageBuffer + offset + writeSize, (void *) (pageAddress + offset + writeSize), padding);

        // Write page
        // Writing 8-bit and 16-bit data is not allowed 
        // and may lead to unpredictable data corruption
        pAlignedDestination = (uint32_t*)pageAddress;
        pAlignedSource = (uint32_t*)pPageBuffer;        
        sizeTmp = AT91C_IFLASH_PAGE_SIZE;
        while (sizeTmp >= 4) {

            *pAlignedDestination++ = *pAlignedSource++;
            sizeTmp -= 4;
        }        
               
        // Send writing command
        error = EFC_PerformCommand(pEfc, EFC_FCMD_EWP, page);
        if (error) {

            return error;
        }

        // Progression
        address += AT91C_IFLASH_PAGE_SIZE;
        pBuffer = (void *) ((uint32_t) pBuffer + writeSize);
        size -= writeSize;
        page++;
        offset = 0;
    }

    return 0;
}
/**
 * \brief Locks all the regions in the given address range. The actual lock range is
 * reported through two output parameters.
 * \param address  Start address of lock range.
 * \param end  End address of lock range.
 * \param pActualStart  Start address of the actual lock range (optional).
 * \param pActualEnd  End address of the actual lock range (optional).
 * \return 0 if successful, otherwise returns an error code.
 */
uint8_t FLASHD_Lock(
    uint32_t start,
    uint32_t end,
    uint32_t *pActualStart,
    uint32_t *pActualEnd)
{
    Efc *pEfc;
    uint32_t actualStart, actualEnd;
    uint16_t startPage, endPage;
    uint8_t error;
    uint16_t numPagesInRegion = AT91C_IFLASH_LOCK_REGION_SIZE / AT91C_IFLASH_PAGE_SIZE;

    // Compute actual lock range and store it
    ComputeLockRange(start, end, &actualStart, &actualEnd);
    if (pActualStart) {

        *pActualStart = actualStart;
    }
    if (pActualEnd) {

        *pActualEnd = actualEnd;
    }

    // Compute page numbers
    EFC_TranslateAddress(&pEfc, actualStart, &startPage, 0);
    EFC_TranslateAddress(0, actualEnd, &endPage, 0);

    // Lock all pages
    while (startPage < endPage) {

        error = EFC_PerformCommand(pEfc, EFC_FCMD_SLB, startPage);
        if (error) {

            return error;
        }
        startPage += numPagesInRegion;
    }

    return 0;
}

/**
 * \brief Unlocks all the regions in the given address range. The actual unlock range is
 * reported through two output parameters.
 * \param address  Start address of unlock range.
 * \param end  End address of unlock range.
 * \param pActualStart  Start address of the actual unlock range (optional).
 * \param pActualEnd  End address of the actual unlock range (optional).
 * \return 0 if successful, otherwise returns an error code.
 */
uint8_t FLASHD_Unlock(
    uint32_t start,
    uint32_t end,
    uint32_t *pActualStart,
    uint32_t *pActualEnd)
{
    Efc *pEfc;
    uint32_t actualStart, actualEnd;
    uint16_t startPage, endPage;
    uint8_t error;
    uint16_t numPagesInRegion = AT91C_IFLASH_LOCK_REGION_SIZE / AT91C_IFLASH_PAGE_SIZE;

    // Compute actual unlock range and store it
    ComputeLockRange(start, end, &actualStart, &actualEnd);
    if (pActualStart) {

        *pActualStart = actualStart;
    }
    if (pActualEnd) {

        *pActualEnd = actualEnd;
    }

    // Compute page numbers
    EFC_TranslateAddress(&pEfc, actualStart, &startPage, 0);
    EFC_TranslateAddress(0, actualEnd, &endPage, 0);

    // Unlock all pages
    while (startPage < endPage) {

        error = EFC_PerformCommand(pEfc, EFC_FCMD_CLB, startPage);
        if (error) {

            return error;
        }
        startPage += numPagesInRegion;
    }
    return 0;
}

/**
 * \brief Returns the number of locked regions inside the given address range.
 *
 * \param address  Start address of range
 * \param end  End address of range.
 */
uint8_t FLASHD_IsLocked(uint32_t start, uint32_t end)
{
    Efc *pEfc;
    uint16_t startPage, endPage;
    uint8_t startRegion, endRegion;
    uint32_t numPagesInRegion;
    uint32_t status;
    uint8_t error;
    uint32_t numLockedRegions = 0;

    SANITY_CHECK(end >= start);
    SANITY_CHECK((start >=AT91C_IFLASH) && (end <= AT91C_IFLASH + AT91C_IFLASH_SIZE)); 

    // Compute page numbers
    EFC_TranslateAddress(&pEfc, start, &startPage, 0);
    EFC_TranslateAddress(0, end, &endPage, 0);

    // Compute region numbers
    numPagesInRegion = AT91C_IFLASH_LOCK_REGION_SIZE / AT91C_IFLASH_PAGE_SIZE;
    startRegion = startPage / numPagesInRegion;
    endRegion = endPage / numPagesInRegion;
    if ((endPage % numPagesInRegion) != 0) {

        endRegion++;
    }

    // Retrieve lock status
    error = EFC_PerformCommand(pEfc, EFC_FCMD_GLB, 0);
    ASSERT(!error, "-F- Error while trying to fetch lock bits status (0x%02X)\n\r", error);
    status = EFC_GetResult(pEfc);

    // Check status of each involved region
    while (startRegion < endRegion) {

        if ((status & (1 << startRegion)) != 0) {

            numLockedRegions++;
        }
        startRegion++;
    }

    return numLockedRegions;
}

/**
 * \brief Check if the given GPNVM bit is set or not.
 *
 * \param gpnvm  GPNVM bit index.
 * \returns 1 if the given GPNVM bit is currently set; otherwise returns 0.
 */
uint8_t FLASHD_IsGPNVMSet(uint8_t gpnvm)
{
    uint8_t error;
    uint32_t status;

    SANITY_CHECK(gpnvm < 2);

    // Get GPNVMs status
    error = EFC_PerformCommand(EFC, EFC_FCMD_GFB, 0);
    ASSERT(!error, "-F- Error while trying to fetch GPNVMs status (0x%02X)\n\r", error);
    status = EFC_GetResult(EFC);

    // Check if GPNVM is set
    if ((status & (1 << gpnvm)) != 0) {
        return 1;
    }
    else {
        return 0;
    }
}
/**
 * \brief Sets the selected GPNVM bit.
 *
 * \param gpnvm  GPNVM bit index.
 * \returns 0 if successful; otherwise returns an error code.
 */
uint8_t FLASHD_SetGPNVM(uint8_t gpnvm)
{
    SANITY_CHECK(gpnvm < 2);

    if (!FLASHD_IsGPNVMSet(gpnvm)) {

        return EFC_PerformCommand(EFC, EFC_FCMD_SFB, gpnvm);
    }
    else {

        return 0;
    }
}

/**
 * \brief Clears the selected GPNVM bit.
 *
 * \param gpnvm  GPNVM bit index.
 * \returns 0 if successful; otherwise returns an error code.
 */
uint8_t FLASHD_ClearGPNVM(uint8_t gpnvm)
{
    SANITY_CHECK(gpnvm < 2);

    if (FLASHD_IsGPNVMSet(gpnvm)) {

        return EFC_PerformCommand(EFC, EFC_FCMD_CFB, gpnvm);
    }
    else {

        return 0;
    }
}
/**
 * \brief Read the unique ID.
 *
 * \param uniqueID pointer on a 4bytes char containing the unique ID value.
 * \returns 0 if successful; otherwise returns an error code.
 */
uint8_t FLASHD_ReadUniqueID (uint32_t * uniqueID)
{
    uint8_t error;
    SANITY_CHECK(uniqueID != NULL);

    uniqueID[0] = 0;
    uniqueID[1] = 0;
    uniqueID[2] = 0;
    uniqueID[3] = 0;

    EFC_StartCommand(EFC, EFC_FCMD_STUI, 0);

    uniqueID[0] = *(uint32_t *)AT91C_IFLASH;
    uniqueID[1] = *(uint32_t *)(AT91C_IFLASH + 4);
    uniqueID[2] = *(uint32_t *)(AT91C_IFLASH + 8);
    uniqueID[3] = *(uint32_t *)(AT91C_IFLASH + 12);

    error = EFC_PerformCommand(EFC, EFC_FCMD_SPUI, 0);
    if (error) return error;

    return 0;
}
