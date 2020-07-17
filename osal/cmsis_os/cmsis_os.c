#include <string.h>
#include "cmsis_os.h"

static k_prio_t priority_cmsis2knl(osPriority prio)
{
    if (prio == osPriorityError) {
        return K_TASK_PRIO_INVALID;
    }

    return (k_prio_t)(3 - prio);
}

static osPriority priority_knl2cmsis(k_prio_t prio)
{
    return (osPriority)(3 - prio);
}

static osStatus errno_knl2cmsis(k_err_t err)
{
    return err == K_ERR_NONE ? osOK : osErrorOS;
}

//  ==== Kernel Control Functions ====
osStatus osKernelStart(void)
{
    return errno_knl2cmsis(tos_knl_start());
}

/**
 * @brief Initialize the RTOS Kernel for creating objects.
 * @return status code that indicates the execution status of the function.
 */
osStatus osKernelInitialize(void)
{
    return errno_knl2cmsis(tos_knl_init());
}

/**
 * @brief Check if the RTOS kernel is already started.
 * @retval #0    RTOS is not started
 * @retval #1    RTOS is started.
 */
int32_t osKernelRunning(void)
{
    return tos_knl_is_running();
}

#if (defined (osFeature_SysTick)  &&  (osFeature_SysTick != 0))     // System Timer available

/**
 * @brief Get the RTOS kernel system timer counter
 * @return RTOS kernel system timer as 32-bit value
 */
uint32_t osKernelSysTick(void)
{
    return tos_systick_get();
}

#endif


//  ==== Thread Management ====

/**
 * @brief Create a thread and add it to Active Threads and set it to state READY.
 * @param[in]   thread_def  thread definition referenced with \ref osThread.
 * @param[in]   argument    pointer that is passed to the thread function as start argument.
 * @return  thread ID for reference by other functions or NULL in case of error.
 */
osThreadId osThreadCreate(const osThreadDef_t *thread_def, void *argument)
{
    k_err_t err;

    if (!thread_def) {
        return NULL;
    }

#if TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u
    if (!thread_def->stackbase && !thread_def->task) {
        k_task_t *task;
        err = tos_task_create_dyn(&task, thread_def->name, (k_task_entry_t)thread_def->pthread,
                                argument, priority_cmsis2knl(thread_def->tpriority),
                                thread_def->stacksize, thread_def->timeslice);
        return err == K_ERR_NONE ? task : NULL;
    }
#endif

    err = tos_task_create((k_task_t *)thread_def->task, thread_def->name, (k_task_entry_t)thread_def->pthread,
                            argument, priority_cmsis2knl(thread_def->tpriority), thread_def->stackbase,
                            thread_def->stacksize, thread_def->timeslice);

    return err == K_ERR_NONE ? thread_def->task : NULL;
}

/**
 * @brief Return the thread ID of the current running thread.
 * @return thread ID for reference by other functions or NULL in case of error.
 */
osThreadId osThreadGetId(void)
{
    return k_curr_task;
}

/**
 * @brief Terminate execution of a thread and remove it from Active Threads.
 * @param[in]   thread_id   thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
 * @return status code that indicates the execution status of the function.
 */
osStatus osThreadTerminate(osThreadId thread_id)
{
    return errno_knl2cmsis(tos_task_destroy((k_task_t *)thread_id));
}

/**
 * @brief Change priority of an active thread.
 * @param[in]   thread_id   thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
 * @param[in]   priority    new priority value for the thread function.
 * @return status code that indicates the execution status of the function.
 */
osStatus osThreadSetPriority(osThreadId thread_id, osPriority priority)
{
    return errno_knl2cmsis(tos_task_prio_change((k_task_t *)thread_id, priority_cmsis2knl(priority)));
}

/**
 * @brief Pass control to next thread that is in state READY.
 * @return status code that indicates the execution status of the function.
 */
osStatus osThreadYield(void)
{
    tos_task_yield();
    
    return osOK;
}

/**
 * @brief Get current priority of an active thread.
 * @param[in]   thread_id   thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
 * @return current priority value of the thread function.
 */
osPriority osThreadGetPriority(osThreadId thread_id)
{
    if (!thread_id) {
        return osPriorityError;
    }

    return priority_knl2cmsis(((k_task_t *)thread_id)->prio);
}

//  ==== Generic Wait Functions ====

/**
 * @brief Wait for Timeout (Time Delay)
 * @param[in]   millisec    time delay value
 * @return status code that indicates the execution status of the function.
 */
osStatus osDelay(uint32_t millisec)
{
    k_tick_t delay;

    if (millisec == osWaitForever) {
        delay = TOS_TIME_FOREVER;
    } else {
        delay = tos_millisec2tick(millisec);
    }

    return errno_knl2cmsis(tos_task_delay(delay));
}

#if TOS_CFG_TIMER_EN > 0
/**
 * @brief Create a timer.
 * @param[in]   timer_def   timer object referenced with \ref osTimer.
 * @param[in]   type        osTimerOnce for one-shot or osTimerPeriodic for periodic behavior.
 * @param[in]   argument    argument to the timer call back function.
 * @return timer ID for reference by other functions or NULL in case of error.
 */
osTimerId osTimerCreate(const osTimerDef_t *timer_def, os_timer_type type, void *argument)
{
    k_err_t err;
    k_opt_t opt = TOS_OPT_TIMER_ONESHOT;

    if (!timer_def) {
        return NULL;
    }

    if (type == osTimerPeriodic) {
        opt = TOS_OPT_TIMER_PERIODIC;
    }

    err = tos_timer_create((k_timer_t *)timer_def->timer, (k_tick_t)1u,
                        (k_tick_t)3000u, (k_timer_callback_t)timer_def->cb, argument, opt);
    return err == K_ERR_NONE ? timer_def->timer : NULL;
}

/**
 * @brief Start or restart a timer.
 * @param[in]   timer_id    timer ID obtained by \ref osTimerCreate.
 * @param[in]   millisec    time delay value of the timer.
 * @return status code that indicates the execution status of the function.
 */
osStatus osTimerStart(osTimerId timer_id, uint32_t millisec)
{
    k_timer_t *timer;
    k_tick_t delay;

    if (!timer_id) {
        return osErrorResource;
    }

    if (millisec == 0) {
        delay = (k_tick_t)1u;
    } else if (millisec == osWaitForever) {
        delay = TOS_TIME_FOREVER;
    } else {
        delay = tos_millisec2tick(millisec);
    }

    timer = (k_timer_t *)timer_id;
    timer->delay    = delay;
    timer->period   = delay;

    return errno_knl2cmsis(tos_timer_start(timer));
}

/**
 * @brief Stop the timer.
 * @param[in]   timer_id    timer ID obtained by \ref osTimerCreate.
 * @return status code that indicates the execution status of the function.
 */
osStatus osTimerStop(osTimerId timer_id)
{
    return errno_knl2cmsis(tos_timer_stop((k_timer_t *)timer_id));
}

/**
 * @brief Delete a timer that was created by \ref osTimerCreate.
 * @param[in]   timer_id    timer ID obtained by \ref osTimerCreate.
 * @return status code that indicates the execution status of the function.
 */
osStatus osTimerDelete(osTimerId timer_id)
{
    return errno_knl2cmsis(tos_timer_destroy((k_timer_t *)timer_id));
}

#endif // TOS_CFG_TIMER_EN

#if TOS_CFG_MUTEX_EN > 0
/**
 * @brief Create and Initialize a Mutex object
 * @param[in]   mutex_def   mutex definition referenced with \ref osMutex.
 * @return mutex ID for reference by other functions or NULL in case of error.
 */
osMutexId osMutexCreate(const osMutexDef_t *mutex_def)
{
    k_err_t err;

    if (!mutex_def) {
        return NULL;
    }

    err = tos_mutex_create((k_mutex_t *)mutex_def->mutex);
    return err == K_ERR_NONE ? mutex_def->mutex : NULL;
}

/**
 * @brief Wait until a Mutex becomes available
 * @param[in]   mutex_id    mutex ID obtained by \ref osMutexCreate.
 * @param[in]   millisec    timeout value or 0 in case of no time-out.
 * @return status code that indicates the execution status of the function.
 */
osStatus osMutexWait(osMutexId mutex_id, uint32_t millisec)
{
    k_tick_t timeout;

    if (millisec == osWaitForever) {
        timeout = TOS_TIME_FOREVER;
    } else {
        timeout = tos_millisec2tick(millisec);
    }

    return errno_knl2cmsis(tos_mutex_pend_timed((k_mutex_t *)mutex_id, timeout));
}

/**
 * @brief Release a Mutex that was obtained by \ref osMutexWait
 * @param[in]   mutex_id    mutex ID obtained by \ref osMutexCreate.
 * @return status code that indicates the execution status of the function.
 */
osStatus osMutexRelease(osMutexId mutex_id)
{
    return errno_knl2cmsis(tos_mutex_post((k_mutex_t *)mutex_id));
}

/**
 * @brief Delete a Mutex that was created by \ref osMutexCreate.
 * @param[in]   mutex_id    mutex ID obtained by \ref osMutexCreate.
 * @return status code that indicates the execution status of the function.
 */
osStatus osMutexDelete(osMutexId mutex_id)
{
    return errno_knl2cmsis(tos_mutex_destroy((k_mutex_t *)mutex_id));
}

#endif // TOS_CFG_MUTEX_EN

#if TOS_CFG_SEM_EN > 0
/**
 * @brief Create and Initialize a Semaphore object used for managing resources
 * @param[in]   semaphore_def   semaphore definition referenced with \ref osSemaphore.
 * @param[in]   count           number of available resources.
 * @return semaphore ID for reference by other functions or NULL in case of error.
 */
osSemaphoreId osSemaphoreCreate(const osSemaphoreDef_t *semaphore_def, int32_t count)
{
    k_err_t err;

    if (!semaphore_def) {
        return NULL;
    }

    err = tos_sem_create((k_sem_t *)semaphore_def->sem, count);
    return err == K_ERR_NONE ? (k_sem_t *)semaphore_def->sem : NULL;
}

/**
 * @brief Wait until a Semaphore token becomes available
 * @param[in]   semaphore_id    semaphore object referenced with \ref osSemaphore.
 * @param[in]   millisec        timeout value or 0 in case of no time-out.
 * @return number of available tokens, or -1 in case of incorrect parameters.
 */
int32_t osSemaphoreWait(osSemaphoreId semaphore_id, uint32_t millisec)
{
    k_err_t err;
    k_tick_t timeout;

    if (millisec == osWaitForever) {
        timeout = TOS_TIME_FOREVER;
    } else {
        timeout = tos_millisec2tick(millisec);
    }

    err = tos_sem_pend((k_sem_t *)semaphore_id, timeout);

    return err == K_ERR_NONE ? ((k_sem_t *)semaphore_id)->count : -1;
}

/**
 * @brief Release a Semaphore token
 * @param[in]   semaphore_id    semaphore object referenced with \ref osSemaphore.
 * @return status code that indicates the execution status of the function.
 */
osStatus osSemaphoreRelease(osSemaphoreId semaphore_id)
{
    return errno_knl2cmsis(tos_sem_post((k_sem_t *)semaphore_id));
}

/**
 * @brief Delete a Semaphore that was created by \ref osSemaphoreCreate.
 * @param[in]   semaphore_id    semaphore object referenced with \ref osSemaphoreCreate.
 * @return status code that indicates the execution status of the function.
 */
osStatus osSemaphoreDelete(osSemaphoreId semaphore_id)
{
    return errno_knl2cmsis(tos_sem_destroy((k_sem_t *)semaphore_id));
}

#endif // TOS_CFG_SEM_EN

//  ==== Memory Pool Management Functions ====

#if (defined (osFeature_Pool)  &&  (osFeature_Pool != 0))  // Memory Pool Management available

/**
 * @brief Create and Initialize a memory pool
 * @param[in]   pool_def    memory pool definition referenced with \ref osPool.
 * @return memory pool ID for reference by other functions or NULL in case of error.
 */
osPoolId osPoolCreate(const osPoolDef_t *pool_def)
{
    k_err_t err;

    if (!pool_def) {
        return NULL;
    }

    err = tos_mmblk_pool_create((k_mmblk_pool_t *)pool_def->mmblk_pool,
                                    pool_def->pool, pool_def->pool_sz, pool_def->item_sz);
    return err == K_ERR_NONE ? pool_def->mmblk_pool : NULL;
}

/**
 * @brief Allocate a memory block from a memory pool
 * @param[in]   pool_id memory pool ID obtain referenced with \ref osPoolCreate.
 * @return address of the allocated memory block or NULL in case of no memory available.
 */
void *osPoolAlloc(osPoolId pool_id)
{
    k_err_t err;
    void *ptr = NULL;

    err = tos_mmblk_alloc((k_mmblk_pool_t *)pool_id, &ptr);
    return err == K_ERR_NONE ? ptr : NULL;
}

/**
 * @brief Allocate a memory block from a memory pool and set memory block to zero
 * @param[in]   pool_id memory pool ID obtain referenced with \ref osPoolCreate.
 * @return address of the allocated memory block or NULL in case of no memory available.
 */
void *osPoolCAlloc(osPoolId pool_id)
{
    k_err_t err;
    void *ptr = NULL;

    err = tos_mmblk_alloc((k_mmblk_pool_t *)pool_id, &ptr);
    if (err == K_ERR_NONE) {
        memset(ptr, 0, ((k_mmblk_pool_t *)pool_id)->blk_size);
        return ptr;
    }

    return NULL;
}

/**
 * @brief Return an allocated memory block back to a specific memory pool
 * @param[in]   pool_id memory pool ID obtain referenced with \ref osPoolCreate.
 * @param[in]   block   address of the allocated memory block that is returned to the memory pool.
 * @return status code that indicates the execution status of the function.
 */
osStatus osPoolFree(osPoolId pool_id, void *block)
{
    return errno_knl2cmsis(tos_mmblk_free((k_mmblk_pool_t *)pool_id, block));
}

#endif // Memory Pool Management available

#if TOS_CFG_MESSAGE_QUEUE_EN > 0u
//  ==== Message Queue Management Functions ====

#if (defined (osFeature_MessageQ) && (osFeature_MessageQ != 0))     // Message Queues available

/**
 * @brief Create and Initialize a Message Queue.
 * @param[in]   queue_def   queue definition referenced with \ref osMessageQ.
 * @param[in]   thread_id   thread ID (obtained by \ref osThreadCreate or \ref osThreadGetId) or NULL.
 * @return message queue ID for reference by other functions or NULL in case of error.
 */
osMessageQId osMessageCreate(const osMessageQDef_t *queue_def, osThreadId thread_id)
{
    k_err_t err;

    if (!queue_def) {
        return NULL;
    }

    thread_id = thread_id; // make compiler happy

    err = tos_msg_q_create((k_msg_q_t *)queue_def->queue, queue_def->pool, queue_def->queue_sz);
    return err == K_ERR_NONE ? (k_msg_q_t *)queue_def->queue : NULL;
}

/**
 * @brief Put a Message to a Queue.
 * @param[in]   queue_id    message queue ID obtained with \ref osMessageCreate.
 * @param[in]   info        message information.
 * @param[in]   millisec    timeout value or 0 in case of no time-out.
 * @return status code that indicates the execution status of the function.
 */
osStatus osMessagePut(osMessageQId queue_id, uint32_t info, uint32_t millisec)
{
    return errno_knl2cmsis(tos_msg_q_post((k_msg_q_t *)queue_id, (uint32_t*)info));
}

/**
 * @brief Get a Message or Wait for a Message from a Queue.
 * @param[in]   queue_id    message queue ID obtained with \ref osMessageCreate.
 * @param[in]   millisec    timeout value or 0 in case of no time-out.
 * @return event information that includes status code.
 */
osEvent osMessageGet(osMessageQId queue_id, uint32_t millisec)
{
    osEvent event;
    k_err_t err;
    k_tick_t timeout;
    void *msg_body = NULL;

    if (millisec == osWaitForever) {
        timeout = TOS_TIME_FOREVER;
    } else {
        timeout = tos_millisec2tick(millisec);
    }

    err = tos_msg_q_pend((k_msg_q_t *)queue_id, &msg_body, timeout);
    if (err == K_ERR_NONE) {
        event.def.message_id    = queue_id;
        event.status            = errno_knl2cmsis(err);
        event.value.v           = (uint32_t)msg_body;
    } else {
        event.def.message_id    = NULL;
        event.status            = osErrorOS;
    }

    return event;
}

#endif // Message Queues available
#endif // TOS_CFG_MESSAGE_QUEUE_EN

