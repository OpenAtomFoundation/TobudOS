/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

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

