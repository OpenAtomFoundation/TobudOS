/*!
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 *
 * ile
 *
 * This is the source file for the Panic module.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "fsl_common.h"
#include "panic.h"
#if (defined(PANIC_ENABLE_LOG) && (PANIC_ENABLE_LOG > 0U))
#include "fsl_debug_console.h"
#endif

/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */
static panic_data_t panic_data;
/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
#if defined(__IAR_SYSTEMS_ICC__)
/* __get_LR() declaration not included to avoid issues with different versions of IAR compiler */
#elif defined(__GNUC__)
#define __get_LR() __builtin_return_address(0)
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
#if (__ARMCC_VERSION >= 6010050)
#define __get_LR() __builtin_return_address(0)
#else
#define __get_LR() __return_address()
#endif
#endif
/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
 * \brief  This function will halt the system
 *
 * \param[in]  id Description of the param2 in parameter
 * \param[in]  location address where the Panic occurred
 * \param[in]  extra1 parameter to be stored in Panic structure
 * \param[in]  extra2 parameter to be stored in Panic structure
 *
 ********************************************************************************** */

void panic(panic_id_t id, uint32_t location, uint32_t extra1, uint32_t extra2)
{
    panic_data.id            = id;
    panic_data.location      = location;
    panic_data.extra1        = extra1;
    panic_data.extra2        = extra2;
    panic_data.linkRegister  = (uint32_t)((uint32_t *)__get_LR());
    panic_data.cpsr_contents = 0;
#if (defined(PANIC_ENABLE_LOG) && (PANIC_ENABLE_LOG > 0U))
    (void)PRINTF("System Panic happen: \r\n");
    (void)PRINTF("Panic id: 0x%x \r\n", id);
    (void)PRINTF("Panic location: 0x%x \r\n", location);
    (void)PRINTF("Panic extra1: 0x%x \r\n", extra1);
    (void)PRINTF("Panic extra2: 0x%x \r\n", extra2);
    (void)PRINTF("Panic linkRegister: 0x%x \r\n", panic_data.linkRegister);
#endif

    (void)DisableGlobalIRQ(); /* disable interrupts */

    /* infinite loop just to ensure this routine never returns */
    for (;;)
    {
    }
}
