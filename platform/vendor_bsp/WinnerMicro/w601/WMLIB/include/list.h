/*
 * @file list.h
 * @brief Doubly-linked list
 * @copyright (c) 2009, Jouni Malinen <j@w1.fi>
 *
 * @note This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef LIST_H
#define LIST_H

#include <stdio.h>

/** struct dl_list - Doubly-linked list */
struct dl_list {
	struct dl_list *next;    /**< pointer to the next */
	struct dl_list *prev;    /**< pointer to the previous */
};

/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup DLIST_APIs DLIST APIs
 * @brief Double listed APIs
 */

/**
 * @addtogroup DLIST_APIs
 * @{
 */

/**
 * @brief          reinitialize the list
 *
 * @param[in]      *list    the list
 *
 * @return         None
 *
 * @note           None
 */
static __inline void dl_list_init(struct dl_list *list)
{
	list->next = list;
	list->prev = list;
}

/**
 * @brief          Insert a new entry after the specified head
 *
 * @param[in]      *list    list head to add it after
 * @param[in]      *item    new entry to be added
 *
 * @return         None
 *
 * @note           None
 */
static __inline void dl_list_add(struct dl_list *list, struct dl_list *item)
{
	item->next = list->next;
	item->prev = list;
	list->next->prev = item;
	list->next = item;
}

/**
 * @brief          Insert a new entry before the specified head
 *
 * @param[in]      *list        list head to add it after
 * @param[in]      *item        new entry to be added
 *
 * @return         None
 *
 * @note           None
 */
static __inline void dl_list_add_tail(struct dl_list *list, struct dl_list *item)
{
	dl_list_add(list->prev, item);
}

/**
 * @brief          deletes entry from list
 *
 * @param[in]      *item    the element to delete from the list
 *
 * @return         None
 *
 * @note           None
 */
static __inline void dl_list_del(struct dl_list *item)
{
	item->next->prev = item->prev;
	item->prev->next = item->next;
	item->next = NULL;
	item->prev = NULL;
}

/**
 * @brief          tests whether a list is empty
 *
 * @param[in]      *list    the list to test
 *
 * @retval         0        not empty
 * @retval         1        empty
 *
 * @note           None
 */
static __inline int dl_list_empty(struct dl_list *list)
{
	return list->next == list;
}

/**
 * @brief          count length of the list
 *
 * @param[in]      *list    the list to count
 *
 * @return         length
 *
 * @note           None
 */
static __inline unsigned int dl_list_len(struct dl_list *list)
{
	struct dl_list *item;
	int count = 0;
	for (item = list->next; item != list; item = item->next)
		count++;
	return count;
}

/**
 * @}
 */

/**
 * @}
 */

#ifndef offsetof
/** offset address of the struct member */
#define offsetof(type, member) ((long) &((type *) 0)->member)
#endif

/**
 * @brief          get the struct for this entry
 *
 * @param[in]      item      the &struct list_head pointer
 * @param[in]      type      the type of the struct this is embedded in
 * @param[in]      member    the name of the list_struct within the struct
 *
 * @return         pointer to the struct for this entry
 *
 * @note           None
 */
#define dl_list_entry(item, type, member) \
	((type *) ((char *) item - offsetof(type, member)))

/**
 * @brief          get the first element from a list
 *
 * @param[in]      list      the list head to take the element from
 * @param[in]      type      the type of the struct this is embedded in
 * @param[in]      member    the name of the list_struct within the struct
 *
 * @return         pointer to the first element from a list
 *
 * @note           None
 */
#define dl_list_first(list, type, member) \
	(dl_list_empty((list)) ? NULL : \
	 dl_list_entry((list)->next, type, member))

/**
 * @brief          get the last element from a list
 *
 * @param[in]      list      the list head to take the element from
 * @param[in]      type      the type of the struct this is embedded in
 * @param[in]      member    the name of the list_struct within the struct
 *
 * @return         pointer to the last element from a list
 *
 * @note           None
 */
#define dl_list_last(list, type, member) \
	(dl_list_empty((list)) ? NULL : \
	 dl_list_entry((list)->prev, type, member))

/**
 * @brief          iterate over list of given type
 *
 * @param[in]      item      a loop cursor
 * @param[in]      list      the head for your list
 * @param[in]      type      struct type
 * @param[in]      member    the name of the list_struct within the struct
 *
 * @return         None
 *
 * @note           None
 */
#define dl_list_for_each(item, list, type, member) \
	for (item = dl_list_entry((list)->next, type, member); \
	     &item->member != (list); \
	     item = dl_list_entry(item->member.next, type, member))

/**
 * @brief          iterate over list of given type safe against removal of list entry
 *
 * @param[in]      item      a loop cursor
 * @param[in]      n         temporary storage
 * @param[in]      list      the head for your list
 * @param[in]      type      struct type
 * @param[in]      member    the name of the list_struct within the struct
 *
 * @return         None
 *
 * @note           None
 */
#define dl_list_for_each_safe(item, n, list, type, member) \
	for (item = dl_list_entry((list)->next, type, member), \
		     n = dl_list_entry(item->member.next, type, member); \
	     &item->member != (list); \
	     item = n, n = dl_list_entry(n->member.next, type, member))

/**
 * @brief          iterate backwards over list of given type
 *
 * @param[in]      item      a loop cursor
 * @param[in]      list      the head for your list
 * @param[in]      type      struct type
 * @param[in]      member    the name of the list_struct within the struct
 *
 * @return         None
 *
 * @note           None
 */
#define dl_list_for_each_reverse(item, list, type, member) \
	for (item = dl_list_entry((list)->prev, type, member); \
	     &item->member != (list); \
	     item = dl_list_entry(item->member.prev, type, member))

/** define the list head */
#define DEFINE_DL_LIST(name) \
	struct dl_list name = { &(name), &(name) }

/**
 * @brief          iterate over list of given type
 *
 * @param[in]      item      the type * to use as a loop cursor
 * @param[in]      list      the head for your list
 * @param[in]      member    the name of the list_struct within the struct
 *
 * @return         None
 *
 * @note           None
 */
#define __dl_list_for_each(item, list, member) \
	for (item = dl_list_entry((list)->next, typeof(*(item)), member); \
	     &item->member != (list); \
	     item = dl_list_entry(item->member.next, typeof(*(item)), member))

/**
 * @brief          iterate over list of given type safe against removal of list entry
 *
 * @param[in]      item      the type * to use as a loop cursor
 * @param[in]      n         temporary storage
 * @param[in]      list      the head for your list
 * @param[in]      member    the name of the list_struct within the struct
 *
 * @return         None
 *
 * @note           None
 */
#define __dl_list_for_each_safe(item, n, list, member) \
	for (item = dl_list_entry((list)->next, typeof(*(item)), member), \
		     n = dl_list_entry(item->member.next, typeof(*(item)), member); \
	     &item->member != (list); \
	     item = n, n = dl_list_entry(n->member.next, typeof(*(item)), member))

/**
 * @brief          iterate backwards over list of given type
 *
 * @param[in]      item      the type * to use as a loop cursor
 * @param[in]      list      the head for your list
 * @param[in]      member    the name of the list_struct within the struct
 *
 * @return         None
 *
 * @note           None
 */
#define __dl_list_for_each_reverse(item, list, member) \
	for (item = dl_list_entry((list)->prev, typeof(*(item)), member); \
	     &item->member != (list); \
	     item = dl_list_entry(item->member.prev, typeof(*(item)), member))

#endif /* LIST_H */
