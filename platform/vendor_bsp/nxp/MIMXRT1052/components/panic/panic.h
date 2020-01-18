/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 *
 *
 * This is the header file for the Panic module.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PANIC_H__
#define __PANIC_H__

/*!
 * @addtogroup Panic
 * @{
 */

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */

/**@brief panic id. */
typedef uint32_t panic_id_t;

/**@brief panic data structure. */
typedef struct _panic_data
{
    panic_id_t id;          /* Panic ID */
    uint32_t location;      /* location address where the Panic occurred */
    uint32_t extra1;        /* extra1 */
    uint32_t extra2;        /* extra2 */
    uint32_t linkRegister;  /* Link register */
    uint32_t cpsr_contents; /* cpsr_contents*/
    uint8_t stack_dump[4];  /* initially just contain the contents of the LR */
} panic_data_t;

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */
#define PANIC_ENABLE_LOG (1)

#define ID_PANIC(grp, value) ((panic_id_t)(((panic_id_t)(grp) << 16) + ((panic_id_t)(value))))

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */

/*!
 * @brief Panic function.
 *
 *
 * @param id                         Panic ID
 * @param location                   location address where the Panic occurred
 * @param extra1                     extra1 parameter to be stored in Panic structure.
 * @param extra1                     extra2 parameter to be stored in Panic structure
 * @retval No return vaule.
 */
void panic(panic_id_t id, uint32_t location, uint32_t extra1, uint32_t extra2);
/*! @}*/
#endif /* __PANIC_H__ */
