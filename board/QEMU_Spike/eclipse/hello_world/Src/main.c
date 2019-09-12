/*
 * main.c
 *
 *  Created on: Sep 13, 2019
 *      Author: ace
 */

#include "tos.h"
#include "riscv_encoding.h"

#define RISCV_MSTATUS_MIE               (1<<3)        /*machine-level interrupt bit*/
#define RISCV_MSTATUS_MPIE              (1<<7)        /*machine-level pre interrupt bit*/
#define RISCV_MSTATUS_MPP               (0x3<<10)     /*machine-level MPP bit*/

#define RISCV_MSTATUS_MPP_MPIE          (RISCV_MSTATUS_MPIE | RISCV_MSTATUS_MPP)
void port_sched_start();
#define STACK_SIZE 512
char stack[STACK_SIZE];
void debug_task() {
	int idle = 0;
	int cnt = 0;
	while(1) {
		idle++;
		cnt++;
	}
}

#include "cmsis_os.h"

//task1
#define TASK1_STK_SIZE 512
void task1(void *pdata);
k_task_t k_task_task1;
uint8_t k_task1_stk[TASK1_STK_SIZE];
int shit = 123;
void task1(void *pdata)
{
	int c = 0;
	while(1)
	{
		c++;
		shit++;
		//k_tick_t delay = tos_millisec2tick(10);
		//tos_task_delay(delay);
		tos_task_yield();
		//osDelay(10);
		//asm("wfi;");
	}
}

void task2(void *pdata);
k_task_t k_task_task2;
uint8_t k_task2_stk[TASK1_STK_SIZE];
void task2(void *pdata)
{
	int c = 0;
	while(1)
	{
		c--;
		shit--;
		//osDelay(10);
		tos_task_yield();
		asm("wfi;");
	}
}

uint32_t *stack_sp;

int main(void)
{
	osKernelInitialize();
#if 0
	while(1) {
		asm("csrs mie, %0"::"r"(MIE_MTIE));
		asm("csrs mstatus, %0"::"r"(MSTATUS_MIE));
		asm("wfi;");
	}
#endif
	tos_task_create(&k_task_task1, "task1", task1, NULL, 3, k_task1_stk, TASK1_STK_SIZE, 0);
	tos_task_create(&k_task_task2, "task2", task2, NULL, 3, k_task2_stk, TASK1_STK_SIZE, 0);
	osKernelStart();
	uint32_t *sp = stack+STACK_SIZE - 4;
	sp = (uint32_t*)(((uint32_t)sp) & 0xFFFFFFF0);

	*(sp - 22) = 0x0ACE0ACE;	// Reg R0: argument
	*(sp - 30) = 0x1234ABCD;	// ra
	*(sp - 31) = debug_task;
	*(sp - 32) = RISCV_MSTATUS_MPIE | RISCV_MSTATUS_MPP;
	sp -= 32;
	stack_sp = sp;
	port_sched_start();
	int c = 0;
	while(1) {
		c++;
	}

}
