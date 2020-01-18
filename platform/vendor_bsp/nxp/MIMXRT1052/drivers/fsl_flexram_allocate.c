/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexram_allocate.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexram"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief FLEXRAM map TCM size to register value
 *
 * @param tcmBankNum tcm banknumber
 * @retval register value correspond to the tcm size
 */
static uint8_t FLEXRAM_MapTcmSizeToRegister(uint8_t tcmBankNum);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint8_t FLEXRAM_MapTcmSizeToRegister(uint8_t tcmBankNum)
{
    uint8_t tcmSizeConfig = 0U;
    uint32_t totalTcmSize = 0U;

    /* if bank number is a odd value, use a new bank number which bigger than target */
    do
    {
        if ((tcmBankNum & (tcmBankNum - 1U)) == 0U)
        {
            break;
        }
    } while (++tcmBankNum < (uint8_t)FSL_FEATURE_FLEXRAM_INTERNAL_RAM_TOTAL_BANK_NUMBERS);

    totalTcmSize = (uint32_t)tcmBankNum * ((uint32_t)FSL_FEATURE_FLEXRAM_INTERNAL_RAM_BANK_SIZE >> 10U);
    /* get bit '1' position */
    while (totalTcmSize != 0x00U)
    {
        if ((totalTcmSize & 1U) == 0U)
        {
            tcmSizeConfig++;
        }
        else
        {
            break;
        }
        totalTcmSize >>= 1U;
    }

    return tcmSizeConfig + 1U;
}

void FLEXRAM_SetTCMSize(uint8_t itcmBankNum, uint8_t dtcmBankNum)
{
    assert(itcmBankNum <= FSL_FEATURE_FLEXRAM_INTERNAL_RAM_TOTAL_BANK_NUMBERS);
    assert(dtcmBankNum <= FSL_FEATURE_FLEXRAM_INTERNAL_RAM_TOTAL_BANK_NUMBERS);

    /* dtcm configuration */
    if (dtcmBankNum != 0U)
    {
        IOMUXC_GPR->GPR14 &= ~IOMUXC_GPR_GPR14_CM7_CFGDTCMSZ_MASK;
        IOMUXC_GPR->GPR14 |= IOMUXC_GPR_GPR14_CM7_CFGDTCMSZ(FLEXRAM_MapTcmSizeToRegister(dtcmBankNum));
        IOMUXC_GPR->GPR16 |= IOMUXC_GPR_GPR16_INIT_DTCM_EN_MASK;
    }
    else
    {
        IOMUXC_GPR->GPR16 &= ~IOMUXC_GPR_GPR16_INIT_DTCM_EN_MASK;
    }

    /* itcm configuration */
    if (itcmBankNum != 0U)
    {
        IOMUXC_GPR->GPR14 &= ~IOMUXC_GPR_GPR14_CM7_CFGITCMSZ_MASK;
        IOMUXC_GPR->GPR14 |= IOMUXC_GPR_GPR14_CM7_CFGITCMSZ(FLEXRAM_MapTcmSizeToRegister(itcmBankNum));
        IOMUXC_GPR->GPR16 |= IOMUXC_GPR_GPR16_INIT_ITCM_EN_MASK;
    }
    else
    {
        IOMUXC_GPR->GPR16 &= ~IOMUXC_GPR_GPR16_INIT_ITCM_EN_MASK;
    }
}

/*!
 * brief FLEXRAM allocate on-chip ram for OCRAM,ITCM,DTCM
 * This function is independent of FLEXRAM_Init, it can be called directly if ram re-allocate
 * is needed.
 * param config allocate configuration.
 * retval kStatus_InvalidArgument the argument is invalid
 * 		   kStatus_Success allocate success
 */
status_t FLEXRAM_AllocateRam(flexram_allocate_ram_t *config)
{
    assert(config != NULL);

    uint8_t dtcmBankNum  = config->dtcmBankNum;
    uint8_t itcmBankNum  = config->itcmBankNum;
    uint8_t ocramBankNum = config->ocramBankNum;
    uint8_t i            = 0U;
    uint32_t bankCfg     = 0U;
    status_t status      = kStatus_Success;

    /* check the arguments */
    if ((uint8_t)FSL_FEATURE_FLEXRAM_INTERNAL_RAM_TOTAL_BANK_NUMBERS < (dtcmBankNum + itcmBankNum + ocramBankNum))
    {
        status = kStatus_InvalidArgument;
    }
    else
    {
        /* flexram bank config value */
        for (i = 0U; i < (uint8_t)FSL_FEATURE_FLEXRAM_INTERNAL_RAM_TOTAL_BANK_NUMBERS; i++)
        {
            if (i < ocramBankNum)
            {
                bankCfg |= ((uint32_t)kFLEXRAM_BankOCRAM) << (i * 2U);
                continue;
            }

            if (i < (dtcmBankNum + ocramBankNum))
            {
                bankCfg |= ((uint32_t)kFLEXRAM_BankDTCM) << (i * 2U);
                continue;
            }

            if (i < (dtcmBankNum + ocramBankNum + itcmBankNum))
            {
                bankCfg |= ((uint32_t)kFLEXRAM_BankITCM) << (i * 2U);
                continue;
            }
        }

        IOMUXC_GPR->GPR17 = bankCfg;

        /* set TCM size */
        FLEXRAM_SetTCMSize(itcmBankNum, dtcmBankNum);
        /* select ram allocate source from FLEXRAM_BANK_CFG */
        FLEXRAM_SetAllocateRamSrc(kFLEXRAM_BankAllocateThroughBankCfg);
    }

    return status;
}
