/*******************************************
 * @breif	HTS221驱动文件
 * @author	Mculover666(www.mculover666.cn)
 * @date	2019-12-27
 * @version	1.0.0
********************************************/

#include <HTS221.h>
#include <i2c.h>

/* 设置工作模式，初始化HTS221 */
void HTS221_Init()
{
	uint8_t cmd = 0;
	
	//设置分辨率
	cmd = 0x3F;
	HAL_I2C_Mem_Write(&hi2c1, HTS221_ADDR_WR, 0x10, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
	
	//设置电源、数据块更新模式、数据输出速率
	cmd = 0x84;
	HAL_I2C_Mem_Write(&hi2c1, HTS221_ADDR_WR, HTS221_CTRL_REG1, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
	
	//设置数据存储块复位模式，关闭内部加热
	cmd = 0x00;
	HAL_I2C_Mem_Write(&hi2c1, HTS221_ADDR_WR, HTS221_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
	
	//关闭数据完成输出信号
	cmd = 0x00;
	HAL_I2C_Mem_Write(&hi2c1, HTS221_ADDR_WR, HTS221_CTRL_REG3, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);

}

/* HTS221启动一次转换 */
static void HTS221_Start()
{
	uint8_t dat = 0;
	
	//读取REG2寄存器中的值，防止设置信息被破坏
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, HTS221_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &dat, 1, 0xFFFF);
	
	//启动一次转换
	dat |= 0x01;
	HAL_I2C_Mem_Write(&hi2c1, HTS221_ADDR_WR, HTS221_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &dat, 1, 0xFFFF);

}

/* 启动一次转换并获取校验后的温度值 */
/* note：该API获取的值是10倍数据   */
uint8_t	HTS221_Get_Temperature(int16_t* temperature)
{
	uint8_t status_dat = 0;
	int16_t T0_degC, T1_degC;
	int16_t T0_out, T1_out, T_out, T0_degC_x8_u16, T1_degC_x8_u16;
	uint8_t T0_degC_x8, T1_degC_x8, tmp;
	uint8_t buffer[4];
	int32_t tmp32;
	
	/*1. 读取T0_degC_x8 和 T1_degC_x8 校验值 */
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x32, I2C_MEMADD_SIZE_8BIT, &T0_degC_x8, 1, 0xFFFF);
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x33, I2C_MEMADD_SIZE_8BIT, &T1_degC_x8, 1, 0xFFFF);
	
	/*2. 读取T1_degC 和 T0_degC 的最高位*/
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x35, I2C_MEMADD_SIZE_8BIT, &tmp, 1, 0xFFFF);
	
	// 计算T0_degC and T1_degC 值 */
	T0_degC_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | ((uint16_t)T0_degC_x8);
	T1_degC_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 6) | ((uint16_t)T1_degC_x8);
	T0_degC = T0_degC_x8_u16>>3;
	T1_degC = T1_degC_x8_u16>>3;
	
	/*3. 读取 T0_OUT 和 T1_OUT 值 */
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x3C, I2C_MEMADD_SIZE_8BIT, &buffer[0], 1, 0xFFFF);
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x3D, I2C_MEMADD_SIZE_8BIT, &buffer[1], 1, 0xFFFF);
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x3E, I2C_MEMADD_SIZE_8BIT, &buffer[0], 1, 0xFFFF);
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x3F, I2C_MEMADD_SIZE_8BIT, &buffer[1], 1, 0xFFFF);
	
	T0_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];
	T1_out = (((uint16_t)buffer[3])<<8) | (uint16_t)buffer[2];
	
	/* 4. 启动转换，等待完成后读取转换后的值T_OUT */
	HTS221_Start();
	while(status_dat != 0x03)
	{
		HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, HTS221_STATUS_REG, I2C_MEMADD_SIZE_8BIT, &status_dat, 1, 0xFFFF);
	}
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, HTS221_TEMP_OUT_L, I2C_MEMADD_SIZE_8BIT, &buffer[0], 1, 0xFFFF);
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, HTS221_TEMP_OUT_H, I2C_MEMADD_SIZE_8BIT, &buffer[1], 1, 0xFFFF);
	
	T_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];
	
	/* 5. 使用线性插值法计算当前对应的温度值 */
	tmp32 = ((int32_t)(T_out - T0_out)) * ((int32_t)(T1_degC - T0_degC)*10);
	*temperature = tmp32 /(T1_out - T0_out) + T0_degC*10;
	
	return 0;
}

/* 启动一次转换并获取校验后的湿度值 */
/* note：该API获取的值是10倍数据   */
uint8_t	HTS221_Get_Humidity(int16_t* humidity)
{
	uint8_t status_dat = 0;
	int16_t H0_T0_out, H1_T0_out, H_T_out;
	int16_t H0_rh, H1_rh;
	uint8_t buffer[2];
	int32_t tmp;
	
	
	/* 1. 读取H0_rH and H1_rH 校验值 */
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x30, I2C_MEMADD_SIZE_8BIT, &buffer[0], 1, 0xFFFF);
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x31, I2C_MEMADD_SIZE_8BIT, &buffer[1], 1, 0xFFFF);
	H0_rh = buffer[0] >> 1;
	H1_rh = buffer[1] >> 1;
	
	/*2. 读取 H0_T0_OUT 校验值 */
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x36, I2C_MEMADD_SIZE_8BIT, &buffer[0], 1, 0xFFFF);
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x37, I2C_MEMADD_SIZE_8BIT, &buffer[1], 1, 0xFFFF);
	H0_T0_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];
	
	/*3. 读取 H1_T0_OUT 校验值 */
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x3A, I2C_MEMADD_SIZE_8BIT, &buffer[0], 1, 0xFFFF);
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, 0x3B, I2C_MEMADD_SIZE_8BIT, &buffer[1], 1, 0xFFFF);
	H1_T0_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];

	/*4. 启动转换，等待完成后读取转换后的值 */
	HTS221_Start();
	while(status_dat != 0x03)
	{
		HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, HTS221_STATUS_REG, I2C_MEMADD_SIZE_8BIT, &status_dat, 1, 0xFFFF);
	}
	
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, HTS221_HUMIDITY_OUT_L, I2C_MEMADD_SIZE_8BIT, &buffer[0], 1, 0xFFFF);
	HAL_I2C_Mem_Read(&hi2c1, HTS221_ADDR_RD, HTS221_HUMIDITY_OUT_H, I2C_MEMADD_SIZE_8BIT, &buffer[1], 1, 0xFFFF);
	H_T_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];

	/*5. 使用线性插值法计算湿度值 RH [%] */
	tmp = ((int32_t)(H_T_out - H0_T0_out)) * ((int32_t)(H1_rh - H0_rh)*10);
	*humidity = (tmp/(H1_T0_out - H0_T0_out) + H0_rh*10);
	//湿度阈值滤波
	if(*humidity>1000)
	{		
		*humidity = 1000;
	}
	
	return 0;
}
