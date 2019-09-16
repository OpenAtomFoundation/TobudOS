#include "bh1750.h"
#include "stm32l4xx.h"
#include "i2c.h"


float result_lx;
uint8_t BUF[2];
int result;


/***************************************************************
* 函数名称: Init_BH1750
* 说    明: 写命令初始化BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_BH1750(void)
{
    uint8_t t_Data = 0x01;
    HAL_I2C_Master_Transmit(&hi2c1,BH1750_Addr,&t_Data,1,0xff);
}

/***************************************************************
* 函数名称: Start_BH1750
* 说    明: 启动BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Start_BH1750(void)
{
    uint8_t t_Data = 0x10;
    HAL_I2C_Master_Transmit(&hi2c1,BH1750_Addr,&t_Data,1,0xff); 
}



/***************************************************************
* 函数名称: Convert_BH1750
* 说    明: 数值转换
* 参    数: 无
* 返 回 值: 光强值
***************************************************************/
float Convert_BH1750(void)
{
	Start_BH1750();
	HAL_Delay(180);
	HAL_I2C_Master_Receive(&hi2c1, BH1750_Addr+1,BUF,2,0xff); 
	result=BUF[0];
	result=(result<<8)+BUF[1];  //合成数据，即光照数据	
	result_lx=(float)(result/1.2);
  return result_lx;
}

