/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if !defined(__FSL_OS_ABSTRACTION_BM_H__)
#define __FSL_OS_ABSTRACTION_BM_H__

/*!
 * @addtogroup os_abstraction_bm
 * @{
 */

/*******************************************************************************
 * Declarations
 ******************************************************************************/
/*! @brief Bare Metal does not use timer. */
#define FSL_OSA_BM_TIMER_NONE 0U

/*! @brief Bare Metal uses SYSTICK as timer. */
#define FSL_OSA_BM_TIMER_SYSTICK 1U

/*! @brief Configure what timer is used in Bare Metal. */
#ifndef FSL_OSA_BM_TIMER_CONFIG
#define FSL_OSA_BM_TIMER_CONFIG FSL_OSA_BM_TIMER_NONE
#endif

/*! @brief Type for task parameter */
typedef void *task_param_t;

/*! @brief Type for an event flags group, bit 32 is reserved */
typedef uint32_t event_flags_t;

/*! @brief Constant to pass as timeout value in order to wait indefinitely. */
#define OSA_WAIT_FOREVER 0xFFFFFFFFU

/*! @brief How many tasks can the bare metal support. */
#define TASK_MAX_NUM 7

/*! @brief OSA's time range in millisecond, OSA time wraps if exceeds this value. */
#define FSL_OSA_TIME_RANGE 0xFFFFFFFFU

/*! @brief The default interrupt handler installed in vector table. */
#define OSA_DEFAULT_INT_HANDLER ((osa_int_handler_t)(&DefaultISR))

/*! @brief The default interrupt handler installed in vector table. */
extern void DefaultISR(void);

/*!
 * @name Thread management
 * @{
 */

/*!
 * @brief Defines a task.
 *
 * This macro defines resources for a task statically. Then, the OSA_TaskCreate
 * creates the task based-on these resources.
 *
 * @param task The task function.
 * @param stackSize The stack size this task needs in bytes.
 */

#define PRIORITY_OSA_TO_RTOS(osa_prio) (osa_prio)
#define PRIORITY_RTOS_TO_OSA(rtos_prio) (rtos_prio)

/*! @}*/
/*! @}*/
#endif /* __FSL_OS_ABSTRACTION_BM_H__ */
/*******************************************************************************
 * EOF
 ******************************************************************************/
