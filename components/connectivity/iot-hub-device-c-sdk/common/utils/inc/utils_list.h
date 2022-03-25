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
 * @file utils_list.h
 * @brief header file for utils list
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-25
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-25 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_LIST_H_
#define IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

/**
 * @brief ListNode iterator direction.
 *
 */
typedef enum {
    LIST_HEAD,
    LIST_TAIL,
} UtilsListDirection;

/**
 * @brief ListNode process result of OnNodeProcessHandle.
 *
 */
typedef enum {
    LIST_TRAVERSE_CONTINUE,
    LIST_TRAVERSE_BREAK,
} UtilsListResult;

/**
 * @brief Utils list function.
 *
 */
typedef struct {
    void *(*list_malloc)(size_t len);
    void (*list_free)(void *val);

    void *(*list_lock_init)(void);
    void (*list_lock)(void *lock);
    void (*list_unlock)(void *lock);
    void (*list_lock_deinit)(void *lock);
} UtilsListFunc;

/**
 * @brief Node process handle called by utils_list_process.
 *
 */
typedef UtilsListResult (*OnNodeProcessHandle)(void *list, void *node, void *val, void *usr_data);

/**
 * @brief Create list with max len, return NULL if fail.
 *
 * @param[in] func function needed by list
 * @param[in] max_len max_len of list
 * @return pointer to list, NULL for failed
 */
void *utils_list_create(UtilsListFunc func, int max_len);

/**
 * @brief Destroy list.
 *
 * @param[in] list pointer to list
 */
void utils_list_destroy(void *list);

/**
 * @brief Get list len.
 *
 * @param[in] list pointer to list
 * @return len of list
 */
int utils_list_len_get(void *list);

/**
 * @brief Push the node to list tail, return NULL if node invalid.
 *
 * @param[in] list pointer to list
 * @param[in] val value needed to push to list
 * @return pointer to node, NULL for failed
 */
void *utils_list_push(void *list, void *val);

/**
 * @brief Pop the val from list head, return NULL if list empty.
 *
 * @param[in] list pointer to list
 * @return val in the head node
 */
void *utils_list_pop(void *list);

/**
 * @brief Delete the node in list and release the resource.
 *
 * @param[in] list pointer to list
 * @param[in] node pointer to node needed remove
 */
void utils_list_remove(void *list, void *node);

/**
 * @brief Process list using handle function.
 *
 * @param[in] list pointer to list
 * @param[in] direction direction to traverse
 * @param[in] handle process function @see OnNodeProcessHandle
 * @param[in,out] usr_data usr data to pass to OnNodeProcessHandle
 */
void utils_list_process(void *list, uint8_t direction, OnNodeProcessHandle handle, void *usr_data);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_LIST_H_
