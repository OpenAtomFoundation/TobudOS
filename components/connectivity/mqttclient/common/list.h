/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 22:47:55
 * @LastEditTime : 2020-01-08 20:39:26
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _LIST_H_
#define _LIST_H_

typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_t;

#define OFFSET_OF_FIELD(type, field) \
    ((size_t)&(((type *)0)->field))

#define CONTAINER_OF_FIELD(ptr, type, field) \
    ((type *)((unsigned char *)(ptr) - OFFSET_OF_FIELD(type, field)))

#define LIST_NODE(node) \
    { &(node), &(node) }

#define LIST_DEFINE(list) \
    list_t list = { &(list), &(list) }

#define LIST_ENTRY(list, type, field) \
    CONTAINER_OF_FIELD(list, type, field)

#define LIST_FIRST_ENTRY(list, type, field) \
    LIST_ENTRY((list)->next, type, field)

#define LIST_FIRST_ENTRY_OR_NULL(list, type, field) \
    (list_is_empty(list) ? NULL : LIST_FIRST_ENTRY(list, type, field))

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

void list_init(list_t *list);
void list_add(list_t *node, list_t *list);
void list_add_tail(list_t *node, list_t *list);
void list_del(list_t *entry);
void list_del_init(list_t *entry);
void list_move(list_t *node, list_t *list);
void list_move_tail(list_t *node, list_t *list);
int list_is_empty(list_t *list);

#endif /* _LIST_H_ */

