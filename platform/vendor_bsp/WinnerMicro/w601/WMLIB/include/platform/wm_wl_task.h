/**
 * @file    wm_wl_task.h
 *
 * @brief   task APIs
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef __TLS_WL_TASK_H__
#define __TLS_WL_TASK_H__

#include "wm_type_def.h"
#include "wm_wl_mbox.h"
#include "wm_wl_timers.h"
//#include "ithread.h"

#define TLS_TASK_START_PRIO                 0
#define TASK_WL_PRIO                        9
#define TASK_WL_PRIO_MAX                    12
#define TLS_SUPPLICANT_TASK_PRIO            (TASK_WL_PRIO_MAX + 1)
#define TLS_SUPPLICANT_TIMER_TASK_PRIO      (TASK_WL_PRIO_MAX + 2)
#define TLS_LWIP_TASK_PRIO                  (TASK_WL_PRIO_MAX + 3)
#define TLS_SYS_TASK_PRIO                   (TASK_WL_PRIO_MAX + 5)
#define TLS_HOSTIF_TASK_PRIO                (TASK_WL_PRIO_MAX + 6)
#define TLS_SPI_SCHEDULER_TASK_PRIO         (TASK_WL_PRIO_MAX + 7)
#define TLS_FWUP_TASK_PRIO                  (TASK_WL_PRIO_MAX + 8)
#define TLS_HTTP_CLIENT_TASK_PRIO           (TASK_WL_PRIO_MAX + 9)
#define AP_SOCKET_S_TASK_PRIO               (TASK_WL_PRIO_MAX + 10)
#define TLS_UPNP_TASK_PRIO                  (TASK_WL_PRIO_MAX + 11)
#define TLS_ONESHOT_TASK_PRIO          		(TASK_WL_PRIO_MAX + 15)
#define TLS_ONESHOT_SPEC_TASK_PRIO			(TASK_WL_PRIO_MAX + 16)


#define TLS_MBOX_ALL_COUNT                  8
#define TLS_MBOX_ID_WL_TASK                 0
#define TLS_MBOX_ID_HOSTIF_TASK             1
#define TLS_MBOX_ID_JDCLOUD_SERVER          2
#define TLS_MBOX_ID_JDCLOUD_DATA            3
#define TLS_MBOX_ID_UPNP_HD                 4
#define TLS_MBOX_ID_UPNP_COMMON             5
#define TLS_MBOX_ID_UPNP_GENA               6
#define TLS_MBOX_ID_UPNP_MINISERVER         7

#define TLS_TIMEO_ALL_COUONT                9
#define TLS_TIMEO_ID_NULL                   0
#define TLS_TIMEO_ID_WL_TASK                1
#define TLS_TIMEO_ID_HOSTIF_TASK            2
#define TLS_TIMEO_ID_JDCLOUD_SERVER         3
#define TLS_TIMEO_ID_JDCLOUD_DATA           4
#define TLS_TIMEO_ID_UPNP_HD                5
#define TLS_TIMEO_ID_UPNP_COMMON            6
#define TLS_TIMEO_ID_UPNP_GENA              7
#define TLS_TIMEO_ID_UPNP_MINISERVER        8

#define TLS_MSG_ALL_COUONT                  9
#define TLS_MSG_ID_TX_MGMT_CMPLT            0
#define TLS_MSG_ID_MLME_TASK                1
#define TLS_MSG_ID_UART_SENT_FREE           2
#define TLS_MSG_ID_UART0_RX                 3
#define TLS_MSG_ID_HSPI_RX_CMD              4
#define TLS_MSG_ID_HSPI_RX_DATA             5
#define TLS_MSG_ID_HSPI_TX_DATA             6
#define TLS_MSG_ID_TX_DATA_CMPLT            7
#define TLS_MSG_ID_UART1_RX                 8

/** pointer to the semaphore */
typedef tls_os_sem_t * tls_sem_t;

/** Thread start routine */
typedef void *(*start_routine)(void *arg);

/** message type of task */
enum task_msg_type {
  TASK_MSG_TIMEOUT,
  TASK_MSG_UNTIMEOUT,
  TASK_MSG_CALLBACK_WITH_BLOCK,
  TASK_MSG_CALLBACK,
  TASK_MSG_CALLBACK_STATIC,
  TASK_MSG_NULL
};

/** message of task */
struct task_msg {
  enum task_msg_type type;
  tls_sem_t *sem;
  union {
    struct {
      start_routine function;
      void *ctx;
    } cb;
    struct {
      start_routine function;
      void *ctx;
      u8 cnt;
    } cbs;
    struct {
      u32 msecs;
      tls_timeout_handler h;
      void *arg;
    } tmo;
  } msg;
};

/** task parameters */
struct task_parameter{
	u8 task_id;             /**< task ID */
	const char * name;      /**< task name */
	u8 *stk_start;          /**< start address of task stack */
	u32 stk_size;           /**< size of task stack */
	u8 mbox_size;           /**< size of mailbox */
    u8 mbox_id;             /**< mailbox ID */
    u8 timeo_id;            /**< timer ID */
};

/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup Task_APIs Task APIs
 * @brief Software task APIs
 */

/**
 * @addtogroup Task_APIs
 * @{
 */

/**
 * @brief          Task initialized
 *
 * @param          None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
s8 tls_wl_task_init(void);

/**
 * @brief          Running the task
 *
 * @param[in]      *task_param    pointer to the task parameters
 *
 * @retval         0              success
 * @retval         other          failed
 *
 * @note           None
 */
s8 tls_wl_task_run(struct task_parameter *task_param);

/**
 * @brief          Running the callback function
 *
 * @param[in]      *task_param      pointer to the task parameters
 * @param[in]      function         the callback function
 * @param[in]      *ctx             parameter of the callback function
 * @param[in]      block
 * @param[in]      msg_id
 *
 * @retval         TLS_OS_SUCCESS   success
 * @retval         TLS_OS_ERROR     failed
 *
 * @note           None
 */
s8 tls_wl_task_callback_static(struct task_parameter *task_param,
                               start_routine function, void *ctx, u8 block, u8 msg_id);

/**
 * @brief          Running the callback function
 *
 * @param[in]      *task_param      pointer to the task parameters
 * @param[in]      function         the callback function
 * @param[in]      *ctx             parameter of the callback function
 * @param[in]      block
 *
 * @retval         TLS_OS_SUCCESS   success
 * @retval         TLS_OS_ERROR     failed
 *
 * @note           None
 */
s8 tls_wl_task_callback(struct task_parameter *task_param,
                        start_routine function, void *ctx, u8 block);

/**
 * @brief          Add a timer to the task
 *
 * @param[in]      *task_param      pointer to the task parameters
 * @param[in]      msecs            timer value
 * @param[in]      h                the callback function
 * @param[in]      *arg             parameter of the callback function
 *
 * @retval         TLS_OS_SUCCESS   success
 * @retval         TLS_OS_ERROR     failed
 *
 * @note           None
 */
s8 tls_wl_task_add_timeout(struct task_parameter *task_param, u32 msecs,
                           tls_timeout_handler h, void *arg);

/**
 * @brief          Stop or delay the timer to expire.
 *
 * @param[in]      *task_param          pointer to the task parameters
 * @param[in]      h                    the callback function
 * @param[in]      *arg                 parameter of the callback function
 *
 * @retval         TLS_OS_SUCCESS       success
 * @retval         TLS_OS_ERROR         failed
 *
 * @note           None
 */
s8 tls_wl_task_untimeout(struct task_parameter *task_param,
                         tls_timeout_handler h, void *arg);

/**
 * @}
 */

/**
 * @}
 */

#endif

