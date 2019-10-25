/* 包含头文件 ----------------------------------------------------------------*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "DHT11_BUS.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define Delay_ms(x)   HAL_Delay(x)
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
static uint8_t DHT11_ReadByte(void);

/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能:
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DHT11_Delay(uint16_t time)
{
	uint8_t i;

  while(time)
  {
	  for (i = 0; i < 10; i++)
    {

    }
    time--;
  }
}

/**
  * 函数功能: DHT11 初始化函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void DHT11_Init ( void )
{
    DHT11_Dout_GPIO_CLK_ENABLE();

    DHT11_Mode_Out_PP();

    DHT11_Dout_HIGH();  // 拉高GPIO
}

/**
  * 函数功能: 使DHT11-DATA引脚变为上拉输入模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DHT11_Mode_IPU(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.Pin   = DHT11_Dout_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_Dout_PORT, &GPIO_InitStruct);

}

/**
  * 函数功能: 使DHT11-DATA引脚变为推挽输出模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DHT11_Mode_Out_PP(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.Pin = DHT11_Dout_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT11_Dout_PORT, &GPIO_InitStruct);
}

/**
  * 函数功能: 从DHT11读取一个字节，MSB先行
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static uint8_t DHT11_ReadByte ( void )
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)
	{
		/*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/
		while(DHT11_Data_IN()==GPIO_PIN_RESET);

		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时
		 */
		DHT11_Delay(40); //延时x us 这个延时需要大于数据0持续的时间即可

		if(DHT11_Data_IN()==GPIO_PIN_SET)/* x us后仍为高电平表示数据“1” */
		{
			/* 等待数据1的高电平结束 */
			while(DHT11_Data_IN()==GPIO_PIN_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行
		}
		else	 // x us后为低电平表示数据“0”
		{
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
		}
	}
	return temp;
}


/*
 *
 *
 */
/**
  * 函数功能: 一次完整的数据传输为40bit，高位先出
  * 输入参数: DHT11_Data:DHT11数据类型
  * 返 回 值: ERROR：  读取出错
  *           SUCCESS：读取成功
  * 说    明：8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和
  */
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{
    uint8_t temp;
    uint16_t humi_temp;

	/*输出模式*/
	DHT11_Mode_Out_PP();
	/*主机拉低*/
	DHT11_Dout_LOW();
	/*延时18ms*/
	Delay_ms(18);
	/*总线拉高 主机延时30us*/
	DHT11_Dout_HIGH();

	DHT11_Delay(30);   //延时30us

	/*主机设为输入 判断从机响应信号*/
	DHT11_Mode_IPU();

	/*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/
	if(DHT11_Data_IN()==GPIO_PIN_RESET)
	{
    /*轮询直到从机发出 的80us 低电平 响应信号结束*/
    while(DHT11_Data_IN()==GPIO_PIN_RESET);

    /*轮询直到从机发出的 80us 高电平 标置信号结束*/
    while(DHT11_Data_IN()==GPIO_PIN_SET);

    cpu_cpsr_t up = tos_cpu_cpsr_save();

    /*开始接收数据*/
    DHT11_Data->humi_high8bit= DHT11_ReadByte();
    DHT11_Data->humi_low8bit = DHT11_ReadByte();
    DHT11_Data->temp_high8bit= DHT11_ReadByte();
    DHT11_Data->temp_low8bit = DHT11_ReadByte();
    DHT11_Data->check_sum    = DHT11_ReadByte();

    tos_cpu_cpsr_restore(up);

    /*读取结束，引脚改为输出模式*/
    DHT11_Mode_Out_PP();
    /*主机拉高*/
    DHT11_Dout_HIGH();

    /* 对数据进行处理 */
    humi_temp=DHT11_Data->humi_high8bit*100+DHT11_Data->humi_low8bit;
    DHT11_Data->humidity =(float)humi_temp/100;
    
    humi_temp=DHT11_Data->temp_high8bit*100+DHT11_Data->temp_low8bit;
    DHT11_Data->temperature=(float)humi_temp/100;    
    
    /*检查读取的数据是否正确*/
    temp = DHT11_Data->humi_high8bit + DHT11_Data->humi_low8bit +
           DHT11_Data->temp_high8bit+ DHT11_Data->temp_low8bit;

    if(DHT11_Data->check_sum==temp)
    {
        return SUCCESS;
    }
    else
        return ERROR;
	}
	else
        return ERROR;

}

