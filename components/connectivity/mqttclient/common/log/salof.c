/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-27 23:10:36
 * @LastEditTime : 2020-01-16 00:37:56
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
/** synchronous asynchronous log output framework */

#include "salof.h"

#ifndef SALOF_BUFF_SIZE
    #define     SALOF_BUFF_SIZE     (1024U)
#endif 
#ifndef SALOF_FIFO_SIZE
    #define     SALOF_FIFO_SIZE     (2048U)
#endif 

static int salof_out(char *buf, int len);

#if USE_SALOF
#include <string.h>
static fifo_t _salof_fifo = NULL;
static int _len;
static char _out_buff[SALOF_BUFF_SIZE];

#if !USE_IDLE_HOOK
static salof_tcb _salof_task;
void salof_task(void *parm);
#else

#if !defined(salof_handler)
    #error "salof_handler need to be defined as your hook function"
#endif

#endif
#endif

static char _format_buff[SALOF_BUFF_SIZE];

int salof_init(void)
{
#if USE_SALOF
    _salof_fifo = fifo_create(SALOF_FIFO_SIZE);
    if(_salof_fifo == NULL)
        return -1;

#if !USE_IDLE_HOOK
    _salof_task = salof_task_create("salof_task", salof_task, NULL, SALOF_TASK_STACK_SIZE, SALOF_TASK_PRIO, SALOF_TASK_TICK);
    if(_salof_task == NULL)
        return -1;
#endif
#endif
    return 0;
}


void salof(const char *fmt, ...)
{
    va_list args;
    int len;
    va_start(args, fmt);

    len = format_nstr(_format_buff, SALOF_BUFF_SIZE - 1, fmt, args);

    if(len > SALOF_BUFF_SIZE)
        len = SALOF_BUFF_SIZE - 1;

#if USE_SALOF
    fifo_write(_salof_fifo, _format_buff, len, 100);
#else
    salof_out(_format_buff, len);
#endif

  va_end(args);
}

static int salof_out(char *buf, int len)
{
    return send_buff(buf, len);
}

#if USE_SALOF
void salof_handler( void )
{
    _len = fifo_read(_salof_fifo, _out_buff, sizeof(_out_buff), 0);
    if(_len > 0) {
        salof_out((char *)_out_buff, _len);
        memset(_out_buff, 0, _len);
    }
}
#endif

#if !USE_IDLE_HOOK
void salof_task(void *parm)
{   
    (void)parm;
    while(1)
    {
#if USE_SALOF
        salof_handler();
#endif
    } 
}
#endif



