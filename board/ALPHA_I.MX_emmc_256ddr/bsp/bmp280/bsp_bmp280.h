#ifndef _BSP_BMP280_H
#define _BSP_BMP280_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_bmp280.h
作者	   : 左忠凯
版本	   : V1.0
描述	   : BMP280驱动头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/17 左忠凯创建
***************************************************************/
#include "bsp.h"
#include "bsp_gpio.h"

/* 宏定义 */
#define BMP280_CSN(n)    (n ? gpio_pinwrite(GPIO1, 20, 1) : gpio_pinwrite(GPIO1, 20, 0))   //SPI片选信号

#define	BMP280_ID_VALUE			0X58	/* BMP280 ID值				*/
#define BMP280_RESET_VALUE		0XB6	/* BMP280复位值 				*/


/* BMP280寄存器宏定义 */
#define	BMP280_TEMPXLSB_REG		0XFC	/* 温度值的bit[3:0]寄存器			*/
#define BMP280_TEMPLSB_REG		0XFB	/* 温度值的bit[11:4]寄存器 		*/
#define BMP280_TEMPMSB_REG		0XFA	/* 温度值的bit[19:12]寄存器		*/
#define BMP280_PRESXLSB_REG		0XF9	/* 压力值的bit[3:0]寄存器			*/
#define BMP280_PRESLSB_REG		0XF8	/* 压力值的bit[11:4]寄存器 		*/
#define BMP280_PRESMSB_REG		0XF7	/* 压力值的bit[19:12]寄存器  		*/
#define BMP280_CONFIG_REG		0XF5	/* 配置寄存器 					*/
#define BMP280_CTRLMEAS_REG		0XF4	/* 测量配置寄存器 					*/
#define BMP280_STATUS_REG		0XF3	/* 状态寄存器 					*/
#define BMP280_RESET_REG		0XE0	/* 复位寄存器 					*/
#define BMP280_ID_REG			0XD0	/* ID寄存器 					*/

#define BMP280_TRIMPARAM_FRG	0X88	/* 参数组寄存器首地址 				*/
#define BMP280_TRIMPARAM_LEN	24		/* 参数组寄存器长度 				*/

#define BMP280_INVALID_DATA		0		/* 无效的数据 					*/

/*
 * BMP280 参数结构体
 */
struct bmp280_calib_param
{
	unsigned short 	dig_T1;		/* T1参数	*/
	short 		 	dig_T2;		/* T2参数 */
	short 			dig_T3;		/* T3参数	*/
	unsigned short  dig_P1;		/* P1参数 */
	short 			dig_P2;		/* P2参数 */
	short 			dig_P3;		/* P3参数 */
	short 			dig_P4;		/* P4参数 */
	short 			dig_P5;		/* P5参数 */
	short 			dig_P6;		/* P6参数 */
	short 			dig_P7;		/* P7参数 */
	short 			dig_P8;		/* P8参数 */
	short 			dig_P9;		/* P9参数 */
	signed int 		t_fine;		/* t_fine参数*/
};

/*
 * BMP280结构体
 */
struct bmp280_dev_struc
{
	struct bmp280_calib_param calib_param;

	signed int uncomp_temp;		/* 原始的温度值 */
	signed int uncomp_pres;		/* 原始的压力值 */
	signed int actual_temp;		/* 真实的温度值 */
	signed int actual_pres;		/* 真实的压力值 */

};

extern struct bmp280_dev_struc bmp280_dev;

/* 函数声明 */
unsigned char bmp280_init(void);
unsigned char bmp280_read_reg(unsigned char reg);
void bmp280_read_len(unsigned char reg, unsigned char *buf, unsigned char len);
void bmp280_write_reg(unsigned char reg, unsigned char value);
void bmp280_get_calibparam(void);
signed int bmp280_get_uncomptemp(void);
signed int bmp280_get_uncomppres(void);
signed int bmp280_get_actualtemp(signed int uncomptemp);
unsigned int bmp280_get_actualpres(s32 uncomppres);

#endif