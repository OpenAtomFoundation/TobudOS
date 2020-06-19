/**
 * @file freertos_queue.c
 * @author LightningSemi WLAN Team
 */
#include "./FreeRTOS_Adapter/freertos_queue.h"
#include "./FreeRTOS_Adapter/freertos_time.h"
#include "./FreeRTOS_Adapter/freertos_debug.h"


/**
 * @brief Create and initialize a queue object
 * @param[in] queue Pointer to the queue object
 * @param[in] queueLen The maximum number of items that the queue can hold at
 *                     any one time.
 * @param[in] itemSize The size, in bytes, of each data item that can be stored
 *                     in the queue.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueCreate(OS_Queue_t *queue, uint32_t queueLen, uint32_t itemSize)
{
//	OS_HANDLE_ASSERT(!OS_QueueIsValid(queue), queue->handle);

	queue->handle = xQueueCreate(queueLen, itemSize);
	if (queue->handle == NULL) {
		OS_ERR("err %"OS_HANDLE_F"\r\n", queue->handle);
		return OS_FAIL;
	}

	return OS_OK;
}

/**
 * @brief Delete the queue object
 * @param[in] queue Pointer to the queue object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueDelete(OS_Queue_t *queue)
{

	OS_HANDLE_ASSERT(OS_QueueIsValid(queue), queue->handle);

	vQueueDelete(queue->handle);
	OS_QueueSetInvalid(queue);
	return OS_OK;
}

/**
 * @brief Send (write) an item to the back of the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the data to be copied into the queue.
 *                 The size of each item the queue can hold is set when the
 *                 queue is created, and that many bytes will be copied from
 *                 item into the queue storage area.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for space to become available on the
 *                   queue, should the queue already be full.
 *                   HAL_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueSend(OS_Queue_t *queue, const void *item, OS_Time_t waitMS)
{
	BaseType_t ret;
	BaseType_t taskWoken;

	OS_HANDLE_ASSERT(OS_QueueIsValid(queue), queue->handle);

	if (OS_IsISRContext()) 
	{
		taskWoken = pdFALSE;
		ret = xQueueSendFromISR(queue->handle, item, &taskWoken);
		if (ret != pdPASS) {
			OS_DBG("%s() fail @ %d\r\n", __func__, __LINE__);
			return OS_FAIL;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} 
	else 
	{
		ret = xQueueSend(queue->handle, item, OS_CalcWaitTicks(waitMS));
		if (ret != pdPASS) {
			OS_DBG("%s() fail @ %d, %"OS_TIME_F" ms\r\n", __func__, __LINE__, waitMS);
			return OS_FAIL;
		}
	}

	return OS_OK;
}

/**
 * @brief Receive (read) an item from the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the memory into which the received data will be
 *                 copied. The length of the buffer must be at least equal to
 *                 the queue item size which is set when the queue is created.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for data to become available on the
 *                   queue, should the queue already be empty.
 *                   HAL_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueReceive(OS_Queue_t *queue, void *item, OS_Time_t waitMS)
{
	BaseType_t ret;
	BaseType_t taskWoken;

	OS_HANDLE_ASSERT(OS_QueueIsValid(queue), queue->handle);

	if (OS_IsISRContext()) 
	{
		taskWoken = pdFALSE;
		ret = xQueueReceiveFromISR(queue->handle, item, &taskWoken);
		if (ret != pdPASS) {
			OS_DBG("%s() fail @ %d\r\n", __func__, __LINE__);
			return OS_FAIL;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} 
	else 
	{
		ret = xQueueReceive(queue->handle, item, OS_CalcWaitTicks(waitMS));
		if (ret != pdPASS) {
			OS_DBG("%s() fail @ %d, %"OS_TIME_F" ms\r\n", __func__, __LINE__, waitMS);
			return OS_FAIL;
		}
	}

	return OS_OK;
}

