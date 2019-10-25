#include "E53_ST1.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "usart.h"
#include "main.h"

gps_msg             gpsmsg;
static unsigned char gps_uart[1000];


TIM_HandleTypeDef htim16;

/***************************************************************
* 函数名称: MX_TIM16_Init
* 说    明: 初始化定时器16
* 参    数: 无
* 返 回 值: 无
***************************************************************/
/* TIM16 init function */
void MX_TIM16_Init(void)
{
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 79;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 999;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim16);

}
/***************************************************************
* 函数名称: HAL_TIM_Base_MspInit
* 说    明: 使能定时器16时钟,设置时钟源
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

	if(tim_baseHandle->Instance==TIM16)
  {
  /* USER CODE BEGIN TIM16_MspInit 0 */

  /* USER CODE END TIM16_MspInit 0 */
    /* TIM16 clock enable */
    __HAL_RCC_TIM16_CLK_ENABLE();
  /* USER CODE BEGIN TIM16_MspInit 1 */

  /* USER CODE END TIM16_MspInit 1 */
  }
}

/***************************************************************
* 函数名称: HAL_TIM_MspPostInit
* 说    明: 使能定时器16硬件初始化
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(timHandle->Instance==TIM16)
  {
  /* USER CODE BEGIN TIM16_MspPostInit 0 */

  /* USER CODE END TIM16_MspPostInit 0 */
  
    /**TIM16 GPIO Configuration    
    PB8     ------> TIM16_CH1 
    */
    GPIO_InitStruct.Pin = ST1_Beep_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_TIM16;
    HAL_GPIO_Init(ST1_Beep_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM16_MspPostInit 1 */

  /* USER CODE END TIM16_MspPostInit 1 */
  }

}

/***************************************************************
* 函数名称: Init_Beep
* 说    明: 初始化E53_ST1的蜂鸣器
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_Beep(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  ST1_Beep_GPIO_CLK_ENABLE();

	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ST1_Beep_GPIO_Port, ST1_Beep_Pin, GPIO_PIN_RESET);
	
	 /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ST1_Beep_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ST1_Beep_GPIO_Port, &GPIO_InitStruct);
}

/***************************************************************
* 函数名称: Init_LED
* 说    明: 初始化E53_ST1的LED灯
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_LED(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  ST1_LED_GPIO_CLK_ENABLE();

	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ST1_LED_GPIO_Port, ST1_LED_Pin, GPIO_PIN_SET);
	
	 /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ST1_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ST1_LED_GPIO_Port, &GPIO_InitStruct);
}
/***************************************************************
* 函数名称: Init_GPS_POW
* 说    明: 初始化E53_ST1的GPS使能引脚
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_GPS_POW(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  ST1_GPS_POW_GPIO_CLK_ENABLE();

	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ST1_GPS_POW_GPIO_Port, ST1_GPS_POW_Pin, GPIO_PIN_RESET);
	
	 /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ST1_GPS_POW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ST1_GPS_POW_GPIO_Port, &GPIO_InitStruct);
}
/***************************************************************
* 函数名称: GPS_Init
* 说    明: 初始化GPS模块
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void GPS_Init(void)
{
	MX_USART3_UART_Init();   //初始化串口
	HAL_UART_Transmit(&huart3, "$CCMSG,GGA,1,0,*19\r\n", 20, 200);
	HAL_UART_Transmit(&huart3, "$CCMSG,GSA,1,0,*0D\r\n", 20, 200);
	HAL_UART_Transmit(&huart3, "$CCMSG,GSV,1,0,*1A\r\n", 20, 200);
}

/***************************************************************
* 函数名称: Init_E53_ST1
* 说    明: 初始化E53_ST1扩展板
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_E53_ST1(void)
{
	GPS_Init();
	Init_Beep();
	Init_LED();
	Init_GPS_POW();	
	MX_TIM16_Init();	
}

/***************************************************\
* 函数名称: NMEA_Comma_Pos
*	函数功能：从buf里面得到第cx个逗号所在的位置
*	输入值：
*	返回值：0~0xFE，代表逗号所在位置的便宜
*				 	0xFF，代表不存在第cx个逗号
\***************************************************/

uint8_t NMEA_Comma_Pos(uint8_t *buf,uint8_t cx)
{
	uint8_t *p = buf;
	while(cx)
	{
		if(*buf=='*'||*buf<' '||*buf>'z')return 0xFF;
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;
}
/***************************************************\
* 函数名称: NMEA_Pow
*	函数功能：返回m的n次方值
*	输入值：底数m和指数n
*	返回值：m^n
\***************************************************/
uint32_t NMEA_Pow(uint8_t m,uint8_t n)
{
	uint32_t result = 1;
	while(n--)result *= m;
	return result;
}
/***************************************************\
* 函数名称: NMEA_Str2num
*	函数功能：str数字转换为（int）数字，以','或者'*'结束
*	输入值：buf，数字存储区
*				 	dx，小数点位数，返回给调用函数
*	返回值：转换后的数值
\***************************************************/
int NMEA_Str2num(uint8_t *buf,uint8_t*dx)
{
	uint8_t *p = buf;
	uint32_t ires = 0,fres = 0;
	uint8_t ilen = 0,flen = 0,i;
	uint8_t mask = 0;
	int res;
	while(1)
	{
		if(*p=='-'){mask |= 0x02;p++;}//说明有负数
		if(*p==','||*p=='*')break;//遇到结束符
		if(*p=='.'){mask |= 0x01;p++;}//遇到小数点
		else if(*p>'9'||(*p<'0'))//数字不在0和9之内，说明有非法字符
		{
			ilen = 0;
			flen = 0;
			break;
		}
		if(mask&0x01)flen++;//小数点的位数
		else ilen++;//str长度加一
		p++;//下一个字符
	}
	if(mask&0x02)buf++;//移到下一位，除去负号
	for(i=0;i<ilen;i++)//得到整数部分数据
	{
		ires += NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;//最多取五位小数
	*dx = flen;
	for(i=0;i<flen;i++)//得到小数部分数据
	{
		fres +=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	}
	res = ires*NMEA_Pow(10,flen)+fres;
	if(mask&0x02)res = -res;
	return res;
}
/***************************************************\
* 函数名称: NMEA_BDS_GPRMC_Analysis
*	函数功能：解析GPRMC信息
*	输入值：gpsx,NMEA信息结构体
*				 buf：接收到的GPS数据缓冲区首地址
\***************************************************/
void NMEA_BDS_GPRMC_Analysis(gps_msg *gpsmsg,uint8_t *buf)
{
	uint8_t *p4,dx;			 
	uint8_t posx;     
	uint32_t temp;	   
	float rs;  
	p4=(uint8_t*)strstr((const char *)buf,"$GPRMC");//"$GPRMC",经常有&和GPRMC分开的情况,故只判断GPRMC.
	posx=NMEA_Comma_Pos(p4,3);								//得到纬度
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p4+posx,&dx);		 	 
		gpsmsg->latitude_bd=temp/NMEA_Pow(10,dx+2);	//得到°
		rs=temp%NMEA_Pow(10,dx+2);				//得到'		 
		gpsmsg->latitude_bd=gpsmsg->latitude_bd*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//转换为° 
	}
	posx=NMEA_Comma_Pos(p4,4);								//南纬还是北纬 
	if(posx!=0XFF)gpsmsg->nshemi_bd=*(p4+posx);					 
 	posx=NMEA_Comma_Pos(p4,5);								//得到经度
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p4+posx,&dx);		 	 
		gpsmsg->longitude_bd=temp/NMEA_Pow(10,dx+2);	//得到°
		rs=temp%NMEA_Pow(10,dx+2);				//得到'		 
		gpsmsg->longitude_bd=gpsmsg->longitude_bd*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//转换为° 
	}
	posx=NMEA_Comma_Pos(p4,6);								//东经还是西经
	if(posx!=0XFF)gpsmsg->ewhemi_bd=*(p4+posx);		  
}

/***************************************************************
* 函数名称: E53_ST1_Read_Data
* 说    明: 获取GPS经纬度信息
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void E53_ST1_Read_Data(void)
{	
	HAL_UART_Receive_IT(&huart3,gps_uart,1000);
	NMEA_BDS_GPRMC_Analysis(&gpsmsg,(uint8_t*)gps_uart);	//分析字符串
	E53_ST1_Data.Longitude=(float)((float)gpsmsg.longitude_bd/100000);	
	E53_ST1_Data.Latitude=(float)((float)gpsmsg.latitude_bd/100000);    
}

/***************************************************************
* 函数名称: E53SF1_LED_StatusSet
* 说    明: E53SF1开发板上的LED灯的亮灭控制
* 参    数: status,LED灯的状态
*									非1,关灯
*									1,开灯
* 返 回 值: 无
***************************************************************/
void E53_ST1_LED_StatusSet(E53ST1_Status_ENUM status)
{
	HAL_GPIO_WritePin(ST1_LED_GPIO_Port, ST1_LED_Pin, status != ST1_ON ? GPIO_PIN_RESET : GPIO_PIN_SET );	
}

/***************************************************************
* 函数名称: E53SF1_BEEP
* 说    明: E53SF1蜂鸣器报警与否
* 参    数: status,LED_ENUM枚举的数据
*									LED_OFF,关灯
*									LED_ON,开灯
* 返 回 值: 无
***************************************************************/
void E53_ST1_Beep(E53ST1_Status_ENUM status)
{
	if(status == ST1_ON)
		HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
	if(status == ST1_OFF)
		HAL_TIM_PWM_Stop(&htim16,TIM_CHANNEL_1);
}


