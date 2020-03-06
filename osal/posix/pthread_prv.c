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

#include "tos_k.h"

#include "pthread.h"
#include "private/pthread.h"

__STATIC__ k_mutex_t pthread_mutex;

__STATIC__ pthread_ctl_t *thread_ctl_table[PTHREAD_THREADS_MAX] = { 0 };

__STATIC__ pthread_key_ctl_t pthread_key_ctl;

__KNL__ pthread_ctl_t *pthread_ctl_self(void)
{
    TOS_CPU_CPSR_ALLOC();
    int i = 0;
    k_task_t *self_task;
    pthread_ctl_t *the_info;

    self_task = tos_task_curr_task_get();

    TOS_CPU_INT_DISABLE();

    for (i = 0; i < TOS_COUNT_OF(thread_ctl_table); ++i) {
        the_info = thread_ctl_table[i];
        if (the_info && the_info->the_ktask == self_task) {
            TOS_CPU_INT_ENABLE();
            return the_info;
        }
    }

    TOS_CPU_INT_ENABLE();
    return K_NULL;
}

__KNL__ pthread_ctl_t *pthread_ctl_by_id(pthread_t id)
{
    pthread_ctl_t *the_ctl;

    the_ctl = thread_ctl_table[id];

    if (!the_ctl) {
        return K_NULL;
    }

    if (the_ctl->id != id) {
        return K_NULL;
    }

    if (the_ctl->threadstate == PTHREAD_STATE_EXITED) {
        return K_NULL;
    }

    return the_ctl;
}

__KNL__ int pthread_id_add(pthread_t id, pthread_ctl_t *info)
{
    TOS_CPU_CPSR_ALLOC();

    if (id < 0 ||
        id >= TOS_COUNT_OF(thread_ctl_table) ||
        thread_ctl_table[id]) {
        return -1;
    }

    TOS_CPU_INT_DISABLE();

    thread_ctl_table[id] = info;

    TOS_CPU_INT_ENABLE();

    return 0;
}

__KNL__ pthread_t pthread_id_alloc(void)
{
    TOS_CPU_CPSR_ALLOC();
    int i = 0;

    TOS_CPU_INT_DISABLE();

    for (i = 0; i < TOS_COUNT_OF(thread_ctl_table); ++i) {
        if (!thread_ctl_table[i]) {
            TOS_CPU_INT_ENABLE();
            return (pthread_t)i;
        }
    }

    TOS_CPU_INT_ENABLE();
    return -1;
}

__KNL__ int pthread_id_free(pthread_t id)
{
    TOS_CPU_CPSR_ALLOC();

    if (id < 0 ||
        id >= TOS_COUNT_OF(thread_ctl_table) ||
        !thread_ctl_table[id]) {
        return -1;
    }

    TOS_CPU_INT_DISABLE();

    thread_ctl_table[id] = K_NULL;

    TOS_CPU_INT_ENABLE();

    return 0;
}

__KNL__ void pthread_data_clear(pthread_key_t key)
{
    int i = 0;
    pthread_ctl_t *the_ctl;

    for (i = 0; i < TOS_COUNT_OF(thread_ctl_table); ++i) {
        the_ctl = thread_ctl_table[i];
        if (the_ctl && the_ctl->thread_data) {
            the_ctl->thread_data[key] = K_NULL;
        }
    }
}

__STATIC__ int pthread_key_ctl_init(void)
{
    int i = 0;

    if (tos_bitmap_create_full(&pthread_key_ctl.key_bitmap,
                                    pthread_key_ctl.key_bitmap_tbl,
                                    PTHREAD_KEYS_MAX) != K_ERR_NONE) {
        return -1;
    }

    for (i = 0; i < PTHREAD_KEYS_MAX; ++i) {
        pthread_key_ctl.destructors[i] = K_NULL;
    }

    return 0;
}

__KNL__ pthread_key_t pthread_key_alloc(void)
{
    int lsb;

    lsb = tos_bitmap_lsb(&pthread_key_ctl.key_bitmap);

    if (lsb > PTHREAD_KEYS_MAX) {
        return -1;
    }

    tos_bitmap_reset(&pthread_key_ctl.key_bitmap, lsb);

    return (pthread_key_t)lsb;
}

__KNL__ int pthread_key_is_alloc(pthread_key_t key)
{
    if (key > PTHREAD_KEYS_MAX || key < 0) {
        return K_FALSE;
    }

    return tos_bitmap_is_reset(&pthread_key_ctl.key_bitmap, key);
}

__KNL__ int pthread_key_free(pthread_key_t key)
{
    if (key > PTHREAD_KEYS_MAX || key < 0) {
        return -1;
    }

    if (tos_bitmap_is_set(&pthread_key_ctl.key_bitmap, key)) {
        /* what we created is a full bitmap, if the bit is set means it is not used */
        return -1;
    }

    /* make it avaliable again */
    tos_bitmap_set(&pthread_key_ctl.key_bitmap, key);

    return 0;
}

__KNL__ int pthread_key_destructor_register(pthread_key_t key, key_destructor_t destructor)
{
    if (key > PTHREAD_KEYS_MAX || key < 0) {
        return -1;
    }

   if (tos_bitmap_is_set(&pthread_key_ctl.key_bitmap, key)) {
        /* what we created is a full bitmap, if the bit is set means it is not used */
        return -1;
    }

    pthread_key_ctl.destructors[key] = destructor;

    return 0;
}

__STATIC__ int pthread_key_destructor_is_register(pthread_key_t key)
{
    if (key > PTHREAD_KEYS_MAX || key < 0) {
        return K_FALSE;
    }

   if (tos_bitmap_is_set(&pthread_key_ctl.key_bitmap, key)) {
        /* what we created is a full bitmap, if the bit is set means it is not used */
        return K_FALSE;
    }

    return pthread_key_ctl.destructors[key] != K_NULL;
}

__KNL__ key_destructor_t pthread_key_destructor_get(pthread_key_t key)
{
    if (!pthread_key_destructor_is_register(key)) {
        return K_NULL;
    }

    return pthread_key_ctl.destructors[key];
}

__KNL__ int pthread_ctl_reap(int pthreads_ready2reap)
{
    int i = 0;
    pthread_ctl_t *the_ctl;
    int pthreads_reaped = 0;

    if (pthreads_ready2reap == 0) {
        return 0;
    }

    for (i = 0; pthreads_ready2reap && i < TOS_COUNT_OF(thread_ctl_table); ++i) {
        the_ctl = thread_ctl_table[i];
        if (!the_ctl || the_ctl->threadstate != PTHREAD_STATE_EXITED) {
            continue;
        }

        pthread_id_free((pthread_t)i);

        tos_sem_destroy(&the_ctl->joinner_sem);

        if (the_ctl->stackaddr) {
            /* the_ctl is just on this stack */
            tos_mmheap_free(the_ctl->stackaddr);
        }

        --pthreads_ready2reap;
        ++pthreads_reaped;
    }

    return pthreads_reaped;
}

__KNL__ void pthread_lock(void)
{
    tos_mutex_pend(&pthread_mutex);
}

__KNL__ void pthread_unlock(void)
{
    tos_mutex_post(&pthread_mutex);
}

__STATIC__ int pthread_lock_init(void)
{
    if (tos_mutex_create(&pthread_mutex) != K_ERR_NONE) {
        return -1;
    }

    return 0;
}

__KNL__ int pthread_init(void)
{
    if (pthread_lock_init() != 0) {
        return -1;
    }

    if (pthread_key_ctl_init() != 0) {
        return -1;
    }

    return 0;
}

