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
 * The flash driver provides the unified interface for flash program operations.
 *
 */
 
#ifndef FLASHD_H
#define FLASHD_H

#include <board.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern void FLASHD_Initialize(uint32_t mck);

extern uint8_t FLASHD_Erase(uint32_t address);

extern uint8_t FLASHD_Write(
    uint32_t address,
    const void *pBuffer,
    uint32_t size);

extern uint8_t FLASHD_Lock(
    uint32_t start,
    uint32_t end,
    uint32_t *pActualStart,
    uint32_t *pActualEnd);

extern uint8_t FLASHD_Unlock(
    uint32_t start,
    uint32_t end,
    uint32_t *pActualStart,
    uint32_t *pActualEnd);

extern uint8_t FLASHD_IsLocked(
    uint32_t start,
    uint32_t end);

extern uint8_t FLASHD_SetGPNVM(uint8_t gpnvm);

extern uint8_t FLASHD_ClearGPNVM(uint8_t gpnvm);

extern uint8_t FLASHD_IsGPNVMSet(uint8_t gpnvm);

#define FLASHD_IsSecurityBitSet() FLASHD_IsGPNVMSet(0)

#define FLASHD_SetSecurityBit()   FLASHD_SetGPNVM(0)

extern uint8_t FLASHD_ReadUniqueID(uint32_t * uniqueID);

#endif //#ifndef FLASHD_H

