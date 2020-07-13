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

#include "stdlib.h"
#include "string.h"
#include "stack.h"

int stack_create(stack_t *stack, int element_max)
{
    e_type_t *elements;

    if (!stack || element_max <= 0) {
        return -1;
    }

    memset(stack, 0, sizeof(stack_t));

    if ((elements = malloc(element_max * sizeof(e_type_t))) == NULL) {
        return -1;
    }

    stack->top          = 0;
    stack->element_max  = element_max;
    stack->elements     = elements;

    return 0;
}

int stack_destroy(stack_t *stack)
{
    if (!stack || !stack->elements) {
        return -1;
    }

    free(stack->elements);

    return 0;
}

int stack_push(stack_t *stack, e_type_t element)
{
    if (!stack || !stack->elements) {
        return -1;
    }

    if (stack_is_full(stack)) {
        return -1;
    }

    stack->elements[stack->top++] = element;

    return 0;
}

e_type_t stack_pop(stack_t *stack)
{
    if (!stack || !stack->elements) {
        return -1;
    }

    if (stack_is_empty(stack)) {
        return -1;
    }

    return stack->elements[--stack->top];
}

e_type_t stack_top(stack_t *stack)
{
    if (!stack || !stack->elements) {
        return -1;
    }

    if (stack_is_empty(stack)) {
        return -1;
    }

    return stack->elements[stack->top - 1];
}


int stack_peek_init(stack_t *stack)
{
    if (!stack || !stack->elements) {
        return -1;
    }

    stack->peek_top = stack->top;

    return 0;
}

e_type_t stack_peek(stack_t *stack)
{
    if (!stack || !stack->elements) {
        return -1;
    }

    if (stack->peek_top == 0) {
        return -1;
    }

    return stack->elements[--stack->peek_top];
}

int stack_is_empty(stack_t *stack)
{
    if (!stack || !stack->elements) {
        return 0;
    }

    return stack->top == 0;
}

int stack_is_full(stack_t *stack)
{
    if (!stack || !stack->elements) {
        return 0;
    }

    return stack->top == stack->element_max;
}

