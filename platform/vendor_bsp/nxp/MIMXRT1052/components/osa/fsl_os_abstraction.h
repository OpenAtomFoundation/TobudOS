/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_OS_ABSTRACTION_H_
#define _FSL_OS_ABSTRACTION_H_

#include "fsl_os_abstraction_config.h"

/*!
 * @addtogroup osa_adapter
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Type for the Task Priority*/
typedef uint16_t osa_task_priority_t;
/*! @brief Type for a task handler */
typedef void *osa_task_handle_t;
/*! @brief Type for the parameter to be passed to the task at its creation */
typedef void *osa_task_param_t;
/*! @brief Type for task pointer. Task prototype declaration */
typedef void (*osa_task_ptr_t)(osa_task_param_t task_param);
/*! @brief Type for the semaphore handler */
typedef void *osa_semaphore_handle_t;
/*! @brief Type for the mutex handler */
typedef void *osa_mutex_handle_t;
/*! @brief Type for the event handler */
typedef void *osa_event_handle_t;
/*! @brief Type for an event flags group, bit 32 is reserved. */
typedef uint32_t osa_event_flags_t;
/*! @brief Message definition. */
typedef void *osa_msg_handle_t;
/*! @brief Type for the message queue handler */
typedef void *osa_msgq_handle_t;
/*! @brief Type for the Timer handler */
typedef void *osa_timer_handle_t;
/*! @brief Type for the Timer callback function pointer. */
typedef void (*osa_timer_fct_ptr_t)(void const *argument);
/*! @brief Thread Definition structure contains startup information of a thread.*/
typedef struct osa_task_def_tag
{
    osa_task_ptr_t pthread; /*!< start address of thread function*/
    uint32_t tpriority;     /*!< initial thread priority*/
    uint32_t instances;     /*!< maximum number of instances of that thread function*/
    uint32_t stacksize;     /*!< stack size requirements in bytes; 0 is default stack size*/
    uint32_t *tstack;       /*!< stack pointer*/
    void *tlink;            /*!< link pointer*/
    uint8_t *tname;         /*!< name pointer*/
    uint8_t useFloat;       /*!< is use float*/
} osa_task_def_t;
/*! @brief Thread Link Definition structure .*/
typedef struct osa_thread_link_tag
{
    uint8_t link[12];                  /*!< link*/
    osa_task_handle_t osThreadId;      /*!< thread id*/
    osa_task_def_t *osThreadDefHandle; /*!< pointer of thread define handle*/
    uint32_t *osThreadStackHandle;     /*!< pointer of thread stack handle*/
} osa_thread_link_t, *osa_thread_link_handle_t;

/*! @Timer Definition structure contains timer parameters.*/
typedef struct osa_time_def_tag
{
    osa_timer_fct_ptr_t pfCallback; /* < start address of a timer function */
    void *argument;                 /* < argument of a timer function */
} osa_time_def_t;

/*! @brief Type for the timer definition*/
typedef enum _osa_timer
{
    KOSA_TimerOnce     = 0, /*!< one-shot timer*/
    KOSA_TimerPeriodic = 1  /*!< repeating timer*/
} osa_timer_t;

/*! @brief Defines the return status of OSA's functions */
typedef enum _osa_status
{
    KOSA_StatusSuccess = kStatus_Success,                  /*!< Success */
    KOSA_StatusError   = MAKE_STATUS(kStatusGroup_OSA, 1), /*!< Failed */
    KOSA_StatusTimeout = MAKE_STATUS(kStatusGroup_OSA, 2), /*!< Timeout occurs while waiting */
    KOSA_StatusIdle    = MAKE_STATUS(kStatusGroup_OSA, 3), /*!< Used for bare metal only, the wait object is not ready
                                                                 and timeout still not occur */
} osa_status_t;

#ifdef USE_RTOS
#undef USE_RTOS
#endif

#if defined(FSL_RTOS_MQX)
#define USE_RTOS (1)
#elif defined(FSL_RTOS_FREE_RTOS)
#define USE_RTOS (1)
#define OSA_TASK_HANDLE_SIZE (16)
#define OSA_EVENT_HANDLE_SIZE (8)
#define OSA_SEM_HANDLE_SIZE (4)
#define OSA_MUTEX_HANDLE_SIZE (4)
#define OSA_MSGQ_HANDLE_SIZE (4)
#define OSA_MSG_HANDLE_SIZE (0)
#elif defined(FSL_RTOS_UCOSII)
#define USE_RTOS (1)
#elif defined(FSL_RTOS_UCOSIII)
#define USE_RTOS (1)
#else
#define USE_RTOS (0)
#define OSA_TASK_HANDLE_SIZE (28)
#if (defined(FSL_OSA_TASK_ENABLE) && (FSL_OSA_TASK_ENABLE > 0U))
#define OSA_EVENT_HANDLE_SIZE (20)
#else
#define OSA_EVENT_HANDLE_SIZE (16)
#endif /* FSL_OSA_TASK_ENABLE */
#define OSA_SEM_HANDLE_SIZE (12)
#define OSA_MUTEX_HANDLE_SIZE (12)
#if (defined(FSL_OSA_TASK_ENABLE) && (FSL_OSA_TASK_ENABLE > 0U))
#define OSA_MSGQ_HANDLE_SIZE (32)
#else
#define OSA_MSGQ_HANDLE_SIZE (28)
#endif /* FSL_OSA_TASK_ENABLE */
#define OSA_MSG_HANDLE_SIZE (4)
#endif

#define OSA_PRIORITY_IDLE (6)
#define OSA_PRIORITY_LOW (5)
#define OSA_PRIORITY_BELOW_NORMAL (4)
#define OSA_PRIORITY_NORMAL (3)
#define OSA_PRIORITY_ABOVE_NORMAL (2)
#define OSA_PRIORITY_HIGH (1)
#define OSA_PRIORITY_REAL_TIME (0)
#define OSA_TASK_PRIORITY_MAX (0)
#define OSA_TASK_PRIORITY_MIN (15)
#define SIZE_IN_UINT32_UNITS(size) (((size) + sizeof(uint32_t) - 1) / sizeof(uint32_t))

/*! @brief Constant to pass as timeout value in order to wait indefinitely. */
#define osaWaitForever_c ((uint32_t)(-1))
#define osaEventFlagsAll_c ((osa_event_flags_t)(0x00FFFFFF))
#define osThreadStackArray(name) osThread_##name##_stack
#define osThreadStackDef(name, stacksize, instances) \
    uint32_t osThreadStackArray(name)[SIZE_IN_UINT32_UNITS(stacksize) * (instances)];

/* ==== Thread Management ==== */

/* Create a Thread Definition with function, priority, and stack requirements.
 * \param         name         name of the thread function.
 * \param         priority     initial priority of the thread function.
 * \param         instances    number of possible thread instances.
 * \param         stackSz      stack size (in bytes) requirements for the thread function.
 * \param         useFloat
 */
#if defined(FSL_RTOS_MQX)
#define OSA_TASK_DEFINE(name, priority, instances, stackSz, useFloat)                                        \
    osa_thread_link_t osThreadLink_##name[instances]                               = {0};                    \
    osThreadStackDef(name, stackSz, instances) osa_task_def_t os_thread_def_##name = {                       \
        (name),           (priority), (instances), (stackSz), osThreadStackArray(name), osThreadLink_##name, \
        (uint8_t *)#name, (useFloat)}
#elif defined(FSL_RTOS_UCOSII)
#if gTaskMultipleInstancesManagement_c
#define OSA_TASK_DEFINE(name, priority, instances, stackSz, useFloat)                                        \
    osa_thread_link_t osThreadLink_##name[instances]                               = {0};                    \
    osThreadStackDef(name, stackSz, instances) osa_task_def_t os_thread_def_##name = {                       \
        (name),           (priority), (instances), (stackSz), osThreadStackArray(name), osThreadLink_##name, \
        (uint8_t *)#name, (useFloat)}
#else
#define OSA_TASK_DEFINE(name, priority, instances, stackSz, useFloat)                  \
    osThreadStackDef(name, stackSz, instances) osa_task_def_t os_thread_def_##name = { \
        (name), (priority), (instances), (stackSz), osThreadStackArray(name), NULL, (uint8_t *)#name, (useFloat)}
#endif
#else
#define OSA_TASK_DEFINE(name, priority, instances, stackSz, useFloat)                       \
    osa_task_def_t os_thread_def_##name = {(name), (priority), (instances),      (stackSz), \
                                           NULL,   NULL,       (uint8_t *)#name, (useFloat)}
#endif
/* Access a Thread defintion.
 * \param         name          name of the thread definition object.
 */
#define OSA_TASK(name) &os_thread_def_##name

#define OSA_TASK_PROTO(name) externosa_task_def_t os_thread_def_##name
/*  ==== Timer Management  ====
 * Define a Timer object.
 * \param         name          name of the timer object.
 * \param         function      name of the timer call back function.
 */

#define OSA_TIMER_DEF(name, function) osa_time_def_t os_timer_def_##name = {(function), NULL}

/* Access a Timer definition.
 * \param         name          name of the timer object.
 */
#define OSA_TIMER(name) &os_timer_def_##name

/* ==== Buffer Definition ==== */

/*!
 * @brief Defines the semaphore handle buffer
 *
 * This macro is used to define the semaphore handle buffer for semaphore queue.
 * And then uses the macro SEMAPHORE_HANDLE_BUFFER_GET to get the semaphore handle buffer pointer.
 * The macro should not be used in a suitable position for its user.
 *
 * This macro is optional, semaphore handle buffer could also be defined by yourself.
 *
 * This is a example,
 * @code
 * SEMAPHORE_HANDLE_BUFFER_DEFINE(semaphoreHandle);
 * @endcode
 *
 * @param name The name string of the semaphore handle buffer.
 */
#define SEMAPHORE_HANDLE_BUFFER_DEFINE(name) uint32_t name[((OSA_SEM_HANDLE_SIZE - 1) >> 2) + 1]
/*!                                                                             \
 * @brief Gets the semaphore buffer pointer                                     \
 *                                                                              \
 * This macro is used to get the semaphore buffer pointer. The macro should     \
 * not be used before the macro SEMAPHORE_HANDLE_BUFFER_DEFINE is used.         \
 *                                                                              \
 * @param name The memory name string of the buffer.                            \
 */
#define SEMAPHORE_HANDLE_BUFFER_GET(name) (osa_semaphore_handle_t *)&name[0]

/*!
 * @brief Defines the mutex handle buffer
 *
 * This macro is used to define the mutex handle buffer for mutex queue.
 * And then uses the macro MUTEX_HANDLE_BUFFER_GET to get the mutex handle buffer pointer.
 * The macro should not be used in a suitable position for its user.
 *
 * This macro is optional, mutex handle buffer could also be defined by yourself.
 *
 * This is a example,
 * @code
 * MUTEX_HANDLE_BUFFER_DEFINE(mutexHandle);
 * @endcode
 *
 * @param name The name string of the mutex handle buffer.
 */
#define MUTEX_HANDLE_BUFFER_DEFINE(name) uint32_t name[((OSA_MUTEX_HANDLE_SIZE - 1) >> 2) + 1]
/*!                                                                             \
 * @brief Gets the mutex buffer pointer                                         \
 *                                                                              \
 * This macro is used to get the mutex buffer pointer. The macro should         \
 * not be used before the macro MUTEX_HANDLE_BUFFER_DEFINE is used.             \
 *                                                                              \
 * @param name The memory name string of the buffer.                            \
 */
#define MUTEX_HANDLE_BUFFER_GET(name) (osa_mutex_handle_t *)&name[0]

/*!
 * @brief Defines the enent handle buffer
 *
 * This macro is used to define the enent handle buffer for enent queue.
 * And then uses the macro EVENT_HANDLE_BUFFER_GET to get the enent handle buffer pointer.
 * The macro should not be used in a suitable position for its user.
 *
 * This macro is optional, enent handle buffer could also be defined by yourself.
 *
 * This is a example,
 * @code
 * EVENT_HANDLE_BUFFER_DEFINE(enentHandle);
 * @endcode
 *
 * @param name The name string of the enent handle buffer.
 */
#define EVENT_HANDLE_BUFFER_DEFINE(name) uint32_t name[((OSA_EVENT_HANDLE_SIZE - 1) >> 2) + 1]
/*!                                                                             \
 * @brief Gets the event buffer pointer                                         \
 *                                                                              \
 * This macro is used to get the event buffer pointer. The macro should         \
 * not be used before the macro EVENT_HANDLE_BUFFER_DEFINE is used.             \
 *                                                                              \
 * @param name The memory name string of the buffer.                            \
 */
#define EVENT_HANDLE_BUFFER_GET(name) (osa_event_handle_t *)&name[0]

/*!
 * @brief Defines the message handle buffer
 *
 * This macro is used to define the message handle buffer for message queue.
 * And then uses the macro MSGQ_HANDLE_BUFFER_GET to get the message handle buffer pointer.
 * The macro should not be used in a suitable position for its user.
 *
 * This macro is optional, message handle buffer could also be defined by yourself.
 *
 * This is a example,
 * @code
 * MSGQ_HANDLE_BUFFER_DEFINE(msgqHandle, 3, sizeof(msgStruct));
 * MSGQ_HANDLE_BUFFER_DEFINE(msgqHandle, 3, 4);
 * @endcode
 *
 * @param name The name string of the message handle buffer.
 * @param numberOfMsgs The number Of messages.
 * @param msgSize The size of a single message structure.
 */
#if defined(FSL_RTOS_FREE_RTOS)
/*< Macro For FREE_RTOS*/
#define MSGQ_HANDLE_BUFFER_DEFINE(name, numberOfMsgs, msgSize) uint32_t name[(((OSA_MSGQ_HANDLE_SIZE)-1) >> 2) + 1]
#else
/*< Macro For BARE_MATEL*/
#define MSGQ_HANDLE_BUFFER_DEFINE(name, numberOfMsgs, msgSize) \
    uint32_t name[(((OSA_MSGQ_HANDLE_SIZE + numberOfMsgs * msgSize) - 1) >> 2) + 1]
#endif
/*!                                                                             \
 * @brief Gets the message buffer pointer                                       \
 *                                                                              \
 * This macro is used to get the message buffer pointer. The macro should       \
 * not be used before the macro MSGQ_HANDLE_BUFFER_DEFINE is used.              \
 *                                                                              \
 * @param name The memory name string of the buffer.                            \
 */
#define MSGQ_HANDLE_BUFFER_GET(name) (osa_msgq_handle_t) & name[0]

/*!
 * @brief Defines the task handle buffer
 *
 * This macro is used to define the task handle buffer for task queue.
 * And then uses the macro TASK_HANDLE_BUFFER_GET to get the task handle buffer pointer.
 * The macro should not be used in a suitable position for its user.
 *
 * This macro is optional, task handle buffer could also be defined by yourself.
 *
 * This is a example,
 * @code
 * TASK_HANDLE_BUFFER_DEFINE(taskHandle1);
 * @endcode
 *
 * @param name The name string of the task handle buffer.
 */
#define TASK_HANDLE_BUFFER_DEFINE(name) uint32_t name[((OSA_TASK_HANDLE_SIZE - 1) >> 2) + 1]
/*!                                                                             \
 * @brief Gets the task buffer pointer                                          \
 *                                                                              \
 * This macro is used to get the task buffer pointer. The macro should          \
 * not be used before the macro TASK_HANDLE_BUFFER_DEFINE is used.              \
 *                                                                              \
 * @param name The memory name string of the buffer.                            \
 */
#define TASK_HANDLE_BUFFER_GET(name) (osa_task_handle_t) & name[0]

#if defined(FSL_RTOS_FREE_RTOS)
#include "fsl_os_abstraction_free_rtos.h"
#else
#include "fsl_os_abstraction_bm.h"
#endif

extern const uint8_t gUseRtos_c;

/*
 * alloc the temporary memory to store the status
 */
#define OSA_SR_ALLOC() uint32_t osaCurrentSr;
/*
 * Enter critical mode
 */
#define OSA_ENTER_CRITICAL() OSA_EnterCritical(&osaCurrentSr)
/*
 * Exit critical mode and retore the previous mode
 */
#define OSA_EXIT_CRITICAL() OSA_ExitCritical(osaCurrentSr)

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief Reserves the requested amount of memory in bytes.
 *
 * The function is used to reserve the requested amount of memory in bytes and initializes it to 0.
 *
 * @param length Amount of bytes to reserve.
 *
 * @return Pointer to the reserved memory. NULL if memory can't be allocated.
 */
void *OSA_MemoryAllocate(uint32_t length);

/*!
 * @brief Frees the memory previously reserved.
 *
 * The function is used to free the memory block previously reserved.
 *
 * @param p Pointer to the start of the memory block previously reserved.
 *
 */
void OSA_MemoryFree(void *p);

/*!
 * @brief Enter critical with nesting mode.
 *
 * @param sr Store current status and return to caller.
 */
void OSA_EnterCritical(uint32_t *sr);

/*!
 * @brief Exit critical with nesting mode.
 *
 * @param sr Previous status to restore.
 */
void OSA_ExitCritical(uint32_t sr);

/*!
 * @name Task management
 * @{
 */

/*!
 * @brief Creates a task.
 *
 * This function is used to create task based on the resources defined
 * by the macro OSA_TASK_DEFINE.

 * @param taskHandle Pointer to a memory space of size #OSA_TASK_HANDLE_SIZE allocated by the caller, task handle.
 * The handle should be 4 byte aligned, because unaligned access does not support on some devices.
 * The macro TASK_HANDLE_BUFFER_GET is used to get the task buffer pointer,
 * and should not be used before the macro TASK_HANDLE_BUFFER_DEFINE is used.
 * @param thread_def pointer to theosa_task_def_t structure which defines the task.
 * @param task_param Pointer to be passed to the task when it is created.
 *
 * @retval KOSA_StatusSuccess The task is successfully created.
 * @retval KOSA_StatusError   The task can not be created.
 * Example:
 *  @code
 *   uint32_t taskHandleBuffer[((OSA_TASK_HANDLE_SIZE + sizeof(uint32_t) - 1) / sizeof(uitn32_t))];
 *   osa_task_handle_t taskHandle = (osa_task_handle_t)&taskHandleBuffer[0];
 *   OSA_TASK_DEFINE( Job1, OSA_PRIORITY_HIGH, 1, 800, 0);
 *   OSA_TaskCreate(taskHandle, OSA_TASK(Job1), (osa_task_param_t)NULL);
 *  @endcode
 */
osa_status_t OSA_TaskCreate(osa_task_handle_t taskHandle, osa_task_def_t *thread_def, osa_task_param_t task_param);

/*!
 * @brief Gets the handler of active task.
 *
 * @return Handler to current active task.
 */
osa_task_handle_t OSA_TaskGetCurrentHandle(void);

/*!
 * @brief Puts the active task to the end of scheduler's queue.
 *
 * When a task calls this function, it gives up the CPU and puts itself to the
 * end of a task ready list.
 *
 * @retval KOSA_StatusSuccess The function is called successfully.
 * @retval KOSA_StatusError   Error occurs with this function.
 */
osa_status_t OSA_TaskYield(void);

/*!
 * @brief Gets the priority of a task.
 *
 * @param taskHandle The handler of the task whose priority is received.
 * The macro TASK_HANDLE_BUFFER_GET is used to get the task buffer pointer,
 * and should not be used before the macro TASK_HANDLE_BUFFER_DEFINE is used.
 *
 * @return Task's priority.
 */
osa_task_priority_t OSA_TaskGetPriority(osa_task_handle_t taskHandle);

/*!
 * @brief Sets the priority of a task.
 *
 * @param taskHandle  The handler of the task whose priority is set.
 * The macro TASK_HANDLE_BUFFER_GET is used to get the task buffer pointer,
 * and should not be used before the macro TASK_HANDLE_BUFFER_DEFINE is used.
 * @param taskPriority The priority to set.
 *
 * @retval KOSA_StatusSuccess Task's priority is set successfully.
 * @retval KOSA_StatusError   Task's priority can not be set.
 */
osa_status_t OSA_TaskSetPriority(osa_task_handle_t taskHandle, osa_task_priority_t taskPriority);

/*!
 * @brief Destroys a previously created task.
 *
 * @param taskHandle The handler of the task to destroy.
 * The macro TASK_HANDLE_BUFFER_GET is used to get the task buffer pointer,
 * and should not be used before the macro TASK_HANDLE_BUFFER_DEFINE is used.
 *
 * @retval KOSA_StatusSuccess The task was successfully destroyed.
 * @retval KOSA_StatusError   Task destruction failed or invalid parameter.
 */
osa_status_t OSA_TaskDestroy(osa_task_handle_t taskHandle);

/*!
 * @brief Creates a semaphore with a given value.
 *
 * This function creates a semaphore and sets the value to the parameter
 * initValue.

 * @param semaphoreHandle Pointer to a memory space of size #OSA_SEM_HANDLE_SIZE allocated by the caller,
 * semaphore handle.
 * The handle should be 4 byte aligned, because unaligned access does not support on some devices.
 * The macro SEMAPHORE_HANDLE_BUFFER_GET is used to get the semaphore buffer pointer,
 * and should not be used before the macro SEMAPHORE_HANDLE_BUFFER_DEFINE is used.
 * @param initValue Initial value the semaphore will be set to.
 *
 * @retval KOSA_StatusSuccess  the new semaphore if the semaphore is created successfully.
 * @retval KOSA_StatusError   if the semaphore can not be created.
 *
 */
osa_status_t OSA_SemaphoreCreate(osa_semaphore_handle_t semaphoreHandle, uint32_t initValue);

/*!
 * @brief Destroys a previously created semaphore.
 *
 * @param semaphoreHandle The semaphore handle.
 * The macro SEMAPHORE_HANDLE_BUFFER_GET is used to get the semaphore buffer pointer,
 * and should not be used before the macro SEMAPHORE_HANDLE_BUFFER_DEFINE is used.
 *
 * @retval KOSA_StatusSuccess The semaphore is successfully destroyed.
 * @retval KOSA_StatusError   The semaphore can not be destroyed.
 */
osa_status_t OSA_SemaphoreDestroy(osa_semaphore_handle_t semaphoreHandle);

/*!
 * @brief Pending a semaphore with timeout.
 *
 * This function checks the semaphore's counting value. If it is positive,
 * decreases it and returns KOSA_StatusSuccess. Otherwise, a timeout is used
 * to wait.
 *
 * @param semaphoreHandle    The semaphore handle.
 * The macro SEMAPHORE_HANDLE_BUFFER_GET is used to get the semaphore buffer pointer,
 * and should not be used before the macro SEMAPHORE_HANDLE_BUFFER_DEFINE is used.
 * @param millisec The maximum number of milliseconds to wait if semaphore is not
 *                 positive. Pass osaWaitForever_c to wait indefinitely, pass 0
 *                 will return KOSA_StatusTimeout immediately.
 *
 * @retval KOSA_StatusSuccess  The semaphore is received.
 * @retval KOSA_StatusTimeout  The semaphore is not received within the specified 'timeout'.
 * @retval KOSA_StatusError    An incorrect parameter was passed.
 */
osa_status_t OSA_SemaphoreWait(osa_semaphore_handle_t semaphoreHandle, uint32_t millisec);

/*!
 * @brief Signals for someone waiting on the semaphore to wake up.
 *
 * Wakes up one task that is waiting on the semaphore. If no task is waiting, increases
 * the semaphore's counting value.
 *
 * @param semaphoreHandle The semaphore handle to signal.
 * The macro SEMAPHORE_HANDLE_BUFFER_GET is used to get the semaphore buffer pointer,
 * and should not be used before the macro SEMAPHORE_HANDLE_BUFFER_DEFINE is used.
 *
 * @retval KOSA_StatusSuccess The semaphore is successfully signaled.
 * @retval KOSA_StatusError   The object can not be signaled or invalid parameter.
 *
 */
osa_status_t OSA_SemaphorePost(osa_semaphore_handle_t semaphoreHandle);

/*!
 * @brief Create an unlocked mutex.
 *
 * This function creates a non-recursive mutex and sets it to unlocked status.
 *
 * @param mutexHandle       Pointer to a memory space of size #OSA_MUTEX_HANDLE_SIZE allocated by the caller,
 * mutex handle.
 * The handle should be 4 byte aligned, because unaligned access does not support on some devices.
 * The macro MUTEX_HANDLE_BUFFER_GET is used to get the mutex buffer pointer,
 * and should not be used before the macro MUTEX_HANDLE_BUFFER_DEFINE is used.
 *
 * @retval KOSA_StatusSuccess  the new mutex if the mutex is created successfully.
 * @retval KOSA_StatusError   if the mutex can not be created.
 */
osa_status_t OSA_MutexCreate(osa_mutex_handle_t mutexHandle);

/*!
 * @brief Waits for a mutex and locks it.
 *
 * This function checks the mutex's status. If it is unlocked, locks it and returns the
 * KOSA_StatusSuccess. Otherwise, waits for a timeout in milliseconds to lock.
 *
 * @param mutexHandle The mutex handle.
 * The macro MUTEX_HANDLE_BUFFER_GET is used to get the message mutex pointer,
 * and should not be used before the macro MUTEX_HANDLE_BUFFER_DEFINE is used.
 * @param millisec The maximum number of milliseconds to wait for the mutex.
 *                 If the mutex is locked, Pass the value osaWaitForever_c will
 *                 wait indefinitely, pass 0 will return KOSA_StatusTimeout
 *                 immediately.
 *
 * @retval KOSA_StatusSuccess The mutex is locked successfully.
 * @retval KOSA_StatusTimeout Timeout occurred.
 * @retval KOSA_StatusError   Incorrect parameter was passed.
 *
 * @note This is non-recursive mutex, a task can not try to lock the mutex it has locked.
 */
osa_status_t OSA_MutexLock(osa_mutex_handle_t mutexHandle, uint32_t millisec);

/*!
 * @brief Unlocks a previously locked mutex.
 *
 * @param mutexHandle The mutex handle.
 * The macro MUTEX_HANDLE_BUFFER_GET is used to get the mutex buffer pointer,
 * and should not be used before the macro MUTEX_HANDLE_BUFFER_DEFINE is used.
 *
 * @retval KOSA_StatusSuccess The mutex is successfully unlocked.
 * @retval KOSA_StatusError   The mutex can not be unlocked or invalid parameter.
 */
osa_status_t OSA_MutexUnlock(osa_mutex_handle_t mutexHandle);

/*!
 * @brief Destroys a previously created mutex.
 *
 * @param mutexHandle The mutex handle.
 * The macro MUTEX_HANDLE_BUFFER_GET is used to get the mutex buffer pointer,
 * and should not be used before the macro MUTEX_HANDLE_BUFFER_DEFINE is used.
 *
 * @retval KOSA_StatusSuccess The mutex is successfully destroyed.
 * @retval KOSA_StatusError   The mutex can not be destroyed.
 *
 */
osa_status_t OSA_MutexDestroy(osa_mutex_handle_t mutexHandle);

/*!
 * @brief Initializes an event object with all flags cleared.
 *
 * This function creates an event object and set its clear mode. If autoClear
 * is TRUE, when a task gets the event flags, these flags will be
 * cleared automatically. Otherwise these flags must
 * be cleared manually.
 *
 * @param eventHandle Pointer to a memory space of size #OSA_EVENT_HANDLE_SIZE allocated by the caller,
 * Event handle.
 * The handle should be 4 byte aligned, because unaligned access does not support on some devices.
 * The macro EVENT_HANDLE_BUFFER_GET is used to get the event buffer pointer,
 * and should not be used before the macro EVENT_HANDLE_BUFFER_DEFINE is used.
 * @param autoClear TRUE The event is auto-clear.
 *                  FALSE The event manual-clear
 * @retval KOSA_StatusSuccess  the new event if the event is created successfully.
 * @retval KOSA_StatusError   if the event can not be created.
 */
osa_status_t OSA_EventCreate(osa_event_handle_t eventHandle, uint8_t autoClear);

/*!
 * @brief Sets one or more event flags.
 *
 * Sets specified flags of an event object.
 *
 * @param eventHandle     The event handle.
 * The macro EVENT_HANDLE_BUFFER_GET is used to get the event buffer pointer,
 * and should not be used before the macro EVENT_HANDLE_BUFFER_DEFINE is used.
 * @param flagsToSet  Flags to be set.
 *
 * @retval KOSA_StatusSuccess The flags were successfully set.
 * @retval KOSA_StatusError   An incorrect parameter was passed.
 */
osa_status_t OSA_EventSet(osa_event_handle_t eventHandle, osa_event_flags_t flagsToSet);

/*!
 * @brief Clears one or more flags.
 *
 * Clears specified flags of an event object.
 *
 * @param eventHandle       The event handle.
 * The macro EVENT_HANDLE_BUFFER_GET is used to get the event buffer pointer,
 * and should not be used before the macro EVENT_HANDLE_BUFFER_DEFINE is used.
 * @param flagsToClear  Flags to be clear.
 *
 * @retval KOSA_StatusSuccess The flags were successfully cleared.
 * @retval KOSA_StatusError   An incorrect parameter was passed.
 */
osa_status_t OSA_EventClear(osa_event_handle_t eventHandle, osa_event_flags_t flagsToClear);

/*!
 * @brief Waits for specified event flags to be set.
 *
 * This function waits for a combination of flags to be set in an event object.
 * Applications can wait for any/all bits to be set. Also this function could
 * obtain the flags who wakeup the waiting task.
 *
 * @param eventHandle     The event handle.
 * The macro EVENT_HANDLE_BUFFER_GET is used to get the event buffer pointer,
 * and should not be used before the macro EVENT_HANDLE_BUFFER_DEFINE is used.
 * @param flagsToWait Flags that to wait.
 * @param waitAll     Wait all flags or any flag to be set.
 * @param millisec    The maximum number of milliseconds to wait for the event.
 *                    If the wait condition is not met, pass osaWaitForever_c will
 *                    wait indefinitely, pass 0 will return KOSA_StatusTimeout
 *                    immediately.
 * @param setFlags    Flags that wakeup the waiting task are obtained by this parameter.
 *
 * @retval KOSA_StatusSuccess The wait condition met and function returns successfully.
 * @retval KOSA_StatusTimeout Has not met wait condition within timeout.
 * @retval KOSA_StatusError   An incorrect parameter was passed.

 *
 * @note    Please pay attention to the flags bit width, FreeRTOS uses the most
 *          significant 8 bis as control bits, so do not wait these bits while using
 *          FreeRTOS.
 *
 */
osa_status_t OSA_EventWait(osa_event_handle_t eventHandle,
                           osa_event_flags_t flagsToWait,
                           uint8_t waitAll,
                           uint32_t millisec,
                           osa_event_flags_t *pSetFlags);

/*!
 * @brief Destroys a previously created event object.
 *
 * @param eventHandle The event handle.
 * The macro EVENT_HANDLE_BUFFER_GET is used to get the event buffer pointer,
 * and should not be used before the macro EVENT_HANDLE_BUFFER_DEFINE is used.
 *
 * @retval KOSA_StatusSuccess The event is successfully destroyed.
 * @retval KOSA_StatusError   Event destruction failed.
 */
osa_status_t OSA_EventDestroy(osa_event_handle_t eventHandle);

/*!
 * @brief Initializes a message queue.
 *
 * This function  allocates memory for and initializes a message queue. Message queue elements are hardcoded as void*.
 *
 * @param msgqHandle    Pointer to a memory space of size #(OSA_MSGQ_HANDLE_SIZE + msgNo*msgSize) on bare-matel
 * and #(OSA_MSGQ_HANDLE_SIZE) on FreeRTOS allocated by the caller, message queue handle.
 * The handle should be 4 byte aligned, because unaligned access does not support on some devices.
 * The macro MSGQ_HANDLE_BUFFER_GET is used to get the message buffer pointer,
 * and should not be used before the macro MSGQ_HANDLE_BUFFER_DEFINE is used.
 * @param msgNo :number of messages the message queue should accommodate.
 * @param msgSize :size of a single message structure.
 *
 * @retval KOSA_StatusSuccess Message queue successfully Create.
 * @retval KOSA_StatusError     Message queue create failure.
 */
osa_status_t OSA_MsgQCreate(osa_msgq_handle_t msgqHandle, uint32_t msgNo, uint32_t msgSize);

/*!
 * @brief Puts a message at the end of the queue.
 *
 * This function puts a message to the end of the message queue. If the queue
 * is full, this function returns the KOSA_StatusError;
 *
 * @param msgqHandle  Message Queue handler.
 * The macro MSGQ_HANDLE_BUFFER_GET is used to get the message buffer pointer,
 * and should not be used before the macro MSGQ_HANDLE_BUFFER_DEFINE is used.
 * @param pMessage Pointer to the message to be put into the queue.
 *
 * @retval KOSA_StatusSuccess Message successfully put into the queue.
 * @retval KOSA_StatusError   The queue was full or an invalid parameter was passed.
 */
osa_status_t OSA_MsgQPut(osa_msgq_handle_t msgqHandle, osa_msg_handle_t pMessage);

/*!
 * @brief Reads and remove a message at the head of the queue.
 *
 * This function gets a message from the head of the message queue. If the
 * queue is empty, timeout is used to wait.
 *
 * @param msgqHandle   Message Queue handler.
 * The macro MSGQ_HANDLE_BUFFER_GET is used to get the message buffer pointer,
 * and should not be used before the macro MSGQ_HANDLE_BUFFER_DEFINE is used.
 * @param pMessage Pointer to a memory to save the message.
 * @param millisec The number of milliseconds to wait for a message. If the
 *                 queue is empty, pass osaWaitForever_c will wait indefinitely,
 *                 pass 0 will return KOSA_StatusTimeout immediately.
 *
 * @retval KOSA_StatusSuccess   Message successfully obtained from the queue.
 * @retval KOSA_StatusTimeout   The queue remains empty after timeout.
 * @retval KOSA_StatusError     Invalid parameter.
 */
osa_status_t OSA_MsgQGet(osa_msgq_handle_t msgqHandle, osa_msg_handle_t pMessage, uint32_t millisec);

/*!
 * @brief Destroys a previously created queue.
 *
 * @param msgqHandle Message Queue handler.
 * The macro MSGQ_HANDLE_BUFFER_GET is used to get the message buffer pointer,
 * and should not be used before the macro MSGQ_HANDLE_BUFFER_DEFINE is used.
 *
 * @retval KOSA_StatusSuccess The queue was successfully destroyed.
 * @retval KOSA_StatusError   Message queue destruction failed.
 */
osa_status_t OSA_MsgQDestroy(osa_msgq_handle_t msgqHandle);

/*!
 * @brief Enable all interrupts.
 */
void OSA_InterruptEnable(void);

/*!
 * @brief Disable all interrupts.
 */
void OSA_InterruptDisable(void);

/*!
 * @brief Enable all interrupts using PRIMASK.
 */
void OSA_EnableIRQGlobal(void);

/*!
 * @brief Disable all interrupts using PRIMASK.
 */
void OSA_DisableIRQGlobal(void);

/*!
 * @brief Delays execution for a number of milliseconds.
 *
 * @param millisec The time in milliseconds to wait.
 */
void OSA_TimeDelay(uint32_t millisec);

/*!
 * @brief This function gets current time in milliseconds.
 *
 * @retval current time in milliseconds
 */
uint32_t OSA_TimeGetMsec(void);

/*!
 * @brief Installs the interrupt handler.
 *
 * @param IRQNumber IRQ number of the interrupt.
 * @param handler The interrupt handler to install.
 */
void OSA_InstallIntHandler(uint32_t IRQNumber, void (*handler)(void));

/*! @}*/
#ifdef __cplusplus
}
#endif
/*! @}*/
#endif
