/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-27 23:10:36
 * @LastEditTime: 2020-06-17 15:22:56
 * @Description: the code belongs to jiejie, please keep the author insalof_formation and source code according to the license.
 */
/** synchronous asynchronous log output framework */

#include "salof.h"

#ifdef SALOF_USING_LOG

#ifndef SALOF_BUFF_SIZE
    #define     SALOF_BUFF_SIZE     (1024U)
#endif 
#ifndef SALOF_FIFO_SIZE
    #define     SALOF_FIFO_SIZE     (2048U)
#endif 

static int salof_out(char *buf, int len);

#if SALOF_USING_SALOF
#include <string.h>
static salof_fifo_t _salof_fifo = NULL;
static int _len;
static char _out_buff[SALOF_BUFF_SIZE];

#if !SALOF_USING_IDLE_HOOK
static salof_tcb _salof_task;
void salof_task(void *parm);
#else

#if !defined(salof_handler)
    #error "salof_handler need to be defined as your hook function"
#endif

#endif
#endif

static char _salof_format_buff[SALOF_BUFF_SIZE];

int salof_init(void)
{
#if SALOF_USING_SALOF
    _salof_fifo = salof_fifo_create(SALOF_FIFO_SIZE);
    if(_salof_fifo == NULL)
        return -1;

#if !SALOF_USING_IDLE_HOOK
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

    len = salof_format_nstr(_salof_format_buff, SALOF_BUFF_SIZE - 1, fmt, args);

    if(len > SALOF_BUFF_SIZE)
        len = SALOF_BUFF_SIZE - 1;

#if SALOF_USING_SALOF
    salof_fifo_write(_salof_fifo, _salof_format_buff, len, 100);
#else
    salof_out(_salof_format_buff, len);
#endif

  va_end(args);
}

static int salof_out(char *buf, int len)
{
    return send_buff(buf, len);
}

#if SALOF_USING_SALOF
void salof_handler( void )
{
    _len = salof_fifo_read(_salof_fifo, _out_buff, sizeof(_out_buff), 0);
    if(_len > 0) {
        salof_out((char *)_out_buff, _len);
        memset(_out_buff, 0, _len);
    }
}
#endif

#if !SALOF_USING_IDLE_HOOK
void salof_task(void *parm)
{   
    (void)parm;
    while(1)
    {
#if SALOF_USING_SALOF
        salof_handler();
#endif
    } 
}
#endif

#endif

