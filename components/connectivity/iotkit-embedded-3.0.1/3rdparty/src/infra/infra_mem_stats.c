/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "infra_config.h"

#ifdef INFRA_MEM_STATS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include "infra_mem_stats.h"
#ifdef INFRA_LOG
    #include "infra_log.h"
    #define utils_emerg(...)    log_emerg("util", __VA_ARGS__)
    #define utils_crit(...)     log_crit("util", __VA_ARGS__)
    #define utils_err(...)      log_err("util", __VA_ARGS__)
    #define utils_warning(...)  log_warning("util", __VA_ARGS__)
    #define utils_info(...)     log_info("util", __VA_ARGS__)
    #define utils_debug(...)    log_debug("util", __VA_ARGS__)
#else
    #define utils_emerg(...)
    #define utils_crit(...)
    #define utils_err(...)
    #define utils_warning(...)
    #define utils_info(...)
    #define utils_debug(...)
#endif

extern void *HAL_Malloc(uint32_t size);
extern void HAL_Free(void *ptr);
void *HAL_MutexCreate(void);
void HAL_MutexDestroy(void *mutex);
void HAL_MutexLock(void *mutex);
void HAL_MutexUnlock(void *mutex);


LIST_HEAD(mem_recs);
LIST_HEAD(mem_module_statis);

typedef struct {
    char *func_name;
    int line;
    list_head_t func_head;
} calling_stack_t;

typedef struct {
    char module_name[32];
    calling_stack_t calling_stack;
    int bytes_total_allocated;
    int bytes_total_freed;
    int bytes_total_in_use;
    int bytes_max_allocated;
    int bytes_max_in_use;
    int iterations_allocated;
    int iterations_freed;
    int iterations_in_use;
    int iterations_max_in_use;
} mem_statis_t;

typedef struct {
    mem_statis_t    mem_statis;
    list_head_t     list;
} module_mem_t;

/* sort module used */
static int _mem_cmp_max_used(list_head_t *a, list_head_t *b)
{
    module_mem_t *temp_a = list_entry(a, module_mem_t, list);
    module_mem_t *temp_b = list_entry(b, module_mem_t, list);

    if (a == NULL || b == NULL) {
        return 0;
    }

    /* return true to swap if a < b */
    return (temp_a->mem_statis.bytes_max_in_use < temp_b->mem_statis.bytes_max_in_use);
}

static void _mem_swap_module_pos(list_head_t *a, list_head_t *b)
{
    list_head_t temp = {NULL, NULL};

    if (a == NULL || a == NULL) {
        return;
    }

    list_add(&temp, b);
    list_del(b);
    list_add(b, a);
    list_del(a);
    list_add(a, &temp);
    list_del(&temp);
}

static void _mem_sort_module_pos(list_head_t *head)
{
    list_head_t *start = NULL;
    list_head_t *end = NULL;

    if (head == NULL) {
        return;
    }

    for (end = head->prev; end != head; end = end->prev) {  /* list_for_each_prev */
        list_for_each(start, head) {
            if (start == end) {
                break;
            }

            if (_mem_cmp_max_used(start, start->next)) {
                _mem_swap_module_pos(start, start->next);

                start = start->prev;
                if (start == end) {
                    end = end->next;
                }
            }
        }
    }
}

static void *mutex_mem_stats = NULL;
static int bytes_total_allocated;
static int bytes_total_freed;
static int bytes_total_in_use;
static int bytes_max_allocated;
static int bytes_max_in_use;
static int iterations_allocated;
static int iterations_freed;
static int iterations_in_use;
static int iterations_max_in_use;

#if defined(__UBUNTU_SDK_DEMO__)

static int tracking_malloc_callstack = 1;

char *LITE_strdup(const char *src, ...)
{
    int             len = 0;
    char           *dst = NULL;

    if (!src) {
        return NULL;
    }
    len = strlen(src) + 1;
    if (len > 1024) {
        utils_err("Too long string to duplicate, abort! len = %d", len);
        return NULL;
    }

    dst = (char *)HAL_Malloc(sizeof(char) * len);

    if (!dst) {
        return NULL;
    }
    strncpy(dst, src, len);

    return dst;
}


static int record_backtrace(int *level, char *** trace)
{
#define MAX_BT_LEVEL    8

    void       *buffer[MAX_BT_LEVEL];

    *level = backtrace(buffer, MAX_BT_LEVEL);
    *trace = backtrace_symbols(buffer, *level);
    if (*trace == NULL) {
        utils_err("backtrace_symbols returns NULL!");
        return -1;
    }

    return 0;
}

void LITE_track_malloc_callstack(int state)
{
    tracking_malloc_callstack = state;
}

#else
void LITE_track_malloc_callstack(int state)
{

}

#endif  /* defined(__UBUNTU_SDK_DEMO__) */

void *LITE_realloc_internal(const char *f, const int l, void *ptr, int size, ...)
{
    void               *temp = NULL;
    int                 magic = 0;
    char               *module_name = NULL;
    va_list             ap;

    if (size <= 0) {
        return NULL;
    }

    va_start(ap, size);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);

    temp = LITE_malloc_internal(f, l, size, magic, module_name);

    if (NULL == temp) {
        utils_err("allocate %d bytes from %s(%d) failed", size, f, l);
        return NULL;
    }

    if (ptr) {
        memcpy(temp, ptr, size);

        LITE_free(ptr);

    }

    return temp;
}

void *_create_mem_table(char *module_name, struct list_head *list_head)
{
    module_mem_t *pos = NULL;
    int len = 0;

    if (!module_name || !list_head) {
        return NULL;
    }

    pos = HAL_Malloc(sizeof(module_mem_t));
    if (!pos) {
        return NULL;
    }
    memset(pos, 0, sizeof(module_mem_t));
    len = strlen(module_name);
    memcpy(pos->mem_statis.module_name, module_name,
           (len >= sizeof(pos->mem_statis.module_name)) ? (sizeof(pos->mem_statis.module_name) - 1) : len);

    INIT_LIST_HEAD(&pos->mem_statis.calling_stack.func_head);

    list_add_tail(&pos->list, list_head);
    return (void *)pos;
}

void *_find_mem_table(char *module_name, struct list_head *list_head)
{

    module_mem_t *pos = NULL, *tmp = NULL;

    if (!module_name || !list_head) {
        return NULL;
    }

    list_for_each_entry_safe(pos, tmp, list_head, list, module_mem_t) {
        if (!strcmp(module_name, pos->mem_statis.module_name)) {
            return (void *)pos;
        }
    }

    return NULL;
}

int _del_mem_table(char *module_name, struct list_head *list_head)
{
    int ret = -1;

    calling_stack_t *calling_stack_pos = NULL, *tmp = NULL;
    module_mem_t *table_pos = (module_mem_t *)_find_mem_table(module_name, list_head);
    if (table_pos) {
        list_for_each_entry_safe(calling_stack_pos, tmp, \
                                 &table_pos->mem_statis.calling_stack.func_head, func_head, calling_stack_t) {
            if (calling_stack_pos) {
                list_del(&calling_stack_pos->func_head);
                if (calling_stack_pos->func_name) {
                    HAL_Free(calling_stack_pos->func_name);
                    calling_stack_pos->func_name = NULL;
                }
                HAL_Free(calling_stack_pos);
                calling_stack_pos = NULL;
            }
        }

        list_del(&table_pos->list);
        HAL_Free(table_pos);
        table_pos = NULL;
        ret = 0;
    }
    return ret;
}

int _count_malloc_internal(const char *f, const int l, OS_malloc_record *os_malloc_pos, va_list ap)
{
    int ret = -1;

    int magic = 0;
    char is_repeat = 0;
    char *module_name = NULL;
    module_mem_t *pos = NULL;
    calling_stack_t    *call_pos, *call_tmp;
    calling_stack_t    *entry = NULL;

    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    } else {
        module_name = "unknown";
    }

    pos = (module_mem_t *)_find_mem_table(module_name, &mem_module_statis);
    if (!pos) {
        if (NULL == (pos = (module_mem_t *)_create_mem_table(module_name, &mem_module_statis))) {
            utils_err("create_mem_table:[%s] failed!", module_name);
            return ret;
        }
    }
    os_malloc_pos->mem_table = (void *)pos;

    if (!strcmp(pos->mem_statis.module_name, "unknown")) {
        list_for_each_entry_safe(call_pos, call_tmp, &pos->mem_statis.calling_stack.func_head, func_head, calling_stack_t) {
            if (!strcmp(call_pos->func_name, f) && (call_pos->line == l)) {
                is_repeat = 1;
                break;
            }
        }
        if (!is_repeat) {
            entry = HAL_Malloc(sizeof(calling_stack_t));
            if (!entry) {
                return ret;
            }

            memset(entry, 0, sizeof(calling_stack_t));
            entry->func_name = LITE_strdup(f);
            entry->line = l;
            list_add(&entry->func_head, &pos->mem_statis.calling_stack.func_head);
        }
    }

    pos->mem_statis.iterations_allocated += 1;
    pos->mem_statis.bytes_total_allocated += os_malloc_pos->buflen;
    pos->mem_statis.bytes_total_in_use += os_malloc_pos->buflen;
    pos->mem_statis.bytes_max_in_use = (pos->mem_statis.bytes_max_in_use > pos->mem_statis.bytes_total_in_use) ?
                                       pos->mem_statis.bytes_max_in_use : pos->mem_statis.bytes_total_in_use;
    pos->mem_statis.bytes_max_allocated = (pos->mem_statis.bytes_max_allocated >= os_malloc_pos->buflen) ?
                                          pos->mem_statis.bytes_max_allocated : os_malloc_pos->buflen;

    pos->mem_statis.iterations_in_use += 1;
    pos->mem_statis.iterations_max_in_use = (pos->mem_statis.iterations_in_use > pos->mem_statis.iterations_max_in_use) ?
                                            pos->mem_statis.iterations_in_use : pos->mem_statis.iterations_max_in_use;
    ret = 0;

    return ret;
}

void _count_free_internal(void *ptr, OS_malloc_record *os_malloc_pos)
{

    module_mem_t *pos = NULL;

    pos = (module_mem_t *)(os_malloc_pos->mem_table);
    if (!pos) {
        utils_err("find mem_table faild");
        return;
    }

    pos->mem_statis.iterations_freed += 1;
    pos->mem_statis.iterations_in_use -= 1;

    pos->mem_statis.bytes_total_freed += os_malloc_pos->buflen;
    pos->mem_statis.bytes_total_in_use -= os_malloc_pos->buflen;
}

void *LITE_malloc_internal(const char *f, const int l, int size, ...)
{
    void                   *ptr = NULL;
    OS_malloc_record       *pos;

    if (size <= 0) {
        return NULL;
    }

    ptr = HAL_Malloc(size);
    if (!ptr) {
        return NULL;
    }

    if (mutex_mem_stats == NULL) {
        mutex_mem_stats = HAL_MutexCreate();
    }
    HAL_MutexLock(mutex_mem_stats);

    iterations_allocated += 1;
    bytes_total_allocated += size;
    bytes_total_in_use += size;
    bytes_max_in_use = (bytes_max_in_use > bytes_total_in_use) ? bytes_max_in_use : bytes_total_in_use;
    bytes_max_allocated = (bytes_max_allocated >= size) ? bytes_max_allocated : size;

#if defined(WITH_TOTAL_COST_WARNING)
    if (bytes_total_in_use > WITH_TOTAL_COST_WARNING) {
        utils_debug(" ");
        utils_debug("==== PRETTY HIGH TOTAL IN USE: %d BYTES ====", bytes_total_in_use);
        LITE_dump_malloc_free_stats(LOG_DEBUG_LEVEL);
    }
#endif

    iterations_in_use += 1;
    iterations_max_in_use = (iterations_in_use > iterations_max_in_use) ? iterations_in_use : iterations_max_in_use;

    pos = HAL_Malloc(sizeof(OS_malloc_record));
    if (NULL == pos) {
        HAL_Free(ptr);
        HAL_MutexUnlock(mutex_mem_stats);
        return NULL;
    }
    memset(pos, 0, sizeof(OS_malloc_record));

    pos->buf = ptr;
    pos->buflen = size;
    pos->func = (char *)f;
    pos->line = (int)l;
#if defined(__UBUNTU_SDK_DEMO__)
    if (tracking_malloc_callstack) {
        record_backtrace(&pos->bt_level, &pos->bt_symbols);
    }
#endif

    list_add_tail(&pos->list, &mem_recs);

    {
        va_list ap;
        va_start(ap, size);
        _count_malloc_internal(f, l, pos, ap);
        va_end(ap);
    }

#if defined(WITH_ALLOC_WARNING_THRESHOLD)
    if (size > WITH_ALLOC_WARNING_THRESHOLD) {
        int             k;

        log_warning("utils", "large allocating @ %s(%d) for %04d bytes!", f, l, size);
        LITE_printf("\r\n");
#if defined(__UBUNTU_SDK_DEMO__)
        for (k = 0; k < pos->bt_level; ++k) {
            int             m;
            const char     *p = strchr(pos->bt_symbols[k], '(');

            if (p[1] == ')') {
                continue;
            }
            for (m = 0; m < k; ++m) {
                LITE_printf("  ");
            }

            LITE_printf("%s\r\n", p);
        }
#endif
        LITE_printf("\r\n");
    }
#endif
    memset(ptr, 0, size);
    HAL_MutexUnlock(mutex_mem_stats);
    return ptr;
}

void LITE_free_internal(void *ptr)
{
    OS_malloc_record       *pos;
    OS_malloc_record       *next;
    int                     found = 0;

    if (!ptr) {
        return;
    }

    HAL_MutexLock(mutex_mem_stats);

    pos = NULL;
    list_for_each_entry_safe(pos, next, &mem_recs, list, OS_malloc_record) {
        if (pos->buf == ptr) {
            found = 1;
            break;
        }
    }

    if (!found) {
        pos = NULL;
    }

    if (NULL == pos) {
        log_warning("utils", "Cannot find %p allocated! Skip stat ...", ptr);

        HAL_MutexUnlock(mutex_mem_stats);
        return;
    } else {
        iterations_freed += 1;
        iterations_in_use -= 1;

        bytes_total_freed += pos->buflen;
        bytes_total_in_use -= pos->buflen;
        _count_free_internal(ptr, pos);

        if (pos->buf && pos->buflen > 0) {
            memset(pos->buf, 0xEE, pos->buflen);
        }
        pos->buf = 0;
        pos->buflen = 0;
        pos->func = "";
        pos->line = 0;
#if defined(__UBUNTU_SDK_DEMO__)
        pos->bt_level = 0;
        HAL_Free(pos->bt_symbols);
        pos->bt_symbols = 0;
#endif

        list_del(&pos->list);
        HAL_Free(pos);
    }
    HAL_MutexUnlock(mutex_mem_stats);
    HAL_Free(ptr);
}

void *LITE_malloc_routine(int size, ...)
{
    int magic = 0;
    char *module_name = NULL;

    va_list ap;
    va_start(ap, size);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);

    return LITE_malloc(size, magic, module_name);
}

void *LITE_calloc_routine(size_t n, size_t s, ...)
{
    int magic = 0;
    char *module_name = NULL;

    va_list ap;
    va_start(ap, s);
    magic = va_arg(ap, int);
    if (MEM_MAGIC == magic) {
        module_name = va_arg(ap, char *);
    }
    va_end(ap);

    return LITE_malloc(n * s, magic, module_name);;
}

void LITE_free_routine(void *ptr)
{
    LITE_free(ptr);
}

void LITE_dump_malloc_free_stats(int level)
{
    OS_malloc_record       *pos;
    module_mem_t *module_pos, *tmp;
    module_mem_t *unknown_mod = NULL;

    if (level > LITE_get_loglevel()) {
        return;
    }

    utils_debug("");
    utils_debug("---------------------------------------------------");
    utils_debug(". bytes_total_allocated:    %d", bytes_total_allocated);
    utils_debug(". bytes_total_freed:        %d", bytes_total_freed);
    utils_debug(". bytes_total_in_use:       %d", bytes_total_in_use);
    utils_warning(". bytes_max_allocated:      %d", bytes_max_allocated);
    utils_info(". bytes_max_in_use:         %d", bytes_max_in_use);
    utils_debug(". iterations_allocated:     %d", iterations_allocated);
    utils_debug(". iterations_freed:         %d", iterations_freed);
    utils_debug(". iterations_in_use:        %d", iterations_in_use);
    utils_debug(". iterations_max_in_use:    %d", iterations_max_in_use);
    utils_debug("---------------------------------------------------");
    utils_debug("");

    _mem_sort_module_pos(&mem_module_statis);

    LITE_printf("\r\n");
    LITE_printf("|               |  max_in_use          |  max_allocated   |  total_allocated      |  total_free\r\n");
    LITE_printf("|---------------|----------------------|------------------|-----------------------|----------------------\r\n");
    list_for_each_entry_safe(module_pos, tmp, &mem_module_statis, list, module_mem_t) {
        if (module_pos) {

            LITE_printf("| %-13s | %6d bytes / %-5d |    %6d bytes  | %6d bytes / %-5d  | %6d bytes / %-5d     \r\n",
                        module_pos->mem_statis.module_name,
                        module_pos->mem_statis.bytes_max_in_use,
                        module_pos->mem_statis.iterations_max_in_use,
                        module_pos->mem_statis.bytes_max_allocated,
                        module_pos->mem_statis.bytes_total_allocated,
                        module_pos->mem_statis.iterations_allocated,
                        module_pos->mem_statis.bytes_total_freed,
                        module_pos->mem_statis.iterations_freed
                       );
            /*
                        LITE_printf("\x1B[1;32mMODULE_NAME: [%s]\x1B[0m\r\n", module_pos->mem_statis.module_name);
                        LITE_printf("---------------------------------------------------\r\n");
                        LITE_printf(". bytes_total_allocated:    %d\r\n", module_pos->mem_statis.bytes_total_allocated);
                        LITE_printf(". bytes_total_freed:        %d\r\n", module_pos->mem_statis.bytes_total_freed);
                        LITE_printf(". bytes_total_in_use:       %d\r\n", module_pos->mem_statis.bytes_total_in_use);
                        LITE_printf(". bytes_max_allocated:      %d\r\n", module_pos->mem_statis.bytes_max_allocated);
                        LITE_printf(". bytes_max_in_use:         %d\r\n", module_pos->mem_statis.bytes_max_in_use);
                        LITE_printf(". iterations_allocated:     %d\r\n", module_pos->mem_statis.iterations_allocated);
                        LITE_printf(". iterations_freed:         %d\r\n", module_pos->mem_statis.iterations_freed);
                        LITE_printf(". iterations_in_use:        %d\r\n", module_pos->mem_statis.iterations_in_use);
                        LITE_printf(". iterations_max_in_use:    %d\r\n", module_pos->mem_statis.iterations_max_in_use);
                        LITE_printf("---------------------------------------------------\r\n");
            */
            if (!strcmp(module_pos->mem_statis.module_name, "unknown")) {
                unknown_mod = module_pos;
            } else {
                HAL_Free(module_pos->mem_statis.calling_stack.func_name);
            }
        }
    }

    if (unknown_mod) {
        calling_stack_t *call_pos, *tmp;
        LITE_printf("\r\n");
        LITE_printf("\x1B[1;33mMissing module-name references:\x1B[0m\r\n");
        LITE_printf("---------------------------------------------------\r\n");

        list_for_each_entry_safe(call_pos, tmp, &unknown_mod->mem_statis.calling_stack.func_head, func_head, calling_stack_t) {
            if (call_pos->func_name) {
                LITE_printf(". \x1B[1;31m%s \x1B[0m Ln:%d\r\n", call_pos->func_name, call_pos->line);

                /* free memery of func_name and calling_stack */
                HAL_Free(call_pos->func_name);
                list_del(&call_pos->func_head);
                HAL_Free(call_pos);
            }
        }
        LITE_printf("\r\n");
    }

    list_for_each_entry_safe(module_pos, tmp, &mem_module_statis, list, module_mem_t) {
        list_del(&module_pos->list);
        HAL_Free(module_pos);
    }

    LITE_printf("\r\n");

    if (LITE_get_loglevel() == level) {
        int         j;
        int         cnt = 0;

        list_for_each_entry(pos, &mem_recs, list, OS_malloc_record) {
            if (pos->buf) {
                LITE_printf("%4d. %-24s Ln:%-5d @ %p: %4d bytes [",
                            ++cnt,
                            pos->func,
                            pos->line,
                            pos->buf,
                            pos->buflen);
                for (j = 0; j < 32 && j < pos->buflen; ++j) {
                    char        c;

                    c = *(char *)(pos->buf + j);
                    if (c < ' ' || c > '~') {
                        c = '.';
                    }
                    LITE_printf("%c", c);
                }
                LITE_printf("]\r\n");

#if defined(__UBUNTU_SDK_DEMO__)
                {
                    int             k;
                    LITE_printf("\r\n");
                    for (k = 0; k < pos->bt_level; ++k) {
                        int             m;
                        const char     *p = strchr(pos->bt_symbols[k], '(');

                        if (p[1] == ')') {
                            continue;
                        }
                        LITE_printf("    ");
                        for (m = 0; m < k; ++m) {
                            LITE_printf("  ");
                        }

                        LITE_printf("%s\r\n", p);
                    }
                }
#endif
                LITE_printf("\r\n");
            }
        }
    }

    return;
}

void **LITE_get_mem_mutex(void)
{
    return &mutex_mem_stats;
}
#endif
