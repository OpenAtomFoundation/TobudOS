/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __COMMON_TASK_H__
#define __COMMON_TASK_H__

/*!
 * @addtogroup CommonTask
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if defined(OSA_USED)

/*! @brief Definition of common task max msg queue count. */
#define COMMON_TASK_MAX_MSGQ_COUNT (8U)

/*! @brief Definition of common task priority. */
#define COMMON_TASK_PRIORITY (9U)

/*! @brief Definition of common task stack size. */
#define COMMON_TASK_STACK_SIZE (2000U)

/*! @brief The status type of common task */
typedef enum _common_task_status
{
    kStatus_COMMON_TASK_Success = kStatus_Success,                          /*!< Success */
    kStatus_COMMON_TASK_Error   = MAKE_STATUS(kStatusGroup_COMMON_TASK, 1), /*!< Failed */
    kStatus_COMMON_TASK_Busy    = MAKE_STATUS(kStatusGroup_COMMON_TASK, 2), /*!< Busy */
} common_task_status_t;

/*! @brief The callback function of common task */
typedef void (*common_task_message_callback_t)(void *callbackParam);

/*! @brief The callback message struct of common task */
typedef struct _common_task_message
{
    common_task_message_callback_t callback;
    void *callbackParam;
} common_task_message_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/*!
 * @name Common task functional operation
 * @{
 */

/*!
 * @brief Initializes the common task module
 *
 * This function is used to initialize the common task module. The module is a delegation for other modules
 * without the self task. The common task will be created when the function is calling.
 * The task stack size is set by #COMMON_TASK_STACK_SIZE. And the task priority is defined by #COMMON_TASK_PRIORITY.
 * And a message queue is created with the length #COMMON_TASK_MAX_MSGQ_COUN by the function.
 *
 * This is an example.
 * @code
 *   COMMON_TASK_init();
 * @endcode
 * @retval kStatus_COMMON_TASK_Success The common task initialization succeed.
 * @retval kStatus_COMMON_TASK_Error An error occurred when the common task is initialized.
 */
common_task_status_t COMMON_TASK_init(void);

/*!
 * @brief De-initializes the common task module
 *
 * This function is used to de-initialize the common task module.
 *
 * @retval kStatus_COMMON_TASK_Success The common task de-initialization succeed.
 * @retval kStatus_COMMON_TASK_Error An error occurred when the common task is de-initialized.
 */
common_task_status_t COMMON_TASK_deinit(void);

/*!
 * @brief Posts a new message to common task
 *
 * This function is used to post a new message to common task. The message space cannot be released until
 * the message is executed.
 *
 * @param msg Pointer to point to a memory space of #common_task_message_t allocated by the caller.
 * @retval kStatus_COMMON_TASK_Success The common task de-initialization succeed.
 * @retval kStatus_COMMON_TASK_Error An error occurred when post a message to the common task.
 */
common_task_status_t COMMON_TASK_post_message(common_task_message_t *msg);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif

#endif
