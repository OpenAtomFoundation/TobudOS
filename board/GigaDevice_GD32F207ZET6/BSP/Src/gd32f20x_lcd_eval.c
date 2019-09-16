/*!
    \file    gd32f20x_lcd_eval.c
    \brief   LCD driver functions

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f20x_lcd_eval.h"
#include "gd32f20x_sdram_eval.h"
#include "gd32f20x_tli.h"
#include <string.h>

#define LCD_FRAME_BUFFER         ((uint32_t)0xC0000000)
#define BUFFER_OFFSET            ((uint32_t)0x7F800)
#define LCD_DEFAULT_FONT         Font16x24

static sFONT *current_font;
static uint16_t current_textcolor = 0x0000;
static uint16_t current_backcolor = 0xFFFF;
static uint32_t current_framebuffer = LCD_FRAME_BUFFER;
static uint32_t current_layer = LCD_LAYER_BACKGROUND;

static void pixel_set(int16_t x, int16_t y);

#define HORIZONTAL_SYNCHRONOUS_PULSE  41
#define HORIZONTAL_BACK_PORCH         2
#define ACTIVE_WIDTH                  480
#define HORIZONTAL_FRONT_PORCH        2

#define VERTICAL_SYNCHRONOUS_PULSE    10
#define VERTICAL_BACK_PORCH           2
#define ACTIVE_HEIGHT                 272
#define VERTICAL_FRONT_PORCH          2

/*!
    \brief      initialize the LCD GPIO and TLI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_init(void)
{
    tli_parameter_struct tli_init_struct;
    /* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOI);
    rcu_periph_clock_enable(RCU_AF);
    
    /* TLI GPIO pin remap configuration */
    gpio_pin_remap1_config(GPIO_PCF3, GPIO_PCF3_TLI_B3_PG11_REMAP | GPIO_PCF3_TLI_B2_PG10_REMAP | 
                                    GPIO_PCF3_TLI_CLK_PG7_REMAP | GPIO_PCF3_TLI_R7_PG6_REMAP |
                                    GPIO_PCF3_TLI_DE_PF10_REMAP | GPIO_PCF3_TLI_B0_PE4_REMAP | 
                                    GPIO_PCF3_TLI_G0_PE5_REMAP | GPIO_PCF3_TLI_G1_PE6_REMAP |
                                    GPIO_PCF3_TLI_R2_PC10_REMAP, ENABLE);
    
    gpio_pin_remap1_config(GPIO_PCF4, GPIO_PCF4_TLI_HSYNC_PI10_REMAP | GPIO_PCF4_TLI_VSYNC_PI9_REMAP |
                                    GPIO_PCF4_TLI_B7_PI7_REMAP | GPIO_PCF4_TLI_B6_PI6_REMAP |
                                    GPIO_PCF4_TLI_B5_PI5_REMAP | GPIO_PCF4_TLI_B4_PI4_REMAP |
                                    GPIO_PCF4_TLI_G7_PI2_REMAP | GPIO_PCF4_TLI_R1_PI3_REMAP |
                                    GPIO_PCF4_TLI_G6_PI1_REMAP | GPIO_PCF4_TLI_G5_PI0_REMAP |
                                    GPIO_PCF4_TLI_G4_PH15_REMAP | GPIO_PCF4_TLI_G3_PH14_REMAP |
                                    GPIO_PCF4_TLI_G2_PH13_REMAP | GPIO_PCF4_TLI_R6_PH12_REMAP |
                                    GPIO_PCF4_TLI_R5_PH11_REMAP | GPIO_PCF4_TLI_R4_PH10_REMAP |
                                    GPIO_PCF4_TLI_R3_PH9_REMAP | GPIO_PCF4_TLI_R0_PH4_REMAP |
                                    GPIO_PCF4_TLI_B1_PG12_REMAP, ENABLE);

    /* configure HSYNC(PI10), VSYNC(PI9), PCLK(PG7) */
    /* configure LCD_R7(PG6), LCD_R6(PH12), LCD_R5(PH11), LCD_R4(PH10), LCD_R3(PH9),LCD_R2(PC10), 
                 LCD_R1(PI3), LCD_R0(PH4), LCD_G7(PI2), LCD_G6(PI1), LCD_G5(PI0), LCD_G4(PH15), 
                 LCD_G3(PH14), LCD_G2(PH13),LCD_G1(PE6), LCD_G0(PE5),LCD_B7(PI7), LCD_B6(PI6), 
                 LCD_B5(PI5), LCD_B4(PI4), LCD_B3(PG11),LCD_B2(PG10), LCD_B1(PG12), LCD_B0(PE4),
                 LCD_CLK(PG7), LCD_HSYNC(PI10), LCD_VSYNC(PI9), LCD_DE(PF10) */    
    gpio_init(GPIOE,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6); 
    
    gpio_init(GPIOH,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_4 | GPIO_PIN_8 |GPIO_PIN_9 | 
                                                    GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12|
                                                    GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    
    gpio_init(GPIOI,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | 
                                                    GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | 
                                                    GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10);  
    
    gpio_init(GPIOG,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 |
                                                    GPIO_PIN_11 | GPIO_PIN_12);     

    gpio_init(GPIOF,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);  
    
    gpio_init(GPIOC,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
    /* back light */
    gpio_init(GPIOH, GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_bit_set(GPIOH,GPIO_PIN_6);

    rcu_periph_clock_enable(RCU_TLI);

    /* configure the TLI clock to generate lcd clock */
    rcu_pllt_config(RCU_PLLTSRC_HXTAL);
#ifdef __CC_ARM
    if(ERROR == rcu_pllt_vco_config(25, 192, 3)){
        while(1);
    }
#endif /* __CC_ARM */
#ifdef __IAR_SYSTEMS_ICC__
    if(ERROR == rcu_pllt_vco_config(25, 192, 6)){
        while(1);
    }
#endif /* __IAR_SYSTEMS_ICC__ */
    rcu_tli_clock_config(RCU_CKTLI_CKPLLTR_DIV8);

    rcu_osci_on(RCU_PLLT_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLLT_CK)){
        while(1);
    }
    
    /* config the EXMC access mode */
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
    
    /* TLI initialization */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
    
    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1;
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;
    /* LCD background color configure*/
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF; 
    tli_init(&tli_init_struct);
}

/*!
    \brief      initialize TLI layer0 or layer1
    \param[in]  layer: LCD layer
      \arg        LCD_LAYER_BACKGROUND
      \arg        LCD_LAYER_FOREGROUND
    \param[in]  width: width of the window
    \param[in]  height: height of the window
    \param[out] none
    \retval     none
*/
void lcd_layer_init(uint32_t layer, uint32_t width, uint32_t height)
{
    tli_layer_parameter_struct  tli_layer_init_struct;
    if(LCD_LAYER_BACKGROUND == layer){
        /* TLI layer0 configuration */
        tli_layer_init_struct.layer_window_leftpos = (HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);
        tli_layer_init_struct.layer_window_rightpos = (width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
        tli_layer_init_struct.layer_window_toppos = (VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);
        tli_layer_init_struct.layer_window_bottompos = (height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
        tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
        tli_layer_init_struct.layer_sa = 0xFF;
        tli_layer_init_struct.layer_default_blue = 0xFF;        
        tli_layer_init_struct.layer_default_green = 0xFF;       
        tli_layer_init_struct.layer_default_red = 0xFF;         
        tli_layer_init_struct.layer_default_alpha = 0xFF;
        tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;    
        tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
        tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER;
        tli_layer_init_struct.layer_frame_line_length = ((width * 2) + 3); 
        tli_layer_init_struct.layer_frame_buf_stride_offset = (width * 2);
        tli_layer_init_struct.layer_frame_total_line_number = height; 
        tli_layer_init(LAYER0, &tli_layer_init_struct);
    }else if(LCD_LAYER_FOREGROUND == layer){
        /* TLI layer1 configuration */
        tli_layer_init_struct.layer_window_leftpos = (HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);
        tli_layer_init_struct.layer_window_rightpos = (width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
        tli_layer_init_struct.layer_window_toppos = (VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);
        tli_layer_init_struct.layer_window_bottompos = (height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
        tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
        tli_layer_init_struct.layer_sa = 0xFF;
        tli_layer_init_struct.layer_default_blue = 0xFF;        
        tli_layer_init_struct.layer_default_green = 0xFF;       
        tli_layer_init_struct.layer_default_red = 0xFF;         
        tli_layer_init_struct.layer_default_alpha = 0x0;
        tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;    
        tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
        tli_layer_init_struct.layer_frame_bufaddr = LCD_FRAME_BUFFER + BUFFER_OFFSET;
        tli_layer_init_struct.layer_frame_line_length = ((width * 2) + 3); 
        tli_layer_init_struct.layer_frame_buf_stride_offset = (width * 2);
        tli_layer_init_struct.layer_frame_total_line_number = height; 
        tli_layer_init(LAYER1, &tli_layer_init_struct);
    }

    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    lcd_font_set(&LCD_DEFAULT_FONT);
}

/*!
    \brief      enable the LCD layer0 or layer1
    \param[in]  layer: LCD layer
      \arg        LCD_LAYER_BACKGROUND
      \arg        LCD_LAYER_FOREGROUND
    \param[out] none
    \retval     none
*/
void lcd_layer_enable(uint32_t layer)
{
    if(LCD_LAYER_BACKGROUND == layer){
        tli_layer_enable(LAYER0);
    }else if(LCD_LAYER_FOREGROUND == layer){
        tli_layer_enable(LAYER1);
    }
    tli_enable();
}

/*!
    \brief      set the LCD layer
    \param[in]  layer: LCD layer
      \arg        LCD_LAYER_BACKGROUND
      \arg        LCD_LAYER_FOREGROUND
    \param[out] none
    \retval     none
*/
void lcd_layer_set(uint32_t layer)
{
    if (LCD_LAYER_BACKGROUND == layer){
        current_framebuffer = LCD_FRAME_BUFFER;
        current_layer = LCD_LAYER_BACKGROUND;
    }else{
        current_framebuffer = LCD_FRAME_BUFFER + BUFFER_OFFSET;
        current_layer = LCD_LAYER_FOREGROUND;
    }
}

/*!
    \brief      set the transparency of LCD
    \param[in]  trans: transparency of LCD, from 0 to 255
    \param[out] none
    \retval     none
*/
void lcd_transparency_set(uint8_t trans)
{
    if (LCD_LAYER_BACKGROUND == current_layer){
        TLI_LxSA(LAYER0) &= ~(TLI_LxSA_SA);
        TLI_LxSA(LAYER0) = trans;
    }else{
        TLI_LxSA(LAYER1) &= ~(TLI_LxSA_SA);
        TLI_LxSA(LAYER1) = trans;
    }
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*!
    \brief      configure the packeted pixel format
    \param[in]  pixel_format: pixel format
      \arg        LAYER_PPF_ARGB8888
      \arg        LAYER_PPF_RGB888
      \arg        LAYER_PPF_RGB565
      \arg        LAYER_PPF_ARGB1555
      \arg        LAYER_PPF_ARGB4444
      \arg        LAYER_PPF_L8
      \arg        LAYER_PPF_AL44
      \arg        LAYER_PPF_AL88
    \param[out] none
    \retval     none
*/
void lcd_pixel_format_config(uint32_t pixel_format)
{
    if(LCD_LAYER_BACKGROUND == current_layer){
        TLI_LxPPF(LAYER0) &= ~(TLI_LxPPF_PPF);
        TLI_LxPPF(LAYER0) = pixel_format;
    }else{
        TLI_LxPPF(LAYER1) &= ~(TLI_LxPPF_PPF);
        TLI_LxPPF(LAYER1) = pixel_format;
    }
}

/*!
    \brief      configure the frame buffer base address
    \param[in]  address: frame buffer base address
    \param[out] none
    \retval     none
*/
void lcd_address_config(uint32_t address)
{
    if (LCD_LAYER_BACKGROUND == current_layer){
        TLI_LxFBADDR(LAYER0) &= ~(TLI_LxFBADDR_FBADD);
        TLI_LxFBADDR(LAYER0) = address;
    }else{
        TLI_LxFBADDR(LAYER1) &= ~(TLI_LxFBADDR_FBADD);
        TLI_LxFBADDR(LAYER1) = address;
    }

    tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*!
    \brief      clear the LCD with specified color
    \param[in]  color: LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
    \param[out] none
    \retval     none
*/
void lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    for (index = 0x00; index < BUFFER_OFFSET; index++){
        *(__IO uint16_t*)(current_framebuffer + (2*index)) = color;
    }
}

/*!
    \brief      set the text font
    \param[in]  font: the text font
    \param[out] none
    \retval     none
*/
void lcd_font_set(sFONT *font)
{
    current_font = font;
}

/*!
    \brief      get the text font
    \param[in]  none
    \param[out] none
    \retval     the text font
*/
sFONT* lcd_font_get(void)
{
    return current_font;
}

/*!
    \brief      set the text color
    \param[in]  color: LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
    \param[out] none
    \retval     none
*/
void lcd_text_color_set(uint16_t color)
{
    current_textcolor = color;
}

/*!
    \brief      get the current text color
    \param[in]  none
    \param[out] none
    \retval     LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
*/
uint16_t lcd_text_color_get(void)
{
    return current_textcolor;
}

/*!
    \brief      set the background color
    \param[in]  color: LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
    \param[out] none
    \retval     none
*/
void lcd_background_color_set(uint16_t color)
{
    current_backcolor = color;
}

/*!
    \brief      get the current background color
    \param[in]  none
    \param[out] none
    \retval     LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
*/
uint16_t lcd_background_color_get(void)
{
    return current_backcolor;
}

/*!
    \brief      set point with the specified position and color
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  color: LCD color
      \arg        LCD_COLOR_WHITE
      \arg        LCD_COLOR_BLACK
      \arg        LCD_COLOR_GREY
      \arg        LCD_COLOR_BLUE
      \arg        LCD_COLOR_BLUE2
      \arg        LCD_COLOR_RED
      \arg        LCD_COLOR_MAGENTA
      \arg        LCD_COLOR_GREEN
      \arg        LCD_COLOR_CYAN
      \arg        LCD_COLOR_YELLOW
    \param[out] none
    \retval     none
*/
void lcd_point_set(uint16_t xpos, uint16_t ypos, uint16_t color)
{
    *(__IO uint16_t*)(current_framebuffer + 2*((LCD_PIXEL_WIDTH*ypos) + xpos)) = color;
}

/*!
    \brief      get the color of point with the specified position 
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[out] none
    \retval     value of point(x, y)
*/
uint16_t lcd_point_get(uint16_t xpos, uint16_t ypos)
{
    return *(__IO uint16_t*)(current_framebuffer + 2*((LCD_PIXEL_WIDTH*ypos) + xpos));
}

/*!
    \brief      draw a line on LCD
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  length: length of line 
    \param[in]  line_direction: direction of line
      \arg        LCD_LINEDIR_HORIZONTAL
      \arg        LCD_LINEDIR_VERTICAL
    \param[out] none
    \retval     none
*/
void lcd_line_draw(uint16_t xpos, uint16_t ypos, uint16_t length, uint8_t line_direction)
{
    if(LCD_LINEDIR_HORIZONTAL == line_direction){
        uint16_t x;
        for(x = xpos; x < xpos + length; x++){
            pixel_set(x, ypos);
        }
    }else{
        uint16_t y;
        for(y = ypos; y < ypos + length; y++){
            pixel_set(xpos, y);
        }
    }
}

/*!
    \brief      draw a rectangle on LCD
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  width: width of rectangle 
    \param[in]  height: height of rectangle
    \param[out] none
    \retval     none
*/
void lcd_rectangle_draw(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height)
{
    /* draw horizontal lines */
    lcd_line_draw(xpos, ypos, width, LCD_LINEDIR_HORIZONTAL);
    lcd_line_draw(xpos, (ypos+ height), width + 1, LCD_LINEDIR_HORIZONTAL);

    /* draw vertical lines */
    lcd_line_draw(xpos, ypos, height, LCD_LINEDIR_VERTICAL);
    lcd_line_draw((xpos + width), ypos, height, LCD_LINEDIR_VERTICAL);
}

/*!
    \brief      draw a circle on LCD using Bresenham algorithm
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  radius: radius of the circle
    \param[out] none
    \retval     none
*/
void lcd_circle_draw(uint16_t xpos, uint16_t ypos, uint16_t radius)
{
    int x, y, e;
    e = 3-2*radius;
    x = 0;
    y = radius;

    /* set four pixel (x-r, y), (x+r, y), (x, y-r), (x, y-r) */
    pixel_set(-radius+xpos, ypos);
    pixel_set(radius+xpos, ypos); 
    pixel_set(xpos, -radius+ypos);
    pixel_set(xpos, radius+ypos);
    
    while(x <= y){
        if(e < 0){
            /* choose the right of the current pixel as the next pixel */
            e = e+4*x+6;
            x++;
        }else{
            /* choose the right bottom of the current pixel as the next pixel */
            e = e+4*(x-y)+10;
            x++;
            y--;
        }
        pixel_set(x+xpos, y+ypos);
        pixel_set(-x+xpos, y+ypos);
        pixel_set(-x+xpos, -y+ypos);
        pixel_set(x+xpos, -y+ypos);
        pixel_set(y+xpos, x+ypos);
        pixel_set(-y+xpos, x+ypos);
        pixel_set(-y+xpos, -x+ypos);
        pixel_set(y+xpos, -x+ypos);
    }
}

/*!
    \brief      set plot point of ellipse
    \param[in]  center_x: x position of ellipse center
    \param[in]  center_y: y position of ellipse center
    \param[in]  x: x value
    \param[in]  y: y value
    \param[out] none
    \retval     none
*/
static void plotpoint_set(int center_x,int center_y,int x,int y)
{
    pixel_set(center_x+x, center_y+y);
    pixel_set(center_x-x, center_y+y);
    pixel_set(center_x+x, center_y-y);
    pixel_set(center_x-x, center_y-y); 
}

/*!
    \brief      draw a ellipse on LCD using the midpoint ellipse algorithm
    \param[in]  xpos: x position of ellipse center
    \param[in]  ypos: y position of ellipse center
    \param[in]  axis1: major axis
    \param[in]  axis2: minor axis
    \param[out] none
    \retval     none
*/
void lcd_ellipse_draw(uint16_t xpos, uint16_t ypos, uint16_t axis1, uint16_t axis2)
{  
    int sq_axis1 = axis1*axis1, sq_axis2 = axis2*axis2;
    int p;
    int x = 0, y = axis2;
    int px = 0, py = 2*sq_axis1*y;
    /* draw four points on the long and short axis of the ellipse */
    plotpoint_set(xpos, ypos, x, y);
    /* calculate the initial value in area 1 */
    p = (int)((sq_axis2-(sq_axis1*axis2)+(0.25*sq_axis1)));
    while(px < py){
        ++x;
        px += 2*sq_axis2;
        if(p<0){
            p += sq_axis2+px;
        }else{
            --y;
            py -= 2*sq_axis1;
            p += sq_axis2+px-py;
        }  
        plotpoint_set(xpos, ypos, x, y);
    }
    /* calculate the initial value with the last point calculated in the area 1 */
    p = (int)((sq_axis2*(x+0.5)*(x+0.5)+sq_axis1*(y-1)*(y-1)-sq_axis1*sq_axis2));
    while(y > 0){
        --y;
        py -= 2*sq_axis1;
        if(p > 0){
            p += sq_axis1-py;
        }else{
            ++x;
            px += 2*sq_axis2;
            p += sq_axis1-py+px;
        }  
        plotpoint_set(xpos, ypos, x, y);
    }
}

/*!
    \brief      draw a triangle on LCD
    \param[in]  point: 
    \param[in]  point_count: 
    \param[out] none
    \retval     none
*/
void lcd_triangle_draw(ptr_point point, uint16_t point_count)
{
}

/*!
    \brief      fill the whole rectangle
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  width: width of the rectangle
    \param[in]  height: height of the rectangle
    \param[out] none
    \retval     none
*/
void lcd_rectangle_fill(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height)
{
    uint16_t x, y;

    for(x = xpos; x < xpos + width; x++){
        for(y = ypos; y < ypos + height; y++){
            pixel_set(x, y);
        }
    }
}

/*!
    \brief      display the string on LCD
    \param[in]  stringline: line to display the character
      \arg        LCD_LINE_x(x=0..39 for 480*272)
    \param[in]  ptr: a pointer to the string
    \param[out] none
    \retval     none
*/
void lcd_string_display(uint16_t stringline, uint8_t *ptr)
{
    uint16_t column = 0;

    while ((column < LCD_PIXEL_WIDTH) && (*ptr != 0)){
        /* display character on LCD */
        lcd_char_display(stringline, column, *ptr);
        column += current_font->Width;
        ptr++;
    }
}

/*!
    \brief      display the vertical string on LCD
    \param[in]  stringline: line to display the character
      \arg        LCD_LINE_x(x=0..39 for 480*272) 
    \param[in]  offset: offset value
    \param[in]  ptr: a pointer to the string
    \param[out] none
    \retval     none
*/
void lcd_vertical_string_display(uint16_t stringline, uint16_t offset, uint8_t *ptr)
{
    uint16_t column = LCD_PIXEL_HEIGHT - (current_font->Width + offset + 2);

    while ((column > 0) && (*ptr != 0)){
        /* display character on LCD */
        lcd_vertical_char_display(stringline, column, *ptr);
        column -= current_font->Width;
        ptr++;
    }
}

/*!
    \brief      display the character on LCD
    \param[in]  line: line to display the character
      \arg        LCD_LINE_x(x=0..39 for 480*272) 
    \param[in]  column: column address
    \param[in]  ascii: character ascii code(from 0x20 to 0x7E)
    \param[out] none
    \retval     none
*/
void lcd_char_display(uint16_t line, uint16_t column, uint8_t ascii)
{
    ascii -= 32;
    lcd_char_draw(line, column, &current_font->table[ascii * current_font->Height]);
}

/*!
    \brief      display the vertical character on LCD
    \param[in]  line: line to display the character
      \arg        LCD_LINE_x(x=0..39 for 480*272) 
    \param[in]  column: column address
    \param[in]  ascii: character ascii code(from 0x20 to 0x7E)
    \param[out] none
    \retval     none
*/
void lcd_vertical_char_display(uint16_t line, uint16_t column, uint8_t ascii)
{
    ascii -= 32;
    lcd_vertical_char_draw(line, column, &current_font->table[ascii * current_font->Height]);
}

/*!
    \brief      draw the character on LCD
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  c: a pointer to the character
    \param[out] none
    \retval     none
*/
void lcd_char_draw(uint16_t xpos, uint16_t ypos, const uint16_t *c)
{
    uint32_t index = 0, counter = 0, x = 0;
    uint32_t xaddress = 0;

    x = xpos * LCD_PIXEL_WIDTH * 2;
    xaddress += ypos;

    for(index = 0; index < current_font->Height; index++){
        for(counter = 0; counter < current_font->Width; counter++){
            if((((c[index] & ((0x80 << ((current_font->Width / 12) * 8)) >> counter)) == 0x00) && (current_font->Width <= 12))||
                (((c[index] & (0x1 << counter)) == 0x00) && (current_font->Width > 12))){
                /* write the background color */
                *(__IO uint16_t*) (current_framebuffer + (2*xaddress) + x) = current_backcolor;
            }else{
                /* write the text color */
                *(__IO uint16_t*) (current_framebuffer + (2*xaddress) + x) = current_textcolor;
            }

            xaddress++;
        }

        xaddress += (LCD_PIXEL_WIDTH - current_font->Width);
    }
}

/*!
    \brief      draw the vertical character on LCD
    \param[in]  xpos: position of x
    \param[in]  ypos: position of y
    \param[in]  c: a pointer to the character
    \param[out] none
    \retval     none
*/
void lcd_vertical_char_draw(uint16_t xpos, uint16_t ypos, const uint16_t *c)
{
    uint32_t index = 0, counter = 0;

    for(index = 0; index < current_font->Height; index++){
        for(counter = 0; counter < current_font->Width; counter++){
            if((((c[index] & ((0x80 << ((current_font->Width / 12) * 8)) >> counter)) == 0x00) && (current_font->Width <= 12))||
                (((c[index] & (0x1 << counter)) == 0x00) && (current_font->Width > 12))){
            }else{
                pixel_set(xpos + index, ypos + current_font->Width - counter);
            }
        }
    }
}

/*!
    \brief      set a pixel
    \param[in]  x: x position
    \param[in]  y: y position
    \param[out] none
    \retval     none
*/
static void pixel_set(int16_t x, int16_t y)
{
    if(x < 0 || x > (LCD_PIXEL_WIDTH - 1) || y < 0 || y > (LCD_PIXEL_HEIGHT - 1)){
        return;
    }

    /* draw pixel with current text color */
    *(__IO uint16_t*)(current_framebuffer + 2*((LCD_PIXEL_WIDTH*y) + x)) = current_textcolor;
}
