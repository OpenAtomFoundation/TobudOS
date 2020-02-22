/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-26 19:11:34
 * @LastEditTime : 2019-12-28 01:51:38
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "config.h"


void *salof_alloc(unsigned int size)
{
    return tos_mmheap_alloc(size);
}


void salof_free(void *mem)
{
    tos_mmheap_free(mem);
}

salof_tcb salof_task_create(const char *name,
                            void (*task_entry)(void *param),
                            void * const param,
                            unsigned int stack_size,
                            unsigned int priority,
                            unsigned int tick)
{
    salof_tcb task;
    k_err_t err;
    k_stack_t *task_stack;
    task = salof_alloc(sizeof(k_task_t));
    task_stack = salof_alloc(stack_size);
    err = tos_task_create(task, 
                          (char*)name, 
                          task_entry,
                          param, 
                          priority, 
                          task_stack,
                          stack_size,
                          tick);
    if(err != K_ERR_NONE)
    {
        tos_mmheap_free(task);
        tos_mmheap_free(task_stack);
    }

    return task;
}

salof_mutex salof_mutex_create(void)
{
    salof_mutex mutex;
    mutex = salof_alloc(sizeof(k_mutex_t));
	tos_mutex_create((salof_mutex)mutex);	
    return mutex;
}

void salof_mutex_delete(salof_mutex mutex)
{
	tos_mutex_destroy((salof_mutex)mutex);
    tos_mmheap_free(mutex);
}


int salof_mutex_pend(salof_mutex mutex, unsigned int timeout)
{

    if(tos_mutex_pend_timed((salof_mutex)mutex, timeout) != K_ERR_NONE)
        return -1;
    return 0;
}

int salof_mutex_post(salof_mutex mutex)
{
    if(tos_mutex_post((salof_mutex)mutex) != K_ERR_NONE)
        return -1;
    return 0;
}

salof_sem salof_sem_create(void)
{
    salof_sem sem;
    sem = salof_alloc(sizeof(k_sem_t));
	tos_sem_create((salof_sem)sem, 0);	
    return sem;
}

void salof_sem_delete(salof_sem sem)
{
	tos_sem_destroy((salof_sem)sem);
    tos_mmheap_free(sem);
}


int salof_sem_pend(salof_sem sem, unsigned int timeout)
{

    if(tos_sem_pend((salof_sem)sem, timeout) != K_ERR_NONE)
        return -1;
    return 0;
}

int salof_sem_post(salof_sem sem)
{
    if(tos_sem_post((salof_sem)sem) != K_ERR_NONE)
        return -1;
    return 0;
}


unsigned int salof_get_tick(void)
{
    return tos_systick_get();
}

char *salof_get_task_name(void)
{
    return k_curr_task->name;
}

