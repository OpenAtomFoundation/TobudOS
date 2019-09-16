#ifndef _TOS_KLIB_H_
#define  _TOS_KLIB_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define TOS_OFFSET_OF_FIELD(type, field) \
    ((uint32_t)&(((type *)0)->field))

#define TOS_CONTAINER_OF_FIELD(ptr, type, field) \
    ((type *)((uint8_t *)(ptr) - TOS_OFFSET_OF_FIELD(type, field)))

#define TOS_PTR_SANITY_CHECK(ptr) \
    do {    \
        if (unlikely((ptr) == K_NULL)) {    \
            return K_ERR_OBJ_PTR_NULL;    \
        }   \
    } while(0)

#define TOS_IN_IRQ_CHECK()    \
    do {    \
        if (unlikely(knl_is_inirq())) {   \
            return K_ERR_IN_IRQ; \
        }   \
    } while(0)

// currently we use default microlib supplied by mdk
#define tos_kprintf(...)         printf(__VA_ARGS__);

#define tos_kprintln(...)   \
    printf(__VA_ARGS__); \
    printf("\n");

#endif /* _TOS_KLIB_H_ */

