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
 * @file utils_log.h
 * @brief header file for utils log
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_LOG_H_
#define IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief SDK log print/upload level.
 *
 */
typedef enum {
    LOG_LEVEL_DISABLE = 0, /**< disable log print/upload */
    LOG_LEVEL_ERROR   = 1, /**< error log level */
    LOG_LEVEL_WARN    = 2, /**< warning log level */
    LOG_LEVEL_INFO    = 3, /**< info log level */
    LOG_LEVEL_DEBUG   = 4, /**< debug log level */
} LogLevel;

/**
 * @brief User's self defined log handler callback.
 *
 */
typedef struct {
    void *(*log_malloc)(size_t len);
    void (*log_free)(void *val);
    void (*log_handle)(const char *message);
    void (*log_upload)(LogLevel log_level, const char *message);
    void (*log_printf)(const char *fmt, ...);
    char *(*log_get_current_time_str)(void);
    void *(*log_mutex_create)(void);
    void (*log_mutex_lock)(void *mutex);
    void (*log_mutex_unlock)(void *mutex);
    void (*log_mutex_destroy)(void *mutex);
} LogHandleFunc;

/**
 * @brief Default log func
 *
 */
#define DEFAULT_LOG_HANDLE_FUNCS                                                                         \
    {                                                                                                    \
        HAL_Malloc, HAL_Free, NULL, NULL, HAL_Printf, HAL_Timer_Current, HAL_MutexCreate, HAL_MutexLock, \
            HAL_MutexUnlock, HAL_MutexDestroy                                                            \
    }

/**
 * @brief Init log with func, log level, max log size.
 *
 * @param[in] func function should be implement for utils log
 * @param[in] log_level @see LogLevel
 * @param[in] max_log_size max size of log to print
 * @return 0 for success
 */
int utils_log_init(LogHandleFunc func, LogLevel log_level, int max_log_size);

/**
 * @brief Deinit log.
 *
 */
void utils_log_deinit(void);

/**
 * @brief Set log level.
 *
 * @param log_level @see LogLevel
 */
void utils_log_set_level(LogLevel log_level);

/**
 * @brief Get log level.
 *
 * @return @see LogLevel
 */
LogLevel utils_log_get_level(void);

/**
 * @brief Generate log if level higher than set.
 *
 * @param[in] file file path
 * @param[in] func function where generate log
 * @param[in] line line of source file where genertate log
 * @param[in] level @see LogLevel
 * @param[in] fmt format of log content
 */
void utils_log_gen(const char *file, const char *func, const int line, const int level, const char *fmt, ...);

// Simple APIs for log generation in different level
#define Log_d(fmt, ...) utils_log_gen(__FILE__, __FUNCTION__, __LINE__, LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define Log_i(fmt, ...) utils_log_gen(__FILE__, __FUNCTION__, __LINE__, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define Log_w(fmt, ...) utils_log_gen(__FILE__, __FUNCTION__, __LINE__, LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define Log_e(fmt, ...) utils_log_gen(__FILE__, __FUNCTION__, __LINE__, LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_LOG_H_
