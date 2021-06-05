/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 22:47:55
 * @LastEditTime: 2020-10-17 14:18:02
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTT_LIST_H_
#define _MQTT_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mqtt_list_node {
    struct mqtt_list_node *next;
    struct mqtt_list_node *prev;
} mqtt_list_t;

#define OFFSET_OF_FIELD(type, field) \
    ((size_t)&(((type *)0)->field))

#define CONTAINER_OF_FIELD(ptr, type, field) \
    ((type *)((unsigned char *)(ptr) - OFFSET_OF_FIELD(type, field)))

#define LIST_NODE(node) \
    { &(node), &(node) }

#define LIST_DEFINE(list) \
    mqtt_list_t list = { &(list), &(list) }

#define LIST_ENTRY(list, type, field) \
    CONTAINER_OF_FIELD(list, type, field)

#define LIST_FIRST_ENTRY(list, type, field) \
    LIST_ENTRY((list)->next, type, field)

#define LIST_FIRST_ENTRY_OR_NULL(list, type, field) \
    (mqtt_list_is_empty(list) ? NULL : LIST_FIRST_ENTRY(list, type, field))

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

void mqtt_list_init(mqtt_list_t *list);
void mqtt_list_add(mqtt_list_t *node, mqtt_list_t *list);
void mqtt_list_add_tail(mqtt_list_t *node, mqtt_list_t *list);
void mqtt_list_del(mqtt_list_t *entry);
void mqtt_list_del_init(mqtt_list_t *entry);
void mqtt_list_move(mqtt_list_t *node, mqtt_list_t *list);
void mqtt_list_move_tail(mqtt_list_t *node, mqtt_list_t *list);
int mqtt_list_is_empty(mqtt_list_t *list);

#ifdef __cplusplus
}
#endif

#endif /* _LIST_H_ */

