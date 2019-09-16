#ifndef _QCLOUD_LIST_H_
#define  _QCLOUD_LIST_H_

typedef struct qcloud_list_node_st {
    struct qcloud_list_node_st *next;
    struct qcloud_list_node_st *prev;
} qcloud_list_t;

#define QCLOUD_OFFSET_OF_FIELD(type, field) \
    ((uint32_t)&(((type *)0)->field))

#define QCLOUD_CONTAINER_OF_FIELD(ptr, type, field) \
    ((type *)((uint8_t *)(ptr) - QCLOUD_OFFSET_OF_FIELD(type, field)))

#define QCLOUD_LIST_NODE(node) \
    { &(node), &(node) }

#define QCLOUD_LIST_DEFINE(list) \
    qcloud_list_t list = { &(list), &(list) }

#define QCLOUD_LIST_ENTRY(list, type, field) \
    QCLOUD_CONTAINER_OF_FIELD(list, type, field)

#define QCLOUD_LIST_FIRST_ENTRY(list, type, field) \
    QCLOUD_LIST_ENTRY((list)->next, type, field)

#define QCLOUD_LIST_FIRST_ENTRY_OR_NULL(list, type, field) \
    (qcloud_list_empty(list) ? NULL : QCLOUD_LIST_FIRST_ENTRY(list, type, field))

#define QCLOUD_LIST_FOR_EACH(curr, list) \
    for (curr = (list)->next; curr != (list); curr = curr->next)

#define QCLOUD_LIST_FOR_EACH_PREV(curr, list) \
    for (curr = (list)->prev; curr != (list); curr = curr->prev)

#define QCLOUD_LIST_FOR_EACH_SAFE(curr, next, list) \
    for (curr = (list)->next, next = curr->next; curr != (list); \
            curr = next, next = curr->next)

#define QCLOUD_LIST_FOR_EACH_PREV_SAFE(curr, next, list) \
    for (curr = (list)->prev, next = curr->prev; \
            curr != (list); \
            curr = next, next = curr->prev)

__QCLOUD_STATIC__ void _qcloud_list_add(qcloud_list_t *node, qcloud_list_t *prev, qcloud_list_t *next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

__QCLOUD_STATIC__ void _qcloud_list_del(qcloud_list_t *prev, qcloud_list_t *next)
{
    next->prev = prev;
    prev->next = next;
}

__QCLOUD_STATIC__ void _qcloud_list_del_entry(qcloud_list_t *entry)
{
    _qcloud_list_del(entry->prev, entry->next);
}

__QCLOUD_API__ __QCLOUD_STATIC__ void qcloud_list_init(qcloud_list_t *list)
{
    list->next = list;
    list->prev = list;
}

__QCLOUD_API__ __QCLOUD_STATIC__ void qcloud_list_add(qcloud_list_t *node, qcloud_list_t *list)
{
    _qcloud_list_add(node, list, list->next);
}

__QCLOUD_API__ __QCLOUD_STATIC__ void qcloud_list_add_tail(qcloud_list_t *node, qcloud_list_t *list)
{
    _qcloud_list_add(node, list->prev, list);
}

__QCLOUD_API__ __QCLOUD_STATIC__ void qcloud_list_del(qcloud_list_t *entry)
{
    _qcloud_list_del(entry->prev, entry->next);
}

__QCLOUD_API__ __QCLOUD_STATIC__ void qcloud_list_del_init(qcloud_list_t *entry)
{
    _qcloud_list_del_entry(entry);
    qcloud_list_init(entry);
}

__QCLOUD_API__ __QCLOUD_STATIC__ void qcloud_list_move(qcloud_list_t *node, qcloud_list_t *list)
{
    _qcloud_list_del_entry(node);
    qcloud_list_add(node, list);
}

__QCLOUD_API__ __QCLOUD_STATIC__ void qcloud_list_move_tail(qcloud_list_t *node, qcloud_list_t *list)
{
    _qcloud_list_del_entry(node);
    qcloud_list_add_tail(node, list);
}

__QCLOUD_API__ __QCLOUD_STATIC__ int qcloud_list_empty(const qcloud_list_t *list)
{
    return list->next == list;
}

#endif /* _QCLOUD_LIST_H_ */

