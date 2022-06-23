/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file qcloud_iot_platform.h
 * @brief hal interface
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-09 <td>1.1     <td>fancyxu   <td>support tls and change port to str format
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_PLATFORM_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_PLATFORM_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "qcloud_iot_device.h"
#include "qcloud_iot_common.h"

/**********************************************************************
 * QCloud IoT C-SDK Hardware Abstraction Layer
 * Platform/OS/IP stack/SSL dependant functions
 * Check platform folder for reference implementation
 * Require porting when adapt SDK to new platform/OS
 *********************************************************************/

/**************************************************************************************
 * os
 **************************************************************************************/

/**
 * @brief Mutex create.
 *
 * @return pointer to mutex
 */
void *HAL_MutexCreate(void);

/**
 * @brief Mutex destroy.
 *
 * @param[in,out] mutex pointer to mutex
 */
void HAL_MutexDestroy(void *mutex);

/**
 * @brief Mutex lock.
 *
 * @param[in,out] mutex pointer to mutex
 */
void HAL_MutexLock(void *mutex);

/**
 * @brief Mutex try lock.
 *
 * @param[in,out] mutex pointer to mutex
 * @return 0 for success
 */
int HAL_MutexTryLock(void *mutex);

/**
 * @brief Mutex unlock.
 *
 * @param[in,out] mutex pointer to mutex
 */
void HAL_MutexUnlock(void *mutex);

/**
 * @brief Malloc from heap.
 *
 * @param[in] size size to malloc
 * @return pointer to buffer, NULL for failed.
 */
void *HAL_Malloc(size_t size);

/**
 * @brief Free buffer malloced by HAL_Malloc.
 *
 * @param[in] ptr
 */
void HAL_Free(void *ptr);

/**
 * @brief Printf with format.
 *
 * @param[in] fmt format
 */
void HAL_Printf(const char *fmt, ...);

/**
 * @brief Snprintf with format.
 *
 * @param[out] str buffer to save
 * @param[in] len buffer len
 * @param[in] fmt format
 * @return length of formatted string, >0 for success.
 */
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

/**
 * @brief Sleep for ms.
 *
 * @param[in] ms ms to sleep
 */
void HAL_SleepMs(uint32_t ms);

#ifdef MULTITHREAD_ENABLED

/**
 * @brief Theard entry function.
 *
 */
typedef void (*ThreadRunFunc)(void *arg);

/**
 * @brief Thread priority.
 *
 */
typedef enum {
    THREAD_PRIORITY_HIGH,
    THREAD_PRIORITY_MIDDLE,
    THREAD_PRIORITY_LOW,
} ThreadPriority;

/**
 * @brief Thread params to create.
 *
 */
typedef struct {
    char          *thread_name; /**< thread name */
    uint64_t       thread_id;   /**< thread handle */
    ThreadRunFunc  thread_func; /**< thread entry function */
    void          *user_arg;    /**< thread entry arg */
    ThreadPriority priority;    /**< thread priority */
    void          *stack_base;  /**< thread stack base */
    uint32_t       stack_size;  /**< thread stack size */
} ThreadParams;

/**
 * @brief platform-dependant thread create function
 *
 * @param[in,out] params params to create thread @see ThreadParams
 * @return @see IotReturnCode
 */
int HAL_ThreadCreate(ThreadParams *params);

/**
 * @brief platform-dependent thread destroy function.
 *
 */
void HAL_ThreadDestroy(void *thread_id);

/**
 * @brief platform-dependent semaphore create function.
 *
 * @return pointer to semaphore
 */
void *HAL_SemaphoreCreate(void);

/**
 * @brief platform-dependent semaphore destory function.
 *
 * @param[in] sem pointer to semaphore
 */
void HAL_SemaphoreDestroy(void *sem);

/**
 * @brief platform-dependent semaphore post function.
 *
 * @param[in] sem pointer to semaphore
 */
void HAL_SemaphorePost(void *sem);

/**
 * @brief platform-dependent semaphore wait function.
 *
 * @param[in] sem pointer to semaphore
 * @param[in] timeout_ms wait timeout
 * @return @see IotReturnCode
 */
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);

/**
 * @brief platform-dependent mail queue init function.
 *
 * @param[in] pool pool using in mail queue
 * @param[in] mail_size mail size
 * @param[in] mail_count mail count
 * @return pointer to mail queue
 */
void *HAL_MailQueueInit(void *pool, size_t mail_size, int mail_count);

/**
 * @brief platform-dependent mail queue deinit function.
 *
 * @param[in] mail_q pointer to mail queue
 */
void HAL_MailQueueDeinit(void *mail_q);

/**
 * @brief platform-dependent mail queue send function.
 *
 * @param[in] mail_q pointer to mail queue
 * @param[in] buf data buf
 * @param[in] size data size
 * @return 0 for success
 */
int HAL_MailQueueSend(void *mail_q, const void *buf, size_t size);

/**
 * @brief platform-dependent mail queue send function.
 *
 * @param[in] mail_q pointer to mail queue
 * @param[out] buf data buf
 * @param[in] size data size
 * @param[in] timeout_ms
 * @return 0 for success
 */
int HAL_MailQueueRecv(void *mail_q, void *buf, size_t *size, uint32_t timeout_ms);

#endif

/**
 * @brief Functions for saving file into NVS(files/FLASH)
 * @param[in] filename file path name
 * @param[in] buf source need write buffer
 * @param[in] write_len length of file to write
 * @return length of data save when success, or 0 for failure
 */
size_t HAL_File_Write(const char *filename, const void *buf, size_t write_len, size_t offset);

/**
 * @brief Functions for reading file from NVS(files/FLASH)
 * @param[in] filename file path name
 * @param[in] buf destination log buffer
 * @param[in] read_len length to read
 * @return length of data read when success, or 0 for failure
 */
size_t HAL_File_Read(const char *filename, void *buf, size_t read_len, size_t offset);

/**
 * @brief Functions for deleting file in NVS(files/FLASH).
 * @param[in] filename file path name
 * @return 0 when success
 */
int HAL_File_Del(const char *filename);

/**
 * @brief Functions for reading the size of file in NVS(files/FLASH).
 * @param[in] filename file path name
 * @return 0 when nothing exist
 */
size_t HAL_File_GetSize(const char *filename);

/**************************************************************************************
 * device info
 **************************************************************************************/

/**
 * @brief Save device info
 *
 * @param[in] device_info @see DeviceInfo
 * @return @see IotReturnCode
 */
int HAL_SetDevInfo(DeviceInfo *device_info);

/**
 * @brief Get device info
 *
 * @param[in] device_info @see DeviceInfo
 * @return @see IotReturnCode
 */
int HAL_GetDevInfo(DeviceInfo *device_info);

/**************************************************************************************
 * timer
 **************************************************************************************/

/**
 * @brief time format string
 *
 * @return time format string, such as "2021-05-31 15:58:46"
 */
char *HAL_Timer_Current(void);

/**
 * @brief Get utc time ms timestamp.
 *
 * @return timestamp
 */
uint64_t HAL_Timer_CurrentMs(void);

/**
 * @brief Set system time using second timestamp
 *
 * @param[in] timestamp_ms
 * @return 0 for success
 */
int HAL_Timer_SetSystimeMs(uint64_t timestamp_ms);

/**************************************************************************************
 * network tcp
 **************************************************************************************/

/**
 * @brief TCP connect in linux
 *
 * @param[in] host host to connect
 * @param[out] port port to connect
 * @return socket fd
 */
int HAL_TCP_Connect(const char *host, const char *port);

/**
 * @brief TCP disconnect
 *
 * @param[in] fd socket fd
 * @return 0 for success
 */
int HAL_TCP_Disconnect(int fd);

/**
 * @brief TCP write
 *
 * @param[in] fd socket fd
 * @param[in] buf buf to write
 * @param[in] len buf len
 * @param[in] timeout_ms timeout
 * @param[out] written_len data written length
 * @return @see IotReturnCode
 */
int HAL_TCP_Write(int fd, const uint8_t *data, uint32_t len, uint32_t timeout_ms, size_t *written_len);

/**
 * @brief TCP read.
 *
 * @param[in] fd socket fd
 * @param[out] buf buffer to save read data
 * @param[in] len buffer len
 * @param[in] timeout_ms timeout
 * @param[out] read_len length of data read
 * @return @see IotReturnCode
 */
int HAL_TCP_Read(int fd, uint8_t *data, uint32_t len, uint32_t timeout_ms, size_t *read_len);

/**************************************************************************************
 * AT module
 **************************************************************************************/

#ifdef AT_MODULE_ENABLE

/**
 * @brief Urc handler.
 *
 */
typedef void (*OnUrcHandler)(const char *data, size_t data_len);

/**
 * @brief Init at module.
 *
 * @return 0 for success
 */
int HAL_Module_Init(void);

/**
 * @brief Deinit at module.
 *
 */
void HAL_Module_Deinit(void);

/**
 * @brief Send at cmd to at module and wait for resp.
 *
 * @param[in] at_cmd at cmd
 * @param[in] at_expect expect resp
 * @param[in] timeout_ms wait timeout
 * @return 0 for success
 */
int HAL_Module_SendAtCmdWaitResp(const char *at_cmd, const char *at_expect, uint32_t timeout_ms);

/**
 * @brief Send at cmd and waif for data.
 *
 * @param[in] at_cmd at cmd
 * @param[in] at_expect expect resp
 * @param[out] recv_buf recv data buffer
 * @param[out] recv_len recv data length
 * @param[in] timeout_ms wait timeout
 * @return 0 for success
 */
int HAL_Module_SendAtCmdWaitRespWithData(const char *at_cmd, const char *at_expect, void *recv_buf, uint32_t *recv_len,
                                         uint32_t timeout_ms);

/**
 * @brief Send date to at module.
 *
 * @param[in] data data to send
 * @param[in] data_len data length
 * @return 0 for success
 */
int HAL_Module_SendAtData(const void *data, int data_len);

/**
 * @brief Set urc.
 *
 * @param[in] urc irc string
 * @param[in] urc_handler urc handler
 * @return 0 for success
 */
int HAL_Module_SetUrc(const char *urc, OnUrcHandler urc_handler);

/**
 * @brief connect network
 *
 * @return int 0 for success
 */
int HAL_Module_ConnectNetwork(void);

#endif

#if defined(__cplusplus)
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_PLATFORM_H_
