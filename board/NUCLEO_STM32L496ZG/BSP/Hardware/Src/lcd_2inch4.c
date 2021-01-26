/*****************************************************************************
* | File      	:   LCD_2IN4_Driver.c
* | Author      :   Waveshare team
* | Function    :   LCD driver
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-07-29
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "lcd_2inch4.h"
#include <string.h>
/*******************************************************************************
function:
	Hardware reset
*******************************************************************************/
static void LCD_2IN4_Reset(void)
{
	LCD_2IN4_RST_1;
	DEV_Delay_ms(100);
	LCD_2IN4_RST_0;
	DEV_Delay_ms(100);
	LCD_2IN4_RST_1;
	DEV_Delay_ms(100);
}

/*******************************************************************************
function:
		Write data and commands
*******************************************************************************/
static void LCD_2IN4_Write_Command(UBYTE data)	 
{	
	LCD_2IN4_CS_0;
	LCD_2IN4_DC_0;
	DEV_SPI_WRITE(data);
}

static void LCD_2IN4_WriteData_Byte(UBYTE data) 
{	
	LCD_2IN4_CS_0;
	LCD_2IN4_DC_1;
	DEV_SPI_WRITE(data);  
	LCD_2IN4_CS_1;
}  

void LCD_2IN4_WriteData_Word(UWORD data)
{
	LCD_2IN4_CS_0;
	LCD_2IN4_DC_1;
	DEV_SPI_WRITE((data>>8) & 0xff);
	DEV_SPI_WRITE(data);
	LCD_2IN4_CS_1;
}	 

/******************************************************************************
function:	
		Common register initialization
******************************************************************************/
void LCD_2IN4_Init(void)
{
	LCD_2IN4_Reset();

	LCD_2IN4_SetBackLight(500);
  	HAL_Delay(100);
	
	//************* Start Initial Sequence **********//
	LCD_2IN4_Write_Command(0x11); //Sleep out 
	HAL_Delay(120);              //Delay 120ms 
	//************* Start Initial Sequence **********// 
	LCD_2IN4_Write_Command(0xCF);
	LCD_2IN4_WriteData_Byte(0x00);
	LCD_2IN4_WriteData_Byte(0xC1);
	LCD_2IN4_WriteData_Byte(0X30);
	LCD_2IN4_Write_Command(0xED);
	LCD_2IN4_WriteData_Byte(0x64);
	LCD_2IN4_WriteData_Byte(0x03);
	LCD_2IN4_WriteData_Byte(0X12);
	LCD_2IN4_WriteData_Byte(0X81);
	LCD_2IN4_Write_Command(0xE8);
	LCD_2IN4_WriteData_Byte(0x85);
	LCD_2IN4_WriteData_Byte(0x00);
	LCD_2IN4_WriteData_Byte(0x79);
	LCD_2IN4_Write_Command(0xCB);
	LCD_2IN4_WriteData_Byte(0x39);
	LCD_2IN4_WriteData_Byte(0x2C);
	LCD_2IN4_WriteData_Byte(0x00);
	LCD_2IN4_WriteData_Byte(0x34);
	LCD_2IN4_WriteData_Byte(0x02);
	LCD_2IN4_Write_Command(0xF7);
	LCD_2IN4_WriteData_Byte(0x20);
	LCD_2IN4_Write_Command(0xEA);
	LCD_2IN4_WriteData_Byte(0x00);
	LCD_2IN4_WriteData_Byte(0x00);
	LCD_2IN4_Write_Command(0xC0); //Power control
	LCD_2IN4_WriteData_Byte(0x1D); //VRH[5:0]
	LCD_2IN4_Write_Command(0xC1); //Power control
	LCD_2IN4_WriteData_Byte(0x12); //SAP[2:0];BT[3:0]
	LCD_2IN4_Write_Command(0xC5); //VCM control
	LCD_2IN4_WriteData_Byte(0x33);
	LCD_2IN4_WriteData_Byte(0x3F);
	LCD_2IN4_Write_Command(0xC7); //VCM control
	LCD_2IN4_WriteData_Byte(0x92);
	LCD_2IN4_Write_Command(0x3A); // Memory Access Control
	LCD_2IN4_WriteData_Byte(0x55);
	LCD_2IN4_Write_Command(0x36); // Memory Access Control
  LCD_2IN4_WriteData_Byte(0x08);
	LCD_2IN4_Write_Command(0xB1);
	LCD_2IN4_WriteData_Byte(0x00);
	LCD_2IN4_WriteData_Byte(0x12);
	LCD_2IN4_Write_Command(0xB6); // Display Function Control
	LCD_2IN4_WriteData_Byte(0x0A);
	LCD_2IN4_WriteData_Byte(0xA2);

	LCD_2IN4_Write_Command(0x44);
	LCD_2IN4_WriteData_Byte(0x02);

	LCD_2IN4_Write_Command(0xF2); // 3Gamma Function Disable
	LCD_2IN4_WriteData_Byte(0x00);
	LCD_2IN4_Write_Command(0x26); //Gamma curve selected
	LCD_2IN4_WriteData_Byte(0x01);
	LCD_2IN4_Write_Command(0xE0); //Set Gamma
	LCD_2IN4_WriteData_Byte(0x0F);
	LCD_2IN4_WriteData_Byte(0x22);
	LCD_2IN4_WriteData_Byte(0x1C);
	LCD_2IN4_WriteData_Byte(0x1B);
	LCD_2IN4_WriteData_Byte(0x08);
	LCD_2IN4_WriteData_Byte(0x0F);
	LCD_2IN4_WriteData_Byte(0x48);
	LCD_2IN4_WriteData_Byte(0xB8);
	LCD_2IN4_WriteData_Byte(0x34);
	LCD_2IN4_WriteData_Byte(0x05);
	LCD_2IN4_WriteData_Byte(0x0C);
	LCD_2IN4_WriteData_Byte(0x09);
	LCD_2IN4_WriteData_Byte(0x0F);
	LCD_2IN4_WriteData_Byte(0x07);
	LCD_2IN4_WriteData_Byte(0x00);
	LCD_2IN4_Write_Command(0XE1); //Set Gamma
	LCD_2IN4_WriteData_Byte(0x00);
	LCD_2IN4_WriteData_Byte(0x23);
	LCD_2IN4_WriteData_Byte(0x24);
	LCD_2IN4_WriteData_Byte(0x07);
	LCD_2IN4_WriteData_Byte(0x10);
	LCD_2IN4_WriteData_Byte(0x07);
	LCD_2IN4_WriteData_Byte(0x38);
	LCD_2IN4_WriteData_Byte(0x47);
	LCD_2IN4_WriteData_Byte(0x4B);
	LCD_2IN4_WriteData_Byte(0x0A);
	LCD_2IN4_WriteData_Byte(0x13);
	LCD_2IN4_WriteData_Byte(0x06);
	LCD_2IN4_WriteData_Byte(0x30);
	LCD_2IN4_WriteData_Byte(0x38);
	LCD_2IN4_WriteData_Byte(0x0F);
	LCD_2IN4_Write_Command(0x29); //Display on
}


/******************************************************************************
function:	Set the cursor position
parameter	:
	  Xstart: 	Start UWORD x coordinate
	  Ystart:	Start UWORD y coordinate
	  Xend  :	End UWORD coordinates
	  Yend  :	End UWORD coordinatesen
******************************************************************************/
void LCD_2IN4_SetWindow(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD  Yend)
{ 
	LCD_2IN4_Write_Command(0x2a);
	LCD_2IN4_WriteData_Byte(Xstart >>8);
	LCD_2IN4_WriteData_Byte(Xstart & 0xff);
	LCD_2IN4_WriteData_Byte((Xend - 1) >> 8);
	LCD_2IN4_WriteData_Byte((Xend - 1) & 0xff);

	LCD_2IN4_Write_Command(0x2b);
	LCD_2IN4_WriteData_Byte(Ystart >>8);
	LCD_2IN4_WriteData_Byte(Ystart & 0xff);
	LCD_2IN4_WriteData_Byte((Yend - 1) >> 8);
	LCD_2IN4_WriteData_Byte((Yend - 1) & 0xff);

	LCD_2IN4_Write_Command(0x2C);
}

/******************************************************************************
function:	Settings window
parameter	:
	  Xstart: 	Start UWORD x coordinate
	  Ystart:	Start UWORD y coordinate

******************************************************************************/
void LCD_2IN4_SetCursor(UWORD X, UWORD Y)
{ 
	LCD_2IN4_Write_Command(0x2a);
	LCD_2IN4_WriteData_Byte(X >> 8);
	LCD_2IN4_WriteData_Byte(X);
	LCD_2IN4_WriteData_Byte(X >> 8);
	LCD_2IN4_WriteData_Byte(X);

	LCD_2IN4_Write_Command(0x2b);
	LCD_2IN4_WriteData_Byte(Y >> 8);
	LCD_2IN4_WriteData_Byte(Y);
	LCD_2IN4_WriteData_Byte(Y >> 8);
	LCD_2IN4_WriteData_Byte(Y);

	LCD_2IN4_Write_Command(0x2C);
}

/******************************************************************************
function:	Clear screen function, refresh the screen to a certain color
parameter	:
	  Color :		The color you want to clear all the screen
******************************************************************************/
void LCD_2IN4_Clear(UWORD Color)
{
    UWORD i,j;
    LCD_2IN4_SetWindow(0, 0, LCD_2IN4_WIDTH, LCD_2IN4_HEIGHT);

	DEV_Digital_Write(DEV_DC_PIN, 1);
	for(i = 0; i < LCD_2IN4_WIDTH; i++){
		for(j = 0; j < LCD_2IN4_HEIGHT; j++){
			LCD_2IN4_WriteData_Word(Color);
		}
	 }
}

/******************************************************************************
function:	Refresh a certain area to the same color
parameter	:
	  Xstart: Start UWORD x coordinate
	  Ystart:	Start UWORD y coordinate
	  Xend  :	End UWORD coordinates
	  Yend  :	End UWORD coordinates
	  color :	Set the color
******************************************************************************/
void LCD_2IN4_ClearWindow(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,UWORD color)
{          
	UWORD i,j; 
	LCD_2IN4_SetWindow(Xstart, Ystart, Xend,Yend);
	for(i = Ystart; i <= Yend; i++){
		for(j = Xstart; j <= Xend; j++){
			LCD_2IN4_WriteData_Word(color);
		}
	}
}

/******************************************************************************
function: Show a picture
parameter	:
		image: Picture buffer
******************************************************************************/
void LCD_2IN4_Display(UWORD *image,int width, int height)
{
  UWORD i,j;
	
	if(width > LCD_2IN4_WIDTH || height > LCD_2IN4_HEIGHT){
		printf("Picture size out of range!\r\n");
		return;
	}
	
  LCD_2IN4_SetWindow(0, 0, width, height);
	DEV_Digital_Write(DEV_DC_PIN, 1);
	for(i = 0; i < width; i++){
		for(j = 0; j < height; j++){
			LCD_2IN4_WriteData_Word(*(image+i*height+j));
		}
	 }
}

/******************************************************************************
function: Draw a point
parameter	:
	    X	: 	Set the X coordinate
	    Y	:	Set the Y coordinate
	  Color :	Set the color
******************************************************************************/
void LCD_2IN4_DrawPaint(UWORD x, UWORD y, UWORD Color)
{
	LCD_2IN4_SetCursor(x, y);
	LCD_2IN4_WriteData_Word(Color); 	    
}
/*******************************************************************************
function:
	Setting backlight
parameter	:
	  value : Range 0~1000   Duty cycle is value/1000	
*******************************************************************************/
void LCD_2IN4_SetBackLight(UWORD Value)
{
	DEV_Set_PWM(Value);
}
