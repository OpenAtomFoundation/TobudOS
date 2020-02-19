/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-23 19:26:27
 * @LastEditTime : 2020-02-15 23:32:25
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_thread.h"
#include "platform_memory.h"

platform_thread_t *platform_thread_init( const char *name,
                                        void (*entry)(void *),
                                        void * const param,
                                        unsigned int stack_size,
                                        unsigned int priority,
                                        unsigned int tick)
{
    platform_thread_t *thread;
    k_err_t err;
    k_stack_t *thread_stack;
    thread = platform_memory_alloc(sizeof(platform_thread_t));
    thread_stack = (k_stack_t*) platform_memory_alloc(stack_size);
    
    err = tos_task_create(&(thread->thread), 
                          (char*)name, 
                          entry,
                          param, 
                          priority, 
                          thread_stack,
                          stack_size,
                          tick);
    
    if(err != K_ERR_NONE) {
        platform_memory_free(thread);
        platform_memory_free(thread_stack);
    }

    return thread;
}

void platform_thread_destroy(platform_thread_t* thread)
{
    if (NULL != thread)
        tos_task_destroy(&(thread->thread));
    platform_memory_free(&(thread->thread));
    platform_memory_free(&(thread->thread.stk_size));
}


