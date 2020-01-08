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
#include <string.h>

#define TOS_OFFSET_OF_FIELD(type, field) \
    ((uint32_t)&(((type *)0)->field))

#define TOS_CONTAINER_OF_FIELD(ptr, type, field) \
    ((type *)((uint8_t *)(ptr) - TOS_OFFSET_OF_FIELD(type, field)))

#define TOS_PTR_SANITY_CHECK(ptr) \
    do {    \
        if (unlikely(!(ptr))) {    \
            return K_ERR_OBJ_PTR_NULL;    \
        }   \
    } while(0)

#define TOS_PTR_SANITY_CHECK_RC(ptr, return_code) \
    do {    \
        if (unlikely(!(ptr))) {    \
            return return_code;    \
        }   \
    } while(0)

#define TOS_IN_IRQ_CHECK()    \
    do {    \
        if (unlikely(knl_is_inirq())) {   \
            return K_ERR_IN_IRQ; \
        }   \
    } while(0)

#if TOS_CFG_OBJECT_VERIFY_EN > 0u

#define TOS_OBJ_INIT(obj, obj_type)     knl_object_init(&obj->knl_obj, obj_type)
#define TOS_OBJ_DEINIT(obj)             knl_object_deinit(&obj->knl_obj)

#define TOS_OBJ_VERIFY(obj, obj_type) \
    do {    \
        if (!knl_object_verify(&obj->knl_obj, obj_type)) {    \
            return K_ERR_OBJ_INVALID;   \
        }   \
    } while (0)

#define TOS_OBJ_VERIFY_RC(obj, obj_type, return_code) \
    do {    \
        if (!knl_object_verify(&obj->knl_obj, obj_type)) {    \
            return return_code;   \
        }   \
    } while (0)

#else

#define TOS_OBJ_INIT(obj, obj_type)
#define TOS_OBJ_DEINIT(obj)
#define TOS_OBJ_VERIFY(obj, obj_type)
#define TOS_OBJ_VERIFY_RC(obj, obj_type, return_code)

#endif

#if TOS_CFG_LIBC_PRINTF_EN > 0u
#define LIBC_PRINTF             printf
#else
#define LIBC_PRINTF(...)
#endif

// currently we use default microlib supplied by mdk
#define tos_kprintf(...)         LIBC_PRINTF(__VA_ARGS__);

#define tos_kprintln(...)   \
    LIBC_PRINTF(__VA_ARGS__); \
    LIBC_PRINTF("\n");

#define TOS_ASSERT_AUX(exp, function, line) \
    if (!(exp)) { \
        tos_kprintln("assert failed: %s %d\n", function, line); \
        tos_knl_sched_lock(); \
        tos_cpu_int_disable(); \
        while (K_TRUE) { \
            ; \
        } \
    }

#define TOS_ASSERT(exp) TOS_ASSERT_AUX(exp, __FUNCTION__, __LINE__)

#endif /* _TOS_KLIB_H_ */

