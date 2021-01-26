#include "sys.h"
#include "ov2640.h"
#include "ov2640cfg.h"
//#include "timer.h"	  
#include "delay.h"
#include "usart.h"			 
#include "sccb.h"	
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板
//OV2640 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//初始化OV2640 
//配置完以后,默认输出是1600*1200尺寸的图片!! 
//返回值:0,成功
//    其他,错误代码
void OV2640_PWDN(uint8_t signal)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, (GPIO_PinState)signal);
}

void OV2640_RST(uint8_t signal)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, (GPIO_PinState)signal);
}

uint8_t OV2640_Init(void)
{ 
	uint16_t i=0;
	uint16_t reg;
	//设置IO     	   
	GPIO_InitTypeDef  GPIO_InitStructure;
	__HAL_RCC_GPIOG_CLK_ENABLE();           //使能GPIOB时钟
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13|GPIO_PIN_15, GPIO_PIN_RESET);
  //GPIOF9,F10初始化设置
  GPIO_InitStructure.Pin = GPIO_PIN_13|GPIO_PIN_15;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pull = GPIO_PULLUP;//上拉
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);//初始�
	
 	OV2640_PWDN(0);	//POWER ON
	delay_ms(10);
	OV2640_RST(0);	//复位OV2640
	delay_ms(10);
	OV2640_RST(1);	//结束复位 
  SCCB_Init();        		//初始化SCCB 的IO口	 
	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);	//操作sensor寄存器
 	SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);	//软复位OV2640
	delay_ms(50); 
	reg=SCCB_RD_Reg(OV2640_SENSOR_MIDH);	//读取厂家ID 高八位
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_MIDL);	//读取厂家ID 低八位
	printf("OV2640_MID = %#X\n" , reg);
	if(reg!=OV2640_MID)
	{
		printf("MID:%d\r\n",reg);
		return 1;
	}
	reg=SCCB_RD_Reg(OV2640_SENSOR_PIDH);	//读取厂家ID 高八位
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_PIDL);	//读取厂家ID 低八位
	if(reg!=OV2640_PID)
	{
		printf("HID:%d\r\n",reg);
		return 2;
	}   
	for(i=0;i<sizeof(ov2640_svga_init_reg_tbl)/2;i++)
	{
	   	SCCB_WR_Reg(ov2640_svga_init_reg_tbl[i][0],ov2640_svga_init_reg_tbl[i][1]);
 	} 
	printf("OV2640_init SUCCESS\n");
  return 0x00; 	//ok
} 
//OV2640切换为JPEG模式
void OV2640_JPEG_Mode(void) 
{
	uint16_t i=0;
	//设置:YUV422格式
	for(i=0;i<(sizeof(ov2640_yuv422_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0],ov2640_yuv422_reg_tbl[i][1]); 
	} 
	
	//设置:输出JPEG数据
	for(i=0;i<(sizeof(ov2640_jpeg_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);  
	}  
}
//OV2640切换为RGB565模式
void OV2640_RGB565_Mode(void) 
{
	uint16_t i=0;
	//设置:RGB565输出
	for(i=0;i<(sizeof(ov2640_rgb565_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_rgb565_reg_tbl[i][0],ov2640_rgb565_reg_tbl[i][1]); 
	}
	printf("OV2640_RGB565 SET!\n");
} 
//自动曝光设置参数表,支持5个等级
const static uint8_t OV2640_AUTOEXPOSURE_LEVEL[5][8]=
{
	{
		0xFF,0x01,
		0x24,0x20,
		0x25,0x18,
		0x26,0x60,
	},
	{
		0xFF,0x01,
		0x24,0x34,
		0x25,0x1c,
		0x26,0x00,
	},
	{
		0xFF,0x01,	
		0x24,0x3e,	
		0x25,0x38,
		0x26,0x81,
	},
	{
		0xFF,0x01,
		0x24,0x48,
		0x25,0x40,
		0x26,0x81,
	},
	{
		0xFF,0x01,	
		0x24,0x58,	
		0x25,0x50,	
		0x26,0x92,	
	},
}; 
//OV2640自动曝光等级设置
//level:0~4
void OV2640_Auto_Exposure(uint8_t level)
{  
	uint8_t i;
	uint8_t *p=(uint8_t*)OV2640_AUTOEXPOSURE_LEVEL[level];
	for(i=0;i<4;i++)
	{ 
		SCCB_WR_Reg(p[i*2],p[i*2+1]); 
	} 
}  
//白平衡设置
//0:自动
//1:太阳sunny
//2,阴天cloudy
//3,办公室office
//4,家里home
void OV2640_Light_Mode(uint8_t mode)
{
	uint8_t regccval=0X5E;//Sunny 
	uint8_t regcdval=0X41;
	uint8_t regceval=0X54;
	switch(mode)
	{ 
		case 0://auto 
			SCCB_WR_Reg(0XFF,0X00);	 
			SCCB_WR_Reg(0XC7,0X10);//AWB ON 
			return;  	
		case 2://cloudy
			regccval=0X65;
			regcdval=0X41;
			regceval=0X4F;
			break;	
		case 3://office
			regccval=0X52;
			regcdval=0X41;
			regceval=0X66;
			break;	
		case 4://home
			regccval=0X42;
			regcdval=0X3F;
			regceval=0X71;
			break;	
	}
	SCCB_WR_Reg(0XFF,0X00);	 
	SCCB_WR_Reg(0XC7,0X40);	//AWB OFF 
	SCCB_WR_Reg(0XCC,regccval); 
	SCCB_WR_Reg(0XCD,regcdval); 
	SCCB_WR_Reg(0XCE,regceval);  
}
//色度设置
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Color_Saturation(uint8_t sat)
{ 
	uint8_t reg7dval=((sat+2)<<4)|0X08;
	SCCB_WR_Reg(0XFF,0X00);		
	SCCB_WR_Reg(0X7C,0X00);		
	SCCB_WR_Reg(0X7D,0X02);				
	SCCB_WR_Reg(0X7C,0X03);			
	SCCB_WR_Reg(0X7D,reg7dval);			
	SCCB_WR_Reg(0X7D,reg7dval); 		
}
//亮度设置
//0:(0X00)-2
//1:(0X10)-1
//2,(0X20) 0
//3,(0X30)+1
//4,(0X40)+2
void OV2640_Brightness(uint8_t bright)
{
  SCCB_WR_Reg(0xff, 0x00);
  SCCB_WR_Reg(0x7c, 0x00);
  SCCB_WR_Reg(0x7d, 0x04);
  SCCB_WR_Reg(0x7c, 0x09);
  SCCB_WR_Reg(0x7d, bright<<4); 
  SCCB_WR_Reg(0x7d, 0x00); 
}
//对比度设置
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Contrast(uint8_t contrast)
{
	uint8_t reg7d0val=0X20;//默认为普通模式
	uint8_t reg7d1val=0X20;
  	switch(contrast)
	{
		case 0://-2
			reg7d0val=0X18;	 	 
			reg7d1val=0X34;	 	 
			break;	
		case 1://-1
			reg7d0val=0X1C;	 	 
			reg7d1val=0X2A;	 	 
			break;	
		case 3://1
			reg7d0val=0X24;	 	 
			reg7d1val=0X16;	 	 
			break;	
		case 4://2
			reg7d0val=0X28;	 	 
			reg7d1val=0X0C;	 	 
			break;	
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,0x04);
	SCCB_WR_Reg(0x7c,0x07);
	SCCB_WR_Reg(0x7d,0x20);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,0x06);
}
//特效设置
//0:普通模式    
//1,负片
//2,黑白   
//3,偏红色
//4,偏绿色
//5,偏蓝色
//6,复古	    
void OV2640_Special_Effects(uint8_t eft)
{
	uint8_t reg7d0val=0X00;//默认为普通模式
	uint8_t reg7d1val=0X80;
	uint8_t reg7d2val=0X80; 
	switch(eft)
	{
		case 1://负片
			reg7d0val=0X40; 
			break;	
		case 2://黑白
			reg7d0val=0X18; 
			break;	 
		case 3://偏红色
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XC0; 
			break;	
		case 4://偏绿色
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0X40; 
			break;	
		case 5://偏蓝色
			reg7d0val=0X18; 
			reg7d1val=0XA0;
			reg7d2val=0X40; 
			break;	
		case 6://复古
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XA6; 
			break;	 
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7c,0x05);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,reg7d2val); 
}
//彩条测试
//sw:0,关闭彩条
//   1,开启彩条(注意OV2640的彩条是叠加在图像上面的)
void OV2640_Color_Bar(uint8_t sw)
{
	uint8_t reg;
	SCCB_WR_Reg(0XFF,0X01);
	reg=SCCB_RD_Reg(0X12);
	reg&=~(1<<1);
	if(sw)reg|=1<<1; 
	SCCB_WR_Reg(0X12,reg);
}
//设置图像输出窗口 
//sx,sy,起始地址
//width,height:宽度(对应:horizontal)和高度(对应:vertical)
void OV2640_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t endx;
	uint16_t endy;
	uint8_t temp; 
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;
	
	SCCB_WR_Reg(0XFF,0X01);			
	temp=SCCB_RD_Reg(0X03);				//读取Vref之前的值
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	SCCB_WR_Reg(0X03,temp);				//设置Vref的start和end的最低2位
	SCCB_WR_Reg(0X19,sy>>2);			//设置Vref的start高8位
	SCCB_WR_Reg(0X1A,endy>>2);			//设置Vref的end的高8位
	
	temp=SCCB_RD_Reg(0X32);				//读取Href之前的值
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	SCCB_WR_Reg(0X32,temp);				//设置Href的start和end的最低3位
	SCCB_WR_Reg(0X17,sx>>3);			//设置Href的start高8位
	SCCB_WR_Reg(0X18,endx>>3);			//设置Href的end的高8位
}
//设置图像输出大小
//OV2640输出图像的大小(分辨率),完全由改函数确定
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
uint8_t OV2640_OutSize_Set(uint16_t width,uint16_t height)
{
	uint16_t outh;
	uint16_t outw;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	SCCB_WR_Reg(0XFF,0X00);	
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0X5A,outw&0XFF);		//设置OUTW的低八位
	SCCB_WR_Reg(0X5B,outh&0XFF);		//设置OUTH的低八位
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	SCCB_WR_Reg(0X5C,temp);				//设置OUTH/OUTW的高位 
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
}
//设置图像开窗大小
//由:OV2640_ImageSize_Set确定传感器输出分辨率从大小.
//该函数则在这个范围上面进行开窗,用于OV2640_OutSize_Set的输出
//注意:本函数的宽度和高度,必须大于等于OV2640_OutSize_Set函数的宽度和高度
//     OV2640_OutSize_Set设置的宽度和高度,根据本函数设置的宽度和高度,由DSP
//     自动计算缩放比例,输出给外部设备.
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
uint8_t OV2640_ImageWin_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{
	uint16_t hsize;
	uint16_t vsize;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	SCCB_WR_Reg(0XFF,0X00);	
	SCCB_WR_Reg(0XE0,0X04);					
	SCCB_WR_Reg(0X51,hsize&0XFF);		//设置H_SIZE的低八位
	SCCB_WR_Reg(0X52,vsize&0XFF);		//设置V_SIZE的低八位
	SCCB_WR_Reg(0X53,offx&0XFF);		//设置offx的低八位
	SCCB_WR_Reg(0X54,offy&0XFF);		//设置offy的低八位
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07; 
	SCCB_WR_Reg(0X55,temp);				//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	SCCB_WR_Reg(0X57,(hsize>>2)&0X80);	//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
} 
//该函数设置图像尺寸大小,也就是所选格式的输出分辨率
//UXGA:1600*1200,SVGA:800*600,CIF:352*288
//width,height:图像宽度和图像高度
//返回值:0,设置成功
//    其他,设置失败
uint8_t OV2640_ImageSize_Set(uint16_t width,uint16_t height)
{ 
	uint8_t temp; 
	SCCB_WR_Reg(0XFF,0X00);			
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0XC0,(width)>>3&0XFF);		//设置HSIZE的10:3位
	SCCB_WR_Reg(0XC1,(height)>>3&0XFF);		//设置VSIZE的10:3位
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80; 
	SCCB_WR_Reg(0X8C,temp);	
	SCCB_WR_Reg(0XE0,0X00);				 
	return 0;
}

















