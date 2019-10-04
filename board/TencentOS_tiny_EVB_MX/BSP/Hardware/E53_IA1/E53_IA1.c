#include "E53_IA1.h"
#include "stm32l4xx.h"
#include "i2c.h"
#include "string.h"

const int16_t POLYNOMIAL = 0x131;
E53_IA1_Data_TypeDef E53_IA1_Data;


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
	float result_lx;
	uint8_t BUF[2];
	int result;
	Start_BH1750();
	HAL_Delay(180);
	HAL_I2C_Master_Receive(&hi2c1, BH1750_Addr+1,BUF,2,0xff); 
	result=BUF[0];
	result=(result<<8)+BUF[1];  //Synthetic Digital Illumination Intensity Data
	result_lx=(float)(result/1.2);
  return result_lx;
}

/***************************************************************
* 函数名称: SHT30_reset
* 说    明: SHT30复位
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void SHT30_reset(void)
{
		uint8_t SHT3X_Resetcommand_Buffer[2]={0x30,0xA2}; //soft reset  
    HAL_I2C_Master_Transmit(&hi2c1,SHT30_Addr<<1,SHT3X_Resetcommand_Buffer,2,0x10);
    HAL_Delay(15);
	
}

/***************************************************************
* 函数名称: Init_SHT30
* 说    明: 初始化SHT30，设置测量周期
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_SHT30(void)
{
		uint8_t SHT3X_Modecommand_Buffer[2]={0x22,0x36}; //periodic mode commands 
		HAL_I2C_Master_Transmit(&hi2c1,SHT30_Addr<<1,SHT3X_Modecommand_Buffer,2,0x10); //send periodic mode commands
	
}

/***************************************************************
* 函数名称: SHT3x_CheckCrc
* 说    明: 检查数据正确性
* 参    数: data：读取到的数据
						nbrOfBytes：需要校验的数量
						checksum：读取到的校对比验值
* 返 回 值: 校验结果，0-成功		1-失败
***************************************************************/
uint8_t SHT3x_CheckCrc(char data[], char nbrOfBytes, char checksum)
{
	
    char crc = 0xFF;
    char bit = 0;
    unsigned char byteCtr;
	
    //calculates 8-Bit checksum with given polynomial
    for(byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
    {
        crc ^= (data[byteCtr]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else crc = (crc << 1);
        }
    }
	
    if(crc != checksum)
		return 1;
    else
		return 0;
	
}

/***************************************************************
* 函数名称: SHT3x_CalcTemperatureC
* 说    明: 温度计算
* 参    数: u16sT：读取到的温度原始数据
* 返 回 值: 计算后的温度数据
***************************************************************/
float SHT3x_CalcTemperatureC(unsigned short u16sT)
{
	
    float temperatureC = 0;            // variable for result

    u16sT &= ~0x0003;           // clear bits [1..0] (status bits)
    //-- calculate temperature [℃] --
    temperatureC = (175 * (float)u16sT / 65535 - 45); //T = -45 + 175 * rawValue / (2^16-1)
	
    return temperatureC;
	
}

/***************************************************************
* 函数名称: SHT3x_CalcRH
* 说    明: 湿度计算
* 参    数: u16sRH：读取到的湿度原始数据
* 返 回 值: 计算后的湿度数据
***************************************************************/
float SHT3x_CalcRH(unsigned short u16sRH)
{
	
    float humidityRH = 0;              // variable for result
	
    u16sRH &= ~0x0003;          // clear bits [1..0] (status bits)
    //-- calculate relative humidity [%RH] --
    humidityRH = (100 * (float)u16sRH / 65535);  // RH = rawValue / (2^16-1) * 10
	
    return humidityRH;
	
}

/***************************************************************
* 函数名称: Init_Motor
* 说    明: 初始化Init_E53_IA1的马达
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_Motor(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  IA1_Motor_GPIO_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IA1_Motor_GPIO_Port, IA1_Motor_Pin, GPIO_PIN_RESET);
  E53_IA1_Data.MotorMode=0;
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = IA1_Motor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IA1_Motor_GPIO_Port, &GPIO_InitStruct);
}

void motor_control(char mode){
  if(mode==1)
  {
     E53_IA1_Data.MotorMode=1;
     HAL_GPIO_WritePin(IA1_Motor_GPIO_Port, IA1_Motor_Pin, GPIO_PIN_SET);
  }
  else if(mode==0)
  {
     E53_IA1_Data.MotorMode=0;
     HAL_GPIO_WritePin(IA1_Motor_GPIO_Port, IA1_Motor_Pin, GPIO_PIN_RESET);
  }
  
}


void light_control(char mode){
  if(mode==1)
  {
     E53_IA1_Data.LightMode=1;
     HAL_GPIO_WritePin(IA1_Light_GPIO_Port, IA1_Light_Pin, GPIO_PIN_SET);
  }
  else if(mode==0)
  {
     E53_IA1_Data.LightMode=0;
     HAL_GPIO_WritePin(IA1_Light_GPIO_Port, IA1_Light_Pin, GPIO_PIN_RESET);
  }
  
}

/***************************************************************
* 函数名称: Init_Light
* 说    明: 初始化Init_E53_IA1的补光灯
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_Light(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  IA1_Light_GPIO_CLK_ENABLE();

	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IA1_Light_GPIO_Port, IA1_Light_Pin, GPIO_PIN_RESET);
  E53_IA1_Data.LightMode=0;
	 /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = IA1_Light_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IA1_Light_GPIO_Port, &GPIO_InitStruct);
}
/***************************************************************
* 函数名称: Init_E53_IA1
* 说    明: 初始化Init_E53_IA1
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_E53_IA1(void)
{
    //printf("E53_sensor_board init!\n");
   	MX_I2C1_Init();
    Init_BH1750();
    Init_SHT30();
    Init_Motor();
    Init_Light();
    memset((char *)&E53_IA1_Data,0,sizeof(E53_IA1_Data));
}

/***************************************************************
* 函数名称: E53_IA1_Read_Data
* 说    明: 测量光照强度、温度、湿度
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void E53_IA1_Read_Data(void)
{
	
    char  data[3];    //data array for checksum verification
    unsigned short tmp = 0;
    uint16_t dat;
    uint8_t SHT3X_Fetchcommand_Bbuffer[2]={0xE0,0x00};								//read the measurement results
    uint8_t SHT3X_Data_Buffer[6]; 																		//byte 0,1 is temperature byte 4,5 is humidity

    E53_IA1_Data.Lux=Convert_BH1750();																								//Read bh1750 sensor data 

    HAL_I2C_Master_Transmit(&hi2c1,SHT30_Addr<<1,SHT3X_Fetchcommand_Bbuffer,2,0x10); //Read sht30 sensor data 
    HAL_I2C_Master_Receive(&hi2c1,(SHT30_Addr<<1)+1,SHT3X_Data_Buffer,6,0x10); 

    //    /* check tem */
    data[0] = SHT3X_Data_Buffer[0];
    data[1] = SHT3X_Data_Buffer[1];
    data[2] = SHT3X_Data_Buffer[2];

    tmp=SHT3x_CheckCrc(data, 2, data[2]);
    if( !tmp ) /* value is ture */
    {
    dat = ((uint16_t)data[0] << 8) | data[1];
    E53_IA1_Data.Temperature = SHT3x_CalcTemperatureC( dat );    
    }

    //    /* check humidity */
    data[0] = SHT3X_Data_Buffer[3];
    data[1] = SHT3X_Data_Buffer[4];
    data[2] = SHT3X_Data_Buffer[5];

    tmp=SHT3x_CheckCrc(data, 2, data[2]);
    if( !tmp ) /* value is ture */
    {
    dat = ((uint16_t)data[0] << 8) | data[1];
    E53_IA1_Data.Humidity = SHT3x_CalcRH( dat );    
    }
    
}



