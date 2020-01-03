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

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Armink (armink.ztl@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "tos_k.h"

#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

extern k_task_t            *k_curr_task;
extern k_task_t            *k_next_task;

__PORT__ void _handle_tick_signal();
__PORT__ void _handle_context_switch();
__PORT__ void _suspend_thread(pthread_t thread_id);
__PORT__ void _resume_thread(pthread_t thread_id);
__PORT__ void _wait_resume();
__PORT__ void _handle_suspend_thread();
__PORT__ void _handle_resume_thread();
__PORT__ void *_thread_entry(void *arg);
__PORT__ void _suspend_task(k_task_t *task);
__PORT__ void _resume_task(k_task_t *task);
__PORT__ void _install_signal(int sig,void (*func)(int));
__PORT__ void _filter_signal(sigset_t *sigset);
__PORT__ uint64_t _get_time_ms(void);

typedef struct {
    sigset_t signal_mask;
    uint32_t count;
}interrupt_manager;

interrupt_manager _int_manager={
    .count = 0
};

static uint64_t tick_ms = 0;
static pthread_t main_thread_id;
static pthread_mutex_t cpsr_mutex;

#define CHECK_IS_MAIN_THREAD(main_thread_id) \
    if(main_thread_id != pthread_self()) return

__PORT__ void port_int_disable(void)
{
    sigset_t signal_mask,*manager_mask=NULL;
    if(_int_manager.count == 0){
        pthread_mutex_lock(&cpsr_mutex);
        manager_mask = &(_int_manager.signal_mask);
        sigfillset(&signal_mask);
        _filter_signal(&signal_mask);
        pthread_sigmask(SIG_BLOCK,&signal_mask,manager_mask);
    }
    _int_manager.count ++;
}

__PORT__ void port_int_enable(void)
{
    if(_int_manager.count == 0) return;

    _int_manager.count--;
    if(_int_manager.count == 0){
        sigaddset(&(_int_manager.signal_mask),SIG_RESUME); //ensure SIG_RESUME is blocked
        _filter_signal(&(_int_manager.signal_mask));
        pthread_mutex_unlock(&cpsr_mutex);
        pthread_sigmask(SIG_SETMASK,&(_int_manager.signal_mask),NULL);
    }
}

__PORT__ cpu_cpsr_t port_cpsr_save(void)
{
    port_int_disable();
    return (cpu_cpsr_t)_int_manager.count;
}

__PORT__ void port_cpsr_restore(cpu_cpsr_t cpsr)
{
    port_int_enable();
}

__PORT__ pthread_t  port_create_thread(void *arg)
{
    pthread_t thread_id;
    pthread_create(&thread_id,NULL,_thread_entry,arg);
    return thread_id;
}

__PORT__ void port_sched_start(void)
{
    k_curr_task = k_next_task;
    _resume_task(k_curr_task);

    while(1){
        _wait_resume();
    }
}

__PORT__ void port_context_switch(void)
{
    raise(SIG_CONTEXT_SWITCH);
}

__PORT__ void port_irq_context_switch(void)
{
    raise(SIG_CONTEXT_SWITCH);
}

__PORT__ void port_cpu_reset(void)
{
    printf("func %s\n",__FUNCTION__);
    exit(0);
}

__PORT__ void port_systick_config(uint32_t cycle_per_tick)
{
    struct itimerval itimer, oitimer;
    uint32_t timeer_usec = cycle_per_tick/(TOS_CFG_CPU_CLOCK/1000000u);

    _install_signal(SIG_TICK,_handle_tick_signal);

	/* Initialise the structure with the current timer information. */
	if ( 0 == getitimer( TIMER_TYPE, &itimer ) )
	{
		/* Set the interval between timer events. */
		itimer.it_interval.tv_sec = 0;
		itimer.it_interval.tv_usec = timeer_usec;

		/* Set the current count-down. */
		itimer.it_value.tv_sec = 0;
		itimer.it_value.tv_usec = timeer_usec;

		/* Set-up the timer interrupt. */
		if ( 0 != setitimer( TIMER_TYPE, &itimer, &oitimer ) )
		{
			printf( "set timer problem.\n" );
		}
	}
	else
	{
		printf( "get timer problem.\n" );
	}
}

__PORT__ void port_init(void)
{
    main_thread_id = pthread_self();
    pthread_mutex_init(&cpsr_mutex,NULL);
    _install_signal(SIG_SUSPEND, _handle_suspend_thread);
    _install_signal(SIG_RESUME, _handle_resume_thread);
    _install_signal(SIG_CONTEXT_SWITCH, _handle_context_switch);
}

__PORT__ void port_delay_ms(uint32_t ms)
{
    uint64_t start_time = _get_time_ms();
    do{
        usleep(100);
    }while((_get_time_ms() - start_time) < ms);
}

__PORT__ void _filter_signal(sigset_t *sigset)
{
    sigdelset(sigset,SIGINT);
    sigdelset(sigset,SIGKILL); //ensure program cloud be kill
}


__PORT__ uint64_t _get_time_ms(void)
{
    return (tick_ms);
}

__PORT__ void _install_signal(int sig,void (*func)(int))
{
    struct sigaction sig_install;

    sig_install.sa_flags = 0;
	sig_install.sa_handler = func;
	sigfillset( &sig_install.sa_mask );

    if ( 0 != sigaction( sig, &sig_install, NULL ) )
	{
		printf( "problem installing %d\n",sig );
	}
}

__PORT__ void _handle_tick_signal()
{
    CHECK_IS_MAIN_THREAD(main_thread_id);
    tick_ms ++;
    if(tos_knl_is_running()) {
        tos_knl_irq_enter();
        tos_tick_handler();
        tos_knl_irq_leave();
    }
}

__PORT__ void _suspend_thread(pthread_t thread_id)
{
    pthread_kill(thread_id,SIG_SUSPEND);
}

__PORT__ void _resume_thread(pthread_t thread_id)
{
    pthread_kill(thread_id,SIG_RESUME);
}

__PORT__ void _suspend_task(k_task_t *task)
{
    cpu_context_t *context = (cpu_context_t *)(task->sp);
    pthread_t thread_id = context->thread_id;
    _suspend_thread(thread_id);
}

__PORT__ void _resume_task(k_task_t *task)
{
    cpu_context_t *context = (cpu_context_t *)(task->sp);
    pthread_t thread_id = context->thread_id;
    _resume_thread(thread_id);
}

__PORT__ void _wait_resume()
{
    sigset_t signals;
    int sig;
    sigemptyset( &signals );
	sigaddset( &signals, SIG_RESUME);
    pthread_sigmask(SIG_BLOCK, &signals,NULL);
    sigwait(&signals,&sig);
}

__PORT__ void _handle_suspend_thread()
{
    _wait_resume();
}

__PORT__ void _handle_resume_thread()
{
    printf("some thread not block SIG_RESUME\n");
}

__PORT__ void *_thread_entry(void *arg)
{
    cpu_context_t *params = (cpu_context_t *)arg;
    _wait_resume();
    params->entry(params->arg);
    params->exit(params->arg);
}

__PORT__ void _handle_context_switch()
{
    _suspend_task(k_curr_task);
    k_curr_task = k_next_task;
    _resume_task(k_curr_task);
}

#if TOS_CFG_TICKLESS_EN > 0u

__PORT__ k_time_t port_systick_max_delay_millisecond(void)
{

}

__PORT__ void port_systick_resume(void)
{

}

__PORT__ void port_systick_suspend(void)
{

}

__PORT__ void port_systick_reload(uint32_t cycle_per_tick)
{

}

__PORT__ void port_systick_pending_reset(void)
{

}

#endif

#if TOS_CFG_PWR_MGR_EN > 0u

__PORT__ void port_sleep_mode_enter(void)
{

}

__PORT__ void port_stop_mode_enter(void)
{

}

__PORT__ void port_standby_mode_enter(void)
{

}

#endif

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
__STATIC__ void port_fault_do_diagnosis(port_fault_regs_t *regs)
{
    k_fault_log_writer("\n\n====================== Fault Diagnosis =====================\n");

}

__PORT__ void port_fault_diagnosis(void)
{
    port_fault_regs_t regs;

    port_fault_do_diagnosis(&regs);
}

// implementation jump to fault_backtrace

#endif /* TOS_CFG_FAULT_BACKTRACE_EN */

