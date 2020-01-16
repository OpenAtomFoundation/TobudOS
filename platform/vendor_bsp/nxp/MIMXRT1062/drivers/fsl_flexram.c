/*
 * Copyright 2017-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexram.h"

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
 * @brief Gets the instance from the base address to be used to gate or ungate the module clock
 *
 * @param base FLEXRAM base address
 *
 * @return The FLEXRAM instance
 */
static uint32_t FLEXRAM_GetInstance(FLEXRAM_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to FLEXRAM bases for each instance. */
static FLEXRAM_Type *const s_flexramBases[] = FLEXRAM_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to FLEXRAM clocks for each instance. */
static const clock_ip_name_t s_flexramClocks[] = FLEXRAM_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t FLEXRAM_GetInstance(FLEXRAM_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_flexramBases); instance++)
    {
        if (s_flexramBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_flexramBases));

    return instance;
}

/*!
 * brief FLEXRAM module initialization function.
 *
 * param base FLEXRAM base address.
 */
void FLEXRAM_Init(FLEXRAM_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate ENET clock. */
    CLOCK_EnableClock(s_flexramClocks[FLEXRAM_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* enable all the interrupt status */
    base->INT_STAT_EN |= (uint32_t)kFLEXRAM_InterruptStatusAll;
    /* clear all the interrupt status */
    base->INT_STATUS |= (uint32_t)kFLEXRAM_InterruptStatusAll;
    /* disable all the interrpt */
    base->INT_SIG_EN = 0U;
}

/*!
 * brief Deinitializes the FLEXRAM.
 *
 */
void FLEXRAN_Deinit(FLEXRAM_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate ENET clock. */
    CLOCK_DisableClock(s_flexramClocks[FLEXRAM_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}
