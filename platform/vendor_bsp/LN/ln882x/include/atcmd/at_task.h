
#ifndef AT_TASK_H
#define AT_TASK_H
#include "proj_config.h"
#include "types.h"
#include "osal/osal.h"


typedef struct
{
    uint8_t len;
    char *pBuffer;
#ifdef __CONFIG_OS_KERNEL
    OS_Semaphore_t              *sem;
#endif
}AT_MSG_T;

#ifdef __CONFIG_OS_KERNEL
  #define ATMSG_QUEUE_LEN     6
  extern OS_Queue_t g_at_msg_queue;
  extern OS_Thread_t g_at_thread;

  void at_task(void *params);
#endif

extern void at_init(void);

#endif


