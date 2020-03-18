#include "cmsis_os.h"
#include "embARC.h"
#include "embARC_debug.h"

static uint32_t cyc_hz_count = (BOARD_CPU_CLOCK / BOARD_SYS_TIMER_HZ);

static void board_timer_isr(void *ptr)
{
	timer_int_clear(BOARD_SYS_TIMER_ID);

	board_timer_update(BOARD_SYS_TIMER_HZ);
}

static void board_timer_init(void)
{
	if (timer_present(BOARD_SYS_TIMER_ID)) {
		int_disable(BOARD_SYS_TIMER_INTNO);                                             /* disable first then enable */
		int_handler_install(BOARD_SYS_TIMER_INTNO, board_timer_isr);
		timer_start(BOARD_SYS_TIMER_ID, TIMER_CTRL_IE | TIMER_CTRL_NH, cyc_hz_count);   /* start 1ms timer interrupt */

		int_enable(BOARD_SYS_TIMER_INTNO);
	}
}

#define APPLICATION_TASK_STK_SIZE       4096
extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

EMBARC_WEAK void application_entry(void *arg)
{
    while (1) {
        printf("This is a demo task,please use your task entry!\r\n");
        tos_task_delay(1000);
    }
}

int main(void)
{
    exc_int_init();
    arc_cache_init();
    board_init();
    board_timer_init();
    xprintf_setup();
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize(); // TOS Tiny kernel initialize
    osThreadCreate(osThread(application_entry), NULL); // Create TOS Tiny task
    osKernelStart(); // Start TOS Tiny
}

