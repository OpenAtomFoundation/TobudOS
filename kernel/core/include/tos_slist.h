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

#ifndef _TOS_SLIST_H_
#define  _TOS_SLIST_H_

typedef struct k_slist_node_st {
    struct k_slist_node_st *next;
} k_slist_t;

#define TOS_SLIST_NODE(dummy) \
    { K_NULL }

#define TOS_SLIST_DEFINE(slist) \
    k_slist_t slist = { K_NULL }

#define TOS_SLIST_ENTRY(node, type, field) \
    TOS_CONTAINER_OF_FIELD(node, type, field)

#define TOS_SLIST_FIRST_ENTRY(slist, type, field) \
    TOS_SLIST_ENTRY((slist)->next, type, field)

#define TOS_SLIST_FIRST_ENTRY_OR_NULL(slist, type, field) \
    (tos_slist_empty(slist) ? K_NULL : TOS_SLIST_FIRST_ENTRY(slist, type, field))

#define TOS_SLIST_FOR_EACH(curr, slist) \
    for (curr = (slist)->next; curr; curr = curr->next)

#define TOS_SLIST_FOR_EACH_SAFE(curr, next, slist) \
    for (curr = (slist)->next, next = curr->next; curr; \
            curr = next, next = curr->next)

#define TOS_SLIST_FOR_EACH_ENTRY(entry, type, field, slist) \
    for (entry = TOS_SLIST_ENTRY((slist)->next, type, field); \
        &entry->field; \
        entry = TOS_SLIST_ENTRY(entry->field.next, type, field))

#define TOS_SLIST_FOR_EACH_ENTRY_SAFE(entry, tmp, type, field, slist) \
    for (entry = TOS_SLIST_ENTRY((slist)->next, type, field), \
            tmp = TOS_SLIST_ENTRY(entry->field.next, type, field); \
            &entry->field; \
            entry = tmp, tmp = TOS_SLIST_ENTRY(entry->field.next, type, field))

__API__ __STATIC_INLINE__ void tos_slist_init(k_slist_t *slist)
{
    slist->next = K_NULL;
}

__API__ __STATIC_INLINE__ k_slist_t *tos_slist_head(k_slist_t *slist)
{
    return slist->next;
}

__API__ __STATIC_INLINE__ k_slist_t *tos_slist_tail(k_slist_t *slist)
{
    if (!slist->next) {
        return K_NULL;
    }

    while (slist->next) {
        slist = slist->next;
    }

    return slist;
}

__API__ __STATIC_INLINE__ void tos_slist_add_head(k_slist_t *node, k_slist_t *slist)
{
    node->next   = slist->next;
    slist->next  = node;
}

__API__ __STATIC_INLINE__ void tos_slist_add_tail(k_slist_t *node, k_slist_t *slist)
{
    node->next = K_NULL;

    while (slist->next) {
        slist = slist->next;
    }

    tos_slist_add_head(node, slist);
}

__API__ __STATIC_INLINE__ void tos_slist_insert(k_slist_t *next_node, k_slist_t *new_node, k_slist_t *slist)
{
    if (!next_node) {
        tos_slist_add_tail(new_node, slist);
        return;
    }

    while (slist->next) {
        if (slist->next == next_node) {
            slist->next     = new_node;
            new_node->next  = next_node;
        }

        slist = slist->next;
    }
}

__API__ __STATIC_INLINE__ k_slist_t *tos_slist_del_head(k_slist_t *slist)
{
    k_slist_t *head;

    if (!slist->next) {
        return K_NULL;
    }

    head        = slist->next;
    slist->next = head->next;
    head->next  = K_NULL;

    return head;
}

__API__ __STATIC_INLINE__ k_slist_t *tos_slist_del_tail(k_slist_t *slist)
{
    while (slist->next) {
        if (!slist->next->next) {
            return tos_slist_del_head(slist);
        }

        slist = slist->next;
    }

    return K_NULL;
}

__API__ __STATIC_INLINE__ void tos_slist_del(k_slist_t *node, k_slist_t *slist)
{
    while (slist->next) {
        if (slist->next == node) {
            slist->next = node->next;
            break;
        }

        slist = slist->next;
    }
}

__API__ __STATIC_INLINE__ int tos_slist_length(k_slist_t *slist)
{
    int len = 0;
    k_slist_t *iter;

    TOS_SLIST_FOR_EACH(iter, slist) {
        ++len;
    }

    return len;
}

__API__ __STATIC_INLINE__ int tos_slist_contains(k_slist_t *node, k_slist_t *slist)
{
    while (slist->next) {
        if (slist->next == node) {
            return K_TRUE;
        }

        slist = slist->next;
    }

    return K_FALSE;
}
__API__ __STATIC_INLINE__ int tos_slist_empty(k_slist_t *slist)
{
    return !slist->next;
}

#endif /* _TOS_SLIST_H_ */

