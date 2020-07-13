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

#ifndef _STACK_H_
#define  _STACK_H_

typedef int                     element_type_t;
typedef element_type_t          e_type_t;

typedef struct stack_st {
    e_type_t   *elements;
    int         element_max;

    int         peek_top;
    int         top;
} stack_t;

int stack_create(stack_t *stack, int element_max);

int stack_destroy(stack_t *stack);

int stack_push(stack_t *stack, e_type_t element);

e_type_t stack_pop(stack_t *stack);

e_type_t stack_top(stack_t *stack);

int stack_peek_init(stack_t *stack);

e_type_t stack_peek(stack_t *stack);

int stack_is_empty(stack_t *stack);

int stack_is_full(stack_t *stack);

#endif

