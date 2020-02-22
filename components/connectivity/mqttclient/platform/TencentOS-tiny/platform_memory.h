/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-14 22:06:35
 * @LastEditTime : 2019-12-20 20:57:32
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _PLATFORM_MEMORY_H_
#define _PLATFORM_MEMORY_H_
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

void *platform_memory_alloc(size_t size);
void *platform_memory_calloc(size_t num, size_t size);
void platform_memory_free(void *ptr);

#endif
