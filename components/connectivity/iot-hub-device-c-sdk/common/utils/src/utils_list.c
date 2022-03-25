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
 * @file utils_list.c
 * @brief utils list operation
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-25
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "utils_list.h"

/**
 * @brief Define list node.
 *
 */
typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
    void *           val;
} ListNode;

/**
 * @brief  Double Linked List.
 *
 */
typedef struct {
    UtilsListFunc func;
    ListNode *    head;
    ListNode *    tail;
    void *        lock;
    int           len;
    int           max_len;
} List;

/**
 * @brief List iterator.
 *
 */
typedef struct {
    List *             list;
    ListNode *         next;
    UtilsListDirection direction;
} ListIterator;

/**
 * @brief Lock list.
 *
 * @param[in] list pointer to list
 */
static inline void _list_lock(List *list)
{
    if (list->lock) {
        list->func.list_lock(list->lock);
    }
}

/**
 * @brief Unlock list.
 *
 * @param[in] list pointer to list
 */
static inline void _list_unlock(List *list)
{
    if (list->lock) {
        list->func.list_unlock(list->lock);
    }
}

/**
 * @brief Delete the node in list and release the resource.
 *
 * @param[in] list pointer to list
 * @param[in] node pointer to node needed remove
 */
static void _list_remove(void *list, void *node)
{
    List *    self      = (List *)list;
    ListNode *list_node = (ListNode *)node;

    list_node->prev ? (list_node->prev->next = list_node->next) : (self->head = list_node->next);

    list_node->next ? (list_node->next->prev = list_node->prev) : (self->tail = list_node->prev);

    self->func.list_free(list_node->val);
    self->func.list_free(list_node);

    if (self->len) {
        --self->len;
    }
}

/**
 * @brief Create list with max len, return NULL if fail.
 *
 * @param[in] func function needed by list
 * @param[in] max_len max_len of list
 * @return pointer to list, NULL for failed
 */
void *utils_list_create(UtilsListFunc func, int max_len)
{
    List *self;

    if (max_len <= 0) {
        return NULL;
    }

    self = (List *)func.list_malloc(sizeof(List));
    if (!self) {
        return NULL;
    }

    memset(self, 0, sizeof(List));

    if (func.list_lock_init) {
        self->lock = func.list_lock_init();
        if (!self->lock) {
            func.list_free(self);
            return NULL;
        }
    }

    self->func    = func;
    self->max_len = max_len;
    return self;
}

/**
 * @brief Destroy list.
 *
 * @param[in] list pointer to list
 */
void utils_list_destroy(void *list)
{
    if (!list) {
        return;
    }

    List *self = (List *)list;

    uint32_t len = self->len;

    ListNode *next;
    ListNode *curr = self->head;

    while (len--) {
        next = curr->next;
        self->func.list_free(curr->val);
        self->func.list_free(curr);
        curr = next;
    }

    if (self->lock) {
        self->func.list_lock_deinit(self->lock);
    }
    self->func.list_free(self);
}

/**
 * @brief Get list len.
 *
 * @param[in] list pointer to list
 * @return len of list
 */
int utils_list_len_get(void *list)
{
    List *self = (List *)list;
    return self->len;
}

/**
 * @brief Push the node to list tail, return NULL if node invalid.
 *
 * @param[in] list pointer to list
 * @param[in] val value needed to push to list
 * @return pointer to node, NULL for failed
 */
void *utils_list_push(void *list, void *val)
{
    List *self = (List *)list;

    _list_lock(self);

    if (!val || self->len >= self->max_len) {
        _list_unlock(self);
        return NULL;
    }

    ListNode *node;
    node = self->func.list_malloc(sizeof(ListNode));
    if (!node) {
        _list_unlock(self);
        return NULL;
    }

    node->prev = NULL;
    node->next = NULL;
    node->val  = val;

    if (self->len) {
        node->prev       = self->tail;
        node->next       = NULL;
        self->tail->next = node;
        self->tail       = node;
    } else {
        self->head = self->tail = node;
        node->prev = node->next = NULL;
    }

    ++self->len;

    _list_unlock(self);
    return node;
}

/**
 * @brief Pop the value from list head, return NULL if list empty.
 *
 * @param[in] list pointer to list
 * @return value in the head node
 */
void *utils_list_pop(void *list)
{
    List *    self = (List *)list;
    ListNode *node = NULL;

    _list_lock(self);

    if (!self->len) {
        _list_unlock(self);
        return NULL;
    }

    node = self->head;

    if (--self->len) {
        (self->head = node->next)->prev = NULL;
    } else {
        self->head = self->tail = NULL;
    }

    node->next = node->prev = NULL;

    _list_unlock(self);

    void *val = node->val;
    self->func.list_free(node);
    return val;
}

/**
 * @brief Delete the node in list and release the resource.
 *
 * @param[in] list pointer to list
 * @param[in] node pointer to node needed remove
 */
void utils_list_remove(void *list, void *node)
{
    List *self = (List *)list;
    _list_lock(self);
    _list_remove(self, node);
    _list_unlock(self);
}

/**
 * @brief Process list using handle function.
 *
 * @param[in] list pointer to list
 * @param[in] direction direction to traverse
 * @param[in] handle process function @see OnNodeProcessHandle
 * @param[in,out] usr_data usr data to pass to OnNodeProcessHandle
 */
void utils_list_process(void *list, uint8_t direction, OnNodeProcessHandle handle, void *usr_data)
{
    int       rc;
    ListNode *node = NULL;
    List *    self = (List *)list;

    _list_lock(self);

    if (!utils_list_len_get(list)) {
        _list_unlock(self);
        return;
    }

    ListIterator iterator = {
        .direction = direction,
        .list      = self,
        .next      = direction == LIST_HEAD ? self->head : self->tail,
    };

    // traverse list to process
    while ((node = iterator.next)) {
        iterator.next = iterator.direction == LIST_HEAD ? node->next : node->prev;
        if (!node->val) {
            _list_remove(list, node);
            continue;
        }
        // process node and val
        if (handle) {
            rc = handle(list, node, node->val, usr_data);
            if (rc) {
                break;
            }
        }
    }
    _list_unlock(list);
}
