#include "sys.h"
#include "ov2640.h"
#include "ov2640cfg.h"
//#include "timer.h"	  
#include "delay.h"
#include "usart.h"			 
#include "sccb.h"	
#include "stdio.h"
  
//≥ı ºªØOV2640 
//≈‰÷√ÕÍ“‘∫Û,ƒ¨»œ ‰≥ˆ «1600*1200≥ﬂ¥ÁµƒÕº∆¨!! 
//∑µªÿ÷µ:0,≥…π¶
//    ∆‰À˚,¥ÌŒÛ¥˙¬Î
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
	//…Ë÷√IO     	   
	GPIO_InitTypeDef  GPIO_InitStructure;
	__HAL_RCC_GPIOG_CLK_ENABLE();           // πƒ‹GPIOB ±÷”
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13|GPIO_PIN_15, GPIO_PIN_RESET);
  //GPIOF9,F10≥ı ºªØ…Ë÷√
  GPIO_InitStructure.Pin = GPIO_PIN_13|GPIO_PIN_15;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pull = GPIO_PULLUP;//…œ¿≠
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);//≥ı ºª
	
 	OV2640_PWDN(0);	//POWER ON
	delay_ms(10);
	OV2640_RST(0);	//∏¥ŒªOV2640
	delay_ms(10);
	OV2640_RST(1);	//Ω· ¯∏¥Œª 
  SCCB_Init();        		//≥ı ºªØSCCB µƒIOø⁄	 
	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);	//≤Ÿ◊˜sensorºƒ¥Ê∆˜
 	SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);	//»Ì∏¥ŒªOV2640
	delay_ms(50); 
	reg=SCCB_RD_Reg(OV2640_SENSOR_MIDH);	//∂¡»°≥ßº“ID ∏ﬂ∞ÀŒª
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_MIDL);	//∂¡»°≥ßº“ID µÕ∞ÀŒª
	printf("OV2640_MID = %#X\n" , reg);
	if(reg!=OV2640_MID)
	{
		printf("MID:%d\r\n",reg);
		return 1;
	}
	reg=SCCB_RD_Reg(OV2640_SENSOR_PIDH);	//∂¡»°≥ßº“ID ∏ﬂ∞ÀŒª
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_PIDL);	//∂¡»°≥ßº“ID µÕ∞ÀŒª
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
//OV2640«–ªªŒ™JPEGƒ£ Ω
void OV2640_JPEG_Mode(void) 
{
	uint16_t i=0;
	//…Ë÷√:YUV422∏Ò Ω
	for(i=0;i<(sizeof(ov2640_yuv422_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0],ov2640_yuv422_reg_tbl[i][1]); 
	} 
	
	//…Ë÷√: ‰≥ˆJPEG ˝æ›
	for(i=0;i<(sizeof(ov2640_jpeg_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);  
	}  
}
//OV2640«–ªªŒ™RGB565ƒ£ Ω
void OV2640_RGB565_Mode(void) 
{
	uint16_t i=0;
	//…Ë÷√:RGB565 ‰≥ˆ
	for(i=0;i<(sizeof(ov2640_rgb565_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_rgb565_reg_tbl[i][0],ov2640_rgb565_reg_tbl[i][1]); 
	}
	printf("OV2640_RGB565 SET!\n");
} 
//◊‘∂Ø∆ÿπ‚…Ë÷√≤Œ ˝±Ì,÷ß≥÷5∏ˆµ»º∂
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
//OV2640◊‘∂Ø∆ÿπ‚µ»º∂…Ë÷√
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
//∞◊∆Ω∫‚…Ë÷√
//0:◊‘∂Ø
//1:Ã´—Ùsunny
//2,“ıÃÏcloudy
//3,∞Ïπ´ “office
//4,º“¿Ôhome
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
//…´∂»…Ë÷√
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
//¡¡∂»…Ë÷√
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
//∂‘±»∂»…Ë÷√
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Contrast(uint8_t contrast)
{
	uint8_t reg7d0val=0X20;//ƒ¨»œŒ™∆’Õ®ƒ£ Ω
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
//Ãÿ–ß…Ë÷√
//0:∆’Õ®ƒ£ Ω    
//1,∏∫∆¨
//2,∫⁄∞◊   
//3,∆´∫Ï…´
//4,∆´¬Ã…´
//5,∆´¿∂…´
//6,∏¥π≈	    
void OV2640_Special_Effects(uint8_t eft)
{
	uint8_t reg7d0val=0X00;//ƒ¨»œŒ™∆’Õ®ƒ£ Ω
	uint8_t reg7d1val=0X80;
	uint8_t reg7d2val=0X80; 
	switch(eft)
	{
		case 1://∏∫∆¨
			reg7d0val=0X40; 
			break;	
		case 2://∫⁄∞◊
			reg7d0val=0X18; 
			break;	 
		case 3://∆´∫Ï…´
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XC0; 
			break;	
		case 4://∆´¬Ã…´
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0X40; 
			break;	
		case 5://∆´¿∂…´
			reg7d0val=0X18; 
			reg7d1val=0XA0;
			reg7d2val=0X40; 
			break;	
		case 6://∏¥π≈
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
//≤ Ãı≤‚ ‘
//sw:0,πÿ±’≤ Ãı
//   1,ø™∆Ù≤ Ãı(◊¢“‚OV2640µƒ≤ Ãı «µ˛º”‘⁄ÕºœÒ…œ√Êµƒ)
void OV2640_Color_Bar(uint8_t sw)
{
	uint8_t reg;
	SCCB_WR_Reg(0XFF,0X01);
	reg=SCCB_RD_Reg(0X12);
	reg&=~(1<<1);
	if(sw)reg|=1<<1; 
	SCCB_WR_Reg(0X12,reg);
}
//…Ë÷√ÕºœÒ ‰≥ˆ¥∞ø⁄ 
//sx,sy,∆ ºµÿ÷∑
//width,height:øÌ∂»(∂‘”¶:horizontal)∫Õ∏ﬂ∂»(∂‘”¶:vertical)
void OV2640_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t endx;
	uint16_t endy;
	uint8_t temp; 
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;
	
	SCCB_WR_Reg(0XFF,0X01);			
	temp=SCCB_RD_Reg(0X03);				//∂¡»°Vref÷Æ«∞µƒ÷µ
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	SCCB_WR_Reg(0X03,temp);				//…Ë÷√Vrefµƒstart∫Õendµƒ◊ÓµÕ2Œª
	SCCB_WR_Reg(0X19,sy>>2);			//…Ë÷√Vrefµƒstart∏ﬂ8Œª
	SCCB_WR_Reg(0X1A,endy>>2);			//…Ë÷√Vrefµƒendµƒ∏ﬂ8Œª
	
	temp=SCCB_RD_Reg(0X32);				//∂¡»°Href÷Æ«∞µƒ÷µ
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	SCCB_WR_Reg(0X32,temp);				//…Ë÷√Hrefµƒstart∫Õendµƒ◊ÓµÕ3Œª
	SCCB_WR_Reg(0X17,sx>>3);			//…Ë÷√Hrefµƒstart∏ﬂ8Œª
	SCCB_WR_Reg(0X18,endx>>3);			//…Ë÷√Hrefµƒendµƒ∏ﬂ8Œª
}
//…Ë÷√ÕºœÒ ‰≥ˆ¥Û–°
//OV2640 ‰≥ˆÕºœÒµƒ¥Û–°(∑÷±Ê¬ ),ÕÍ»´”…∏ƒ∫Ø ˝»∑∂®
//width,height:øÌ∂»(∂‘”¶:horizontal)∫Õ∏ﬂ∂»(∂‘”¶:vertical),width∫Õheight±ÿ–Î «4µƒ±∂ ˝
//∑µªÿ÷µ:0,…Ë÷√≥…π¶
//    ∆‰À˚,…Ë÷√ ß∞‹
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
	SCCB_WR_Reg(0X5A,outw&0XFF);		//…Ë÷√OUTWµƒµÕ∞ÀŒª
	SCCB_WR_Reg(0X5B,outh&0XFF);		//…Ë÷√OUTHµƒµÕ∞ÀŒª
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	SCCB_WR_Reg(0X5C,temp);				//…Ë÷√OUTH/OUTWµƒ∏ﬂŒª 
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
}
//…Ë÷√ÕºœÒø™¥∞¥Û–°
//”…:OV2640_ImageSize_Set»∑∂®¥´∏–∆˜ ‰≥ˆ∑÷±Ê¬ ¥”¥Û–°.
//∏√∫Ø ˝‘Ú‘⁄’‚∏ˆ∑∂Œß…œ√ÊΩ¯––ø™¥∞,”√”⁄OV2640_OutSize_Setµƒ ‰≥ˆ
//◊¢“‚:±æ∫Ø ˝µƒøÌ∂»∫Õ∏ﬂ∂»,±ÿ–Î¥Û”⁄µ»”⁄OV2640_OutSize_Set∫Ø ˝µƒøÌ∂»∫Õ∏ﬂ∂»
//     OV2640_OutSize_Set…Ë÷√µƒøÌ∂»∫Õ∏ﬂ∂»,∏˘æ›±æ∫Ø ˝…Ë÷√µƒøÌ∂»∫Õ∏ﬂ∂»,”…DSP
//     ◊‘∂Øº∆À„Àı∑≈±»¿˝, ‰≥ˆ∏¯Õ‚≤ø…Ë±∏.
//width,height:øÌ∂»(∂‘”¶:horizontal)∫Õ∏ﬂ∂»(∂‘”¶:vertical),width∫Õheight±ÿ–Î «4µƒ±∂ ˝
//∑µªÿ÷µ:0,…Ë÷√≥…π¶
//    ∆‰À˚,…Ë÷√ ß∞‹
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
	SCCB_WR_Reg(0X51,hsize&0XFF);		//…Ë÷√H_SIZEµƒµÕ∞ÀŒª
	SCCB_WR_Reg(0X52,vsize&0XFF);		//…Ë÷√V_SIZEµƒµÕ∞ÀŒª
	SCCB_WR_Reg(0X53,offx&0XFF);		//…Ë÷√offxµƒµÕ∞ÀŒª
	SCCB_WR_Reg(0X54,offy&0XFF);		//…Ë÷√offyµƒµÕ∞ÀŒª
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07; 
	SCCB_WR_Reg(0X55,temp);				//…Ë÷√H_SIZE/V_SIZE/OFFX,OFFYµƒ∏ﬂŒª
	SCCB_WR_Reg(0X57,(hsize>>2)&0X80);	//…Ë÷√H_SIZE/V_SIZE/OFFX,OFFYµƒ∏ﬂŒª
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
} 
//∏√∫Ø ˝…Ë÷√ÕºœÒ≥ﬂ¥Á¥Û–°,“≤æÕ «À˘—°∏Ò Ωµƒ ‰≥ˆ∑÷±Ê¬ 
//UXGA:1600*1200,SVGA:800*600,CIF:352*288
//width,height:ÕºœÒøÌ∂»∫ÕÕºœÒ∏ﬂ∂»
//∑µªÿ÷µ:0,…Ë÷√≥…π¶
//    ∆‰À˚,…Ë÷√ ß∞‹
uint8_t OV2640_ImageSize_Set(uint16_t width,uint16_t height)
{ 
	uint8_t temp; 
	SCCB_WR_Reg(0XFF,0X00);			
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0XC0,(width)>>3&0XFF);		//…Ë÷√HSIZEµƒ10:3Œª
	SCCB_WR_Reg(0XC1,(height)>>3&0XFF);		//…Ë÷√VSIZEµƒ10:3Œª
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80; 
	SCCB_WR_Reg(0X8C,temp);	
	SCCB_WR_Reg(0XE0,0X00);				 
	return 0;
}

















