/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 KY-zhang-X
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mpconfig.h"
#include "py/mpthread.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "tos_k.h"

#if MICROPY_PY_THREAD

#define MP_THREAD_MIN_STACK_SIZE                        (2 * 1024)
#define MP_THREAD_DEFAULT_STACK_SIZE                    (4 * 1024)

// this structure form a linked list, one node per active thread
typedef struct _mp_thread_t {
    k_task_t *id;
    int ready;
    void *arg;
    void *stack;
    size_t stack_len;
    struct _mp_thread_t *next;
    void *data;
} mp_thread_t;

// the mutex controls access to the linked list
STATIC mp_thread_mutex_t thread_mutex;
STATIC mp_thread_t thread_entry0;
STATIC mp_thread_t *thread = NULL;

void mp_thread_init(void)
{
    mp_thread_mutex_init(&thread_mutex);

    // create first entry in linked list of all threads
    thread = &thread_entry0;
    thread->id = tos_task_curr_task_get();
    thread->ready = 1;
    thread->arg = NULL;
    thread->stack = thread->id->stk_base;
    thread->stack_len = thread->id->stk_size / sizeof(uintptr_t);
    thread->next = NULL;
    // mp_thread_set_state(&mp_state_ctx.thread)
    thread->data = &mp_state_ctx.thread;
}

void mp_thread_gc_others(void)
{
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        gc_collect_root((void **)&th, 1);
        gc_collect_root(&th->arg, 1);
        gc_collect_root(&th->stack, 1);
        if (th->id != tos_task_curr_task_get() && th->ready) {
            gc_collect_root(th->stack, th->stack_len);
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_deinit(void)
{
    for (;;) {
        k_task_t *id = NULL;
        mp_thread_mutex_lock(&thread_mutex, 1);
        for (mp_thread_t *th = thread; th != NULL; th = th->next) {
            if (th->id != tos_task_curr_task_get()) {
                id = th->id;
                break;
            }
        }
        mp_thread_mutex_unlock(&thread_mutex);
        if (id == NULL) {
            break;
        } else {
            tos_task_destroy(id);
        }
    }
}

void mp_thread_set_state(mp_state_thread_t *state)
{
    mp_thread_mutex_lock(&thread_mutex, 1);
    k_task_t *id = tos_task_curr_task_get();
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == id) {
            th->data = (void *)state;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void)
{
    mp_thread_mutex_lock(&thread_mutex, 1);
    k_task_t *id = tos_task_curr_task_get();
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == id) {
            mp_thread_mutex_unlock(&thread_mutex);
            return (mp_state_thread_t *)th->data;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
    return NULL;
}


void mp_thread_create_ex(void *(*entry)(void *), void *arg, size_t *stack_size, int priority, char *name) {
    
    if (*stack_size == 0) {
        *stack_size = MP_THREAD_DEFAULT_STACK_SIZE;
    } else if (*stack_size < MP_THREAD_MIN_STACK_SIZE) {
        *stack_size = MP_THREAD_MIN_STACK_SIZE;
    }

    // Allocate linked-list node (must be outside thread_mutex lock)
    mp_thread_t *th = m_new_obj(mp_thread_t);

    mp_thread_mutex_lock(&thread_mutex, 1);

    // create thread
    k_err_t ret = tos_task_create_dyn(&th->id, name, (k_task_entry_t)entry, arg, priority, *stack_size, 0);
    if (ret != K_ERR_NONE) {
        mp_thread_mutex_unlock(&thread_mutex);
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("can't create thread"));
    }

    // add thread to linked list of all threads
    th->ready = 0;
    th->arg = arg;
    th->stack = th->id->stk_base;
    th->stack_len = *stack_size / sizeof(uintptr_t);
    th->next = thread;
    thread = th;

    // adjust the stack_size to provide room to recover from hitting the limit
    *stack_size -= MP_THREAD_STACK_LIMIT_MARGIN;

    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size)
{
    int priority = tos_task_curr_task_get()->prio;
    if (priority > 0) {
        // subthread has higher priority
        priority -= 1;
    }
    mp_thread_create_ex(entry, arg, stack_size, priority, "mp_thread");
}

void mp_thread_start(void)
{
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == tos_task_curr_task_get()) {
            th->ready = 1;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_finish(void)
{
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread, *prev = NULL; th != NULL; prev = th, th = th->next) {
        if (th->id == tos_task_curr_task_get()) {
            if (prev != NULL) {
                prev->next = th->next;
            } else {
                thread = th->next;
            }
            th->ready = 0;
            m_del_obj(mp_thread_t, th);
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
    tos_task_destroy_dyn(NULL);
}


void mp_thread_mutex_init(mp_thread_mutex_t *mutex)
{
    // TODO return value
    tos_mutex_create(mutex);
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait)
{
    return K_ERR_NONE == tos_mutex_pend_timed(mutex, wait ? TOS_TIME_FOREVER : TOS_TIME_NOWAIT);
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) 
{
    tos_mutex_post(mutex);
}

#endif /* MICROPY_PY_THREAD */
