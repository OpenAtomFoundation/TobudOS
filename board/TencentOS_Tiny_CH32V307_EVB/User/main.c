/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*******************************************************************************/
#include "debug.h"
#include "tos_k.h"
#include "lcd_init.h"
#include "lcd.h"
#include "pic.h"


void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}
#define TASK1_STK_SIZE       1024
k_task_t task1;
__aligned(4) uint8_t task1_stk[TASK1_STK_SIZE];


#define TASK2_STK_SIZE       1024
k_task_t task2;
__aligned(4) uint8_t task2_stk[TASK2_STK_SIZE];


#define APPLICATION_TASK_STK_SIZE       4096
k_task_t application_task;
__aligned(4) uint8_t application_task_stk[APPLICATION_TASK_STK_SIZE];

extern void application_entry1(void *arg);

void task1_entry(void *arg)
{
    while (1)
    {
        printf("###I am task1\r\n");
        tos_task_delay(2000);
    }
}

void task2_entry(void *arg)
{
    while (1)
    {
        printf("***I am task2\r\n");
        tos_task_delay(1000);
    }
}


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
int main(void)
{
    u8 i,j;
    float t=0;
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	led_init();
    LCD_Init();//LCD≥ı ºªØ
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);

    printf("Welcome to TencentOS tiny(%s)\r\n", TOS_VERSION);
    LCD_ShowString(0,0,"Welcome to TencentOS",RED,WHITE,16,0);
    tos_knl_init();
    tos_task_create(&application_task, "application_task", application_entry1, NULL, 4, application_task_stk, APPLICATION_TASK_STK_SIZE, 0);
    //tos_task_create(&task1, "task1", task1_entry, NULL, 3, task1_stk, TASK1_STK_SIZE, 0); // Create task1
    //tos_task_create(&task2, "task2", task2_entry, NULL, 3, task2_stk, TASK2_STK_SIZE, 0);// Create task2
    tos_knl_start();

    printf("should not run at here!\r\n");

	while(1)
	{
	    LCD_ShowString(0,0,"Welcome TencentOS",RED,WHITE,32,0);
        LCD_ShowString(0,40,"LCD_W:",RED,WHITE,16,0);
        LCD_ShowIntNum(48,40,LCD_W,3,RED,WHITE,16);
        LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
        LCD_ShowIntNum(128,40,LCD_H,3,RED,WHITE,16);
        LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
        LCD_ShowString(0,70,"Increaseing Nun:",RED,WHITE,16,0);
        LCD_ShowFloatNum1(128,70,t,4,RED,WHITE,16);
        t+=0.11;
        for(j=0;j<3;j++)
        {
            for(i=0;i<6;i++)
            {
                LCD_ShowPicture(40*i,120+j*40,40,40,gImage_1);
                printf("pic test\r\n");
            }
        }
        printf("lcd test\r\n");
	}
}







