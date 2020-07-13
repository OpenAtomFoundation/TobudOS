/**
 * @file   freertos_semaphore.c
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#include "./FreeRTOS_Adapter/freertos_semaphore.h"
#include "./FreeRTOS_Adapter/freertos_time.h"
#include "./FreeRTOS_Adapter/freertos_debug.h"


/**
 * @brief Create and initialize a counting semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @param[in] initCount The count value assigned to the semaphore when it is
 *                      created.
 * @param[in] maxCount The maximum count value that can be reached. When the
 *                     semaphore reaches this value it can no longer be
 *                     released.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_SemaphoreCreate(OS_Semaphore_t *sem, uint32_t initCount, uint32_t maxCount)
{
//	OS_HANDLE_ASSERT(!OS_SemaphoreIsValid(sem), sem->handle);

	sem->handle = xSemaphoreCreateCounting(maxCount, initCount);
	if (sem->handle == NULL) {
		OS_ERR("err %"OS_HANDLE_F"\r\n", sem->handle);
		return OS_FAIL;
	}

	return OS_OK;
}

/**
 * @brief Create and initialize a binary semaphore object
 * @note A binary semaphore is equal to a counting semaphore created by calling
         OS_SemaphoreCreate(sem, 0, 1).
 * @param[in] sem Pointer to the semaphore object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_SemaphoreCreateBinary(OS_Semaphore_t *sem)
{
//	OS_HANDLE_ASSERT(!OS_SemaphoreIsValid(sem), sem->handle);

	sem->handle = xSemaphoreCreateBinary();
	if (sem->handle == NULL) {
		OS_ERR("err %"OS_HANDLE_F"\r\n", sem->handle);
		return OS_FAIL;
	}

	return OS_OK;
}

/**
 * @brief Delete the semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_SemaphoreDelete(OS_Semaphore_t *sem)
{
	OS_HANDLE_ASSERT(OS_SemaphoreIsValid(sem), sem->handle);

	vSemaphoreDelete(sem->handle);
	OS_SemaphoreSetInvalid(sem);
	return OS_OK;
}

/**
 * @brief Wait until the semaphore object becomes available
 * @param[in] sem Pointer to the semaphore object
 * @param[in] waitMS The maximum amount of time (in millisecond) the thread
 *                   should remain in the blocked state to wait for the
 *                   semaphore to become available.
 *                   HAL_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_SemaphoreWait(OS_Semaphore_t *sem, OS_Time_t waitMS)
{
	BaseType_t ret;
	BaseType_t taskWoken;

	OS_HANDLE_ASSERT(OS_SemaphoreIsValid(sem), sem->handle);

	if (OS_IsISRContext()) 
	{
		if (waitMS != 0) {
			OS_ERR("%s() in ISR, wait %u ms\r\n", __func__, waitMS);
			return OS_E_ISR;
		}
		taskWoken = pdFALSE;
		ret = xSemaphoreTakeFromISR(sem->handle, &taskWoken);
		if (ret != pdPASS) {
			OS_DBG("%s() fail @ %d\r\n", __func__, __LINE__);
			return OS_E_TIMEOUT;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} 
	else 
	{
		ret = xSemaphoreTake(sem->handle, OS_CalcWaitTicks(waitMS));
		if (ret != pdPASS) {
			OS_DBG("%s() fail @ %d, %"OS_TIME_F" ms\r\n", __func__, __LINE__, waitMS);
			return OS_E_TIMEOUT;
		}
	}

	return OS_OK;
}

/**
 * @brief Release the semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_SemaphoreRelease(OS_Semaphore_t *sem)
{
	BaseType_t ret;
	BaseType_t taskWoken;

	OS_HANDLE_ASSERT(OS_SemaphoreIsValid(sem), sem->handle);

	if (OS_IsISRContext()) {
		taskWoken = pdFALSE;
		ret = xSemaphoreGiveFromISR(sem->handle, &taskWoken);
		if (ret != pdPASS) {
			OS_DBG("%s() fail @ %d\r\n", __func__, __LINE__);
			return OS_FAIL;
		}
		portEND_SWITCHING_ISR(taskWoken);
	} 
	else 
	{
		ret = xSemaphoreGive(sem->handle);
		if (ret != pdPASS) {
			OS_DBG("%s() fail @ %d\r\n", __func__, __LINE__);
			return OS_FAIL;
		}
	}

	return OS_OK;
}

UBaseType_t OS_SemaphoreGetCount(OS_Semaphore_t *sem)
{
    
    OS_HANDLE_ASSERT(OS_SemaphoreIsValid(sem), sem->handle);
    
    if (OS_IsISRContext()) {
		return uxSemaphoreGetCountFromISR(sem->handle);
	}else{
		return uxSemaphoreGetCount(sem->handle);
	}
}


