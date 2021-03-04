/**
 * @file TencentOS_queue.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_TENCENT_OS_QUEUE_H_
#define _KERNEL_OS_TENCENT_OS_QUEUE_H_

#include "./TencentOS_Adapter/TencentOS_common.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef k_msg_q_t * OS_QueueHandle;
    
typedef struct OS_Queue {
    k_mail_q_t * handle;
} OS_Queue_t;

typedef struct OS_MsgQueue {
    k_msg_q_t * handle;
} OS_MsgQueue_t;

OS_Status OS_QueueCreate(OS_Queue_t *queue, uint32_t queueLen, uint32_t itemSize);
OS_Status OS_QueueDelete(OS_Queue_t *queue);
OS_Status OS_QueueSend(OS_Queue_t *queue, const void *item, OS_Time_t waitMS);
OS_Status OS_QueueReceive(OS_Queue_t *queue, void *item, OS_Time_t waitMS);


OS_Status OS_MsgQueueCreate(OS_MsgQueue_t *queue, uint32_t queueLen);
OS_Status OS_MsgQueueDelete(OS_MsgQueue_t *queue);
OS_Status OS_MsgQueueSend(OS_MsgQueue_t *queue, void *msg, OS_Time_t waitMS);
OS_Status OS_MsgQueueReceive(OS_MsgQueue_t *queue, void **msg, OS_Time_t waitMS);


static __always_inline int OS_MsgQueueIsValid(OS_MsgQueue_t *queue)
{
	return ((queue->handle != NULL) && (queue->handle->knl_obj.type == KNL_OBJ_TYPE_MESSAGE_QUEUE));
}

static __always_inline void OS_MsgQueueSetInvalid(OS_MsgQueue_t *queue)
{
	queue->handle->knl_obj.type = KNL_OBJ_TYPE_NONE;
}



#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_TENCENT_OS_QUEUE_H_ */
