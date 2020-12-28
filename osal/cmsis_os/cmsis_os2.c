#include "cmsis_os2.h"
#include <string.h>

static k_prio_t priority_cmsis2knl(osPriority_t prio) {
  if (prio == osPriorityError || prio == osPriorityISR ||
      prio == osPriorityReserved) {
    return K_TASK_PRIO_INVALID;
  }

  return (k_prio_t)(6 - prio / 8);
}

static osPriority_t priority_knl2cmsis(k_prio_t prio) {
  return (osPriority_t)((6 - prio) * 8);
}

static osStatus_t errno_knl2cmsis(k_err_t err) {
  osStatus_t state;
  switch (err) {
    case K_ERR_NONE:
      state = osOK;
      break;
    case K_ERR_IN_IRQ:
      state = osErrorISR;
      break;
    case K_ERR_OUT_OF_MEMORY:
      state = osErrorNoMemory;
      break;
    case K_ERR_PEND_TIMEOUT:
      state = osErrorTimeout;
      break;
    case K_ERR_PEND_DESTROY:
      state = osErrorResource;
      break;
    default:
      state = osError;
      break;
  }
  return state;
}

/*---------------------------------------------------------------------------*/

osStatus_t osKernelInitialize(void) {
  return errno_knl2cmsis(tos_knl_init());
}

osStatus_t osKernelGetInfo(osVersion_t* version,
                           char* id_buf,
                           uint32_t id_size) {
  if (version != NULL) {
    version->api = osCMSIS;
    version->kernel = osCMSIS_KERNEL;
  }

  if ((id_buf != NULL) && (id_size != 0U)) {
    if (id_size > sizeof(osKernelSystemId)) {
      id_size = sizeof(osKernelSystemId);
    }
    memcpy(id_buf, osKernelSystemId, id_size);
  }

  return osOK;
}

osKernelState_t osKernelGetState(void) {
  osKernelState_t state;

  switch (k_knl_state) {
    case KNL_STATE_RUNNING:
      state = osKernelRunning;
      break;
    default:
      state = osKernelInactive;
      break;
  }

  return state;
}

osStatus_t osKernelStart(void) {
  return errno_knl2cmsis(tos_knl_start());
}

int32_t osKernelLock(void) {
  tos_knl_sched_lock();
  if (knl_is_sched_locked()) {
    return 1;
  } else {
    return 0;
  }
}

int32_t osKernelUnlock(void) {
  tos_knl_sched_unlock();
  if (knl_is_sched_locked()) {
    return 1;
  } else {
    return 0;
  }
}

int32_t osKernelRestoreLock(int32_t lock) {
  if (lock == 1) {
    tos_knl_sched_lock();
  } else if (lock == 0) {
    tos_knl_sched_unlock();
  } else {
    return -1;
  }

  if (knl_is_sched_locked()) {
    return 1;
  } else {
    return 0;
  }
}

uint32_t osKernelSuspend(void) {
  // todo
  return 0;
}

void osKernelResume(uint32_t sleep_ticks) {
  (void)sleep_ticks;
  // todo
}

uint32_t osKernelGetTickCount(void) {
  return (uint32_t)tos_systick_get();
}

uint32_t osKernelGetTickFreq(void) {
  return TOS_CFG_CPU_TICK_PER_SECOND;
}

uint32_t osKernelGetSysTimerCount(void) {
  k_tick_t ticks;
  uint32_t val;

  ticks = osKernelGetTickCount();
  val = ticks * (TOS_CFG_CPU_CLOCK / TOS_CFG_CPU_TICK_PER_SECOND);

  return val;
}

uint32_t osKernelGetSysTimerFreq(void) {
  return TOS_CFG_CPU_CLOCK;
}

/*---------------------------------------------------------------------------*/

osThreadId_t osThreadNew(osThreadFunc_t func,
                         void* argument,
                         const osThreadAttr_t* attr) {
  k_err_t err;
  uint32_t stack_size = K_TASK_STK_SIZE_MIN;
  k_task_t* taskId = NULL;
  k_prio_t prio;

  if (attr && func) {
    if (attr->priority != osPriorityNone) {
      prio = priority_cmsis2knl(attr->priority);
    }
    if (attr->stack_size > 0U) {
      stack_size = attr->stack_size;
    }

    if (attr->stack_mem && attr->cb_mem) {
      err = tos_task_create((k_task_t*)attr->cb_mem, (char*)attr->name,
                            (k_task_entry_t)func, argument, prio,
                            attr->stack_mem, stack_size, 0);
      taskId = err == K_ERR_NONE ? attr->cb_mem : NULL;
    } else {
#if TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u
      k_task_t* task;
      err = tos_task_create_dyn(&task, attr->name, (k_task_entry_t)func,
                                argument, prio, stack_size, 0);
      taskId = err == K_ERR_NONE ? task : NULL;
#endif
    }
  }

  return (osThreadId_t)taskId;
}

const char* osThreadGetName(osThreadId_t thread_id) {
  k_task_t* taskId = (k_task_t*)thread_id;
  const char* name;

  if (taskId == NULL) {
    name = NULL;
  } else {
    name = taskId->name;
  }

  return name;
}

osThreadId_t osThreadGetId(void) {
  return (osThreadId_t)tos_task_curr_task_get();
}

osThreadState_t osThreadGetState(osThreadId_t thread_id) {
  k_task_t* taskId = (k_task_t*)thread_id;
  osThreadState_t state = osThreadError;

  if (taskId != NULL) {
    if (tos_task_curr_task_get() == taskId) {
      state = osThreadRunning;
    } else if (task_state_is_sleeping(taskId)) {
      state = osThreadBlocked;
    } else if (task_state_is_pending(taskId)) {
      state = osThreadBlocked;
    } else if (task_state_is_suspended(taskId)) {
      state = osThreadBlocked;
    } else if (task_state_is_ready(taskId)) {
      state = osThreadReady;
    } else if (taskId->state == K_TASK_STATE_DELETED) {
      state = osThreadTerminated;
    }
  }

  return state;
}

uint32_t osThreadGetStackSize(osThreadId_t thread_id) {
  k_task_t* taskId = (k_task_t*)thread_id;
  return taskId->stk_size;
}

uint32_t osThreadGetStackSpace(osThreadId_t thread_id) {
  (void)thread_id;
  // todo
  return 0;
}

osStatus_t osThreadSetPriority(osThreadId_t thread_id, osPriority_t priority) {
  k_task_t* taskId = (k_task_t*)thread_id;
  k_prio_t prio = priority_cmsis2knl(priority);
  osStatus_t state;

  if (taskId == NULL || prio == K_TASK_PRIO_INVALID) {
    state = osErrorParameter;
  } else {
    state = errno_knl2cmsis(tos_task_prio_change(taskId, prio));
  }

  return state;
}

osPriority_t osThreadGetPriority(osThreadId_t thread_id) {
  k_task_t* taskId = (k_task_t*)thread_id;
  osPriority_t prio;

  if (taskId == NULL) {
    prio = osPriorityError;
  } else {
    prio = priority_knl2cmsis(taskId->prio);
  }

  return prio;
}

osStatus_t osThreadYield(void) {
  osStatus_t state;

  if (knl_is_inirq()) {
    state = osErrorISR;
  } else {
    tos_task_yield();
    state = osOK;
  }

  return state;
}

osStatus_t osThreadSuspend(osThreadId_t thread_id) {
  k_task_t* taskId = (k_task_t*)thread_id;
  osStatus_t state;

  if (taskId == NULL) {
    state = osErrorParameter;
  } else {
    state = errno_knl2cmsis(tos_task_suspend(taskId));
  }

  return state;
}

osStatus_t osThreadResume(osThreadId_t thread_id) {
  k_task_t* taskId = (k_task_t*)thread_id;
  osStatus_t state;

  if (taskId == NULL) {
    state = osErrorParameter;
  } else {
    state = errno_knl2cmsis(tos_task_resume(taskId));
  }

  return state;
}
osStatus_t osThreadDetach(osThreadId_t thread_id) {
  (void)thread_id;
  // todo
  return osError;
}

osStatus_t osThreadJoin(osThreadId_t thread_id) {
  (void)thread_id;
  // todo
  return osError;
}

__NO_RETURN void osThreadExit(void) {
  tos_task_destroy(k_curr_task);
  while (1)
    ;
}

osStatus_t osThreadTerminate(osThreadId_t thread_id) {
  k_task_t* taskId = (k_task_t*)thread_id;
  osStatus_t state;

  if (taskId == NULL) {
    state = osErrorParameter;
  } else {
    if (taskId->state == K_TASK_STATE_DELETED) {
      state = osErrorResource;
    } else {
      state = errno_knl2cmsis(tos_task_destroy(taskId));
    }
  }

  return state;
}

uint32_t osThreadGetCount(void) {
  uint32_t count = 0;

  TOS_CPU_CPSR_ALLOC();
  k_task_t* task;

  TOS_CPU_INT_DISABLE();

  TOS_LIST_FOR_EACH_ENTRY(task, k_task_t, stat_list, &k_stat_list) {
    count += 1;
  }

  TOS_CPU_INT_ENABLE();

  return count;
}

uint32_t osThreadEnumerate(osThreadId_t* thread_array, uint32_t array_items) {
  uint32_t count = 0;

  if ((thread_array != NULL) && (array_items != 0U)) {
    TOS_CPU_CPSR_ALLOC();
    k_task_t* task;

    TOS_CPU_INT_DISABLE();

    TOS_LIST_FOR_EACH_ENTRY(task, k_task_t, stat_list, &k_stat_list) {
      while (count < array_items) {
        thread_array[count] = (osThreadId_t)task;
        count += 1;
      }
    }

    TOS_CPU_INT_ENABLE();
  }

  return count;
}

uint32_t osThreadFlagsSet(osThreadId_t thread_id, uint32_t flags) {
  uint32_t rflags = 0xFFFFFFFF;
  // todo
  return rflags;
}

uint32_t osThreadFlagsClear(uint32_t flags) {
  uint32_t rflags = 0xFFFFFFFF;
  // todo
  return rflags;
}

uint32_t osThreadFlagsGet(void) {
  uint32_t rflags = 0xFFFFFFFF;
  // todo
  return rflags;
}

uint32_t osThreadFlagsWait(uint32_t flags, uint32_t options, uint32_t timeout) {
  uint32_t rflags = 0xFFFFFFFF;
  // todo
  return rflags;
}

osStatus_t osDelay(uint32_t ticks) {
  k_tick_t delay;

  if (ticks == osWaitForever) {
    delay = TOS_TIME_FOREVER;
  } else {
    delay = (k_tick_t)ticks;
  }

  return errno_knl2cmsis(tos_task_delay(delay));
}

osStatus_t osDelayUntil(uint32_t ticks) {
  k_tick_t delay;

  if (ticks == osWaitForever) {
    delay = TOS_TIME_FOREVER;
  } else {
    k_tick_t now = tos_systick_get();
    if ((k_tick_t)ticks < now) {
      return osErrorParameter;
    }
    delay = (k_tick_t)ticks - now;
  }

  return errno_knl2cmsis(tos_task_delay(delay));
}

/*---------------------------------------------------------------------------*/

osTimerId_t osTimerNew(osTimerFunc_t func,
                       osTimerType_t type,
                       void* argument,
                       const osTimerAttr_t* attr) {
  k_err_t err;
  k_opt_t opt = TOS_OPT_TIMER_ONESHOT;
  k_timer_t* timerId = NULL;

  if (attr && func) {
    if (type == osTimerPeriodic) {
      opt = TOS_OPT_TIMER_PERIODIC;
    } else if (type == osTimerOnce) {
      opt = TOS_OPT_TIMER_ONESHOT;
    }

    if (attr->cb_mem) {
      err = tos_timer_create((k_timer_t*)attr->cb_mem, (k_tick_t)1000u,
                             (k_tick_t)1000u, (k_timer_callback_t)func,
                             argument, opt);
      timerId = err == K_ERR_NONE ? attr->cb_mem : NULL;
    }
  }

  return (osTimerId_t)timerId;
}

const char* osTimerGetName(osTimerId_t timer_id) {
  (void)timer_id;
  // todo
  return NULL;
}

osStatus_t osTimerStart(osTimerId_t timer_id, uint32_t ticks) {
  k_timer_t* timerId = (k_timer_t*)timer_id;
  osStatus_t state;

  if (timerId == NULL) {
    state = osErrorParameter;
  } else {
    tos_timer_delay_change(timerId, ticks);
    tos_timer_period_change(timerId, ticks);
    state = errno_knl2cmsis(tos_timer_start(timerId));
  }

  return state;
}

osStatus_t osTimerStop(osTimerId_t timer_id) {
  k_timer_t* timerId = (k_timer_t*)timer_id;
  osStatus_t state;

  if (timerId == NULL) {
    state = osErrorParameter;
  } else {
    state = errno_knl2cmsis(tos_timer_stop(timerId));
  }

  return state;
}

uint32_t osTimerIsRunning(osTimerId_t timer_id) {
  k_timer_t* timerId = (k_timer_t*)timer_id;
  uint32_t running;

  if (timerId == NULL) {
    running = 0U;
  } else {
    running = timerId->state == TIMER_STATE_RUNNING;
  }

  return running;
}

osStatus_t osTimerDelete(osTimerId_t timer_id) {
  k_timer_t* timerId = (k_timer_t*)timer_id;

  return errno_knl2cmsis(tos_timer_destroy(timerId));
}

/*---------------------------------------------------------------------------*/

osEventFlagsId_t osEventFlagsNew(const osEventFlagsAttr_t* attr) {
  k_err_t err;
  k_event_t* eventId = NULL;

  if (attr) {
    if (attr->cb_mem) {
      err = tos_event_create((k_event_t*)attr->cb_mem, 0);
      eventId = err == K_ERR_NONE ? attr->cb_mem : NULL;
    }
  }

  return (osEventFlagsId_t)eventId;
}

const char* osEventFlagsGetName(osEventFlagsId_t ef_id) {
  (void)ef_id;
  // todo
  return NULL;
}

uint32_t osEventFlagsSet(osEventFlagsId_t ef_id, uint32_t flags) {
  k_event_t* eventId = (k_event_t*)ef_id;
  k_err_t err;
  uint32_t rflags;

  if (eventId == NULL) {
    rflags = (uint32_t)osErrorParameter;
  } else {
    err = tos_event_post_keep(eventId, (k_event_flag_t)flags);
    rflags = err == K_ERR_NONE ? (uint32_t)eventId->flag
                               : (uint32_t)errno_knl2cmsis(err);
  }

  return rflags;
}

uint32_t osEventFlagsClear(osEventFlagsId_t ef_id, uint32_t flags) {
  k_event_t* eventId = (k_event_t*)ef_id;
  k_err_t err;
  uint32_t rflags;

  if (eventId == NULL) {
    rflags = (uint32_t)osErrorParameter;
  } else {
    flags = eventId->flag & (~flags);
    err = tos_event_post(eventId, (k_event_flag_t)flags);
    rflags = err == K_ERR_NONE ? (uint32_t)eventId->flag
                               : (uint32_t)errno_knl2cmsis(err);
  }

  return rflags;
}

uint32_t osEventFlagsGet(osEventFlagsId_t ef_id) {
  k_event_t* eventId = (k_event_t*)ef_id;
  uint32_t rflags;

  if (ef_id == NULL) {
    rflags = 0U;
  } else {
    rflags = (uint32_t)eventId->flag;
  }

  return rflags;
}

uint32_t osEventFlagsWait(osEventFlagsId_t ef_id,
                          uint32_t flags,
                          uint32_t options,
                          uint32_t timeout) {
  k_event_t* eventId = (k_event_t*)ef_id;
  k_err_t err;
  uint32_t rflags;

  if (ef_id == NULL) {
    rflags = (uint32_t)osErrorParameter;
  } else {
    k_event_flag_t flag_match;
    k_opt_t opt_pend = 0;
    timeout =
        ((timeout == 0) || (timeout == osWaitForever)) ? TOS_TIME_FOREVER : timeout;
    if (options & (0x01 == 0)) {
      opt_pend |= TOS_OPT_EVENT_PEND_ANY;
    } else {
      opt_pend |= TOS_OPT_EVENT_PEND_ALL;
    }
    if (options & (0x02 == 0)) {
      opt_pend |= TOS_OPT_EVENT_PEND_CLR;
    }

    err = tos_event_pend(eventId, (k_event_flag_t)flags, &flag_match, (k_tick_t)timeout,
                         opt_pend);
    rflags = err == K_ERR_NONE ? (uint32_t)flag_match
                               : (uint32_t)errno_knl2cmsis(err);
  }

  return rflags;
}

osStatus_t osEventFlagsDelete(osEventFlagsId_t ef_id) {
  k_event_t* eventId = (k_event_t*)ef_id;

  return errno_knl2cmsis(tos_event_destroy(eventId));
}

/*---------------------------------------------------------------------------*/

osMutexId_t osMutexNew(const osMutexAttr_t* attr) {
  k_err_t err;
  k_mutex_t* mutexId = NULL;

  if (attr) {
    if (attr->cb_mem) {
      err = tos_mutex_create((k_mutex_t*)attr->cb_mem);
      mutexId = err == K_ERR_NONE ? attr->cb_mem : NULL;
    }
  }

  return (osEventFlagsId_t)mutexId;
}

const char* osMutexGetName(osMutexId_t mutex_id) {
  (void)mutex_id;
  // todo
  return NULL;
}

osStatus_t osMutexAcquire(osMutexId_t mutex_id, uint32_t timeout) {
  k_mutex_t* mutexId = (k_mutex_t*)mutex_id;

  return errno_knl2cmsis(tos_mutex_pend_timed(mutexId, timeout));
}

osStatus_t osMutexRelease(osMutexId_t mutex_id) {
  k_mutex_t* mutexId = (k_mutex_t*)mutex_id;

  return errno_knl2cmsis(tos_mutex_post(mutexId));
}

osThreadId_t osMutexGetOwner(osMutexId_t mutex_id) {
  k_mutex_t* mutexId = (k_mutex_t*)mutex_id;

  return (osThreadId_t)mutexId->owner;
}

osStatus_t osMutexDelete(osMutexId_t mutex_id) {
  k_mutex_t* mutexId = (k_mutex_t*)mutex_id;

  return errno_knl2cmsis(tos_mutex_destroy(mutexId));
}

/*---------------------------------------------------------------------------*/

osSemaphoreId_t osSemaphoreNew(uint32_t max_count,
                               uint32_t initial_count,
                               const osSemaphoreAttr_t* attr) {
  k_err_t err;
  k_sem_t* semId = NULL;

  if (attr) {
    if (attr->cb_mem) {
      err =
          tos_sem_create_max((k_sem_t*)attr->cb_mem, (k_sem_cnt_t)initial_count,
                             (k_sem_cnt_t)max_count);
      semId = err == K_ERR_NONE ? attr->cb_mem : NULL;
    }
  }

  return (osEventFlagsId_t)semId;
}

const char* osSemaphoreGetName(osSemaphoreId_t semaphore_id) {
  (void)semaphore_id;
  // todo
  return NULL;
}

osStatus_t osSemaphoreAcquire(osSemaphoreId_t semaphore_id, uint32_t timeout) {
  k_sem_t* semId = (k_sem_t*)semaphore_id;

  return errno_knl2cmsis(tos_sem_pend(semId, timeout));
}

osStatus_t osSemaphoreRelease(osSemaphoreId_t semaphore_id) {
  k_sem_t* semId = (k_sem_t*)semaphore_id;

  return errno_knl2cmsis(tos_sem_post(semId));
}

uint32_t osSemaphoreGetCount(osSemaphoreId_t semaphore_id) {
  k_sem_t* semId = (k_sem_t*)semaphore_id;

  return (uint32_t)semId->count;
}

osStatus_t osSemaphoreDelete(osSemaphoreId_t semaphore_id) {
  k_sem_t* semId = (k_sem_t*)semaphore_id;

  return errno_knl2cmsis(tos_sem_destroy(semId));
}

/*---------------------------------------------------------------------------*/

osMemoryPoolId_t osMemoryPoolNew(uint32_t block_count,
                                 uint32_t block_size,
                                 const osMemoryPoolAttr_t* attr) {
  k_err_t err;
  k_mmblk_pool_t* mpId = NULL;

  if (attr) {
    if (attr->cb_mem) {
      err = tos_mmblk_pool_create((k_mmblk_pool_t*)attr->cb_mem, attr->mp_mem,
                                  block_count, block_size);
      mpId = err == K_ERR_NONE ? attr->cb_mem : NULL;
    }
  }

  return (osMemoryPoolId_t)mpId;
}

const char* osMemoryPoolGetName(osMemoryPoolId_t mp_id) {
  (void)mp_id;
  // todo
  return NULL;
}

void* osMemoryPoolAlloc(osMemoryPoolId_t mp_id, uint32_t timeout) {
  k_mmblk_pool_t* mpId = (k_mmblk_pool_t*)mp_id;
  k_err_t err;
  void* blk = NULL;

  err = tos_mmblk_alloc(mpId, &blk);

  return err == K_ERR_NONE ? blk : NULL;
}

osStatus_t osMemoryPoolFree(osMemoryPoolId_t mp_id, void* block) {
  k_mmblk_pool_t* mpId = (k_mmblk_pool_t*)mp_id;

  return errno_knl2cmsis(tos_mmblk_free(mpId, block));
}
uint32_t osMemoryPoolGetCapacity(osMemoryPoolId_t mp_id) {
  k_mmblk_pool_t* mpId = (k_mmblk_pool_t*)mp_id;
  return mpId->blk_max;
}
uint32_t osMemoryPoolGetBlockSize(osMemoryPoolId_t mp_id) {
  k_mmblk_pool_t* mpId = (k_mmblk_pool_t*)mp_id;
  return mpId->blk_size;
}
uint32_t osMemoryPoolGetCount(osMemoryPoolId_t mp_id) {
  k_mmblk_pool_t* mpId = (k_mmblk_pool_t*)mp_id;
  return mpId->blk_max - mpId->blk_free;
}
uint32_t osMemoryPoolGetSpace(osMemoryPoolId_t mp_id) {
  k_mmblk_pool_t* mpId = (k_mmblk_pool_t*)mp_id;
  return mpId->blk_free;
}
osStatus_t osMemoryPoolDelete(osMemoryPoolId_t mp_id) {
  k_mmblk_pool_t* mpId = (k_mmblk_pool_t*)mp_id;

  return errno_knl2cmsis(tos_mmblk_pool_destroy(mpId));
}

/*---------------------------------------------------------------------------*/

#if TOS_CFG_MESSAGE_QUEUE_EN > 0u
osMessageQueueId_t osMessageQueueNew(uint32_t msg_count,
                                     uint32_t msg_size,
                                     const osMessageQueueAttr_t* attr) {
  k_err_t err;
  k_msg_q_t* mqId = NULL;

  if (attr) {
    if (attr->mq_mem && msg_size == sizeof(void*)) {
      err = tos_msg_q_create((k_msg_q_t*)attr->cb_mem, attr->mq_mem, msg_count);
      mqId = err == K_ERR_NONE ? attr->cb_mem : NULL;
    }
  }

  return mqId;
}

const char* osMessageQueueGetName(osMessageQueueId_t mq_id) {
  (void)mq_id;
  // todo
  return NULL;
}

osStatus_t osMessageQueuePut(osMessageQueueId_t mq_id,
                             const void* msg_ptr,
                             uint8_t msg_prio,
                             uint32_t timeout) {
  k_msg_q_t* mqId = (k_msg_q_t*)mq_id;

  return errno_knl2cmsis(tos_msg_q_post(mqId, (void*)msg_ptr));
}

osStatus_t osMessageQueueGet(osMessageQueueId_t mq_id,
                             void* msg_ptr,
                             uint8_t* msg_prio,
                             uint32_t timeout) {
  k_msg_q_t* mqId = (k_msg_q_t*)mq_id;

  return errno_knl2cmsis(tos_msg_q_pend(mqId, &msg_ptr, timeout));
}

uint32_t osMessageQueueGetCapacity(osMessageQueueId_t mq_id) {
  k_msg_q_t* mqId = (k_msg_q_t*)mq_id;
  uint32_t capacity;

  if (mqId == NULL) {
    capacity = 0U;
  } else {
    capacity = mqId->ring_q.item_cnt;
  }

  return capacity;
}

uint32_t osMessageQueueGetMsgSize(osMessageQueueId_t mq_id) {
  k_msg_q_t* mqId = (k_msg_q_t*)mq_id;
  uint32_t size;

  if (mqId == NULL) {
    size = 0U;
  } else {
    size = mqId->ring_q.item_size;
  }

  return size;
}

uint32_t osMessageQueueGetCount(osMessageQueueId_t mq_id) {
  k_msg_q_t* mqId = (k_msg_q_t*)mq_id;
  uint32_t count;

  if (mqId == NULL) {
    count = 0U;
  } else {
    count = mqId->ring_q.total;
  }

  return count;
}

uint32_t osMessageQueueGetSpace(osMessageQueueId_t mq_id) {
  k_msg_q_t* mqId = (k_msg_q_t*)mq_id;
  uint32_t space;

  if (mqId == NULL) {
    space = 0U;
  } else {
    space = mqId->ring_q.item_cnt - mqId->ring_q.total;
  }

  return space;
}

osStatus_t osMessageQueueReset(osMessageQueueId_t mq_id) {
  k_msg_q_t* mqId = (k_msg_q_t*)mq_id;

  return errno_knl2cmsis(tos_msg_q_flush(mqId));
}

osStatus_t osMessageQueueDelete(osMessageQueueId_t mq_id) {
  k_msg_q_t* mqId = (k_msg_q_t*)mq_id;

  return errno_knl2cmsis(tos_msg_q_destroy(mqId));
}
#endif

/*---------------------------------------------------------------------------*/
