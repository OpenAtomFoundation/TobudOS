/**
 * @file   freertos_thread.c
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */
 
#include "./FreeRTOS_Adapter/freertos_thread.h"
#include "./FreeRTOS_Adapter/freertos_debug.h"


/**
 * @brief Create and start a thread
 *
 * This function starts a new thread. The new thread starts execution by
 * invoking entry(). The argument arg is passed as the sole argument of entry().
 *
 * @note After finishing execution, the new thread should call OS_ThreadDelete()
 *       to delete itself. Failing to do this and just returning from entry()
 *       will result in undefined behavior.
 *
 * @param[in] thread Pointer to the thread object
 * @param[in] name A descriptive name for the thread. This is mainly used to
 *                 facilitate debugging.
 * @param[in] entry Entry, which is a function pointer, to the thread function
 * @param[in] arg The sole argument passed to entry()
 * @param[in] priority The priority at which the thread will execute
 * @param[in] stackSize The number of bytes the thread stack can hold
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_ThreadCreate(OS_Thread_t *thread, const char *name,
                          OS_ThreadEntry_t entry, void *arg,
                          OS_Priority priority, uint32_t stackSize)
{
	BaseType_t ret;

	OS_HANDLE_ASSERT(!OS_ThreadIsValid(thread), thread->handle);

	ret = xTaskCreate(entry, name, stackSize / sizeof(StackType_t), arg, priority, &thread->handle);
	if (ret != pdPASS) {
		OS_ERR("err %"OS_BASETYPE_F"\r\n", ret);
		OS_ThreadSetInvalid(thread);
		return OS_FAIL;
	}
	return OS_OK;
}

/**
 * @brief Terminate the thread
 * @note Only memory that is allocated to a thread by the kernel itself is
 *       automatically freed when a thread is deleted. Memory, or any other
 *       resource, that the application (rather than the kernel) allocates
 *       to a thread must be explicitly freed by the application when the task
 *       is deleted.
 * @param[in] thread Pointer to the thread object to be deleted.
 *                   A thread can delete itself by passing NULL in place of a
 *                   valid thread object.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_ThreadDelete(OS_Thread_t *thread)
{
	TaskHandle_t handle;
	TaskHandle_t curHandle;

	if (thread == NULL) {
		vTaskDelete(NULL); /* delete self */
		return OS_OK;
	}

	OS_HANDLE_ASSERT(OS_ThreadIsValid(thread), thread->handle);

	handle = thread->handle;
	curHandle = xTaskGetCurrentTaskHandle();
	if (handle == curHandle) 
	{
		/* delete self */
		OS_ThreadSetInvalid(thread);
		vTaskDelete(NULL);
	} 
	else 
	{
		/* delete other thread */
		OS_WARN("thread %"OS_HANDLE_F" delete %"OS_HANDLE_F"\r\n", curHandle, handle);
		vTaskDelete(handle);
		OS_ThreadSetInvalid(thread);
	}

	return OS_OK;
}

#if INCLUDE_uxTaskGetStackHighWaterMark
/**
 * @brief Get the minimum amount of free stack space that has been available
 *        since the thread started executing.
 * @param[in] thread Pointer to the thread object
 * @return The minimum amount of free stack space that has been available since
 *         the thread started executing. This is the amount of stack that
 *         remained unused when stack usage was at its greatest (or deepest)
 *         value.
 */
uint32_t OS_ThreadGetStackMinFreeSize(OS_Thread_t *thread)
{
	TaskHandle_t handle;

	handle =  thread ? thread->handle : NULL;
	return (uxTaskGetStackHighWaterMark(handle) * sizeof(StackType_t));
}
#endif

