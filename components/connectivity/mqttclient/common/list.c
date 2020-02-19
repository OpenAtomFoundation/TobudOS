/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 22:46:33
 * @LastEditTime : 2020-01-05 17:01:51
 * @Description: the following code references TencentOS tiny, please keep the author information and source code according to the license.
 */

# include "list.h"

static void _list_add(list_t *node, list_t *prev, list_t *next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static void _list_del(list_t *prev, list_t *next)
{
    next->prev = prev;
    prev->next = next;
}

static void _list_del_entry(list_t *entry)
{
    _list_del(entry->prev, entry->next);
}

void list_init(list_t *list)
{
    list->next = list;
    list->prev = list;
}

void list_add(list_t *node, list_t *list)
{
    _list_add(node, list, list->next);
}

void list_add_tail(list_t *node, list_t *list)
{
    _list_add(node, list->prev, list);
}

void list_del(list_t *entry)
{
    _list_del(entry->prev, entry->next);
}

void list_del_init(list_t *entry)
{
    _list_del_entry(entry);
    list_init(entry);
}

void list_move(list_t *node, list_t *list)
{
    _list_del_entry(node);
    list_add(node, list);
}

void list_move_tail(list_t *node, list_t *list)
{
    _list_del_entry(node);
    list_add_tail(node, list);
}

int list_is_empty(list_t *list)
{
    return list->next == list;
}
