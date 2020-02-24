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

/** \addtogroup efc_module Working with Enhanced Embedded Flash
 * The EEFC driver provides the interface to configure and use the EEFC
 * peripheral.
 * 
 * The user needs to set the number of wait states depending on the frequency used.\n
 * Configure number of cycles for flash read/write operations in the FWS field of EEFC_FMR.
 * 
 * It offers a function to send flash command to EEFC and waits for the
 * flash to be ready.
 *
 * To send flash command, the user could do in either of following way:
 * <ul>
 * <li>Write a correct key, command and argument in EEFC_FCR. </li>
 * <li>Or, Use IAP (In Application Programming) function which is executed from
 * ROM directly, this allows flash programming to be done by code running in flash.</li>
 * <li>Once the command is achieved, it can be detected even by polling EEFC_FSR or interrupt.
 * </ul>
 *
 * The command argument could be a page number,GPNVM number or nothing, it depends on
 * the command itself. Some useful functions in this driver could help user tranlate physical 
 * flash address into a page number and vice verse.
 *
 * For more accurate information, please look at the EEFC section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref efc.c\n
 * \ref efc.h.\n
*/
/*@{*/
/*@}*/


/**
 * \file
 *
 * Implementation of Emhance embedded Flash (EEFC) controller.
 *
 */


/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include <board.h>
#include "efc.h"
#include <utility/assert.h>
#include <utility/trace.h>


/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Enables the flash ready interrupt source on the EEFC peripheral.
 *
 * \param efc  Pointer to a Efc instance
 */
void EFC_EnableFrdyIt(Efc *efc)
{
    efc->EEFC_FMR |= EEFC_FMR_FRDY;
}

/**
 * \brief Disables the flash ready interrupt source on the EEFC peripheral.
 *
 * \param efc  Pointer to a Efc instance
 */

void EFC_DisableFrdyIt(Efc *efc)
{
    efc->EEFC_FMR &= ~EEFC_FMR_FRDY;
}


/**
 * \brief Set read/write wait state on the EEFC perpherial.
 *
 * \param efc  Pointer to a Efc instance
 * \param cycles  the number of wait states in cycle.
 */

void EFC_SetWaitState(Efc *efc, uint8_t cycles)
{
    uint32_t value;
    value = efc->EEFC_FMR;
    value &= ~EEFC_FMR_FWS;
    value |= cycles << 8;
    efc->EEFC_FMR = value;
}

/**
 * \brief Returns the current status of the EEFC. 
 *
 * \note Keep in mind that this function clears the value of some status bits (LOCKE, PROGE).
 *
 * \param efc  Pointer to a Efc instance
 */
uint32_t EFC_GetStatus(Efc *efc)
{
    return efc->EEFC_FSR;
}

/**
 * \brief Returns the result of the last executed command.
 *
 * \param efc  Pointer to a Efc instance
 */
uint32_t EFC_GetResult(Efc *efc) {

    return efc->EEFC_FRR;
}

/**
 * \brief Translates the given address page and offset values. 
 * \note The resulting values are stored in the provided variables if they are not null.
 *
 * \param efc  Pointer to a Efc instance
 * \param address  Address to translate.
 * \param pPage  First page accessed.
 * \param pOffset  Byte offset in first page.
 */
void EFC_TranslateAddress(
    Efc **efc,
    uint32_t address,
    unsigned short *pPage,
    unsigned short *pOffset)
{
    Efc *pEfc;
    unsigned short page;
    unsigned short offset;
    
    SANITY_CHECK(address >= AT91C_IFLASH);
    SANITY_CHECK(address <= (AT91C_IFLASH + AT91C_IFLASH_SIZE));

    pEfc = EFC;
    page = (address - AT91C_IFLASH) / AT91C_IFLASH_PAGE_SIZE;
    offset = (address - AT91C_IFLASH) % AT91C_IFLASH_PAGE_SIZE;
  
    TRACE_DEBUG("Translated 0x%08X to page=%d and offset=%d\n\r", address, page, offset);
    // Store values
    if (pEfc) {
        *efc = pEfc;
    }
    if (pPage) {
        *pPage = page;
    }
    if (pOffset) {
        *pOffset = offset;
    }
}

/**
 * \brief Computes the address of a flash access given the page and offset.
 *
 * \param efc  Pointer to a Efc instance
 * \param page  Page number.
 * \param offset  Byte offset inside page.
 * \param pAddress  Computed address (optional).
 */
void EFC_ComputeAddress(
    Efc *efc,
    unsigned short page,
    unsigned short offset,
    uint32_t *pAddress)
{
    uint32_t address;
    SANITY_CHECK(efc);
    SANITY_CHECK(page <= AT91C_IFLASH_NB_OF_PAGES);
    SANITY_CHECK(offset < AT91C_IFLASH_PAGE_SIZE);
    // Compute address
    address = AT91C_IFLASH + page * AT91C_IFLASH_PAGE_SIZE + offset;
    // Store result
    if (pAddress) {
        *pAddress = address;
    }
}

/**
 * \brief Starts the executing the given command on the EEFC and returns as soon as the command is started.
 *
 * \note It does NOT set the FMCN field automatically.
 * \param efc  Pointer to a Efc instance
 * \param command  Command to execute.
 * \param argument  Command argument (should be 0 if not used).
 */
void EFC_StartCommand(Efc *efc, uint8_t command, unsigned short argument)
{
   // Check command & argument
    switch (command) {

        case EFC_FCMD_WP:
        case EFC_FCMD_WPL:
        case EFC_FCMD_EWP: 
        case EFC_FCMD_EWPL:
        case EFC_FCMD_SLB:
        case EFC_FCMD_CLB:
            ASSERT(argument < AT91C_IFLASH_NB_OF_PAGES,
                   "-F- Embedded flash has only %d pages\n\r",
                   AT91C_IFLASH_NB_OF_PAGES);
            break;

        case EFC_FCMD_SFB:
        case EFC_FCMD_CFB:
            ASSERT(argument < 2, "-F- Embedded flash has only %d GPNVMs\n\r", 2);
            break;

        case EFC_FCMD_GETD:
        case EFC_FCMD_EA:
        case EFC_FCMD_GLB:
        case EFC_FCMD_GFB:
        case EFC_FCMD_STUI:
            ASSERT(argument == 0, "-F- Argument is meaningless for the given command.\n\r");
            break;

        default: ASSERT(0, "-F- Unknown command %d\n\r", command);
    }

    // Start command Embedded flash 
    ASSERT((efc->EEFC_FSR & EEFC_FMR_FRDY) == EEFC_FMR_FRDY, "-F- EEFC is not ready\n\r");
    efc->EEFC_FCR = (0x5A << 24) | (argument << 8) | command;
}

/**
 * \brief Performs the given command and wait until its completion (or an error).
 *
 * \param efc  Pointer to a Efc instance
 * \param command  Command to perform.
 * \param argument  Optional command argument.
 * \return 0 if successful, otherwise returns an error code.
 */

uint8_t EFC_PerformCommand(Efc *efc, uint8_t command, unsigned short argument)
{
    
#if defined(flash) || defined(USE_IAP_FEATURE)
   // Pointer on IAP function in ROM
    static uint32_t  (*IAP_PerformCommand)(uint32_t, uint32_t);
    IAP_PerformCommand = (uint32_t (*)(uint32_t, uint32_t)) *((uint32_t *) CHIP_FLASH_IAP_ADDRESS);
    IAP_PerformCommand(0, (0x5A << 24) | (argument << 8) | command);
    return (efc->EEFC_FSR & (EEFC_FSR_FLOCKE | EEFC_FSR_FCMDE));

#else
    uint32_t status;
    efc->EEFC_FCR = (0x5A << 24) | (argument << 8) | command;
    do {

        status = efc->EEFC_FSR;
    }
    while ((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);

    return (status & (EEFC_FSR_FLOCKE | EEFC_FSR_FCMDE));
#endif    
}


