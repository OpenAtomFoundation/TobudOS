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

#ifndef _TOS_LIST_H_
#define  _TOS_LIST_H_

typedef struct k_list_node_st {
    struct k_list_node_st *next;
    struct k_list_node_st *prev;
} k_list_t;

#define TOS_LIST_NODE(node) \
    { &(node), &(node) }

#define TOS_LIST_DEFINE(list) \
    k_list_t list = { &(list), &(list) }

#define TOS_LIST_ENTRY(list, type, field) \
    TOS_CONTAINER_OF_FIELD(list, type, field)

#define TOS_LIST_FIRST_ENTRY(list, type, field) \
    TOS_LIST_ENTRY((list)->next, type, field)

#define TOS_LIST_FIRST_ENTRY_OR_NULL(list, type, field) \
    (tos_list_empty(list) ? K_NULL : TOS_LIST_FIRST_ENTRY(list, type, field))

#define TOS_LIST_FOR_EACH(curr, list) \
    for (curr = (list)->next; curr != (list); curr = curr->next)

#define TOS_LIST_FOR_EACH_PREV(curr, list) \
    for (curr = (list)->prev; curr != (list); curr = curr->prev)

#define TOS_LIST_FOR_EACH_SAFE(curr, next, list) \
    for (curr = (list)->next, next = curr->next; curr != (list); \
            curr = next, next = curr->next)

#define TOS_LIST_FOR_EACH_PREV_SAFE(curr, next, list) \
    for (curr = (list)->prev, next = curr->prev; \
            curr != (list); \
            curr = next, next = curr->prev)

__STATIC_INLINE__ void _list_add(k_list_t *node, k_list_t *prev, k_list_t *next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

__STATIC_INLINE__ void _list_del(k_list_t *prev, k_list_t *next)
{
    next->prev = prev;
    prev->next = next;
}

__STATIC_INLINE__ void _list_del_entry(k_list_t *entry)
{
    _list_del(entry->prev, entry->next);
}

__API__ __STATIC_INLINE__ void tos_list_init(k_list_t *list)
{
    list->next = list;
    list->prev = list;
}

__API__ __STATIC_INLINE__ void tos_list_add(k_list_t *node, k_list_t *list)
{
    _list_add(node, list, list->next);
}

__API__ __STATIC_INLINE__ void tos_list_add_tail(k_list_t *node, k_list_t *list)
{
    _list_add(node, list->prev, list);
}

__API__ __STATIC_INLINE__ void tos_list_del(k_list_t *entry)
{
    _list_del(entry->prev, entry->next);
}

__API__ __STATIC_INLINE__ void tos_list_del_init(k_list_t *entry)
{
    _list_del_entry(entry);
    tos_list_init(entry);
}

__API__ __STATIC_INLINE__ void tos_list_move(k_list_t *node, k_list_t *list)
{
    _list_del_entry(node);
    tos_list_add(node, list);
}

__API__ __STATIC_INLINE__ void tos_list_move_tail(k_list_t *node, k_list_t *list)
{
    _list_del_entry(node);
    tos_list_add_tail(node, list);
}

__API__ __STATIC_INLINE__ int tos_list_empty(const k_list_t *list)
{
    return list->next == list;
}

#endif /* _TOS_LIST_H_ */

