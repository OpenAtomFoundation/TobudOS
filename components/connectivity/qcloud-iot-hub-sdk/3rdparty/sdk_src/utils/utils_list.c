/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "utils_list.h"

#include "qcloud_iot_export_log.h"
#include "qcloud_iot_import.h"

/*
 * create list, return NULL if fail
 */
List *list_new(void)
{
    List *self;
    self = (List *)HAL_Malloc(sizeof(List));
    if (!self) {
        return NULL;
    }
    self->head  = NULL;
    self->tail  = NULL;
    self->free  = NULL;
    self->match = NULL;
    self->len   = 0;
    return self;
}

/*
 * destroy list
 */
void list_destroy(List *self)
{
    unsigned int len = self->len;
    ListNode *   next;
    ListNode *   curr = self->head;

    while (len--) {
        next = curr->next;
        if (self->free) {
            self->free(curr->val);
        }
        HAL_Free(curr);
        curr = next;
    }

    HAL_Free(self);
}

/*
 * push the node to list tail, return NULL if node invalid
 */
ListNode *list_rpush(List *self, ListNode *node)
{
    if (!node) {
        return NULL;
    }

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
    return node;
}

/*
 * pop the node from list tail, return NULL if list empty
 */
ListNode *list_rpop(List *self)
{
    ListNode *node = NULL;
    if (!self->len) {
        return NULL;
    }

    node = self->tail;

    if (--self->len) {
        (self->tail = node->prev)->next = NULL;
    } else {
        self->tail = self->head = NULL;
    }

    node->next = node->prev = NULL;
    return node;
}

/*
 * pop the node from list headl, return NULL if list empty
 */
ListNode *list_lpop(List *self)
{
    ListNode *node = NULL;
    if (!self->len) {
        return NULL;
    }

    node = self->head;

    if (--self->len) {
        (self->head = node->next)->prev = NULL;
    } else {
        self->head = self->tail = NULL;
    }

    node->next = node->prev = NULL;
    return node;
}

/*
 * push the node to list head, return NULL if node invalid
 */
ListNode *list_lpush(List *self, ListNode *node)
{
    if (!node) {
        return NULL;
    }

    if (self->len) {
        node->next       = self->head;
        node->prev       = NULL;
        self->head->prev = node;
        self->head       = node;
    } else {
        self->head = self->tail = node;
        node->prev = node->next = NULL;
    }

    ++self->len;
    return node;
}

/*
 * find the node via node value, return NULL if not found
 */
ListNode *list_find(List *self, void *val)
{
    ListIterator *it;
    ListNode *    node;

    if (NULL == (it = list_iterator_new(self, LIST_HEAD))) {
        return NULL;
    }
    node = list_iterator_next(it);
    while (node) {
        if (self->match) {
            if (self->match(val, node->val)) {
                list_iterator_destroy(it);
                return node;
            }
        } else {
            if (val == node->val) {
                list_iterator_destroy(it);
                return node;
            }
        }
        node = list_iterator_next(it);
    }

    list_iterator_destroy(it);
    return NULL;
}

/*
 * find the node via list index, return NULL if not found
 */
ListNode *list_at(List *self, int index)
{
    ListDirection direction = LIST_HEAD;

    if (index < 0) {
        direction = LIST_TAIL;
        index     = ~index;
    }

    if ((unsigned)index < self->len) {
        ListIterator *it;
        ListNode *    node;

        if (NULL == (it = list_iterator_new(self, direction))) {
            return NULL;
        }
        node = list_iterator_next(it);

        while (index--) {
            node = list_iterator_next(it);
        }
        list_iterator_destroy(it);
        return node;
    }

    return NULL;
}

/*
 * delete the node in list and release the resource
 */
void list_remove(List *self, ListNode *node)
{
    node->prev ? (node->prev->next = node->next) : (self->head = node->next);

    node->next ? (node->next->prev = node->prev) : (self->tail = node->prev);

    if (self->free) {
        self->free(node->val);
    }

    HAL_Free(node);
    if (self->len)
        --self->len;
}

/*
 * create a new ListIterator and set the ListDirection.
 */
ListIterator *list_iterator_new(List *list, ListDirection direction)
{
    ListNode *node = direction == LIST_HEAD ? list->head : list->tail;
    return list_iterator_new_from_node(node, direction);
}

/*
 * create a new ListIterator and set the ListDirection and node
 */
ListIterator *list_iterator_new_from_node(ListNode *node, ListDirection direction)
{
    ListIterator *self;
    self = HAL_Malloc(sizeof(ListIterator));
    if (!self) {
        return NULL;
    }
    self->next      = node;
    self->direction = direction;
    return self;
}

/*
 * return next node
 */
ListNode *list_iterator_next(ListIterator *self)
{
    ListNode *curr = self->next;
    if (curr) {
        self->next = self->direction == LIST_HEAD ? curr->next : curr->prev;
    }
    return curr;
}

/*
 * release the ListIterator
 */
void list_iterator_destroy(ListIterator *self)
{
    HAL_Free(self);
}

/*
 * create a new node and set the value
 */
ListNode *list_node_new(void *val)
{
    ListNode *self;
    self = HAL_Malloc(sizeof(ListNode));
    if (!self) {
        return NULL;
    }

    self->prev = NULL;
    self->next = NULL;
    self->val  = val;
    return self;
}

#ifdef __cplusplus
}
#endif
