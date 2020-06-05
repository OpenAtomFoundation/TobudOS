/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _LIST_H_
#define  _LIST_H_

#include "stdint.h"

typedef struct list_node_st {
    struct list_node_st *next;
    struct list_node_st *prev;
} list_t;

#define OFFSET_OF_FIELD(type, field)    \
    ((uint32_t)&(((type *)0)->field))

#define CONTAINER_OF_FIELD(ptr, type, field)    \
    ((type *)((uint8_t *)(ptr) - OFFSET_OF_FIELD(type, field)))

#define LIST_NODE(node) \
    { &(node), &(node) }

#define LIST_DEFINE(list) \
    dlist_t list = { &(list), &(list) }

#define LIST_ENTRY(node, type, field) \
    CONTAINER_OF_FIELD(node, type, field)

#define LIST_FIRST_ENTRY(list, type, field) \
    LIST_ENTRY((list)->next, type, field)

#define LIST_FIRST_ENTRY_OR_NULL(list, type, field) \
    (list_empty(list) ? K_NULL : LIST_FIRST_ENTRY(list, type, field))

#define LIST_FOR_EACH(curr, list) \
    for (curr = (list)->next; curr != (list); curr = curr->next)

#define LIST_FOR_EACH_PREV(curr, list) \
    for (curr = (list)->prev; curr != (list); curr = curr->prev)

#define LIST_FOR_EACH_SAFE(curr, next, list) \
    for (curr = (list)->next, next = curr->next; curr != (list); \
            curr = next, next = curr->next)

#define LIST_FOR_EACH_PREV_SAFE(curr, next, list) \
    for (curr = (list)->prev, next = curr->prev; \
            curr != (list); \
            curr = next, next = curr->prev)

#define LIST_FOR_EACH_ENTRY(entry, type, field, list) \
    for (entry = LIST_ENTRY((list)->next, type, field); \
        &entry->field != (list); \
        entry = LIST_ENTRY(entry->field.next, type, field))

#define LIST_FOR_EACH_ENTRY_REVERSE(entry, type, field, list) \
    for (entry = LIST_ENTRY((list)->prev, type, field); \
            &entry->field != (list); \
            entry = LIST_ENTRY(entry->field.prev, type, field))

#define LIST_FOR_EACH_ENTRY_SAFE(entry, tmp, type, field, list) \
    for (entry = LIST_ENTRY((list)->next, type, field), \
            tmp = LIST_ENTRY(entry->field.next, type, field); \
            &entry->field != (list); \
            entry = tmp, tmp = LIST_ENTRY(entry->field.next, type, field))

#define LIST_FOR_EACH_ENTRY_SAFE_REVERSE(entry, tmp, type, field, list) \
    for (entry = LIST_ENTRY((list)->prev, type, field), \
            tmp = LIST_ENTRY(entry->field.prev, type, field); \
            &entry->field != (list); \
            entry = tmp, tmp = LIST_ENTRY(entry->field.prev, type, field))

static inline void _list_add(list_t *node, list_t *prev, list_t *next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static inline void _list_del(list_t *prev, list_t *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void _list_del_node(list_t *node)
{
    _list_del(node->prev, node->next);
}

static inline void list_init(list_t *list)
{
    list->next = list;
    list->prev = list;
}

static inline void list_add(list_t *node, list_t *list)
{
    _list_add(node, list, list->next);
}

static inline void list_add_tail(list_t *node, list_t *list)
{
    _list_add(node, list->prev, list);
}

static inline void list_del(list_t *node)
{
    _list_del(node->prev, node->next);
}

static inline void list_del_init(list_t *node)
{
    _list_del_node(node);
    list_init(node);
}

static inline void list_move(list_t *node, list_t *list)
{
    _list_del_node(node);
    list_add(node, list);
}

static inline void list_move_tail(list_t *node, list_t *list)
{
    _list_del_node(node);
    list_add_tail(node, list);
}

static inline int list_empty(const list_t *list)
{
    return list->next == list;
}

#endif /* _LIST_H_ */

