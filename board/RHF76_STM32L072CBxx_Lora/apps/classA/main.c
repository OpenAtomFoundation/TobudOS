#include "LoRaApi.h"
#include <stdio.h>
#include <string.h>
#include "tos_k.h"
#include "atcmd-board.h"
/*LORA入网状态*/
uint8_t g_join_state = 0;
uint8_t g_st_state = 0;
/*报文计数*/
uint32_t g_rx_num = 0;

//任务优先级
#define TASK1_PRIO		3
//任务堆栈大小
#define TASK1_STK_SIZE 		(1024 * 4)
//任务控制块
k_task_t Task1TCB;
//任务堆栈
k_stack_t TASK1_STK[TASK1_STK_SIZE];
//任务函数
void task1(void *arg);
void lora_send_timer(void* context)
{
	g_st_state =2;
}
void task1(void *arg)
{
	at_cmd_init();
	LoRa_Init();
	printf_device_info();
	TimerInit( &TxNextPacketTimer, lora_send_timer );
	while(1)
	{
		LoRaMacProcess();
		if(g_join_state == 0)
		{
				printf("start join...\r\n");
				/*入网*/
				LoRa_JoinNetwork(1);
				g_join_state = 1;
		}
		if(g_join_state == 2)
		{
				/*入网后定时发送数据*/
				if(g_st_state == 0)
				{
						g_st_state = 1;
						TimerSetValue( &TxNextPacketTimer, 60000 );
						TimerStart( &TxNextPacketTimer);
						printf("\r\ntime to ");
				}
				else if(g_st_state == 2)
				{
						g_st_state = 3;
						printf("send!\r\n");
						/*发送lora数据*/
						uint8_t tmpbuf[] = "tos tiny!";
						Lora_Send(0, 2, tmpbuf, sizeof(tmpbuf));
						g_rx_num++;
				}
		}
		at_cmd_handle();
	}
}

extern uint8_t sx1276_band;
int main(void)
{
	k_err_t err;
	TOS_CPU_CPSR_ALLOC();
	BoardInitMcu( );
	BoardInitPeriph( );
	sx1276_band = 2;
	printf("welcome to tencent IoTOS\r\n");
	err = tos_knl_init();		//初始化TOS
	if (err != K_ERR_NONE)
	{
		printf("tos init failed\r\n");
		return err;
	}
	TOS_CPU_INT_DISABLE();//进入临界区
	err = tos_task_create((k_task_t 	* )&Task1TCB,		//任务控制块
					 (char	* )"task1", 		//任务名字
									 (k_task_entry_t )task1, 			//任务函数
									 (void		* )0,					//传递给任务函数的参数
									 (k_prio_t	  )TASK1_PRIO,     //任务优先级
									  TASK1_STK,	//任务堆栈基地址
									 (size_t)TASK1_STK_SIZE,		//任务堆栈大小
									 (size_t	  )0);			//当使能时间片轮转时的时间片长度，为0时为默认长度，

	TOS_CPU_INT_ENABLE();	//退出临界区
	tos_knl_start();      //开启TOS
  while (1)
  {

  }
}

