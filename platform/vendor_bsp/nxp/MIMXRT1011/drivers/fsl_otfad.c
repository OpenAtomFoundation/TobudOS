/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_otfad.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.otfad"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * brief OTFAD module get default configuration.
 *
 * param config configuration.
 */
void OTFAD_GetDefaultConfig(otfad_config_t *config)
{
    assert(config != NULL);

    /* Including OTFAD operate mode, IRQ eable, key blob and restricted register access */
    config->enableIntRequest    = false;
    config->forceError          = 0x00U;
    config->forceSVM            = 0x00U;
    config->forceLDM            = 0x00U;
    config->keyBlobScramble     = 0x00U;
    config->keyBlobProcess      = 0x00U;
    config->restrictedRegAccess = 0x00U;

    /* Start key blob processing */
    config->startKeyBlobProcessing = 0x00U;

    /* Global OTFAD enable */
    config->enableOTFAD = false;
}

/*!
 * brief OTFAD module initialization function, this function need to put into ram for keyblob process.
 *
 * param base OTFAD base address.
 */
AT_QUICKACCESS_SECTION_CODE(void OTFAD_Init(OTFAD_Type *base, const otfad_config_t *config))
{
    assert(config != NULL);

    /* No independent clock and it is bind with flexspi clock,
       so it need to enable clock for flexspi firstly with initialization. */

    uint32_t temp = 0U;

    /* Set OTFAD operate mode, IRQ eable, key blob and restricted register access */
    temp = OTFAD_CR_IRQE(config->enableIntRequest) | OTFAD_CR_FERR(config->forceError) |
           OTFAD_CR_FLDM(config->forceLDM) |
#if defined(FSL_FEATURE_OTFAD_HAS_SVM_MODE) && FSL_FEATURE_OTFAD_HAS_SVM_MODE
           OTFAD_CR_FSVM(config->forceSVM) |
#endif
           OTFAD_CR_KBSE(config->keyBlobScramble) | OTFAD_CR_KBPE(config->keyBlobProcess) |
           OTFAD_CR_RRAE(config->restrictedRegAccess);

    /* Enable global OTFAD operation */
    temp |= OTFAD_CR_GE(config->enableOTFAD);

    /* Wait for AHB bus idle, otherwise the keyblob process will fail */
    while (
        !((((FLEXSPI_Type *)config->flexspiBaseAddr)->STS0 & FLEXSPI_STS0_ARBIDLE_MASK) > FLEXSPI_STS0_ARBIDLE_SHIFT))
    {
    }

    /* Start key blob processing, and software set SKBP only once after a hard reset */
    if (config->startKeyBlobProcessing == 0x01U)
    {
        temp |= OTFAD_CR_SKBP(config->startKeyBlobProcessing);
    }

    base->CR |= temp;

    /* Check key blob precessing done if enabled */
    if (config->keyBlobProcess == 0x01U)
    {
        while (0U == ((base->SR & OTFAD_SR_KBD_MASK) >> OTFAD_SR_KBD_SHIFT))
        {
            ; /* Intentional empty */
        }
    }
}

/*!
 * brief Deinitializes the OTFAD.
 *
 */
AT_QUICKACCESS_SECTION_CODE(void OTFAD_Deinit(OTFAD_Type *base))
{
    /* Disable global OTFAD operation */
    base->CR &= ~OTFAD_CR_GE(1U);
}

status_t OTFAD_SetEncryptionConfig(OTFAD_Type *base, const otfad_encryption_config_t *config)
{
    assert(config != NULL);

    status_t status             = kStatus_Success;
    uint32_t temp               = 0U;
    uint32_t startAddr          = config->startAddr;
    uint32_t endAddr            = config->endAddr;
    const uint32_t *tempKey     = config->key;
    const uint32_t *tempCounter = config->counter;
    uint8_t contextIndex        = config->contextIndex;
    uint8_t regAccessMode       = 0U;

    /* Get restricted register access mode */
    regAccessMode = (uint8_t)((base->SR & OTFAD_SR_RRAM_MASK) >> OTFAD_SR_RRAM_SHIFT);

    if (regAccessMode == 0U)
    {
        /* Set context region */
        if (startAddr <= endAddr)
        {
            /* Address is aligned with 1KBytes */
            startAddr = (startAddr & OTFAD_RGD_W0_SRTADDR_MASK) >> OTFAD_RGD_W0_SRTADDR_SHIFT;
            endAddr   = (endAddr & OTFAD_RGD_W1_ENDADDR_MASK) >> OTFAD_RGD_W1_ENDADDR_SHIFT;

            temp |= OTFAD_RGD_W1_ENDADDR(endAddr) | OTFAD_RGD_W1_RO(config->readOnly) |
                    OTFAD_RGD_W1_ADE(config->AESdecryption) | OTFAD_RGD_W1_VLD(config->valid);

            /* Set context n start address  */
            base->CTX[contextIndex].RGD_W0 = OTFAD_RGD_W0_SRTADDR(startAddr);
            /* Set context n end address and enable context n AES decryption */
            base->CTX[contextIndex].RGD_W1 = temp;
        }
        else
        {
            status = kStatus_OTFAD_AddressError;
        }

        /* Set encryption key */
        base->CTX[contextIndex].KEY[0] = tempKey[0];
        base->CTX[contextIndex].KEY[1] = tempKey[1];
        base->CTX[contextIndex].KEY[2] = tempKey[2];
        base->CTX[contextIndex].KEY[3] = tempKey[3];

        /* Set encryption counter */
        base->CTX[contextIndex].CTR[0] = tempCounter[0];
        base->CTX[contextIndex].CTR[1] = tempCounter[1];
    }
    else
    {
        /* Restricted register mode, access to these registers is treated as read-as-zero, write-ignored */
        status = kStatus_OTFAD_ResRegAccessMode;
    }

    return status;
}

status_t OTFAD_GetEncryptionConfig(OTFAD_Type *base, otfad_encryption_config_t *config)
{
    assert(config != NULL);

    status_t status       = kStatus_Success;
    uint8_t regAccessMode = 0U;

    /* Get restricted register access mode */
    regAccessMode = (uint8_t)((base->SR & OTFAD_SR_RRAM_MASK) >> OTFAD_SR_RRAM_SHIFT);

    if (regAccessMode == 0U)
    {
        config->key[0] = base->CTX[config->contextIndex].KEY[0];
        config->key[1] = base->CTX[config->contextIndex].KEY[1];
        config->key[2] = base->CTX[config->contextIndex].KEY[2];
        config->key[3] = base->CTX[config->contextIndex].KEY[3];

        config->counter[0] = base->CTX[config->contextIndex].CTR[0];
        config->counter[1] = base->CTX[config->contextIndex].CTR[1];

        config->startAddr =
            (base->CTX[config->contextIndex].RGD_W0 & OTFAD_RGD_W0_SRTADDR_MASK) >> OTFAD_RGD_W0_SRTADDR_SHIFT;
        config->endAddr =
            (base->CTX[config->contextIndex].RGD_W1 & OTFAD_RGD_W1_ENDADDR_MASK) >> OTFAD_RGD_W1_ENDADDR_SHIFT;
    }
    else
    {
        /* Restricted register mode, access to these registers is treated as read-as-zero, write-ignored */
        status = kStatus_OTFAD_ResRegAccessMode;
    }

    return status;
}

status_t OTFAD_HitDetermination(OTFAD_Type *base, uint32_t address, uint8_t *contextIndex)
{
    status_t status   = kStatus_Success;
    bool contextValid = false;
    uint8_t hitNumber = 0U;
    uint8_t totalHits = 0U;
    uint8_t index     = 0U;
    uint8_t i;
    uint32_t startAddr;
    uint32_t endAddr;
    uint32_t tempAddr;

    /* Address is aligned with 1KBytes */
    tempAddr = (address & 0xFFFFFC00U) >> 10U;

    for (i = 0U; i < OTFAD_RGD_W1_COUNT; i++)
    {
        startAddr = (base->CTX[index].RGD_W0 & OTFAD_RGD_W0_SRTADDR_MASK) >> OTFAD_RGD_W0_SRTADDR_SHIFT;
        endAddr   = (base->CTX[index].RGD_W1 & OTFAD_RGD_W1_ENDADDR_MASK) >> OTFAD_RGD_W1_ENDADDR_SHIFT;
        contextValid =
            ((base->CTX[index].RGD_W1 & OTFAD_RGD_W1_VLD_MASK) >> OTFAD_RGD_W1_VLD_SHIFT) == 1U ? true : false;

        /* Check address hits context n or not */
        if ((tempAddr >= startAddr) && (tempAddr <= endAddr) && contextValid)
        {
            totalHits++;
            hitNumber = i;
        }

        index++;
    }

    /* Hit in multiple contexts or no contexts */
    if (totalHits != 1U)
    {
        status = kStatus_OTFAD_RegionOverlap;
    }
    /* Hit in single context */
    else
    {
        *contextIndex = hitNumber;
    }

    return status;
}
