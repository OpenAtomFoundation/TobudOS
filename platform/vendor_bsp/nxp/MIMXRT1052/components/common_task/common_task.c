/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"

/*
 * The OSA_USED macro can only be defined when the OSA component is used.
 * If the source code of the OSA component does not exist, the OSA_USED cannot be defined.
 * OR, If OSA component is not added into project event the OSA source code exists, the OSA_USED
 * also cannot be defined.
 * The source code path of the OSA component is <MCUXpresso_SDK>/components/osa.
 *
 */
#if defined(OSA_USED)

#include "fsl_os_abstraction.h"

#include "common_task.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct _common_task_state
{
    uint8_t commonTaskHandle[OSA_TASK_HANDLE_SIZE];
    MSGQ_HANDLE_BUFFER_DEFINE(msgqhandle, COMMON_TASK_MAX_MSGQ_COUNT, sizeof(void *));
    uint8_t isInitialized;
} common_task_state_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void COMMON_TASK_task(osa_task_param_t param);

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern const uint8_t gUseRtos_c;
static common_task_state_t s_commonTaskState[1];
OSA_TASK_DEFINE(COMMON_TASK_task, COMMON_TASK_PRIORITY, 1, COMMON_TASK_STACK_SIZE, false);

/*******************************************************************************
 * Code
 ******************************************************************************/

void COMMON_TASK_task(osa_task_param_t param)
{
    common_task_state_t *commonTaskStateHandle = (common_task_state_t *)param;
    common_task_message_t *msg;
    do
    {
        if (KOSA_StatusSuccess ==
            OSA_MsgQGet((osa_msgq_handle_t)commonTaskStateHandle->msgqhandle, &msg, osaWaitForever_c))
        {
            if (msg->callback)
            {
                msg->callback(msg->callbackParam);
            }
        }
    } while (gUseRtos_c);
}

common_task_status_t COMMON_TASK_init(void)
{
    osa_status_t status;

    if (s_commonTaskState->isInitialized)
    {
        return kStatus_COMMON_TASK_Error;
    }
    s_commonTaskState->isInitialized = 1U;

    status =
        OSA_MsgQCreate((osa_msgq_handle_t)s_commonTaskState->msgqhandle, COMMON_TASK_MAX_MSGQ_COUNT, sizeof(void *));
    assert(KOSA_StatusSuccess == status);

    status = OSA_TaskCreate((osa_task_handle_t)s_commonTaskState->commonTaskHandle, OSA_TASK(COMMON_TASK_task),
                            s_commonTaskState);
    assert(KOSA_StatusSuccess == status);
    (void)status;

    return kStatus_COMMON_TASK_Success;
}

common_task_status_t COMMON_TASK_deinit(void)
{
    if (!s_commonTaskState->isInitialized)
    {
        return kStatus_COMMON_TASK_Error;
    }

    OSA_MsgQDestroy((osa_msgq_handle_t)s_commonTaskState->msgqhandle);
    OSA_TaskDestroy((osa_task_handle_t)s_commonTaskState->commonTaskHandle);
    s_commonTaskState->isInitialized = 0U;

    return kStatus_COMMON_TASK_Success;
}

common_task_status_t COMMON_TASK_post_message(common_task_message_t *msg)
{
    assert(msg);
    assert(msg->callback);
    assert(s_commonTaskState->isInitialized);

    if (KOSA_StatusSuccess != OSA_MsgQPut((osa_msgq_handle_t)s_commonTaskState->msgqhandle, &msg))
    {
        return kStatus_COMMON_TASK_Error;
    }
    return kStatus_COMMON_TASK_Success;
}
#endif
