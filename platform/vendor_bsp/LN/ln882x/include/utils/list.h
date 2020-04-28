#ifndef __LIST_H__
#define __LIST_H__

#include "ln88xx.h"

typedef struct list_s {
    struct list_s *next;
    struct list_s *prev;
} list_t;

#define list_entry(node, type, member) ((type *)((uint8_t *)(node) - (size_t)(&((type *)0)->member)))

__STATIC_INLINE void list_init(list_t *list_head)
{
    list_head->next = list_head;
    list_head->prev = list_head;
}

__STATIC_INLINE bool is_list_empty(list_t *list)
{
    return (list->next == list);
}

__STATIC_INLINE void list_insert(list_t *head, list_t *element)
{
    element->prev = head->prev;
    element->next = head;

    head->prev->next = element;
    head->prev       = element;
}

__STATIC_INLINE void list_add(list_t *head, list_t *element)
{
    element->prev = head;
    element->next = head->next;

    head->next->prev = element;
    head->next = element;
}

__STATIC_INLINE void list_rm(list_t *element)
{
    element->prev->next = element->next;
    element->next->prev = element->prev;
}

__STATIC_INLINE void list_rm_init(list_t *element)
{
    element->prev->next = element->next;
    element->next->prev = element->prev;

    element->next = element->prev = element;
}

#endif /* __LIST_H__ */

