/**
 * @file freertos_queue.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_FREERTOS_OS_QUEUE_H_
#define _KERNEL_OS_FREERTOS_OS_QUEUE_H_

#include "./FreeRTOS_Adapter/freertos_common.h"
#include "os_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Queue object definition
 */
typedef struct OS_Queue {
    QueueHandle_t   handle;
} OS_Queue_t;

OS_Status OS_QueueCreate(OS_Queue_t *queue, uint32_t queueLen, uint32_t itemSize);
OS_Status OS_QueueDelete(OS_Queue_t *queue);
OS_Status OS_QueueSend(OS_Queue_t *queue, const void *item, OS_Time_t waitMS);
OS_Status OS_QueueReceive(OS_Queue_t *queue, void *item, OS_Time_t waitMS);


/**
 * @brief Check whether the queue object is valid or not
 * @param[in] queue Pointer to the queue object
 * @return 1 on valid, 0 on invalid
 */
__STATIC_INLINE int OS_QueueIsValid(OS_Queue_t *queue)
{
	return (queue->handle != OS_INVALID_HANDLE);
}

/**
 * @brief Set the queue object to invalid state
 * @param[in] queue Pointer to the queue object
 * @return None
 */
__STATIC_INLINE void OS_QueueSetInvalid(OS_Queue_t *queue)
{
	queue->handle = OS_INVALID_HANDLE;
}

/**
 * @brief Create and initialize a message queue object
 * @note A message queue is a queue with each data item can store a pointer.
 *       The size of each data item (message) is equal to sizeof(void *).
 * @param[in] queue Pointer to the message queue object
 * @param[in] queueLen The maximum number of items that the message queue can
 *                     hold at any one time.
 * @retval OS_Status, OS_OK on success
 */
__STATIC_INLINE OS_Status OS_MsgQueueCreate(OS_Queue_t *queue, uint32_t queueLen)
{
	return OS_QueueCreate(queue, queueLen, sizeof(void *));
}

/**
 * @brief Delete the message queue object
 * @param[in] queue Pointer to the message queue object
 * @retval OS_Status, OS_OK on success
 */
__STATIC_INLINE OS_Status OS_MsgQueueDelete(OS_Queue_t *queue)
{
	return OS_QueueDelete(queue);
}

/**
 * @brief Send (write) a message to the back of the message queue
 * @param[in] queue Pointer to the message queue object
 * @param[in] msg A message, which is a pointer, to be copied into the queue
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for space to become available on the
 *                   message queue, should the message queue already be full.
 *                   HAL_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
__STATIC_INLINE OS_Status OS_MsgQueueSend(OS_Queue_t *queue, void *msg, OS_Time_t waitMS)
{
	return OS_QueueSend(queue, &msg, waitMS);
}

/**
 * @brief Receive (read) a message from the message queue
 * @param[in] queue Pointer to the message queue object
 * @param[in] msg Pointer to the message buffer into which the received message
 *                will be copied. A message is a pointer.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for message to become available on
 *                   the message queue, should the message queue already be
 *                   empty.
 *                   HAL_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
__STATIC_INLINE OS_Status OS_MsgQueueReceive(OS_Queue_t *queue, void **msg, OS_Time_t waitMS)
{
	return OS_QueueReceive(queue, msg, waitMS);
}

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_FREERTOS_OS_QUEUE_H_ */
