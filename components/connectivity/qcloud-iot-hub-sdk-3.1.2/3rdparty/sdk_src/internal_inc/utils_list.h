/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_IOT_UTILS_LIST_H_
#define QCLOUD_IOT_UTILS_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>


/*
 * ListNode iterator direction
 */
typedef enum {
    LIST_HEAD,
    LIST_TAIL
} ListDirection;

/*
 * define list node
 */
typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
    void *val;
} ListNode;

/*
 * Double Linked List
 */
typedef struct {
    ListNode *head;
    ListNode *tail;
    unsigned int len;
    void (*free)(void *val);
    int (*match)(void *a, void *b);
} List;

/*
 * list iterator
 */
typedef struct {
    ListNode *next;
    ListDirection direction;
} ListIterator;


/* create node */
ListNode *list_node_new(void *val);

/* create list */
List *list_new(void);

ListNode *list_rpush(List *self, ListNode *node);

ListNode *list_lpush(List *self, ListNode *node);

ListNode *list_find(List *self, void *val);

ListNode *list_at(List *self, int index);

ListNode *list_rpop(List *self);

ListNode *list_lpop(List *self);

void list_remove(List *self, ListNode *node);

void list_destroy(List *self);

/* create iterator */
ListIterator *list_iterator_new(List *list, ListDirection direction);

ListIterator *list_iterator_new_from_node(ListNode *node, ListDirection direction);

ListNode *list_iterator_next(ListIterator *self);

void list_iterator_destroy(ListIterator *self);

#ifdef __cplusplus
}
#endif
#endif //QCLOUD_IOT_UTILS_LIST_H_
