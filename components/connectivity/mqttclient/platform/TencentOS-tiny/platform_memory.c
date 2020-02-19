/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-14 22:02:07
 * @LastEditTime : 2019-12-20 20:43:38
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "tos_k.h"


void *platform_memory_alloc(size_t size)
{
    return tos_mmheap_alloc(size);
}

void *platform_memory_calloc(size_t num, size_t size)
{
    return tos_mmheap_calloc(num, size);
}

void platform_memory_free(void *ptr)
{
    tos_mmheap_free(ptr);
}



