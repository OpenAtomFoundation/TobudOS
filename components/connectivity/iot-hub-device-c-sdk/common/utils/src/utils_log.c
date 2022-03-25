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
 * @file utils_log.c
 * @brief different level log generator
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

#include "utils_log.h"

static const char   *LEVEL_STR[] = {"DIS", "ERR", "WRN", "INF", "DBG"};
static LogHandleFunc sg_log_handle_func;
static char         *sg_log_buffer;
static int           sg_log_max_size;
static void         *sg_log_mutex;
static LogLevel      sg_log_print_level = LOG_LEVEL_DEBUG;
extern LogLevel      sg_log_upload_level;

/**
 * @brief Get file name form path.
 *
 * @param[in] path file path
 * @return file name
 */
static const char *_get_filename(const char *path)
{
#ifdef WIN32
    char ch = '\\';
#else
    char ch = '/';
#endif
    const char *q = strrchr(path, ch);
    if (!q) {
        q = path;
    } else {
        q++;
    }
    return q;
}

/**
 * @brief Init log with func, log level, max log size.
 *
 * @param[in] func function should be implement for utils log
 * @param[in] log_level @see LogLevel
 * @param[in] max_log_size max size of log to print
 * @return 0 for success
 */
int utils_log_init(LogHandleFunc func, LogLevel log_level, int max_log_size)
{
    sg_log_handle_func = func;
    sg_log_print_level = log_level;
    sg_log_max_size    = max_log_size;
    if (func.log_mutex_create) {
        sg_log_mutex = func.log_mutex_create();
        if (!sg_log_mutex) {
            return -1;
        }
    }

    if (func.log_malloc) {
        sg_log_buffer = func.log_malloc(max_log_size);
        if (!sg_log_buffer) {
            if (sg_log_mutex) {
                func.log_mutex_destroy(sg_log_mutex);
            }
        }
    }
    return !sg_log_buffer;
}

/**
 * @brief Deinit log.
 *
 */
void utils_log_deinit(void)
{
    sg_log_handle_func.log_free(sg_log_buffer);
    if (sg_log_mutex) {
        sg_log_handle_func.log_mutex_destroy(sg_log_mutex);
    }
    sg_log_mutex  = NULL;
    sg_log_buffer = NULL;
}

/**
 * @brief Set log level.
 *
 * @param log_level @see LogLevel
 */
void utils_log_set_level(LogLevel log_level)
{
    sg_log_print_level = log_level;
}

/**
 * @brief Get log level.
 *
 * @return @see LogLevel
 */
LogLevel utils_log_get_level(void)
{
    return sg_log_print_level;
}

/**
 * @brief Generate log if level higher than set.
 *
 * @param[in] file file path
 * @param[in] func function where generate log
 * @param[in] line line of source file where genertate log
 * @param[in] level @see LogLevel
 * @param[in] fmt format of log content
 */
void utils_log_gen(const char *file, const char *func, const int line, const int level, const char *fmt, ...)
{
    if (level > sg_log_print_level) {
        return;
    }

    if (sg_log_mutex) {
        sg_log_handle_func.log_mutex_lock(sg_log_mutex);
    }

    /* format log content */
    const char *file_name = _get_filename(file);
    char       *o         = sg_log_buffer;

    memset(sg_log_buffer, 0, sg_log_max_size);

    o += snprintf(sg_log_buffer, sg_log_max_size, "%s|%s|%s|%s(%d): ", LEVEL_STR[level],
                  sg_log_handle_func.log_get_current_time_str(), file_name, func, line);

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(o, sg_log_max_size - 2 - strlen(sg_log_buffer), fmt, ap);
    va_end(ap);

    strncat(sg_log_buffer, "\r\n", sg_log_max_size - strlen(sg_log_buffer) - 1);

    if (level <= sg_log_print_level) {
        if (sg_log_handle_func.log_handle) {
            sg_log_handle_func.log_handle(sg_log_buffer);
        }
        sg_log_handle_func.log_printf("%s", sg_log_buffer);
    }

    /* append to upload buffer */
    if (sg_log_handle_func.log_upload) {
        sg_log_handle_func.log_upload(level, sg_log_buffer);
    }

    if (sg_log_mutex) {
        sg_log_handle_func.log_mutex_unlock(sg_log_mutex);
    }

    return;
}
