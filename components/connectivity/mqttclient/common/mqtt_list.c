/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 22:46:33
 * @LastEditTime: 2020-04-27 23:28:12
 * @Description: the following code references TencentOS tiny, please keep the author information and source code according to the license.
 */

# include "mqtt_list.h"

static void _mqtt_list_add(mqtt_list_t *node, mqtt_list_t *prev, mqtt_list_t *next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static void _mqtt_list_del(mqtt_list_t *prev, mqtt_list_t *next)
{
    next->prev = prev;
    prev->next = next;
}

static void _mqtt_list_del_entry(mqtt_list_t *entry)
{
    _mqtt_list_del(entry->prev, entry->next);
}

void mqtt_list_init(mqtt_list_t *list)
{
    list->next = list;
    list->prev = list;
}

void mqtt_list_add(mqtt_list_t *node, mqtt_list_t *list)
{
    _mqtt_list_add(node, list, list->next);
}

void mqtt_list_add_tail(mqtt_list_t *node, mqtt_list_t *list)
{
    _mqtt_list_add(node, list->prev, list);
}

void mqtt_list_del(mqtt_list_t *entry)
{
    _mqtt_list_del(entry->prev, entry->next);
}

void mqtt_list_del_init(mqtt_list_t *entry)
{
    _mqtt_list_del_entry(entry);
    mqtt_list_init(entry);
}

void mqtt_list_move(mqtt_list_t *node, mqtt_list_t *list)
{
    _mqtt_list_del_entry(node);
    mqtt_list_add(node, list);
}

void mqtt_list_move_tail(mqtt_list_t *node, mqtt_list_t *list)
{
    _mqtt_list_del_entry(node);
    mqtt_list_add_tail(node, list);
}

int mqtt_list_is_empty(mqtt_list_t *list)
{
    return list->next == list;
}
