/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __MEM_STATS_H__
#define __MEM_STATS_H__

#include "infra_list.h"
#include "infra_log.h"

#if defined(__UBUNTU_SDK_DEMO__)
    #include <execinfo.h>
#endif

typedef struct {
    void               *buf;
    int                 buflen;
    char               *func;
    int                 line;
#if defined(_PLATFORM_IS_LINUX_)
    char              **bt_symbols;
    int                 bt_level;
#endif
    list_head_t         list;
    void               *mem_table;
} OS_malloc_record;


#define MEM_MAGIC                       (0x1234)

#define LITE_calloc(num, size, ...)     LITE_malloc_internal(__func__, __LINE__, (num * size), ##__VA_ARGS__)
#define LITE_malloc(size, ...)          LITE_malloc_internal(__func__, __LINE__, size, ##__VA_ARGS__)
#define LITE_realloc(ptr, size, ...)    LITE_realloc_internal(__func__, __LINE__, ptr, size, ##__VA_ARGS__)
#define LITE_free(ptr)              \
    do { \
        if (!ptr) { \
            log_warning("utils", "%s == NULL! LITE_free(%s) aborted.", #ptr, #ptr); \
            break; \
        } \
        \
        LITE_free_internal((void *)ptr); \
        ptr = NULL; \
    } while(0)

void *LITE_malloc_internal(const char *f, const int l, int size, ...);
void *LITE_realloc_internal(const char *f, const int l, void *ptr, int size, ...);
void LITE_free_internal(void *ptr);
void LITE_dump_malloc_free_stats(int level);
void **LITE_get_mem_mutex(void);

#endif  /* __MEM_STATS_H__ */

