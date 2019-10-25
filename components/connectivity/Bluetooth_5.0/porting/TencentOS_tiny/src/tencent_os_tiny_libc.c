#include "tos.h"

#ifdef __CC_ARM
/* avoid the heap and heap-using library functions supplied by arm */
#pragma import(__use_no_heap)
#endif

void *malloc(size_t n)
{
    return tos_mmheap_alloc(n);
}

void *realloc(void *rmem, size_t newsize)
{
    return tos_mmheap_realloc(rmem, newsize);
}

void *calloc(size_t nelem, size_t elsize)
{
    return tos_mmheap_calloc(nelem, elsize);
}

void free(void *rmem)
{
    tos_mmheap_free(rmem);
}

